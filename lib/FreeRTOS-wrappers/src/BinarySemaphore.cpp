#include "BinarySemaphore.hpp"

BinarySemaphore::BinarySemaphore(){
    handle = xSemaphoreCreateBinaryStatic(&buf);
}

void BinarySemaphore::take(TickType_t block){
    xSemaphoreTake(handle, block);
}

void BinarySemaphore::give(){
    xSemaphoreGive(handle);
}

void BinarySemaphore::giveFromISR(){
    xSemaphoreGiveFromISR(handle,NULL);
}

void BinarySemaphore::takeFromISR(){
    xSemaphoreTakeFromISR(handle,NULL);
}