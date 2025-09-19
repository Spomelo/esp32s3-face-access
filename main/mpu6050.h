#pragma once
#include "esp_err.h"
#ifdef __cplusplus
extern "C"
{
#endif

    esp_err_t mpu6050_init(void);
    esp_err_t mpu6050_read_accel_g(float *ax, float *ay, float *az);
    void mpu6050_compute_tilt_deg(float ax, float ay, float az, float *pitch_deg, float *roll_deg);

#ifdef __cplusplus
}
#endif