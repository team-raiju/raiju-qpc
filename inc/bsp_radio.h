#ifndef BSP_RADIO_H
#define BSP_RADIO_H

#include <stdint.h>

typedef enum {
   RADIO_CH1,
   RADIO_CH2,
   RADIO_CH3,
   RADIO_CH4,
   RADIO_CH_SIZE
} channel_num_t;

void BSP_radioInit(void);
int16_t BSP_radioGetChannel(channel_num_t ch);
void BSP_radioSetChannel(channel_num_t ch, int16_t value);



#endif /* BSP_RADIO_H */
