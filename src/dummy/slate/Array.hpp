#include "SlateKey.hpp"
#include "ArduinoJson.h"
#include <functional>

template <class T, size_t N>
class Array : public SlateKeyGeneric
{
public: //THIS IS STILL NOT WORKING....? we could try a tuple? idk.
    const std::array<std::reference_wrapper<T>, N> list;
    Array(const std::string id, std::array<std::reference_wrapper<T>, N> keys): SlateKeyGeneric(id), list(keys){};
    
    T& operator[](int index)
    {
        return list.at(index).get();
    }

    void dump(JsonVariant dst){
        Serial.println("dump array start");
        JsonArray arr = dst.createNestedArray(id);
        Serial.println("dump array B");
        for(std::reference_wrapper<T> elem : list){
            Serial.println("dump array Loop A");
            Serial.println(static_cast<SlateKeyGeneric>(elem.get()).id.c_str());
            Serial.println("dump array Loop B");
            SlateKeyGeneric& key = static_cast<SlateKeyGeneric&>(elem.get());
            key.dump(arr);
        }
        Serial.println("dump array C");
    }
};  