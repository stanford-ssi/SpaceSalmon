#pragma once

#include <FreeRTOS.h>
#include <task.h>
#include "Task.hpp"

#define NUM_SOLENOIDS 8

class SolenoidTask:Task<1000> {
public:
    SolenoidTask(uint8_t priority) : Task(priority, "LED"){};
    void activity();
};