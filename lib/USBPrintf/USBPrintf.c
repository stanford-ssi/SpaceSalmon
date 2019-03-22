#include "USBPrintf.h"

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

char string1[100];
uint8_t fill1 = 0;

char string2[100];
uint8_t fill2 = 0;

bool state = false; //false = write into string1
					//true  = write into string2

volatile bool tx_done = true;

volatile bool dtr_state = false;

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
	tx_done = true;
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
		//cdcdf_acm_register_callback(CDCDF_ACM_CB_WRITE, (FUNC_PTR)usb_device_cb_bulk_in); //this is not needed really
	}
	dtr_state = state.rs232.DTR;

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

int _write(int file, char *ptr, int len)
{
	__disable_irq();

	if(state){
		string2[fill2] = *ptr;
		fill2++;
	}else{
		string1[fill1] = *ptr;
		fill1++;
	}
	

	//state -> writing into 2 -> sending 1 -> should start sending 2
	

	if(state){
		if(tx_done && fill2 > 0){
			cdcdf_acm_register_callback(CDCDF_ACM_CB_WRITE, (FUNC_PTR)usb_device_cb_bulk_in); //is this hacky? or ok?
			tx_done = false;
			cdcdf_acm_write((uint8_t *)string2, fill2);
			fill1 = 0;
			state = false;
		}
	}else{
		if(tx_done && fill1 > 0){
			cdcdf_acm_register_callback(CDCDF_ACM_CB_WRITE, (FUNC_PTR)usb_device_cb_bulk_in); //is this hacky? or ok?
			tx_done = false;
			cdcdf_acm_write((uint8_t *)string1, fill1);
			fill2 = 0;
			state = true;
		}
	}
		
	

	__enable_irq();

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