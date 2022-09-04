#pragma once

class System;

#include <Arduino.h>
// #include "SensorTask.hpp"
// #include "CommandTask.hpp"
// #include "WebsiteTask.hpp"
#include "ethTest.hpp"
#include "generated/telemetry.h"

class System
{
public:
    class Tasks
    {
    public:
        SSIEth ethernet = SSIEth(1,MacAddr({0x00, 0x00, 0x00, 0x00, 0x20, 0x76}));
        // SensorTask sensor_demo = SensorTask(1);
        // CommandTask command_demo = CommandTask(2);
        // WebsiteTask website_demo = WebsiteTask(2);
        EthTestTask task = EthTestTask(2);
        telemetry_t telem;
    };

    Tasks tasks;
};

#include "main.hpp"