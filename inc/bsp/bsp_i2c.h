#ifndef BSP_I2c_H
#define BSP_I2c_H

#include <stdint.h>

typedef enum {
    BSP_I2C_OK,
    BSP_I2C_ERROR
} bsp_i2c_error_t;

typedef enum {
    IO_I2C_MEM_SIZE_8BIT,
    IO_I2C_MEM_SIZE_16BIT,
} bsp_i2c_mem_size_t;

typedef enum {
    IO_I2C_1
} io_i2c_channel_t;

void bsp_i2c_init(io_i2c_channel_t i2c_channel);
void bsp_i2c_deinit(io_i2c_channel_t i2c_channel);
bsp_i2c_error_t bsp_i2c_read(io_i2c_channel_t i2c_channel, uint16_t device_address, uint16_t mem_address,
                             bsp_i2c_mem_size_t mem_size, uint8_t *data, uint16_t size);
bsp_i2c_error_t bsp_i2c_write(io_i2c_channel_t i2c_channel, uint16_t device_address, uint16_t mem_address,
                              bsp_i2c_mem_size_t mem_size, uint8_t *data, uint16_t size);

int scan_i2c();

#endif /* BSP_I2c_H */
