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
        EndPoint<TankState>& oxState;
        Solenoid& oxMain;

        TickType_t lastTick = 0;
};