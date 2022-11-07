#pragma once

#include "FreeRTOS.h"
#include "../../slate/slate_utils.h"
#include "Arduino.h"

class ValveController
{
private:
    enum class valve_state
    {
        off,
        on,
        on_timer
    };

    SlateField<bool> &_active;
    SlateField<int16_t> &_timer;
    uint8_t _pin;
    valve_state _state;
    uint32_t _timer_end;

public:
    ValveController(SlateField<bool> &active, SlateField<int16_t> &timer, uint8_t pin) : _active(active), _timer(timer), _pin(pin) 
    {
    }

    void serviceValve()
    {
        switch (_state)
        {
        case valve_state::off:
            if (_active.get())
            {
                _state = valve_state::on;
            }
            else if (_timer.get() > 0)
            {
                _state = valve_state::on_timer;
                _timer_end = xTaskGetTickCount() + _timer.get(); // WARNING! rolls over at 49 days!
                _active.set(true);
            }

            break;

        case valve_state::on:
            if (_active.get() == false)
            {
                _state = valve_state::off;
                _timer.set(0);
            }
            break;

        case valve_state::on_timer:
            uint32_t time_now = xTaskGetTickCount();
            if (_active.get() == false)
            {
                _state = valve_state::off;
                _timer.set(0);
            }
            else if (time_now < _timer_end)
            {
                _timer.set(_timer_end - time_now);
            }
            else
            {
                _state = valve_state::off;
                _timer.set(0);
                _active.set(false);
            }
        }

        // take action
        if (_state == valve_state::on || _state == valve_state::on_timer)
        {
            analogWrite(_pin, 180);
        }
        else
        {
            analogWrite(_pin, 0);
        }
    }
};