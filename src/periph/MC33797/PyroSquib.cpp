#include "PyroSquib.h"
#include "main.hpp"

PyroSquib::PyroSquib(){

}

void PyroSquib::arm(){
    armed = true;

}

void PyroSquib::disarm(){
    armed = false;

}

bool PyroSquib::fire(PyroChannel channel){
    if(armed){
        if(channel == SquibA){

        }else if(channel == SquibB){

        }else if (channel == SquibNone){

        }
        return true;
    }else{
        return false;
    }
}

bool PyroSquib::getStatus(PyroChannel channel){
    uint16_t value = 0;
    if(channel == SquibA){

    }else if(channel == SquibB){

    }
    return (value > 400);
}