#pragma once

#include "Task.hpp"
#include "SSIEth.hpp"

class EthTestTask : public Task<4000>{
public:
    EthTestTask(uint8_t priority):Task(priority, "EthTest"){};
    void activity();
};