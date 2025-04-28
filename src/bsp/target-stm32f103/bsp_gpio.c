/***************************************************************************************************
 * INCLUDES
 **************************************************************************************************/

#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include "bsp_gpio.h"
#include "gpio.h"
#include "bsp_gpio_mapping.h"

#include "qpc.h"
#include "qk_port.h"
/***************************************************************************************************
 * LOCAL DEFINES
 **************************************************************************************************/

/***************************************************************************************************
 * LOCAL TYPEDEFS
 **************************************************************************************************/

/***************************************************************************************************
 * LOCAL FUNCTION PROTOTYPES
 **************************************************************************************************/

/***************************************************************************************************
 * LOCAL VARIABLES
 **************************************************************************************************/
static bsp_gpio_dist_callback_t dist_callback_function = NULL;
static bsp_button_callback_t button_callback_function = NULL;

static uint8_t gpio_dist_sensor_pins[BOARD_NUM_DIST_SENSORS] = {
    GPIO_DIST_SENSOR_1_PIN, /* HARDWARE SILK DIST 1 */
    GPIO_DIST_SENSOR_9_PIN, /* HARDWARE SILK DIST 9 */
    GPIO_DIST_SENSOR_3_PIN, /* HARDWARE SILK DIST 3 */
    // GPIO_DIST_SENSOR_4_PIN, /* HARDWARE SILK DIST 4 - UNUSED*/
    // GPIO_DIST_SENSOR_5_PIN, /* HARDWARE SILK DIST 5 - UNUSED*/
    GPIO_DIST_SENSOR_6_PIN, /* HARDWARE SILK DIST 6 */
    GPIO_DIST_SENSOR_7_PIN, /* HARDWARE SILK DIST 7 */
    GPIO_DIST_SENSOR_8_PIN, /* HARDWARE SILK DIST 8 */
    GPIO_DIST_SENSOR_2_PIN, /* HARDWARE SILK DIST 2 */
};

static uint8_t gpio_dist_sensor_ports[BOARD_NUM_DIST_SENSORS] = {
    GPIO_DIST_SENSOR_1_PORT, /* HARDWARE SILK DIST 1 */
    GPIO_DIST_SENSOR_9_PORT, /* HARDWARE SILK DIST 9 */
    GPIO_DIST_SENSOR_3_PORT, /* HARDWARE SILK DIST 3 */
    // GPIO_DIST_SENSOR_4_PORT, /* HARDWARE SILK DIST 4 - UNUSED*/
    // GPIO_DIST_SENSOR_5_PORT, /* HARDWARE SILK DIST 5 - UNUSED*/
    GPIO_DIST_SENSOR_6_PORT, /* HARDWARE SILK DIST 6 */
    GPIO_DIST_SENSOR_7_PORT, /* HARDWARE SILK DIST 7 */
    GPIO_DIST_SENSOR_8_PORT, /* HARDWARE SILK DIST 8 */
    GPIO_DIST_SENSOR_2_PORT, /* HARDWARE SILK DIST 2 */
};

/***************************************************************************************************
 * GLOBAL VARIABLES
 **************************************************************************************************/

/***************************************************************************************************
 * LOCAL FUNCTIONS
 **************************************************************************************************/
static uint16_t BSP_GPIO_Pin_Mapping(io_pin_t io_pin)
{
    uint16_t hardware_pin_num[] = { GPIO_PIN_0,  GPIO_PIN_1,  GPIO_PIN_2,  GPIO_PIN_3, GPIO_PIN_4,  GPIO_PIN_5,
                                    GPIO_PIN_6,  GPIO_PIN_7,  GPIO_PIN_8,  GPIO_PIN_9, GPIO_PIN_10, GPIO_PIN_11,
                                    GPIO_PIN_12, GPIO_PIN_13, GPIO_PIN_14, GPIO_PIN_15 };

    if (io_pin < sizeof(hardware_pin_num) / sizeof(uint16_t)) {
        return hardware_pin_num[io_pin];
    }

    return GPIO_PIN_0;
}

static GPIO_TypeDef *BSP_GPIO_Port_Mapping(io_port_t io_port)
{
    GPIO_TypeDef *hardware_port_num[] = {
        GPIOA, //IO_PORTA
        GPIOB, //IO_PORTB
        GPIOC, //IO_PORTC
        GPIOD, //IO_PORTD
    };

    if (io_port < sizeof(hardware_port_num) / sizeof(GPIO_TypeDef *)) {
        return hardware_port_num[io_port];
    }

    return GPIOA;
}

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
    QK_ISR_ENTRY();

    // Button
    if (GPIO_Pin == BSP_GPIO_Pin_Mapping(GPIO_BUTTON_PIN)) {
        if (button_callback_function != NULL) {
            button_callback_function();
        }
        return;
    }

    // Distance sensors
    for (int i = 0; i < BOARD_NUM_DIST_SENSORS; i++) {
        if (GPIO_Pin == BSP_GPIO_Pin_Mapping(gpio_dist_sensor_pins[i])) {
            if (dist_callback_function != NULL) {
                io_level_t state = BSP_GPIO_Read_Pin(gpio_dist_sensor_ports[i], gpio_dist_sensor_pins[i]);
                dist_callback_function(i, state);
            }
            return;
        }
    }

    QK_ISR_EXIT();
}

/***************************************************************************************************
 * GLOBAL FUNCTIONS
 **************************************************************************************************/
io_level_t BSP_GPIO_Read_Pin(io_port_t port, io_pin_t gpio_pin)
{
    return (io_level_t)HAL_GPIO_ReadPin(BSP_GPIO_Port_Mapping(port), BSP_GPIO_Pin_Mapping(gpio_pin));
}

void BSP_GPIO_Write_Pin(io_port_t port, io_pin_t gpio_pin, io_level_t level)
{
    HAL_GPIO_WritePin(BSP_GPIO_Port_Mapping(port), BSP_GPIO_Pin_Mapping(gpio_pin), (uint16_t)level);
}

void BSP_GPIO_Toggle_Pin(io_port_t port, io_pin_t gpio_pin)
{
    HAL_GPIO_TogglePin(BSP_GPIO_Port_Mapping(port), BSP_GPIO_Pin_Mapping(gpio_pin));
}

void BSP_GPIO_Register_Distance_Callback(bsp_gpio_dist_callback_t callback_function)
{
    dist_callback_function = callback_function;
}

void BSP_GPIO_Register_Button_Callback(bsp_button_callback_t callback_function)
{
    button_callback_function = callback_function;
}