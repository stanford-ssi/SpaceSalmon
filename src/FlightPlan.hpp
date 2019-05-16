#pragma once

class FlightPlan;

#include "main.hpp"

static const FlightEvent eventList[] = {{Falling,   VelLess,    0.0,      AltNone,    0.0,      BlowSquib,  SquibA  },  //Apogee Event, at velocity 0-crossing
                                        {Falling,   VelNone,    0.0,      AltLess,    500.0,    BlowSquib,  SquibB  },  //Main Event, when below 500 meters
                                        {Falling,   VelLess,    -100.0,   AltLess,    1500.0,   BlowSquib,  SquibC  }}; //Backup Main Event, if drouge does not deploy

class FlightPlan{
    public:
        FlightPlan();
        void update(AltFilter filter);

    private:
        float pad_alts[2] ={0.0, 0.0}; //a buffer of past altitudes, the 0th of which will be the pad altitude
        uint8_t pad_alt_counter = 0;
        FlightState state;

};

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

typedef enum {
    SquibNone,
    SquibA,
    SquibB,
    SquibC
} SquibChannel;

typedef struct {
    FlightState state;
    VelocityCondition velCond;
    float velocity; //meters/sec
    AltCondition altCond;
    float altitude; //meters AGL
    EventAction action;
    SquibChannel squib;
} FlightEvent;