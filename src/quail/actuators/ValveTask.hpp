
#pragma once
#include "FreeRTOS.h"
#include "Task.hpp"
#include "ValveController.hpp"
#include <functional>

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
            for (auto valve : valveList)
            {
                valve.get().serviceValve();
            }
        }
    }

private:
    ValveController
        valve1,
        valve2,
        valve3,
        valve4,
        valve5,
        valve6,
        valve7,
        valve8,
        valve9,
        valve10,
        valve11,
        valve12;

    std::reference_wrapper<ValveController> valveList[12]{
        valve1,
        valve2,
        valve3,
        valve4,
        valve5,
        valve6,
        valve7,
        valve8,
        valve9,
        valve10,
        valve11,
        valve12};
};