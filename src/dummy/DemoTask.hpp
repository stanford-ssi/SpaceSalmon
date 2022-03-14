#pragma once

#include "Task.hpp"

class DemoTask : public Task<4000>{
public:
    DemoTask(uint8_t priority):Task(priority, "Demo"){};
    void activity();
};