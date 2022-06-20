#pragma once

#include <FreeRTOS.h>
#include <task.h>
#include <message_buffer.h>
#include <semphr.h>
#include <hal_rtos.h>

#include "Task.hpp"

class ArmingTask : Task<2000>
{
public:
    ArmingTask(uint8_t priority);

private:

    void activity();
};

#include "main.hpp"