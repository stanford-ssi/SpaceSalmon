#pragma once

#include "periph/ADXL375/ADXL375.hpp"
#include "periph/BMP388/BMP388.hpp"
#include "periph/BMI088/BMI088.hpp"

struct SensorData
{
    ADXL375::Data adxl1_data;
    ADXL375::Data adxl2_data;
    BMP388::Data bmp1_data;
    BMP388::Data bmp2_data;
    BMI088Gyro::Data bmigyro1_data;
    BMI088Gyro::Data bmigyro2_data;
    BMI088Accel::Data bmiaccel1_data;
    BMI088Accel::Data bmiaccel2_data;
    uint32_t tick;
};