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
   
    
    class Solenoid : public Array<SlateKey<bool>,5>
    {
    public:
        SlateKey<bool> s1 = SlateKey<bool>("1", false);
        SlateKey<bool> s2 = SlateKey<bool>("2", false);
        SlateKey<bool> s3 = SlateKey<bool>("3", false);
        SlateKey<bool> s4 = SlateKey<bool>("4", false);
        SlateKey<bool> s5 = SlateKey<bool>("5", false);

        Solenoid(const std::string id) : Array(id, {std::ref(s1),std::ref(s2),std::ref(s3),std::ref(s4),std::ref(s5)}){};
    } solenoid = Solenoid("solenoid");

    // Array<SlateKey<bool>,5> solenoids = Array<SlateKey<bool>,5>("solenoids",{
    //     SlateKey<bool>("1", false),
    //     SlateKey<bool>("2", false),
    //     SlateKey<bool>("3", false),
    //     SlateKey<bool>("4", false),
    //     SlateKey<bool>("5", false)
    // });

    
    Slate(const std::string id) : Container(id, {std::ref(arm), std::ref(fire), std::ref(squib), std::ref(solenoid)}){};
};
