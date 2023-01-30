#ifndef BSP_ADC_FAKE_H
#define BSP_ADC_FAKE_H

#include <stdbool.h>

void ADC_Fake_ConvCpltCallback(bool fl, bool fr, bool bl, bool br, bool ctrl_bat_full, bool pwr_bat_full);
void ADC_Fake_Start_Module(bool stat);

#endif /* BSP_ADC_FAKE_H */
