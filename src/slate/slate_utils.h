#pragma once
#include <stdint.h>
#include "FreeRTOS.h"
#include "portmacro.h"

template <typename T, typename P>
inline void seralize_field(P &pkt , T val, uint32_t offset){
    *(T*)(((uint8_t*)(&pkt)) + offset) = val;
}



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

class Slate
{
public:
    virtual void set_uint32_t_field(uint16_t offset, uint32_t value){}
    virtual void set_int16_t_field(uint16_t offset, int16_t value){}
    virtual void set_bool_field(uint16_t offset, bool value){}
    virtual void set_float_field(uint16_t offset, float value){}
    
    virtual const uint64_t& get_metaslate_hash(){ return metaslate_hash; }
private:
    static constexpr uint64_t metaslate_hash = 0;
};