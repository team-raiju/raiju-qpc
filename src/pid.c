#include "pid.h"
#include "utils.h"

void PID_init(PID_controller *pid, float kp, float ki, float kd, float integral_limit)
{
    if (!pid) {
        return;
    }
    pid->kp = kp;
    pid->ki = ki;
    pid->kd = kd;
    pid->integral_limit = integral_limit;
    pid->integral = 0.0f;
    pid->previous_error = 0.0f;
}

void PID_reset(PID_controller *pid)
{
    if (!pid) {
        return;
    }
    pid->integral = 0.0f;
    pid->previous_error = 0.0f;
}

float PID_calculate(PID_controller *pid, float target, float measured_value)
{
    if (!pid) {
        return 0.0f;
    }

    float error = target - measured_value;

    pid->integral += error;

    pid->integral = constrain(pid->integral, -pid->integral_limit, pid->integral_limit);

    float derivative = error - pid->previous_error;
    pid->previous_error = error;

    return pid->kp * error + pid->ki * pid->integral + pid->kd * derivative;
}

float PID_get_integral(PID_controller *pid)
{
    if (!pid) {
        return 0.0f;
    }
    return pid->integral;
}