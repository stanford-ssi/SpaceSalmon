#pragma once

#include "Task.hpp"

class EngineControlTask : public Task<200>
{
public:
    EngineControlTask(uint8_t priority);
    void activity();

private:

};