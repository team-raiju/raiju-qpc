#ifndef BLE_SERVICE_H
#define BLE_SERVICE_H

#include <stdint.h>


void ble_service_init(void);

void ble_service_send_data(uint8_t * data, uint8_t size);
void ble_service_last_packet(uint8_t * data);

#endif /* BLE_SERVICE_H */
