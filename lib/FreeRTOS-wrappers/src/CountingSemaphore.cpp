#include "CountingSemaphore.hpp"

CountingSemaphore::CountingSemaphore(uint32_t maxCount, uint32_t initCount){
    handle = xSemaphoreCreateCountingStatic(maxCount, initCount, &buf);
}

void CountingSemaphore::take(TickType_t block){
    xSemaphoreTake(handle, block);
}

void CountingSemaphore::give(){
    xSemaphoreGive(handle);
}

void CountingSemaphore::giveFromISR(int32_t &taskWoken){
    xSemaphoreGiveFromISR(handle,&taskWoken);
}

void CountingSemaphore::takeFromISR(){
    xSemaphoreTakeFromISR(handle,NULL);
}