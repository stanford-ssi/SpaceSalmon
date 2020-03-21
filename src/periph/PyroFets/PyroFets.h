#pragma once
#include "../Pyro.h"
#include "stdint.h"

class PyroFets : public Pyro {

    public:
        PyroFets(uint8_t fire1, uint8_t sense1, uint8_t fire2, uint8_t sense2);
        void arm();
        void disarm();
        bool fire(PyroChannel channel);
        
        bool getStatus(PyroChannel channel);

    private:
        //armed
        bool armed;
        //pins
        uint8_t fire1_pin;
        uint8_t sense1_pin;
        uint8_t fire2_pin;
        uint8_t sense2_pin;
};