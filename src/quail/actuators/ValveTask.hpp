#pragma once

#include <FreeRTOS.h>
#include <task.h>
#include "event_groups.h"
#include <hal_rtos.h>
#include <timers.h>

#include "Task.hpp"
#include "../config.h"
#include "Array.hpp"
#include "../slate/SlateAbstractions.hpp"

class ValveTask : public Task<2000> {
    public:
        ValveTask(uint8_t priority, uint8_t valve_pin_start);

        void activity();

        bool openSolenoid(uint8_t ch, bool update = true) { return updateSolenoid(ch, update, OPEN); };
        bool closeSolenoid(uint8_t ch, bool update = true) { return updateSolenoid(ch, update, CLOSED); };
        bool pulseSolenoid(uint8_t sol_ch);
        bool pulseSolenoid(uint8_t ch, uint16_t pulse_dur);
        bool updatePulse(uint8_t ch, uint16_t pulse_dur);

    private:
        uint8_t valve_pin_start;

        Array<Solenoid, NUM_SOLENOIDS> &slate;
        friend class PulseEndpoint;
        friend class Solenoid;

        StaticEventGroup_t evBuf;
        EventGroupHandle_t valveManager;
        
        bool updateSolenoid(uint8_t ch, bool update, solenoid_state_t state);

        void _updateValves(uint8_t index);
        bool _updatePulse(uint8_t ch);
        bool _updatePower(uint8_t ch);
        bool _pulseSolenoid(uint8_t ch);
        bool _pulseSolenoind(uint8_t ch, uint16_t pulse_dur);

        static TimerHandle_t pulseTimers[NUM_SOLENOIDS]; // xTimers for callback (timer IDS correspond to the array index & solenoid channel)
        static StaticTimer_t pulseBufs[NUM_SOLENOIDS]; // xTimer static buffer for pulse timers

        static void _callback(TimerHandle_t xTimer);
};