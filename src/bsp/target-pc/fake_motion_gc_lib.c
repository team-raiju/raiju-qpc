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
// static float angle_z_degrees = 0;
// static float angle_x_degrees = 0;
// static float angle_y_degrees = 0;

MGC_knobs_t default_knobs = {
    .AccThr = 0.004,
    .GyroThr = 0.1,
    .FilterConst = 0.002,
    .FastStart = 1,
    .MaxGyro = 15,
    .MaxAcc = 1.3
};

MGC_output_t default_gyro_bias = {
    .GyroBiasX = 0,
    .GyroBiasY = 0,
    .GyroBiasZ = 0
};
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
    (void)mcu_type;
    (void)freq;
}

/**
  * @brief  Get the knobs setting of the library
  * @param  knobs  pointer to knobs setting structure
  * @retval none
  */
void MotionGC_GetKnobs(MGC_knobs_t *knobs){
    *knobs = default_knobs;
}

/**
  * @brief  Set the knobs setting of the library
  * @param  knobs  pointer to knobs setting structure
  * @retval none
  */
void MotionGC_SetKnobs(MGC_knobs_t *knobs){
    default_knobs = *knobs;
}

/**
  * @brief  Run gyroscope calibration algorithm and return compensation parameters
  * @param  data_in  pointer to acceleration [g] and angular rate values [dps]
  * @param  gyro_bias  pointer to actual gyroscope offset value in [dps]
  * @param  bias_update  pointer to an integer that is set to 1 if the gyroscope bias was updated, 0 otherwise
  * @retval none
  */
void MotionGC_Update(MGC_input_t *data_in, MGC_output_t *gyro_bias, int *bias_update){
    (void)data_in;
    *gyro_bias = (MGC_output_t){0, 0, 0};
    *bias_update = 0;
}

/**
  * @brief  Get the gyroscope compensation parameters
  * @param  gyro_bias  pointer to actual gyroscope offset value in [dps]
  * @retval none
  */
void MotionGC_GetCalParams(MGC_output_t *gyro_bias){
    *gyro_bias = default_gyro_bias;
}

/**
  * @brief  Set the initial gyroscope compensation parameters
  * @param  gyro_bias  pointer to actual gyroscope offset value in [dps]
  * @retval none
  */
void MotionGC_SetCalParams(MGC_output_t *gyro_bias){
    default_gyro_bias = *gyro_bias;
}

/**
  * @brief  Set new sample frequency
  * @param  freq  new sample frequency in [Hz]
  * @retval none
  */
void MotionGC_SetFrequency(float *freq){
    (void)freq;
}


/**
  * @brief  Get the library version
  * @param  version  pointer to an array of 35 char
  * @retval Number of characters in the version string
  */
uint8_t MotionGC_GetLibVersion(char *version){
    (void)version;
    return 0;
}
