#pragma once

#include "Container.hpp"
#include "SlateKey.hpp"
#include "Array.hpp"
#include "SlateEndpoints.hpp"
#include "window.hpp"
#include "../config.h"
#include <string>

class SensorSlate : public Container<4> {
    public:
        EndSensor val;
        EndDerived avg;
        EndDerived drv;
        EndDerived ntg;

        SensorSlate(const std::string, const std::string quailID);
        SensorSlate &operator<<(const float in);

    private:
        std::string quailID;
        Window window;
};

class Igniter : public Container<2> {
    public:
        EndActuator<ematch_arm_state_t> arm;
        EndActuator<ematch_fire_state_t> state;

        Igniter(const std::string id, const std::string quailID);
        Igniter &operator<<(const JsonVariant src) override;

    private:
        std::string quailID;
};

class Solenoid : public Container<4> {
    public: 
        EndPoint<solenoid_normal_t> normal;
        EndPoint<solenoid_pwm_t> pwm;
        EndPoint<uint16_t> time;
        EndActuator<bool> pulse;
        EndActuator<solenoid_state_t> state;

        Solenoid(const std::string id, const std::string quailID);
        Solenoid& operator<<(const JsonVariant src) override;

    private:
        std::string quailID;
};