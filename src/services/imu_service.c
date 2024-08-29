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

#define SAMPLE_FREQ_HZ      125
#define OUTPUT_DATA_RATE_HZ 200
#define IMU_PERIOD_MS       (1000 / SAMPLE_FREQ_HZ)


#define MFX_STR_LENG 35
#define STATE_SIZE   (size_t)(2450)

#define LSM6DSR_I2C_ADDR    0xD6
#define LSM6DSR_I2C_CHANNEL IO_I2C_1
#define WHO_I_AM_VAL        0x6B

#define INTIAL_VALUE_FLAG 0xffffffff

#define RAD_90_DEGREES  M_PI_2
#define RAD_360_DEGREES (2 * M_PI)

/***************************************************************************************************
 * LOCAL TYPEDEFS
 **************************************************************************************************/

typedef struct imu_tag {
    QActive super;
    QTimeEvt timeEvt;
    bool initialized;
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

imu_ao_t imu_inst;
QActive *const imu_service_AO = &imu_inst.super;


static LSM6DSR_Object_t lsm6dsr_ctx;
static LSM6DSR_IO_t lsm6dsr_io_ctx;
static uint8_t who_i_am_val = 0;
static axis_3x_data_t angles = { 0.0, 0.0, 0.0 };
static float set_point = 0.0;
static int8_t base_speed = 0.0;
static axis_3x_data_t angular_rate_dps;


static float kp;
static float kd;
static float ki;
static float last_error;
static float integral_error;

static uint8_t near_angle_th;
static float inclinated_th = 20;

static bool last_inclination = false;
static bool current_inclination = false;

static bool imu_enabled = true;

static float x_gbias = 0.33;
static float y_gbias = -0.426;
static float z_gbias = -0.13;

static float sample_frequency = SAMPLE_FREQ_HZ;

static const bool use_motion_gc = true;

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

static int imu_sensor_init(imu_ao_t *const me)
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

    me->initialized = true;

    return 0;
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

static float limit_angle(float angle)
{
    if (angle > 360.0f) {
        angle -= 360.0f;
    } else if (angle < 0.0f) {
        angle += 360.0f;
    }

    return angle;
}

#ifndef Q_SPY

static void transform_mdps_to_dps(LSM6DSR_Axes_t *mdps, axis_3x_data_t *dps)
{
    dps->x = mdps->x / 1000.0f;
    dps->y = mdps->y / 1000.0f;
    dps->z = mdps->z / 1000.0f;
}

static int8_t imu_update()
{
    LSM6DSR_Axes_t angular_rate_mdps;
    LSM6DSR_Axes_t acceleration_mg;
    MGC_input_t data_in_gc;
    MGC_output_t data_out_gc;
    int bias_update;

    /* Get raw data */
    if (use_motion_gc) {
        if (LSM6DSR_ACC_GetAxes(&lsm6dsr_ctx, &acceleration_mg) != LSM6DSR_OK) {
            return -1;
        }
    }

    if (LSM6DSR_GYRO_GetAxes(&lsm6dsr_ctx, &angular_rate_mdps) != LSM6DSR_OK) {
        return -1;
    }

    /* Transform to SI units */
    transform_mdps_to_dps(&angular_rate_mdps, &angular_rate_dps);
    float delta_time = calculate_delta_time_s();

    /* Apply calibration */
    if (use_motion_gc) {
        float new_frequency = 1.0f / delta_time;
        MotionGC_SetFrequency(&new_frequency);

        data_in_gc.Acc[0] = (acceleration_mg.x / 1000.0f);
        data_in_gc.Acc[1] = (acceleration_mg.y / 1000.0f);
        data_in_gc.Acc[2] = (acceleration_mg.z / 1000.0f);

        data_in_gc.Gyro[0] = angular_rate_dps.x;
        data_in_gc.Gyro[1] = angular_rate_dps.y;
        data_in_gc.Gyro[2] = angular_rate_dps.z;
        MotionGC_Update(&data_in_gc, &data_out_gc, &bias_update);

        angular_rate_dps.x = (data_in_gc.Gyro[0] - data_out_gc.GyroBiasX);
        angular_rate_dps.y = (data_in_gc.Gyro[1] - data_out_gc.GyroBiasY);
        angular_rate_dps.z = (data_in_gc.Gyro[2] - data_out_gc.GyroBiasZ);
    }

    /* Update angles */
    float temp_angle_z = angles.z + angular_rate_dps.z * delta_time;
    float temp_angle_x = angles.x + angular_rate_dps.x * delta_time;
    float temp_angle_y = angles.y + angular_rate_dps.y * delta_time;

    /* Filter angle Z*/
    angles.z = limit_angle(temp_angle_z);  // yaw
    angles.x = limit_angle(temp_angle_x);  // pitch
    angles.y = limit_angle(temp_angle_y);  // roll


    /* Calc inclination */
    bool angle_x_inclination = angles.x < -inclinated_th;
    bool angle_y_inclination = angles.y > inclinated_th || angles.y < -inclinated_th;
    current_inclination = angle_x_inclination || angle_y_inclination;

    if (current_inclination && !last_inclination) {
        QEvt evt = { .sig = IMU_INCLINATION_SIG };
        QHSM_DISPATCH(&AO_SumoHSM->super, &evt, SIMULATOR);
    }

    /* Send event */
    QEvt evt = { .sig = IMU_UPDATED_SIG };
    QHSM_DISPATCH(&AO_SumoHSM->super, &evt, SIMULATOR);

    last_inclination = current_inclination;
    return 0;
}

#else
static int8_t imu_update()
{
    static float reference;
    // Changes reference everytime last_time_imu is reseted
    if (last_time_imu == INTIAL_VALUE_FLAG){
        reference = MotionGC_get_angle_z();
    }

    angles.z = MotionGC_get_angle_z() - reference;
    angles.z = limit_angle(angles.z);


    angles.x = MotionGC_get_angle_x();
    angles.y = MotionGC_get_angle_y();

    calculate_delta_time_s();


    /* Calc inclination */
    bool angle_x_inclination = angles.x < -inclinated_th;
    bool angle_y_inclination = angles.y > inclinated_th || angles.y < -inclinated_th;
    current_inclination = angle_x_inclination || angle_y_inclination;

    if (current_inclination && !last_inclination) {
        QEvt evt = { .sig = IMU_INCLINATION_SIG };
        QHSM_DISPATCH(&AO_SumoHSM->super, &evt, SIMULATOR);
    }

    /* Send event */
    QEvt evt = { .sig = IMU_UPDATED_SIG };
    QHSM_DISPATCH(&AO_SumoHSM->super, &evt, SIMULATOR);

    angular_rate_dps.x = 0;
    angular_rate_dps.y = 0;
    angular_rate_dps.z = 0;

    return 0;
}
#endif

static int8_t imu_init_motion_gc()
{
    MotionGC_Initialize(MGC_MCU_STM32, &sample_frequency);
    MGC_knobs_t knobs;
    MotionGC_GetKnobs(&knobs);

    /* Experimentaly set */
    knobs.AccThr = 0.05f;
    knobs.GyroThr = 0.4f;
    knobs.MaxGyro = 15.0f;
    knobs.MaxAcc = 1.4f;
    MotionGC_SetKnobs(&knobs);

    MGC_output_t initial_calib = {
        .GyroBiasX = x_gbias,
        .GyroBiasY = y_gbias,
        .GyroBiasZ = z_gbias,
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
        imu_sensor_init(me);
        imu_init_motion_gc();
        status_ = Q_HANDLED();
        break;
    }

    case IMU_POLL_SIG: {
        if (me->initialized && imu_enabled) {
            imu_update();
            QTimeEvt_rearm(&me->timeEvt, IMU_PERIOD_MS * BSP_TICKS_PER_MILISSEC);
        }
        status_ = Q_HANDLED();
        break;
    }

    case IMU_RESTART_SIG: {
        QTimeEvt_disarm(&me->timeEvt);
        inclinated_th = 20;
        if (use_motion_gc) {    
            reset_imu_angle_z();
            reset_inclination();
        }

        QTimeEvt_rearm(&me->timeEvt, 250 * BSP_TICKS_PER_MILISSEC);
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
    #ifdef Q_SPY
    me->initialized = true;
    #else
    me->initialized = false;
    #endif
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
    static QEvt const *imu_service_queueSto[50];

    imu_service_ctor();                                                             /* explicitly call the "constructor" */
    QACTIVE_START(imu_service_AO, 3U,                                               /* priority */
                  imu_service_queueSto, Q_DIM(imu_service_queueSto), (void *)0, 0U, /* no stack */
                  (void *)0);                                                       /* no initialization parameter */
}

void imu_reset_pid()
{
    last_error = 0;
    integral_error = 0;
}

float get_imu_angle_z()
{
    return angles.z;
}

void reset_imu_angle_z()
{
    last_time_imu = INTIAL_VALUE_FLAG;
    angles.z = 0;
}

void reset_inclination()
{
    last_inclination = false;
    current_inclination = false;
    angles.x = 0;
    angles.y = 0;

}

static float calc_error_degree(float current_angle_degrees)
{
    float error = 0;
    float aux_error = set_point - current_angle_degrees;

    if (aux_error > 180) {
        error = aux_error - 360;
    } else if (aux_error < -180) {
        error = aux_error + 360;
    } else {
        error = aux_error;
    }

    return error;
}

void imu_set_setpoint(float set_point_)
{
    set_point = set_point_;
    integral_error = 0;
}

void imu_set_base_speed(int8_t base_speed_)
{
    base_speed = base_speed_;
}

float imu_get_setpoint()
{
    return set_point;
}

bool near_set_point()
{   
    float error = calc_error_degree(angles.z);
    return fabsf(error) < near_angle_th;
}

int8_t imu_pid_process(int8_t *left_speed, int8_t *right_speed)
{
    float last_error = 0;
    float error = calc_error_degree(angles.z);

    float derivative = 0;
    integral_error += error;

    if (error > RAD_90_DEGREES && last_error < -RAD_90_DEGREES){
        derivative = (error - RAD_360_DEGREES) - last_error; 
    } else if (error < -RAD_90_DEGREES && last_error > RAD_90_DEGREES){
        derivative = (error + RAD_360_DEGREES) - last_error; 
    } else {
        derivative = error - last_error;
    }


    int32_t l_speed = base_speed - (error * kp + derivative * kd + integral_error * ki);
    int32_t r_speed = base_speed + (error * kp + derivative * kd + integral_error * ki);

    *left_speed = constrain(l_speed, -100, 100);
    *right_speed = constrain(r_speed, -100, 100);

    last_error = error;

    return 0;
}

bool get_imu_inclination_stat()
{
    return current_inclination;
}

void imu_set_kp(uint16_t kp_)
{
    kp = kp_ / 1000.0f;
}

void imu_set_kd(uint16_t kd_)
{
    kd = kd_ / 1000.0f;
}

void imu_set_ki(uint16_t ki_)
{
    ki = ki_ / 1000.0f;
}

void imu_set_near_angle_th(uint8_t near_angle_th_)
{
    near_angle_th = near_angle_th_;
}

void imu_set_inclinated_th_x10(uint8_t inclinated_th_x_10)
{
    inclinated_th = inclinated_th_x_10 / 10.0;
}


void stop_g_bias_calculation()
{
    MGC_output_t gyro_bias;
    MotionGC_GetCalParams(&gyro_bias);

    //DEBUG_PRINT("Gyro bias: %f %f %f\r\n", gyro_bias.GyroBiasX, gyro_bias.GyroBiasY, gyro_bias.GyroBiasZ);

    x_gbias = gyro_bias.GyroBiasX;
    y_gbias = gyro_bias.GyroBiasY;
    z_gbias = gyro_bias.GyroBiasZ;
}


void set_imu_z_bias(uint32_t set_z_bias, uint32_t signal_negative)
{
    if (signal_negative) {
        z_gbias = -1.0 * (float)set_z_bias / 1000.0f;
    } else {
        z_gbias = set_z_bias / 1000.0f;
    }

    MGC_output_t gyro_bias = {
        .GyroBiasX = x_gbias,
        .GyroBiasY = y_gbias,
        .GyroBiasZ = z_gbias,
    };
    MotionGC_SetCalParams(&gyro_bias);
}

uint32_t get_imu_z_bias()
{
    if (z_gbias < 0) {
        return -z_gbias * 1000;
    } else {
        return z_gbias * 1000;
    }
}

uint32_t get_imu_z_bias_signal_negative()
{
    return (z_gbias < 0);
}


void imu_set_imu_enabled(uint8_t imu_enabled_)
{
    imu_enabled = imu_enabled_;
}