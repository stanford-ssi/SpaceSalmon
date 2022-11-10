#pragma once

#include <FreeRTOS.h>
#include <task.h>
#include <message_buffer.h>
#include <semphr.h>
#include "event_groups.h"
#include <hal_rtos.h>
#include "ssi_adc.h"
#include "SlateKey.hpp"
#include "WatchdogController.hpp"

#include "Task.hpp"

class PowerTask : public Task<1500>
{
public:
    PowerTask(uint8_t priority);
    void activity();
    ADC adc0 = ADC(ADC0);
    WatchdogController watchdog;

private:
    StaticEventGroup_t evbuf;
    EventGroupHandle_t evgroup;
};

#include "main.hpp"
#include "ArduinoJson.h"
