#include "SlateKey.hpp"

template <typename T>
class Array : public SlateKeyGeneric
{
    const std::initializer_list<T>& list;
    public:
        Array(const std::string id, std::initializer_list<T> keys): SlateKeyGeneric(id), list(keys){};
        
        T operator[](int index)
        {
            return *(list.begin() + index);
        }
};  