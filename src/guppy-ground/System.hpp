#pragma once

class System;

#include <Arduino.h>

#include "LoggerTask.hpp"
#include "RadioTask.hpp"
#include "TXTask.hpp"
#include "RXTask.hpp"

class System
{
public:

    const bool shitl = false;
    const bool silent = true;

    class Tasks
    {
    public:
        LoggerTask logger = LoggerTask(5); //logs to USB/SD
        RadioTask radio = RadioTask(2);    //controls radio
        TXTask test = TXTask(3);       //test
        RXTask rx = RXTask(4);
    };

    Tasks tasks;
};

#include "main.hpp"