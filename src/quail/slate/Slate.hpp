#pragma once
#include "Container.hpp"
#include "SlateKey.hpp"
#include "Array.hpp"

class Slate
{
public:

    Array<SlateKey<bool>> solenoids = Array<SlateKey<bool>>("solenoids",{
        SlateKey<bool>("1", false),
        SlateKey<bool>("2", false),
        SlateKey<bool>("3", false),
        SlateKey<bool>("4", false),
        SlateKey<bool>("5", false),
        SlateKey<bool>("6", false),
        SlateKey<bool>("7", false),
        SlateKey<bool>("8", false)
    });

    Array<SlateKey<float>> adc = Array<SlateKey<float>>("adc",{
        SlateKey<float>("1", 0.0),
        SlateKey<float>("2", 0.0),
        SlateKey<float>("3", 0.0),
        SlateKey<float>("4", 0.0),
        SlateKey<float>("5", 0.0),
    });

    class Squib : public Container<2>
    {
        public:

            SlateKey<bool> arm = SlateKey<bool>("arm", false);

            Array<SlateKey<bool>> fire = Array<SlateKey<bool>>("fire",{
                SlateKey<bool>("1", false),
                SlateKey<bool>("2", false),
                SlateKey<bool>("3", false),
                SlateKey<bool>("4", false),
                SlateKey<bool>("5", false),
                SlateKey<bool>("6", false),
                SlateKey<bool>("7", false),
                SlateKey<bool>("8", false)
            });

            SlateKeyGeneric *list[2] = {&arm, &fire};
            Squib(const std::string id): Container(id){};
    };

    Squib squib = Squib("squib");
};

