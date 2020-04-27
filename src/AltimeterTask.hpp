#pragma once

#include <FreeRTOS.h>
#include <task.h>
#include <semphr.h>
#include <hal_rtos.h>
#include "Poster.hpp"
#include "./periph/OneBattery/OneBattery.hpp"

#define ALT_LED 1

class AltimeterTask
{
private:
    static const size_t stackSize = 2000;

    static TaskHandle_t taskHandle;
    static StaticTask_t xTaskBuffer;
    static StackType_t xStack[stackSize];

    static void activity(void *p);

public:
    AltimeterTask(uint8_t priority);
    TaskHandle_t getTaskHandle();
    Poster<OneBattery::cell_voltage_t> battData;
};

#include "periph/ADXL375/ADXL375.hpp"
#include "periph/BMP388/BMP388.hpp"
#include "periph/BMI088/BMI088.hpp"
#include "main.hpp"
#include "ArduinoJson.h"
#include "periph/OneBattery/OneBattery.hpp"