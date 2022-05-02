#pragma once
#include "SlateKey.hpp"
#include <functional>

template <size_t N>
class Container : public SlateKeyGeneric
{
public:
    Container(const std::string id, std::array<std::reference_wrapper<SlateKeyGeneric>, N> keys) : SlateKeyGeneric(id), list(keys){};

    void dump(JsonVariant dst) override {
        JsonObject obj = dst.createNestedObject(id);
        for (auto elem : list) {
            SlateKeyGeneric &test = elem.get();
            test.dump(obj); 
        }
    }

    void metadump(JsonVariant dst) override {
        JsonObject obj = dst.createNestedObject(id);
        for (auto elem : list) {
            SlateKeyGeneric &test = elem.get();
            test.metadump(obj); 
        }
    }

    Container &operator<<(const JsonVariant src) override{
        if (src.containsKey(id)) {
            for (auto elem : list) {
                SlateKeyGeneric &test = elem.get();
                test << src[id];
            }
        }
        return *this;
    }

private:
    const std::array<std::reference_wrapper<SlateKeyGeneric>, N> list;
};