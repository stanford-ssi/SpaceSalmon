#include "SlateKey.hpp"
#include "ArduinoJson.h"
#include <functional>

template <class T, size_t N>
class Array : public SlateKeyGeneric
{
public:
    std::array<T, N> &listref;

    Array(const std::string id, std::array<T, N> &list) : SlateKeyGeneric(id), listref(list){};

    T &operator[](int index)
    {
        return listref.at(index);
    }

    void dump(JsonVariant dst)
    {
        JsonArray arr = dst.createNestedArray(id);
        for (T &elem : listref)
        {
            SlateKeyGeneric &key = static_cast<SlateKeyGeneric &>(elem);
            key.dump(arr);
        }
    }
};