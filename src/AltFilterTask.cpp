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

AltFilterTask::AltFilterTask(uint8_t priority)
{
    AltFilterTask::taskHandle = xTaskCreateStatic(activity,                  //static function to run
                                               "AltFilter",                 //task name
                                               stackSize,                 //stack depth (words!)
                                               NULL,                      //parameters
                                               priority,                         //priority
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
    xMessageBufferReceive(bufferHandle, &data, sizeof(data), portMAX_DELAY);
    filter.init(data);

    while (true)
        {   //Flight Control Loop: runs every sensor data cycle
            xMessageBufferReceive(bufferHandle, &data, sizeof(data), portMAX_DELAY);
            digitalWrite(4,true);
            filter.update(data);
            plan.update(filter);
            digitalWrite(4,false);
        }
}