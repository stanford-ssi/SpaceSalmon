#pragma once

#include "Task.hpp"
#include "../../slate/slate_utils.h"
#include "../../slate/slate_enums.h"
#include "TankFSM.hpp"
#include "../config.h"

class EngineFSM : public Task<500> {
    public:
        EngineFSM(uint8_t priority);
        void activity();

    private:
        const TickType_t FSM_FREQ = 50;
        const TickType_t ACTUATION_PULSE = 100;

        EngineState last_state = ENGINE_ABORT;
        SlateField<EngineState>& state;

        // Igniter& igniter;
        // Solenoid& igniter;
        SlateField<uint32_t>& comms;
        SlateField<bool>& danger;

        SlateField<TankState>& oxdz_state;
        SlateField<bool>& oxdz_main;
        SlateField<uint16_t>& oxdz_main_pulse;

        SlateField<TankState>& fuel_state;
        SlateField<bool>& fuel_main;
        SlateField<uint16_t>& fuel_main_pulse;

        TickType_t lastTick = 0;
        TickType_t ignitionTime = 0;

        bool prepTank(SlateField<TankState>& tank_state);
        bool possiblyFull(SlateField<TankState>& tank_state);
        bool certainlyFull(SlateField<TankState>& tank_state);
        bool safeToApproach(SlateField<TankState>& tank_state);
};