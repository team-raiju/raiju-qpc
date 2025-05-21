/***************************************************************************************************
 * INCLUDES
 **************************************************************************************************/
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include "bsp_tim_input_capture.h"
#include "fake_start_module.h"
/***************************************************************************************************
 * LOCAL DEFINES
 **************************************************************************************************/

/* Microseconds per unit of time_diff */
#define TIME_DIFF_UNITS_US 10

/* SONY SIRC protocol */
#define HEADER_SIZE_US 3000
#define BIT_0_SIZE_US  1200
#define BIT_1_SIZE_US  1800

#define HEADER_SIZE (HEADER_SIZE_US / TIME_DIFF_UNITS_US)
#define BIT_0_SIZE  (BIT_0_SIZE_US / TIME_DIFF_UNITS_US)
#define BIT_1_SIZE  (BIT_1_SIZE_US / TIME_DIFF_UNITS_US)

/***************************************************************************************************
 * LOCAL TYPEDEFS
 **************************************************************************************************/
#define JSUMO_START_MODULE 0
#define IR_RECEIVER        1
#define START_MODULE_TYPE  IR_RECEIVER /* JSUMO_START_MODULE or IR_RECEIVER */
/***************************************************************************************************
 * LOCAL FUNCTION PROTOTYPES
 **************************************************************************************************/

/***************************************************************************************************
 * LOCAL VARIABLES
 **************************************************************************************************/
static bsp_tim_capture_callback capture_callback = NULL;
static bsp_tim_capture_edge_t current_edge = BSP_TIM_FALLING_EDGE;
/***************************************************************************************************
 * GLOBAL VARIABLES
 **************************************************************************************************/

/***************************************************************************************************
 * LOCAL FUNCTIONS
 **************************************************************************************************/

/***************************************************************************************************
 * GLOBAL FUNCTIONS
 **************************************************************************************************/
void bsp_tim_input_capture_init()
{
}

void bsp_tim_input_capture_start()
{
}

void bsp_tim_input_capture_stop()
{
}

void bsp_tim_capture_register_callback(bsp_tim_capture_callback callback_function)
{
    capture_callback = callback_function;
}

void bsp_tim_set_capture_level(bsp_tim_capture_edge_t edge)
{
    current_edge = edge;
}

#if START_MODULE_TYPE == JSUMO_START_MODULE
void fake_start_module_key_1(void)
{
}
void fake_start_module_key_2(void)
{
    if (current_edge == BSP_TIM_RISING_EDGE) {
        capture_callback(0);
    }
}
void fake_start_module_key_3(void)
{
    if (current_edge == BSP_TIM_FALLING_EDGE) {
        capture_callback(0);
    }
}
#else
void fake_start_module_key_1(void)
{
    if (capture_callback != NULL) {
        capture_callback(HEADER_SIZE);

        for (int i = 0; i < 7; i++) {
            capture_callback(BIT_0_SIZE);
        }

        capture_callback(30000);
    }
}

void fake_start_module_key_2(void)
{
    if (capture_callback != NULL) {
        capture_callback(HEADER_SIZE);

        capture_callback(BIT_1_SIZE);
        for (int i = 0; i < 6; i++) {
            capture_callback(BIT_0_SIZE);
        }

        capture_callback(30000);
    }
}

void fake_start_module_key_3(void)
{
    if (capture_callback != NULL) {
        capture_callback(HEADER_SIZE);

        capture_callback(BIT_0_SIZE);
        capture_callback(BIT_1_SIZE);
        for (int i = 0; i < 5; i++) {
            capture_callback(BIT_0_SIZE);
        }

        capture_callback(30000);
    }
}
#endif