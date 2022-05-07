#pragma once

#include "Container.hpp"
#include "SlateKey.hpp"
#include "Array.hpp"
#include "../config.h"
#include "SlateEndpoints.hpp"
#include "SlateAbstractions.hpp"
#include <functional>
#include <string>

class Slate : public Container<7>
{
public:
    class Sensors : public Container<12> // need to increment this when changing number of sensors
    {
    public:
        SensorSlate pt1 = SensorSlate("PT1", "PT1");
        SensorSlate pt2 = SensorSlate("PT2", "PT2");
        SensorSlate pt3 = SensorSlate("PT3", "PT3");
        SensorSlate pt4 = SensorSlate("PT4", "PT4");
        SensorSlate pt5 = SensorSlate("PT5", "PT5");
        SensorSlate pt6 = SensorSlate("PT6", "PT6");
        SensorSlate pt7 = SensorSlate("PT7", "PT7");
        SensorSlate pt8 = SensorSlate("PT8", "PT8");
        SensorSlate lc1 = SensorSlate("LC1", "LC1");
        SensorSlate lc2 = SensorSlate("LC2", "LC2");
        SensorSlate tc1 = SensorSlate("TC1", "TC1");
        SensorSlate tc2 = SensorSlate("TC2", "TC2");
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
            std::ref(tc1),
            std::ref(tc2)
        }){};
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
            Igniter ig1 = Igniter("E1", "E1");
            Igniter ig2 = Igniter("E2", "E2");
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
            Solenoid s1 = Solenoid("S1", "S1");            
            Solenoid s2 = Solenoid("S2", "S2");
            Solenoid s3 = Solenoid("S3", "S3");
            Solenoid s4 = Solenoid("S4", "S4");
            Solenoid s5 = Solenoid("S5", "S5");
            Solenoid s6 = Solenoid("S6", "S6");
            Solenoid s7 = Solenoid("S7", "S7");
            Solenoid s8 = Solenoid("S8", "S8");
            Solenoid s9 = Solenoid("S9", "S9");
            Solenoid s10 = Solenoid("S10", "S10");
            Solenoid s11 = Solenoid("S11", "S11");
            Solenoid s12 = Solenoid("S12", "S12");
    } valves = Valves("valves");

    class ADCIn : public Array<EndSensor, 4> {
        public:
            ADCIn(const std::string id) : Array(id, {
                std::ref(es1),
                std::ref(es2),
                std::ref(es3),
                std::ref(es4)
            }){};

        private:
            EndSensor es1 = EndSensor("ADC1", "ADC1", 0.0);
            EndSensor es2 = EndSensor("ADC2", "ADC2", 0.0);
            EndSensor es3 = EndSensor("ADC3", "ADC3", 0.0);
            EndSensor es4 = EndSensor("ADC4", "ADC4", 0.0);
    } adc_in = ADCIn("adc_in");

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
            EndPoint<bool> logging = EndPoint<bool>("logging", NO_QUAIL_ID, false, false);
            EndPoint<unsigned> error = EndPoint<unsigned>("error", NO_QUAIL_ID, 0, true);
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
        std::ref(valves),  
        std::ref(squib),
        std::ref(adc_in), 
        std::ref(sequence),
        std::ref(battery),
        std::ref(board)
    }){};
};