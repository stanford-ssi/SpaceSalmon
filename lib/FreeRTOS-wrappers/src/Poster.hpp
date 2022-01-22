#pragma once
#include "Mutex.hpp"

/*
Poster is a class template that allows safe access to a single variable in a post()/get() style.
Use this if you want to provide a way to access the most recent value of a peice of data, but dont need every sample (in that case, you would use a buffer).
This is useful for telemetry and diagnostic indicators. 
*/

template <typename T>
class Poster
{
private:
    T safe;
    Mutex m;

public:
    Poster() {}

    Poster(const T &data)
    {
        post(data);
    }

    void post(const T &data)
    {
        m.take(NEVER);
        safe = data;
        m.give();
    }

    void get(T &data)
    {
        m.take(NEVER);
        data = safe;
        m.give();
    }

    operator T()
    {
        T copy;
        m.take(NEVER);
        copy = safe;
        m.give();
        return copy;
    }
};