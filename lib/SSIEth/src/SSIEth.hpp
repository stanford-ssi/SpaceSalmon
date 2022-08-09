#pragma once

#include "lwip/api.h"
#include "lwip/netif.h"
#include "Task.hpp"
#include "EthMAC.h"
#include "ieee8023_mii_standard_config.h"
#include "PHY.h"

class MacAddr
{
public:
    u8_t mac_addr[6];
};

class SSIEth : public Task<4000>
{
public:
    static void lwip_init_done_cb(void *arg)
    {
        ((SSIEth *)arg)->lwip_init_done();
    }

    EthMAC ethMAC = EthMAC(GMAC);
    PHY phy = PHY(ethMAC, CONF_ETHERNET_PHY_0_IEEE8023_MII_PHY_ADDRESS);
    SSIEth(uint8_t priority, MacAddr macAddr) : Task(priority, "Ethernet"), macAddr(macAddr) { global_eth = this; };
    void activity();
    void lwip_init_done();

    MacAddr macAddr;

    sys_sem_t lwip_init_sem;

    /** RX task notification semaphore. */
    sys_sem_t rx_sem;

    bool link_up = false;
    volatile bool recv_flag = false;

    static void gmac_task_cb(void *arg)
    {
        ((SSIEth *)arg)->gmac_task();
    }

    void gmac_task();

    static SSIEth *global_eth;

    static void gmac_handler_cb(void);

    static err_t netif_init(struct netif *netif);

    struct netif lwip_netif;
};
