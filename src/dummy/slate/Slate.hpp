#pragma once
#include "Container.hpp"
#include "SlateKey.hpp"
#include "Array.hpp"
#include <functional>

class Slate : public Container<4>
{
public:
    class Squib : public Container<1>
    {
    public:
        SlateKey<bool> lol = SlateKey<bool>("lol", false);
        Squib(const std::string id) : Container(id, {std::ref(lol)}){};
    } squib = Squib("squib");

    SlateKey<bool> arm = SlateKey<bool>("arm", false);
    SlateKey<bool> fire = SlateKey<bool>("fire", false);
    
    std::array<SlateKey<bool>,5> _solenoid = {
        SlateKey<bool>("1", false),
        SlateKey<bool>("2", false),
        SlateKey<bool>("3", false),
        SlateKey<bool>("4", false),
        SlateKey<bool>("5", false)
    };  
    Array<SlateKey<bool>,5> solenoid = Array<SlateKey<bool>,5>("solenoid",_solenoid);
    
    Slate(const std::string id) : Container(id, {std::ref(arm), std::ref(fire), std::ref(squib), std::ref(solenoid)}){};
};
