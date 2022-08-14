#pragma once

#include "lwip/api.h"
#include "lwip/netif.h"
#include "Task.hpp"
#include "EthMAC.h"
#include "PHY.h"

class MacAddr
{
public:
    u8_t mac_addr[6];
};

class SSIEth : public Task<500>
{
private:

public:
    SSIEth(uint8_t priority, MacAddr macAddr) : Task(priority, "Ethernet"), macAddr(macAddr) {};
    
private:

    void activity();

    static void lwip_setup(void *arg) { ((SSIEth *)arg)->lwip_setup();}
    void lwip_setup();

    static err_t netif_init(struct netif *netif);

    EthMAC ethMAC = EthMAC(GMAC);
    PHY phy = PHY(ethMAC, 0);

    MacAddr macAddr;
    
    sys_sem_t rx_sem;

    struct netif lwip_netif;

    static void rx_frame_cb(void*);
    static struct pbuf *low_level_input(struct netif *netif);
    static void ethernetif_mac_input(struct netif *netif);
    static err_t mac_low_level_output(struct netif *netif, struct pbuf *p);
};
