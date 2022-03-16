/**
 * webserver_tasks.h
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

#ifndef WEBSERVER_TASKS_H_
#define WEBSERVER_TASKS_H_

#include "FreeRTOS.h"
#include "task.h"
#include "lwip/sys.h"
#include "hpl_gmac_config.h"
#include "lwip_macif_config.h"
#include "arch/sys_arch.h"

extern struct mac_async_descriptor COMMUNICATION_IO;

void mac_receive_cb(struct mac_async_descriptor *desc);
void gmac_handler_cb(void);
void tcpip_init_done(void *arg);
void gmac_task(void *pvParameters);

typedef struct tag_gmac_device {
	/** Reference to lwIP netif structure. */
	struct netif *netif;

	/** RX task notification semaphore. */
	sys_sem_t rx_sem;

} gmac_device;


#endif /* WEBSERVER_TASKS_H_ */
