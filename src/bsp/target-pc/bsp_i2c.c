/***************************************************************************************************
 * INCLUDES
 **************************************************************************************************/
#include <stdbool.h>
#include <stdio.h>
#include "bsp_i2c.h"

#include "qpc.h"
#include "bsp.h"
#include "utils.h"

#ifdef Q_SPY
#include "qs_defines.h"
#endif

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

static bool i2c_is_init[2] = { false };

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

    printf("I2C channel 1 init \r\n");

    i2c_is_init[i2c_channel] = true;
}

void bsp_i2c_deinit(io_i2c_channel_t i2c_channel)
{
    if (i2c_channel > IO_I2C_1) {
        return;
    }

    printf("I2C deinit channel enum %d\r\n", i2c_channel);

    i2c_is_init[i2c_channel] = false;
}

bsp_i2c_error_t bsp_i2c_read(io_i2c_channel_t i2c_channel, uint16_t device_address, uint16_t mem_address,
                             bsp_i2c_mem_size_t mem_size, uint8_t *data, uint16_t size)
{
    (void)i2c_channel;
    (void)mem_size;

    return BSP_I2C_OK;
}

bsp_i2c_error_t bsp_i2c_write(io_i2c_channel_t i2c_channel, uint16_t device_address, uint16_t mem_address,
                              bsp_i2c_mem_size_t mem_size, uint8_t *data, uint16_t size)
{
    (void)i2c_channel;
    (void)mem_size;

    return BSP_I2C_OK;
}

int scan_i2c()
{
    int ret = 0;
    for (int i = 0; i < 0xff; i++) {
        printf("Trying to read i2c addr 0x%x\r\n", i);
    }

    return ret;
}
