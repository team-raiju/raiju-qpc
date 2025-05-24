#ifndef PID_H
#define PID_H

typedef struct {
    float kp;
    float ki;
    float kd;
    float integral_limit;

    float integral;
    float previous_error;
} PID_controller;

void PID_init(PID_controller *pid, float kp, float ki, float kd, float integral_limit);

float PID_calculate(PID_controller *pid, float target, float measured_value);

float PID_get_integral(PID_controller *pid);

void PID_reset(PID_controller *pid);

#endif // ALGORITHM_PID_H