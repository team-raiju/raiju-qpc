/***************************************************************************************************
 * INCLUDES
 **************************************************************************************************/

#include <stdbool.h>
#include "qpc.h"
#include "bsp.h"
#include "radio_service.h"
#include "utils.h"

#if defined(RADIO_MODE_UART)
#include "bsp_uart_radio.h"
#elif defined(RADIO_MODE_UART_CRSF)
#include "bsp_uart_crsf.h"
#endif

/***************************************************************************************************
 * LOCAL DEFINES
 **************************************************************************************************/

#define CHANNEL_MIN_VAL (-100)
#define CHANNEL_MAX_VAL (100)

/***************************************************************************************************
 * LOCAL TYPEDEFS
 **************************************************************************************************/

/***************************************************************************************************
 * LOCAL FUNCTION PROTOTYPES
 **************************************************************************************************/
static void radio_dispatch_events(void);

static void radio_data_interrupt_uart(uint16_t *ch_data, uint8_t ch_amount);

/***************************************************************************************************
 * LOCAL VARIABLES
 **************************************************************************************************/
static volatile int8_t radio_data[NUM_OF_RADIO_CHANNELS];
static volatile int8_t last_radio_data[NUM_OF_RADIO_CHANNELS];
static bool radio_data_sig_enable = true;

/***************************************************************************************************
 * GLOBAL VARIABLES
 **************************************************************************************************/

/***************************************************************************************************
 * LOCAL FUNCTIONS
 **************************************************************************************************/
static void radio_dispatch_events()
{
    if (radio_data_sig_enable) {
        QEvt evt = { .sig = RADIO_DATA_SIG };
        QHSM_DISPATCH(&AO_SumoHSM->super, &evt, SIMULATOR);
    }

    if (radio_data[RADIO_CH3] > 75 && last_radio_data[RADIO_CH3] <= 75) {
        QEvt evt = { .sig = CHANGE_STATE_EVT_SIG };
        QHSM_DISPATCH(&AO_SumoHSM->super, &evt, SIMULATOR);
    }

    if (radio_data[RADIO_CH6] > 75 && last_radio_data[RADIO_CH6] <= 75) {
        QEvt evt = { .sig = CHANGE_STRATEGY_EVT_SIG };
        QHSM_DISPATCH(&AO_SumoHSM->super, &evt, SIMULATOR);
    }

    if (radio_data[RADIO_CH7] > 75 && last_radio_data[RADIO_CH7] <= 75) {
        QEvt evt = { .sig = CHANGE_PRE_STRATEGY_EVT_SIG };
        QHSM_DISPATCH(&AO_SumoHSM->super, &evt, SIMULATOR);
    }
}

static void radio_data_interrupt_uart(uint16_t *ch_data, uint8_t ch_amount)
{
    for (uint8_t i = 0; i < ch_amount; i++) {
        if (i >= NUM_OF_RADIO_CHANNELS) {
            break;
        }

#if defined(RADIO_MODE_UART)
        uint16_t ch_val = constrain(*(ch_data + i), RX_RADIO_MIN_VALUE, RX_RADIO_MAX_VALUE);
        radio_data[i] = map(ch_val, RX_RADIO_MIN_VALUE, RX_RADIO_MAX_VALUE, CHANNEL_MIN_VAL, CHANNEL_MAX_VAL);
#elif defined(RADIO_MODE_UART_CRSF)
        uint16_t ch_val = constrain(*(ch_data + i), RADIO_CRSF_MIN_VALUE, RADIO_CRSF_MAX_VALUE);
        radio_data[i] = map(ch_val, RADIO_CRSF_MIN_VALUE, RADIO_CRSF_MAX_VALUE, CHANNEL_MIN_VAL, CHANNEL_MAX_VAL);
#endif
    }

    radio_dispatch_events();

    for (uint8_t i = 0; i < NUM_OF_RADIO_CHANNELS; i++) {
        last_radio_data[i] = radio_data[i];
    }
}


/***************************************************************************************************
 * GLOBAL FUNCTIONS
 **************************************************************************************************/

void radio_service_init()
{
#if defined(RADIO_MODE_UART)
    bsp_uart_radio_init();
    bsp_uart_radio_register_callback(radio_data_interrupt_uart);
    bsp_uart_radio_start();
#elif defined(RADIO_MODE_UART_CRSF)
    bsp_uart_crsf_init();
    bsp_uart_crsf_register_callback(radio_data_interrupt_uart);
    bsp_uart_crsf_start();
#endif
}

void radio_service_enable()
{
#if defined(RADIO_MODE_UART)
    bsp_uart_radio_start();
#elif defined(RADIO_MODE_UART_CRSF)
    bsp_uart_crsf_start();
#endif
}

void radio_service_disable()
{
#if defined(RADIO_MODE_UART)
    bsp_uart_radio_stop();
#elif defined(RADIO_MODE_UART_CRSF)
    bsp_uart_crsf_stop();
#endif
}

int8_t radio_service_get_channel(radio_channel_t ch)
{
    if (ch > NUM_OF_RADIO_CHANNELS) {
        return 0;
    }

    return radio_data[ch];
}

void radio_service_en_radio_data_sig(bool enabled)
{
    radio_data_sig_enable = enabled;
}