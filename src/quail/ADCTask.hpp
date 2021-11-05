#pragma once

#include <FreeRTOS.h>
#include <task.h>
#include <message_buffer.h>
#include <semphr.h>
#include <hal_rtos.h>

#include "Task.hpp"

typedef enum
{
    PRESSURE_CFG,
    LOAD_CFG,
    THERMAL_CFG
} sensorcfg_t; // sensor config types

class ADCTask : public Task<2000>
{
public:
    ADCTask(uint8_t priority);

private:
    void activity();
};

#include "main.hpp"
#include "ArduinoJson.h"
