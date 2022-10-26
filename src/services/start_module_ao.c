/***************************************************************************************************
 * INCLUDES
 **************************************************************************************************/

#include "qpc.h"
#include "start_module_ao.h"
#include "bsp.h"
#include <stdio.h> 

Q_DEFINE_THIS_FILE
/***************************************************************************************************
 * LOCAL DEFINES
 **************************************************************************************************/

/***************************************************************************************************
 * LOCAL TYPEDEFS
 **************************************************************************************************/
enum BlinkySignals {
    START_MODULE_CHECK_SIG = MAX_SIG_SUMO_HSM,
    MAX_SIG_START_MODULE
};

typedef struct start_module {
    QActive super;
    QTimeEvt timeEvt;
} start_module_t;

/***************************************************************************************************
 * LOCAL FUNCTION PROTOTYPES
 **************************************************************************************************/

static QState StartModule_Initial(start_module_t * const me, void const * const par);
static QState StartModule_Check(start_module_t * const me, QEvt const * const e);

/***************************************************************************************************
 * LOCAL VARIABLES
 **************************************************************************************************/

start_module_t start_module_inst;
QActive * const start_module_AO = &start_module_inst.super;

/***************************************************************************************************
 * GLOBAL VARIABLES
 **************************************************************************************************/

/***************************************************************************************************
 * LOCAL FUNCTIONS
 **************************************************************************************************/

static void start_module_ctor(void) {
    start_module_t *me = &start_module_inst;
    QActive_ctor(&me->super, Q_STATE_CAST(&StartModule_Initial));
    QTimeEvt_ctorX(&me->timeEvt, &me->super, START_MODULE_CHECK_SIG, 0U);
}

static QState StartModule_Initial(start_module_t * const me, void const * const par) {

    (void)par; /* unused parameter */
    QTimeEvt_armX(&me->timeEvt, 100,100);

    QS_FUN_DICTIONARY(&StartModule_Check);

    return Q_TRAN(&StartModule_Check);
}

static QState StartModule_Check(start_module_t * const me, QEvt const * const e) {
    QState status_;
    switch (e->sig) {
        case Q_ENTRY_SIG: {
            status_ = Q_HANDLED();
            break;
        }
        case START_MODULE_CHECK_SIG: {
            printf("Check Sensor \r\n");
        }
        default: {
            status_ = Q_SUPER(&QHsm_top);
            break;
        }
    }
    return status_;
}

/***************************************************************************************************
 * GLOBAL FUNCTIONS
 **************************************************************************************************/


void start_module_ao_init() {
    /* statically allocate event queue buffer for the start_module_t AO */
    static QEvt const *start_module_queueSto[10];

    start_module_ctor(); /* explicitly call the "constructor" */
    QACTIVE_START(start_module_AO,
                  3U, /* priority */
                  start_module_queueSto, Q_DIM(start_module_queueSto),
                  (void *)0, 0U, /* no stack */
                  (void *)0);    /* no initialization parameter */
}