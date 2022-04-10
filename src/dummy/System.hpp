#pragma once

class System;

#include <Arduino.h>
<<<<<<< HEAD

#include "TestTask.hpp"
#include "Slate.hpp"
=======
#include "SensorTask.hpp"
#include "CommandTask.hpp"
#include "WebsiteTask.hpp"
>>>>>>> origin/ethernet

class System
{
public:
    class Tasks
    {
    public:
        SSIEth ethernet = SSIEth(1);
        SensorTask sensor_demo = SensorTask(1);
        CommandTask command_demo = CommandTask(2);
        WebsiteTask website_demo = WebsiteTask(2);
    };

    Tasks tasks;
};

#include "main.hpp"