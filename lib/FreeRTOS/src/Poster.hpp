#pragma once
#include "Mutex.hpp"

/*
Poster is a class template that allows safe access to a single variable in a post()/get() style.
Use this if you want to provide a way to access the most recent value of a peice of data, but dont need every sample (in that case, you would use a buffer).
This is useful for telemetry and diagnostic indicators. 
*/

template <typename T> 
class Poster { 
private: 
    T safe;
    Mutex m;
public: 
    void post(T& data);
    void get(T& data);
}; 

template <typename T> 
void Poster<T>::post(T& data) { 
    m.take(NEVER);
    safe = data;
    m.give();
} 

template <typename T> 
void Poster<T>::get(T& data) { 
    m.take(NEVER);
    data = safe;
    m.give();
}