#include "bsp_radio.h"

int16_t channels[RADIO_CH_SIZE];

void BSP_radioInit(void){

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
