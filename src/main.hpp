#pragma once

#include "support/atmel_start.h"
#include "LoggerTask.hpp"
#include "SensorTask.hpp"

class Globals{
    public:
        static SensorTask sensor;
        static LoggerTask logger;
};