/***************************************************************************************************
 * INCLUDES
 **************************************************************************************************/

#include "qpc.h"    
#include "bsp.h"

#include "adc_service.h"
#include "bsp_adc_dma.h"


/***************************************************************************************************
 * LOCAL DEFINES
 **************************************************************************************************/
#define ADC_MAX_VALUE             4095.0
#define ADC_MAX_VOLTAGE           3.3  

#define WHITE_THRESHOLD           1000

#define BATTERY_THRESHOLD_MV      1480.0
#define BAT_VOLTAGE_DIV_R1        47.0
#define BAT_VOLTAGE_DIV_R2        10.0
#define BAT_VOLTAGE_MULTIPLIER   ((BAT_VOLTAGE_DIV_R1 + BAT_VOLTAGE_DIV_R2)/BAT_VOLTAGE_DIV_R2)


#define  BAT_POSITION_IN_ADC  0

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
static void battery_value_update(uint16_t bat_raw_adc);
static void adc_data_interrupt(uint32_t* out_data);
static void gen_line_events(void);
static void gen_battery_events(void);

/***************************************************************************************************
 * LOCAL VARIABLES
 **************************************************************************************************/

static volatile bool line_sensor_is_white[NUM_OF_LINE_SENSORS];
static volatile bool line_sensor_is_white_last[NUM_OF_LINE_SENSORS];
static volatile double battery_voltage_mv;
static volatile double battery_voltage_mv_last;

/***************************************************************************************************
 * GLOBAL VARIABLES
 **************************************************************************************************/

/***************************************************************************************************
 * LOCAL FUNCTIONS
 **************************************************************************************************/

static void battery_value_update(uint16_t bat_raw_adc){

    double measured_voltage = (bat_raw_adc / ADC_MAX_VALUE) * ADC_MAX_VOLTAGE;
    
    battery_voltage_mv = (measured_voltage * BAT_VOLTAGE_MULTIPLIER);

    gen_battery_events();

    battery_voltage_mv_last = battery_voltage_mv;
}

static void gen_battery_events() {

    if (battery_voltage_mv_last > BATTERY_THRESHOLD_MV && battery_voltage_mv <= BATTERY_THRESHOLD_MV){
        QEvt evt = {.sig = LOW_BATTERY_SIG};
        QHSM_DISPATCH(&AO_SumoHSM->super, &evt, SIMULATOR);
    }

}

static void gen_line_events(void){

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

static void adc_data_interrupt(uint32_t* out_data){

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

    battery_value_update(aux_readings[BAT_POSITION_IN_ADC]);


}

/***************************************************************************************************
 * GLOBAL FUNCTIONS
 **************************************************************************************************/

void adc_service_init() {
    BSP_ADC_DMA_Init();
    BSP_ADC_DMA_Start();
    BSP_ADC_DMA_Register_Callback(adc_data_interrupt);
}



bool adc_line_is_white(line_sensor_t position){

    if (position > NUM_OF_LINE_SENSORS){
        return 0;
    }

    return line_sensor_is_white[position];

}

bool adc_get_low_battery() {
    return (battery_voltage_mv <= BATTERY_THRESHOLD_MV);
}

double adc_get_battery_mv(){
    return battery_voltage_mv;
}