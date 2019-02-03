/*
 * Code generated from Atmel Start.
 *
 * This file will be overwritten when reconfiguring your Atmel Start project.
 * Please copy examples or other code you want to keep to a separate file
 * to avoid losing it when reconfiguring.
 */

#include "driver_init.h"
#include <peripheral_clk_config.h>
#include <utils.h>
#include <hal_init.h>

struct mci_sync_desc IO_BUS;

void SD_init(void)
{

	hri_mclk_set_AHBMASK_SDHC0_bit(MCLK);
	hri_gclk_write_PCHCTRL_reg(GCLK, SDHC0_GCLK_ID, CONF_GCLK_SDHC0_SRC | (1 << GCLK_PCHCTRL_CHEN_Pos));
	hri_gclk_write_PCHCTRL_reg(GCLK, SDHC0_GCLK_ID_SLOW, CONF_GCLK_SDHC0_SLOW_SRC | (1 << GCLK_PCHCTRL_CHEN_Pos));

	mci_sync_init(&IO_BUS, SDHC0);

	gpio_set_pin_direction(SD_SCK, GPIO_DIRECTION_OUT);
	gpio_set_pin_level(SD_SCK, false);
	gpio_set_pin_pull_mode(SD_SCK, GPIO_PULL_OFF);
	gpio_set_pin_function(SD_SCK, PINMUX_PB11I_SDHC0_SDCK);

	gpio_set_pin_direction(SD_CMD, GPIO_DIRECTION_OUT);
	gpio_set_pin_level(SD_CMD, false);
	gpio_set_pin_pull_mode(SD_CMD, GPIO_PULL_OFF);
	gpio_set_pin_function(SD_CMD, PINMUX_PA08I_SDHC0_SDCMD);

	gpio_set_pin_direction(SD_DAT0, GPIO_DIRECTION_OUT);
	gpio_set_pin_level(SD_DAT0, false);
	gpio_set_pin_pull_mode(SD_DAT0, GPIO_PULL_OFF);
	gpio_set_pin_function(SD_DAT0, PINMUX_PA09I_SDHC0_SDDAT0);

	gpio_set_pin_direction(SD_DAT1, GPIO_DIRECTION_OUT);
	gpio_set_pin_level(SD_DAT1, false);
	gpio_set_pin_pull_mode(SD_DAT1,  GPIO_PULL_OFF);
	gpio_set_pin_function(SD_DAT1, PINMUX_PA10I_SDHC0_SDDAT1);

	gpio_set_pin_direction(SD_DAT2, GPIO_DIRECTION_OUT);
	gpio_set_pin_level(SD_DAT2, false);
	gpio_set_pin_pull_mode(SD_DAT2, GPIO_PULL_OFF);
	gpio_set_pin_function(SD_DAT2, PINMUX_PA11I_SDHC0_SDDAT2);

	gpio_set_pin_direction(SD_DAT3, GPIO_DIRECTION_OUT);
	gpio_set_pin_level(SD_DAT3, false);
	gpio_set_pin_pull_mode(SD_DAT3, GPIO_PULL_OFF);
	gpio_set_pin_function(SD_DAT3, PINMUX_PB10I_SDHC0_SDDAT3);
}


/* The USB module requires a GCLK_USB of 48 MHz ~ 0.25% clock
 * for low speed and full speed operation. */
#if (CONF_GCLK_USB_FREQUENCY > (48000000 + 48000000 / 400)) || (CONF_GCLK_USB_FREQUENCY < (48000000 - 48000000 / 400))
#warning USB clock should be 48MHz ~ 0.25% clock, check your configuration!
#endif

void USB_init(void)
{
	hri_gclk_write_PCHCTRL_reg(GCLK, USB_GCLK_ID, CONF_GCLK_USB_SRC | GCLK_PCHCTRL_CHEN);
	hri_mclk_set_AHBMASK_USB_bit(MCLK);
	hri_mclk_set_APBBMASK_USB_bit(MCLK);

	usb_d_init();

	gpio_set_pin_direction(USB_DM, GPIO_DIRECTION_OUT);
	gpio_set_pin_level(USB_DM, false);
	gpio_set_pin_pull_mode(USB_DM, GPIO_PULL_OFF);
	gpio_set_pin_function(USB_DM, PINMUX_PA24H_USB_DM);

	gpio_set_pin_direction(USB_DP,GPIO_DIRECTION_OUT);
	gpio_set_pin_level(USB_DP, false);
	gpio_set_pin_pull_mode(USB_DP, GPIO_PULL_OFF);
	gpio_set_pin_function(USB_DP, PINMUX_PA25H_USB_DP);
}

void led_init(void){

	gpio_set_pin_level(LED1, false);
	gpio_set_pin_direction(LED1,GPIO_DIRECTION_OUT);
	gpio_set_pin_function(LED1,GPIO_PIN_FUNCTION_OFF);

	gpio_set_pin_level(LED2, false);
	gpio_set_pin_direction(LED2,GPIO_DIRECTION_OUT);
	gpio_set_pin_function(LED2,GPIO_PIN_FUNCTION_OFF);

	gpio_set_pin_level(LED3,false);
	gpio_set_pin_direction(LED3, GPIO_DIRECTION_OUT);
	gpio_set_pin_function(LED3, GPIO_PIN_FUNCTION_OFF);

	gpio_set_pin_level(LED4,false);
	gpio_set_pin_direction(LED4, GPIO_DIRECTION_OUT);
	gpio_set_pin_function(LED4, GPIO_PIN_FUNCTION_OFF);

	gpio_set_pin_level(BUZZER, false);
	gpio_set_pin_direction(BUZZER,GPIO_DIRECTION_OUT);
	gpio_set_pin_function(BUZZER,GPIO_PIN_FUNCTION_OFF);
	
}



void system_init(void)
{
	init_mcu();

	led_init();
	SD_init();
	USB_init();
}
