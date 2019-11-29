#include "AltFilterTask.h"

TaskHandle_t AltFilterTask::taskHandle = NULL;
StaticTask_t AltFilterTask::xTaskBuffer;
StackType_t AltFilterTask::xStack[stackSize];

MessageBufferHandle_t AltFilterTask::bufferHandle;
StaticMessageBuffer_t AltFilterTask::messageBufferStruct;
uint8_t AltFilterTask::ucStorageBuffer[bufferSize];

//this might not need to be static...
SensorData AltFilterTask::data;
AltFilter AltFilterTask::filter;
FlightPlan AltFilterTask::plan;

AltFilterTask::AltFilterTask()
{
    AltFilterTask::taskHandle = xTaskCreateStatic(activity,                  //static function to run
                                               "AltFilter",                 //task name
                                               stackSize,                 //stack depth (words!)
                                               NULL,                      //parameters
                                               6,                         //priority
                                               AltFilterTask::xStack,        //stack object
                                               &AltFilterTask::xTaskBuffer); //TCB object

    AltFilterTask::bufferHandle = xMessageBufferCreateStatic(bufferSize, AltFilterTask::ucStorageBuffer, &AltFilterTask::messageBufferStruct);
}

TaskHandle_t AltFilterTask::getTaskHandle()
{
    return taskHandle;
}

void AltFilterTask::queueSensorData(SensorData data)
{
    vPortEnterCritical();
    xMessageBufferSend(bufferHandle, &data, sizeof(data), 0);
    vPortExitCritical();
}

void AltFilterTask::activity(void *ptr)
{
    plan.dumpConfig();
    while (true)
        {   //Flight Control Loop: runs every sensor data cycle
            if (xMessageBufferReceive(bufferHandle, &data, sizeof(data), portMAX_DELAY) > 0)
            {
                gpio_set_pin_level(LED4,true);
                filter.update(data);
                plan.update(filter);
                gpio_set_pin_level(LED4,false);
            }
        }
}