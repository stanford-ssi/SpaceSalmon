#pragma once

#include <FreeRTOS.h>
#include <task.h>
#include <message_buffer.h>
#include <semphr.h>
#include <hal_rtos.h>

//class AltFilterTask;

#include "AltFilter.hpp"
#include "FlightPlan.hpp"
#include "SensorData.h"

class AltFilterTask
{
public:
    AltFilterTask();
    TaskHandle_t getTaskHandle();
    void queueSensorData(SensorData data);

private:
    static const size_t stackSize = 2000;

    static TaskHandle_t taskHandle;
    static StaticTask_t xTaskBuffer;
    static StackType_t xStack[stackSize];

    static const size_t bufferSize = 1000;
    static MessageBufferHandle_t bufferHandle;
    static StaticMessageBuffer_t messageBufferStruct;
    static uint8_t ucStorageBuffer[bufferSize];

    static SensorData data;
    static AltFilter filter;
    static FlightPlan plan;
    static void activity(void *p);

};

#include "main.hpp"
#include "ArduinoJson.h"
