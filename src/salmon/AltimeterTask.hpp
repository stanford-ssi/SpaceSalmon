#pragma once

#include <FreeRTOS.h>
#include <task.h>
#include <semphr.h>
#include <hal_rtos.h>
#include "Poster.hpp"
#include "./periph/OneBattery/OneBattery.hpp"
#include "Task.hpp"

#define ALT_LED 1

class AltimeterTask : Task<2000>
{
private:
    void activity();

public:
    AltimeterTask(uint8_t priority);
    Poster<OneBattery::cell_voltage_t> battData;
};

#include "periph/ADXL375/ADXL375.hpp"
#include "periph/BMP388/BMP388.hpp"
#include "periph/BMI088/BMI088.hpp"
#include "main.hpp"
#include "ArduinoJson.h"
#include "periph/OneBattery/OneBattery.hpp"