#pragma once

#include "Container.hpp"
#include "SlateKey.hpp"
#include "Array.hpp"
#include "../config.h"
#include "SlateEndpoints.hpp"
#include "SlateAbstractions.hpp"
#include <functional>
#include <string>

class Slate : public Container<6>
{
public:
    class Sensors : public Container<13> // need to increment this when changing number of sensors
    {
    public:
        SensorSlate pt1 = SensorSlate("PT1", "PT1", "GN2 Source");
        SensorSlate pt2 = SensorSlate("PT2", "PT2", "GN2 Postfill");
        SensorSlate pt3 = SensorSlate("PT3", "PT3", "Ox Source");
        SensorSlate pt4 = SensorSlate("PT4", "PT4", "Ox Postfill");
        SensorSlate pt5 = SensorSlate("PT5", "PT5", NO_QUAIL_ID);
        SensorSlate pt6 = SensorSlate("PT6", "PT6", NO_QUAIL_ID);
        SensorSlate pt7 = SensorSlate("PT7", "PT7", NO_QUAIL_ID);
        SensorSlate pt8 = SensorSlate("PT8", "PT8", NO_QUAIL_ID);
        SensorSlate lc1 = SensorSlate("LC1", "LC1", "left LC");
        SensorSlate lc2 = SensorSlate("LC2", "LC2", "right LC");
        SensorSlate lcs = SensorSlate("LCSum", NO_QUAIL_ID, "Thrust+Weight");
        SensorSlate tc1 = SensorSlate("TC1", "TC1", "Ox Bottle");
        SensorSlate tc2 = SensorSlate("TC2", "TC2", "Ox Tank");
        Sensors(const std::string id) : Container(id, {
            std::ref(pt1),
            std::ref(pt2),
            std::ref(pt3),
            std::ref(pt4),
            std::ref(pt5),
            std::ref(pt6),
            std::ref(pt7),
            std::ref(pt8),
            std::ref(lc1),
            std::ref(lc2),
            std::ref(lcs),
            std::ref(tc1),
            std::ref(tc2)
        }){
            lcs.updateUnits("N");
        };

        void dump(const JsonVariant dst) override{
            lcs << lc1() + lc2();
            Container::dump(dst);
        }
    } sense = Sensors("sensors");

    class Squibs : public Array<Igniter, NUM_EM_CHANNELS> {
        public:
            Squibs(const std::string id) : Array(id, {
                std::ref(ig1),
                std::ref(ig2),
                std::ref(ig3),
                std::ref(ig4),
            }){};

        private:
            Igniter ig1 = Igniter("E1", "E1", "The one the goes boom");
            Igniter ig2 = Igniter("E2", "E2", "The other one");
            Igniter ig3 = Igniter("E3", "E3");
            Igniter ig4 = Igniter("E4", "E4");
    } squib = Squibs("squib");

    class Valves : public Array<Solenoid, NUM_SOLENOIDS> {
        public:
            Valves(const std::string id) : Array(id, {
                std::ref(s1),
                std::ref(s2),
                std::ref(s3),
                std::ref(s4),
                std::ref(s5),
                std::ref(s6),
                std::ref(s7),
                std::ref(s8),
                std::ref(s9),
                std::ref(s10),
                std::ref(s11),
                std::ref(s12)
            }){};

        private:
            Solenoid s1 = Solenoid("S1", "S1", 0, "Fuel Main");            
            Solenoid s2 = Solenoid("S2", "S2", 1, "Ox Main");
            Solenoid s3 = Solenoid("S3", "S3", 2, "Fuel Fill");
            Solenoid s4 = Solenoid("S4", "S4", 3, "Fuel Bleed");
            Solenoid s5 = Solenoid("S5", "S5", 4, "Ox Fill");
            Solenoid s6 = Solenoid("S6", "S6", 5, "Ox Bleed");
            Solenoid s7 = Solenoid("S7", "S7", 6);
            Solenoid s8 = Solenoid("S8", "S8", 7, "OX Vent");
            Solenoid s9 = Solenoid("S9", "S9", 8, "Safe To Approach");
            Solenoid s10 = Solenoid("S10", "S10", 9);
            Solenoid s11 = Solenoid("S11", "S11", 10);
            Solenoid s12 = Solenoid("S12", "S12", 11);
    } valves = Valves("valves");

    class Sequence : public Container<5> {
        public:
            EndPoint<EngineState> engineState = EndPoint<EngineState>("Engine State", NO_QUAIL_ID, ENGINE_IDLE, true);
            
            EndPoint<TankState> oxState = EndPoint<TankState>("Ox Tank State", NO_QUAIL_ID, TANK_IDLE_EMPTY, false);
            EndPoint<float> oxOpPressure = EndPoint<float>("Ox Operating Pressure", NO_QUAIL_ID, 0.9 * MAWP, true);
            
            EndPoint<TankState> fuelState = EndPoint<TankState>("Fuel Tank State", NO_QUAIL_ID, TANK_IDLE_EMPTY, false);
            EndPoint<float> fuelOpPressure = EndPoint<float>("Fuel Operating Pressure", NO_QUAIL_ID, 0.9 * MAWP, true);
            
            Sequence(const std::string id) : Container(id, {
                std::ref(engineState),
                std::ref(oxState),
                std::ref(oxOpPressure),
                std::ref(fuelState),
                std::ref(fuelOpPressure)
            }){};
    } sequence = Sequence("sequence");

    class Battery : public Container<2> {
        public : 
            SensorSlate v_batt = SensorSlate("Voltage", "PB09", "Main Bus Voltage");
            SensorSlate i_batt = SensorSlate("Current", "PB09", "Total Current");
            Battery(const std::string id) : Container(id, {
                std::ref(v_batt),
                std::ref(i_batt)
            }){};
    } battery = Battery("battery");

    class Board : public Container<4> {
        public:
            EndPoint<bool> logging = EndPoint<bool>("logging", NO_QUAIL_ID, false, false);
            EndPoint<unsigned> error = EndPoint<unsigned>("error", NO_QUAIL_ID, 0, false);
            EndPoint<unsigned> tick = EndPoint<unsigned>("tick", NO_QUAIL_ID, 0, false);
            EndPoint<bool> comms = EndPoint<bool>("comms", NO_QUAIL_ID, true, true);
            Board(const std::string id) : Container(id, {
                std::ref(logging),
                std::ref(error),
                std::ref(tick),
                std::ref(comms)
            }){};
    } board = Board("board");

    Slate(const std::string id) : Container(id, { 
        // If you change the length of this list, you also need to change the
        // class definition (its templated for the length)
        std::ref(sense),
        std::ref(valves),  
        std::ref(squib),
        std::ref(sequence),
        std::ref(battery),
        std::ref(board)
    }){};
};