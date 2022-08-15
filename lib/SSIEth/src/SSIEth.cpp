#include "SSIEth.hpp"

#include "rtos_port.h"
#include "config/hpl_gmac_config.h"
#include <string.h>
#include "lwip/sys.h"
#include "lwip/tcpip.h"
#include "lwip/etharp.h"
#include <lwip/dhcp.h>
#include <lwip/ip_addr.h>
#include "netif/ethernet.h"
#include "gpio.h"

void SSIEth::activity()
{

	vTaskDelay(3000);

	hri_mclk_set_AHBMASK_GMAC_bit(MCLK);
	hri_mclk_set_APBCMASK_GMAC_bit(MCLK);

	gpio_set_pin_function(GPIO(GPIO_PORTB, 14), PINMUX_PB14L_GMAC_GMDC);
	gpio_set_pin_function(GPIO(GPIO_PORTB, 15), PINMUX_PB15L_GMAC_GMDIO);
	gpio_set_pin_function(GPIO(GPIO_PORTA, 13), PINMUX_PA13L_GMAC_GRX0);
	gpio_set_pin_function(GPIO(GPIO_PORTA, 12), PINMUX_PA12L_GMAC_GRX1);
	gpio_set_pin_function(GPIO(GPIO_PORTC, 20), PINMUX_PC20L_GMAC_GRXDV);
	gpio_set_pin_function(GPIO(GPIO_PORTA, 15), PINMUX_PA15L_GMAC_GRXER);
	gpio_set_pin_function(GPIO(GPIO_PORTA, 18), PINMUX_PA18L_GMAC_GTX0);
	gpio_set_pin_function(GPIO(GPIO_PORTA, 19), PINMUX_PA19L_GMAC_GTX1);
	gpio_set_pin_function(GPIO(GPIO_PORTA, 14), PINMUX_PA14L_GMAC_GTXCK);
	gpio_set_pin_function(GPIO(GPIO_PORTA, 17), PINMUX_PA17L_GMAC_GTXEN);

	mac.init();

	/* Incoming packet notification semaphore. */
	rx_sem.sem = xSemaphoreCreateCounting(CONF_GMAC_RXDESCR_NUM, 0);

	tcpip_init(lwip_setup, this);

	while (true)
	{
		/* Wait for the counting RX notification semaphore. */
		xSemaphoreTake((QueueHandle_t)rx_sem.sem, portMAX_DELAY);

		/* Process the incoming packet. */
		ethernetif_mac_input(&lwip_netif);
	}
}

void SSIEth::lwip_setup()
{
	ip4_addr_t ip;
	ip4_addr_t nm;
	ip4_addr_t gw;

	IP4_ADDR(&ip, 192, 168, 2, 2);
	IP4_ADDR(&nm, 255, 255, 255, 0);
	IP4_ADDR(&gw, 192, 168, 2, 1);

	netif_add(&lwip_netif,
			  &ip,
			  &nm,
			  &gw,
			  this,
			  netif_init,
			  ethernet_input);
}

err_t SSIEth::netif_init(struct netif *netif)
{
	LWIP_ASSERT("netif != NULL", (netif != NULL));
	LWIP_ASSERT("netif->state != NULL", (netif->state != NULL));

	SSIEth *Eth = (SSIEth *)netif->state;

	Eth->mac.register_callback(MAC_ASYNC_RECEIVE_CB, rx_frame_cb, Eth);

	bool link_up = false;
	while ((Eth->phy.get_link_status(link_up)) != ERR_NONE && !(link_up))
	{
		vTaskDelay(20);
	}

	printf("Ethernet link up\n");

	netif->input = tcpip_input;
	netif->output = etharp_output;
	netif->linkoutput = mac_low_level_output;

	memcpy(netif->hwaddr, Eth->macAddr.mac_addr, NETIF_MAX_HWADDR_LEN);
	netif->hwaddr_len = ETHARP_HWADDR_LEN;
	netif->flags = NETIF_FLAG_BROADCAST | NETIF_FLAG_ETHARP | NETIF_FLAG_ETHERNET;
	netif->mtu = 1500;
	memcpy(netif->name, "en", 2);

	// Set MAC address filter
	struct mac_async_filter filter;
	memcpy(filter.mac, netif->hwaddr, NETIF_MAX_HWADDR_LEN);
	filter.tid_enable = false;
	Eth->mac.set_filter(0, &filter);

	Eth->mac.enable();

	netif_set_default(netif);
	netif_set_up(netif);
	netif_set_link_up(netif);

	return ERR_OK;
}

