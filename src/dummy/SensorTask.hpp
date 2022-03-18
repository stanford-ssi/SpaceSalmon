#pragma once

#include "Task.hpp"
#include "SSIEth.hpp"

class SensorTask : public Task<4000>{
public:
    SensorTask(uint8_t priority):Task(priority, "Sensor"){this->suspend();};
    void activity();
};