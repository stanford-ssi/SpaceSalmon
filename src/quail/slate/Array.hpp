#include "SlateKey.hpp"

template <typename T, int size>
class Array : public SlateKeyGeneric
{
    public:
        Array(const std::string id): SlateKeyGeneric(id){};
        SlateKey<T> *list[size];

        SlateKey<T>& operator[](int index)
        {
            return *list[index];
        }
};