/**
 * \file
 *
 * \brief SPI RTOS related functionality declaration.
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

#ifndef _HAL_SPI_M_OS_H_INCLUDED
#define _HAL_SPI_M_OS_H_INCLUDED

#include <hal_io.h>
#include <hpl_spi_m_async.h>
#include <hal_rtos.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \addtogroup doc_driver_hal_spi_master_os
 *
 * @{
 */

/* Forward declaration of spi_descriptor. */
struct spi_m_os_descriptor;

/** \brief SPI HAL driver struct for asynchronous access
 */
struct spi_m_os_descriptor {
	struct _spi_m_async_hpl_interface *func;
	struct _spi_m_async_dev            dev;      /** Pointer to SPI device instance */
	struct io_descriptor               io;       /** I/O read/write */
	sem_t                              xfer_sem; /** SPI semphore */
	struct spi_xfer                    xfer;     /** Transfer information copy, for R/W/Transfer */
	uint32_t                           xfercnt;  /** Character count in current transfer */
	uint32_t                           error;    /** SPI complete status, 0:no error*/
};

/** \brief Set the SPI HAL instance function pointer for HPL APIs.
 *
 *  Set SPI HAL instance function pointer for HPL APIs.
 *
 *  \param[in] spi Pointer to the HAL SPI instance.
 *  \param[in] func Pointer to the HPL api structure.
 *
 */
void spi_m_os_set_func_ptr(struct spi_m_os_descriptor *spi, void *const func);

/** \brief Initialize the SPI HAL instance and hardware for RTOS mode
 *
 *  Initialize SPI HAL with interrupt mode (uses RTOS).
 *
 *  \param[in] spi Pointer to the HAL SPI instance.
 *  \param[in] hw Pointer to the hardware base.
 *
 *  \return Operation status.
 *  \retval ERR_NONE Success.
 *  \retval ERR_INVALID_DATA Error, initialized.
 */
int32_t spi_m_os_init(struct spi_m_os_descriptor *const spi, void *const hw);

/** \brief Deinitialize the SPI HAL instance
 *
 *  Abort transfer, disable and reset SPI, de-init software.
 *
 *  \param[in] spi Pointer to the HAL SPI instance.
 *
 *  \return Operation status.
 *  \retval ERR_NONE Success.
 *  \retval <0 Error code.
 */
int32_t spi_m_os_deinit(struct spi_m_os_descriptor *const spi);

/** \brief Enable SPI
 *
 *  \param[in] spi Pointer to the HAL SPI instance.
 *
 *  \return Operation status.
 *  \retval ERR_NONE Success.
 *  \retval <0 Error code.
 */
int32_t spi_m_os_enable(struct spi_m_os_descriptor *const spi);

/** \brief Disable the SPI and abort any pending transfer in progress
 *
 * If there is any pending transfer, the complete callback is invoked
 * with the \c ERR_ABORTED status.
 *
 *  \param[in] spi Pointer to the HAL SPI instance.
 *
 *  \return Operation status.
 *  \retval ERR_NONE Success.
 *  \retval <0 Error code.
 */
int32_t spi_m_os_disable(struct spi_m_os_descriptor *const spi);

/** \brief Set SPI baudrate
 *
 *  Works if the SPI is initialized as master.
 *  In the function a sanity check is used to confirm it's called in the correct mode.
 *
 *  \param[in] spi Pointer to the HAL SPI instance.
 *  \param[in] baud_val The target baudrate value
 *                  (see "baudrate calculation" for calculating the value).
 *
 *  \return Operation status.
 *  \retval ERR_NONE Success.
 *  \retval ERR_BUSY Busy.
 */
int32_t spi_m_os_set_baudrate(struct spi_m_os_descriptor *const spi, const uint32_t baud_val);

/** \brief Set SPI mode
 *
 *  Set the SPI transfer mode (\ref spi_transfer_mode),
 *  which controls the clock polarity and clock phase:
 *  - Mode 0: leading edge is rising edge, data sample on leading edge.
 *  - Mode 1: leading edge is rising edge, data sample on trailing edge.
 *  - Mode 2: leading edge is falling edge, data sample on leading edge.
 *  - Mode 3: leading edge is falling edge, data sample on trailing edge.
 *
 *  \param[in] spi Pointer to the HAL SPI instance.
 *  \param[in] mode The mode (\ref spi_transfer_mode).
 *
 *  \return Operation status.
 *  \retval ERR_NONE Success.
 *  \retval ERR_BUSY Busy, CS activated.
 */
int32_t spi_m_os_set_mode(struct spi_m_os_descriptor *const spi, const enum spi_transfer_mode mode);

/** \brief Set SPI transfer character size in number of bits
 *
 *  The character size (\ref spi_char_size) influence the way the data is
 *  sent/received.
 *  For char size <= 8-bit, data is stored byte by byte.
 *  For char size between 9-bit ~ 16-bit, data is stored in 2-byte length.
 *  Note that the default and recommended char size is 8-bit since it's
 *  supported by all system.
 *
 *  \param[in] spi Pointer to the HAL SPI instance.
 *  \param[in] char_size The char size (\ref spi_char_size).
 *
 *  \return Operation status.
 *  \retval ERR_NONE Success.
 *  \retval ERR_BUSY Busy, CS activated.
 *  \retval ERR_INVALID_ARG The char size is not supported.
 */
int32_t spi_m_os_set_char_size(struct spi_m_os_descriptor *const spi, const enum spi_char_size char_size);

/** \brief Set SPI transfer data order
 *
 *  \param[in] spi Pointer to the HAL SPI instance.
 *  \param[in] order The data order: send LSB/MSB first.
 *
 *  \return Operation status.
 *  \retval ERR_NONE Success.
 *  \retval ERR_BUSY Busy, CS activated.
 *  \retval ERR_INVALID The data order is not supported.
 */
int32_t spi_m_os_set_data_order(struct spi_m_os_descriptor *const spi, const enum spi_data_order order);

/** \brief Perform the SPI data transfer (TX and RX) using RTOS
 *
 *  Log the TX and RX buffers and transfer them in background. It blocks
 *  task/thread until the transfer done.
 *
 *  \param[in] spi Pointer to the HAL SPI instance.
 *  \param[in] txbuf Pointer to the transfer information (\ref spi_transfer).
 *  \param[out] rxbuf Pointer to the receiver information (\ref spi_receive).
 *  \param[in] length SPI transfer data length.
 *
 *  \return Operation status.
 *  \retval ERR_NONE Success.
 *  \retval ERR_BUSY Busy.
 */
int32_t spi_m_os_transfer(struct spi_m_os_descriptor *const spi, uint8_t const *txbuf, uint8_t *const rxbuf,
                          const uint16_t length);

/**
 * \brief Return the I/O descriptor for this SPI instance
 *
 * This function will return an I/O instance for this SPI driver instance.
 *
 * \param[in] spi An SPI master descriptor, which is used to communicate through
 *                SPI
 * \param[in, out] io A pointer to an I/O descriptor pointer type
 *
 * \retval ERR_NONE
 */
static inline int32_t spi_m_os_get_io_descriptor(struct spi_m_os_descriptor *const spi, struct io_descriptor **io)
{
	*io = &spi->io;
	return ERR_NONE;
}

/** \brief Retrieve the current driver version
 *
 *  \return Current driver version.
 */
uint32_t spi_m_os_get_version(void);

/**@}*/

#ifdef __cplusplus
}
#endif
/**@}*/
#endif /* ifndef _HAL_SPI_M_OS_H_INCLUDED */
