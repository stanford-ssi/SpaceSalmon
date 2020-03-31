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

#include "MsgBuffer.hpp"

class AltFilterTask
{
public:
    AltFilterTask(uint8_t priority);
    TaskHandle_t getTaskHandle();
    void queueSensorData(SensorData &data);

    static SensorData data;
    static AltFilter filter;
    static FlightPlan plan;

private:
    static const size_t stackSize = 2000;

    static TaskHandle_t taskHandle;
    static StaticTask_t xTaskBuffer;
    static StackType_t xStack[stackSize];

    static MsgBuffer<SensorData, 1000> dataBuffer;

    static void activity(void *p);
};

#include "main.hpp"
#include "ArduinoJson.h"
