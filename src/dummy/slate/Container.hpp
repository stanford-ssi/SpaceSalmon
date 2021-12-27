#include "SlateKey.hpp"

template <int size>
class Container : public SlateKeyGeneric
{
    public:
        Container(const std::string id): SlateKeyGeneric(id){};
        SlateKeyGeneric *list[size];
        
        SlateKeyGeneric& operator[](int index)
        {
            return *list[index];
        }

        void dump(JsonVariant dst){
            const JsonObject obj = dst.createNestedObject(id);
            for(SlateKeyGeneric *elem : list){
                elem->dump(obj);
            }
        }
};