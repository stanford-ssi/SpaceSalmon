#pragma once

#include <FreeRTOS.h>
#include <task.h>
#include <message_buffer.h>
#include <semphr.h>
#include <hal_rtos.h>
#include "event_groups.h"

#include "Task.hpp"
#include "../config.h"
#include "../slate/SlateAbstractions.hpp"
#include "Array.hpp"

#include "../../periph/MC33797/Squib.hpp"

typedef struct {
    Squib* squib;
    Squib_FireType firecmd;
} squibChannel_t;

class FireTask : public Task<2000>
{
public:
    FireTask(uint8_t priority, uint8_t Squib_SS);
    bool fireEmatch(uint8_t ch, bool update = true); // fire the ematch at zero-indexed ch
    void activity();

private:
    Squib squib; // squib controlling E1-E4
    squibChannel_t ch_map [NUM_EM_CHANNELS];

    Array<Igniter, NUM_EM_CHANNELS> &slate;
    friend class Igniter; // Igniters can tell me to update

    StaticEventGroup_t evbuf;
    EventGroupHandle_t squibManager;

    void _updateSquibs();
};

#include "main.hpp"
#include "ArduinoJson.h"
