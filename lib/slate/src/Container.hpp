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
        JsonObject obj = dst.createNestedObject(id);
        for (auto elem : list)
        {
            SlateKeyGeneric &test = elem.get();
            test.dump(obj); //crashes here
        }
    }

    Container &operator<<(const JsonVariant src) {
        if (src.containsKey(id)) {
            for (auto elem : list) {
                SlateKeyGeneric &test = elem.get();
                test << src[id];
            }
        }
        return *this;
    }
};