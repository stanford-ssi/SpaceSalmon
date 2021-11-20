#pragma once

#include <FreeRTOS.h>
#include <task.h>
#include <message_buffer.h>
#include <semphr.h>
#include <hal_rtos.h>
#include "event_groups.h"

#include "Task.hpp"

#include "../periph/MC33797/Squib.hpp"

#define NUM_EM_CHANNELS 8

#define FIRE_CMD_RCVD 0b01

typedef struct {
    Squib* squib;
    Squib_FireType firecmd;
} squibChannel_t;

class FireTask : public Task<2000>
{
public:
    FireTask(uint8_t priority, uint8_t Squib1_SS, uint8_t Squib2_SS);
    Squib squib1; // squib controlling E1-E4
    Squib squib2; // squib controlling E5-E8
    void fireEmatch(uint8_t ch); // fire the ematch at zero-indexed ch
    void activity();

private:
    squibChannel_t ch_map [NUM_EM_CHANNELS];
    StaticEventGroup_t evbuf;
    EventGroupHandle_t evgroup;
};

#include "main.hpp"
#include "ArduinoJson.h"
