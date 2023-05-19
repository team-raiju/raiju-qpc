#ifndef BSP_TIMER_INPUT_H
#define BSP_TIMER_INPUT_H

typedef void (*bsp_tim_capture_callback)(uint16_t time_diff);

typedef enum {
    BSP_TIM_RISING_EDGE,
    BSP_TIM_FALLING_EDGE
} bsp_tim_capture_edge_t;

void bsp_tim_input_capture_init();
void bsp_tim_input_capture_start();
void bsp_tim_input_capture_stop();
void bsp_tim_capture_register_callback(bsp_tim_capture_callback callback_function);
void bsp_tim_set_capture_level(bsp_tim_capture_edge_t edge);

#endif /* BSP_TIMER_INPUT_H */
