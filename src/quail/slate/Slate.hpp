#pragma once

#include "Container.hpp"
#include "SlateKey.hpp"
#include "Array.hpp"
#include "../config.h"
#include "endpoints.hpp"
#include "abstractions.hpp"
#include <functional>
#include <string>

class Slate : public Container<7>
{
public:
    class Sense : public Container<11> // need to increment this when changing number of sensors
    {
    public:
        SensorSlate pt1 = SensorSlate(NO_QUAIL_ID, "PT1");
        SensorSlate pt2 = SensorSlate(NO_QUAIL_ID, "PT2");
        SensorSlate pt3 = SensorSlate(NO_QUAIL_ID, "PT3");
        SensorSlate pt4 = SensorSlate(NO_QUAIL_ID, "PT4");
        SensorSlate pt5 = SensorSlate(NO_QUAIL_ID, "PT5");
        SensorSlate pt6 = SensorSlate(NO_QUAIL_ID, "PT6");
        SensorSlate pt7 = SensorSlate(NO_QUAIL_ID, "PT7");
        SensorSlate lc1 = SensorSlate(NO_QUAIL_ID, "LC1");
        SensorSlate lc2 = SensorSlate(NO_QUAIL_ID, "LC2");
        SensorSlate tc1 = SensorSlate(NO_QUAIL_ID, "TC1");
        SensorSlate tc2 = SensorSlate(NO_QUAIL_ID, "TC2");
        Sense(const std::string id) : Container(id, {
            std::ref(pt1),
            std::ref(pt2),
            std::ref(pt3),
            std::ref(pt4),
            std::ref(pt5),
            std::ref(pt6),
            std::ref(pt7),
            std::ref(lc1),
            std::ref(lc2),
            std::ref(tc1),
            std::ref(tc2)}){};
    } sense = Sense("sensors");

    Array<Igniter, NUM_EM_CHANNELS> squib = Array<Igniter, NUM_EM_CHANNELS>("squib", {
        Igniter(NO_QUAIL_ID, "E1"),
        Igniter(NO_QUAIL_ID, "E2"),
        Igniter(NO_QUAIL_ID, "E3"),
        Igniter(NO_QUAIL_ID, "E4"),
        Igniter(NO_QUAIL_ID, "E5"),
        Igniter(NO_QUAIL_ID, "E6"),
        Igniter(NO_QUAIL_ID, "E7"),
        Igniter(NO_QUAIL_ID, "E8")      
    });
    
    Array<Solenoid, NUM_SOLENOIDS> solenoid = Array<Solenoid, NUM_SOLENOIDS>("solenoid",{
        Solenoid(NO_QUAIL_ID, "S1"),
        Solenoid(NO_QUAIL_ID, "S2"),
        Solenoid(NO_QUAIL_ID, "S3"),
        Solenoid(NO_QUAIL_ID, "S4"),
        Solenoid(NO_QUAIL_ID, "S5"),
        Solenoid(NO_QUAIL_ID, "S6"),
        Solenoid(NO_QUAIL_ID, "S7"),
        Solenoid(NO_QUAIL_ID, "S8")
    });

    Array<SlateKey<float>, 4> adc_in = Array<SlateKey<float>,4>("adc_in",{
        EndSensor(NO_QUAIL_ID, "1", 0.0),
        EndSensor(NO_QUAIL_ID, "2", 0.0),
        EndSensor(NO_QUAIL_ID, "3", 0.0),
        EndSensor(NO_QUAIL_ID, "4", 0.0)
    });

    class Sequence : public Container<5> {
        public:
            EndPoint<SEQUENCE_STATE> redline = EndPoint<SEQUENCE_STATE>("REDLINE", NO_QUAIL_ID, RUNNING, true);
            EndPoint<SEQUENCE_STATE> abort = EndPoint<SEQUENCE_STATE>("ABORT", NO_QUAIL_ID, SUSPEND, false);
            EndPoint<SEQUENCE_STATE> dischargeOx = EndPoint<SEQUENCE_STATE>("DISCHARGEOX", NO_QUAIL_ID, SUSPEND, false);
            EndPoint<SEQUENCE_STATE> dischargeFuel = EndPoint<SEQUENCE_STATE>("DISCHARGEFUEL", NO_QUAIL_ID, SUSPEND, false);
            EndPoint<SEQUENCE_STATE> launch = EndPoint<SEQUENCE_STATE>("LAUNCH", NO_QUAIL_ID, SUSPEND, false);
            Sequence(const std::string id) : Container(id, {
                std::ref(redline),
                std::ref(abort),
                std::ref(dischargeOx),
                std::ref(dischargeFuel),
                std::ref(launch)
            }){};
    } sequence = Sequence("sequence");

    class Battery : public Container<2> {
        public : 
            SensorSlate v_batt = SensorSlate("Voltage", "PB09");
            SensorSlate i_batt = SensorSlate("Current", "PB09");
            Battery(const std::string id) : Container(id, {
                std::ref(v_batt),
                std::ref(i_batt)
            }){};
    } battery = Battery("battery");

    class Board : public Container<3> {
        public:
            EndPoint<float> logging = EndPoint<float>("logging", NO_QUAIL_ID, false, false);
            EndPoint<float> error = EndPoint<float>("error", NO_QUAIL_ID, false, false);
            EndPoint<unsigned> tick = EndPoint<unsigned>("tick", NO_QUAIL_ID, 0, false);
            Board(const std::string id) : Container(id, {
                std::ref(logging),
                std::ref(error),
                std::ref(tick)
            }){};
    } board = Board("board");

    Slate(const std::string id) : Container(id, { 
        // If you change the length of this list, you also need to change the
        // class definition (its templated for the length)
        std::ref(sense), 
        std::ref(squib), 
        std::ref(solenoid), 
        std::ref(adc_in), 
        std::ref(sequence),
        std::ref(battery),
        std::ref(board)
    }){};
};