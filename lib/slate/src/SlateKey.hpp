#pragma once
#include <string>
#include "Mutex.hpp"
#include "ArduinoJson.h"

class SlateKeyGeneric
{
public:
    SlateKeyGeneric(const std::string id) : id(id) {}
    virtual void dump(JsonVariant dst){Serial.println("virtual");};
    virtual SlateKeyGeneric &operator<<(const JsonVariant) {Serial.println("virtual");};
    virtual void metadump(JsonVariant dst) {Serial.println("virtual");};

    // TODO: make const
    std::string id;
};

// This is by-copy, so best for small data types, (less than the size of a reference).
template <typename T>
class SlateKey : public SlateKeyGeneric
{
public:
    SlateKey(const std::string id, T init) : SlateKeyGeneric(id), value(init){}

    T get(void) {
        mutex.take(NEVER);
        T temp = value;
        mutex.give();
        return temp;
    }

    void set(const T to) {
        mutex.take(NEVER);
        value = to;
        mutex.give();
    }

    void dump(JsonVariant dst) {
        if (dst.is<JsonArray>()) {
            dst.add(get());
        } else {
            dst[id] = get();
            // if (id == "Armed") {
            //     if (dst.containsKey(id)) {
            //         Serial.println("Good");
            //     } else {
            //         Serial.println("Bad");
            //     }
            // }
        }
    }

    T operator()() { return get(); }

    SlateKey<T> &operator=(SlateKey<T>& src) {
        set(src.get()); 
        return *this;
    }

    SlateKey<T> &operator<<(const T &in) {
        set(in);
        return *this;
    }

    SlateKey<T> &operator<<(const JsonVariant src) {
        if(src.containsKey(id)) {
            *this << (T)src[id];
        }
        return *this;
    }

    void operator>>(JsonVariant dst) { dump(dst); }
  
private:
    T value;
    Mutex mutex;
};