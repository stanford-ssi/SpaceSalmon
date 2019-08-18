#include "AltFilterTask.hpp"

TaskHandle_t AltFilterTask::taskHandle = NULL;
StaticTask_t AltFilterTask::xTaskBuffer;
StackType_t AltFilterTask::xStack[stackSize];

MessageBufferHandle_t AltFilterTask::bufferHandle;
StaticMessageBuffer_t AltFilterTask::messageBufferStruct;
uint8_t AltFilterTask::ucStorageBuffer[bufferSize];

AltFilterTask::AltFilterTask()
{
    AltFilterTask::taskHandle = xTaskCreateStatic(activity,                  //static function to run
                                               "AltFilter",                 //task name
                                               stackSize,                 //stack depth (words!)
                                               NULL,                      //parameters
                                               3,                         //priority
                                               AltFilterTask::xStack,        //stack object
                                               &AltFilterTask::xTaskBuffer); //TCB object

    AltFilterTask::bufferHandle = xMessageBufferCreateStatic(bufferSize, AltFilterTask::ucStorageBuffer, &AltFilterTask::messageBufferStruct);
}

TaskHandle_t AltFilterTask::getTaskHandle()
{
    return taskHandle;
}

void AltFilterTask::newSensorData(SensorData data)
{
    vPortEnterCritical();
    xMessageBufferSend(bufferHandle, &data, sizeof(data), 0);
    vPortExitCritical();
}

void AltFilterTask::activity(void *ptr)
{
    while (true)
        {   //Flight Control Loop: runs every sensor data cycle
            if (xMessageBufferReceive(bufferHandle, &data, sizeof(data), portMAX_DELAY) > 0)
            {
                filter.update(data);
                plan.update(filter);
            }
        }
}