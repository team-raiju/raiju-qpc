/*$file${.::main.c} vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv*/
/*
* Model: blinky_console.qm
* File:  ${.::main.c}
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
/*$endhead${.::main.c} ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^*/
#include "qpc.h"    /* QP/C framework API */
#include "bsp.h"    /* Board Support Package interface */

Q_DEFINE_THIS_FILE

/* the main function -------------------------------------------------------*/
int main() {
    static QEvt const *sumoHSM_queueSto[10]; /* event queue buffer for SumoHSM */

    QF_init();  /* initialize the framework */
    BSP_init(); /* initialize the BSP */

    /* instantiate and start the SumoHSM active object */
    SumoHSM_ctor(); /* in C you must explicitly call the SumoHSM constructor */

    #ifdef Q_SPY
    sumoHSM_update_qs_dict();
    #endif

    QACTIVE_START(AO_SumoHSM, /* active object to start */
        1U,                  /* priority of the active object */
        sumoHSM_queueSto,     /* event queue buffer */
        Q_DIM(sumoHSM_queueSto), /* the length of the buffer */
        (void *)0, 0U,       /* private stack (not used) */
        (QEvt *)0);          /* initialization event (not used) */

    return QF_run(); /* let the framework run the application */
}
