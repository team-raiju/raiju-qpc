/***************************************************************************************************
 * INCLUDES
 **************************************************************************************************/

#include "qpc.h"    
#include "bsp.h"
#include "radio_service.h"
#include "utils.h"

#ifndef UART_RADIO
#include "bsp_ppm.h"
#else
#include "bsp_uart_radio.h"
#endif 

/***************************************************************************************************
 * LOCAL DEFINES
 **************************************************************************************************/

#define CHANNEL_MIN_VAL     (-100)
#define CHANNEL_MAX_VAL     (100)

/***************************************************************************************************
 * LOCAL TYPEDEFS
 **************************************************************************************************/


/***************************************************************************************************
 * LOCAL FUNCTION PROTOTYPES
 **************************************************************************************************/
static void radio_dispatch_events(void);

#ifndef UART_RADIO
static void radio_data_interrupt_ppm(uint8_t ppm_num, uint16_t ppm_val);
#endif 


/***************************************************************************************************
 * LOCAL VARIABLES
 **************************************************************************************************/
volatile int8_t radio_data[NUM_OF_RADIO_CHANNELS];
volatile int8_t last_radio_data[NUM_OF_RADIO_CHANNELS];

/***************************************************************************************************
 * GLOBAL VARIABLES
 **************************************************************************************************/

/***************************************************************************************************
 * LOCAL FUNCTIONS
 **************************************************************************************************/

static void radio_dispatch_events(){

    QEvt evt = {.sig = RADIO_DATA_SIG};
    QHSM_DISPATCH(&AO_SumoHSM->super, &evt, SIMULATOR);


    if (radio_data[RADIO_CH3] > 75 && last_radio_data[RADIO_CH3] < 75){
        QEvt evt = {.sig = RADIO_EVT_1_SIG};
        QHSM_DISPATCH(&AO_SumoHSM->super, &evt, SIMULATOR);
    }

    if (radio_data[RADIO_CH4] > 75 && last_radio_data[RADIO_CH4] < 75){
        QEvt evt = {.sig = RADIO_EVT_2_SIG};
        QHSM_DISPATCH(&AO_SumoHSM->super, &evt, SIMULATOR);
    }

    if (radio_data[RADIO_CH2] > 75 && last_radio_data[RADIO_CH2] < 75){
        QEvt evt = {.sig = RADIO_EVT_3_SIG};
        QHSM_DISPATCH(&AO_SumoHSM->super, &evt, SIMULATOR);
    }

}

#ifndef UART_RADIO

static void radio_data_interrupt_ppm(uint8_t ppm_num, uint16_t ppm_val){

    if (ppm_num >= NUM_OF_RADIO_CHANNELS){
        return;
    }

    ppm_val = constrain(ppm_val, PPM_MIN_VALUE_MS, PPM_MAX_VALUE_MS);

    radio_data[ppm_num] = map(ppm_val, PPM_MIN_VALUE_MS, PPM_MAX_VALUE_MS, CHANNEL_MIN_VAL, CHANNEL_MAX_VAL);

    radio_dispatch_events();

    last_radio_data[ppm_num] = radio_data[ppm_num];
    
}

#else 

static void radio_data_interrupt_uart(uint8_t ch_num, uint16_t ch_val){

    if (ch_num >= NUM_OF_RADIO_CHANNELS){
        return;
    }

    ch_val = constrain(ch_val, RX_RADIO_MIN_VALUE, RX_RADIO_MAX_VALUE);

    radio_data[ch_num] = map(ch_num, RX_RADIO_MIN_VALUE, RX_RADIO_MAX_VALUE, CHANNEL_MIN_VAL, CHANNEL_MAX_VAL);

    radio_dispatch_events();

    last_radio_data[ch_num] = radio_data[ch_num];
    
}

#endif 

/***************************************************************************************************
 * GLOBAL FUNCTIONS
 **************************************************************************************************/

void radio_service_init(){

    #ifndef UART_RADIO
    bsp_ppm_init();
    bsp_ppm_register_callback(radio_data_interrupt_ppm);
    #endif 


}

void radio_service_enable(){

    #ifndef UART_RADIO
    bsp_ppm_start();
    #endif 

}

void radio_service_disable(){

    #ifndef UART_RADIO
    bsp_ppm_stop();
    #endif 

}



int8_t radio_service_get_channel(radio_channel_t ch){

    if (ch > NUM_OF_RADIO_CHANNELS){
        return 0;
    }

    return radio_data[ch];

}
