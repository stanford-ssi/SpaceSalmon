#include "TankFSM.hpp"
#include "main.hpp"

TankFSM::TankFSM(uint8_t priority) : TankGeneric(
    priority,
    sys.telem_slate.fuel_state,
    sys.telem_slate.fuel_op_press,
    sys.telem_slate.fuel_source_press,
    sys.telem_slate.fuel_up_press,
    sys.telem_slate.fuel_fill,
    sys.telem_slate.fuel_main,
    sys.telem_slate.fuel_bleed) {}

void TankFSM::activity() {
    lastTick = xTaskGetTickCount();

    while (true) {
        if (down_press() > MAWP) state << TANK_DRAIN;
        ZERO_PRESS = 0.1 * op_press();

        switch(state()) {
            case TANK_IDLE_EMPTY:  // only way to get out of idle is through user or tank command
                if(down_press() > MAWP / 2) {
                    state << TANK_IDLE_PRESS;
                }
                break;
            case TANK_IDLE_PRESS: // if at any point idle tank is pressurized assume pressurized till reset
                break;
            case TANK_EMPTY:
                if(down_press() > ZERO_PRESS) {
                    state << TANK_DRAIN;
                }
                break;
            case TANK_DRAIN:
                if (down_press() > ZERO_PRESS) {
                    vent_sol << true;
                } else {
                    vent_sol << false;
                    state << TANK_EMPTY;
                }
                break;
            case TANK_FILL:
                if (inBounds()) {
                    state << TANK_FULL;
                } {
                    TickType_t pulseTime = xTaskGetTickCount();
                    fill_sol << true;
                    vTaskDelayUntil(&pulseTime, PULSE_FREQ);
                    fill_sol << false;
                }
                break;
            case TANK_FULL:
                if (!inBounds()) {
                    state << TANK_FILL;
                } 
                break;
            case TANK_BLEED:
                if (up_press() > ZERO_PRESS) {
                    bleed_sol << true;
                } else {
                    bleed_sol << false;
                    state << TANK_READY; // assume bleed to prepare for launch
                }
                break;
            case TANK_READY: // lines are bled so won't fill again
                break;
        }

        vTaskDelayUntil(&lastTick, FSM_FREQ);
    }
}

bool TankFSM::inBounds() {
    return op_press() - down_press() <= ZERO_PRESS;
}