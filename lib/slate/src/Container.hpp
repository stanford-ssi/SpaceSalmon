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
            test.dump(obj); //crashes here
            // if (id == "E1") {
            //     if (obj.isNull()){
            //         Serial.println("eureka");
            //         Serial.println(obj.memoryUsage());
            //     } else {
            //         Serial.println("false flag");
            //     }
            //     if (dst.containsKey("E1")) {
            //         Serial.println("pre-dope");
            //     } else {
            //         Serial.println("pre-mope");
            //     }
            // }
        }
    }

    void metadump(JsonVariant dst) override {
        JsonObject obj = dst.createNestedObject(id);
        for (auto elem : list) {
            SlateKeyGeneric &test = elem.get();
            test.metadump(obj); //crashes here
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