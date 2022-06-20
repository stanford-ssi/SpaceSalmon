#pragma once
#include "../Pyro.h"
#include "stdint.h"
#include "ssi_adc.h"
#include "Squib.hpp"

class PyroSquib : public Pyro {

    public:
        PyroSquib(SPIClass &spi, int8_t cspin);
        void init();
        void arm();
        void disarm();
        bool fire(PyroChannel channel);
        
        bool getStatus(PyroChannel channel);
        void getResistance();

    private:
        Squib squib;
        Mutex squib_mx;
        //armed
        bool armed;
};