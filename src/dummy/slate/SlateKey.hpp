#pragma once
#include <string>
#include "Mutex.hpp"
#include "ArduinoJson.h"

// This is by-copy, so best for small data types, (less than the size of a reference).
class SlateKeyGeneric{
    public:
        SlateKeyGeneric(const std::string id): id(id){}
        const std::string id;
        virtual void dump(JsonVariant dst){};
};

template <typename T>
class SlateKey : public SlateKeyGeneric
{
    public:
        SlateKey(const std::string id, T init): SlateKeyGeneric(id){}
        
        const T get(void){
            //mutex.take(NEVER);
            return value;
            //mutex.give();
        }

        void set(T to) {
            Serial.println("set A");
            //mutex.take(NEVER);
            Serial.println("set B");
            value = to;
            Serial.println("set C");
            //mutex.give();
            Serial.println("set D");
        }

        operator T() { return get(); }
        SlateKey &operator=(T const &in) { set(in); return *this; }

        void dump(JsonVariant dst){

            if (dst.is<JsonArray>()){
                Serial.println("dump key into array");
                dst.add(get());
            }else{
                Serial.println("dump key into object");
                dst[id] = get();
            }
            Serial.println("dump key end");
        }

    private:
        T value;
        Mutex mutex;
};