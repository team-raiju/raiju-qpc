/***************************************************************************************************
 * INCLUDES
 **************************************************************************************************/
#include <stdbool.h>
#include <string.h>

#include "ble_service.h"
#include "bsp_ble.h"
#include "bsp.h"
#include "qpc.h"    


/***************************************************************************************************
 * LOCAL DEFINES
 **************************************************************************************************/

/***************************************************************************************************
 * LOCAL TYPEDEFS
 **************************************************************************************************/

/***************************************************************************************************
 * LOCAL FUNCTION PROTOTYPES
 **************************************************************************************************/

static void ble_callback(uint8_t * ble_data, uint8_t rcv_size);
static void ble_process_events(ble_rcv_packet_t rcv_packet);

/***************************************************************************************************
 * LOCAL VARIABLES
 **************************************************************************************************/

static volatile ble_rcv_packet_t ble_last_data;

/***************************************************************************************************
 * GLOBAL VARIABLES
 **************************************************************************************************/

/***************************************************************************************************
 * LOCAL FUNCTIONS
 **************************************************************************************************/

static void ble_process_events(ble_rcv_packet_t rcv_packet){

    // QEvt evt = {.sig = BLE_DATA_SIG};
    // QHSM_DISPATCH(&AO_SumoHSM->super, &evt, SIMULATOR);

    if (rcv_packet._raw[0] == 0xFE && rcv_packet._raw[1] == 0xFE){
        // Data request
    } else if (rcv_packet._raw[0] == 0xFE && rcv_packet._raw[1] == 0xEF){
        QEvt evt = {.sig = CHANGE_STATE_EVT_SIG};
        QHSM_DISPATCH(&AO_SumoHSM->super, &evt, SIMULATOR);
    } else {
        // Change Strategy
    }

}

static void ble_callback(uint8_t * ble_data, uint8_t rcv_size) {

    if (rcv_size != BLE_RECEIVE_PACKET_SIZE){
        return;
    }

    // Checksum

    memcpy((void * restrict) ble_last_data._raw, ble_data, BLE_RECEIVE_PACKET_SIZE);

    ble_process_events(ble_last_data);

}


/***************************************************************************************************
 * GLOBAL FUNCTIONS
 **************************************************************************************************/

void ble_service_init(void){
    bsp_ble_init();
    bsp_ble_register_callback(ble_callback);
}


void ble_service_send_data(uint8_t * data, uint8_t size){

    bsp_ble_transmit(data, size);

}

void ble_service_last_packet(ble_rcv_packet_t * data){

    *data = ble_last_data;

}

