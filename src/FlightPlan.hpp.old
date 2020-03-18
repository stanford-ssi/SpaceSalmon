#pragma once
#include "periph/Pyro.h"
#include <hal_rtos.h>

class FlightPlan;

typedef enum {
    Waiting,    //On pad wating for Launch
    Flight,     //Flying with Positive Velocity
    Falling,    //Falling with Negative Velocity
    Landed      //Landed
} FlightState;

typedef enum {
    VelNone,
    VelLess, //NEGATIVE VELOCITY is DOWN!
    VelMore  //POSITIVE VELOCITY is UP!
} VelocityCondition;

typedef enum {
    AltNone,
    AltLess,
    AltMore
} AltCondition;

typedef enum {
    BlowSquib
} EventAction;

typedef Pyro::PyroChannel SquibChannel;

typedef struct {
    FlightState state;
    VelocityCondition velCond;
    float velocity; //meters/sec
    AltCondition altCond;
    float altitude; //meters AGL
    EventAction action;
    SquibChannel squib;
    TickType_t time; //time to fire for (min)
} FlightEvent;

static const FlightEvent eventList[] = {{Falling,   VelLess,    0.0,      AltNone,    0.0,      BlowSquib,  Pyro::PyroChannel::SquibA,  2000    },  //Apogee Event, at velocity 0-crossing
                                        {Falling,   VelLess,  -10.0,      AltNone,    0.0,      BlowSquib,  Pyro::PyroChannel::SquibB,  2000    }}; //Backup Event, if drouge does not deploy

#include "AltFilter.hpp"
#include "stdint.h"

class FlightPlan{
    public:
        FlightPlan();
        void update(AltFilter& filter);
        void dumpConfig();
        void logState();

    private:
        float pad_alts[2] = {0.0, 0.0}; //a buffer of past altitudes, the 0th of which will be the pad altitude
        uint8_t pad_alt_counter = 0;
        uint32_t print_timer = 0;
        uint32_t state_timer = 0;
        FlightState state;
        bool event_done[(sizeof(eventList)/sizeof(FlightEvent))] = {false,false};
        uint32_t event_timer = 0;

};

