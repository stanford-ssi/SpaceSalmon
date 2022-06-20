#pragma once

#include "periph/ADXL375/ADXL375.hpp"
#include "periph/BMI088/BMI088.hpp"
#ifdef MS5611
#include "periph/MS5611/MS5611.hpp"
#else
#include "periph/BMP388/BMP388.hpp"
#endif

struct SensorData
{
    ADXL375::Data adxl1_data;
    ADXL375::Data adxl2_data;
#ifdef MS5611
    MS5611_SPI::Data pres1_data;
    MS5611_SPI::Data pres2_data;
#else
    BMP388::Data pres1_data;
    BMP388::Data pres2_data;
#endif
    BMI088Gyro::Data bmigyro1_data;
    BMI088Gyro::Data bmigyro2_data;
    BMI088Accel::Data bmiaccel1_data;
    BMI088Accel::Data bmiaccel2_data;
    uint32_t tick;
};