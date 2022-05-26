#include "EngineFSM.hpp"
#include "main.hpp"


EngineFSM::EngineFSM(uint8_t priority) : Task(priority, "Engine"), 
    state(sys.slate.sequence.engineState),
    igniter(sys.slate.squib[0]),
    comms(sys.slate.board.comms),
    safe(sys.slate.valves[8]),
    oxState(sys.slate.sequence.oxState),
    oxMain(sys.slate.valves[1]),
    fuelState(sys.slate.sequence.fuelState),
    fuelMain(sys.slate.valves[0]) {};

void EngineFSM::activity() {
    lastTick = xTaskGetTickCount();

    while (true) {
        if(!comms()) state << ABORT;

        switch(state()) {
            case ENGINE_IDLE: // only way to get out of idle is through user command
                safe.state << CLOSED;
                break;
            case ENGINE_FILL: // if tanks are not full, fill them
                safe.state << OPEN;
                if (oxState() != TANK_FULL &&
                    oxState() != TANK_BLEED &&
                    oxState() != TANK_READY) {
                    oxState << TANK_FILL;
                } else if(fuelState() != TANK_FULL &&
                    fuelState() != TANK_BLEED &&
                    fuelState() != TANK_READY) {
                    fuelState << TANK_FILL;
                } else {
                    state << ENGINE_FULL;
                } 
                break;
            case ENGINE_FULL: // only way out of full is user cmd
                safe.state << OPEN;
                break;
            case ENGINE_FIRE: // arm igniter, prep tanks, fire
                safe.state << OPEN;
                igniter.arm << ARMED;
                prepTank(oxState);
                prepTank(fuelState);
                if (oxState() == TANK_READY && fuelState() == TANK_READY) {
                    igniter.state << FIRED;
                    state << MAIN_ACTUATION;
                }
                break;
            case MAIN_ACTUATION: // empty tanks
                safe.state << OPEN;
                vTaskDelay(FIRE_DELAY);
                oxMain.state << OPEN;
                fuelMain.state << OPEN;
                break;
            case ABORT: // drain tanks and reset quail
                safe.state << OPEN;
                if(oxState() != TANK_EMPTY) {
                    oxState << TANK_DRAIN;
                } else  if (fuelState() != TANK_EMPTY) {
                    fuelState << TANK_DRAIN;
                } else {
                    safe.state << CLOSED;
                    NVIC_SystemReset();
                }
                break;
        }

        sys.tasks.valvetask._updateValves();
        sys.tasks.firetask._updateSquibs();
        vTaskDelayUntil(&lastTick, FSM_FREQ);
    }
}

void EngineFSM::prepTank(EndPoint<TankState>& tankState) {
    switch(tankState()) {
        case TANK_EMPTY:
            tankState << TANK_FILL;
            break;
        case TANK_FULL:
            tankState << TANK_BLEED;
            break;
    }
}