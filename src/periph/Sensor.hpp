#pragma once

enum SensorStatus
{
    Init,
    Alive,
    Error
};

class Sensor
{
public:
    Sensor(char* newId);
    void readData(); //read data from sensor, store it internally
    SensorStatus getStatus(); //get the status of the sensor
    void init(); //initialize the sensor
    const char* getID();
protected:
    char id[5];
};

