#pragma once
#include "stdint.h"
#include "samd.h"
#include "hri/hri_d51.h"
#include "hal/include/hpl_adc_sync.h"
#include "Mutex.hpp"

class ADC{
    public:
        ADC(Adc* hw_addr);
        uint16_t read(uint8_t pin);
    private:
    Adc* hw;
    Mutex mx;
};