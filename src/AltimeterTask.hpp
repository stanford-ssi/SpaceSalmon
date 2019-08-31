#pragma once

class AltimeterTask;

#include <FreeRTOS.h>
#include <task.h>
#include <semphr.h>
#include <hal_rtos.h>
#include "support/driver_init.h"
#include "printf.h"
#include "support/atmel_start_pins.h"
#include "periph/ADXL375/ADXL375.hpp"
#include "periph/BMP388/BMP388.hpp"
#include "periph/BMI088/BMI088.hpp"
#include "main.hpp"
#include "ArduinoJson.h"
#include "Battery.hpp"

#define ALT_LED LED1

class AltimeterTask
{
private:
    static const size_t stackSize = 2000;

    static TaskHandle_t taskHandle;
    static StaticTask_t xTaskBuffer;
    static StackType_t xStack[stackSize];

    static void activity(void *p);

public:
    AltimeterTask();
    TaskHandle_t getTaskHandle();
};