#pragma once

#include "SSIEth.hpp"

class SlateServer{
    public:
        result_t bind(uint16_t port);
        result_t connect(ip4_addr_t addr, uint16_t port);
        result_t send(const uint8_t* message, uint16_t len);

    private:
        bool isBound = false;
        bool isConnected = false;

        netconn *slate_netconn;
};