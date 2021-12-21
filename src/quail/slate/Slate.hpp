#pragma once
#include "Container.hpp"
#include "SlateKey.hpp"
#include "Array.hpp"

class Slate
{
    public:
    class Solenoids : public Container<6>
    {
        public:
            SlateKey<bool> sol1 = SlateKey<bool>("1", false);
            SlateKey<bool> sol2 = SlateKey<bool>("2", false);
            SlateKey<bool> sol3 = SlateKey<bool>("3", false);
            SlateKey<bool> sol4 = SlateKey<bool>("4", false);
            SlateKey<bool> sol5 = SlateKey<bool>("5", false);
            SlateKey<bool> sol6 = SlateKey<bool>("6", false);
            SlateKey<bool> sol7 = SlateKey<bool>("7", false);
            SlateKey<bool> sol8 = SlateKey<bool>("8", false);
            SlateKeyGeneric *list[6] = {&sol1, &sol2, &sol3, &sol4, &sol5, &sol6} ;
            Solenoids(const std::string id): Container(id){};
    };

    class AdcRaw : public Array<float,5>
    {
        public:
            SlateKey<float> ch1 = SlateKey<float>("1", 0.0);
            SlateKey<float> ch2 = SlateKey<float>("2", 0.0);
            SlateKey<float> ch3 = SlateKey<float>("3", 0.0);
            SlateKey<float> ch4 = SlateKey<float>("4", 0.0);
            SlateKey<float> ch5 = SlateKey<float>("5", 0.0);
            SlateKey<float> *list[5] = {&ch1, &ch2, &ch3, &ch4, &ch5};
            AdcRaw(const std::string id): Array(id){};
    };

    class Squib : public Container<2>
    {
        public:
            SlateKey<bool> arm = SlateKey<bool>("arm", false);
            class Fire : public Array<bool,8>
            {
                public:
                    SlateKey<bool> squib1 = SlateKey<bool>("1", false);
                    SlateKey<bool> squib2 = SlateKey<bool>("2", false);
                    SlateKey<bool> squib3 = SlateKey<bool>("3", false);
                    SlateKey<bool> squib4 = SlateKey<bool>("4", false);
                    SlateKey<bool> squib5 = SlateKey<bool>("5", false);
                    SlateKey<bool> squib6 = SlateKey<bool>("6", false);
                    SlateKey<bool> squib7 = SlateKey<bool>("7", false);
                    SlateKey<bool> squib8 = SlateKey<bool>("8", false);
                    SlateKey<bool> *list[8] = {&squib1, &squib2, &squib3, &squib4, &squib5, &squib6, &squib7, &squib8};
                    Fire(const std::string id): Array(id){};
            };
            Fire fire = Fire("fire");
            SlateKeyGeneric *list[2] = {&arm, &fire};
            Squib(const std::string id): Container(id){};
    };

    Squib squib = Squib("squib");
    Solenoids solenoids = Solenoids("solenoids");
    AdcRaw adc = AdcRaw("adc_raw");
};

