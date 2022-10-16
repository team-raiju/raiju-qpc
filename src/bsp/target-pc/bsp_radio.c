#include <stdio.h>  
#include "qpc.h"  
#include "bsp.h"
#include "bsp_radio.h"


#ifdef Q_SPY
#include "qs_defines.h"
#endif

int16_t channels[RADIO_CH_SIZE];


void BSP_radioInit(void){
    printf("Radio init \r\n"); 
}

int16_t BSP_radioGetChannel(channel_num_t ch){

    if (ch >= RADIO_CH_SIZE){
        return 0;
    }

    return channels[ch];

}

void BSP_radioSetChannel(channel_num_t ch, int16_t value){

    if (ch >= RADIO_CH_SIZE){
        return;
    }

    channels[ch] = value;

    // Check if event on ch3 or ch4

    static int16_t last_ch2;
    static int16_t last_ch3;
    static int16_t last_ch4;

    if (channels[RADIO_CH2] > 1 && last_ch2 == 0){
        QEvt evt = {.sig = RADIO_EVT_3_SIG};
        QHSM_DISPATCH(&AO_SumoHSM->super, &evt, SIMULATOR);
    }

    if (channels[RADIO_CH3] == 1 && last_ch3 == 0){
        QEvt evt = {.sig = RADIO_EVT_1_SIG};
        QHSM_DISPATCH(&AO_SumoHSM->super, &evt, SIMULATOR);
    }

    if (channels[RADIO_CH4] == 1 && last_ch4 == 0){
        QEvt evt = {.sig = RADIO_EVT_2_SIG};
        QHSM_DISPATCH(&AO_SumoHSM->super, &evt, SIMULATOR);
    }

    last_ch2 = channels[RADIO_CH2];
    last_ch3 = channels[RADIO_CH3];
    last_ch4 = channels[RADIO_CH4];


}
