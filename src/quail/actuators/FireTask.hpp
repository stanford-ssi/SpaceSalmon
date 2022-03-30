#pragma once

#include <FreeRTOS.h>
#include <task.h>
#include <message_buffer.h>
#include <semphr.h>
#include <hal_rtos.h>
#include "event_groups.h"

#include "Task.hpp"
#include "../config.h"
#include "../slate/abstractions.hpp"
#include "Array.hpp"

#include "../../periph/MC33797/Squib.hpp"

typedef struct {
    Squib* squib;
    Squib_FireType firecmd;
} squibChannel_t;

class FireTask : public Task<2000>
{
public:
    FireTask(uint8_t priority, uint8_t Squib1_SS, uint8_t Squib2_SS);
    bool fireEmatch(uint8_t ch, bool update = true); // fire the ematch at zero-indexed ch
    void activity();

private:
    Squib squib1; // squib controlling E1-E4
    Squib squib2; // squib controlling E5-E8

    Array<Igniter, NUM_EM_CHANNELS>& slate;
    squibChannel_t ch_map [NUM_EM_CHANNELS];

    StaticEventGroup_t evbuf;
    EventGroupHandle_t squibManager;

    void _updateSquibs();
};

#include "main.hpp"
#include "ArduinoJson.h"
