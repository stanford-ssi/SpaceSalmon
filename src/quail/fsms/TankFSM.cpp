#include "TankFSM.hpp"
#include "main.hpp"

// different tanks are parametrized by different Endpoints
TankFSM::TankFSM(uint8_t priority,
    EndPoint<TankState>& state, 
    EndPoint<float>& opPress,
    SensorSlate& press,
    Solenoid& fillSol,
    Solenoid& ventSol,
    Solenoid& bleedSol) : 
    TankGeneric(priority, state, opPress, press, weight, fillSol, ventSol, bleedSol)
    {
        weight << 0;
    }


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
                    ventSol.state << OPEN;
                } else {
                    ventSol.state << CLOSED;
                    state << TANK_EMPTY;
                }
                break;
            case TANK_FILL:
                bleedSol.state << CLOSED;
                if (!inBounds()) {
                    if (press() > op()) {
                        ventSol.time << FSM_PTM;
                    } else {
                        fillSol.time << FSM_PTM;
                    }
                } else {
                    fillSol.state << CLOSED;
                    ventSol.state << CLOSED;
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

bool TankFSM::inBounds() {
    return abs(press() - op()) <= ZERO_PRESS;
}