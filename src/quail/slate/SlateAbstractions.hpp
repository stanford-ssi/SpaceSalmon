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

class PulseEndpoint : public EndPoint<uint16_t> {
    public:
        PulseEndpoint(const std::string quailID, uint8_t index);
        PulseEndpoint();

        PulseEndpoint& operator<<(const JsonVariant src) override;

        PulseEndpoint& operator<<(uint16_t in);

    private:
        uint8_t index;
};

class Solenoid : public Container<4> {
    public: 
        EndPoint<solenoid_normal_t> normal;
        EndPoint<solenoid_pwm_t> pwm;
        PulseEndpoint time;
        EndActuator<solenoid_state_t> state;

        Solenoid(const std::string id, const std::string quailID, uint8_t index);
        Solenoid& operator<<(const JsonVariant src) override;

    private:
        std::string quailID;
        uint8_t index;
};