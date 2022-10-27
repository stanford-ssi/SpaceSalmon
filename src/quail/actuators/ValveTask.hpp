
#pragma once
#include "FreeRTOS.h"
#include "Task.hpp"
#include "ValveController.hpp"

class ValveTask : public Task<256>
{
public:
    ValveTask(uint8_t priority);

    void activity()
    {
        TickType_t valve_time = xTaskGetTickCount();
        while (true)
        {
            vTaskDelayUntil(&valve_time, 10);
            valve1.serviceValve();
        }
    }

private:
    ValveController valve1;
};