#pragma once
#include <string>
#include "Mutex.hpp"
#include "ArduinoJson.h"

class SlateKeyGeneric
{
public:
    SlateKeyGeneric(const std::string id) : id(id) {}
    virtual void dump(JsonVariant dst){};
    void metadump(JsonVariant dst) {dump(dst);};

protected:
    std::string id;
};

// This is by-copy, so best for small data types, (less than the size of a reference).
template <typename T>
class SlateKey : public SlateKeyGeneric
{
public:
    SlateKey(const std::string id, T init) : SlateKeyGeneric(id), value(init){}

    T get(void) 
    {
        mutex.take(NEVER);
        T temp = value;
        mutex.give();
        return temp;
    }

    void set(const T to)
    {
        mutex.take(NEVER);
        value = to;
        mutex.give();
    }

    T operator()() { return get(); }

    SlateKey<T> &operator<<(const T &in)
    {
        set(in);
        return *this;
    }

    SlateKey<T> &operator=(SlateKey<T>& src) {
        set(src.get()); 
        return *this;
    }

    void operator>>(JsonVariant dst)
    {
        dump(dst);
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