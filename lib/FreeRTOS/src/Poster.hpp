#pragma once
#include "Mutex.hpp"

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