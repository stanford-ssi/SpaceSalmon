#pragma once

#include "SlateKey.hpp"
#include "ArduinoJson.h"
#include <functional>

template <class T, size_t N>
class Array : public SlateKeyGeneric
{
public:
    std::array<T, N> listref;

    Array(const std::string id, std::array<T, N>&& list) : SlateKeyGeneric(id), listref(std::move(list)){};

    T &operator[](int index)
    {
        return listref.at(index);
    }

    void dump(JsonVariant dst) override {
        JsonArray arr = dst.createNestedArray(id);
        for (T &elem : listref) {
            SlateKeyGeneric &key = static_cast<SlateKeyGeneric &>(elem);
            key.dump(arr);
        }
    }

    void metadump(JsonVariant dst) override {
        JsonArray arr = dst.createNestedArray(id);
        for (T &elem : listref)
        {
            SlateKeyGeneric &key = static_cast<SlateKeyGeneric &>(elem);
            key.metadump(arr);
        }
    }

    Array &operator<<(const JsonVariant src) override {
        if (src.containsKey(id)) {
            for (T &elem : listref) {
                (SlateKeyGeneric)elem << src[id];
            }
        }
        return *this;
    }
};