#pragma once

#include "Task.hpp"
#include "../slate/SlateEndpoints.hpp"
#include "../slate/SlateAbstractions.hpp"

class OxFSM : public Task<500> {
    public:
        OxFSM(uint8_t priority);
        void activity();

    private:
        EndPoint<TankState>& state;
        EndPoint<float>& oxOp;
        SensorSlate& oxPressure;
        Solenoid& oxFill;
        Solenoid& oxVent;
        Solenoid& oxBleed;

        TickType_t lastTick = 0;
};