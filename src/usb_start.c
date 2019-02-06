/*
 * Code generated from Atmel Start.
 *
 * This file will be overwritten when reconfiguring your Atmel Start project.
 * Please copy examples or other code you want to keep to a separate file or main.c
 * to avoid loosing it when reconfiguring.
 */
#include "atmel_start.h"
#include "usb_start.h"

#if CONF_USBD_HS_SP
static uint8_t single_desc_bytes[] = {
    /* Device descriptors and Configuration descriptors list. */
    CDCD_ACM_HS_DESCES_LS_FS};
static uint8_t single_desc_bytes_hs[] = {
    /* Device descriptors and Configuration descriptors list. */
    CDCD_ACM_HS_DESCES_HS};
#define CDCD_ECHO_BUF_SIZ CONF_USB_CDCD_ACM_DATA_BULKIN_MAXPKSZ_HS
#else
static uint8_t single_desc_bytes[] = {
    /* Device descriptors and Configuration descriptors list. */
    CDCD_ACM_DESCES_LS_FS};
#define CDCD_ECHO_BUF_SIZ CONF_USB_CDCD_ACM_DATA_BULKIN_MAXPKSZ
#endif

static struct usbd_descriptors single_desc[]
    = {{single_desc_bytes, single_desc_bytes + sizeof(single_desc_bytes)}
#if CONF_USBD_HS_SP
       ,
       {single_desc_bytes_hs, single_desc_bytes_hs + sizeof(single_desc_bytes_hs)}
#endif
};

/** Buffers to receive and echo the communication bytes. */
static uint8_t usbd_cdc_buffer[2][CDCD_ECHO_BUF_SIZ];


/** Ctrl endpoint buffer */
static uint8_t ctrl_buffer[64];

volatile bool buffer = false;
volatile bool writing[2] = {false,false};

uint8_t full[2] = {0,0};

/**
 * \brief Callback invoked when bulk OUT data received
 */
//static bool usb_device_cb_bulk_out(const uint8_t ep, const enum usb_xfer_code rc, const uint32_t count)
//{
	//cdcdf_acm_write((uint8_t *)usbd_cdc_buffer, count+5);

	/* No error. */
//	return false;
//}

/**
 * \brief Callback invoked when bulk IN data received
 */
static bool usb_device_cb_bulk_in(const uint8_t ep, const enum usb_xfer_code rc, const uint32_t count)
{
	writing[!buffer] = false; //done writing!
	/* No error. */
	return false;
}

/**
 * \brief Callback invoked when Line State Change
 */
static bool usb_device_cb_state_c(usb_cdc_control_signal_t state)
{
	if (state.rs232.DTR) {
		/* Callbacks must be registered after endpoint allocation */
		//cdcdf_acm_register_callback(CDCDF_ACM_CB_READ, (FUNC_PTR)usb_device_cb_bulk_out);
		cdcdf_acm_register_callback(CDCDF_ACM_CB_WRITE, (FUNC_PTR)usb_device_cb_bulk_in);//this isint needed
		/* Start Rx */
		//cdcdf_acm_read((uint8_t *)usbd_cdc_buffer, sizeof(usbd_cdc_buffer));
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

/**
 * Example of using CDC ACM Function.
 * \note
 * In this example, we will use a PC as a USB host:
 * - Connect the DEBUG USB on XPLAINED board to PC for program download.
 * - Connect the TARGET USB on XPLAINED board to PC for running program.
 * The application will behave as a virtual COM.
 * - Open a HyperTerminal or other COM tools in PC side.
 * - Send out a character or string and it will echo the content received.
 */
void cdcd_acm_example(void)
{
	cdcdf_acm_register_callback(CDCDF_ACM_CB_STATE_C, (FUNC_PTR)usb_device_cb_state_c);
}

void usb_init(void)
{

	cdc_device_acm_init();
}

void puty(const char* msg, int len)
{
	if(len < 63 - full[buffer]){
		memcpy((uint8_t *)usbd_cdc_buffer[buffer] + full[buffer], msg, len);
		full[buffer] += len;
	}

	if(!writing[!buffer]){
		buffer = !buffer;
		cdcdf_acm_register_callback(CDCDF_ACM_CB_WRITE, (FUNC_PTR)usb_device_cb_bulk_in);//is this hacky? or ok?
		cdcdf_acm_write((uint8_t *)usbd_cdc_buffer[!buffer],full[!buffer]);//start writing
		writing[!buffer] = true;
		full[buffer] = 0;
	}

}

int __attribute__((weak)) _write(int file, char *ptr, int len)
{
	int n = 0;

	/*if ((file != 1) && (file != 2) && (file != 3)) {
		return -1;
	}*/

	puty(ptr,len);
		
	return len;
}

int __attribute__((weak)) _read(int file, char *ptr, int len)
{
	int n = 0;

	if (file != 0) {
		return -1;
	}

	if (n < 0) {
		return -1;
	}

	return n;
}