/*$file${.::src::sumo_hsm.c} vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv*/
/*
* Model: sumo_hsm.qm
* File:  ${.::src::sumo_hsm.c}
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
/*$endhead${.::src::sumo_hsm.c} ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^*/
#include <math.h>
#include "qpc.h"    /* QP/C framework API */
#include "bsp.h"    /* Board Support Package interface */
#include "bsp_led.h"
#include "bsp_motors.h"

#define CALIB_ANGLE_MULT    2.5
#define M_PI                3.14159265

/* ask QM to declare the Blinky class --------------------------------------*/
/*$declare${AOs::SumoHSM} vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv*/

/*${AOs::SumoHSM} ..........................................................*/
typedef struct {
/* protected: */
    QActive super;

/* private: */
    QTimeEvt timeEvt;
    QTimeEvt buzzerTimeEvt;
    uint8_t buzzerCount;
    uint8_t strategy;
    QTimeEvt timeEvt_2;
    uint32_t calib_time_1;
    uint32_t calib_time_2;
    uint8_t calib_status;
    uint16_t turn_180_time_ms;
    uint8_t star_velocity;
} SumoHSM;

/* protected: */
static QState SumoHSM_initial(SumoHSM * const me, void const * const par);
static QState SumoHSM_Idle(SumoHSM * const me, QEvt const * const e);
static QState SumoHSM_RCWait(SumoHSM * const me, QEvt const * const e);
static QState SumoHSM_StarStrategy(SumoHSM * const me, QEvt const * const e);
static QState SumoHSM_AutoWait(SumoHSM * const me, QEvt const * const e);
static QState SumoHSM_StepsStrategy(SumoHSM * const me, QEvt const * const e);
static QState SumoHSM_LineGoBack(SumoHSM * const me, QEvt const * const e);
static QState SumoHSM_LineTurn(SumoHSM * const me, QEvt const * const e);
static QState SumoHSM_CalibTurn(SumoHSM * const me, QEvt const * const e);
static QState SumoHSM_CalibLineGoBack(SumoHSM * const me, QEvt const * const e);
static QState SumoHSM_CalibeLineTurn(SumoHSM * const me, QEvt const * const e);
static QState SumoHSM_CalibWait(SumoHSM * const me, QEvt const * const e);
static QState SumoHSM_CalibFront(SumoHSM * const me, QEvt const * const e);
static QState SumoHSM_CalibFrontGoBack(SumoHSM * const me, QEvt const * const e);
static QState SumoHSM_CalibeFrontTurn(SumoHSM * const me, QEvt const * const e);
/*$enddecl${AOs::SumoHSM} ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^*/

/* instantiate the Blinky active object ------------------------------------*/
static SumoHSM l_sumo_hsm;
QActive * const AO_SumoHSM = &l_sumo_hsm.super;

/* ask QM to define the Blinky class ---------------------------------------*/
/*$skip${QP_VERSION} vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv*/
/* Check for the minimum required QP version */
#if (QP_VERSION < 690U) || (QP_VERSION != ((QP_RELEASE^4294967295U) % 0x3E8U))
#error qpc version 6.9.0 or higher required
#endif
/*$endskip${QP_VERSION} ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^*/

/*$define${AOs::SumoHSM_ctor} vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv*/

/*${AOs::SumoHSM_ctor} .....................................................*/
void SumoHSM_ctor(void) {
    SumoHSM *me = (SumoHSM *)AO_SumoHSM;
    QActive_ctor(&me->super, Q_STATE_CAST(&SumoHSM_initial));
    QTimeEvt_ctorX(&me->timeEvt, &me->super, TIMEOUT_SIG, 0U);
    QTimeEvt_ctorX(&me->timeEvt_2, &me->super, TIMEOUT_2_SIG, 0U);
    QTimeEvt_ctorX(&me->buzzerTimeEvt, &me->super, PLAY_BUZZER_SIG, 0U);
    me->strategy = 0;
    me->calib_time_1 = 0;
    me->calib_time_2 = 0;
    me->calib_status = 0;
    me->turn_180_time_ms = 800;
    me->star_velocity = 60;
}
/*$enddef${AOs::SumoHSM_ctor} ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^*/
/*$define${AOs::SumoHSM} vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv*/

