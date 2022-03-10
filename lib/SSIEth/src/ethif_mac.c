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
#include "netif/etharp.h"
#include <string.h>

struct mac_async_descriptor COMMUNICATION_IO;

/**
 * \brief Initialize the MAC hardware
 */
void mac_low_level_init(struct netif *netif)
{
	struct mac_async_descriptor *mac;
	struct mac_async_filter      filter;

	mac = (struct mac_async_descriptor *)(netif->state);

	/* set MAC hardware address */
	memcpy(filter.mac, netif->hwaddr, NETIF_MAX_HWADDR_LEN);
	filter.tid_enable = false;
	mac_async_set_filter(mac, 0, &filter);
}

/**
 * \berif Transmission packet though the MAC hardware.
 */
err_t mac_low_level_output(struct netif *netif, struct pbuf *p)
{
	struct mac_async_descriptor *mac;
	struct pbuf *                q;
	void *                       tbuf;
	uint8_t *                    pos;

	mac = (struct mac_async_descriptor *)(netif->state);

#if ETH_PAD_SIZE
	pbuf_header(p, -ETH_PAD_SIZE); /* drop the padding word */
#endif

	if (p->tot_len == p->len) {
		mac_async_write(mac, p->payload, p->tot_len);
	} else {
		tbuf = mem_malloc(LWIP_MEM_ALIGN_SIZE(p->tot_len));
		pos  = tbuf;
		if (tbuf == NULL) {
			return ERR_MEM;
		}
		for (q = p; q != NULL; q = q->next) {
			memcpy(pos, q->payload, q->len);
			pos += q->len;
		}
		mac_async_write(mac, tbuf, p->tot_len);
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
	struct mac_async_descriptor *mac;
	struct pbuf *                p;
	u16_t                        len;

	mac = (struct mac_async_descriptor *)(netif->state);
	len = mac_async_read_len(mac); /* Obtain the size of the packet */
	if (len == 0) {
		return NULL;
	}

#if ETH_PAD_SIZE
	len += ETH_PAD_SIZE; /* allow room for Ethernet padding */
#endif

	/* Allocate a pbuf as one large chunk, This include protocol header */
	p = pbuf_alloc(PBUF_RAW, len, PBUF_RAM);

	if (p != NULL) {
#if ETH_PAD_SIZE
		pbuf_header(p, -ETH_PAD_SIZE); /* drop the padding word */
#endif

		/* Read the entire packet into the pbuf. */
		mac_async_read(mac, p->payload, p->len);

#if ETH_PAD_SIZE
		pbuf_header(p, ETH_PAD_SIZE); /* reclaim the padding word */
#endif

		LINK_STATS_INC(link.recv);
	} else {
		mac_async_read(mac, NULL, 0);
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
		ethhdr = p->payload;

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
