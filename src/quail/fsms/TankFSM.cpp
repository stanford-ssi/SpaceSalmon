#include "TankFSM.hpp"
#include "main.hpp"

// default constructor is for ox tank in launch config
TankFSM::TankFSM(uint8_t priority) : TankFSM(priority, 
    sys.slate.sequence.oxState, 
    sys.slate.sequence.oxOpPressure,
    sys.slate.sense.pt4,
    sys.slate.valves[4],
    sys.slate.valves[7],
    sys.slate.valves[5]) {};

// different tanks are parametrized by different Endpoints
TankFSM::TankFSM(uint8_t priority,
    EndPoint<TankState>& state, 
    EndPoint<float>& opPress,
    SensorSlate& press,
    Solenoid& fillSol,
    Solenoid& ventSol,
    Solenoid& bleedSol) : 
    Task(priority, "Tank"),
    state(state),
    opPress(opPress),
    press(press),
    fillSol(fillSol),
    ventSol(ventSol),
    bleedSol(bleedSol) {}


void TankFSM::activity() {
    lastTick = xTaskGetTickCount();

    while (true) {
        if (press() > MAWP) state << TANK_DRAIN;
        const float ZERO = 0.1 * opPress();

        switch(state()) {
            case TANK_IDLE:  // only way to get out of idle is through user or tank command
                break;
            case TANK_EMPTY:
                if(press() > ZERO) {
                    state << TANK_DRAIN;
                }
                break;
            case TANK_DRAIN:
                if (press() > ZERO) {
                    ventSol.state << OPEN;
                } else {
                    ventSol.state << CLOSED;
                    state << TANK_EMPTY;
                }
                break;
            case TANK_FILL:
                bleedSol.state << CLOSED;
                if (abs(press() - opPress()) > ZERO) {
                    if (press() > opPress()) {
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
                if (abs(press() - opPress()) > ZERO) {
                    state << TANK_FILL;
                } 
                break;
            case TANK_BLEED:
                if (press() > ZERO) {
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