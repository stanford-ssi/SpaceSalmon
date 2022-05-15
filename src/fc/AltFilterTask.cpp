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

        // should we collapse data.adxl1_data.y to only 9.8 or -9.8 before putting through ema?
        ema_accel = ema_alpha*data.adxl1_data.y + (1-ema_alpha)*ema_accel;
        
        // whats the right way to access 'armed'? sys.armed.get(.)? or just use (bool)sys.armed?
        // also whats the best condition. Armed? Waiting state? Armed and Waiting state?
        if ((bool)sys.armed == false) {
            if (ema_accel > 0) {
                negate_accel.post(false);
            } else {
                negate_accel.post(true);
            }
        }

        filter.update(data);
        plan.update(filter);
        digitalWrite(4, false);
    }
}