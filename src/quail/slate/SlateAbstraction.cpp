#include "main.hpp"
#include "SlateAbstractions.hpp"

SensorSlate::SensorSlate(const std::string id, const std::string quailID, const std::string desc) : Container(id, {
    std::ref(val),
    std::ref(avg),
    // std::ref(drv),
    // std::ref(ntg)
    std::ref(cal),
    std::ref(ofs)
}), quailID(quailID) {
    val = EndSensor("raw", quailID, 0.0, desc);
    avg = EndDerived("avg", 0.0);
    // drv = EndDerived("drv", 0.0);
    // ntg = EndDerived("ntg", 0.0);

    // cal = EndDerived("cal", 0.0);
    // ofs = EndDerived("ofs", 0.0);

    // Ideally both would be editable
    // you could write zero to the calibrated one to calibrate it to that value
    // or write to the offset one to set a given offset
    cal = EndPoint<float>("cal", quailID, 0.0, false, desc + " Calibrated");
    ofs = EndPoint<float>("ofs", NO_QUAIL_ID, 0.0, true, "ofset");
};

SensorSlate& SensorSlate::operator<<(const float in) {
    window += in;
    val << window.peek();
    avg << window.avg();
    // drv << window.delta();
    // ntg << ntg() + avg() * window.dt();
    cal << val.get() + ofs.get();

    // this shouldn't be happening every looop but corners need to be cut
    cal.unit = val.unit;

    return *this;
};

float SensorSlate::operator()() { return avg(); }


Igniter::Igniter(const std::string id, const std::string quailID, const std::string desc) : Container(id, {
    std::ref(arm),
    std::ref(state)
}), quailID(quailID) {
    arm = EndActuator<ematch_arm_state_t>("arm", quailID);
    state = EndActuator<ematch_fire_state_t>("fir", quailID, desc);
};

Igniter& Igniter::operator<<(const JsonVariant src) {
    Container::operator<<(src);
    sys.tasks.firetask._updateSquibs();
    return *this;
}


PulseEndpoint::PulseEndpoint(const std::string quailID, uint8_t index, const std::string desc) : index(index), EndPoint("ptm", quailID, 1, true, desc) { this->unit = "ms"; };

PulseEndpoint::PulseEndpoint() : PulseEndpoint(NO_QUAIL_ID, 0, "none") { this->unit = "ms"; };

PulseEndpoint& PulseEndpoint::operator<<(const JsonVariant src) {
    EndPoint::operator<<(src);
    if (src.containsKey(this->id)) { sys.tasks.valvetask._updatePulse(index); }
    return *this;
};

PulseEndpoint& PulseEndpoint::operator<<(const uint16_t in) {
    this->operator<<(in);
    return *this;
};


Solenoid::Solenoid(const std::string id, const std::string quailID, uint8_t index, const std::string desc) : Container(id, {
    std::ref(normal),
    std::ref(pwm),
    std::ref(time),
    std::ref(state)
}), quailID(quailID), index(index) {
    normal = EndPoint<solenoid_normal_t>("nor", quailID, NORMALLY_CLOSED, false);
    pwm = EndPoint<solenoid_pwm_t>("pwm", quailID, MEDIUM, false);
    time = PulseEndpoint(quailID, index, desc);
    state = EndActuator<solenoid_state_t>("stt", quailID, desc);
};

Solenoid& Solenoid::operator<<(const JsonVariant src) {
    Container::operator<<(src);
    if (src.containsKey(this->id)) { sys.tasks.valvetask._updateValves(index); }
    return *this;
}