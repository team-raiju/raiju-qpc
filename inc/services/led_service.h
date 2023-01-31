#ifndef LED_SERVICE_H
#define LED_SERVICE_H

#include <stdint.h>

#define LED_STRIPE_NUM 16

typedef struct color {
    uint8_t R;
    uint8_t G;
    uint8_t B;
} color_rgb_t;

typedef enum {
    COLOR_RED,
    COLOR_GREEN,
    COLOR_BLUE,
    COLOR_PURPLE,
    COLOR_YELLOW,
    COLOR_ORANGE,
    COLOR_PINK,
    COLOR_WHITE,
    COLOR_BLACK,
} color_name_t;

void board_led_toggle();
void board_led_on();
void board_led_off();

void led_stripe_init();
void led_stripe_send();

void led_stripe_prepare_range_color(uint8_t from, uint8_t to, color_name_t color_name);
void led_stripe_set(uint8_t idx, color_rgb_t color);
void led_stripe_set_range(uint8_t from, uint8_t to, color_rgb_t color);
void led_stripe_set_all(color_rgb_t color);

void led_stripe_set_color(uint8_t idx, color_name_t color_name);
void led_stripe_set_range_color(uint8_t from, uint8_t to, color_name_t color_name);
void led_stripe_set_all_color(color_name_t color_name);

void led_stripe_reset();
void led_stripe_send();

#endif /* LED_SERVICE_H */
