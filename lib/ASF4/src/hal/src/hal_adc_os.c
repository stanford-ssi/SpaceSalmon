/**
 * \file
 *
 * \brief ADC functionality implementation.
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

#include <utils_assert.h>
#include <utils.h>
#include <hal_atomic.h>
#include <hal_adc_os.h>

/**
 * \brief Driver version
 */
#define HAL_ADC_OS_DRIVER_VERSION 0x00000001u

/**
 * \internal Process conversion completion
 *
 * \param[in] device   The pointer to ADC device structure
 * \param[in] channel  Channel number
 * \param[in] data     Converted data
 */
static void adc_os_channel_conversion_done(struct _adc_async_device *device, const uint8_t channel,
                                           const uint16_t data);
/**
 * \internal Process conversion window threshold reached
 *
 * \param[in] device The pointer to ADC device structure
 * \param[in] channel  Channel number
 */
static void adc_os_window_threshold_reached(struct _adc_async_device *device, const uint8_t channel);

/**
 * \internal Process conversion error
 *
 * \param[in] device The pointer to ADC device structure
 * \param[in] channel  Channel number
 */
static void adc_os_error_occured(struct _adc_async_device *device, const uint8_t channel);

/**
 * \brief Initialize ADC
 */
int32_t adc_os_init(struct adc_os_descriptor *const descr, void *const hw, uint8_t *channel_map, uint8_t channel_max,
                    uint8_t channel_amount, struct adc_os_channel_descriptor *const descr_ch)
{
	int32_t                   init_status;
	uint32_t                  i;
	struct _adc_async_device *device;

	ASSERT(descr && hw && channel_map && channel_amount && descr_ch);
	ASSERT(channel_amount <= (channel_max + 1));

	device = &descr->device;
	for (i = 0; i <= channel_max; i++) {
		channel_map[i] = 0xFF;
	}

	descr->channel_map    = channel_map;
	descr->channel_max    = channel_max;
	descr->channel_amount = channel_amount;
	descr->descr_ch       = descr_ch;

	init_status = _adc_async_init(device, hw);
	if (init_status) {
		return init_status;
	}

	init_status = sem_init(&descr->monitor_sem, 0);
	if (init_status < 0) {
		_adc_async_deinit(device);
		return init_status;
	}

	for (i = 0; i < channel_amount; i++) {
		init_status = sem_init(&descr_ch[i].rx_sem, 0);
		if (init_status < 0) {
			_adc_async_deinit(device);
			sem_deinit(&descr->monitor_sem);
			while (i > 0) {
				sem_deinit(&descr_ch[--i].rx_sem);
			}
			return init_status;
		}
	}

	device->adc_async_ch_cb.convert_done = adc_os_channel_conversion_done;
	device->adc_async_cb.window_cb       = adc_os_window_threshold_reached;
	device->adc_async_cb.error_cb        = adc_os_error_occured;

	return ERR_NONE;
}

/**
 * \brief Deinitialize ADC
 */
int32_t adc_os_deinit(struct adc_os_descriptor *const descr)
{
	ASSERT(descr);
	uint32_t i;

	_adc_async_deinit(&descr->device);

	sem_deinit(&descr->monitor_sem);
	for (i = 0; i < descr->channel_amount; i++) {
		sem_deinit(&descr->descr_ch[i].rx_sem);
	}

	return ERR_NONE;
}

/**
 * \brief Register channel buffer
 */
int32_t adc_os_register_channel_buffer(struct adc_os_descriptor *const descr, const uint8_t channel,
                                       uint8_t *const convert_buffer, const uint16_t convert_buffer_length)
{
	uint8_t i;
	uint8_t index = 0;

	ASSERT(descr && convert_buffer && convert_buffer_length);
	ASSERT(descr->channel_max >= channel);

	if (descr->channel_map[channel] != 0xFF) {
		return ERR_INVALID_ARG;
	}

	for (i = 0; i <= descr->channel_max; i++) {
		if (descr->channel_map[i] != 0xFF) {
			index++;
		}
	}

	if (index > descr->channel_amount) {
		return ERR_NO_RESOURCE;
	}
	if (ERR_NONE != ringbuffer_init(&descr->descr_ch[index].convert, convert_buffer, convert_buffer_length)) {
		return ERR_INVALID_ARG;
	}

	descr->channel_map[channel]  = index;
	descr->descr_ch[index].size  = 0;
	descr->descr_ch[index].num   = 0;
	descr->descr_ch[index].rxbuf = NULL;

	return ERR_NONE;
}

