/***************************************************************************************************
 * INCLUDES
 **************************************************************************************************/

#include "start_module.h"
#include "bsp_gpio.h"
#include "bsp_gpio_mapping.h"
#include "bsp_tim_input_capture.h"

/***************************************************************************************************
 * LOCAL DEFINES
 **************************************************************************************************/
/* Microseconds per unit of time_diff */
#define TIME_DIFF_UNITS_US   10     

/* SONY SIRC Protocol defines */
/* Ref: https://faculty-web.msoe.edu/johnsontimoj/Common/FILES/sony_sirc_protocol.pdf */
/* Ref: https://www.sbprojects.net/knowledge/ir/sirc.php */

#define HEADER_SIZE_US    3000
#define BIT_0_SIZE_US     1200
#define BIT_1_SIZE_US     1800

#define ERROR_MARGIN_US   100

#define HEADER_SIZE_MIN   (HEADER_SIZE_US - ERROR_MARGIN_US) / TIME_DIFF_UNITS_US
#define HEADER_SIZE_MAX   (HEADER_SIZE_US + ERROR_MARGIN_US) / TIME_DIFF_UNITS_US

#define BIT_0_SIZE_MIN    (BIT_0_SIZE_US - ERROR_MARGIN_US) / TIME_DIFF_UNITS_US
#define BIT_0_SIZE_MAX    (BIT_0_SIZE_US + ERROR_MARGIN_US) / TIME_DIFF_UNITS_US

#define BIT_1_SIZE_MIN    (BIT_1_SIZE_US - ERROR_MARGIN_US) / TIME_DIFF_UNITS_US
#define BIT_1_SIZE_MAX    (BIT_1_SIZE_US + ERROR_MARGIN_US) / TIME_DIFF_UNITS_US


/***************************************************************************************************
 * LOCAL TYPEDEFS
 **************************************************************************************************/

/***************************************************************************************************
 * LOCAL FUNCTION PROTOTYPES
 **************************************************************************************************/

static void tim_capture_interrupt(uint16_t time_diff);

/***************************************************************************************************
 * LOCAL VARIABLES
 **************************************************************************************************/

static uint8_t command_bits[7];
static uint8_t command;
static bool packet_started = false;
static uint8_t counter = 0;

/***************************************************************************************************
 * GLOBAL VARIABLES
 **************************************************************************************************/

/***************************************************************************************************
 * LOCAL FUNCTIONS
 **************************************************************************************************/

static void tim_capture_interrupt(uint16_t time_diff) {

    if (time_diff > HEADER_SIZE_MIN && time_diff < HEADER_SIZE_MAX) {
        counter = 0;
        packet_started = true;
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
        
        if (counter >= 7) {
            packet_started = false;
            command = 0;
            for (uint8_t i = 0; i < sizeof(command_bits); i++)
            {
                command |= command_bits[i] << i;
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
    bsp_tim_capture_register_callback(tim_capture_interrupt);
}


void start_module_disable(void)
{
    BSP_GPIO_Write_Pin(GPIO_START_MODULE_EN_PORT, GPIO_START_MODULE_EN_PIN, IO_LOW);
}

void start_module_enable(void)
{
    BSP_GPIO_Write_Pin(GPIO_START_MODULE_EN_PORT, GPIO_START_MODULE_EN_PIN, IO_HIGH);
}