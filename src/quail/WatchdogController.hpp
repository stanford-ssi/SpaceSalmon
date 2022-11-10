#pragma once
#include "../slate/slate_utils.h"
#include "Adafruit_SleepyDog.h"

// When the watchdog is initialized, watchdog_control and watchdog_counter are both initalized to the
// same starting value. watchdog_counter begins to count down from that value to 0, and the WDT is
// started. When the initalization value is writen to the watchdog_counter channel, a reset of the
// WDT is triggered, and the counter restarts is downward count. When a different value is written to
// the watchdog_control channel, the WDT is re-initialized to that new timout. If 0 is written to
// watchdog_control, the WDT is disabled.

class WatchdogController
{
private:
    SlateField<uint32_t> &_counter;
    SlateField<uint32_t> &_control;
    uint32_t last_reset = 0;
    uint32_t interval = 0;

public:
    WatchdogController(SlateField<uint32_t> &counter, SlateField<uint32_t> &control) : _counter(counter), _control(control) {}

    // set and enable the WTD to the given timeout. If timeout is 0, the WDT is disabled.
    void set_interval(uint32_t timeout)
    {
        printf("got here\n");
        if (timeout)
        {
            printf("and here\n");
            interval = Watchdog.enable(timeout);
        }
        else
        {
            printf("but not here\n");
            interval = 0;
            Watchdog.disable();
        }

        _control.set(interval);
        _counter.set(timeout ? (interval - 1) : interval);

        if (xTaskGetSchedulerState() == taskSCHEDULER_RUNNING)
        {
            last_reset = xTaskGetTickCount();
        }
        else
        {
            last_reset = 0;
        }
    }

    void service()
    {
        if (interval)
        {
            //printf("also later here\n");
            bool done = false;
            while (!done)
            {
                uint32_t new_counter = _counter.get();
                if (new_counter >= interval)
                {
                    printf("it got pet\n");
                    Watchdog.reset();
                    last_reset = xTaskGetTickCount();
                    _counter.set(interval - 1);
                    break;
                }
                uint32_t time_left = max((int32_t)interval - (int32_t)(xTaskGetTickCount() - last_reset), 0L);
                done = _counter.CAS(new_counter, time_left);
            }
        }
        uint32_t new_interval = _control.get();
        if (new_interval != interval)
        {
            set_interval(new_interval);
        }
    }
};