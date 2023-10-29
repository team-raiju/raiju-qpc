/***************************************************************************************************
 * INCLUDES
 **************************************************************************************************/
#include <stdbool.h>
#include "bsp_i2c.h"
#include "i2c.h"
/***************************************************************************************************
 * LOCAL DEFINES
 **************************************************************************************************/

#define I2C_TIMEOUT 1000
/***************************************************************************************************
 * LOCAL TYPEDEFS
 **************************************************************************************************/

/***************************************************************************************************
 * LOCAL FUNCTION PROTOTYPES
 **************************************************************************************************/

/***************************************************************************************************
 * LOCAL VARIABLES
 **************************************************************************************************/
// static osMutexId_t i2c_mutexes[2];

static I2C_HandleTypeDef *i2c_handlers[1] = {
    /* IO_I2C_1 */ &hi2c1,
};

static uint16_t mem_add_size[2] = {
    /* BSP_I2C_MEM_SIZE_8BIT */ I2C_MEMADD_SIZE_8BIT,
    /* BSP_I2C_MEM_SIZE_16BIT */ I2C_MEMADD_SIZE_16BIT,
};

static bool i2c_is_init[1] = { false };

/***************************************************************************************************
 * GLOBAL VARIABLES
 **************************************************************************************************/

/***************************************************************************************************
 * LOCAL FUNCTIONS
 **************************************************************************************************/

/***************************************************************************************************
 * GLOBAL FUNCTIONS
 **************************************************************************************************/

void bsp_i2c_init(io_i2c_channel_t i2c_channel)
{
    if (i2c_is_init[i2c_channel] || i2c_channel > IO_I2C_1) {
        return;
    }

    MX_I2C1_Init();

    i2c_is_init[i2c_channel] = true;
}

void bsp_i2c_deinit(io_i2c_channel_t i2c_channel)
{
    if (i2c_channel > IO_I2C_1) {
        return;
    }

    HAL_I2C_DeInit(i2c_handlers[i2c_channel]);
    i2c_is_init[i2c_channel] = false;
}

bsp_i2c_error_t bsp_i2c_read(io_i2c_channel_t i2c_channel, uint16_t device_address, uint16_t mem_address,
                             bsp_i2c_mem_size_t mem_size, uint8_t *data, uint16_t size)
{
    HAL_StatusTypeDef ret;
    ret = HAL_I2C_Mem_Read(i2c_handlers[i2c_channel], device_address, mem_address, mem_add_size[mem_size], data, size,
                           I2C_TIMEOUT);

    return (ret == HAL_OK) ? BSP_I2C_OK : BSP_I2C_ERROR;
}

bsp_i2c_error_t bsp_i2c_write(io_i2c_channel_t i2c_channel, uint16_t device_address, uint16_t mem_address,
                              bsp_i2c_mem_size_t mem_size, uint8_t *data, uint16_t size)
{
    HAL_StatusTypeDef ret;
    ret = HAL_I2C_Mem_Write(i2c_handlers[i2c_channel], device_address, mem_address, mem_add_size[mem_size], data, size,
                            I2C_TIMEOUT);
    return (ret == HAL_OK) ? BSP_I2C_OK : BSP_I2C_ERROR;
}

int scan_i2c()
{
    uint8_t data[1];
    int ret = 0;

    if (!i2c_is_init[IO_I2C_1]) {
        return 0;
    }

    for (int i = 0; i < 0xff; i++) {
        if (HAL_I2C_Mem_Read(i2c_handlers[IO_I2C_1], i, 0, I2C_MEMADD_SIZE_8BIT, data, 1, 100) == HAL_OK) {
            ret++;
        }

        HAL_Delay(1);
    }

    return ret;
}