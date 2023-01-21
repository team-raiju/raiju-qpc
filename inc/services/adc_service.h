#ifndef ADC_SERVICE_H
#define ADC_SERVICE_H

#include <stdint.h>
#include <stdbool.h>

#define NUM_OF_LINE_SENSORS 4

// Position of sensor on ADC DMA buffer
typedef enum line_sensor 
{
    LINE_FR, 
    LINE_FL,
    LINE_BR,      
    LINE_BL,
} line_sensor_t;


void adc_service_init(void);
void adc_service_start_callback();
bool adc_line_is_white(line_sensor_t position);
uint16_t adc_line_get_all(void);
bool adc_line_none_white();
bool adc_get_low_ctrl_bat(void);
double adc_get_ctrl_bat_mv(void);
bool adc_get_low_pwr_bat(void);
double adc_get_pwr_bat_mv(void);
void adc_line_set_mask(uint8_t mask);
uint32_t adc_get_raw_line(line_sensor_t position);

#endif /* ADC_SERVICE_H */
