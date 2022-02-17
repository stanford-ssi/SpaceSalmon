#pragma once
#include "Container.hpp"
#include "SlateKey.hpp"
#include "Array.hpp"
#include <functional>
#include <string>

class Slate : public Container<10>
{
public:

    class Sense : public Container<6> // need to increment this when changing number of sensors
    {
    public:
        SlateKey<float> pt1 = SlateKey<float>("PT1", 0);
        SlateKey<float> pt2 = SlateKey<float>("PT2", 0);
        SlateKey<float> pt3 = SlateKey<float>("PT3", 0);
        SlateKey<float> pt4 = SlateKey<float>("PT4", 0);
        // SlateKey<float> pt5 = SlateKey<float>("PT5", 0);
        SlateKey<float> lc1 = SlateKey<float>("LC1", 0);
        // SlateKey<float> lc2 = SlateKey<float>("LC2", 0);
        SlateKey<float> tc1 = SlateKey<float>("TC1", 0);
        Sense(const std::string id) : Container(id, {
            std::ref(pt1),
            std::ref(pt2),
            std::ref(pt3),
            std::ref(pt4),
            std::ref(lc1),
            std::ref(tc1)}){};
    } sense = Sense("sense");

    class Squib : public Container<2>
    {
    public:
        SlateKey<bool> arm = SlateKey<bool>("arm", false);
        Array<SlateKey<bool>,8> fire = Array<SlateKey<bool>,8>("fire",{
            SlateKey<bool>("tick", false),
            SlateKey<bool>("2", false),
            SlateKey<bool>("3", false),
            SlateKey<bool>("4", false),
            SlateKey<bool>("5", false),
            SlateKey<bool>("6", false),
            SlateKey<bool>("7", false),
            SlateKey<bool>("8", false)
        });
        Squib(const std::string id) : Container(id, {std::ref(arm),std::ref(fire)}){};
    } squib = Squib("squib");
    
    Array<SlateKey<bool>,8> solenoid = Array<SlateKey<bool>,8>("solenoid",{
        SlateKey<bool>("1", false),
        SlateKey<bool>("2", false),
        SlateKey<bool>("3", false),
        SlateKey<bool>("4", false),
        SlateKey<bool>("5", false),
        SlateKey<bool>("6", false),
        SlateKey<bool>("7", false),
        SlateKey<bool>("8", false)
    });

    Array<SlateKey<float>,4> adc_in = Array<SlateKey<float>,4>("adc_in",{
        SlateKey<float>("1", 0.0),
        SlateKey<float>("2", 0.0),
        SlateKey<float>("3", 0.0),
        SlateKey<float>("4", 0.0)
    });

    class Sequence : public Container<3>
    {
    public:
        SlateKey<std::string> name = SlateKey<std::string>("name", "");
        SlateKey<bool> start = SlateKey<bool>("start", false);
        SlateKey<bool> play = SlateKey<bool>("play", false);
        Sequence(const std::string id) : Container(id, {std::ref(name),std::ref(start),std::ref(play)}){};
    } sequence = Sequence("sequence");

    SlateKey<float> SV = SlateKey<float>("SV", 0);
    SlateKey<float> EM = SlateKey<float>("EM", 0);
    SlateKey<float> v_batt = SlateKey<float>("v_batt", 0.0);
    SlateKey<float> i_batt = SlateKey<float>("i_batt", 0.0);
    SlateKey<float> logging = SlateKey<float>("logging", false);
    SlateKey<float> error = SlateKey<float>("error", false);

    Slate(const std::string id) : Container(id, {
        std::ref(squib), 
        std::ref(solenoid), 
        std::ref(adc_in), 
        std::ref(sequence),
        std::ref(SV),
        std::ref(EM),
        std::ref(v_batt),
        std::ref(i_batt),
        std::ref(logging),
        std::ref(error)
    }){};
};