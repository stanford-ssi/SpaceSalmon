#pragma once

#include "Task.hpp"
#include "../slate/SlateEndpoints.hpp"
#include "../slate/SlateAbstractions.hpp"

class EngineFSM : public Task<500> {
    public:
        EngineFSM(uint8_t priority);
        void activity();

    private:
        EndPoint<EngineState>& state;
        Igniter& igniter;
        EndPoint<bool>& comms;
        Solenoid& safe;

        EndPoint<TankState>& oxState;
        Solenoid& oxMain;

        EndPoint<TankState>& fuelState;
        Solenoid& fuelMain;

        TickType_t lastTick = 0;

        void prepTank(EndPoint<TankState>& tankState);
};