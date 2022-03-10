/*
 * Code generated from Atmel Start.
 *
 * This file will be overwritten when reconfiguring your Atmel Start project.
 * Please copy examples or other code you want to keep to a separate file or main.c
 * to avoid loosing it when reconfiguring.
 */
#ifndef ETH_IPSTACK_MAIN_H
#define ETH_IPSTACK_MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

#include <ethif_mac.h>

extern struct netif TCPIP_STACK_INTERFACE_0_desc;

void TCPIP_STACK_INTERFACE_0_init(u8_t hwaddr[6]);

#ifdef __cplusplus
}
#endif

#endif /* ETH_IPSTACK_MAIN_H */
