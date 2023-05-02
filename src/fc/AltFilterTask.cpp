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
    sys.pyro.init();
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
        if (sys.shitl) {
            // Serial.println("in shitl");
            StaticJsonDocument<500> json;
            json["tick"] = 0; 
            JsonArray x_json = json.createNestedArray("alt");
            x_json.add(filter.getAltitude());
            JsonArray z_json = json.createNestedArray("vel");
            z_json.add(filter.getVelocity());       
            sys.tasks.logger.logJSON(json, "altitude");
        }
        digitalWrite(4, false);
    }
}