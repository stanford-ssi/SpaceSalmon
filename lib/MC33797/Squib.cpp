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