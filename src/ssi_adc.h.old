#pragma once
#include "stdint.h"
#include "driver_init.h"
#include "hpl_adc_sync.h"

class ADC{
    public:
        ADC(Adc* hw_addr);
        uint16_t read(uint8_t pin);
    private:
    Adc* hw;
    mutex_buf_t mutex_buf;
    mutex_handle_t mutex_handle;
};