/**
 * \brief Enable ADC
 */
int32_t adc_os_enable_channel(struct adc_os_descriptor *const descr, const uint8_t channel)
{
	uint8_t index = 0;

	ASSERT(descr);
	ASSERT(descr->channel_max >= channel);

	index = descr->channel_map[channel];
	ASSERT(descr->channel_max >= index);

	_adc_async_enable_channel(&descr->device, channel);

	_adc_async_set_irq_state(&descr->device, channel, ADC_ASYNC_DEVICE_CONVERT_CB, true);

	return ERR_NONE;
}

/**
 * \brief Disable ADC
 */
int32_t adc_os_disable_channel(struct adc_os_descriptor *const descr, const uint8_t channel)
{
	ASSERT(descr);
	ASSERT(descr->channel_max >= channel);

	_adc_async_set_irq_state(&descr->device, channel, ADC_ASYNC_DEVICE_CONVERT_CB, false);
	_adc_async_disable_channel(&descr->device, channel);

	return ERR_NONE;
}

/*
 * \brief Read data from ADC
 */
int32_t adc_os_read_channel(struct adc_os_descriptor *const descr, const uint8_t channel, uint8_t *const buffer,
                            const uint16_t length)
{
	uint8_t  data_size;
	uint8_t  index     = 0;
	uint8_t  read_flag = 0;
	uint32_t num       = 0;
	uint32_t timeout   = ~0;
	uint16_t was_read  = 0;

	ASSERT(descr && buffer && length);
	ASSERT(descr->channel_max >= channel);

	data_size = _adc_async_get_data_size(&descr->device);
	ASSERT(!(length % data_size));
	(void)data_size;

	index                                      = descr->channel_map[channel];
	struct adc_os_channel_descriptor *descr_ch = &descr->descr_ch[index];

	CRITICAL_SECTION_ENTER()
	num = ringbuffer_num(&descr_ch->convert);
	if (num < length) {
		read_flag       = 1;
		descr_ch->size  = 0;
		descr_ch->num   = length;
		descr_ch->rxbuf = buffer;

		while (descr_ch->size < num) {
			ringbuffer_get(&descr_ch->convert, &descr_ch->rxbuf[descr_ch->size++]);
		}
	}
	CRITICAL_SECTION_LEAVE()

	if (read_flag == 1) {
		if (sem_down(&descr_ch->rx_sem, timeout) != 0) {
			return ERR_TIMEOUT;
		}
		return length;
	}

	while (was_read < length) {
		ringbuffer_get(&descr_ch->convert, &buffer[was_read++]);
	}

	return length;
}

/**
 * \brief reach window threshold conversion
 */
int32_t adc_os_wait_channel_threshhold_reach(struct adc_os_descriptor *const descr, const uint8_t channel)
{
	uint32_t timeout = ~0;

	ASSERT(descr);

	if (sem_down(&descr->monitor_sem, timeout) != 0) {
		return ERR_TIMEOUT;
	}

	return ERR_NONE;
}

/**
 * \brief Start conversion
 */
int32_t adc_os_start_conversion(struct adc_os_descriptor *const descr)
{
	ASSERT(descr);

	_adc_async_convert(&descr->device);

	return ERR_NONE;
}

/**
 * \brief Set ADC reference source
 */
int32_t adc_os_set_reference(struct adc_os_descriptor *const descr, const adc_reference_t reference)
{
	ASSERT(descr);

	_adc_async_set_reference_source(&descr->device, reference);
	return ERR_NONE;
}

/**
 * \brief Set ADC resolution
 */
int32_t adc_os_set_resolution(struct adc_os_descriptor *const descr, const adc_resolution_t resolution)
{
	ASSERT(descr);

	_adc_async_set_resolution(&descr->device, resolution);
	return ERR_NONE;
}

/**
 * \brief Set ADC input source for a channel
 */
int32_t adc_os_set_inputs(struct adc_os_descriptor *const descr, const adc_pos_input_t pos_input,
                          const adc_neg_input_t neg_input, const uint8_t channel)
{
	ASSERT(descr);
	ASSERT(descr->channel_max >= channel);

	_adc_async_set_inputs(&descr->device, pos_input, neg_input, channel);
	return ERR_NONE;
}

