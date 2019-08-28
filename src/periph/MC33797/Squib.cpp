/*
TODO: This library works, but can only be used once. It needs to be re-writen as a C++ class that can be instantiated multiple times.
*/

#include "Squib.hpp"

Squib::Squib(struct spi_m_sync_descriptor *spi, int8_t cspin)
{
    Squib_Init(spi, cspin);
}

Squib_ReturnType Squib::fire(Squib_FireType fire)
{
    return Squib_Fire(fire);
}

Squib_ReturnType Squib::getStatus()
{
    return Squib_GetStatus(&status);
}