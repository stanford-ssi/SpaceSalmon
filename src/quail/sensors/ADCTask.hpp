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

class ADCTask : public Task<2000>
{
public:
    ADCTask(uint8_t priority);

    /**
     * @brief set flag to indicate that all sensor channels have been configured
     */
    void sensorsConfigured();

    void activity();

private:
    static void adcISR();
    StaticEventGroup_t evbuf;
    EventGroupHandle_t evgroup;
};

#include "main.hpp"
#include "ArduinoJson.h"