/*${AOs::SumoHSM} ..........................................................*/

/*${AOs::SumoHSM::SM} ......................................................*/
static QState SumoHSM_initial(SumoHSM * const me, void const * const par) {
    /*${AOs::SumoHSM::SM::initial} */
    (void)par; /* unused parameter */
    /* arm the private time event to expire in 1/2s
    * and periodically every 1/2 second
    */
    QTimeEvt_armX(&me->buzzerTimeEvt, BSP_TICKS_PER_SEC/10, 0);

    me->buzzerCount = 0U;

    QS_FUN_DICTIONARY(&SumoHSM_Idle);
    QS_FUN_DICTIONARY(&SumoHSM_RCWait);
    QS_FUN_DICTIONARY(&SumoHSM_StarStrategy);
    QS_FUN_DICTIONARY(&SumoHSM_AutoWait);
    QS_FUN_DICTIONARY(&SumoHSM_StepsStrategy);
    QS_FUN_DICTIONARY(&SumoHSM_LineGoBack);
    QS_FUN_DICTIONARY(&SumoHSM_LineTurn);
    QS_FUN_DICTIONARY(&SumoHSM_CalibTurn);
    QS_FUN_DICTIONARY(&SumoHSM_CalibLineGoBack);
    QS_FUN_DICTIONARY(&SumoHSM_CalibeLineTurn);
    QS_FUN_DICTIONARY(&SumoHSM_CalibWait);
    QS_FUN_DICTIONARY(&SumoHSM_CalibFront);
    QS_FUN_DICTIONARY(&SumoHSM_CalibFrontGoBack);
    QS_FUN_DICTIONARY(&SumoHSM_CalibeFrontTurn);

    return Q_TRAN(&SumoHSM_Idle);
}

/*${AOs::SumoHSM::SM::Idle} ................................................*/
static QState SumoHSM_Idle(SumoHSM * const me, QEvt const * const e) {
    QState status_;
    switch (e->sig) {
        /*${AOs::SumoHSM::SM::Idle} */
        case Q_ENTRY_SIG: {
            BSP_ledOff();
            BSP_motors(0,0);
            QTimeEvt_armX(&me->timeEvt, BSP_TICKS_PER_SEC/2, BSP_TICKS_PER_SEC/2);
            status_ = Q_HANDLED();
            break;
        }
        /*${AOs::SumoHSM::SM::Idle} */
        case Q_EXIT_SIG: {
            QTimeEvt_disarm(&me->timeEvt);
            QTimeEvt_disarm(&me->buzzerTimeEvt);
            status_ = Q_HANDLED();
            break;
        }
        /*${AOs::SumoHSM::SM::Idle::TIMEOUT} */
        case TIMEOUT_SIG: {
            BSP_ledToggle();
            status_ = Q_HANDLED();
            break;
        }
        /*${AOs::SumoHSM::SM::Idle::START_RC} */
        case START_RC_SIG: {
            status_ = Q_TRAN(&SumoHSM_RCWait);
            break;
        }
        /*${AOs::SumoHSM::SM::Idle::START_AUTO} */
        case START_AUTO_SIG: {
            status_ = Q_TRAN(&SumoHSM_AutoWait);
            break;
        }
        /*${AOs::SumoHSM::SM::Idle::PLAY_BUZZER} */
        case PLAY_BUZZER_SIG: {
            BSP_ledStrip(me->buzzerCount, 1);
            BSP_buzzer_beep();

            if (me->buzzerCount == 16) {
                QTimeEvt_armX(&me->buzzerTimeEvt, 1.6 * BSP_TICKS_PER_SEC, 0);
            } else if (me->buzzerCount < 16){
                QTimeEvt_armX(&me->buzzerTimeEvt, BSP_TICKS_PER_SEC/10, 0);
            }


            me->buzzerCount += 1;
            status_ = Q_HANDLED();
            break;
        }
        /*${AOs::SumoHSM::SM::Idle::START_CALIB} */
        case START_CALIB_SIG: {
            status_ = Q_TRAN(&SumoHSM_CalibWait);
            break;
        }
        default: {
            status_ = Q_SUPER(&QHsm_top);
            break;
        }
    }
    return status_;
}

