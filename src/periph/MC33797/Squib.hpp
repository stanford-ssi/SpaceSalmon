#pragma once

#include "MC33797.h"

class Squib
{
  public:
    typedef Squib_StatusType Status;
    typedef Squib_FireType Fire;
    //contructor, provide SPI interface and cspin
    Squib(struct spi_m_sync_descriptor *spi, int8_t cspin);
    //fire some squibs!
    Squib_ReturnType fire(Fire fire);
    //read the status
    Squib_ReturnType getStatus();
    //here is the status
    Status status;
};