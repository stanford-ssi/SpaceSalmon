#pragma once

class System;

#include <Arduino.h>

#include "../periph/ADXL375/ADXL375.hpp"
#include "../periph/BMI088/BMI088.hpp"
#include "../periph/BMP388/BMP388.hpp"

#include "SPI.h"
#include "Tone.h"

#include "LEDTask.hpp"

#include "ssi_adc.h"

class System
{ 
public:

    // const bool shitl = false;

    // const bool silent = true;

    // Poster<bool> armed = Poster<bool>(false);

    
    class Tasks
    {
    public:
        LEDTask buzz = LEDTask(2); //buzzes!
    };

    Tasks tasks;
};

#include "main.hpp"