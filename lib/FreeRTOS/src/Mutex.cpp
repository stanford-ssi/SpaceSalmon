#include "Mutex.hpp"

Mutex::Mutex(){
    handle = xSemaphoreCreateMutexStatic(&buf);
}

void Mutex::take(TickType_t block){
    xSemaphoreTake(handle, block);
}

void Mutex::give(){
    xSemaphoreGive(handle);
}