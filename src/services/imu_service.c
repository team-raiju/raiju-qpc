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
#include "motion_fx.h"
#include "bsp_i2c.h"
#include "lsm6dsr.h"
#include "lsm6dsr_reg.h"
#include "bsp_crc.h"
#include "utils.h"


Q_DEFINE_THIS_FILE
/***************************************************************************************************
 * LOCAL DEFINES
 **************************************************************************************************/

#define IMU_POLL_PERIOD_MS  8
#define OUTPUT_DATA_RATE_HZ 200

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
static int8_t imu_init_motion_fx(void);

/***************************************************************************************************
 * LOCAL VARIABLES
 **************************************************************************************************/

imu_ao_t imu_inst;
QActive *const imu_service_AO = &imu_inst.super;

static char lib_version[MFX_STR_LENG];
static uint8_t mfxstate[STATE_SIZE];
static MFX_knobs_t iKnobs;

static LSM6DSR_Object_t lsm6dsr_ctx;
static LSM6DSR_IO_t lsm6dsr_io_ctx;
static uint8_t who_i_am_val = 0;
static axis_3x_data_t angles = { 0.0, 0.0, 0.0 };
static axis_3x_data_t ref_angles = { 0.0, 0.0, 0.0 };
static axis_3x_data_t angles_raw = { 0.0, 0.0, 0.0 };
static float set_point = 0.0;
static int8_t base_speed = 0.0;

static float kp;
static float kd;
static float ki;
static float last_error;
static float integral_error;

static uint8_t near_angle_th;
static uint8_t inclinated_th;

static bool last_inclination = false;
static bool current_inclination = false;

static float acc_gbias = 0.000309;
static float gyro_gbias = 0.000282;

static bool imu_enabled = true;
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

static void transform_mdps_to_dps(LSM6DSR_Axes_t *mdps, axis_3x_data_t *dps)
{
    dps->x = mdps->x / 1000.0f;
    dps->y = mdps->y / 1000.0f;
    dps->z = mdps->z / 1000.0f;
}

static void transform_mg_to_g(LSM6DSR_Axes_t *mg, axis_3x_data_t *g)
{
    g->x = mg->x / 1000.0f;
    g->y = mg->y / 1000.0f;
    g->z = mg->z / 1000.0f;
}

volatile float delta_time_s;
static float calculate_delta_time_s()
{
    static uint32_t last_time = INTIAL_VALUE_FLAG;
    uint32_t current_time = BSP_GetTick();

    if (last_time == INTIAL_VALUE_FLAG) {
        last_time = current_time;
    }

    uint32_t delta_time_ticks = (current_time - last_time);

    delta_time_s = delta_time_ticks / 1000.0f;
    last_time = current_time;

    return delta_time_s;
}

