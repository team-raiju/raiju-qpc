/***************************************************************************************************
 * INCLUDES
 **************************************************************************************************/

#include <stdint.h>
#include <stddef.h>
#include <stdio.h>
#include "qpc.h"
#include "imu_service.h"
#include "bsp.h"
#include "bsp_gpio_mapping.h"
#include "motion_gc.h"
#include "bsp_i2c.h"
#include "lsm6dsr.h"
#include "lsm6dsr_reg.h"
#include "bsp_crc.h"
#include "utils.h"

Q_DEFINE_THIS_FILE
/***************************************************************************************************
 * LOCAL DEFINES
 **************************************************************************************************/

#define IMU_POLL_PERIOD_MS  1
#define SAMPLE_FREQ_HZ      1000
#define OUTPUT_DATA_RATE_HZ 1000

#define LSM6DSR_I2C_ADDR    0xD6
#define LSM6DSR_I2C_CHANNEL IO_I2C_1
#define WHO_I_AM_VAL        0x6B

#define INTIAL_VALUE_FLAG 0xffffffff

/***************************************************************************************************
 * LOCAL TYPEDEFS
 **************************************************************************************************/

typedef struct imu_tag {
    QActive super;
    QTimeEvt timeEvt;
} imu_ao_t;

typedef struct {
    float x;
    float y;
    float z;
} axis_3x_data_t;

/***************************************************************************************************
 * LOCAL FUNCTION PROTOTYPES
 **************************************************************************************************/
static void imu_service_ctor(void);
static QState ImuService_Initial(imu_ao_t *const me, void const *const par);
static QState ImuService_Run(imu_ao_t *const me, QEvt const *const e);
static int8_t imu_init_motion_gc(void);
/***************************************************************************************************
 * LOCAL VARIABLES
 **************************************************************************************************/
static bool imu_error = true;
static bool enable_motion_gc = false;

// Angular Velocity in rad/s
static float ω_z;
static float ω_x;
static float ω_y;

// Angular Displacement in rad
static float φ_z;
static float φ_x;
static float φ_y;

// Vertical acceleration in m/s²
static float a_z;


imu_ao_t imu_inst;
QActive *const imu_service_AO = &imu_inst.super;

static LSM6DSR_Object_t lsm6dsr_ctx;
static LSM6DSR_IO_t lsm6dsr_io_ctx;
static uint8_t who_i_am_val = 0;

static uint32_t last_time_imu = INTIAL_VALUE_FLAG;

/***************************************************************************************************
 * GLOBAL VARIABLES
 **************************************************************************************************/

/***************************************************************************************************
 * LOCAL FUNCTIONS
 **************************************************************************************************/

static int32_t imu_init_peripheral()
{
    return 0;
}

static int32_t imu_deinit_peripheral()
{
    return 0;
}

static int32_t get_tick_adapter()
{
    int32_t ticks = (int32_t)BSP_GetTick();
    return ticks;
}

static int32_t imu_read_i2c(uint16_t addr, uint16_t mem_address, uint8_t *data, uint16_t len)
{
    return bsp_i2c_read(LSM6DSR_I2C_CHANNEL, addr, mem_address, IO_I2C_MEM_SIZE_8BIT, data, len);
}

static int32_t imu_write_i2c(uint16_t addr, uint16_t mem_address, uint8_t *data, uint16_t len)
{
    return bsp_i2c_write(LSM6DSR_I2C_CHANNEL, addr, mem_address, IO_I2C_MEM_SIZE_8BIT, data, len);
}

static float limit_angle(float angle)
{
    // Return to first revolution
    while (angle > M_TWOPI) {
        angle -= M_TWOPI;
    }

    while (angle < 0.0f) {
        angle += M_TWOPI;
    }

    // Convert from 0-360 to -180-180
    if (angle > M_PI) {
        angle -= M_TWOPI;
    }

    return angle;
}

static float calculate_delta_time_s()
{
    uint32_t current_time = BSP_GetTick();

    if (last_time_imu == INTIAL_VALUE_FLAG) {
        last_time_imu = current_time;
    }

    uint32_t delta_time_ticks = (current_time - last_time_imu);

    float delta_time_s = delta_time_ticks / 1000.0f;
    last_time_imu = current_time;

    return delta_time_s;
}

