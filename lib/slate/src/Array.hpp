#pragma once

#include "Container.hpp"
#include "SlateKey.hpp"
#include "ArduinoJson.h"
#include <functional>

template <class T, size_t N>
class Array : public SlateKeyGeneric {
    public:
        Array(const std::string id, std::array<std::reference_wrapper<T>, N> keys) : 
        SlateKeyGeneric(id), list(keys) {};

        T &operator[](int index) {
            return list[index].get();
        }

        void dump(JsonVariant dst) {
            JsonArray arr = dst.createNestedArray(id);
            for (auto elem : list) {
                T &key = elem.get();
                key.dump(arr);
                // if (id == "squib") {
                //     if (arr.isNull()) {
                //         Serial.println("array eureka");
                //     } else {
                //         Serial.println("array nvm");
                //         Serial.println(arr.memoryUsage());
                //     }
                //     if (dst.containsKey("squib")) {
                //         Serial.println("tears");
                //     } else {
                //         Serial.println("other trears");
                //     }
                // }
            }
        }

        void metadump(JsonVariant dst) {
            JsonArray arr = dst.createNestedArray(id);
            for (auto elem : list) {
                T &key = elem.get();
                key.metadump(arr);
            }
        }

    private:
        const std::array<std::reference_wrapper<T>, N> list;
};