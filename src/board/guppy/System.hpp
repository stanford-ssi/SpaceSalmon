#pragma once

class System;

#include "../../support/atmel_start.h"
#include "../../periph/ADXL375/ADXL375.hpp"
#include "../../periph/BMI088/BMI088.hpp"
#include "../../periph/BMP388/BMP388.hpp"

#include "../../periph/PyroFets/PyroFets.h"
#include "../../periph/Pyro.h"

#include "Tone.h"

#include "SensorTask.hpp"
#include "LoggerTask.hpp"
#include "AltimeterTask.hpp"
#include "AltFilterTask.h"

#include "ssi_adc.h"

class System
{
public:

    PyroFets pyrofets = PyroFets(GPIO(GPIO_PORTA,21), GPIO(GPIO_PORTB,1), GPIO(GPIO_PORTA,23), GPIO(GPIO_PORTB,0));
    Pyro &pyro = pyrofets;

    ADC adc0 = ADC(ADC0);

    Tone buzzer = Tone(BUZZER);

    const bool shitl = false;

    const bool silent = true;

    class Sensors
    {
    public:
        ADXL375 adxl1 = ADXL375(&SPI_SENSOR, GPIO(GPIO_PORTB, 6), "adxl1");
        ADXL375 adxl2 = ADXL375(&SPI_SENSOR, GPIO(GPIO_PORTA, 2), "adxl2");
        BMI088 imu1 = BMI088(&SPI_SENSOR, GPIO(GPIO_PORTB, 9), GPIO(GPIO_PORTB, 8), "imu1");
        BMI088 imu2 = BMI088(&SPI_SENSOR, GPIO(GPIO_PORTB, 5), GPIO(GPIO_PORTB, 4), "imu2");
        BMP388 pres1 = BMP388(&SPI_SENSOR, GPIO(GPIO_PORTB, 7), "pres1");
        BMP388 pres2 = BMP388(&SPI_SENSOR, GPIO(GPIO_PORTA, 3), "pres2");
        Sensor *list[6] = {&imu1, &imu2, &adxl1, &adxl2, &pres1, &pres2};
    };

    class Tasks
    {
    public:
        SensorTask sensor; //reads data from sensors
        LoggerTask logger; //logs to USB/SD
        AltFilterTask filter; //handles data processing, chute events
        AltimeterTask altimeter; //handles system monitoring
    };

    Sensors sensors;
    Tasks tasks;
};

#include "main.hpp"


//LED 1 : Altimeter
//LED 2 : Sensor
//LED 3 : Logger
//LED 4 : Filter