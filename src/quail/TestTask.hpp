#pragma once

#include <FreeRTOS.h>
#include <task.h>
#include "Task.hpp"
#include "event_groups.h"
#include <hal_rtos.h>

class TestTask : public Task<2000>{
public:
    TestTask(uint8_t priority):Task(priority, "Test"){};
    void activity();
};