#ifndef IMU_SERVICE_H
#define IMU_SERVICE_H

#include <stdint.h>

void imu_service_init(void);

void imu_reset_pid();

float get_imu_angle_z();

void reset_imu_angle_z();

void reset_inclination();

void start_g_bias_calculation();

void stop_g_bias_calculation();

void imu_set_setpoint(float set_point_);

float imu_get_setpoint();

bool near_set_point();

void imu_set_base_speed(int8_t base_speed_);

int8_t imu_pid_process(int8_t *left_speed, int8_t *right_speed);

bool get_imu_inclination_stat();

void imu_set_kp(uint16_t kp_);

void imu_set_kd(uint16_t kd_);

void imu_set_ki(uint16_t ki_);

void imu_set_near_angle_th(uint8_t near_angle_th_);

void imu_set_inclinated_th_x10(uint8_t inclinated_th_);

void stop_g_bias_calculation();

void imu_set_imu_enabled(uint8_t imu_enabled_);

uint32_t get_imu_z_bias();

uint32_t get_imu_z_bias_signal_negative();

#endif /* IMU_SERVICE_H */
