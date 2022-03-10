/**
 * \file
 *
 * \brief Starts Ethernet, GMAC and TCP tasks
 *
 * Copyright (c) 2019 Microchip Technology Inc. and its subsidiaries.
 *
 * \asf_license_start
 *
 * \page License
 *
 * Subject to your compliance with these terms, you may use Microchip
 * software and any derivatives exclusively with Microchip products.
 * It is your responsibility to comply with third party license terms applicable
 * to your use of third party software (including open source software) that
 * may accompany Microchip software.
 *
 * THIS SOFTWARE IS SUPPLIED BY MICROCHIP "AS IS". NO WARRANTIES,
 * WHETHER EXPRESS, IMPLIED OR STATUTORY, APPLY TO THIS SOFTWARE,
 * INCLUDING ANY IMPLIED WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY,
 * AND FITNESS FOR A PARTICULAR PURPOSE. IN NO EVENT WILL MICROCHIP BE
 * LIABLE FOR ANY INDIRECT, SPECIAL, PUNITIVE, INCIDENTAL OR CONSEQUENTIAL
 * LOSS, DAMAGE, COST OR EXPENSE OF ANY KIND WHATSOEVER RELATED TO THE
 * SOFTWARE, HOWEVER CAUSED, EVEN IF MICROCHIP HAS BEEN ADVISED OF THE
 * POSSIBILITY OR THE DAMAGES ARE FORESEEABLE.  TO THE FULLEST EXTENT
 * ALLOWED BY LAW, MICROCHIP'S TOTAL LIABILITY ON ALL CLAIMS IN ANY WAY
 * RELATED TO THIS SOFTWARE WILL NOT EXCEED THE AMOUNT OF FEES, IF ANY,
 * THAT YOU HAVE PAID DIRECTLY TO MICROCHIP FOR THIS SOFTWARE.
 *
 * \asf_license_stop
 *
 */

#include "eth_ipstack_main.h"
#include "webserver_tasks.h"
#include "lwip/tcpip.h"
#include "ethernet_phy_main.h"

uint16_t led_blink_rate = BLINK_NORMAL;

gmac_device          gs_gmac_dev;
static bool          link_up   = false;
volatile static bool recv_flag = false;

static TaskHandle_t xLed_Task;

/**
 * OS task that blinks LED
 */
static void led_task(void *p)
{
	(void)p;
	for (;;) {
		//gpio_toggle_pin_level(LED_0);
		os_sleep(led_blink_rate);
	}
}

/**
 * \brief Callback for GMAC interrupt.
 * Give semaphore for which gmac_task waits
 */
void gmac_handler_cb(void)
{
	portBASE_TYPE xGMACTaskWoken = pdFALSE;
	xSemaphoreGiveFromISR(gs_gmac_dev.rx_sem, &xGMACTaskWoken);
	portEND_SWITCHING_ISR(xGMACTaskWoken);
}

/**
 * \brief Invoked after completion of TCP/IP init
 */
void tcpip_init_done(void *arg)
{
	sys_sem_t *sem;
	sem         = (sys_sem_t *)arg;
	u8_t mac[6] = {0x00, 0x00, 0x00, 0x00, 0x20, 0x76};
	mac_async_register_callback(&COMMUNICATION_IO, MAC_ASYNC_RECEIVE_CB, gmac_handler_cb);
	hri_gmac_set_IMR_RCOMP_bit(COMMUNICATION_IO.dev.hw);

	while ((ethernet_phy_get_link_status(&ETHERNET_PHY_0_desc, &link_up)) != ERR_NONE && !(link_up)) {
		os_sleep(20);
	}

	printf("\r\nEthernet link up\r\n\r\n");

	/* Enable NVIC GMAC interrupt. */
	/* Interrupt priorities. (lowest value = highest priority) */
	/* ISRs using FreeRTOS *FromISR APIs must have priorities below or equal to */
	/* configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY. */
	NVIC_SetPriority(GMAC_IRQn, 4);
	NVIC_EnableIRQ(GMAC_IRQn);
	mac_async_enable(&COMMUNICATION_IO);

	TCPIP_STACK_INTERFACE_0_init(mac);

	TCPIP_STACK_INTERFACE_0_desc.input = tcpip_input;

	gs_gmac_dev.netif = &TCPIP_STACK_INTERFACE_0_desc;

	sys_thread_t id;

	/* Incoming packet notification semaphore. */
	gs_gmac_dev.rx_sem.sem = xSemaphoreCreateCounting(CONF_GMAC_RXDESCR_NUM, 0);

	id = sys_thread_new("GMAC", gmac_task, &gs_gmac_dev, netifINTERFACE_TASK_STACK_SIZE, netifINTERFACE_TASK_PRIORITY);
	LWIP_ASSERT("ethernetif_init: GMAC Task allocation ERROR!\n", (id != 0));

	netif_set_default(&TCPIP_STACK_INTERFACE_0_desc);

#if CONF_TCPIP_STACK_INTERFACE_0_DHCP
	/* DHCP mode. */
	if (ERR_OK != dhcp_start(&TCPIP_STACK_INTERFACE_0_desc)) {
		LWIP_ASSERT("ERR_OK != dhcp_start", 0);
	}
#else
	netif_set_up(&TCPIP_STACK_INTERFACE_0_desc);

#endif

	sys_sem_signal(sem); /* Signal the waiting thread that the TCP/IP init is done. */
}

/**
 * \brief Task for GMAC.
 * Waits for GMAC interrupt and begins processing of received packets
 */
void gmac_task(void *pvParameters)
{
	gmac_device *ps_gmac_dev = pvParameters;

	while (1) {
		/* Wait for the counting RX notification semaphore. */
		xSemaphoreTake(ps_gmac_dev->rx_sem, portMAX_DELAY);

		/* Process the incoming packet. */
		ethernetif_mac_input(ps_gmac_dev->netif);
	}
}

/**
 * \brief Create OS task for LED blinking
 */
void task_led_create(void)
{

	/* Create task to make led blink */
	if (xTaskCreate(led_task, "Led", TASK_LED_STACK_SIZE, NULL, TASK_LED_TASK_PRIORITY, &xLed_Task) != pdPASS) {
		while (1) {
			;
		}
	}
}
