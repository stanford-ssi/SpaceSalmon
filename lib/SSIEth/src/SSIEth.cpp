#include "SSIEth.hpp"

#include "hal_gpio.h"
#include "lwip/sys.h"
#include "lwip/tcpip.h"
#include "lwip/etharp.h"
#include "rtos_port.h"
#include "config/hpl_gmac_config.h"
#include "ethif_mac.h"
#include "lwip_macif_config.h"
#include <string.h>
#include <ethif_mac.h>
#include "netif/ethernet.h"
#include <lwip/dhcp.h>
#include <lwip/ip_addr.h>

SSIEth *SSIEth::global_eth = nullptr;

#define PA12 GPIO(GPIO_PORTA, 12)
#define PA13 GPIO(GPIO_PORTA, 13)
#define PA14 GPIO(GPIO_PORTA, 14)
#define PA15 GPIO(GPIO_PORTA, 15)
#define PA17 GPIO(GPIO_PORTA, 17)
#define PA18 GPIO(GPIO_PORTA, 18)
#define PA19 GPIO(GPIO_PORTA, 19)
#define PB14 GPIO(GPIO_PORTB, 14)
#define PB15 GPIO(GPIO_PORTB, 15)
#define PC20 GPIO(GPIO_PORTC, 20)

void SSIEth::activity()
{

	hri_mclk_set_AHBMASK_GMAC_bit(MCLK);
	hri_mclk_set_APBCMASK_GMAC_bit(MCLK);

	gpio_set_pin_function(PB14, PINMUX_PB14L_GMAC_GMDC);
	gpio_set_pin_function(PB15, PINMUX_PB15L_GMAC_GMDIO);
	gpio_set_pin_function(PA13, PINMUX_PA13L_GMAC_GRX0);
	gpio_set_pin_function(PA12, PINMUX_PA12L_GMAC_GRX1);
	gpio_set_pin_function(PC20, PINMUX_PC20L_GMAC_GRXDV);
	gpio_set_pin_function(PA15, PINMUX_PA15L_GMAC_GRXER);
	gpio_set_pin_function(PA18, PINMUX_PA18L_GMAC_GTX0);
	gpio_set_pin_function(PA19, PINMUX_PA19L_GMAC_GTX1);
	gpio_set_pin_function(PA14, PINMUX_PA14L_GMAC_GTXCK);
	gpio_set_pin_function(PA17, PINMUX_PA17L_GMAC_GTXEN);

	ethMAC.init();
	ethMAC.enable();
#if CONF_ETHERNET_PHY_0_IEEE8023_MII_CONTROL_REG0_SETTING == 1
	ethernet_phy_write_reg(&ETHERNET_PHY_0_desc, MDIO_REG0_BMCR, CONF_ETHERNET_PHY_0_IEEE8023_MII_CONTROL_REG0);
#endif /* CONF_ETHERNET_PHY_0_IEEE8023_MII_CONTROL_REG0_SETTING */

	sys_sem_new(&lwip_init_sem, 0); /* Create a new semaphore. */

	tcpip_init(lwip_init_done_cb, this);
	sys_sem_wait(&lwip_init_sem); /* Block until the lwIP stack is initialized. */
	sys_sem_free(&lwip_init_sem); /* Free the semaphore. */
	vTaskDelete(xTaskGetCurrentTaskHandle());
}

