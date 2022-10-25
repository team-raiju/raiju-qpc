/*$file${.::bsp.c} vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv*/
/*
* Model: blinky_console.qm
* File:  ${.::bsp.c}
*
* This code has been generated by QM 5.2.1 <www.state-machine.com/qm>.
* DO NOT EDIT THIS FILE MANUALLY. All your changes will be lost.
*
* SPDX-License-Identifier: GPL-3.0-or-later
*
* This generated code is open source software: you can redistribute it under
* the terms of the GNU General Public License as published by the Free
* Software Foundation.
*
* This code is distributed in the hope that it will be useful, but WITHOUT
* ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
* FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for
* more details.
*
* NOTE:
* Alternatively, this generated code may be distributed under the terms
* of Quantum Leaps commercial licenses, which expressly supersede the GNU
* General Public License and are specifically designed for licensees
* interested in retaining the proprietary status of their code.
*
* Contact information:
* <www.state-machine.com/licensing>
* <info@state-machine.com>
*/
/*$endhead${.::bsp.c} ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^*/
/* Board Support Package implementation for desktop OS (Windows, Linux, MacOS) */
#include <stdio.h>  /* for printf()/fprintf() */
#include <stdlib.h> /* for exit() */
#include <stdbool.h>

#include "qpc.h"    /* QP/C framework API */
#include "bsp.h"    /* Board Support Package interface */
#include "bsp_led.h"
#include "bsp_motors.h"
#include "bsp_buzzer.h"
#include "bsp_radio.h"
#include "bsp_adc_fake.h"
#include "bsp_gpio_fake.h"
#include "bsp_gpio.h"
#include "bsp_dist_sensor.h"


#ifdef Q_SPY

#include "qs_defines.h"
static QSpyId const l_clock_tick = { QS_AP_ID };

#endif

void BSP_init(void)   {
    printf("SumoHSM;  QP/C version: %s\r\n",  QP_VERSION_STR);

    BSP_ledInit();
    BSP_motorsInit();
    BSP_buzzerInit();
    BSP_radioInit();

    #ifdef Q_SPY

    QS_INIT((void *)0);
    
    QS_FUN_DICTIONARY(&QHsm_top);
    QS_OBJ_DICTIONARY(&l_clock_tick);
    QS_USR_DICTIONARY(SIMULATOR);

    /* setup the QS filters... */
    QS_GLB_FILTER(QS_ALL_RECORDS);
    QS_GLB_FILTER(-QS_QF_TICK);

    #endif

}


/* callback functions needed by the framework ------------------------------*/
void QF_onStartup(void) {
    QF_setTickRate(BSP_TICKS_PER_SEC, 50); /* desired tick rate/ticker-prio */
}
void QF_onCleanup(void) {}
void QF_onClockTick(void) {
    QF_TICK_X(0U, (void *)0); /* QF clock tick processing for rate 0 */

    #ifdef Q_SPY
    QS_RX_INPUT(); /* handle the QS-RX input */
    QS_OUTPUT();   /* handle the QS output */
    #endif

}
void Q_onAssert(char const * const module, int loc) {
    fprintf(stderr, "Assertion failed in %s:%d", module, loc);
    exit(-1);
}

#ifdef Q_SPY
/*..........................................................................*/
/*! callback function to execute user commands */
void QS_onCommand(uint8_t cmdId,
                  uint32_t param1, uint32_t param2, uint32_t param3)
{
    typedef struct {
    /* protected: */
        QActive super;

    /* private: */
        QTimeEvt timeEvt;
        QTimeEvt buzzerTimeEvt;
        uint8_t buzzerCount;
        uint8_t strategy;
        QTimeEvt timeEvt_2;
    } SumoHSM;

    switch (cmdId) {
       case 0: { 
            QEvt evt = {.sig = START_SIG};
            QHSM_DISPATCH(&AO_SumoHSM->super, &evt, SIMULATOR);
            break;
        }
        case 1: { 
            QEvt evt = {.sig = STOP_SIG};
            QHSM_DISPATCH(&AO_SumoHSM->super, &evt, SIMULATOR);
            break;
        }

        case 2: { 
            SumoHSM *me = (SumoHSM *)AO_SumoHSM;
            me->strategy = param1;
            printf("Strategy = %d\r\n", param1);
            break;
        }

        case 3: { 
            // When seeing line, real sensor responds with 0
            bool sensor_line_fl = !((param1 & 8) >> 3);
            bool sensor_line_fr = !((param1 & 4) >> 2);
            bool sensor_line_bl = !((param1 & 2) >> 1);
            bool sensor_line_br = !((param1 & 1));

            ADC_Fake_ConvCpltCallback(sensor_line_fl, sensor_line_fr, sensor_line_bl, sensor_line_br);

            break;
        }

        case 4: { 

            static uint8_t last_sensor_updated;

            uint8_t sensor_num = param1;

            if (sensor_num != 0){
                BSP_GPIO_Write_Pin(bsp_dist_sensor_port(sensor_num), bsp_dist_sensor_pin(sensor_num), true);
                HAL_Fake_GPIO_EXTI_Callback(sensor_num);
            } 

            BSP_GPIO_Write_Pin(bsp_dist_sensor_port(last_sensor_updated), bsp_dist_sensor_pin(last_sensor_updated), false);
            HAL_Fake_GPIO_EXTI_Callback(last_sensor_updated);

            last_sensor_updated = sensor_num;
            break;
        }

        case 5: { 
            QEvt evt = {.sig = RADIO_EVT_1_SIG};
            QHSM_DISPATCH(&AO_SumoHSM->super, &evt, SIMULATOR);
            break;
        }

        case 6: { 
            QEvt evt = {.sig = RADIO_EVT_2_SIG};
            QHSM_DISPATCH(&AO_SumoHSM->super, &evt, SIMULATOR);
            break;
        }

        case 7: { 
            QEvt evt = {.sig = RADIO_DATA_SIG};
            QHSM_DISPATCH(&AO_SumoHSM->super, &evt, SIMULATOR);
            int16_t radio_ch1 = param1 - 127;
            int16_t radio_ch2 = param2 - 127;
            BSP_radioSetChannel(RADIO_CH1, radio_ch1);
            BSP_radioSetChannel(RADIO_CH2, radio_ch2);
            break;
        }

        case 8: { 
            QEvt evt = {.sig = RADIO_DATA_SIG};
            QHSM_DISPATCH(&AO_SumoHSM->super, &evt, SIMULATOR);
            int16_t radio_ch3 = param1;
            int16_t radio_ch4 = param2;
            BSP_radioSetChannel(RADIO_CH3, radio_ch3);
            BSP_radioSetChannel(RADIO_CH4, radio_ch4);
            break;
        }



       default:
           break;
    }

    /* unused parameters */
    (void)param1;
    (void)param2;
    (void)param3;
}
#endif
