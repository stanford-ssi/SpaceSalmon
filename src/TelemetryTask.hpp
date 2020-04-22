#pragma once

#include <FreeRTOS.h>
#include <task.h>
#include <message_buffer.h>
#include <semphr.h>
#include <hal_rtos.h>
#include "Poster.hpp"

class TelemetryTask
{
public:
    TelemetryTask(uint8_t priority);
    TaskHandle_t getTaskHandle();

private:
    static const size_t stackSize = 2000;

    static TaskHandle_t taskHandle;
    static StaticTask_t xTaskBuffer;
    static StackType_t xStack[stackSize];

    static void activityWrapper(void *p);
    void activity();
};

#include "main.hpp"
#include "ArduinoJson.h"