static void update_sensor_fusion(axis_3x_data_t *gyro_in_dps, axis_3x_data_t *accel_in_g, MFX_output_t *data_out_fx,
                                 float delta_time_s)
{
    MFX_input_t data_in_fx;
    data_in_fx.acc[0] = accel_in_g->x;
    data_in_fx.acc[1] = accel_in_g->y;
    data_in_fx.acc[2] = accel_in_g->z;

    data_in_fx.gyro[0] = gyro_in_dps->x;
    data_in_fx.gyro[1] = gyro_in_dps->y;
    data_in_fx.gyro[2] = gyro_in_dps->z;

    MotionFX_propagate(mfxstate, data_out_fx, &data_in_fx, &delta_time_s);
    MotionFX_update(mfxstate, data_out_fx, &data_in_fx, &delta_time_s, NULL);
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

static int8_t sensor_fusion()
{
    LSM6DSR_Axes_t acceleration_mg;
    LSM6DSR_Axes_t angular_rate_mdps;
    axis_3x_data_t acceleration_g;
    axis_3x_data_t angular_rate_dps;

    /* Get raw data */
    if (LSM6DSR_ACC_GetAxes(&lsm6dsr_ctx, &acceleration_mg) != LSM6DSR_OK) {
        return -1;
    }

    if (LSM6DSR_GYRO_GetAxes(&lsm6dsr_ctx, &angular_rate_mdps) != LSM6DSR_OK) {
        return -1;
    }

    /* Transform to SI units */
    transform_mdps_to_dps(&angular_rate_mdps, &angular_rate_dps);
    transform_mg_to_g(&acceleration_mg, &acceleration_g);
    float delta_time = calculate_delta_time_s();

    /* Run Sensor Fusion algorithm */
    MFX_output_t data_out_fx;
    update_sensor_fusion(&angular_rate_dps, &acceleration_g, &data_out_fx, delta_time);

    angles_raw.z = (360 - data_out_fx.rotation[0]);
    float temp_angle_z = angles_raw.z - ref_angles.z;

    if (temp_angle_z < 0) {
        temp_angle_z += 360;
    }

    angles_raw.x = data_out_fx.rotation[1]; // pitch
    angles_raw.y = data_out_fx.rotation[2]; // row

    angles.z = temp_angle_z;            // yaw
    angles.x = angles_raw.x - ref_angles.x; // pitch
    angles.y = angles_raw.y - ref_angles.y; // row

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

static void imu_deinit_motion_fx()
{
    memset(mfxstate, 0, sizeof(mfxstate));
}

static int8_t imu_init_motion_fx()
{
    if (STATE_SIZE < MotionFX_GetStateSize()) {
        return -1;
    }

    /* Sensor Fusion API initialization function */
    MotionFX_initialize((MFXState_t *)mfxstate);

    /* Optional: Get version */
    MotionFX_GetLibVersion(lib_version);

    /* Modify knobs settings & set the knobs */
    MotionFX_getKnobs(mfxstate, &iKnobs);

    iKnobs.modx = 2;
    iKnobs.gbias_acc_th_sc = acc_gbias;
    iKnobs.gbias_gyro_th_sc = gyro_gbias;
    iKnobs.gbias_mag_th_sc = 0;
    iKnobs.LMode = 2;
    // iKnobs.ATime = 0.9;

    MotionFX_setKnobs(mfxstate, &iKnobs);

    MotionFX_enable_9X(mfxstate, MFX_ENGINE_DISABLE);
    MotionFX_enable_6X(mfxstate, MFX_ENGINE_ENABLE);

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
        status_ = Q_HANDLED();
        break;
    }

    case IMU_POLL_SIG: {
        if (me->initialized && imu_enabled) {
            sensor_fusion();
            QTimeEvt_rearm(&me->timeEvt, IMU_POLL_PERIOD_MS * BSP_TICKS_PER_MILISSEC);
        }
        status_ = Q_HANDLED();
        break;
    }

    case IMU_RESTART_SIG: {
        QTimeEvt_disarm(&me->timeEvt);
        imu_deinit_motion_fx();
        imu_init_motion_fx();
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
    ref_angles.z = angles_raw.z;
}

void reset_inclination()
{
    last_inclination = false;
    current_inclination = false;
    ref_angles.x = angles_raw.x;
    ref_angles.y = angles_raw.y;
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

void imu_set_inclinated_th(uint8_t inclinated_th_)
{
    inclinated_th = inclinated_th_;
}


void start_g_bias_calculation()
{
    MotionFX_getKnobs(mfxstate, &iKnobs);

    iKnobs.start_automatic_gbias_calculation = 1;

    MotionFX_setKnobs(mfxstate, &iKnobs);
}

void stop_g_bias_calculation()
{
    MotionFX_getKnobs(mfxstate, &iKnobs);

    acc_gbias = iKnobs.gbias_acc_th_sc;
    gyro_gbias = iKnobs.gbias_gyro_th_sc;

    iKnobs.start_automatic_gbias_calculation = 0;
    MotionFX_setKnobs(mfxstate, &iKnobs);
}

void set_gbias(uint32_t set_acc_gbias, uint32_t set_gyro_gbias)
{
    acc_gbias = set_acc_gbias / 1000000.0f;
    gyro_gbias = set_gyro_gbias / 1000000.0f;
}

uint32_t get_acc_gbias()
{
    return acc_gbias * 1000000.0f;
}

uint32_t get_gyro_gbias()
{
    return gyro_gbias * 1000000.0f;
}

void imu_set_imu_enabled(uint8_t imu_enabled_)
{
    imu_enabled = imu_enabled_;
}