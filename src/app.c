#include "app.h"
#include "driving_service.h"
#include "distance_service.h"
#include "led_service.h"
#include "line_service.h"
#include "button.h"
#include "buzzer_service.h"
#include "radio_service.h"
#include "bsp_eeprom.h"

void APP_init(){

    distance_service_init();
    driving_init();
    led_stripe_init();
    line_service_init();
    button_init();
    buzzer_service_init();
    radio_service_init();
    BSP_eeprom_init();

}