#include "EngineControlTask.hpp"
#include "main.hpp"

EngineControlTask::EngineControlTask(uint8_t priority)
    : Task(priority, "Engine Control") {}

void EngineControlTask::activity()
{
    TickType_t time = xTaskGetTickCount();

    while (true)
    {
        xTaskDelayUntil(&time, 100);
        float gross_mass = (sys.telem_slate.lc1.get() + sys.telem_slate.lc2.get()) / 9.807;
        float load_mass = gross_mass - sys.telem_slate.load_mass_offset.get();
        sys.telem_slate.load_mass.set(load_mass);
    }
}