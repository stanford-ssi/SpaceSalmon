#pragma once
#include "hal_rtos.h"

class Mutex{

public:
    Mutex();
    void take(TickType_t block);
    void give();

    Mutex(const Mutex&); //custom copy
	Mutex& operator=(const Mutex&) = delete; //assignment does not make sense

    Mutex(Mutex&&) = delete; //no such thing as move
    Mutex& operator=(Mutex&&) = delete; //assignment does not make sense
    
    ~Mutex();

private:
    StaticSemaphore_t buf;
    SemaphoreHandle_t handle;
};