/***************************************************************************************************
 * INCLUDES
 **************************************************************************************************/

#include "qpc.h"
#include "bsp.h"

#include "adc_service.h"
#include "bsp_adc_dma.h"

/***************************************************************************************************
 * LOCAL DEFINES
 **************************************************************************************************/
#define ADC_MAX_VALUE      4095.0
#define ADC_MAX_VOLTAGE_MV 3300

#define WHITE_THRESHOLD 1000

/* Vcc ---- R1 --- R2 --- GND*/
#define CTRL_BATTERY_THRESHOLD_MV   14800.0
#define CTRL_BAT_VOLTAGE_DIV_R1     47.0
#define CTRL_BAT_VOLTAGE_DIV_R2     10.0
#define CTRL_BAT_VOLTAGE_MULTIPLIER ((CTRL_BAT_VOLTAGE_DIV_R1 + CTRL_BAT_VOLTAGE_DIV_R2) / CTRL_BAT_VOLTAGE_DIV_R2)
#define CTRL_BAT_POSITION_IN_ADC    0

#define PWR_BATTERY_THRESHOLD_MV   38000.0
#define PWR_BAT_VOLTAGE_DIV_R1     330.0
#define PWR_BAT_VOLTAGE_DIV_R2     23.5
#define PWR_BAT_VOLTAGE_MULTIPLIER ((PWR_BAT_VOLTAGE_DIV_R1 + PWR_BAT_VOLTAGE_DIV_R2) / PWR_BAT_VOLTAGE_DIV_R2)
#define PWR_BAT_POSITION_IN_ADC    6

#define START_MODULE_LOW             250
#define START_MODULE_HIGH            3000
#define START_MODULE_POSITION_IN_ADC 4

/***************************************************************************************************
 * LOCAL TYPEDEFS
 **************************************************************************************************/

typedef enum line_position_in_adc {
    LINE_POS_FR = 3,
    LINE_POS_FL = 2,
    LINE_POS_BR = 5,
    LINE_POS_BL = 1,
} line_position_in_adc_t;

/***************************************************************************************************
 * LOCAL FUNCTION PROTOTYPES
 **************************************************************************************************/
static void ctrl_battery_value_update(uint16_t bat_raw_adc);
static void adc_data_interrupt(uint32_t *out_data);
static void gen_line_events(void);
static void gen_ctrl_battery_events(void);
static void gen_pwr_battery_events(void);
static void start_module_value_update(uint16_t start_module_raw);
static void gen_start_module_events(void);

/***************************************************************************************************
 * LOCAL VARIABLES
 **************************************************************************************************/

static volatile bool line_sensor_is_white[NUM_OF_LINE_SENSORS];
static volatile bool line_sensor_is_white_last[NUM_OF_LINE_SENSORS];
static volatile double ctrl_bat_voltage_mv = 16000;
static volatile double ctrl_bat_voltage_mv_last = 16000;
static volatile double pwr_bat_voltage_mv = 42000;
static volatile double pwr_bat_voltage_mv_last = 42000;
static volatile uint16_t start_module_adc = 0;
static volatile uint16_t start_module_adc_last = 0;

static uint8_t line_sensor_mask = 0xff;

static volatile uint32_t last_adc_raw[NUM_OF_LINE_SENSORS];

/***************************************************************************************************
 * GLOBAL VARIABLES
 **************************************************************************************************/

/***************************************************************************************************
 * LOCAL FUNCTIONS
 **************************************************************************************************/

static void start_module_value_update(uint16_t start_module_raw)
{
    start_module_adc = start_module_raw;
    gen_start_module_events();
    start_module_adc_last = start_module_adc;
}

static void ctrl_battery_value_update(uint16_t bat_raw_adc)
{
    double measured_voltage = (bat_raw_adc / ADC_MAX_VALUE) * ADC_MAX_VOLTAGE_MV;

    ctrl_bat_voltage_mv = (measured_voltage * CTRL_BAT_VOLTAGE_MULTIPLIER);

    gen_ctrl_battery_events();

    ctrl_bat_voltage_mv_last = ctrl_bat_voltage_mv;
}

