#include "PyroFets.h"
#include "main.hpp"

PyroFets::PyroFets(uint8_t fire1, uint8_t sense1, uint8_t fire2, uint8_t sense2, ADC &adc) : adc{adc} {
    adc = adc;

    fire1_pin = fire1;
    sense1_pin = sense1;
    fire2_pin = fire2;
    sense2_pin = sense2;

    digitalWrite(fire1_pin, false);
    pinMode(fire1_pin, INPUT);

    digitalWrite(fire2_pin, false);
    pinMode(fire2_pin,INPUT);

    digitalWrite(sense1_pin, false);
    pinMode(sense1_pin,INPUT);

    digitalWrite(sense2_pin, false);
    pinMode(sense2_pin,INPUT);

}

void PyroFets::arm(){
    armed = true;
    digitalWrite(fire1_pin, false);
    digitalWrite(fire2_pin, false);
    pinMode(fire1_pin, OUTPUT);
    pinMode(fire2_pin, OUTPUT);
}

void PyroFets::disarm(){
    armed = false;
    digitalWrite(fire1_pin, false);
    digitalWrite(fire2_pin, false);
    pinMode(fire1_pin, INPUT);
    pinMode(fire2_pin, INPUT);
}

bool PyroFets::fire(PyroChannel channel){
    if(armed){
        if(channel == SquibA){
            digitalWrite(fire2_pin, false);
            digitalWrite(fire1_pin, true);
        }else if(channel == SquibB){
            digitalWrite(fire1_pin, false);
            digitalWrite(fire2_pin, true);
        }else if (channel == SquibNone){
            digitalWrite(fire1_pin, false);
            digitalWrite(fire2_pin, false);
        }
        return true;
    }else{
        return false;
    }
}

bool PyroFets::getStatus(PyroChannel channel){
    uint16_t value = 0;
    if(channel == SquibA){
        value = adc.read(0x0D);
    }else if(channel == SquibB){
        value = adc.read(0x0C);
    }
    return (value > 400);
}