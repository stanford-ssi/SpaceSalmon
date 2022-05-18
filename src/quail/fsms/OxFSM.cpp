#include "OxFSM.hpp"
#include "main.hpp"

OxFSM::OxFSM(uint8_t priority) : Task(priority, "Engine"), 
    state(sys.slate.sequence.oxState), 
    oxOp(sys.slate.sequence.oxOpPressure),
    oxPressure(sys.slate.sense.pt4),
    oxFill(sys.slate.valves[4]),
    oxVent(sys.slate.valves[7]),
    oxBleed(sys.slate.valves[5]) {};

void OxFSM::activity() {
    if (lastTick == 0) lastTick = xTaskGetTickCount();
    vTaskDelayUntil(&lastTick, FSM_FREQ);

    if (oxPressure() > MAWP) state << TANK_EMPTY;

    switch(state()) {
        case TANK_EMPTY:  // only way to get out of idle is through user or tank command
            break;
        case TANK_DRAIN:
            if (oxPressure() > 10) {
                oxVent.state << OPEN;
            } else {
                oxVent.state << CLOSED;
                state << TANK_EMPTY;
            }
            break;
        case TANK_FILL:
            oxBleed.state << CLOSED;
            if (abs(oxPressure() - oxOp()) > 10) {
                if (oxPressure() > oxOp()) {
                    oxVent.time << FSM_PTM;
                } else {
                    oxFill.time << FSM_PTM;
                }
            } else {
                oxFill.state << CLOSED;
                oxVent.state << CLOSED;
                state << TANK_FULL;
            }
            break;
        case TANK_FULL:
            break;
        case TANK_BLEED:
            if (oxPressure() > 10) {
                oxBleed.state << OPEN;
            } else {
                oxBleed.state << CLOSED;
                state << TANK_READY; // assume bleed to prepare for launch
            }
            break;
        case TANK_READY:
            break;
    }

    sys.tasks.valvetask._updateValves();
}