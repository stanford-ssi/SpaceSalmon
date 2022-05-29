#include "PyroSquib.h"
#include "main.hpp"

PyroSquib::PyroSquib(SPIClass &spi, int8_t cspin) : squib(spi, cspin){

}

void PyroSquib::init(){
    squib.Init();
    pinMode(34,OUTPUT);
    digitalWrite(34,LOW); //ARMED!
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
            squib.fire(CMD_FIRE_1A);
        }else if(channel == SquibB){
            squib.fire(CMD_FIRE_1B);
        }else if(channel == SquibC){
            squib.fire(CMD_FIRE_2A);
        }else if(channel == SquibD){
            squib.fire(CMD_FIRE_2B);
        }else if (channel == SquibNone){
            squib.fire(CMD_FIRE_NO_SQUIBS);
        }
        return true;
    }else{
        return false;
    }
}

bool PyroSquib::getStatus(PyroChannel channel){
    squib.getStatus();
    if(channel == SquibA){
        return (squib.status.Squib_Stat1AResistance <= 63);
    }else if(channel == SquibB){
        return (squib.status.Squib_Stat1BResistance <= 63);
    }else if(channel == SquibC){
        return (squib.status.Squib_Stat2AResistance <= 63);
    }else if(channel == SquibD){
        return (squib.status.Squib_Stat2BResistance <= 63);
    }
}