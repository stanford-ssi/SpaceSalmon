#include "PyroSquib.h"
#include "main.hpp"

PyroSquib::PyroSquib(SPIClass &spi, int8_t cspin) : squib(spi, cspin){
    squib_mx.take(portMAX_DELAY);
}

void PyroSquib::init(){
    squib.Init();
    pinMode(34,OUTPUT);
    digitalWrite(34,LOW); //ARMED!
    squib_mx.give();
}

void PyroSquib::arm(){
    armed = true;
}

void PyroSquib::disarm(){
    armed = false;
}

bool PyroSquib::fire(PyroChannel channel){
    if(armed){
        squib_mx.take(portMAX_DELAY);
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
        squib_mx.give();
        return true;
    }else{
        return false;
    }
}

bool PyroSquib::getStatus(PyroChannel channel){
    squib_mx.take(portMAX_DELAY);
    squib.getStatus();
    bool ret = false;
    if(channel == SquibA){
        ret = (squib.status.Squib_Stat1AResistance <= 63);
    }else if(channel == SquibB){
        ret = (squib.status.Squib_Stat1BResistance <= 63);
    }else if(channel == SquibC){
        ret = (squib.status.Squib_Stat2AResistance <= 63);
    }else if(channel == SquibD){
        ret = (squib.status.Squib_Stat2BResistance <= 63);
    }
    squib_mx.give();
    return ret;
}

void PyroSquib::getResistance(){
    squib_mx.take(portMAX_DELAY);
    squib.getStatus();

    StaticJsonDocument<1000> res_json;

    res_json.add(squib.status.Squib_Stat1AResistance);
    res_json.add(squib.status.Squib_Stat1BResistance);
    res_json.add(squib.status.Squib_Stat2AResistance);
    res_json.add(squib.status.Squib_Stat2BResistance);
    sys.tasks.logger.logJSON(res_json,"res");

    squib_mx.give();
}