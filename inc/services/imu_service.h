#ifndef IMU_SERVICE_H
#define IMU_SERVICE_H

#include <stdint.h>
#include "motion_gc.h"

void imu_service_init(void);

float get_imu_angle_z();

float get_imu_ang_vel_rad_s();

void reset_imu_angle_z();

MGC_output_t get_g_bias();

#endif /* IMU_SERVICE_H */