/*${AOs::SumoHSM::SM::RCWait} ..............................................*/
static QState SumoHSM_RCWait(SumoHSM * const me, QEvt const * const e) {
    QState status_;
    switch (e->sig) {
        /*${AOs::SumoHSM::SM::RCWait} */
        case Q_ENTRY_SIG: {
            BSP_ledOff();
            status_ = Q_HANDLED();
            break;
        }
        /*${AOs::SumoHSM::SM::RCWait::RADIO_DATA} */
        case RADIO_DATA_SIG: {
            int coord_x = BSP_Get_Radio_X();
            int coord_y = BSP_Get_Radio_Y();

            int mot1 = coord_y + coord_x;
            int mot2 = coord_y - coord_x;

            BSP_motors(mot1, mot2);
            status_ = Q_HANDLED();
            break;
        }
        /*${AOs::SumoHSM::SM::RCWait::GO_TO_IDLE} */
        case GO_TO_IDLE_SIG: {
            status_ = Q_TRAN(&SumoHSM_Idle);
            break;
        }
        default: {
            status_ = Q_SUPER(&QHsm_top);
            break;
        }
    }
    return status_;
}

/*${AOs::SumoHSM::SM::StarStrategy} ........................................*/
static QState SumoHSM_StarStrategy(SumoHSM * const me, QEvt const * const e) {
    QState status_;
    switch (e->sig) {
        /*${AOs::SumoHSM::SM::StarStrategy} */
        case Q_ENTRY_SIG: {
            BSP_motors(me->star_velocity, me->star_velocity);
            status_ = Q_HANDLED();
            break;
        }
        /*${AOs::SumoHSM::SM::StarStrategy::LINE_DETECTED} */
        case LINE_DETECTED_SIG: {
            status_ = Q_TRAN(&SumoHSM_LineGoBack);
            break;
        }
        /*${AOs::SumoHSM::SM::StarStrategy::STOP_AUTO} */
        case STOP_AUTO_SIG: {
            status_ = Q_TRAN(&SumoHSM_AutoWait);
            break;
        }
        /*${AOs::SumoHSM::SM::StarStrategy::DIST_SENSOR_CHANGE} */
        case DIST_SENSOR_CHANGE_SIG: {
            int sensors_on = BSP_Check_Dist();

            switch(sensors_on){
                case 0:
                    BSP_motors(60, 60);
                    break;
                case 1:
                    BSP_motors(80,-80);
                    break;
                case 2:
                    BSP_motors(80,0);
                    break;
                case 3:
                    BSP_motors(100,100);
                    break;
                case 4:
                    BSP_motors(0,80);
                    break;
                case 5:
                    BSP_motors(-80,80);
                    break;
            }

            status_ = Q_HANDLED();
            break;
        }
        default: {
            status_ = Q_SUPER(&QHsm_top);
            break;
        }
    }
    return status_;
}

/*${AOs::SumoHSM::SM::AutoWait} ............................................*/
static QState SumoHSM_AutoWait(SumoHSM * const me, QEvt const * const e) {
    QState status_;
    switch (e->sig) {
        /*${AOs::SumoHSM::SM::AutoWait} */
        case Q_ENTRY_SIG: {
            BSP_motors(0,0);
            BSP_ledOn();
            status_ = Q_HANDLED();
            break;
        }
        /*${AOs::SumoHSM::SM::AutoWait::START_AUTO} */
        case START_AUTO_SIG: {
            /*${AOs::SumoHSM::SM::AutoWait::START_AUTO::[strategy_0]} */
            if (me->strategy == 0) {
                status_ = Q_TRAN(&SumoHSM_StarStrategy);
            }
            /*${AOs::SumoHSM::SM::AutoWait::START_AUTO::[strategy_1]} */
            else if (me->strategy == 1) {
                status_ = Q_TRAN(&SumoHSM_StepsStrategy);
            }
            else {
                status_ = Q_UNHANDLED();
            }
            break;
        }
        /*${AOs::SumoHSM::SM::AutoWait::GO_TO_IDLE} */
        case GO_TO_IDLE_SIG: {
            status_ = Q_TRAN(&SumoHSM_Idle);
            break;
        }
        default: {
            status_ = Q_SUPER(&QHsm_top);
            break;
        }
    }
    return status_;
}

