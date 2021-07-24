#ifndef __MPU6050_H__
#define __MPU6050_H__

#include <Arduino.h>
#include <Wire.h>

class MPU6050 {

    public:
        uint8_t i2c_address;

        MPU6050(uint8_t i2c_address);
        void get_accel_reading(int16_t *reading);

    private:
        static const uint8_t ACCEL_ADDR         = 0x3b;
        static const uint8_t TEMP_ADDR          = 0x41;
        static const uint8_t GYRO_ADDR          = 0x43;
        static const uint8_t POWER_MGT_1_ADDR   = 0x6b;
};

#endif /* __MPU6050_H__ */