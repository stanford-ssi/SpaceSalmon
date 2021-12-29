#pragma once
#include <string>
#include "Mutex.hpp"
#include "ArduinoJson.h"

// This is by-copy, so best for small data types, (less than the size of a reference).
class SlateKeyGeneric
{
public:
    SlateKeyGeneric(const std::string id) : id(id) {}
    const std::string id;
    virtual void dump(JsonVariant dst){};
};

template <typename T>
class SlateKey : public SlateKeyGeneric
{
public:
    uint32_t originalptr;
    SlateKey(const std::string id, T init) : SlateKeyGeneric(id), originalptr((uint32_t)this) {}

    const T get(void)
    {
        mutex.take(NEVER);
        T temp = value;
        mutex.give();
        return temp;
    }

    void set(T to)
    {
        Serial.println("set A");
        mutex.take(NEVER);
        Serial.println("set B");
        value = to;
        Serial.println("set C");
        mutex.give();
        Serial.println("set D");
    }

    operator T() { return get(); }
    SlateKey &operator=(T const &in)
    {
        set(in);
        return *this;
    }

    void dump(JsonVariant dst)
    {

        if (dst.is<JsonArray>())
        {
            Serial.println("dump key into array");
            dst.add(get());
        }
        else
        {
            Serial.println("dump key into object");
            dst[id] = get();
        }
        Serial.println("dump key end");
    }

    //This is not actually safe. We count on the mutex not being used when the key is moved around...
    SlateKey(SlateKey &&e) : SlateKeyGeneric(e.id), originalptr(e.originalptr), value(e.value) {}

private:
    T value;
    Mutex mutex;
};