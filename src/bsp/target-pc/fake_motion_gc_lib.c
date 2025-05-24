/***************************************************************************************************
 * INCLUDES
 **************************************************************************************************/
#include <stdlib.h>
#include "motion_gc.h"
/***************************************************************************************************
 * LOCAL DEFINES
 **************************************************************************************************/

/***************************************************************************************************
 * LOCAL TYPEDEFS
 **************************************************************************************************/

/***************************************************************************************************
 * LOCAL FUNCTION PROTOTYPES
 **************************************************************************************************/

/***************************************************************************************************
 * LOCAL VARIABLES
 **************************************************************************************************/
static MGC_knobs_t fake_knobs = {
    .AccThr = 0,
    .GyroThr = 0.4f,
    .FilterConst = 0.002f,
    .FastStart = 1,
    .MaxGyro = 15.0f,
    .MaxAcc = 1.4f,
};

static MGC_output_t fake_gyro_bias = {
    .GyroBiasX = 0.0f,
    .GyroBiasY = 0.0f,
    .GyroBiasZ = 0.0f,
};

static float angular_speed = 0.0f;
/***************************************************************************************************
 * GLOBAL VARIABLES
 **************************************************************************************************/

/***************************************************************************************************
 * LOCAL FUNCTIONS
 **************************************************************************************************/

/***************************************************************************************************
 * GLOBAL FUNCTIONS
 **************************************************************************************************/
void MotionGC_Initialize(MGC_mcu_type_t mcu_type, float *freq){

}

void MotionGC_GetKnobs(MGC_knobs_t *knobs) {
    knobs->AccThr = fake_knobs.AccThr;
    knobs->GyroThr = fake_knobs.GyroThr;
    knobs->FilterConst = fake_knobs.FilterConst;
    knobs->FastStart = fake_knobs.FastStart;
    knobs->MaxGyro = fake_knobs.MaxGyro;
    knobs->MaxAcc = fake_knobs.MaxAcc;
}

void MotionGC_SetKnobs(MGC_knobs_t *knobs) {
    fake_knobs.AccThr = knobs->AccThr;
    fake_knobs.GyroThr = knobs->GyroThr;
    fake_knobs.FilterConst = knobs->FilterConst;
    fake_knobs.FastStart = knobs->FastStart;
    fake_knobs.MaxGyro = knobs->MaxGyro;
    fake_knobs.MaxAcc = knobs->MaxAcc;
}

void MotionGC_Update(MGC_input_t *data_in, MGC_output_t *gyro_bias, int *bias_update) {
    gyro_bias->GyroBiasX = 0.0f;
    gyro_bias->GyroBiasY = 0.0f;
    gyro_bias->GyroBiasZ = 0.0f;
    *bias_update = 0;
}


void MotionGC_GetCalParams(MGC_output_t *gyro_bias) {
    gyro_bias->GyroBiasX = fake_gyro_bias.GyroBiasX;
    gyro_bias->GyroBiasY = fake_gyro_bias.GyroBiasY;
    gyro_bias->GyroBiasZ = fake_gyro_bias.GyroBiasZ;
}

void MotionGC_SetCalParams(MGC_output_t *gyro_bias) {
    fake_gyro_bias.GyroBiasX = gyro_bias->GyroBiasX;
    fake_gyro_bias.GyroBiasY = gyro_bias->GyroBiasY;
    fake_gyro_bias.GyroBiasZ = gyro_bias->GyroBiasZ;
}

void MotionGC_SetFrequency(float *freq) {
    (void)(freq);
}

uint8_t MotionGC_GetLibVersion(char *version) {
    (void)(version);
    return 0;
}

void FakeMotionGC_SetAngularVelocity(float ω_z) {
    angular_speed = ω_z;
}

float FakeMotionGC_GetAngularVelocity(void) {
    return angular_speed;
}
