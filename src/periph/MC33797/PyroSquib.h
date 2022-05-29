#pragma once
#include "../Pyro.h"
#include "stdint.h"
#include "ssi_adc.h"

class PyroSquib : public Pyro {

    public:
        PyroSquib();
        void arm();
        void disarm();
        bool fire(PyroChannel channel);
        
        bool getStatus(PyroChannel channel);

    private:
        //armed
        bool armed;
};