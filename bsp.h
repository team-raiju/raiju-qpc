/*$file${.::bsp.h} vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv*/
/*
* Model: blinky_console.qm
* File:  ${.::bsp.h}
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
/*$endhead${.::bsp.h} ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^*/
#ifndef BSP_H
#define BSP_H

/* a very simple Board Support Package (BSP) -------------------------------*/
enum {
    BSP_TICKS_PER_SEC = 100
}; /* number of clock ticks in a second */

void BSP_init(void);
void BSP_ledOff(void);
void BSP_ledOn(void);
void BSP_ledToggle(void);
void BSP_motorsOff(void);
void BSP_ledStrip(int num, int stat);
void BSP_buzzer_beep(void);
void BSP_startRC(void);
void BSP_startAuto(void);

/* define the event signals used in the application ------------------------*/
enum SumoHSMSignals {
    TIMEOUT_SIG = Q_USER_SIG, /* offset the first signal by Q_USER_SIG */
    PLAY_BUZZER_SIG,
    START_RC_SIG,
    START_AUTO_SIG,
    MAX_SIG, /* keep last (the number of signals) */
};

/* active object(s) used in this application -------------------------------*/
extern QActive * const AO_SumoHSM; /* opaque pointer to the SumoHSM AO */
/*$declare${AOs::SumoHSM_ctor} vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv*/

/*${AOs::SumoHSM_ctor} .....................................................*/
void SumoHSM_ctor(void);
/*$enddecl${AOs::SumoHSM_ctor} ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^*/

#ifdef Q_SPY
void sumoHSM_update_qs_dict(void);
#endif

#endif /* BSP_H */
