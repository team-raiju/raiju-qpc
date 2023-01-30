#ifndef BSP_ADC_DMA_H
#define BSP_ADC_DMA_H

#include <stdint.h>

#define ADC_DMA_CHANNELS    8
#define READINGS_PER_ADC    32


#define ADC_DMA_BUFFER_SIZE             ADC_DMA_CHANNELS * READINGS_PER_ADC
#define ADC_DMA_HALF_BUFFER_SIZE        (ADC_DMA_BUFFER_SIZE / 2)

/* When using 12MHz as ADC clock source */
/* When using 55 cycles per reading */
/* And 7 ADC channels. We have the following adc read time */

/* READINGS_PER_ADC = 32  -> 0.625ms */ /* 5m/s -> 3.1mm */
/* READINGS_PER_ADC = 64  -> 1.250ms */ /* 5m/s -> 6.2mm */
/* READINGS_PER_ADC = 128 -> 2.500ms */ /* 5m/s -> 12.4mm */

/* When using 12MHz as ADC clock source */
/* When using 55 cycles per reading */
/* And 8 ADC channels. We have the following adc read time */

/* READINGS_PER_ADC = 32  -> 0.72ms */ /*5m/s -> 3.6mm */
/* READINGS_PER_ADC = 64  -> 1.44ms */ /*5m/s -> 7.2mm */
/* READINGS_PER_ADC = 128 -> 2.88ms */ /*5m/s -> 14.4mm */

typedef void( * bsp_adc_dma_callback_t) (uint32_t * adc_data);

void BSP_ADC_DMA_Init();
void BSP_ADC_DMA_Start();
void BSP_ADC_DMA_Stop();
void BSP_ADC_DMA_Register_Callback(bsp_adc_dma_callback_t callback_function);

#endif /* BSP_ADC_DMA_H */