void SSIEth::lwip_init_done()
{
	NVIC_DisableIRQ(GMAC_IRQn);
	NVIC_ClearPendingIRQ(GMAC_IRQn);

	ethMAC.register_callback(MAC_ASYNC_RECEIVE_CB, gmac_handler_cb);

	hri_gmac_set_IMR_RCOMP_bit(ethMAC.hw);

	while ((phy.get_link_status(link_up)) != ERR_NONE && !(link_up))
	{
		vTaskDelay(20);
	}

	printf("\r\nEthernet link up\r\n\r\n");

	/* Enable NVIC GMAC interrupt. */
	/* Interrupt priorities. (lowest value = highest priority) */
	/* ISRs using FreeRTOS *FromISR APIs must have priorities below or equal to */
	/* configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY. */
	NVIC_SetPriority(GMAC_IRQn, 4);
	ethMAC.enable();

	ip4_addr_t ip;
	ip4_addr_t nm;
	ip4_addr_t gw;
#if CONF_TCPIP_STACK_INTERFACE_0_DHCP
	ip_addr_set_zero(&ip);
	ip_addr_set_zero(&nm);
	ip_addr_set_zero(&gw);
#else
	ipaddr_aton(CONF_TCPIP_STACK_INTERFACE_0_IP, &ip);
	ipaddr_aton(CONF_TCPIP_STACK_INTERFACE_0_NETMASK, &nm);
	ipaddr_aton(CONF_TCPIP_STACK_INTERFACE_0_GATEWAY, &gw);
#endif

	netif_add(&lwip_netif,
			  &ip,
			  &nm,
			  &gw,
			  this,
			  netif_init,
			  ethernet_input);

	lwip_netif.input = tcpip_input;

	sys_thread_t id;

	/* Incoming packet notification semaphore. */
	rx_sem.sem = xSemaphoreCreateCounting(CONF_GMAC_RXDESCR_NUM, 0);

	id = sys_thread_new("GMAC", gmac_task_cb, this, 512, 2);
	LWIP_ASSERT("ethernetif_init: GMAC Task allocation ERROR!\n", (id.thread_handle != NULL));

	netif_set_default(&lwip_netif);

#if CONF_TCPIP_STACK_INTERFACE_0_DHCP
	/* DHCP mode. */
	if (ERR_OK != dhcp_start(&TCPIP_STACK_INTERFACE_0_desc))
	{
		LWIP_ASSERT("ERR_OK != dhcp_start", 0);
	}
#else
	netif_set_up(&lwip_netif);
	netif_set_link_up(&lwip_netif);

#endif

	sys_sem_signal(&lwip_init_sem); /* Signal the waiting thread that the TCP/IP init is done. */
	NVIC_EnableIRQ(GMAC_IRQn);
}

/**
 * \brief Callback for GMAC interrupt.
 * Give semaphore for which gmac_task waits
 */
void SSIEth::gmac_handler_cb(void)
{
	if (SSIEth::global_eth != nullptr)
	{
		portBASE_TYPE xGMACTaskWoken = pdFALSE;
		xSemaphoreGiveFromISR((QueueHandle_t)SSIEth::global_eth->rx_sem.sem, &xGMACTaskWoken);
		portEND_SWITCHING_ISR(xGMACTaskWoken);
	}
}

/**
 * \brief Task for GMAC.
 * Waits for GMAC interrupt and begins processing of received packets
 */
void SSIEth::gmac_task()
{
	while (1)
	{
		/* Wait for the counting RX notification semaphore. */
		xSemaphoreTake((QueueHandle_t)rx_sem.sem, portMAX_DELAY);

		/* Process the incoming packet. */
		ethernetif_mac_input(&lwip_netif);
	}
}

err_t SSIEth::netif_init(struct netif *netif)
{
	LWIP_ASSERT("netif != NULL", (netif != NULL));
	LWIP_ASSERT("netif->state != NULL", (netif->state != NULL));

	netif->output = etharp_output;
	netif->linkoutput = mac_low_level_output;

	memcpy(netif->hwaddr, global_eth->macAddr.mac_addr, NETIF_MAX_HWADDR_LEN);
	netif->hwaddr_len = ETHARP_HWADDR_LEN;
	netif->flags = CONF_TCPIP_STACK_INTERFACE_0_FLAG;
	netif->mtu = CONF_TCPIP_STACK_INTERFACE_0_MTU;

#if LWIP_NETIF_HOSTNAME
	/* Initialize interface hostname */
	LWIP_MACIF_desc.hostname = CONF_TCPIP_STACK_INTERFACE_0_HOSTNAME;
#endif
	memcpy(netif->name, CONF_TCPIP_STACK_INTERFACE_0_HOSTNAME_ABBR, 2);

	/* initialize the mac hardware */
	struct mac_async_filter filter;

	/* set MAC hardware address */
	memcpy(filter.mac, netif->hwaddr, NETIF_MAX_HWADDR_LEN);
	filter.tid_enable = false;
	// mac_async_set_filter(mac, 0, &filter);
	global_eth->ethMAC.set_filter(0, &filter);

	return ERR_OK;
}