static int imu_sensor_init()
{
    lsm6dsr_io_ctx.Init = imu_init_peripheral;
    lsm6dsr_io_ctx.DeInit = imu_deinit_peripheral;
    lsm6dsr_io_ctx.Address = LSM6DSR_I2C_ADDR;
    lsm6dsr_io_ctx.BusType = LSM6DSR_I2C_BUS;
    lsm6dsr_io_ctx.GetTick = get_tick_adapter;
    lsm6dsr_io_ctx.ReadReg = imu_read_i2c;
    lsm6dsr_io_ctx.WriteReg = imu_write_i2c;

    if (LSM6DSR_RegisterBusIO(&lsm6dsr_ctx, &lsm6dsr_io_ctx) != LSM6DSR_OK) {
        return -1;
    }

    if (LSM6DSR_Init(&lsm6dsr_ctx) != LSM6DSR_OK) {
        return -1;
    }

    if (LSM6DSR_ReadID(&lsm6dsr_ctx, &who_i_am_val) != LSM6DSR_OK) {
        return -1;
    }

    if (who_i_am_val != WHO_I_AM_VAL) {
        return -1;
    }

    if (LSM6DSR_ACC_Enable(&lsm6dsr_ctx) != LSM6DSR_OK) {
        return -1;
    }

    if (LSM6DSR_GYRO_Enable(&lsm6dsr_ctx) != LSM6DSR_OK) {
        return -1;
    }

    if (LSM6DSR_GYRO_SetFullScale(&lsm6dsr_ctx, 4000) != LSM6DSR_OK) {
        return -1;
    }

    if (LSM6DSR_ACC_SetFullScale(&lsm6dsr_ctx, 2) != LSM6DSR_OK) {
        return -1;
    }

    if (LSM6DSR_ACC_SetOutputDataRate(&lsm6dsr_ctx, OUTPUT_DATA_RATE_HZ) != LSM6DSR_OK) {
        return -1;
    }

    if (LSM6DSR_GYRO_SetOutputDataRate(&lsm6dsr_ctx, OUTPUT_DATA_RATE_HZ) != LSM6DSR_OK) {
        return -1;
    }

    imu_error = false;

    return 0;
}

static int8_t imu_update()
{
    // Angular Velocity in milidegrees per second
    LSM6DSR_Axes_t mω = { 0, 0, 0 };

    // Gravity acceleration in mm/s²
    LSM6DSR_Axes_t mg = { 0, 0, 0 };

    MGC_input_t data_in_gc;
    MGC_output_t data_out_gc;
    int bias_update;

    if (enable_motion_gc) {
        if (LSM6DSR_ACC_GetAxes(&lsm6dsr_ctx, &mg) != LSM6DSR_OK) {
            imu_error = true;
            return -1;
        }
    }

    if (LSM6DSR_GYRO_GetAxes(&lsm6dsr_ctx, &mω) != LSM6DSR_OK) {
        imu_error = true;
        return -1;
    }

    float δt = calculate_delta_time_s();

    ω_z = deg2rad(mω.z / 1000.0f);
    ω_x = deg2rad(mω.x / 1000.0f);
    ω_y = deg2rad(mω.y / 1000.0f);

    a_z = mg.z / 1000.0f;

    if (enable_motion_gc) {
        float new_frequency = 1.0f / δt;
        MotionGC_SetFrequency(&new_frequency);

        data_in_gc.Acc[0] = (mg.x / 1000.0f);
        data_in_gc.Acc[1] = (mg.y / 1000.0f);
        data_in_gc.Acc[2] = (mg.z / 1000.0f);

        data_in_gc.Gyro[0] = mω.x / 1000.0f;
        data_in_gc.Gyro[1] = mω.y / 1000.0f;
        data_in_gc.Gyro[2] = mω.z / 1000.0f;
        MotionGC_Update(&data_in_gc, &data_out_gc, &bias_update);

        ω_z = deg2rad(data_in_gc.Gyro[2] - data_out_gc.GyroBiasZ);
        ω_x = deg2rad(data_in_gc.Gyro[0] - data_out_gc.GyroBiasX);
        ω_y = deg2rad(data_in_gc.Gyro[1] - data_out_gc.GyroBiasY);
    } else {
        MGC_output_t g_bias = get_g_bias();

        ω_z = deg2rad((mω.z / 1000.0f) - g_bias.GyroBiasZ);
        ω_x = deg2rad((mω.x / 1000.0f) - g_bias.GyroBiasX);
        ω_y = deg2rad((mω.y / 1000.0f) - g_bias.GyroBiasY);
    }

    φ_z += ω_z * δt;
    φ_x += ω_x * δt;
    φ_y += ω_y * δt;

    // Return to first revolution
    φ_z = limit_angle(φ_z);
    φ_x = limit_angle(φ_x);
    φ_y = limit_angle(φ_y);

    // QEvt evt = { .sig = IMU_UPDATED_SIG };
    // QHSM_DISPATCH(&AO_SumoHSM->super, &evt, SIMULATOR);

    imu_error = false;
    return 0;
}