/**
 * \brief Set ADC thresholds
 */
int32_t adc_os_set_thresholds(struct adc_os_descriptor *const descr, const adc_threshold_t low_threshold,
                              const adc_threshold_t up_threshold)
{
	ASSERT(descr);

	_adc_async_set_thresholds(&descr->device, low_threshold, up_threshold);
	return ERR_NONE;
}

/**
 * \brief Set ADC gain
 */
int32_t adc_os_set_channel_gain(struct adc_os_descriptor *const descr, const uint8_t channel, const adc_gain_t gain)
{
	ASSERT(descr);
	ASSERT(descr->channel_max >= channel);

	_adc_async_set_channel_gain(&descr->device, channel, gain);
	return ERR_NONE;
}

/**
 * \brief Set ADC conversion mode
 */
int32_t adc_os_set_conversion_mode(struct adc_os_descriptor *const descr, const enum adc_conversion_mode mode)
{
	ASSERT(descr);

	_adc_async_set_conversion_mode(&descr->device, mode);
	return ERR_NONE;
}

/**
 * \brief Set ADC differential mode
 */
int32_t adc_os_set_channel_differential_mode(struct adc_os_descriptor *const descr, const uint8_t channel,
                                             const enum adc_differential_mode mode)
{
	ASSERT(descr);
	ASSERT(descr->channel_max >= channel);

	_adc_async_set_channel_differential_mode(&descr->device, channel, mode);
	return ERR_NONE;
}

/**
 * \brief Set ADC window mode
 */
int32_t adc_os_set_window_mode(struct adc_os_descriptor *const descr, const adc_window_mode_t mode)
{
	ASSERT(descr);

	_adc_async_set_window_mode(&descr->device, mode);
	return ERR_NONE;
}

/**
 * \brief Retrieve threshold state
 */
int32_t adc_os_get_threshold_state(const struct adc_os_descriptor *const descr, adc_threshold_status_t *const state)
{
	ASSERT(descr && state);

	_adc_async_get_threshold_state(&descr->device, state);
	return ERR_NONE;
}

/**
 * \brief flush adc ringbuf
 */
int32_t adc_os_flush_rx_buffer(struct adc_os_descriptor *const descr, const uint8_t channel)
{
	ASSERT(descr);
	ASSERT(descr->channel_max >= channel);

	uint8_t index = descr->channel_map[channel];
	ASSERT(descr->channel_max >= index);

	struct adc_os_channel_descriptor *descr_ch = &descr->descr_ch[index];

	return ringbuffer_flush(&descr_ch->convert);
}

/**
 * \brief Retrieve the current driver version
 */
uint32_t adc_os_get_version(void)
{
	return HAL_ADC_OS_DRIVER_VERSION;
}

/**
 * \internal Process conversion completion
 */
static void adc_os_channel_conversion_done(struct _adc_async_device *device, const uint8_t channel, const uint16_t data)
{
	struct adc_os_descriptor *const descr = CONTAINER_OF(device, struct adc_os_descriptor, device);

	uint8_t                           index    = descr->channel_map[channel];
	struct adc_os_channel_descriptor *descr_ch = &descr->descr_ch[index];

	if (descr_ch->rxbuf == NULL) {
		ringbuffer_put(&descr_ch->convert, data);
		if (_adc_async_get_data_size(&descr->device) == 2) {
			ringbuffer_put(&descr_ch->convert, data >> 8);
		}
	} else {
		descr_ch->rxbuf[descr_ch->size++] = data;
		if (_adc_async_get_data_size(&descr->device) == 2) {
			descr_ch->rxbuf[descr_ch->size++] = data >> 8;
		}

		if (descr_ch->size >= descr_ch->num) {
			descr_ch->rxbuf = NULL;
			sem_up(&descr_ch->rx_sem);
		}
	}
}

/**
 * \internal Process conversion window threshold reached
 */
static void adc_os_window_threshold_reached(struct _adc_async_device *device, const uint8_t channel)
{
	struct adc_os_descriptor *const descr = CONTAINER_OF(device, struct adc_os_descriptor, device);

	sem_up(&descr->monitor_sem);
}

/**
 * \internal Process conversion error
 */
static void adc_os_error_occured(struct _adc_async_device *device, const uint8_t channel)
{
	(void)device;
}
