#pragma once

#include <FreeRTOS.h>
#include <task.h>
#include <message_buffer.h>
#include <semphr.h>
#include "event_groups.h"
#include <hal_rtos.h>
#include "ssi_adc.h"
#include "SlateKey.hpp"

#include "Task.hpp"

class PowerTask : public Task<2000>
{
public:
    PowerTask(uint8_t priority, SlateKey<float> &current_channel, SlateKey<float> &voltage_channel)
        : Task(priority, "PowerMonitor"), current_channel(current_channel), voltage_channel(voltage_channel){};
    void activity();
    ADC adc0 = ADC(ADC0);

protected:
    SlateKey<float> current_channel;
    SlateKey<float> voltage_channel;

private:
    StaticEventGroup_t evbuf;
    EventGroupHandle_t evgroup;
};

#include "main.hpp"
#include "ArduinoJson.h"