/*${AOs::SumoHSM::SM::StepsStrategy} .......................................*/
static QState SumoHSM_StepsStrategy(SumoHSM * const me, QEvt const * const e) {
    QState status_;
    switch (e->sig) {
        /*${AOs::SumoHSM::SM::StepsStrategy} */
        case Q_ENTRY_SIG: {
            QTimeEvt_armX(&me->timeEvt, 3 * BSP_TICKS_PER_SEC, 3 * BSP_TICKS_PER_SEC);
            status_ = Q_HANDLED();
            break;
        }
        /*${AOs::SumoHSM::SM::StepsStrategy} */
        case Q_EXIT_SIG: {
            QTimeEvt_disarm(&me->timeEvt);
            QTimeEvt_disarm(&me->timeEvt_2);
            status_ = Q_HANDLED();
            break;
        }
        /*${AOs::SumoHSM::SM::StepsStrategy::LINE_DETECTED} */
        case LINE_DETECTED_SIG: {
            status_ = Q_TRAN(&SumoHSM_LineGoBack);
            break;
        }
        /*${AOs::SumoHSM::SM::StepsStrategy::TIMEOUT} */
        case TIMEOUT_SIG: {
            BSP_motors(100,100);
            QTimeEvt_armX(&me->timeEvt_2, BSP_TICKS_PER_MILISSEC * 100, 0);
            status_ = Q_HANDLED();
            break;
        }
        /*${AOs::SumoHSM::SM::StepsStrategy::TIMEOUT_2} */
        case TIMEOUT_2_SIG: {
            BSP_motors(0,0);
            status_ = Q_HANDLED();
            break;
        }
        /*${AOs::SumoHSM::SM::StepsStrategy::STOP_AUTO} */
        case STOP_AUTO_SIG: {
            status_ = Q_TRAN(&SumoHSM_AutoWait);
            break;
        }
        /*${AOs::SumoHSM::SM::StepsStrategy::DIST_SENSOR_CHANGE} */
        case DIST_SENSOR_CHANGE_SIG: {
            int sensors_on = BSP_Check_Dist();
            QTimeEvt_disarm(&me->timeEvt);
            QTimeEvt_disarm(&me->timeEvt_2);

            switch(sensors_on){
                case 0:
                    BSP_motors(0,0);
                    QTimeEvt_armX(&me->timeEvt, 3 * BSP_TICKS_PER_SEC, 3 * BSP_TICKS_PER_SEC);
                    break;
                case 1:
                    BSP_motors(80,-80);
                    break;
                case 2:
                    BSP_motors(80,0);
                    break;
                case 3:
                    BSP_motors(100,100);
                    break;
                case 4:
                    BSP_motors(0,80);
                    break;
                case 5:
                    BSP_motors(-80,80);
                    break;
            }

            status_ = Q_HANDLED();
            break;
        }
        default: {
            status_ = Q_SUPER(&QHsm_top);
            break;
        }
    }
    return status_;
}

/*${AOs::SumoHSM::SM::LineGoBack} ..........................................*/
static QState SumoHSM_LineGoBack(SumoHSM * const me, QEvt const * const e) {
    QState status_;
    switch (e->sig) {
        /*${AOs::SumoHSM::SM::LineGoBack} */
        case Q_ENTRY_SIG: {
            BSP_motors(-100,-100);
            QTimeEvt_armX(&me->timeEvt, BSP_TICKS_PER_MILISSEC * 250, 0);
            status_ = Q_HANDLED();
            break;
        }
        /*${AOs::SumoHSM::SM::LineGoBack::TIMEOUT} */
        case TIMEOUT_SIG: {
            status_ = Q_TRAN(&SumoHSM_LineTurn);
            break;
        }
        default: {
            status_ = Q_SUPER(&QHsm_top);
            break;
        }
    }
    return status_;
}