static int8_t imu_init_motion_gc()
{
    float freq = SAMPLE_FREQ_HZ;
    MotionGC_Initialize(MGC_MCU_STM32, &freq);
    MGC_knobs_t knobs;
    MotionGC_GetKnobs(&knobs);

    // Empiric
    knobs.AccThr = 0.05f;
    knobs.GyroThr = 0.4f;
    // knobs.FilterConst = 0.002f;
    // knobs.FastStart = 1;
    knobs.MaxGyro = 15.0f;
    knobs.MaxAcc = 1.4f;

    MotionGC_SetKnobs(&knobs);

    MGC_output_t initial_calib = {
        .GyroBiasX = 0.122,
        .GyroBiasY = -0.19,
        .GyroBiasZ = 0.1,
    };

    MotionGC_SetCalParams(&initial_calib);
    return 0;
}

static QState ImuService_Initial(imu_ao_t *const me, void const *const par)
{
    (void)par; /* unused parameter */

    QS_FUN_DICTIONARY(&ImuService_Run);

    return Q_TRAN(&ImuService_Run);
}

static QState ImuService_Run(imu_ao_t *const me, QEvt const *const e)
{
    QState status_;
    switch (e->sig) {
    case Q_ENTRY_SIG: {
        imu_sensor_init();
        // if (ret == 0 ) {
        imu_init_motion_gc();
        QTimeEvt_rearm(&me->timeEvt, 500 * BSP_TICKS_PER_MILISSEC);
        // }
        status_ = Q_HANDLED();
        break;
    }

    case IMU_POLL_SIG: {
        imu_update();
        QTimeEvt_rearm(&me->timeEvt, IMU_POLL_PERIOD_MS * BSP_TICKS_PER_MILISSEC);
        status_ = Q_HANDLED();
        break;
    }

    default: {
        status_ = Q_SUPER(&QHsm_top);
        break;
    }
    }
    return status_;
}

static void imu_service_ctor(void)
{
    imu_ao_t *me = &imu_inst;
    QActive_ctor(&me->super, Q_STATE_CAST(&ImuService_Initial));
    QTimeEvt_ctorX(&me->timeEvt, &me->super, IMU_POLL_SIG, 0U);
}

/***************************************************************************************************
 * GLOBAL FUNCTIONS
 **************************************************************************************************/

void imu_service_init()
{
    bsp_crc_init();

    /* statically allocate event queue buffer for the imu_ao_t AO */
    static QEvt const *imu_service_queueSto[30];

    imu_service_ctor();                                                             /* explicitly call the "constructor" */
    QACTIVE_START(imu_service_AO, 3U,                                               /* priority */
                  imu_service_queueSto, Q_DIM(imu_service_queueSto), (void *)0, 0U, /* no stack */
                  (void *)0);                                                       /* no initialization parameter */
}

float get_imu_angle_z()
{
    return φ_z;
}

void reset_imu_angle_z()
{
    φ_z = 0.0f;
    last_time_imu = INTIAL_VALUE_FLAG;
}

float get_imu_ang_vel_rad_s()
{
    #ifdef Q_SPY
    return FakeMotionGC_GetAngularVelocity();
    #else
    return ω_z;
    #endif
}

bool get_imu_error()
{
    return imu_error;
}

MGC_output_t get_g_bias()
{
    MGC_output_t gyro_bias;
    MotionGC_GetCalParams(&gyro_bias);
    return gyro_bias;
}
