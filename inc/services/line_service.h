#ifndef LINE_SERVICE_H
#define LINE_SERVICE_H

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


void line_service_init(void);
bool line_is_white(line_sensor_t position);

#endif /* LINE_SERVICE_H */