/*${AOs::SumoHSM::SM::LineGoBack::LineTurn} ................................*/
static QState SumoHSM_LineTurn(SumoHSM * const me, QEvt const * const e) {
    QState status_;
    switch (e->sig) {
        /*${AOs::SumoHSM::SM::LineGoBack::LineTurn} */
        case Q_ENTRY_SIG: {
            BSP_motors(-100,100);
            QTimeEvt_armX(&me->timeEvt, BSP_TICKS_PER_MILISSEC * me->turn_180_time_ms * (0.6), 0);
            status_ = Q_HANDLED();
            break;
        }
        /*${AOs::SumoHSM::SM::LineGoBack::LineTurn} */
        case Q_EXIT_SIG: {
            BSP_motors(0,0);
            status_ = Q_HANDLED();
            break;
        }
        /*${AOs::SumoHSM::SM::LineGoBack::LineTurn::TIMEOUT} */
        case TIMEOUT_SIG: {
            /*${AOs::SumoHSM::SM::LineGoBack::LineTurn::TIMEOUT::[strategy_0]} */
            if (me->strategy == 0) {
                status_ = Q_TRAN(&SumoHSM_StarStrategy);
            }
            /*${AOs::SumoHSM::SM::LineGoBack::LineTurn::TIMEOUT::[strategy_1]} */
            else if (me->strategy == 1) {
                status_ = Q_TRAN(&SumoHSM_StepsStrategy);
            }
            else {
                status_ = Q_UNHANDLED();
            }
            break;
        }
        default: {
            status_ = Q_SUPER(&SumoHSM_LineGoBack);
            break;
        }
    }
    return status_;
}

/*${AOs::SumoHSM::SM::CalibTurn} ...........................................*/
static QState SumoHSM_CalibTurn(SumoHSM * const me, QEvt const * const e) {
    QState status_;
    switch (e->sig) {
        /*${AOs::SumoHSM::SM::CalibTurn} */
        case Q_ENTRY_SIG: {
            QTimeEvt_armX(&me->timeEvt_2, 0xFFFFFFFF, 0);
            BSP_motors(me->star_velocity, me->star_velocity);
            status_ = Q_HANDLED();
            break;
        }
        /*${AOs::SumoHSM::SM::CalibTurn::LINE_DETECTED} */
        case LINE_DETECTED_SIG: {
            if (me->calib_status == 0){
                me->calib_time_1 = 0xFFFFFFFF - QTimeEvt_currCtr(&me->timeEvt_2);
            } else {
                me->calib_time_2 = 0xFFFFFFFF - QTimeEvt_currCtr(&me->timeEvt_2);
            }
            status_ = Q_TRAN(&SumoHSM_CalibLineGoBack);
            break;
        }
        default: {
            status_ = Q_SUPER(&QHsm_top);
            break;
        }
    }
    return status_;
}

/*${AOs::SumoHSM::SM::CalibTurn::CalibLineGoBack} ..........................*/
static QState SumoHSM_CalibLineGoBack(SumoHSM * const me, QEvt const * const e) {
    QState status_;
    switch (e->sig) {
        /*${AOs::SumoHSM::SM::CalibTurn::CalibLineGoBack} */
        case Q_ENTRY_SIG: {
            BSP_motors(-100,-100);
            QTimeEvt_armX(&me->timeEvt, BSP_TICKS_PER_MILISSEC * 250, 0);
            status_ = Q_HANDLED();
            break;
        }
        /*${AOs::SumoHSM::SM::CalibTurn::CalibLineGoBack::TIMEOUT} */
        case TIMEOUT_SIG: {
            status_ = Q_TRAN(&SumoHSM_CalibeLineTurn);
            break;
        }
        default: {
            status_ = Q_SUPER(&SumoHSM_CalibTurn);
            break;
        }
    }
    return status_;
}

