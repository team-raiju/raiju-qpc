/***************************************************************************************************
 * INCLUDES
 **************************************************************************************************/
#include "qpc.h"
#include "bsp.h"

#include "start_module.h"
#include "bsp_gpio.h"
#include "bsp_gpio_mapping.h"
#include "bsp_tim_input_capture.h"

/***************************************************************************************************
 * LOCAL DEFINES
 **************************************************************************************************/
#define JSUMO_START_MODULE      0
#define IR_RECEIVER             1
#define START_MODULE_TYPE       IR_RECEIVER      /* JSUMO_START_MODULE or IR_RECEIVER */

/* Microseconds per unit of time_diff */
#define TIME_DIFF_UNITS_US 10

/* SONY SIRC Protocol defines */
/* Ref: https://faculty-web.msoe.edu/johnsontimoj/Common/FILES/sony_sirc_protocol.pdf */
/* Ref: https://www.sbprojects.net/knowledge/ir/sirc.php */

#define HEADER_SIZE_US 3000
#define BIT_0_SIZE_US  1200
#define BIT_1_SIZE_US  1800

#define ERROR_MARGIN_UP_US   150
#define ERROR_MARGIN_DOWN_US 100

#define HEADER_SIZE_MIN ((HEADER_SIZE_US - ERROR_MARGIN_DOWN_US) / TIME_DIFF_UNITS_US)
#define HEADER_SIZE_MAX ((HEADER_SIZE_US + ERROR_MARGIN_UP_US) / TIME_DIFF_UNITS_US)

#define BIT_0_SIZE_MIN ((BIT_0_SIZE_US - ERROR_MARGIN_DOWN_US) / TIME_DIFF_UNITS_US)
#define BIT_0_SIZE_MAX ((BIT_0_SIZE_US + ERROR_MARGIN_UP_US) / TIME_DIFF_UNITS_US)

#define BIT_1_SIZE_MIN ((BIT_1_SIZE_US - ERROR_MARGIN_DOWN_US) / TIME_DIFF_UNITS_US)
#define BIT_1_SIZE_MAX ((BIT_1_SIZE_US + ERROR_MARGIN_UP_US) / TIME_DIFF_UNITS_US)

#define MIN_TIME_US_BETWEEN_EVENTS    300000
#define MIN_TIME_UNITS_BETWEEN_EVENTS (MIN_TIME_US_BETWEEN_EVENTS / TIME_DIFF_UNITS_US)

/***************************************************************************************************
 * LOCAL TYPEDEFS
 **************************************************************************************************/
typedef enum {
    KEY_1 = 0x00,
    KEY_2 = 0x01,
    KEY_3 = 0x02,
    KEY_4 = 0x03,
    KEY_5 = 0x04,
    KEY_6 = 0x05,
    KEY_7 = 0x06,
    KEY_8 = 0x07,
    KEY_9 = 0x08,
    KEY_0 = 0x09,
    KEY_CH_UP = 0x10,
    KEY_CH_DOWN = 0x11,
    KEY_VOL_UP = 0x12,
    KEY_VOL_DOWN = 0x13,
    KEY_MUTE = 0x14,
    KEY_POWER = 0x15,
    KEY_ENTER = 0x0B
} sirc_cmd_codes_t;

/***************************************************************************************************
 * LOCAL FUNCTION PROTOTYPES
 **************************************************************************************************/

static void tim_capture_interrupt(uint16_t time_diff);
static void tim_capture_interrupt_jsumo(uint16_t time_diff);
static void gen_start_module_events(sirc_cmd_codes_t key_pressed);

/***************************************************************************************************
 * LOCAL VARIABLES
 **************************************************************************************************/
/* Variable for Jsumo module */
#if (START_MODULE_TYPE == JSUMO_START_MODULE)
static bsp_tim_capture_edge_t current_edge = BSP_TIM_RISING_EDGE;
#else
static bsp_tim_capture_edge_t current_edge = BSP_TIM_FALLING_EDGE;
#endif

/* Variables for Generic IR module */
/* command_bits can have up to 7 bits, but we are only interested in the first 2 */
static uint8_t command_bits[2];
static bool packet_started = false;
static uint8_t counter = 0;
static uint32_t time_from_last_evt = MIN_TIME_UNITS_BETWEEN_EVENTS;

