#pragma once
#include "FreeRTOS.h"
#include "semphr.h"

class CountingSemaphore{

public:
    CountingSemaphore(uint32_t maxCount, uint32_t initCount);
    void take(TickType_t block);
    void give();
    void giveFromISR(int32_t &taskWoken);
    void takeFromISR();

private:
    StaticSemaphore_t buf;
    xSemaphoreHandle handle;
};