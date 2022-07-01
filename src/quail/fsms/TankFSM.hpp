#pragma once

#include "Task.hpp"
#include "../slate/SlateEndpoints.hpp"
#include "../slate/SlateAbstractions.hpp"
#include "TankGeneric.hpp"


class TankFSM : public TankGeneric {
    public:
        TankFSM(uint8_t priority,
            EndPoint<TankState>& state, 
            EndPoint<float>& opPress,
            SensorSlate& press,
            Solenoid& fillSol,
            Solenoid& ventSol,
            Solenoid& bleedSol);
        void activity() override;

    private:
        bool inBounds() override;
        SensorSlate weight = SensorSlate(NO_QUAIL_ID, NO_QUAIL_ID, NO_QUAIL_ID);
};