/***************************************************************************************************
 * INCLUDES
 **************************************************************************************************/

#include "qpc.h"
#include "start_module_ao.h"
#include "bsp.h"
#include "bsp_gpio_mapping.h"

Q_DEFINE_THIS_FILE
/***************************************************************************************************
 * LOCAL DEFINES
 **************************************************************************************************/

#define CHECK_START_DELAY_MS        10
#define CHECK_STOP_DELAY_MS         100

/***************************************************************************************************
 * LOCAL TYPEDEFS
 **************************************************************************************************/

typedef struct start_module {
    QActive super;
    QTimeEvt timeEvt;
} start_module_t;

/***************************************************************************************************
 * LOCAL FUNCTION PROTOTYPES
 **************************************************************************************************/
static void start_module_disable(void);
static void start_module_enable(void);
static void start_module_ctor(void);
static QState StartModule_Initial(start_module_t * const me, void const * const par);
static QState StartModule_Idle(start_module_t * const me, QEvt const * const e);
static QState StartModule_WaitStart(start_module_t * const me, QEvt const * const e);
static QState StartModule_WaitStop(start_module_t * const me, QEvt const * const e);

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

static void start_module_disable(void) {
    BSP_GPIO_Write_Pin(GPIO_START_MODULE_EN_PORT, GPIO_START_MODULE_EN_PIN, IO_LOW);
}

static void start_module_enable(void) {
    BSP_GPIO_Write_Pin(GPIO_START_MODULE_EN_PORT, GPIO_START_MODULE_EN_PIN, IO_HIGH);
}

static void start_module_ctor(void) {
    start_module_t *me = &start_module_inst;
    QActive_ctor(&me->super, Q_STATE_CAST(&StartModule_Initial));
    QTimeEvt_ctorX(&me->timeEvt, &me->super, START_MODULE_CHECK_SIG, 0U);
}

static QState StartModule_Initial(start_module_t * const me, void const * const par) {

    (void)par; /* unused parameter */

    QS_FUN_DICTIONARY(&StartModule_Idle);
    QS_FUN_DICTIONARY(&StartModule_WaitStart);
    QS_FUN_DICTIONARY(&StartModule_WaitStop);

    return Q_TRAN(&StartModule_Idle);
}

static QState StartModule_Idle(start_module_t * const me, QEvt const * const e) {
    QState status_;
    switch (e->sig) {
        case Q_ENTRY_SIG: {
            QTimeEvt_disarm(&me->timeEvt);
            start_module_disable();
            status_ = Q_HANDLED();
            break;
        }
        case START_MODULE_CHECK_SIG: {
            start_module_enable();
            status_ = Q_TRAN(&StartModule_WaitStart);
            break;
        }
        default: {
            status_ = Q_SUPER(&QHsm_top);
            break;
        }
    }
    return status_;
}

static QState StartModule_WaitStart(start_module_t * const me, QEvt const * const e) {
    QState status_;
    switch (e->sig) {
        case Q_ENTRY_SIG: {
            QTimeEvt_disarm(&me->timeEvt);
            QTimeEvt_armX(&me->timeEvt, CHECK_START_DELAY_MS * BSP_TICKS_PER_MILISSEC, CHECK_START_DELAY_MS * BSP_TICKS_PER_MILISSEC);
            status_ = Q_HANDLED();
            break;
        }
        case START_MODULE_CHECK_SIG: {
            io_level_t state = BSP_GPIO_Read_Pin(GPIO_START_MODULE_PORT, GPIO_START_MODULE_PIN);
            if (state == IO_HIGH){
                QEvt evt = {.sig = START_SIG};
                QHSM_DISPATCH(&AO_SumoHSM->super, &evt, SIMULATOR);
                status_ = Q_TRAN(&StartModule_WaitStop);
            } else {
                status_ = Q_HANDLED();
            }
            
            break;
        }
        case START_MODULE_DISABLE_SIG: {
            status_ = Q_TRAN(&StartModule_Idle);
            break;
        }
        default: {
            status_ = Q_SUPER(&QHsm_top);
            break;
        }
    }
    return status_;
}

static QState StartModule_WaitStop(start_module_t * const me, QEvt const * const e) {
    QState status_;
    switch (e->sig) {
        case Q_ENTRY_SIG: {
            QTimeEvt_disarm(&me->timeEvt);
            QTimeEvt_armX(&me->timeEvt, CHECK_STOP_DELAY_MS * BSP_TICKS_PER_MILISSEC, CHECK_STOP_DELAY_MS * BSP_TICKS_PER_MILISSEC);
            status_ = Q_HANDLED();
            break;
        }
        case START_MODULE_CHECK_SIG: {
            io_level_t state = BSP_GPIO_Read_Pin(GPIO_START_MODULE_PORT, GPIO_START_MODULE_PIN);
            if (state == IO_LOW){
                QEvt evt = {.sig = STOP_SIG};
                QHSM_DISPATCH(&AO_SumoHSM->super, &evt, SIMULATOR);
                status_ = Q_TRAN(&StartModule_Idle);
            } else {
                status_ = Q_HANDLED();
            }
            break;
        }
        case START_MODULE_DISABLE_SIG: {
            status_ = Q_TRAN(&StartModule_Idle);
            break;
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


void start_module_check_event(){
    QEvt evt = {.sig = START_MODULE_CHECK_SIG};
    QHSM_DISPATCH(&start_module_AO->super, &evt, SIMULATOR);
}

void start_module_disable_event(){
    QEvt evt = {.sig = START_MODULE_DISABLE_SIG};
    QHSM_DISPATCH(&start_module_AO->super, &evt, SIMULATOR);
}


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