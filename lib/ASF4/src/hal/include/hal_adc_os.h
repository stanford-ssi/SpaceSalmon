/**
 * \file
 *
 * \brief ADC functionality declaration.
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

#ifndef HAL_ADC_OS_H_INCLUDED
#define HAL_ADC_OS_H_INCLUDED

#include <hal_rtos.h>
#include <hpl_adc_async.h>
#include <utils_ringbuffer.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \addtogroup doc_driver_hal_adc_os
 *
 * @{
 */

/**
 * \brief ADC descriptor
 *
 * The ADC descriptor forward declaration.
 */
struct adc_os_descriptor;

/**
 * \brief ADC os channel buffer descriptor
 */
struct adc_os_channel_descriptor {
	struct ringbuffer convert; /*!< Storage the ADC convert data */
	sem_t             rx_sem;  /*!< ADC read data semaphore */
	uint8_t *         rxbuf;   /*!< Pointer to data buffer to RX */
	uint16_t          size;    /*!< Size of data characters in RX */
	uint16_t          num;     /*!< The user wants to read data number */
};

/**
 * \brief ADC descriptor
 */
struct adc_os_descriptor {
	struct _adc_async_device          device;         /*!< Pointer to ADC device instance */
	struct adc_os_channel_descriptor *descr_ch;       /*!< Pointer to  ADC os channel instance */
	sem_t                             monitor_sem;    /*!< ADC window_threshold_reached semaphore */
	uint8_t *                         channel_map;    /*!< Enabled channel map */
	uint8_t                           channel_max;    /*!< Enabled maximum channel number */
	uint8_t                           channel_amount; /*!< Enabled channel amount */
};

/**
 * \brief Initialize ADC
 *
 * This function initializes the given ADC descriptor.
 * It checks if the given hardware is not initialized and if the given hardware
 * is permitted to be initialized.
 *
 * \param[out] descr         An ADC descriptor to initialize
 * \param[in] hw             The pointer to hardware instance
 * \param[in] channel_map    The pointer to adc channel mapping
 * \param[in] channel_max    ADC enabled maximum channel number
 * \param[in] channel_amount ADC enabled channel amount
 * \param[in] descr_ch       A buffer to keep all channel descriptor
 *
 * \return Initialization status.
 * \retval <0 Passed parameters were invalid or an ADC is already initialized
 * \retval ERR_NONE The initialization is completed successfully
 */
int32_t adc_os_init(struct adc_os_descriptor *const descr, void *const hw, uint8_t *channel_map, uint8_t channel_max,
                    uint8_t channel_amount, struct adc_os_channel_descriptor *const descr_ch);

/**
 * \brief Deinitialize ADC
 *
 * This function deinitializes the given ADC descriptor.
 * It checks if the given hardware is initialized and if the given hardware is
 * permitted to be deinitialized.
 *
 * \param[in] descr An ADC descriptor to deinitialize
 *
 * \return ERR_NONE
 */
int32_t adc_os_deinit(struct adc_os_descriptor *const descr);

/**
 * \brief Register ADC channel buffer
 *
 * This function initializes the given ADC channel buffer descriptor.
 *
 * \param[in] descr          An ADC descriptor to initialize
 * \param[in] channel        Channel number
 * \param[in] convert_buffer A buffer to keep converted values
 * \param[in] convert_buffer_length The length of the buffer above
 *
 * \return Initialization status.
 * \retval -1 Passed parameters were invalid or an ADC is already initialized
 * \retval 0 The initialization is completed successfully
 */
int32_t adc_os_register_channel_buffer(struct adc_os_descriptor *const descr, const uint8_t channel,
                                       uint8_t *const convert_buffer, const uint16_t convert_buffer_length);

/**
 * \brief Enable channel of ADC
 *
 * Use this function to set the ADC peripheral to enabled state.
 *
 * \param[in] descr    Pointer to the ADC descriptor
 * \param[in] channel  Channel number
 *
 * \return ERR_NONE
 */
int32_t adc_os_enable_channel(struct adc_os_descriptor *const descr, const uint8_t channel);

/**
 * \brief Disable channel of ADC
 *
 * Use this function to set the ADC peripheral to disabled state.
 *
 * \param[in] descr   Pointer to the ADC descriptor
 * \param[in] channel  Channel number
 *
 * \return ERR_NONE
 */
int32_t adc_os_disable_channel(struct adc_os_descriptor *const descr, const uint8_t channel);

/**
 * \brief Read data asked for from the ring buffer
 *
 *  If the number of data in ring buffer less than want to read,
 *  the task/thread will be blocked until read is done.
 *
 * \param[in] descr     The pointer to ADC descriptor
 * \param[in] channel   Channel number
 * \param[in] buf       A buffer to read data
 * \param[in] length    The size of the buffer
 *
 * \return The number of bytes the user wants to read.
 */
int32_t adc_os_read_channel(struct adc_os_descriptor *const descr, const uint8_t channel, uint8_t *const buffer,
                            const uint16_t length);

