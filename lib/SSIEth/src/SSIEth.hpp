#pragma once

#include "lwip/api.h"

extern struct mac_async_descriptor COMMUNICATION_IO;

class SSIEth
{
private:
    static void startup_activity(void *p);
public:
    static void init();
    static void test();
};


