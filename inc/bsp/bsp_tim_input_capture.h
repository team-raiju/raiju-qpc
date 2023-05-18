#ifndef BSP_TIMER_INPUT_H
#define BSP_TIMER_INPUT_H

typedef void (*bsp_tim_capture_callback)(uint16_t time_diff);

void bsp_tim_input_capture_init();
void bsp_tim_input_capture_start();
void bsp_tim_input_capture_stop();
void bsp_tim_capture_register_callback(bsp_tim_capture_callback callback_function);


#endif /* BSP_TIMER_INPUT_H */
