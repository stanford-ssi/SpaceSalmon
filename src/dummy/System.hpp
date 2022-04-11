#pragma once

class System;

#include <Arduino.h>

#include "TestTask.hpp"
#include "Slate.hpp"

class System
{
public:

    Slate slate = Slate("quail");
    
    class Tasks
    {
    public:
        TestTask rx = TestTask(1);
    };

    Tasks tasks;
};

#include "main.hpp"