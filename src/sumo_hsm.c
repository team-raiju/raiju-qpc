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
#include "qpc.h"    /* QP/C framework API */
#include "bsp.h"    /* Board Support Package interface */

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
    QTimeEvt_armX(&me->timeEvt, BSP_TICKS_PER_SEC/2, BSP_TICKS_PER_SEC/2);
    QTimeEvt_armX(&me->buzzerTimeEvt, BSP_TICKS_PER_SEC/10, 0);

    me->buzzerCount = 0U;

    QS_FUN_DICTIONARY(&SumoHSM_Idle);
    QS_FUN_DICTIONARY(&SumoHSM_RCWait);
    QS_FUN_DICTIONARY(&SumoHSM_StarStrategy);
    QS_FUN_DICTIONARY(&SumoHSM_AutoWait);
    QS_FUN_DICTIONARY(&SumoHSM_StepsStrategy);
    QS_FUN_DICTIONARY(&SumoHSM_LineGoBack);
    QS_FUN_DICTIONARY(&SumoHSM_LineTurn);

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
            BSP_startRC();
            status_ = Q_HANDLED();
            break;
        }
        /*${AOs::SumoHSM::SM::RCWait::RADIO_DATA} */
        case RADIO_DATA_SIG: {
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
            BSP_motors(60,60);
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
        /*${AOs::SumoHSM::SM::AutoWait::START_AUTO} */
        case START_AUTO_SIG: {
            /*${AOs::SumoHSM::SM::AutoWait::START_AUTO::[startegy_0]} */
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
                    BSP_motors(-80,80);
                    break;
                case 2:
                    BSP_motors(0,80);
                    break;
                case 3:
                    BSP_motors(100,100);
                    break;
                case 4:
                    BSP_motors(80,0);
                    break;
                case 5:
                    BSP_motors(80,-80);
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
            QTimeEvt_armX(&me->timeEvt, BSP_TICKS_PER_MILISSEC * 500, 0);
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
/*$enddef${AOs::SumoHSM} ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^*/

#ifdef Q_SPY

void sumoHSM_update_qs_dict(){

    QS_OBJ_DICTIONARY(&l_sumo_hsm);
    QS_OBJ_DICTIONARY(&l_sumo_hsm.timeEvt);
    QS_OBJ_DICTIONARY(&l_sumo_hsm.timeEvt_2);
    QS_OBJ_DICTIONARY(&l_sumo_hsm.buzzerTimeEvt);
    QS_OBJ_DICTIONARY(&l_sumo_hsm.strategy);

    QS_SIG_DICTIONARY(TIMEOUT_SIG,     (void *)0);
    QS_SIG_DICTIONARY(TIMEOUT_2_SIG, (void *)0);
    QS_SIG_DICTIONARY(PLAY_BUZZER_SIG,    (void *)0);
    QS_SIG_DICTIONARY(START_RC_SIG,  (void *)0);
    QS_SIG_DICTIONARY(START_AUTO_SIG,  (void *)0);
    QS_SIG_DICTIONARY(GO_TO_IDLE_SIG,  (void *)0);
    QS_SIG_DICTIONARY(STOP_AUTO_SIG,  (void *)0);
    QS_SIG_DICTIONARY(LINE_DETECTED_SIG,  (void *)0);
    QS_SIG_DICTIONARY(DIST_SENSOR_CHANGE_SIG,  (void *)0);
    QS_SIG_DICTIONARY(RADIO_DATA_SIG,  (void *)0);


}

#endif