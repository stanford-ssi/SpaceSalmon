#pragma once
#include "RadioLib.h"

/*
SX1262S
This is a wrapper around the RadioLib SX1262 radio driver. I removed some of the calls to configure and reset the IRQs.\
This allows for better integration with the SSI S6M library, which has a FreeRTOS thread dedicated to handling all radio interupts.
By: Tim Vrakas, March 2020
*/

class SX1262S : public SX1262
{
public:
    SX1262S(Module* mod);
    int16_t startTransmit(uint8_t* data, size_t len, uint8_t addr = 0);
    int16_t startReceiveCommon();
    int16_t startReceive(uint32_t timeout = SX126X_RX_TIMEOUT_INF);
    uint32_t symbolToMs(uint32_t symbols);
};