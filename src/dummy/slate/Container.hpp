#pragma once
#include "SlateKey.hpp"
#include <functional>

template <size_t N>
class Container : public SlateKeyGeneric
{
public:
    Container(const std::string id, std::array<std::reference_wrapper<SlateKeyGeneric>, N> keys) : SlateKeyGeneric(id), list(keys){};

    const std::array<std::reference_wrapper<SlateKeyGeneric>, N> list;

    void dump(JsonVariant dst)
    {
        Serial.println("dump container start");

        JsonObject obj = dst.createNestedObject(id);
        Serial.println("dump container A");
        Serial.println(list.size());
        for (auto elem : list)
        {
            Serial.println("dump container loop A");
            SlateKeyGeneric &test = elem.get();
            //sizeof(elem);
            Serial.println((uint32_t)&test, HEX);
            Serial.println("dump container loop B");
            Serial.println(test.id.c_str());
            Serial.println("dump container loop C");
            test.dump(obj); //crashes here
            Serial.println("dump container loop D");
        }
        Serial.println("dump container end");
    }
};