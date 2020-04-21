#pragma once

#include <FreeRTOS.h>
#include <task.h>
#include <message_buffer.h>
#include <semphr.h>
#include <hal_rtos.h>

class GPSTask
{
public:
    GPSTask(uint8_t priority);
    TaskHandle_t getTaskHandle();

private:
    static const size_t stackSize = 2000;

    static TaskHandle_t taskHandle;
    static StaticTask_t xTaskBuffer;
    static StackType_t xStack[stackSize];

    static void activity(void *p);
};

#include "main.hpp"
#include "ArduinoJson.h"