/**
 * \brief Start conversion
 *
 * This function starts single conversion if no automatic (free-run) mode is
 * enabled.
 *
 * \param[in] descr The pointer to ADC descriptor
 *
 * \return ERR_NONE
 */
int32_t adc_os_start_conversion(struct adc_os_descriptor *const descr);

/**
 * \brief Set ADC reference source
 *
 * This function sets ADC reference source.
 *
 * \param[in] descr     The pointer to ADC descriptor
 * \param[in] reference A reference source to set
 *
 * \return ERR_NONE
 */
int32_t adc_os_set_reference(struct adc_os_descriptor *const descr, const adc_reference_t reference);

/**
 * \brief Set ADC resolution
 *
 * This function sets ADC resolution.
 *
 * \param[in] descr      The pointer to ADC descriptor
 * \param[in] resolution A resolution to set
 *
 * \return ERR_NONE
 */
int32_t adc_os_set_resolution(struct adc_os_descriptor *const descr, const adc_resolution_t resolution);

/**
 * \brief Set ADC input source of a channel
 *
 * This function sets ADC positive and negative input sources.
 *
 * \param[in] descr     The pointer to ADC descriptor
 * \param[in] pos_input A positive input source to set
 * \param[in] neg_input A negative input source to set
 * \param[in] channel   Channel number
 *
 * \return ERR_NONE
 */
int32_t adc_os_set_inputs(struct adc_os_descriptor *const descr, const adc_pos_input_t pos_input,
                          const adc_neg_input_t neg_input, const uint8_t channel);

/**
 * \brief Set ADC conversion mode
 *
 * This function sets ADC conversion mode.
 *
 * \param[in] descr The pointer to ADC descriptor
 * \param[in] mode  A conversion mode to set
 *
 * \return ERR_NONE
 */
int32_t adc_os_set_conversion_mode(struct adc_os_descriptor *const descr, const enum adc_conversion_mode mode);

/**
 * \brief Set ADC differential mode
 *
 * This function sets ADC differential mode.
 *
 * \param[in] descr   The pointer to ADC descriptor
 * \param[in] channel Channel number
 * \param[in] mode    A differential mode to set
 *
 * \return ERR_NONE
 */
int32_t adc_os_set_channel_differential_mode(struct adc_os_descriptor *const descr, const uint8_t channel,
                                             const enum adc_differential_mode mode);

/**
 * \brief Set ADC gain
 *
 * This function sets ADC gain.
 *
 * \param[in] descr   The pointer to ADC descriptor
 * \param[in] channel Channel number
 * \param[in] gain    A gain to set
 *
 * \return ERR_NONE
 */
int32_t adc_os_set_channel_gain(struct adc_os_descriptor *const descr, const uint8_t channel, const adc_gain_t gain);

/**
 * \brief Set ADC window mode
 *
 * This function sets ADC window mode.
 *
 * \param[in] descr The pointer to ADC descriptor
 * \param[in] mode  A window mode to set
 *
 * \return ERR_NONE
 */
int32_t adc_os_set_window_mode(struct adc_os_descriptor *const descr, const adc_window_mode_t mode);

/**
 * \brief Set ADC thresholds
 *
 * This function sets ADC positive and negative thresholds.
 *
 * \param[in] descr         The pointer to ADC descriptor
 * \param[in] low_threshold A lower threshold to set
 * \param[in] up_threshold  An upper threshold to set
 *
 * \return ERR_NONE
 */
int32_t adc_os_set_thresholds(struct adc_os_descriptor *const descr, const adc_threshold_t low_threshold,
                              const adc_threshold_t up_threshold);

/**
 * \brief Retrieve threshold state
 *
 * This function retrieves ADC threshold state.
 *
 * \param[in] descr  The pointer to ADC descriptor
 * \param[out] state The threshold state
 *
 * \return ERR_NONE
 */
int32_t adc_os_get_threshold_state(const struct adc_os_descriptor *const descr, adc_threshold_status_t *const state);

/**
 * \brief Wait reach the ADC window threshhold
 *
 * This function waits for the reach of ADC window threshhold.
 * The task/thread will block until reach window threshhold.
 *
 * \param[in] descr The pointer to ADC descriptor
 * \param[in] channel Channel number
 *
 * \retval ERR_TIMEOUT The semaphore down timeout
 * \retval ERR_NONE The window threshhold has reached
 */
int32_t adc_os_wait_channel_threshhold_reach(struct adc_os_descriptor *const descr, const uint8_t channel);

/**
 * \brief Flush ADC ringbuf
 *
 * This function flushes ADC RX ringbuf.
 *
 * \param[in] descr    The pointer to ADC descriptor
 * \param[in] channel  Channel number
 *
 * \return ERR_NONE
 */
int32_t adc_os_flush_rx_buffer(struct adc_os_descriptor *const descr, const uint8_t channel);

/**
 * \brief Retrieve the current ADC RTOS driver version
 *
 * \return Current driver version.
 */
uint32_t adc_os_get_version(void);
/**@}*/

#ifdef __cplusplus
}
#endif

#endif /* HAL_ADC_OS_H_INCLUDED */
