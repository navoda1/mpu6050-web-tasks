#include "MPU6050.h"

MPU6050::MPU6050(uint8_t i2c_address) {
    this->i2c_address = i2c_address;
    Wire.begin();

    // Wake up the device (without this sequence, it sends all zeros)
    Wire.beginTransmission(i2c_address);
    Wire.write(this->POWER_MGT_1_ADDR);
    Wire.write(0x00);
    Wire.endTransmission();
}

void MPU6050::get_accel_reading(int16_t *reading) {
    uint8_t error;

    // Set the read address for accelerator
    Wire.beginTransmission(this->i2c_address);
    Wire.write(this->ACCEL_ADDR);
    error = Wire.endTransmission(false);

    if (error == 0) {
        // Request the 3 address values for X,Y,Z axes of the accelerometer
        Wire.requestFrom(this->i2c_address, 3*2, true); 
        reading[0] = (Wire.read() << 8) | Wire.read();
        reading[1] = (Wire.read() << 8) | Wire.read();
        reading[2] = (Wire.read() << 8) | Wire.read();
    }
    else {
        Serial.println("ERROR: Could not access accelerometer readings");
    }
}