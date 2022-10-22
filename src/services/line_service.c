#include "line_service.h"
#include "bsp_adc.h"

#define WHITE_THRESHOLD 2500

void line_start() {

}

bool line_is_white(line_position_t position){

    BSP_ADC_CalcAndRestart();

    uint32_t adc_readings = BSP_ADC_GetReadings();

    return (adc_readings < WHITE_THRESHOLD);

}

uint16_t line_raw_reading(line_position_t pos){

    BSP_ADC_CalcAndRestart();

    uint32_t adc_readings = BSP_ADC_GetReadings();

    return adc_readings
 
}
