#pragma once

#include <FreeRTOS.h>
#include <task.h>
#include <message_buffer.h>
#include <semphr.h>
#include "event_groups.h"
#include <hal_rtos.h>

#include "Task.hpp"

#define ADC_READY 0b01
#define SENSORS_READY 0b10

 struct {
    uint8_t channel = 0;
    uint32_t dataword = 0;
} typedef adcdata_t;

class ADCTask : public Task<2000>
{
public:
    ADCTask(uint8_t priority);

    void sensorsConfigured();

    void activity();

private:
    static void adcISR();
    StaticEventGroup_t evbuf;
    EventGroupHandle_t evgroup;
};

#include "main.hpp"
#include "ArduinoJson.h"
