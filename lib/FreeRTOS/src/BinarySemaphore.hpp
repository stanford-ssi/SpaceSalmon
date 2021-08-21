#pragma once
#include "hal_rtos.h"

class BinarySemaphore{

public:
    BinarySemaphore();
    void take(TickType_t block);
    void give();
    void giveFromISR();
    void takeFromISR();

private:
    StaticSemaphore_t buf;
    SemaphoreHandle_t handle;
};