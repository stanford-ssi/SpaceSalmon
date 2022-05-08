#include "main.hpp"
#include "SlateAbstractions.hpp"

SensorSlate::SensorSlate(const std::string id, const std::string quailID) : Container(id, {
    std::ref(val),
    std::ref(avg),
    std::ref(drv),
    std::ref(ntg)
}), quailID(quailID) {
    val = EndSensor("raw", quailID, 0.0);
    avg = EndDerived("avg", 0.0);
    drv = EndDerived("drv", 0.0);
    ntg = EndDerived("ntg", 0.0);
};

SensorSlate& SensorSlate::operator<<(const float in) {
    window += in;
    val << window.peek();
    avg << window.avg();
    drv << window.delta();
    ntg << ntg() + avg() * window.dt();
    return *this;
};

float SensorSlate::operator()() { return avg(); }


Igniter::Igniter(const std::string id, const std::string quailID) : Container(id, {
    std::ref(arm),
    std::ref(state)
}), quailID(quailID) {
    arm = EndActuator<ematch_arm_state_t>("arm", quailID);
    state = EndActuator<ematch_fire_state_t>("fir", quailID);
};

Igniter& Igniter::operator<<(const JsonVariant src) {
    Container::operator<<(src);
    sys.tasks.firetask._updateSquibs();
    return *this;
}


PulseEndpoint::PulseEndpoint(const std::string quailID, uint8_t index) : index(index), EndPoint("ptm", quailID, 1, true) {};

PulseEndpoint::PulseEndpoint() : PulseEndpoint(NO_QUAIL_ID, 0) {};

PulseEndpoint& PulseEndpoint::operator<<(const JsonVariant src) {
    EndPoint::operator<<(src);
    if (src.containsKey(this->id)) { sys.tasks.valvetask._updatePulse(index); }
    return *this;
};

PulseEndpoint& PulseEndpoint::operator<<(const uint16_t in) {
    this->operator<<(in);
    return *this;
};


Solenoid::Solenoid(const std::string id, const std::string quailID, uint8_t index) : Container(id, {
    std::ref(normal),
    std::ref(pwm),
    std::ref(time),
    std::ref(state)
}), quailID(quailID), index(index) {
    normal = EndPoint<solenoid_normal_t>("nor", quailID, NORMALLY_CLOSED, false);
    pwm = EndPoint<solenoid_pwm_t>("pwm", quailID, MEDIUM, false);
    time = PulseEndpoint(quailID, index);
    state = EndActuator<solenoid_state_t>("stt", quailID);
};

Solenoid& Solenoid::operator<<(const JsonVariant src) {
    Container::operator<<(src);
    if (src.containsKey(this->id)) { sys.tasks.valvetask._updateValves(index); }
    return *this;
}