static void pwr_battery_value_update(uint16_t bat_raw_adc)
{
    double measured_voltage = (bat_raw_adc / ADC_MAX_VALUE) * ADC_MAX_VOLTAGE_MV;

    pwr_bat_voltage_mv = (measured_voltage * PWR_BAT_VOLTAGE_MULTIPLIER);

    gen_pwr_battery_events();

    pwr_bat_voltage_mv_last = pwr_bat_voltage_mv;
}

static void gen_ctrl_battery_events()
{
    if (ctrl_bat_voltage_mv_last > CTRL_BATTERY_THRESHOLD_MV && ctrl_bat_voltage_mv <= CTRL_BATTERY_THRESHOLD_MV) {
        QEvt evt = { .sig = LOW_BATTERY_SIG };
        QHSM_DISPATCH(&AO_SumoHSM->super, &evt, SIMULATOR);
    }
}

static void gen_pwr_battery_events()
{
    if (pwr_bat_voltage_mv_last > PWR_BATTERY_THRESHOLD_MV && pwr_bat_voltage_mv <= PWR_BATTERY_THRESHOLD_MV) {
        QEvt evt = { .sig = LOW_BATTERY_SIG };
        QHSM_DISPATCH(&AO_SumoHSM->super, &evt, SIMULATOR);
    }
}

static void gen_start_module_events()
{
    if (start_module_adc >= START_MODULE_HIGH && start_module_adc_last < START_MODULE_HIGH) {
        QEvt evt = { .sig = START_SIG };
        QHSM_DISPATCH(&AO_SumoHSM->super, &evt, SIMULATOR);
    } else if (start_module_adc <= START_MODULE_LOW && start_module_adc_last > START_MODULE_LOW) {
        QEvt evt = { .sig = STOP_SIG };
        QHSM_DISPATCH(&AO_SumoHSM->super, &evt, SIMULATOR);
    }
}

static void gen_line_events(void)
{
    bool line_changed[NUM_OF_LINE_SENSORS] = { 0 };
    for (int i = 0; i < NUM_OF_LINE_SENSORS; i++) {
        if (line_sensor_is_white_last[i] != line_sensor_is_white[i]) {
            line_changed[i] = true;
        }
    }

    bool sensor_enable = line_sensor_mask & (1 << LINE_FR);
    if (line_changed[LINE_FR] && sensor_enable) {
        QEvt evt = { .sig = LINE_CHANGED_FR_SIG };
        QHSM_DISPATCH(&AO_SumoHSM->super, &evt, SIMULATOR);
    }

    sensor_enable = line_sensor_mask & (1 << LINE_FL);
    if (line_changed[LINE_FL] && sensor_enable) {
        QEvt evt = { .sig = LINE_CHANGED_FL_SIG };
        QHSM_DISPATCH(&AO_SumoHSM->super, &evt, SIMULATOR);
    }

    sensor_enable = line_sensor_mask & (1 << LINE_BR);
    if (line_changed[LINE_BR] && sensor_enable) {
        QEvt evt = { .sig = LINE_CHANGED_BR_SIG };
        QHSM_DISPATCH(&AO_SumoHSM->super, &evt, SIMULATOR);
    }

    sensor_enable = line_sensor_mask & (1 << LINE_BL);
    if (line_changed[LINE_BL] && sensor_enable) {
        QEvt evt = { .sig = LINE_CHANGED_BL_SIG };
        QHSM_DISPATCH(&AO_SumoHSM->super, &evt, SIMULATOR);
    }
}

