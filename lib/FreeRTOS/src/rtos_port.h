/**
 * \file
 *
 * \brief FreeRtos HAL API declaration.
 *
 * Copyright (c) 2018 Microchip Technology Inc. and its subsidiaries.
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
#ifndef _RTOS_PORT_H_INCLUDED
#define _RTOS_PORT_H_INCLUDED

#ifdef __cplusplus
extern "C" {
#endif

#include "FreeRTOS.h"
#include "semphr.h"
#include "task.h"

/* Semaphore */

#define SEMAPHORE_ENABLED
#define MUTEX_ENABLED

/**
 * \brief Semaphore element type
 */
typedef SemaphoreHandle_t sem_handle_t;

typedef SemaphoreHandle_t mutex_handle_t;

typedef StaticSemaphore_t sem_buf_t;

typedef StaticSemaphore_t mutex_buf_t;

typedef sem_handle_t sem_t;

typedef mutex_handle_t mutex_t;

#define NEVER portMAX_DELAY

/* sleep */
#define os_sleep(tick) vTaskDelay(tick)

/* os lock */

#define os_lock() vTaskSuspendAll()
#define os_unlock() xTaskResumeAll()

#ifdef __cplusplus
}
#endif
#endif /* _RTOS_PORT_H_INCLUDED */
