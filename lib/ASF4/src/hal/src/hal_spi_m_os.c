/**
 * \file
 *
 * \brief I/O SPI related functionality implementation.
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

#include "hal_atomic.h"
#include "hal_spi_m_os.h"
#include <utils_assert.h>

#include <utils.h>

/**
 * \brief RTOS Driver version
 */
#define SPI_M_OS_DRIVER_VERSION 0x00000001u

/** \brief Do SPI read in background (RTOS version)
 *  For SPI master, register the buffer, do activate CS and send 0xFFs to get
 *  data, then deactivate CS in background.
 *
 *  It blocks task/thread until transfer done. user check the retval to
 *  know transfer status and when data is ready to process.
 *
 *  \param[in] io Pointer to the HAL SPI instance.
 *  \param[out] buf Pointer to the buffer to store read data.
 *  \param[in] length size Size of the data in number of characters.
 *
 *  \return length on success, or an error code on failure.
 */
static int32_t spi_m_os_io_read(struct io_descriptor *io, uint8_t *const buf, const uint16_t length)
{
	ASSERT(io && buf);
	struct spi_m_os_descriptor *spi = CONTAINER_OF(io, struct spi_m_os_descriptor, io);

	spi->xfer.rxbuf = buf;
	spi->xfer.txbuf = NULL;
	spi->xfer.size  = length;
	spi->xfercnt    = 0;
	spi->error      = ERR_NONE;

	_spi_m_async_enable_rx(&spi->dev, true);
	_spi_m_async_write_one(&spi->dev, SPI_DUMMY_CHAR);

	if (sem_down(&spi->xfer_sem, ~0) < 0) {
		return ERR_TIMEOUT;
	}

	if (spi->error) {
		return ERR_IO;
	}

	return length;
}

/** \brief Do SPI data write in background (RTOS)
 *  For SPI master, register buffer, do activate CS, buffer send and
 *  deactivate CS in background.
 *
 *  The data read back is discarded.
 *
 *  It blocks task/thread until transfer done. user check the retval to
 *  know transfer status and when data is ready to process.
 *
 *  \param[in] io Pointer to the HAL SPI instance.
 *  \param[in] buf Pointer to the buffer to store data to write.
 *  \param[in] length size Size of the data in number of characters.
 *
 *  \return length on success, or an error code on failure.
 */
static int32_t spi_m_os_io_write(struct io_descriptor *io, const uint8_t *const buf, const uint16_t length)
{
	ASSERT(io && buf);
	struct spi_m_os_descriptor *spi = CONTAINER_OF(io, struct spi_m_os_descriptor, io);

	spi->xfer.rxbuf = NULL;
	spi->xfer.txbuf = (uint8_t *)buf;
	spi->xfer.size  = length;
	spi->xfercnt    = 0;
	spi->error      = ERR_NONE;

	_spi_m_async_enable_tx(&spi->dev, true);

	if (sem_down(&spi->xfer_sem, ~0) < 0) {
		return ERR_TIMEOUT;
	}

	if (spi->error) {
		return ERR_IO;
	}

	return length;
}

/**
 *  \brief Callback for TX
 *  \param[in] dev Pointer to the SPI device instance.
 */
static void spi_m_os_tx(struct _spi_m_async_dev *dev)
{
	struct spi_m_os_descriptor *spi = CONTAINER_OF(dev, struct spi_m_os_descriptor, dev);

	if (!(dev->char_size > 1)) {
		_spi_m_async_write_one(dev, spi->xfer.txbuf[spi->xfercnt++]);
	} else {
		_spi_m_async_write_one(dev, ((uint16_t *)spi->xfer.txbuf)[spi->xfercnt++]);
	}

	if (spi->xfercnt >= spi->xfer.size) {
		_spi_m_async_enable_tx(dev, false);
		sem_up(&spi->xfer_sem);
	}
}

/**
 *  \brief Callback for RX
 *  \param[in] dev Pointer to the SPI device instance.
 */
static void spi_m_os_rx(struct _spi_m_async_dev *dev)
{
	struct spi_m_os_descriptor *spi = CONTAINER_OF(dev, struct spi_m_os_descriptor, dev);

	if (!(dev->char_size > 1)) {
		/* 8-bit or less */
		spi->xfer.rxbuf[spi->xfercnt++] = (uint8_t)_spi_m_async_read_one(dev);
	} else {
		/* 9-bit or more */
		((uint16_t *)spi->xfer.rxbuf)[spi->xfercnt++] = (uint16_t)_spi_m_async_read_one(dev);
	}

	if (spi->xfercnt < spi->xfer.size) {
		if (spi->xfer.txbuf) {
			if (dev->char_size == SPI_CHAR_SIZE_8) {
				_spi_m_async_write_one(dev, spi->xfer.txbuf[spi->xfercnt]);
			} else {
				_spi_m_async_write_one(dev, ((uint16_t *)spi->xfer.txbuf)[spi->xfercnt]);
			}
		} else {
			_spi_m_async_write_one(dev, dev->dummy_byte);
		}
	} else {
		_spi_m_async_enable_rx(dev, false);
		sem_up(&spi->xfer_sem);
	}
}

/**
 *  \brief Callback for error
 *  \param[in] dev Pointer to the SPI device instance.
 *  \param[in] status Error status.
 */
