#include "StateTask.hpp"

StateTask::StateTask(uint8_t priority) : Task(priority, "State"){};

StaticJsonDocument<1024> StateTask::getState()
{
    return this->state;
};

void StateTask::updateSensorState(sensordata_t sensedata)
{
    this->sensorbuf.send(sensedata);
};

void StateTask::activity()
{
    while(true)
    {
        vTaskDelay(1000); // run through this loop as fast as we can sample and still write to SD
        sensordata_t sensedata;
        while(!this->sensorbuf.empty())
            this->sensorbuf.receive(sensedata, 0); // shouldn't have to wait at all
        // TODO: add logger and put a log call here, passing an encoded state to logger
        // TODO: add a multiplicative function that sets the relative rate of telem downlink, pass encoded state to 
        // telem task on those iterations where mod(iteration, multiplicate factor) = 0
    }
};