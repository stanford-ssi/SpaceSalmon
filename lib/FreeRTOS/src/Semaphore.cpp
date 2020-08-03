#include "Semaphore.hpp"

Semaphore::Semaphore(){
    handle = xSemaphoreCreateBinaryStatic(&buf);
}

void Semaphore::take(TickType_t block){
    xSemaphoreTake(handle, block);
}

void Semaphore::give(){
    xSemaphoreGive(handle);
}

void Semaphore::giveFromISR(){
    BaseType_t needToSwitch;
    xSemaphoreGiveFromISR(handle,&needToSwitch);
    portYIELD_FROM_ISR(needToSwitch);
}
