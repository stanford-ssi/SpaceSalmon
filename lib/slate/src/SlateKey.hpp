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
    void metadump(JsonVariant dst) {dump(dst);};
};

// This is by-copy, so best for small data types, (less than the size of a reference).
template <typename T>
class SlateKey : public SlateKeyGeneric
{
public:
    SlateKey(const std::string id, T init) : SlateKeyGeneric(id), value(init){}

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

    T operator()() { return get(); }

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
  
private:
    T value;
    Mutex mutex;
};