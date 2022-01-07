#pragma once
#include "../slate/Container.hpp"
#include "../slate/SlateKey.hpp"
#include "../slate/Array.hpp"
#include <functional>

class Slate : public Container<3>
{
public:
    class Squib : public Container<2>
    {
    public:
        SlateKey<bool> arm = SlateKey<bool>("arm", false);
        Array<SlateKey<bool>,8> fire = Array<SlateKey<bool>,8>("fire",{
            SlateKey<bool>("1", false),
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
    
    Slate(const std::string id) : Container(id, {std::ref(squib), std::ref(solenoid), std::ref(adc_in)}){};
};
