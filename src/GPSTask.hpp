#pragma once

#define GPS_LED LED4

#include <FreeRTOS.h>
#include <task.h>
#include <semphr.h>
#include <hal_rtos.h>
#include "TinyGPS++.h"

class GPSTask
{
private:
    static const size_t stackSize = 2000;

    static TaskHandle_t taskHandle;
    static StaticTask_t xTaskBuffer;
    static StackType_t xStack[stackSize];

    static void activity(void *p);

    static TinyGPSPlus parser;

public:
    GPSTask();
    TaskHandle_t getTaskHandle();
};

#include "main.hpp"
#include "support/driver_init.h"
#include "printf.h"
#include "support/atmel_start_pins.h"
#include "ArduinoJson.h"

