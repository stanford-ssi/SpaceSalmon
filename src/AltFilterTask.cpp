#include "AltFilterTask.h"

TaskHandle_t AltFilterTask::taskHandle = NULL;
StaticTask_t AltFilterTask::xTaskBuffer;
StackType_t AltFilterTask::xStack[stackSize];

MsgBuffer<SensorData, 1000> AltFilterTask::dataBuffer;

//this might not need to be static...
SensorData AltFilterTask::data;
AltFilter AltFilterTask::filter;
FlightPlan AltFilterTask::plan;

AltFilterTask::AltFilterTask(uint8_t priority)
{
    AltFilterTask::taskHandle = xTaskCreateStatic(activity,                     //static function to run
                                                  "AltFilter",                  //task name
                                                  stackSize,                    //stack depth (words!)
                                                  NULL,                         //parameters
                                                  priority,                     //priority
                                                  AltFilterTask::xStack,        //stack object
                                                  &AltFilterTask::xTaskBuffer); //TCB object
}

TaskHandle_t AltFilterTask::getTaskHandle()
{
    return taskHandle;
}

void AltFilterTask::queueSensorData(SensorData &data)
{
    dataBuffer.send(data);
}

void AltFilterTask::activity(void *ptr)
{
    plan.dumpConfig();
    dataBuffer.receive(data);
    filter.init(data);

    while (true)
    { //Flight Control Loop: runs every sensor data cycle
        dataBuffer.receive(data);
        digitalWrite(4, true);
        filter.update(data);
        plan.update(filter);
        digitalWrite(4, false);
    }
}