/*${AOs::SumoHSM::SM::CalibTurn::CalibLineGoBack::CalibeLineTurn} ..........*/
static QState SumoHSM_CalibeLineTurn(SumoHSM * const me, QEvt const * const e) {
    QState status_;
    switch (e->sig) {
        /*${AOs::SumoHSM::SM::CalibTurn::CalibLineGoBack::CalibeLineTurn} */
        case Q_ENTRY_SIG: {
            BSP_motors(-100,100);
            QTimeEvt_armX(&me->timeEvt, BSP_TICKS_PER_MILISSEC * me->turn_180_time_ms, 0);
            status_ = Q_HANDLED();
            break;
        }
        /*${AOs::SumoHSM::SM::CalibTurn::CalibLineGoBack::CalibeLineTurn::TIMEOUT} */
        case TIMEOUT_SIG: {
            /*${AOs::SumoHSM::SM::CalibTurn::CalibLineGoBack::CalibeLineTurn::TIMEOUT::[calib_0]} */
            if (me->calib_status == 0) {
                me->calib_status++;
                QTimeEvt_rearm(&me->timeEvt_2, 0xFFFFFFFF);
                BSP_motors(60,60);

                status_ = Q_TRAN(&SumoHSM_CalibTurn);
            }
            /*${AOs::SumoHSM::SM::CalibTurn::CalibLineGoBack::CalibeLineTurn::TIMEOUT::[calib_1]} */
            else if (me->calib_status != 0) {
                QTimeEvt_disarm(&me->timeEvt_2);

                uint8_t angle_diff;
                if (me->calib_time_2 > me->calib_time_1){
                    angle_diff = 0;
                } else {
                    angle_diff = ((180 / M_PI) * acos((me->calib_time_2) / (double) me->calib_time_1));
                }

                if (me->strategy == 0) {
                    me->turn_180_time_ms += angle_diff * CALIB_ANGLE_MULT;
                } else {
                    me->turn_180_time_ms -= angle_diff * CALIB_ANGLE_MULT;
                }


                status_ = Q_TRAN(&SumoHSM_CalibWait);
            }
            else {
                status_ = Q_UNHANDLED();
            }
            break;
        }
        default: {
            status_ = Q_SUPER(&SumoHSM_CalibLineGoBack);
            break;
        }
    }
    return status_;
}

/*${AOs::SumoHSM::SM::CalibWait} ...........................................*/
static QState SumoHSM_CalibWait(SumoHSM * const me, QEvt const * const e) {
    QState status_;
    switch (e->sig) {
        /*${AOs::SumoHSM::SM::CalibWait} */
        case Q_ENTRY_SIG: {
            BSP_motors(0,0);
            me->calib_status = 0;
            status_ = Q_HANDLED();
            break;
        }
        /*${AOs::SumoHSM::SM::CalibWait::START_CALIB} */
        case START_CALIB_SIG: {
            /*${AOs::SumoHSM::SM::CalibWait::START_CALIB::[strategy_0||strategy_1]} */
            if (me->strategy == 0 || me->strategy == 1) {
                status_ = Q_TRAN(&SumoHSM_CalibTurn);
            }
            /*${AOs::SumoHSM::SM::CalibWait::START_CALIB::[strategy_2]} */
            else if (me->strategy == 2) {
                status_ = Q_TRAN(&SumoHSM_CalibFront);
            }
            else {
                status_ = Q_UNHANDLED();
            }
            break;
        }
        /*${AOs::SumoHSM::SM::CalibWait::GO_TO_IDLE} */
        case GO_TO_IDLE_SIG: {
            status_ = Q_TRAN(&SumoHSM_Idle);
            break;
        }
        default: {
            status_ = Q_SUPER(&QHsm_top);
            break;
        }
    }
    return status_;
}

/*${AOs::SumoHSM::SM::CalibFront} ..........................................*/
static QState SumoHSM_CalibFront(SumoHSM * const me, QEvt const * const e) {
    QState status_;
    switch (e->sig) {
        /*${AOs::SumoHSM::SM::CalibFront} */
        case Q_ENTRY_SIG: {
            QTimeEvt_armX(&me->timeEvt_2, 0xFFFFFFFF, 0);
            BSP_motors(me->star_velocity, me->star_velocity);
            status_ = Q_HANDLED();
            break;
        }
        /*${AOs::SumoHSM::SM::CalibFront::LINE_DETECTED} */
        case LINE_DETECTED_SIG: {
            uint32_t time_until_line = 0xFFFFFFFF - QTimeEvt_currCtr(&me->timeEvt_2);

            int16_t diff_to_reference = time_until_line - 285;

            me->star_velocity += (diff_to_reference / 5);


            status_ = Q_TRAN(&SumoHSM_CalibFrontGoBack);
            break;
        }
        default: {
            status_ = Q_SUPER(&QHsm_top);
            break;
        }
    }
    return status_;
}

