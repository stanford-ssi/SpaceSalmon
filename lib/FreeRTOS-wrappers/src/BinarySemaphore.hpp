#pragma once
#include "FreeRTOS.h"
#include "semphr.h"

class BinarySemaphore{

public:
    BinarySemaphore();
    void take(TickType_t block);
    void give();
    void giveFromISR();
    void takeFromISR();

private:
    StaticSemaphore_t buf;
    xSemaphoreHandle handle;
};