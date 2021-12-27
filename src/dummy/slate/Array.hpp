#include "SlateKey.hpp"
#include "ArduinoJson.h"

template <typename T>
class Array : public SlateKeyGeneric
{
public:
    const std::initializer_list<T>& list;
    Array(const std::string id, std::initializer_list<T> keys): SlateKeyGeneric(id), list(keys){};
    
    T operator[](int index)
    {
        return *(list.begin() + index);
    }

    void dump(JsonVariant dst){
        const JsonArray arr = dst.createNestedArray(id);
        for(auto elem : list){
            arr.add(elem.get());
        }
    }
};  