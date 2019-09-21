#include "Sensor.hpp"
#include <cstring>

Sensor::Sensor(char * newId){
    strncpy(id, newId, sizeof(id));
}

const char * Sensor::getID(){
    return id;
}

SensorStatus Sensor::getStatus(){
    return Init;
}