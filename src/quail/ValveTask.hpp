#pragma once

#include <FreeRTOS.h>
#include <task.h>
#include "Task.hpp"
#include "event_groups.h"
#include <hal_rtos.h>

#define SOLENOID_START 22
#define MAX_SOLENOIDS 8

typedef struct {
    char* name;
    uint8_t num;
    uint8_t pwm;
} solenoid_t;

class ValveTask : public Task<2000> {
    public:
        ValveTask(uint8_t priority, uint8_t num_valves);
        ValveTask(uint8_t priority);

        void activity();
        void openValve(const uint8_t valve);
        void openValve(const char* name);
        bool checkValve(const uint8_t valve);
        bool checkValve(const char* name);

    private:
        void configure();
        uint8_t numValves;
        solenoid_t valves[MAX_SOLENOIDS];

        StaticEventGroup_t evBuf;
        EventGroupHandle_t valveManager;
};