static void spi_m_os_error(struct _spi_m_async_dev *dev, int32_t status)
{
	struct spi_m_os_descriptor *spi = CONTAINER_OF(dev, struct spi_m_os_descriptor, dev);

	if (status == 0) {
		return;
	}
	_spi_m_async_enable_tx(dev, false);
	_spi_m_async_enable_rx(dev, false);

	spi->error = status;
	sem_up(&spi->xfer_sem);
}

/**
 *  \brief Initialize the SPI HAL instance function pointer for HPL APIs.
 */
void spi_m_os_set_func_ptr(struct spi_m_os_descriptor *spi, void *const func)
{
	ASSERT(spi);
	spi->func = (struct _spi_m_async_hpl_interface *)func;
}

/**
 *  \brief Initialize the SPI HAL instance and hardware for rtos mode.
 */
int32_t spi_m_os_init(struct spi_m_os_descriptor *const spi, void *const hw)
{
	int32_t rc = 0;
	ASSERT(spi && hw);
	spi->dev.prvt = (void *)hw;
	rc            = _spi_m_async_init(&spi->dev, hw);

	if (rc == ERR_NONE) {
		_spi_m_async_register_callback(&spi->dev, SPI_DEV_CB_TX, (FUNC_PTR)spi_m_os_tx);
		_spi_m_async_register_callback(&spi->dev, SPI_DEV_CB_RX, (FUNC_PTR)spi_m_os_rx);
		_spi_m_async_register_callback(&spi->dev, SPI_DEV_CB_COMPLETE, (FUNC_PTR)spi_m_os_error);
	} else {
		return rc;
	}

	spi->io.read  = spi_m_os_io_read;
	spi->io.write = spi_m_os_io_write;

	sem_init(&spi->xfer_sem, 0);

	return ERR_NONE;
}

/**
 *  \brief Deinitialize the SPI HAL instance.
 */
int32_t spi_m_os_deinit(struct spi_m_os_descriptor *const spi)
{
	ASSERT(spi);

	sem_deinit(&spi->xfer_sem);

	spi->io.read  = NULL;
	spi->io.write = NULL;
	_spi_m_async_register_callback(&spi->dev, SPI_DEV_CB_TX, NULL);
	_spi_m_async_register_callback(&spi->dev, SPI_DEV_CB_RX, NULL);
	_spi_m_async_register_callback(&spi->dev, SPI_DEV_CB_COMPLETE, NULL);
	return _spi_m_async_deinit(&spi->dev);
}

/**
 *  \brief Enable SPI.
 */
int32_t spi_m_os_enable(struct spi_m_os_descriptor *const spi)
{
	ASSERT(spi);

	if (_spi_m_async_enable(&spi->dev) < 0) {
		return ERR_DENIED;
	}

	return ERR_NONE;
}

/**
 *  \brief Disable SPI and abort pending transfer in progress.
 */
int32_t spi_m_os_disable(struct spi_m_os_descriptor *const spi)
{
	ASSERT(spi);
	_spi_m_async_enable_tx(&spi->dev, false);
	_spi_m_async_enable_rx(&spi->dev, false);

	if (_spi_m_async_disable(&spi->dev) < 0) {
		return ERR_DENIED;
	}

	return ERR_NONE;
}

/**
 *  \brief Set SPI baudrate.
 */
int32_t spi_m_os_set_baudrate(struct spi_m_os_descriptor *const spi, const uint32_t baud_val)
{
	ASSERT(spi);
	return _spi_m_async_set_baudrate(&spi->dev, baud_val);
}

/**
 *  \brief Set SPI mode.
 */
int32_t spi_m_os_set_mode(struct spi_m_os_descriptor *const spi, const enum spi_transfer_mode mode)
{
	ASSERT(spi);
	return _spi_m_async_set_mode(&spi->dev, mode);
}

/**
 *  \brief Set SPI transfer character size in number of bits.
 */
int32_t spi_m_os_set_char_size(struct spi_m_os_descriptor *const spi, const enum spi_char_size char_size)
{
	ASSERT(spi);
	return _spi_m_async_set_char_size(&spi->dev, char_size);
}

/**
 *  \brief Set SPI transfer data order.
 */
int32_t spi_m_os_set_data_order(struct spi_m_os_descriptor *const spi, const enum spi_data_order order)
{
	ASSERT(spi);
	return _spi_m_async_set_data_order(&spi->dev, order);
}

/**
 *  \brief Do SPI data transfer (TX & RX) using rtos.
 */
int32_t spi_m_os_transfer(struct spi_m_os_descriptor *const spi, uint8_t const *txbuf, uint8_t *const rxbuf,
                          const uint16_t length)
{
	ASSERT(spi && txbuf && rxbuf);

	/* Fill transfer descriptor */
	spi->xfer.rxbuf = (uint8_t *)rxbuf;
	spi->xfer.txbuf = (uint8_t *)txbuf;
	spi->xfer.size  = length;
	spi->xfercnt    = 0;
	_spi_m_async_enable_rx(&spi->dev, true);
	_spi_m_async_write_one(&spi->dev, txbuf[spi->xfercnt]);

	if (0 != sem_down(&spi->xfer_sem, ~0)) {
		return ERR_TIMEOUT;
	}

	return ERR_NONE;
}

/**
 *  \brief Retrieve the current driver version.
 */
uint32_t spi_m_os_get_version(void)
{
	return SPI_M_OS_DRIVER_VERSION;
}
