#include "TankFSM.hpp"
#include "main.hpp"

// different tanks are parametrized by different Endpoints
TankFSM::TankFSM(uint8_t priority,
    SlateField<TankState>& state, 
    SlateField<float>& op_press,
    SlateField<float>& press,
    SlateField<bool>& fill_sol,
    SlateField<bool>& vent_sol,
    SlateField<bool>& bleed_sol) :
    TankGeneric(priority, state, op_press, press, weight, fill_sol, vent_sol, bleed_sol) { }


void TankFSM::activity() {
    lastTick = xTaskGetTickCount();

    while (true) {
        if (press() > MAWP) state << TANK_DRAIN;
        ZERO_PRESS = 0.1 * op();

        switch(state()) {
            case TANK_IDLE_EMPTY:  // only way to get out of idle is through user or tank command
                if(press() > MAWP / 2) {
                    state << TANK_IDLE_PRESS;
                }
                break;
            case TANK_IDLE_PRESS: // if at any point idle tank is pressurized assume pressurized till reset
                break;
            case TANK_EMPTY:
                if(press() > ZERO_PRESS) {
                    state << TANK_DRAIN;
                }
                break;
            case TANK_DRAIN:
                if (press() > ZERO_PRESS) {
                    vent_sol << true;
                } else {
                    vent_sol << false;
                    state << TANK_EMPTY;
                }
                break;
            case TANK_FILL:
                bleed_sol<< CLOSED;
                if (!inBounds()) {
                    fill_sol << true;
                } else {
                    fill_sol << false;
                    state << TANK_FULL;
                }
                break;
            case TANK_FULL:
                if (!inBounds()) {
                    state << TANK_FILL;
                } 
                break;
            case TANK_BLEED:
                if (press() > ZERO_PRESS) {
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
    return op() - press() <= ZERO_PRESS;
}