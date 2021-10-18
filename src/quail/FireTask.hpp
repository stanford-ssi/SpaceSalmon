#pragma once

#include <FreeRTOS.h>
#include <task.h>
#include <message_buffer.h>
#include <semphr.h>
#include <hal_rtos.h>

#include "Task.hpp"

#include "../periph/MC33797/Squib.hpp"

class FireTask : public Task<2000>
{
public:
    FireTask(uint8_t priority);
    Squib squib1;

private:
    void activity();
};

#include "main.hpp"
#include "ArduinoJson.h"
