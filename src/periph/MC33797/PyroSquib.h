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

    private:
        Squib squib;
        //armed
        bool armed;
};