#pragma once
#include "stdint.h"
#include "sam.h"
#include "Mutex.hpp"

class ADC{
    public:
        ADC(Adc* hw_addr);
        uint16_t read(uint8_t pin);
    private:
    Adc* hw;
    Mutex mx;
};