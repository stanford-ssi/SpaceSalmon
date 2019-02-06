#include "atmel_start.h"
#include "usb_start.h"

#if CONF_USBD_HS_SP //faster, but we don't need it for now
	static uint8_t single_desc_bytes[] = {CDCD_ACM_HS_DESCES_LS_FS};
	static uint8_t single_desc_bytes_hs[] = {CDCD_ACM_HS_DESCES_HS};
	#define CDCD_ECHO_BUF_SIZ CONF_USB_CDCD_ACM_DATA_BULKIN_MAXPKSZ_HS
	static struct usbd_descriptors single_desc[] = {{single_desc_bytes, single_desc_bytes + sizeof(single_desc_bytes)},
												{single_desc_bytes_hs, single_desc_bytes_hs + sizeof(single_desc_bytes_hs)}};
#else
	static uint8_t single_desc_bytes[] = {CDCD_ACM_DESCES_LS_FS};
	#define CDCD_ECHO_BUF_SIZ CONF_USB_CDCD_ACM_DATA_BULKIN_MAXPKSZ
	static struct usbd_descriptors single_desc[] = {{single_desc_bytes, single_desc_bytes + sizeof(single_desc_bytes)}};
#endif
											

/** Ctrl endpoint tx_flag */ //used internally by the CDC library
static uint8_t ctrl_buffer[64];

/** Buffers to receive and echo the communication bytes. */
static uint8_t usb_tx_buffer[2][CDCD_ECHO_BUF_SIZ];
volatile bool tx_flag = false;
volatile bool tx_status[2] = {false, false};
uint8_t tx_fill[2] = {0, 0};

/**
 * 
 * \brief Callback invoked when RX data has been saved to a buffer
 */
//static bool usb_device_cb_bulk_out(const uint8_t ep, const enum usb_xfer_code rc, const uint32_t count)
//{
//cdcdf_acm_write((uint8_t *)usb_tx_buffer, count+5);

/* No error. */
//	return false;
//}

/**
 * \brief Callback invoked when TX data has been fully writen over USB
 */
static bool usb_device_cb_bulk_in(const uint8_t ep, const enum usb_xfer_code rc, const uint32_t count)
{
	tx_status[!tx_flag] = false;
	return false;
}

/**
 * \brief Callback invoked when Line State Changes (Serial Port Opens)
 */
static bool usb_device_cb_state_c(usb_cdc_control_signal_t state)
{
	if (state.rs232.DTR)
	{
		/* Callbacks must be registered after endpoint allocation */
		//cdcdf_acm_register_callback(CDCDF_ACM_CB_READ, (FUNC_PTR)usb_device_cb_bulk_out);
		cdcdf_acm_register_callback(CDCDF_ACM_CB_WRITE, (FUNC_PTR)usb_device_cb_bulk_in); //this is not needed really
	}

	/* No error. */
	return false;
}

/**
 * \brief CDC ACM Init
 */
void cdc_device_acm_init(void)
{
	/* usb stack init */
	usbdc_init(ctrl_buffer);

	/* usbdc_register_funcion inside */
	cdcdf_acm_init();

	usbdc_start(single_desc);
	usbdc_attach();
}

void usb_init(void)
{
	cdc_device_acm_init();
	cdcdf_acm_register_callback(CDCDF_ACM_CB_STATE_C, (FUNC_PTR)usb_device_cb_state_c);
}

int __attribute__((weak)) _write(int file, char *ptr, int len)
{
	if (len < 63 - tx_fill[tx_flag])
	{
		memcpy((uint8_t *)usb_tx_buffer[tx_flag] + tx_fill[tx_flag], ptr, len);
		tx_fill[tx_flag] += len;
	}

	if (!tx_status[!tx_flag])
	{
		tx_flag = !tx_flag;
		cdcdf_acm_register_callback(CDCDF_ACM_CB_WRITE, (FUNC_PTR)usb_device_cb_bulk_in); //is this hacky? or ok?
		cdcdf_acm_write((uint8_t *)usb_tx_buffer[!tx_flag], tx_fill[!tx_flag]);				  //start tx_status
		tx_status[!tx_flag] = true;
		tx_fill[tx_flag] = 0;
	}

	return len;
}

int __attribute__((weak)) _read(int file, char *ptr, int len) //TODO scanf!
{
	int n = 0;

	if (file != 0)
	{
		return -1;
	}

	if (n < 0)
	{
		return -1;
	}

	return n;
}