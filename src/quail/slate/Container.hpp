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
};