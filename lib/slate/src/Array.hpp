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
            JsonObject arr = dst.createNestedObject(id); // yes i know there are JsonArrays, no they dont work
            for (auto elem : list) {
                T &key = elem.get();    
                key.dump(arr);
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