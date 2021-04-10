#pragma once

#include <FreeRTOS.h>
#include <task.h>
#include <message_buffer.h>
#include <semphr.h>
#include <hal_rtos.h>
#include "Task.hpp"

//class AltFilterTask;

#include "AltFilter.hpp"
#include "FlightPlan.hpp"
#include "SensorData.h"

#include "MsgBuffer.hpp"

class AltFilterTask : Task<2000>
{
public:
    AltFilterTask(uint8_t priority);
    void queueSensorData(SensorData &data);

    static SensorData data;
    static AltFilter filter;
    static FlightPlan plan;

private:

    static MsgBuffer<SensorData, 1000> dataBuffer;

    void activity();
};

#include "main.hpp"
#include "ArduinoJson.h"
