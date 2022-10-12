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

}
