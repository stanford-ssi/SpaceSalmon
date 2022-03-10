#pragma once

#include <FreeRTOS.h>
#include <task.h>
#include "Task.hpp"
#include "event_groups.h"
#include <hal_rtos.h>
#include <timers.h>

#define UPDATE_VALVES 0b01
#define NUM_SOLENOIDS 8
#define MAX_NUM_VALVES_POWERED 2

typedef enum{
    OPEN = 1,
    CLOSED = 0,
} solenoid_state_t; // state indicators for SVs to keep the nomenclature consistent

typedef enum{
    NORMALLY_CLOSED = OPEN,
    NORMALLY_OPEN = CLOSED,
} solenoid_normal_t; // defines when the solenoid ought to be powered

typedef enum {
    REAL_SMALL = 10,
    P_SMALL = 50,
    SMALL = 130,
    MEDIUM = 220, // Edelbrook
    LARGE = 180, // Pro BigShot
    ABORT = 255 // don't pwm, just digital write
} solenoid_pwm_t; // making solenoid pwm values more readable, values taken from old quail

typedef struct {
    char* name;
    solenoid_pwm_t pwm;
    solenoid_normal_t normal;
} solenoid_t;

class ValveTask : public Task<2000> {
    public:
        ValveTask(uint8_t priority, uint8_t valve_pin_start);
        ValveTask(uint8_t priority);

        void activity();

        bool openSolenoid(uint8_t ch, bool update = true) {return _updateSolenoid(ch, update, OPEN);};
        bool closeSolenoid(uint8_t ch, bool update = true) {return _updateSolenoid(ch, update, CLOSED);};
        bool pulseSolenoid(uint8_t sol_ch, uint16_t pulse_dur);

    private:
        uint8_t valve_pin_start;
        uint8_t num_valves_powered = 0;
        solenoid_t valves[NUM_SOLENOIDS];

        StaticEventGroup_t evBuf;
        EventGroupHandle_t valveManager;
        
        void _updateValves();
        bool _updateSolenoid(uint8_t ch, bool update, solenoid_state_t state);

        static TimerHandle_t pulseTimers[NUM_SOLENOIDS]; // xTimers for callback (timer IDS correspond to the array index & solenoid channel)
        static StaticTimer_t pulseBufs[NUM_SOLENOIDS]; // xTimer static buffer for pulse timers

        static void _callback(TimerHandle_t xTimer);
};