/*
 * Code generated from Atmel Start.
 *
 * This file will be overwritten when reconfiguring your Atmel Start project.
 * Please copy examples or other code you want to keep to a separate file or main.c
 * to avoid loosing it when reconfiguring.
 */

#include <ieee8023_mii_standard_config.h>
#include <ethernet_phy_main.h>

extern struct mac_async_descriptor COMMUNICATION_IO;

struct ethernet_phy_descriptor ETHERNET_PHY_0_desc;

void ETHERNET_PHY_0_init(void)
{
	mac_async_enable(&COMMUNICATION_IO);
	ethernet_phy_init(&ETHERNET_PHY_0_desc, &COMMUNICATION_IO, CONF_ETHERNET_PHY_0_IEEE8023_MII_PHY_ADDRESS);
#if CONF_ETHERNET_PHY_0_IEEE8023_MII_CONTROL_REG0_SETTING == 1
	ethernet_phy_write_reg(&ETHERNET_PHY_0_desc, MDIO_REG0_BMCR, CONF_ETHERNET_PHY_0_IEEE8023_MII_CONTROL_REG0);
#endif /* CONF_ETHERNET_PHY_0_IEEE8023_MII_CONTROL_REG0_SETTING */
}

void ETHERNET_PHY_0_example(void)
{
	bool    link_state;
	int32_t rst;
	/* Restart an auto-negotiation */
	rst = ethernet_phy_restart_autoneg(&ETHERNET_PHY_0_desc);
	while (rst != ERR_NONE) {
	}

	/* Wait for PHY link up */
	do {
		rst = ethernet_phy_get_link_status(&ETHERNET_PHY_0_desc, &link_state);
	} while (rst == ERR_NONE && link_state == true);
}

void ethernet_phys_init(void)
{

	ETHERNET_PHY_0_init();
}
