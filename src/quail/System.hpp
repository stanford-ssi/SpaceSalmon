#pragma once

class System;

#include <Arduino.h>

#include "SPI.h"

#include "LEDTask.hpp"


class System
{ 
public:

    SPIClass adc_spi = SPIClass(&sercom1, 5, 6, 7, SPI_PAD_0_SCK_1, SERCOM_RX_PAD_3);
    
    class Tasks
    {
    public:
        LEDTask buzz = LEDTask(2); //buzzes!
    };

    Tasks tasks;
};

#include "main.hpp"