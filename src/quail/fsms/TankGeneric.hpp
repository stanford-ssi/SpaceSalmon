#pragma once

#include "Task.hpp"
#include "../slate/SlateEndpoints.hpp"
#include "../slate/SlateAbstractions.hpp"


class TankGeneric : public Task<500> {
    public:
        TankGeneric(uint8_t priority, EndPoint<TankState>& state, EndPoint<float>& op, 
            SensorSlate& press, SensorSlate& weight, 
            Solenoid& fillSol, Solenoid& ventSol, Solenoid& bleedSol) : 
            Task(priority, "Tank"), state(state), op(op), 
                press(press), weight(weight), 
                fillSol(fillSol), ventSol(ventSol), bleedSol(bleedSol) {};
        virtual void activity() {/** ask my kids **/};

    protected:
        EndPoint<TankState>& state;
        EndPoint<float>& op;
        SensorSlate& press;
        SensorSlate& weight;
        Solenoid& fillSol;
        Solenoid& ventSol;
        Solenoid& bleedSol;

        TickType_t lastTick = 0;

        virtual bool inBounds() {/** ask my kids **/};
        float ZERO_WEIGHT = 0;
        float ZERO_PRESS = 0;
};