/***************************************************************************************************
 * GLOBAL VARIABLES
 **************************************************************************************************/

/***************************************************************************************************
 * LOCAL FUNCTIONS
 **************************************************************************************************/

static void gen_start_module_events(sirc_cmd_codes_t key_pressed)
{
    switch (key_pressed) {
    case KEY_1: {
        QEvt evt = { .sig = START_MODULE_TEST_SIG };
        QHSM_DISPATCH(&AO_SumoHSM->super, &evt, SIMULATOR);
        break;
    }
    case KEY_2: {
        QEvt evt = { .sig = START_SIG };
        QHSM_DISPATCH(&AO_SumoHSM->super, &evt, SIMULATOR);
        break;
    }
    case KEY_3: {
        QEvt evt = { .sig = STOP_SIG };
        QHSM_DISPATCH(&AO_SumoHSM->super, &evt, SIMULATOR);
        break;
    }
    case KEY_4: {
        QEvt evt = { .sig = CHANGE_STATE_EVT_SIG };
        QHSM_DISPATCH(&AO_SumoHSM->super, &evt, SIMULATOR);
        break;
    }
    default:
        break;
    }
}

static void tim_capture_interrupt_jsumo(uint16_t time_diff)
{
    Q_UNUSED_PAR(time_diff);
    if (current_edge == BSP_TIM_RISING_EDGE) {
        gen_start_module_events(KEY_2);
        current_edge = BSP_TIM_FALLING_EDGE;
    } else {
        gen_start_module_events(KEY_3);
        current_edge = BSP_TIM_RISING_EDGE;
    }

    bsp_tim_set_capture_level(current_edge);
}

static void tim_capture_interrupt(uint16_t time_diff)
{
    time_from_last_evt += time_diff;

    if (time_diff > HEADER_SIZE_MIN && time_diff < HEADER_SIZE_MAX) {
        counter = 0;
        packet_started = true;
        return;
    }

    if (packet_started) {
        if (time_diff > BIT_0_SIZE_MIN && time_diff < BIT_0_SIZE_MAX) {
            command_bits[counter] = 0;
        } else if (time_diff > BIT_1_SIZE_MIN && time_diff < BIT_1_SIZE_MAX) {
            command_bits[counter] = 1;
        } else {
            packet_started = false;
            return;
        }

        counter++;

        if (counter >= sizeof(command_bits)) {
            packet_started = false;
            uint8_t command = 0;
            for (uint8_t i = 0; i < sizeof(command_bits); i++) {
                command |= command_bits[i] << i;
            }

            if (time_from_last_evt > MIN_TIME_UNITS_BETWEEN_EVENTS) {
                gen_start_module_events(command);
                time_from_last_evt = 0;
            }
        }
    }
}


/***************************************************************************************************
 * GLOBAL FUNCTIONS
 **************************************************************************************************/
void start_module_init_capture(void)
{
    bsp_tim_input_capture_init();
    bsp_tim_set_capture_level(current_edge);
    #if (START_MODULE_TYPE == JSUMO_START_MODULE)
    bsp_tim_capture_register_callback(tim_capture_interrupt_jsumo);
    #else
    bsp_tim_capture_register_callback(tim_capture_interrupt);
    #endif

}

void start_module_disable(void)
{
    BSP_GPIO_Write_Pin(GPIO_START_MODULE_EN_PORT, GPIO_START_MODULE_EN_PIN, IO_LOW);
}

void start_module_enable(void)
{
    BSP_GPIO_Write_Pin(GPIO_START_MODULE_EN_PORT, GPIO_START_MODULE_EN_PIN, IO_HIGH);
}

void start_module_change_type(void){
    #if (START_MODULE_TYPE == JSUMO_START_MODULE)
    current_edge = BSP_TIM_FALLING_EDGE;
    bsp_tim_set_capture_level(current_edge);
    bsp_tim_capture_register_callback(tim_capture_interrupt);
    #else
    current_edge = BSP_TIM_RISING_EDGE;
    bsp_tim_set_capture_level(current_edge);
    bsp_tim_capture_register_callback(tim_capture_interrupt_jsumo);
    #endif
}