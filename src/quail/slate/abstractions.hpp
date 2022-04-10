#pragma once

#include "Container.hpp"
#include "SlateKey.hpp"
#include "Array.hpp"
#include "endpoints.hpp"
#include "../config.h"
#include <string>

class SensorSlate : public Container<4> {
    public:
        EndSensor val;
        EndDerived avg;
        EndDerived dif;
        EndDerived ntg;
        SensorSlate(const std::string id, const std::string quailID) : Container(id, {
            std::ref(val),
            std::ref(avg),
            std::ref(dif),
            std::ref(ntg)
        }), quailID(quailID) {
            val = EndSensor("Raw", quailID, 0.0);
            avg = EndDerived("Average", 0.0);
            dif = EndDerived("Derivative", 0.0);
            ntg = EndDerived("Integral", 0.0);
        };

    private:
        std::string quailID;
};

class Igniter : public Container<2> {
    public:
        EndActuator<ematch_arm_state_t> arm;
        EndActuator<ematch_fire_state_t> state;
        Igniter(const std::string id, const std::string quailID) : Container(id, {
            std::ref(arm),
            std::ref(state)
        }), quailID(quailID) {
            arm = EndActuator<ematch_arm_state_t>("Armed", quailID);
            state = EndActuator<ematch_fire_state_t>("Fired", quailID);
        };

    private:
        std::string quailID;
};

class Solenoid : public Container<3> {
    public: 
        EndPoint<solenoid_normal_t> normal;
        EndPoint<solenoid_pwm_t> pwm;
        EndPoint<uint16_t> pulse;
        EndActuator<solenoid_state_t> state;
        Solenoid(const std::string id, const std::string quailID) : Container(id, {
            std::ref(normal),
            std::ref(pwm),
            std::ref(state)
        }), quailID(quailID) {
            normal = EndPoint<solenoid_normal_t>("Normally", quailID, NORMALLY_CLOSED, false);
            pwm = EndPoint<solenoid_pwm_t>("PWM", quailID, MEDIUM, false);
            pulse = EndPoint<uint16_t>("Pulse Time", quailID, 1, true); // 0 is not a valid timer period
            state = EndActuator<solenoid_state_t>("Open", quailID);
        };

    private:
        std::string quailID;
};