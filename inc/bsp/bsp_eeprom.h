#ifndef BSP_EEPROM_H
#define BSP_EEPROM_H

#include <stdint.h>

typedef enum eeprom_result {

    EEPROM_OK,
    EEPROM_ERROR,
    EEPROM_DATA_NOT_FOUND,

} eeprom_result_t;

eeprom_result_t BSP_eeprom_init();
eeprom_result_t BSP_eeprom_read(uint16_t address, uint32_t* data);
eeprom_result_t BSP_eeprom_write(uint16_t address, uint32_t data);


#endif /* BSP_EEPROM_H */
