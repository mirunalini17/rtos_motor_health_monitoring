
#ifndef MPU6050_H
#define MPU6050_H

#include "main.h"
#include <stdint.h>

#define MPU6050_ADDR          (0x68 << 1)

#define MPU6050_WHO_AM_I      0x75
#define MPU6050_PWR_MGMT_1    0x6B
#define MPU6050_ACCEL_CONFIG  0x1C
#define MPU6050_ACCEL_XOUT_H  0x3B

typedef struct
{
    int16_t accel_x;
    int16_t accel_y;
    int16_t accel_z;

    float ax;
    float ay;
    float az;

} MPU6050_Data;

HAL_StatusTypeDef MPU6050_Init(I2C_HandleTypeDef *hi2c);

HAL_StatusTypeDef MPU6050_Read(
    I2C_HandleTypeDef *hi2c,
    MPU6050_Data *data
);

#endif
