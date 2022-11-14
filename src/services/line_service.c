/***************************************************************************************************
 * INCLUDES
 **************************************************************************************************/

#include "qpc.h"    
#include "bsp.h"

#include "line_service.h"
#include "bsp_adc_dma.h"


/***************************************************************************************************
 * LOCAL DEFINES
 **************************************************************************************************/

#define WHITE_THRESHOLD 1000

/***************************************************************************************************
 * LOCAL TYPEDEFS
 **************************************************************************************************/

typedef enum line_position_in_adc 
{
    LINE_POS_FR = 3, 
    LINE_POS_FL = 2,
    LINE_POS_BR = 5,      
    LINE_POS_BL = 1,
} line_position_in_adc_t;

/***************************************************************************************************
 * LOCAL FUNCTION PROTOTYPES
 **************************************************************************************************/

static void gen_line_events();
static void line_data_interrupt(uint32_t* out_data);

/***************************************************************************************************
 * LOCAL VARIABLES
 **************************************************************************************************/

static volatile bool line_sensor_is_white[NUM_OF_LINE_SENSORS];
static volatile bool line_sensor_is_white_last[NUM_OF_LINE_SENSORS];

/***************************************************************************************************
 * GLOBAL VARIABLES
 **************************************************************************************************/

/***************************************************************************************************
 * LOCAL FUNCTIONS
 **************************************************************************************************/

static void gen_line_events(){

    bool line_changed[NUM_OF_LINE_SENSORS] = {0};
    for (int i = 0; i < NUM_OF_LINE_SENSORS; i++) {
        if (line_sensor_is_white_last[i] != line_sensor_is_white[i]){
            line_changed[i] = true;
        }
    }

    if (line_changed[LINE_FR]){
        QEvt evt = {.sig = LINE_CHANGED_FR_SIG};
        QHSM_DISPATCH(&AO_SumoHSM->super, &evt, SIMULATOR);
    }

    if (line_changed[LINE_FL]){
        QEvt evt = {.sig = LINE_CHANGED_FL_SIG};
        QHSM_DISPATCH(&AO_SumoHSM->super, &evt, SIMULATOR);
    }


    if (line_changed[LINE_BR]){
        QEvt evt = {.sig = LINE_CHANGED_BR_SIG};
        QHSM_DISPATCH(&AO_SumoHSM->super, &evt, SIMULATOR);
    }


    if (line_changed[LINE_BL]){
        QEvt evt = {.sig = LINE_CHANGED_BL_SIG};
        QHSM_DISPATCH(&AO_SumoHSM->super, &evt, SIMULATOR);
    }

}

static void line_sensor_update(uint32_t* adc_raw_data) {

    line_sensor_is_white[LINE_FR] = (adc_raw_data[LINE_POS_FR] < WHITE_THRESHOLD);
    line_sensor_is_white[LINE_FL] = (adc_raw_data[LINE_POS_FL] < WHITE_THRESHOLD);
    line_sensor_is_white[LINE_BR] = (adc_raw_data[LINE_POS_BR] < WHITE_THRESHOLD);
    line_sensor_is_white[LINE_BL] = (adc_raw_data[LINE_POS_BL] < WHITE_THRESHOLD);

    gen_line_events();

    for (int i = 0; i < NUM_OF_LINE_SENSORS; i++) {
        line_sensor_is_white_last[i] = line_sensor_is_white[i];
    }


}

static void line_data_interrupt(uint32_t* out_data){

    uint32_t aux_readings[ADC_DMA_CHANNELS];

    for (uint16_t i = 0; i < ADC_DMA_HALF_BUFFER_SIZE; i += ADC_DMA_CHANNELS) {
        for (uint16_t j = 0; j < ADC_DMA_CHANNELS; j++) {
            aux_readings[j] += out_data[i + j];
        }
    }

    for (uint16_t j = 0; j < ADC_DMA_CHANNELS; j++) {
        aux_readings[j] /= (ADC_DMA_HALF_BUFFER_SIZE / ADC_DMA_CHANNELS);
    }

    line_sensor_update(aux_readings);

}

/***************************************************************************************************
 * GLOBAL FUNCTIONS
 **************************************************************************************************/

void line_service_init() {
    BSP_ADC_DMA_Init();
    BSP_ADC_DMA_Start();
    BSP_ADC_DMA_Register_Callback(line_data_interrupt);
}



bool line_is_white(line_sensor_t position){

    if (position > NUM_OF_LINE_SENSORS){
        return 0;
    }

    return line_sensor_is_white[position];

}
