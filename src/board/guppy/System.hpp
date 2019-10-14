#pragma once

class System;

#include "support/atmel_start.h"
#include "periph/ADXL375/ADXL375.hpp"
#include "periph/BMI088/BMI088.hpp"
#include "periph/BMP388/BMP388.hpp"
#include "SensorTask.hpp"
#include "SquibTask.hpp"
#include "LoggerTask.hpp"
#include "AltimeterTask.hpp"
#include "main.hpp"

class System
{
public:
    class Sensors
    {
    public:
        ADXL375 adxl1 = ADXL375(&SPI_SENSOR, 10, "adxl1");
        ADXL375 adxl2 = ADXL375(&SPI_SENSOR, 11, "adxl2");
        BMI088 imu1 = BMI088(&SPI_SENSOR, 12, 13, "imu1");
        BMI088 imu2 = BMI088(&SPI_SENSOR, 14, 15, "imu2");
        BMP388 pres1 = BMP388(&SPI_SENSOR, 16, "pres1");
        BMP388 pres2 = BMP388(&SPI_SENSOR, 17, "pres2");
        Sensor *list[6] = {&imu1, &imu2, &adxl1, &adxl2, &pres1, &pres2};
    };
    class Tasks
    {
    public:
        SensorTask sensor;
        LoggerTask logger;
        //SquibTask squib; //this should not be a task...
        //AltimeterTask altimeter;
    };

    Sensors sensors;
    Tasks tasks;
};