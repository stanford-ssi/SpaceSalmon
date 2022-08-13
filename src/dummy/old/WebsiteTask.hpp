#pragma once

#include "Task.hpp"
#include "SSIEth.hpp"

class WebsiteTask : public Task<4000>{
public:
    WebsiteTask(uint8_t priority):Task(priority, "Sensor"){};
    void activity();
};