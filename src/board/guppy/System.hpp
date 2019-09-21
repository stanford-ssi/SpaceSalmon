#pragma once

#include "support/atmel_start.h"
#include "periph/ADXL375/ADXL375.hpp"
#include "periph/BMI088/BMI088.hpp"
#include "periph/BMP388/BMP388.hpp"
#include "SensorTask.hpp"
#include "SquibTask.hpp"
#include "LoggerTask.hpp"
#include "AltimeterTask.hpp"

class System
{
public:
    class Sensors
    {
        const ADXL375 imu1 = ADXL375(&SPI_SENSOR, 10, "imu1");
        const ADXL375 imu2 = ADXL375(&SPI_SENSOR, 11, "imu2");
        const BMI088 imu3 = BMI088(&SPI_SENSOR, 12, 13, "imu3");
        const BMI088 imu4 = BMI088(&SPI_SENSOR, 14, 15, "imu4");
        const BMP388 pres1 = BMP388(&SPI_SENSOR, 16, "pres1");
        const BMP388 pres2 = BMP388(&SPI_SENSOR, 17, "pres2");
        const Sensor *list[6] = {&imu1, &imu2, &imu3, &imu4, &pres1, &pres2};
    };
    class Tasks
    {
        SensorTask sensor;
        LoggerTask logger;
        SquibTask squib; //this should not be a task...
        AltimeterTask altimeter;
    };
};