static void line_sensor_update(uint32_t *adc_raw_data)
{
    last_adc_raw[LINE_FR] = adc_raw_data[LINE_POS_FR];
    last_adc_raw[LINE_FL] = adc_raw_data[LINE_POS_FL];
    last_adc_raw[LINE_BR] = adc_raw_data[LINE_POS_BR];
    last_adc_raw[LINE_BL] = adc_raw_data[LINE_POS_BL];

    line_sensor_is_white[LINE_FR] = (last_adc_raw[LINE_FR] < WHITE_THRESHOLD);
    line_sensor_is_white[LINE_FL] = (last_adc_raw[LINE_FL] < WHITE_THRESHOLD);
    line_sensor_is_white[LINE_BR] = (last_adc_raw[LINE_BR] < WHITE_THRESHOLD);
    line_sensor_is_white[LINE_BL] = (last_adc_raw[LINE_BL] < WHITE_THRESHOLD);

    gen_line_events();

    for (int i = 0; i < NUM_OF_LINE_SENSORS; i++) {
        line_sensor_is_white_last[i] = line_sensor_is_white[i];
    }
}

static void adc_data_interrupt(uint32_t *out_data)
{
    uint32_t aux_readings[ADC_DMA_CHANNELS] = { 0 };

    for (uint16_t i = 0; i < (ADC_DMA_HALF_BUFFER_SIZE - 1); i += ADC_DMA_CHANNELS) {
        for (uint16_t j = 0; j < ADC_DMA_CHANNELS; j++) {
            aux_readings[j] += out_data[i + j];
        }
    }

    for (uint16_t j = 0; j < ADC_DMA_CHANNELS; j++) {
        aux_readings[j] /= (ADC_DMA_HALF_BUFFER_SIZE / ADC_DMA_CHANNELS);
    }

    line_sensor_update(aux_readings);

    start_module_value_update(aux_readings[START_MODULE_POSITION_IN_ADC]);
    ctrl_battery_value_update(aux_readings[CTRL_BAT_POSITION_IN_ADC]);
    pwr_battery_value_update(aux_readings[PWR_BAT_POSITION_IN_ADC]);
}

/***************************************************************************************************
 * GLOBAL FUNCTIONS
 **************************************************************************************************/

void adc_service_init()
{
    BSP_ADC_DMA_Init();
    BSP_ADC_DMA_Start();
}

void adc_service_start_callback()
{
    BSP_ADC_DMA_Register_Callback(adc_data_interrupt);
}

bool adc_line_is_white(line_sensor_t position)
{
    if (position > NUM_OF_LINE_SENSORS) {
        return 0;
    }

    bool line_sensor_enable = line_sensor_mask & (1 << position);

    return (line_sensor_is_white[position] & line_sensor_enable);
}

uint16_t adc_line_get_all()
{
    uint16_t mask = 0;
    for (int i = 0; i < NUM_OF_LINE_SENSORS; i++) {
        if (adc_line_is_white(i)) {
            mask |= (1 << i);
        }
    }

    return mask;
}

bool adc_line_none_white()
{
    return !adc_line_is_white(LINE_FL) && !adc_line_is_white(LINE_FR) && !adc_line_is_white(LINE_BL) &&
           !adc_line_is_white(LINE_BR);
}

bool adc_get_low_ctrl_bat()
{
    return (ctrl_bat_voltage_mv <= CTRL_BATTERY_THRESHOLD_MV);
}

uint16_t adc_get_ctrl_bat_mv()
{
    return (uint16_t)ctrl_bat_voltage_mv;
}

bool adc_get_low_pwr_bat()
{
    return (pwr_bat_voltage_mv <= PWR_BATTERY_THRESHOLD_MV);
}

uint16_t adc_get_pwr_bat_mv()
{
    return (uint16_t)pwr_bat_voltage_mv;
}

void adc_line_set_mask(uint8_t mask)
{
    line_sensor_mask = mask;
}

uint32_t adc_get_raw_line(line_sensor_t position)
{
    if (position > NUM_OF_LINE_SENSORS) {
        return 0;
    }

    return last_adc_raw[position];
}