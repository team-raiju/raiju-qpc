#include "app.h"
#include "driving_service.h"
#include "distance_service.h"
#include "led_service.h"
#include "line_service.h"
#include "button.h"

void APP_init(){

    distance_service_init();
    driving_init();
    led_stripe_init();
    line_service_init();
    button_init();

}