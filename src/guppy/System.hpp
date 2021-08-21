#pragma once

class System;

#include <Arduino.h>

#include "../periph/ADXL375/ADXL375.hpp"
#include "../periph/BMI088/BMI088.hpp"
#include "../periph/BMP388/BMP388.hpp"

#include "SPI.h"
#include "Tone.h"

#include "../periph/PyroFets/PyroFets.h"

#include "fc/SensorTask.hpp"
#include "fc/LoggerTask.hpp"
#include "fc/AltimeterTask.hpp"
#include "fc/AltFilterTask.h"
#include "fc/BuzzerTask.hpp"
#include "GPSTask.hpp"
#include "TelemetryTask.hpp"
#include "RadioTask.hpp"
#include "ArmingTask.hpp"

#include "ssi_adc.h"

class System
{ 
public:

    PyroFets pyrofets = PyroFets(31,33,32,34,adc0);
    Pyro &pyro = pyrofets;

    ADC adc0 = ADC(ADC0);

    Tone buzzer = Tone(5);

    const bool shitl = false;

    const bool silent = false;

    Poster<bool> armed = Poster<bool>(false);

    class Sensors
    {
    public:
        SPIClass spi = SPIClass(&sercom0, 8, 9, 10, SPI_PAD_3_SCK_1, SERCOM_RX_PAD_0);

        ADXL375 adxl1 = ADXL375(&spi, 11, "adxl1");
        ADXL375 adxl2 = ADXL375(&spi, 15, "adxl2");
        BMI088 imu1 = BMI088(&spi, 14, 13, "imu1");
        BMI088 imu2 = BMI088(&spi, 18, 17, "imu2");
        BMP388 pres1 = BMP388(&spi, 12, "pres1");
        BMP388 pres2 = BMP388(&spi, 16, "pres2");

        Sensor *list[6] = {&imu1, &imu2, &adxl1, &adxl2, &pres1, &pres2};
    };

    class Tasks
    {
    public:
        SensorTask sensor = SensorTask(5); //reads data from sensors
        LoggerTask logger = LoggerTask(1); //logs to USB/SD
        AltimeterTask alt = AltimeterTask(4); //monitors system health
        AltFilterTask filter = AltFilterTask(6); //KF for altitude estimations
        BuzzerTask buzz = BuzzerTask(2); //buzzes!
        GPSTask gps = GPSTask(3);
        TelemetryTask telem = TelemetryTask(3);
        RadioTask radio = RadioTask(3);
        ArmingTask arm = ArmingTask(3);
    };

    Sensors sensors;
    Tasks tasks;
};


#include "main.hpp"


//LED 1 : Altimeter
//LED 2 : Sensor
//LED 3 : Logger
//LED 4 : Filter