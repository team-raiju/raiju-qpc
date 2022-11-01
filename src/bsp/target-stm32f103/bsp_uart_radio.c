/***************************************************************************************************
 * INCLUDES
 **************************************************************************************************/

#include <stdbool.h>
#include "usart.h"
#include "bsp_uart_radio.h"
/***************************************************************************************************
 * LOCAL DEFINES
 **************************************************************************************************/
#define RXDATA_SIZE 25
#define CHANNELS 16
#define RX_RADIO_FIRST_BYTE 0x0F
#define RX_RADIO_LAST_BYTE  0x00

/***************************************************************************************************
 * LOCAL TYPEDEFS
 **************************************************************************************************/

/***************************************************************************************************
 * LOCAL FUNCTION PROTOTYPES
 **************************************************************************************************/

/***************************************************************************************************
 * LOCAL VARIABLES
 **************************************************************************************************/
uint8_t rxdata[RXDATA_SIZE] = {0};
uint16_t channels[CHANNELS] = {0};
bool last_data_broken;
bsp_uart_radio_callback_t external_callback;


/***************************************************************************************************
 * GLOBAL VARIABLES
 **************************************************************************************************/

/***************************************************************************************************
 * LOCAL FUNCTIONS
 **************************************************************************************************/

void HAL_UART_RxCpltCallback(UART_HandleTypeDef* huart) {

    if (huart->Instance != huart1.Instance){
        return;
    }

    do {
        // invalid packet; Ignore packet
        if (rxdata[0] != RX_RADIO_FIRST_BYTE || rxdata[RXDATA_SIZE - 1] != RX_RADIO_LAST_BYTE) {
            memset(channels, 1000, sizeof(channels));
            break;
        }

        /* SBUS_SIGNAL_LOST */
        // if (rxdata[23] & (1 << 2)) {
        //     memset(channels, 1000, sizeof(channels));
        //     break;
        // }

        // /* SBUS_SIGNAL_FAILSAFE */
        // if (rxdata[23] & (1 << 3)) {
        //     memset(channels, 1000, sizeof(channels));
        //     break;
        // }

        // last_sbus_packet.reset();

        channels[0] = ((rxdata[1] | rxdata[2] << 8) & 0x07FF);
        channels[1] = ((rxdata[2] >> 3 | rxdata[3] << 5) & 0x07FF);
        channels[2] = ((rxdata[3] >> 6 | rxdata[4] << 2 | rxdata[5] << 10) & 0x07FF);
        channels[3] = ((rxdata[5] >> 1 | rxdata[6] << 7) & 0x07FF);
        channels[4] = ((rxdata[6] >> 4 | rxdata[7] << 4) & 0x07FF);
        channels[5] = ((rxdata[7] >> 7 | rxdata[8] << 1 | rxdata[9] << 9) & 0x07FF);
        channels[6] = ((rxdata[9] >> 2 | rxdata[10] << 6) & 0x07FF);
        channels[7] = ((rxdata[10] >> 5 | rxdata[11] << 3) & 0x07FF);
        channels[8] = ((rxdata[12] | rxdata[13] << 8) & 0x07FF);
        /* UNUSED */
        // channels[9] = ((rxdata[13] >> 3 | rxdata[14] << 5) & 0x07FF);
        // channels[10] = ((rxdata[14] >> 6 | rxdata[15] << 2 | rxdata[16] << 10) & 0x07FF);
        // channels[11] = ((rxdata[16] >> 1 | rxdata[17] << 7) & 0x07FF);
        // channels[12] = ((rxdata[17] >> 4 | rxdata[18] << 4) & 0x07FF);
        // channels[13] = ((rxdata[18] >> 7 | rxdata[19] << 1 | rxdata[20] << 9) & 0x07FF);
        // channels[14] = ((rxdata[20] >> 2 | rxdata[21] << 6) & 0x07FF);
        // channels[15] = ((rxdata[21] >> 5 | rxdata[22] << 3) & 0x07FF);
    } while (0);
    
    // external_callback()
    HAL_UART_Receive_IT(&huart1, rxdata, RXDATA_SIZE);

}


/***************************************************************************************************
 * GLOBAL FUNCTIONS
 **************************************************************************************************/
