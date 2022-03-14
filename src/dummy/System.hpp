#pragma once

class System;

#include <Arduino.h>
#include "DemoTask.hpp"

class System
{
public:

    
    class Tasks
    {
    public:
        DemoTask demo = DemoTask(1);
    };

    Tasks tasks;
};

#include "main.hpp"