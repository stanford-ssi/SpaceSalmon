/**
 * \file
 *
 * \brief LwIP Ethernet Interface functionality implement.
 *
 * Copyright (c) 2016-2018 Microchip Technology Inc. and its subsidiaries.
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

#include "ethif_mac.h"
#include "lwip/opt.h"
#include "lwip/def.h"
#include "lwip/mem.h"
#include "lwip/pbuf.h"
#include <lwip/stats.h>
#include <lwip/snmp.h>
#include "lwip/etharp.h"
#include "netif/ethernet.h"
#include <string.h>

#include "hal_rtos.h"

#include "EthMAC.h"

/**
 * \berif Transmission packet though the MAC hardware.
 */
err_t mac_low_level_output(struct netif *netif, struct pbuf *p)
{
	struct pbuf *                q;
	void *                       tbuf;
	uint8_t *                    pos;

	EthMAC* mac = (EthMAC*)(netif->state);

#if ETH_PAD_SIZE
	pbuf_header(p, -ETH_PAD_SIZE); /* drop the padding word */
#endif

	if (p->tot_len == p->len) {
		mac->write((uint8_t*)p->payload, p->tot_len);
	} else {
		tbuf = mem_malloc(LWIP_MEM_ALIGN_SIZE(p->tot_len));
		pos  = (uint8_t*)tbuf;
		if (tbuf == NULL) {
			return ERR_MEM;
		}
		for (q = p; q != NULL; q = q->next) {
			memcpy(pos, q->payload, q->len);
			pos += q->len;
		}
		mac->write((uint8_t*)tbuf, p->tot_len);
		mem_free(tbuf);
	}

#if ETH_PAD_SIZE
	pbuf_header(p, ETH_PAD_SIZE); /* reclaim the padding word */
#endif

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
static struct pbuf *low_level_input(struct netif *netif)
{
	struct pbuf *                p;
	uint32_t                        len;

	EthMAC* mac = (EthMAC*)(netif->state);
	mac->read_len(len); /* Obtain the size of the packet */
	if (len == 0) {
		return NULL;
	}

#if ETH_PAD_SIZE
	len += ETH_PAD_SIZE; /* allow room for Ethernet padding */
#endif

	/* Allocate a pbuf as one large chunk, This include protocol header */
	p = pbuf_alloc(PBUF_RAW, (uint16_t)len, PBUF_RAM);

	if (p != NULL) {
#if ETH_PAD_SIZE
		pbuf_header(p, -ETH_PAD_SIZE); /* drop the padding word */
#endif

		/* Read the entire packet into the pbuf. */
		uint32_t rx_len;
		mac->read((uint8_t*)p->payload, p->len, rx_len);

#if ETH_PAD_SIZE
		pbuf_header(p, ETH_PAD_SIZE); /* reclaim the padding word */
#endif

		LINK_STATS_INC(link.recv);
	} else {
		uint32_t rx_len;
		mac->read(NULL,0,rx_len);
		LINK_STATS_INC(link.memerr);
		LINK_STATS_INC(link.drop);
	}

	return p;
}

/**
 * \brief Process incoming ethernet packet.
 */
void ethernetif_mac_input(struct netif *netif)
{
	struct eth_hdr *ethhdr;
	struct pbuf *   p;

	/* move received packet into a new pbuf */
	do {
		p = low_level_input(netif);
		/* no packet could be read, silently ignore this */
		if (p == NULL) {
			return;
		}
		/* points to packet payload, which starts with an Ethernet header */
		ethhdr = (struct eth_hdr *)p->payload;

		switch (htons(ethhdr->type)) {
		/* IP or ARP packet? */
		case ETHTYPE_IP:
		case ETHTYPE_ARP:
#if PPPOE_SUPPORT
			/* PPPoE packet? */
		case ETHTYPE_PPPOEDISC:
		case ETHTYPE_PPPOE:
#endif /* PPPOE_SUPPORT */
			/* full packet send to tcpip_thread to process */
			if (netif->input(p, netif) != ERR_OK) {
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
