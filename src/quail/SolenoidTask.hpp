#pragma once

#include <FreeRTOS.h>
#include <task.h>
#include <message_buffer.h>
#include <semphr.h>
#include <hal_rtos.h>
#include "event_groups.h"

#include "Task.hpp"

#define SV_STATE_UPDATED 0b1

class SolenoidTask:Task<1000> {
public:
    SolenoidTask(uint8_t priority, uint8_t firstSVpin, uint8_t lastSVpin);
    void activity();
    void update_SV();
private:
    const uint8_t firstSVpin;
    const uint8_t lastSVpin;
    StaticEventGroup_t evbuf;
    EventGroupHandle_t evgroup;
};