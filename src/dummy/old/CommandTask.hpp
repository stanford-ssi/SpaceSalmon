#pragma once

#include "Task.hpp"
#include "SSIEth.hpp"

class CommandTask : public Task<4000>{
public:
    CommandTask(uint8_t priority):Task(priority, "Command"){};
    void activity();
};