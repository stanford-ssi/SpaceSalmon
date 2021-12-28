#include "SlateKey.hpp"
#include "ArduinoJson.h"
#include <functional>

template <class T, size_t N>
class Array : public SlateKeyGeneric
{
public:
    std::array<T, N> &listref;

    Array(const std::string id, std::array<T, N> &list) : SlateKeyGeneric(id), listref(list){};

    T &operator[](int index)
    {
        return listref.at(index);
    }

    void dump(JsonVariant dst)
    {
        Serial.println("dump array start");
        JsonArray arr = dst.createNestedArray(id);
        Serial.println("dump array B");
        for(T& elem : listref){
            Serial.println("dump array Loop A");
            Serial.println(static_cast<SlateKeyGeneric>(elem).id.c_str());
            Serial.println("dump array Loop B");
            SlateKeyGeneric& key = static_cast<SlateKeyGeneric&>(elem);
            key.dump(arr);
        }
        Serial.println("dump array C");
    }
};