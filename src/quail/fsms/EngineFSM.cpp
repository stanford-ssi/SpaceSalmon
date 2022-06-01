#include "EngineFSM.hpp"
#include "main.hpp"


EngineFSM::EngineFSM(uint8_t priority) : Task(priority, "Engine"), 
    state(sys.slate.sequence.engineState),
    // igniter(sys.slate.squib[0]),
    igniter(sys.slate.valves[11]),
    comms(sys.slate.board.comms),
    safe(sys.slate.valves[8]),
    oxState(sys.slate.sequence.oxState),
    oxMain(sys.slate.valves[1]),
    fuelState(sys.slate.sequence.fuelState),
    fuelMain(sys.slate.valves[0]) {};

void EngineFSM::activity() {
    lastTick = xTaskGetTickCount();

    while (true) {
        if(!comms()) state << ENGINE_ABORT;

        switch(state()) {
            case ENGINE_ABORT: // drain tanks and reset quail
                safe.state << OPEN;
                if(possiblyFull(oxState)) {
                    oxState << TANK_DRAIN;
                } else  if (possiblyFull(fuelState)) {
                    fuelState << TANK_DRAIN;
                } else {
                    safe.state << CLOSED;
                    NVIC_SystemReset();
                }
                lastState = ENGINE_ABORT;
                break;
            case ENGINE_IDLE: // only way to get out of idle is through user command
                if(safeToApproach(oxState) && safeToApproach(fuelState)){
                    safe.state << CLOSED;
                } else {
                    safe.state << OPEN;
                }
                lastState = ENGINE_IDLE;
                break;
            case ENGINE_PREPOX: // if ox tank is not full, fill
                safe.state << OPEN;
                if (!certainlyFull(oxState)) {
                    oxState << TANK_FILL;
                } else if(prepTank(oxState)){
                    state << ENGINE_OXPREPPED;
                } 
                lastState = ENGINE_PREPOX;
                break;
            case ENGINE_OXPREPPED:
                if(lastState << ENGINE_PREPOX) state << lastState;
                if(!certainlyFull(oxState)) state << ENGINE_PREPOX;
                lastState = ENGINE_OXPREPPED;
                break;
            case ENGINE_PREPFUEL:
                if(lastState < ENGINE_OXPREPPED) state << lastState;
                if(!certainlyFull(fuelState)) {
                    fuelState << TANK_FILL;
                } else if (prepTank(fuelState)) {
                    state << ENGINE_PREPPED;
                }
                lastState = ENGINE_PREPFUEL;
                break;
            case ENGINE_PREPPED: // only way out of full is user cmd
                // if(!certainlyFull(oxState)) state << ENGINE_PREPOX;
                // igniter.arm << ARMED;
                if(lastState < ENGINE_PREPFUEL) state << lastState;
                if(!certainlyFull(fuelState)) {
                    // igniter.arm << UNARMED;
                    state << ENGINE_PREPFUEL;
                }
                lastState = ENGINE_PREPPED;
                break;
            case ENGINE_FIRE: // arm igniter
                if(lastState != ENGINE_PREPPED) state << lastState;
                
                // igniter.state << FIRED;
                igniter.state << OPEN; // until squibs are unfucked
                ignitionTime = xTaskGetTickCount();
                state << MAIN_ACTUATION;

                lastState = ENGINE_FIRE;
                break;
            case MAIN_ACTUATION: // empty tanks
                if(lastState != ENGINE_FIRE) state << lastState;

                vTaskDelayUntil(&ignitionTime, FIRE_DELAY);
                oxMain.state << OPEN;
                fuelMain.state << OPEN;

                lastState = MAIN_ACTUATION;
                break;
            default:
                state << lastState;
                break;
        }

        sys.tasks.valvetask._updateValves();
        // sys.tasks.firetask._updateSquibs();
        vTaskDelayUntil(&lastTick, FSM_FREQ);
    }
}

bool EngineFSM::prepTank(EndPoint<TankState>& tankState) {
    if(tankState() == TANK_READY) return true;
    if(tankState() == TANK_FULL) tankState << TANK_BLEED;
    return false;
}

bool EngineFSM::possiblyFull(EndPoint<TankState>& tankState) {
    return tankState() >= TANK_DRAIN || tankState() == TANK_IDLE_PRESS;
}

bool EngineFSM::certainlyFull(EndPoint<TankState>& tankState) {
    return tankState() >= TANK_FULL;
}

bool EngineFSM::safeToApproach(EndPoint<TankState>& tankState) {
    return tankState() == TANK_IDLE_EMPTY || tankState() == TANK_EMPTY;
}