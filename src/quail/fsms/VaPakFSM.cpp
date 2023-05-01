#include "VaPakFSM.hpp"
#include "main.hpp"

// different tanks are parametrized by different Endpoints
VaPakFSM::VaPakFSM(uint8_t priority) : TankGeneric(
    priority, 
    sys.telem_slate.oxdz_state, 
    sys.telem_slate.oxdz_op_press, 
    sys.telem_slate.oxdz_up_press, 
    sys.telem_slate.oxdz_down_press, 
    sys.telem_slate.oxdz_fill, 
    sys.telem_slate.oxdz_vent,
    sys.telem_slate.oxdz_bleed), 
    op_mass(sys.telem_slate.oxdz_op_mass),
    mass(sys.telem_slate.load_mass),
    mass_offset(sys.telem_slate.load_mass_offset) { }

void VaPakFSM::activity() {
    lastTick = xTaskGetTickCount();

    while (true) {
        if (down_press() > MAWP) state << TANK_DRAIN;
        ZERO_PRESS = 0.1 * op_press();
        ZERO_WEIGHT = 0.1 * op_mass();

        float last_mass = 0, current_mass = 0;

        switch(state()) {
            last_mass = current_mass;
            current_mass = mass() - mass_offset();

            case TANK_IDLE_EMPTY:  // only way to get out of idle is through user or tank command
                if(down_press() > MAWP / 2) {
                    state << TANK_IDLE_PRESS;
                }
                mass_offset << mass();
                break;
            case TANK_IDLE_PRESS: // if at any point idle tank is pressurized assume pressurized till reset
                break;
            case TANK_EMPTY:
                if(down_press() > ZERO_PRESS) {
                    state << TANK_DRAIN;
                }
                mass_offset << mass();
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
                fill_sol << true;
                if (inBounds()) {
                    fill_sol << false;
                    state << TANK_FULL;
                }
                if (notFilling(current_mass, last_mass)) {
                    TickType_t pulseTime = xTaskGetTickCount();
                    vent_sol << true;
                    vTaskDelayUntil(&pulseTime, PULSE_FREQ);
                    vent_sol << false;
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

bool VaPakFSM::inBounds() {
    return mass() - mass_offset() - op_mass() <= ZERO_WEIGHT;
}

bool VaPakFSM::notFilling(float current_mass, float last_mass) {
    return abs(current_mass - last_mass) < MIN_MASS_DELTA;
}