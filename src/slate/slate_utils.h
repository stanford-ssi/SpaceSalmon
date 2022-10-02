#pragma once
#include <stdint.h>
#include "FreeRTOS.h"
#include "portmacro.h"

template <typename T, typename P>
inline void seralize_field(P &pkt , T val, uint32_t offset){
    *(T*)(((uint8_t*)(&pkt)) + offset) = val;
}

typedef struct
{
    uint64_t slate_hash;
    uint16_t offset;
    uint8_t data[8];
} cmd_pkt_t;


template <typename T>
class SlateField
{
private:
    T value;

public:
    SlateField(T init) : value(init){};
    T get()
    {
        // omitting critical section for now. Most fields are atomic anyway...
        return value;
    }

    void set(T new_value)
    {
        // omitting critical section for now. Most fields are atomic anyway...
        value = new_value;
    }

    bool CAS(T compare, T set)
    {
        vPortEnterCritical();
        bool valid = (value == compare);
        if (valid)
        {
            value = set;
        }
        vPortExitCritical();
        return valid;
    }
};