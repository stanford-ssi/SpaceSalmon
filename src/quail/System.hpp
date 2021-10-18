#pragma once

class System;

#include <Arduino.h>

#include "SPI.h"

#include "ADCTask.hpp"
#include "FireTask.hpp"

class System
{ 
public:

    SPIClass adc_spi = SPIClass(&sercom1, 5, 6, 7, SPI_PAD_0_SCK_1, SERCOM_RX_PAD_3);
    SPIClass squib_spi = SPIClass(&sercom0, 17, 18, 19, SPI_PAD_0_SCK_1, SERCOM_RX_PAD_3);
    
    class Tasks
    {
    public:
        //ADCTask adc = ADCTask(2); //reads from ADC
        FireTask fire = FireTask(3); //fires MC33797
    };

    Tasks tasks;
};

#include "main.hpp"