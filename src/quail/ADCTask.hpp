#pragma once

#include <FreeRTOS.h>
#include <task.h>
#include <message_buffer.h>
#include <semphr.h>
#include "event_groups.h"
#include <hal_rtos.h>

#include "Task.hpp"

#define ADC_READY 0b1

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

    void activity();

private:
    static void adcISR();
    StaticEventGroup_t evbuf;
    EventGroupHandle_t evgroup;
};

#include "main.hpp"
#include "ArduinoJson.h"
