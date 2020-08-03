#pragma once
#include "hal_rtos.h"

class Semaphore{

public:
    Semaphore();
    void take(TickType_t block);
    void give();
    void giveFromISR();

private:
    StaticSemaphore_t buf;
    SemaphoreHandle_t handle;
};