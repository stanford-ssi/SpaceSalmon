#include "VaPakFSM.hpp"
#include "main.hpp"

// default constructor is for ox tank in launch config
// TankFSM::TankFSM(uint8_t priority) : TankGeneric(priority, 
//     sys.slate.sequence.oxState, 
//     sys.slate.sequence.oxOpPressure,
//     sys.slate.sense.pt4,
//     sys.slate.valves[4],
//     sys.slate.valves[7],
//     sys.slate.valves[5]) {};

void VaPakFSM::activity() {
    lastTick = xTaskGetTickCount();
    ZERO_PRESS = 0.1 * MAWP;

    while (true) {
        if (press() > MAWP) state << TANK_DRAIN;
        ZERO_WEIGHT = 0.1 * op();

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
                } else {
                    weight.ofs << -weight.val();
                }
                break;
            case TANK_DRAIN:
                if (press() > ZERO_PRESS) {
                    ventSol.state << OPEN;
                } else {
                    ventSol.state << CLOSED;
                    state << TANK_EMPTY;
                }
                break;
            case TANK_FILL:
                vTaskDelayUntil(&lastBoundsCheck, OX_FILL_DELAY);
                if (!inBounds()) { // all solenoids closed after bounds check
                    if (press() > op()) {
                        ventSol.time << FSM_PTM;
                    } else {
                        fillSol.state << OPEN;
                        ventSol.state << OPEN;
                    }
                } else {
                    state << TANK_FULL;
                }
                break;
            case TANK_FULL:
                vTaskDelayUntil(&lastBoundsCheck, OX_FILL_DELAY);
                if (!inBounds()) {
                    state << TANK_FILL;
                } 
                break;
            case TANK_BLEED:
                if (press() > ZERO_PRESS) {
                    bleedSol.state << OPEN;
                } else {
                    bleedSol.state << CLOSED;
                    state << TANK_READY; // assume bleed to prepare for launch
                }
                break;
            case TANK_READY: // lines are bled so won't fill again
                break;
        }

        sys.tasks.valvetask._updateValves();
        vTaskDelayUntil(&lastTick, FSM_FREQ);
    }
}

bool VaPakFSM::inBounds() {
    fillSol.state << CLOSED;
    ventSol.state << CLOSED;
    bleedSol.state << CLOSED;
    sys.tasks.valvetask._updateValves();
    vTaskDelay(LOAD_EQUILIBRIUM_DELAY);
    lastBoundsCheck = xTaskGetTickCount();
    return abs(weight() - op()) < ZERO_WEIGHT;
}