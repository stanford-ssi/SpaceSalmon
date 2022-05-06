#pragma once

#include <FreeRTOS.h>
#include <task.h>
#include "Task.hpp"
#include "event_groups.h"
#include <hal_rtos.h>

#define VALVES_UPDATED 0b01
#define NUM_SOLENOIDS 12

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
        void updateValves();
 
    private:
        uint8_t valve_pin_start;
        solenoid_t valves[NUM_SOLENOIDS];

        StaticEventGroup_t evBuf;
        EventGroupHandle_t valveManager;
};