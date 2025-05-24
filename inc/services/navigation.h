#ifndef NAVIGATION_H
#define NAVIGATION_H


typedef enum {
    FORWARD,
    BACKWARD,

    TURN_RIGHT_45_STATIC,
    TURN_LEFT_45_STATIC,
    TURN_RIGHT_90_STATIC,
    TURN_LEFT_90_STATIC,

    TURN_RIGHT_45_MOVING,
    TURN_LEFT_45_MOVING,
    TURN_RIGHT_90_MOVING,
    TURN_LEFT_90_MOVING,

    STOP,
} movement_type_t;

typedef struct strategy_movements {
    movement_type_t movement;
    uint8_t lenght_cm;
} strategy_movements_t;

void control_reset();
void control_update();
void navigation_reset(strategy_movements_t strategy_movement);
bool navigation_step();


#endif /* NAVIGATION_H */
