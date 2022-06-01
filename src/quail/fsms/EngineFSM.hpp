#pragma once

#include "Task.hpp"
#include "../slate/SlateEndpoints.hpp"
#include "../slate/SlateAbstractions.hpp"

class EngineFSM : public Task<500> {
    public:
        EngineFSM(uint8_t priority);
        void activity();

    private:
        EngineState lastState = ENGINE_ABORT;

        EndPoint<EngineState>& state;
        // Igniter& igniter;
        Solenoid& igniter;
        EndPoint<bool>& comms;
        Solenoid& safe;

        EndPoint<TankState>& oxState;
        Solenoid& oxMain;

        EndPoint<TankState>& fuelState;
        Solenoid& fuelMain;

        TickType_t lastTick = 0;
        TickType_t ignitionTime = 0;

        bool prepTank(EndPoint<TankState>& tankState);
        bool possiblyFull(EndPoint<TankState>& tankState);
        bool certainlyFull(EndPoint<TankState>& tankState);
        bool safeToApproach(EndPoint<TankState>& tankState);
};