#include "EngineFSM.hpp"
#include "main.hpp"

EngineFSM::EngineFSM(uint8_t priority) : Task(priority, "Engine"), 
    state(sys.slate.sequence.engineState), 
    oxState(sys.slate.sequence.oxState),
    oxMain(sys.slate.valves[1]) 
    {};

void EngineFSM::activity() {
    while(true) vTaskDelay(FSM_FREQ);
    if (lastTick == 0) lastTick = xTaskGetTickCount();
    vTaskDelayUntil(&lastTick, FSM_FREQ);

    switch(state()) {
        case ENGINE_IDLE: // only way to get out of idle is through user command
            break;
        case ENGINE_FILL:
            if(oxState() == TANK_FULL) {
                state << ENGINE_FULL;
            } else {
                oxState << TANK_FILL;
            }
            break;
        case ENGINE_FULL: // only way out of full is user cmd
            break;
        case LAUNCH:
            switch(oxState()) {
                case TANK_EMPTY:
                    oxState << TANK_FILL;
                    break;
                case TANK_FULL:
                    oxState << TANK_BLEED;
                    break;
            }
            if (oxState() == TANK_READY) {
                oxMain.state << OPEN;
            }
            break;
        case ABORT:
            if (oxState() == TANK_EMPTY) {
                state << ENGINE_IDLE;
            } else {
                oxState << TANK_DRAIN;
            }
            break;
    }

    sys.tasks.valvetask._updateValves();
}
