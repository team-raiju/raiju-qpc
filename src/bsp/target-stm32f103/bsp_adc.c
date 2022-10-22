

#include <stdbool.h>
#include <stdint.h>

#include "adc.h"
#include "dma.h"
#include "utils.h"

extern ADC_HandleTypeDef hadc1;
extern DMA_HandleTypeDef hdma_adc1;

bool reading_done;

static const size_t amount = 7;
static const size_t readings_per_adc = 256;


uint32_t dma_buffer[amount * readings_per_adc];
uint32_t readings[amount];
uint32_t aux_readings[amount];


static void ADC_StartReading() {
    reading_done = false;
    HAL_ADC_Start_DMA(&hadc1, dma_buffer, amount * readings_per_adc);
}



void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef* hadc) {
    if (hadc->Instance == &hadc1.Instance) {
        HAL_ADC_Stop_DMA(&hadc1);
        reading_done = true;
    }
}



void BSP_ADC_CalcAndRestart() {
    if (!reading_done) {
        return;
    }

    memset(aux_readings, 0, amount * sizeof(uint32_t));

    for (size_t i = 0; i < (amount * readings_per_adc); i += amount) {
        for (size_t j = 0; j < amount; j++) {
            aux_readings[j] += dma_buffer[i + j];
        }
    }

    for (size_t j = 0; j < amount; j++) {
        readings[j] = aux_readings[j] / ((amount * readings_per_adc) / amount);
    }

    ADC_StartReading();
}


uint32_t BSP_ADC_GetReadings(size_t idx)  {
    idx = min(idx, amount - 1);
    return readings[idx];
}