/*${AOs::SumoHSM::SM::CalibFront::CalibFrontGoBack} ........................*/
static QState SumoHSM_CalibFrontGoBack(SumoHSM * const me, QEvt const * const e) {
    QState status_;
    switch (e->sig) {
        /*${AOs::SumoHSM::SM::CalibFront::CalibFrontGoBack} */
        case Q_ENTRY_SIG: {
            BSP_motors(-100,-100);
            QTimeEvt_armX(&me->timeEvt, BSP_TICKS_PER_MILISSEC * 250, 0);
            status_ = Q_HANDLED();
            break;
        }
        /*${AOs::SumoHSM::SM::CalibFront::CalibFrontGoBack::TIMEOUT} */
        case TIMEOUT_SIG: {
            status_ = Q_TRAN(&SumoHSM_CalibeFrontTurn);
            break;
        }
        default: {
            status_ = Q_SUPER(&SumoHSM_CalibFront);
            break;
        }
    }
    return status_;
}

/*${AOs::SumoHSM::SM::CalibFront::CalibFrontGoBack::CalibeFrontTurn} .......*/
static QState SumoHSM_CalibeFrontTurn(SumoHSM * const me, QEvt const * const e) {
    QState status_;
    switch (e->sig) {
        /*${AOs::SumoHSM::SM::CalibFront::CalibFrontGoBack::CalibeFrontTurn} */
        case Q_ENTRY_SIG: {
            BSP_motors(-100,100);
            QTimeEvt_armX(&me->timeEvt, BSP_TICKS_PER_MILISSEC * me->turn_180_time_ms, 0);
            status_ = Q_HANDLED();
            break;
        }
        /*${AOs::SumoHSM::SM::CalibFront::CalibFrontGoBack::CalibeFrontTurn::TIMEOUT} */
        case TIMEOUT_SIG: {
            QTimeEvt_disarm(&me->timeEvt_2);
            status_ = Q_TRAN(&SumoHSM_CalibWait);
            break;
        }
        default: {
            status_ = Q_SUPER(&SumoHSM_CalibFrontGoBack);
            break;
        }
    }
    return status_;
}
/*$enddef${AOs::SumoHSM} ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^*/

#ifdef Q_SPY

void sumoHSM_update_qs_dict(){

    QS_OBJ_DICTIONARY(&l_sumo_hsm);
    QS_OBJ_DICTIONARY(&l_sumo_hsm.timeEvt);
    QS_OBJ_DICTIONARY(&l_sumo_hsm.timeEvt_2);
    QS_OBJ_DICTIONARY(&l_sumo_hsm.buzzerTimeEvt);
    QS_OBJ_DICTIONARY(&l_sumo_hsm.strategy);
    QS_OBJ_DICTIONARY(&l_sumo_hsm.calib_time_1);
    QS_OBJ_DICTIONARY(&l_sumo_hsm.calib_time_2);

    QS_SIG_DICTIONARY(TIMEOUT_SIG,     (void *)0);
    QS_SIG_DICTIONARY(TIMEOUT_2_SIG, (void *)0);
    QS_SIG_DICTIONARY(PLAY_BUZZER_SIG,    (void *)0);
    QS_SIG_DICTIONARY(START_RC_SIG,  (void *)0);
    QS_SIG_DICTIONARY(START_AUTO_SIG,  (void *)0);
    QS_SIG_DICTIONARY(START_CALIB_SIG,  (void *)0);
    QS_SIG_DICTIONARY(GO_TO_IDLE_SIG,  (void *)0);
    QS_SIG_DICTIONARY(STOP_AUTO_SIG,  (void *)0);
    QS_SIG_DICTIONARY(LINE_DETECTED_SIG,  (void *)0);
    QS_SIG_DICTIONARY(DIST_SENSOR_CHANGE_SIG,  (void *)0);
    QS_SIG_DICTIONARY(RADIO_DATA_SIG,  (void *)0);


}

#endif
