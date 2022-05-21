#pragma once

#include "Task.hpp"
#include "../slate/SlateEndpoints.hpp"
#include "../slate/SlateAbstractions.hpp"


class TankFSM : public Task<500> {
    public:
        TankFSM(uint8_t priority);
        TankFSM(uint8_t priority,
            EndPoint<TankState>& state, 
            EndPoint<float>& opPress,
            SensorSlate& press,
            Solenoid& fillSol,
            Solenoid& ventSol,
            Solenoid& bleedSol);
        void activity();

    private:
        EndPoint<TankState>& state;
        EndPoint<float>& opPress;
        SensorSlate& press;
        Solenoid& fillSol;
        Solenoid& ventSol;
        Solenoid& bleedSol;

        TickType_t lastTick = 0;
};