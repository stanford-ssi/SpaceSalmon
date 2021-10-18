#pragma once

#include <FreeRTOS.h>
#include <task.h>
#include <message_buffer.h>
#include <semphr.h>
#include <hal_rtos.h>

#include "Task.hpp"

class ADCTask : public Task<2000>
{
public:
    ADCTask(uint8_t priority);

private:
    void activity();
};

#include "main.hpp"
#include "ArduinoJson.h"
