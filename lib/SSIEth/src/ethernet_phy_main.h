/*
 * Code generated from Atmel Start.
 *
 * This file will be overwritten when reconfiguring your Atmel Start project.
 * Please copy examples or other code you want to keep to a separate file or main.c
 * to avoid loosing it when reconfiguring.
 */
#ifndef ETHERNET_PHY_MAIN_H
#define ETHERNET_PHY_MAIN_H

#pragma once

#include "ieee8023_mii_standard_config.h"
#include <ethernet_phy.h>

extern struct mac_async_descriptor COMMUNICATION_IO;

extern PHY phy;

#ifdef __cplusplus
extern "C" {
#endif


void ethernet_phys_init(void);
void ETHERNET_PHY_0_example(void);

/**
 * \brief Ethernet PHY devices
 */
void ethernet_phys_init(void);

#ifdef __cplusplus
}
#endif

#endif /* ETHERNET_PHY_MAIN_H */
