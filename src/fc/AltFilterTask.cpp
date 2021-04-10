#include "AltFilterTask.h"

MsgBuffer<SensorData, 1000> AltFilterTask::dataBuffer;

//this might not need to be static...
SensorData AltFilterTask::data;
AltFilter AltFilterTask::filter;
FlightPlan AltFilterTask::plan;

AltFilterTask::AltFilterTask(uint8_t priority) : Task(priority, "AltFilter") {}

void AltFilterTask::queueSensorData(SensorData &data)
{
    dataBuffer.send(data);
}

void AltFilterTask::activity()
{
    plan.dumpConfig();

    for (int i = 0; i < 10; i++)
    {
        dataBuffer.receive(data, true);
    }

    dataBuffer.receive(data, true);
    filter.init(data);

    while (true)
    { //Flight Control Loop: runs every sensor data cycle
        dataBuffer.receive(data, true);
        digitalWrite(4, true);
        filter.update(data);
        plan.update(filter);
        digitalWrite(4, false);
    }
}