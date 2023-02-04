#include "EngineFSM.hpp"
#include "main.hpp"


EngineFSM::EngineFSM(uint8_t priority) : Task(priority, "Engine"), 
    state(sys.telem_slate.engn_state),
    // igniter(sys.slate.squib[0]),
    // igniter(sys.slate.valves[11]),
    comms(sys.telem_slate.watchdog_counter),
    danger(sys.telem_slate.danger),
    oxdz_state(sys.telem_slate.oxdz_state),
    oxdz_main(sys.telem_slate.oxdz_main),
    oxdz_main_pulse(sys.telem_slate.oxdz_main_pulse),
    fuel_state(sys.telem_slate.fuel_state),
    fuel_main(sys.telem_slate.fuel_main),
    fuel_main_pulse(sys.telem_slate.fuel_main_pulse) {};

void EngineFSM::activity() {
    lastTick = xTaskGetTickCount();

    while (true) {
        if(!comms()) state << ENGINE_ABORT;

        switch(state()) {
            case ENGINE_ABORT: // drain tanks and reset quail
                danger << true;
                if(possiblyFull(oxdz_state)) {
                    oxdz_state << TANK_DRAIN;
                } else  if (possiblyFull(fuel_state)) {
                    fuel_state << TANK_DRAIN;
                } else {
                    danger << false;
                    NVIC_SystemReset();
                }
                last_state = ENGINE_ABORT;
                break;
            case ENGINE_IDLE: // only way to get out of idle is through user command
                if(safeToApproach(oxdz_state) && safeToApproach(fuel_state)){
                    danger << false;
                } else {
                    danger << true;
                }
                last_state = ENGINE_IDLE;
                break;
            case ENGINE_PREPOX: // if ox tank is not full, fill
                danger << true;
                if (!certainlyFull(oxdz_state)) {
                    oxdz_state << TANK_FILL;
                } 
                else if(prepTank(oxdz_state)){
                    state << ENGINE_OXPREPPED;
                } 
                last_state = ENGINE_PREPOX;
                break;
            case ENGINE_OXPREPPED:
                if(last_state < ENGINE_PREPOX) state << last_state;
                if(!certainlyFull(oxdz_state)) state << ENGINE_PREPOX;
                else prepTank(oxdz_state);
                last_state = ENGINE_OXPREPPED;
                break;
            case ENGINE_PREPFUEL:
                if(last_state < ENGINE_OXPREPPED) state << last_state;
                if(!certainlyFull(fuel_state)) {
                    fuel_state << TANK_FILL;
                } else if (prepTank(fuel_state)) {
                    state << ENGINE_PREPPED;
                }
                last_state = ENGINE_PREPFUEL;
                break;
            case ENGINE_PREPPED: // only way out of full is user cmd
                if(!certainlyFull(oxdz_state)) state << ENGINE_PREPOX;
                // igniter.arm << ARMED;
                if(last_state < ENGINE_PREPFUEL) state << last_state;
                if(!certainlyFull(fuel_state)) {
                    // igniter.arm << UNARMED;
                    state << ENGINE_PREPFUEL;
                }
                last_state = ENGINE_PREPPED;
                break;
            case ENGINE_FIRE: // arm igniter
                if(last_state != ENGINE_PREPPED) state << last_state;
                
                // igniter.state << FIRED;
                // igniter.time << FSM_PTM; // until squibs are unfucked
                ignitionTime = xTaskGetTickCount();
                state << MAIN_ACTUATION;

                last_state = ENGINE_FIRE;
                break;
            case MAIN_ACTUATION: // empty tanks
                if(last_state != ENGINE_FIRE) state << last_state;

                vTaskDelayUntil(&ignitionTime, FIRE_DELAY);
                oxdz_main_pulse << ACTUATION_PULSE;
                fuel_main_pulse << ACTUATION_PULSE;

                last_state = MAIN_ACTUATION;
                break;
            default:
                state << last_state;
                break;
        }

        vTaskDelayUntil(&lastTick, FSM_FREQ);
    }
}

bool EngineFSM::prepTank(SlateField<TankState>& tankState) {
    if(tankState() == TANK_READY) return true;
    if(tankState() == TANK_FULL) tankState << TANK_BLEED;
    return false;
}

bool EngineFSM::possiblyFull(SlateField<TankState>& tankState) {
    return tankState() >= TANK_DRAIN || tankState() == TANK_IDLE_PRESS;
}

bool EngineFSM::certainlyFull(SlateField<TankState>& tankState) {
    vTaskDelay(CERTAINLY_FULL_DELAY);
    return tankState() >= TANK_FULL;
}

bool EngineFSM::safeToApproach(SlateField<TankState>& tankState) {
    return tankState() == TANK_IDLE_EMPTY || tankState() == TANK_EMPTY;
}