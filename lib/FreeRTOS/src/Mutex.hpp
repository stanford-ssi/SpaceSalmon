#pragma once
#include "hal_rtos.h"

class Mutex{

public:
    Mutex();
    void take(TickType_t block);
    void give();
    Mutex(const Mutex&) = delete; 
	Mutex& operator=(const Mutex&) = delete;
    ~Mutex();

private:
    StaticSemaphore_t buf;
    SemaphoreHandle_t handle;
};