/**
 * \brief Called by MAC driver when frame received.
 */
void SSIEth::rx_frame_cb(void *arg)
{
	if (arg != nullptr)
	{
		SSIEth *ethObj = (SSIEth *)arg;
		portBASE_TYPE xGMACTaskWoken = pdFALSE;
		xSemaphoreGiveFromISR((QueueHandle_t)ethObj->rx_sem.sem, &xGMACTaskWoken);
		portEND_SWITCHING_ISR(xGMACTaskWoken);
	}
}

/**
 * \berif Transmission packet though the MAC hardware.
 */
err_t SSIEth::mac_low_level_output(struct netif *netif, struct pbuf *p)
{
	MAC *mac = &((SSIEth *)netif->state)->mac;

	pbuf_header(p, -ETH_PAD_SIZE); /* drop the padding word */

	if (p->tot_len == p->len)
	{
		mac->write(p);
	}
	else
	{
		printf("ERROR NOT YET IMPLEMENTED!\n");
	}

	pbuf_header(p, ETH_PAD_SIZE); /* reclaim the padding word */

	LINK_STATS_INC(link.xmit);

	return ERR_OK;
}

/**
 * Should allocate a pbuf and transfer the bytes of the incoming
 * packet from the interface into the pbuf.
 *
 * @param netif the lwip network interface structure for this ethernetif
 * @return a pbuf filled with the received packet (including MAC header)
 *         NULL on memory error
 */
struct pbuf *SSIEth::low_level_input(struct netif *netif)
{
	struct pbuf *p;
	uint32_t len;

	MAC *mac = &((SSIEth *)netif->state)->mac;
	mac->read_len(len); /* Obtain the size of the packet */
	if (len == 0)
	{
		return NULL;
	}

	len += ETH_PAD_SIZE; /* allow room for Ethernet padding */

	/* Allocate a pbuf as one large chunk, This include protocol header */
	p = pbuf_alloc(PBUF_RAW, (uint16_t)len, PBUF_RAM);

	if (p != NULL)
	{
		pbuf_header(p, -ETH_PAD_SIZE); /* drop the padding word */

		/* Read the entire packet into the pbuf. */
		uint32_t rx_len;
		mac->read((uint8_t *)p->payload, p->len, rx_len);

		pbuf_header(p, ETH_PAD_SIZE); /* reclaim the padding word */

		LINK_STATS_INC(link.recv);
	}
	else
	{
		uint32_t rx_len;
		mac->read(NULL, 0, rx_len);
		LINK_STATS_INC(link.memerr);
		LINK_STATS_INC(link.drop);
	}

	return p;
}

/**
 * \brief Process incoming ethernet packet.
 */
void SSIEth::ethernetif_mac_input(struct netif *netif)
{
	struct eth_hdr *ethhdr;
	struct pbuf *p;

	/* move received packet into a new pbuf */
	do
	{
		p = low_level_input(netif);
		/* no packet could be read, silently ignore this */
		if (p == NULL)
		{
			return;
		}
		/* points to packet payload, which starts with an Ethernet header */
		ethhdr = (struct eth_hdr *)p->payload;

		switch (htons(ethhdr->type))
		{
		/* IP or ARP packet? */
		case ETHTYPE_IP:
		case ETHTYPE_ARP:
			/* full packet send to tcpip_thread to process */
			if (netif->input(p, netif) != ERR_OK)
			{
				LWIP_DEBUGF(NETIF_DEBUG, ("ethernetif_mac_input: IP input error\n"));
				pbuf_free(p);
				p = NULL;
			}
			break;

		default:
			pbuf_free(p);
			p = NULL;
			break;
		}
	} while (true);
}
