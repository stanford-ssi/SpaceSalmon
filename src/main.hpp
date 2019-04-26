#pragma once

#include "support/atmel_start.h"
#include "LoggerTask.hpp"
#include "SensorTask.hpp"
#include "SquibTask.hpp"
#include "AltimeterTask.hpp"

class Globals{
    public:
        static SensorTask sensor;
        static LoggerTask logger;
        static SquibTask squib; //this should not be a task...
        static AltimeterTask altimeter;
};