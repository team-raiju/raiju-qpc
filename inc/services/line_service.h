#ifndef LINE_SERVICE_H
#define LINE_SERVICE_H

#include <stdint.h>
#include <stdbool.h>

typedef enum line_position {
    FR1 = 3,     // ou 4,
    BR = 5,      // ou 6,
    FL1 = 2,
    BL = 1,
    FR2 = 4,
    FL2 = 6,
} line_position_t;

void line_start();
bool line_is_white(line_position_t position);
uint16_t line_raw_reading(line_position_t pos);

#endif /* LINE_SERVICE_H */
