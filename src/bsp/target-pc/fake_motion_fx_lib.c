/***************************************************************************************************
 * INCLUDES
 **************************************************************************************************/
#include <stdlib.h>
#include "motion_fx.h"
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
static float angle_z_degrees = 0;
static float angle_x_degrees = 0;
static float angle_y_degrees = 0;
/***************************************************************************************************
 * GLOBAL VARIABLES
 **************************************************************************************************/

/***************************************************************************************************
 * LOCAL FUNCTIONS
 **************************************************************************************************/

/***************************************************************************************************
 * GLOBAL FUNCTIONS
 **************************************************************************************************/
size_t MotionFX_GetStateSize(void)
{
    return 0;
}

void MotionFX_initialize(MFXState_t mfxstate_pt)
{
    (void)mfxstate_pt;
}

void MotionFX_setKnobs(MFXState_t mfxstate_pt, MFX_knobs_t *knobs)
{
    (void)mfxstate_pt;
    (void)knobs;
}

void MotionFX_getKnobs(MFXState_t mfxstate_pt, MFX_knobs_t *knobs)
{
    (void)mfxstate_pt;
    (void)knobs;
}

MFX_engine_state_t MotionFX_getStatus_6X(MFXState_t mfxstate_pt)
{
    (void)mfxstate_pt;
    return 0;
}

MFX_engine_state_t MotionFX_getStatus_9X(MFXState_t mfxstate_pt)
{
    (void)mfxstate_pt;
    return 0;
}

void MotionFX_enable_6X(MFXState_t mfxstate_pt, MFX_engine_state_t enable)
{
    (void)mfxstate_pt;
    (void)enable;
}

void MotionFX_enable_9X(MFXState_t mfxstate_pt, MFX_engine_state_t enable)
{
    (void)mfxstate_pt;
    (void)enable;
}

void MotionFX_setGbias(MFXState_t mfxstate_pt, float *gbias)
{
    (void)mfxstate_pt;
    (void)gbias;
}

void MotionFX_getGbias(MFXState_t mfxstate_pt, float *gbias)
{
    (void)mfxstate_pt;
    (void)gbias;
}

void MotionFX_update(MFXState_t mfxstate_pt, MFX_output_t *data_out, MFX_input_t *data_in, float *eml_deltatime,
                     float *eml_q_update)
{
    (void)mfxstate_pt;
    (void)data_in;
    (void)eml_deltatime;
    (void)eml_q_update;

    data_out->rotation[0] = angle_z_degrees; /* Yaw */
    data_out->rotation[1] = angle_x_degrees; /* Pitch */
    data_out->rotation[2] = angle_y_degrees; /* Roll */
}

void MotionFX_propagate(MFXState_t mfxstate_pt, MFX_output_t *data_out, MFX_input_t *data_in, float *eml_deltatime)
{
    (void)mfxstate_pt;
}

void MotionFX_MagCal_init(int sampletime, unsigned short int enable)
{
    (void)sampletime;
    (void)enable;
}

void MotionFX_MagCal_run(MFX_MagCal_input_t *data_in)
{
    (void)data_in;
}

void MotionFX_MagCal_getParams(MFX_MagCal_output_t *data_out)
{
    (void)data_out;
}

uint8_t MotionFX_GetLibVersion(char *version)
{
    (void)version;
    return 0;
}

void MotionFX_set_angle_z(float angle)
{
    angle_z_degrees = angle;
}

void MotionFX_set_angle_x(float angle)
{
    angle_x_degrees = angle;
}

void MotionFX_set_angle_y(float angle)
{
    angle_y_degrees = angle;
}