#pragma once
#include <string>
#include "Mutex.hpp"
#include "ArduinoJson.h"

class SlateKeyGeneric
{
public:
    SlateKeyGeneric(const std::string id) : id(id) {}
    const std::string id;
    virtual void dump(JsonVariant dst){};
};

// This is by-copy, so best for small data types, (less than the size of a reference).
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
        mutex.take(NEVER);
        value = to;
        mutex.give();
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
            dst.add(get());
        }
        else
        {
            dst[id] = get();
        }
    }

    //This is not actually safe. We count on the mutex not being used when the key is moved around...
    SlateKey(SlateKey &&e) : SlateKeyGeneric(e.id), originalptr(e.originalptr), value(e.value) {}

private:
    T value;
    Mutex mutex;
};