#include "VaPakFSM.hpp"
#include "main.hpp"

// different tanks are parametrized by different Endpoints
VaPakFSM::VaPakFSM(uint8_t priority) : TankGeneric(
    priority, 
    sys.telem_slate.oxdz_state, 
    sys.telem_slate.oxdz_op_press, 
    sys.telem_slate.oxdz_press, 
    weight, 
    sys.telem_slate.oxdz_fill, 
    sys.telem_slate.oxdz_vent,
    sys.telem_slate.oxdz_bleed) { }

void VaPakFSM::activity() {
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
                bleed_sol << true;
                if (!inBounds()) {
                    // if (press() > op()) {
                    //     ventSol.time << FSM_PTM;
                    // } else {
                    //     fillSol.time << FSM_PTM;
                    // }
                } else {
                    fill_sol << true;
                    vent_sol << false;
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

bool VaPakFSM::inBounds() {
    return op() - press() <= ZERO_PRESS;
}