#include "app.h"
#include "driving_service.h"
#include "led_service.h"


void APP_init(){

    driving_init();
    led_stripe_init();

}