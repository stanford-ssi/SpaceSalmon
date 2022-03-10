#pragma once

#include "SlateKey.hpp"

class DerivedSensor{
public:
    DerivedSensor(const char* ch_name, SlateKey<float> &ref_channelA, SlateKey<float> &ref_channelB, SlateKey<float> &slate_channel) :
        ch_name(ch_name), slate_channel(slate_channel), ref_channelA(ref_channelA), ref_channelB(ref_channelB){ num_derived++; };
    
    virtual void update(){};
    static uint8_t num_derived;
protected:
    const char* ch_name; // data channel name
    SlateKey<float> slate_channel; // slate channel
    SlateKey<float> &ref_channelA;
    SlateKey<float> &ref_channelB;
};

class SensorSum: public DerivedSensor{
public:
    SensorSum(const char* ch_name, SlateKey<float> &ref_channelA, SlateKey<float> &ref_channelB, SlateKey<float> &slate_channel):
    DerivedSensor(ch_name, ref_channelA, ref_channelB, slate_channel){};

    void update() override{
        this->slate_channel = this->ref_channelA.get() + this->ref_channelB.get();
    };
};

class SensorMultiply: public DerivedSensor{
public:
    SensorMultiply(const char* ch_name, SlateKey<float> &ref_channelA, SlateKey<float> &ref_channelB, SlateKey<float> &slate_channel):
    DerivedSensor(ch_name, ref_channelA, ref_channelB, slate_channel){};

    void update() override{
        this->slate_channel = this->ref_channelA.get() * this->ref_channelB.get();
    };
};

class SensorSubtract: public DerivedSensor{
public:
    SensorSubtract(const char* ch_name, SlateKey<float> &ref_channelA, SlateKey<float> &ref_channelB, SlateKey<float> &slate_channel):
    DerivedSensor(ch_name, ref_channelA, ref_channelB, slate_channel){};

    void update() override{
        this->slate_channel = this->ref_channelA.get() - this->ref_channelB.get();
    };
};

class SensorDivide: public DerivedSensor{
public:
    SensorDivide(const char* ch_name, SlateKey<float> &ref_channelA, SlateKey<float> &ref_channelB, SlateKey<float> &slate_channel):
    DerivedSensor(ch_name, ref_channelA, ref_channelB, slate_channel){};

    void update() override{
        this->slate_channel = this->ref_channelA.get()/this->ref_channelB.get();
    };
};