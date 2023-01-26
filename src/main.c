/*$file${.::src::main.c} vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv*/
/*
* Model: sumo_hsm.qm
* File:  ${.::src::main.c}
*
* This code has been generated by QM 5.2.2 <www.state-machine.com/qm>.
* DO NOT EDIT THIS FILE MANUALLY. All your changes will be lost.
*
* This code is covered by the following QP license:
* License #    : QM-EVAL-QPC
* Issued to    : Users of QP/C Real-Time Embedded Framework (RTEF)
* Framework(s) : qpc
* Support ends : 2023-12-31
* License scope:
*
* SPDX-License-Identifier: GPL-3.0-or-later
*
* This generated code is free software: you can redistribute it under
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
/*$endhead${.::src::main.c} ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^*/
#include "qpc.h"    /* QP/C framework API */
#include "bsp.h"    /* Board Support Package interface */
#include "app.h"
#include "start_module_ao.h"

Q_DEFINE_THIS_FILE

/* the main function -------------------------------------------------------*/
int main() {
    static QEvt const *sumoHSM_queueSto[10]; /* event queue buffer for SumoHSM */

    QF_init();  /* initialize the framework */
    BSP_init(); /* initialize the BSP */

    sumo_parameters_init();
    APP_init();

    /* instantiate and start the SumoHSM active object */
    SumoHSM_ctor(); /* in C you must explicitly call the SumoHSM constructor */

    #ifdef Q_SPY
    sumoHSM_update_qs_dict();
    #endif

    QACTIVE_START(AO_SumoHSM, /* active object to start */
        2U,                  /* priority of the active object */
        sumoHSM_queueSto,     /* event queue buffer */
        Q_DIM(sumoHSM_queueSto), /* the length of the buffer */
        (void *)0, 0U,       /* private stack (not used) */
        (QEvt *)0);          /* initialization event (not used) */

    start_module_ao_init();

    return QF_run(); /* let the framework run the application */
}
