#ifndef PRE_STRATEGIES_H
#define PRE_STRATEGIES_H

#include "navigation.h"

typedef enum {
    PRE_STRATEGY_45_LEFT,
    PRE_STRATEGY_45_RIGHT,
} pre_strategy_t;


strategy_movements_t pre_strategy_45_left[] = {
    {TURN_LEFT_45_STATIC, 1},
    {FORWARD, 45},
    {TURN_RIGHT_45_STATIC,1}
};

strategy_movements_t pre_strategy_45_right[] = {
    {TURN_RIGHT_45_STATIC, 1},
    {FORWARD, 45},
    {TURN_LEFT_45_STATIC,1}
};

strategy_movements_t* pre_strategies[] = {
    [PRE_STRATEGY_45_LEFT] = pre_strategy_45_left,
    [PRE_STRATEGY_45_RIGHT] = pre_strategy_45_right,
};


uint8_t pre_strategy_lenghts[] = {
    [PRE_STRATEGY_45_LEFT] = sizeof(pre_strategy_45_left) / sizeof(strategy_movements_t),
    [PRE_STRATEGY_45_RIGHT] = sizeof(pre_strategy_45_right) / sizeof(strategy_movements_t),
};

#endif /* PRE_STRATEGIES_H */
