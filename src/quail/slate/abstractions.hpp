#pragma once

#include "Container.hpp"
#include "SlateKey.hpp"
#include "Array.hpp"
#include "endpoints.hpp"
#include "../config.h"

class SensorSlate : public Container<4> {
    public:
        EndSensor val = EndSensor("Raw", quailID, 0.0);
        EndDerived avg = EndDerived("Average", 0.0);
        EndDerived dif = EndDerived("Derivative", 0.0);
        EndDerived ntg = EndDerived("Integral", 0.0);
        SensorSlate(const std::string id, const std::string quailID) : Container(id, {
            std::ref(val),
            std::ref(avg),
            std::ref(dif),
            std::ref(ntg)
        }), quailID(quailID) {};

    private:
        const std::string quailID;
};

class Igniter : public Container<2> {
    public:
        EndActuator<ematch_arm_state_t> arm = EndActuator<ematch_arm_state_t>("Armed", quailID);
        EndActuator<ematch_fire_state_t> state = EndActuator<ematch_fire_state_t>("Fired", quailID);
        Igniter(const std::string id, const std::string quailID) : Container(id, {
            std::ref(arm),
            std::ref(state)
        }) {};

    private:
        const std::string quailID;
};

class Solenoid : public Container<3> {
    public:
        EndPoint<solenoid_normal_t> normal = EndPoint<solenoid_normal_t>("Normally", quailID, NORMALLY_CLOSED, false);
        EndPoint<solenoid_pwm_t> pwm = EndPoint<solenoid_pwm_t>("PWM", quailID, MEDIUM, false);
        EndActuator<uint16_t> pulse = EndActuator<uint16_t>("Pulse Time", quailID);
        EndActuator<solenoid_state_t> state = EndActuator<solenoid_state_t>("Open", quailID);
        Solenoid(const std::string id, const std::string quailID) : Container(id, {
            std::ref(normal),
            std::ref(pwm),
            std::ref(state)
        }) {};

    private:
        const std::string quailID;
};