#pragma once

#include "lwip/api.h"
#include "Task.hpp"

extern struct mac_async_descriptor COMMUNICATION_IO;

class SSIEth: public Task<4000>
{
public:
    SSIEth(uint8_t priority):Task(priority, "Ethernet"){};
    void activity();
};


