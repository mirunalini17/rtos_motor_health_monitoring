
#include "mpu6050.h"

#define MPU6050_TIMEOUT 100

HAL_StatusTypeDef MPU6050_Init(I2C_HandleTypeDef *hi2c)
{
    uint8_t check;
    uint8_t data;

    // Check device identity
    if (HAL_I2C_Mem_Read(
            hi2c,
            MPU6050_ADDR,
            MPU6050_WHO_AM_I,
            I2C_MEMADD_SIZE_8BIT,
            &check,
            1,
            MPU6050_TIMEOUT) != HAL_OK)
    {
        return HAL_ERROR;
    }

    if (check != 0x68)
    {
        return HAL_ERROR;
    }

    // Wake up MPU6050
    data = 0x00;

    if (HAL_I2C_Mem_Write(
            hi2c,
            MPU6050_ADDR,
            MPU6050_PWR_MGMT_1,
            I2C_MEMADD_SIZE_8BIT,
            &data,
            1,
            MPU6050_TIMEOUT) != HAL_OK)
    {
        return HAL_ERROR;
    }

    HAL_Delay(100);

    // Set accelerometer range to +/- 2g
    data = 0x00;

    if (HAL_I2C_Mem_Write(
            hi2c,
            MPU6050_ADDR,
            MPU6050_ACCEL_CONFIG,
            I2C_MEMADD_SIZE_8BIT,
            &data,
            1,
            MPU6050_TIMEOUT) != HAL_OK)
    {
        return HAL_ERROR;
    }

    return HAL_OK;
}


HAL_StatusTypeDef MPU6050_Read(
    I2C_HandleTypeDef *hi2c,
    MPU6050_Data *data)
{
    uint8_t raw[6];

    // Read six accelerometer registers individually
    for (uint8_t i = 0; i < 6; i++)
    {
        if (HAL_I2C_Mem_Read(
                hi2c,
                MPU6050_ADDR,
                MPU6050_ACCEL_XOUT_H + i,
                I2C_MEMADD_SIZE_8BIT,
                &raw[i],
                1,
                MPU6050_TIMEOUT) != HAL_OK)
        {
            return HAL_ERROR;
        }
    }

    // Combine high and low bytes
    data->accel_x =
        (int16_t)((raw[0] << 8) | raw[1]);

    data->accel_y =
        (int16_t)((raw[2] << 8) | raw[3]);

    data->accel_z =
        (int16_t)((raw[4] << 8) | raw[5]);

    // Convert raw values into acceleration (g)
    // Sensitivity at +/- 2g = 16384 LSB/g
    data->ax = data->accel_x / 16384.0f;
    data->ay = data->accel_y / 16384.0f;
    data->az = data->accel_z / 16384.0f;

    return HAL_OK;
}
