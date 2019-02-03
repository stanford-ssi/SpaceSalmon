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

void IO_BUS_PORT_init(void)
{

	gpio_set_pin_direction(PB11,
						   // <y> Pin direction
						   // <id> pad_direction
						   // <GPIO_DIRECTION_OFF"> Off
						   // <GPIO_DIRECTION_IN"> In
						   // <GPIO_DIRECTION_OUT"> Out
						   GPIO_DIRECTION_OUT);

	gpio_set_pin_level(PB11,
					   // <y> Initial level
					   // <id> pad_initial_level
					   // <false"> Low
					   // <true"> High
					   false);

	gpio_set_pin_pull_mode(PB11,
						   // <y> Pull configuration
						   // <id> pad_pull_config
						   // <GPIO_PULL_OFF"> Off
						   // <GPIO_PULL_UP"> Pull-up
						   // <GPIO_PULL_DOWN"> Pull-down
						   GPIO_PULL_OFF);

	gpio_set_pin_function(PB11,
						  // <y> Pin function
						  // <id> pad_function
						  // <i> Auto : use driver pinmux if signal is imported by driver, else turn off function
						  // <PINMUX_PB11I_SDHC0_SDCK"> Auto
						  // <GPIO_PIN_FUNCTION_OFF"> Off
						  // <GPIO_PIN_FUNCTION_A"> A
						  // <GPIO_PIN_FUNCTION_B"> B
						  // <GPIO_PIN_FUNCTION_C"> C
						  // <GPIO_PIN_FUNCTION_D"> D
						  // <GPIO_PIN_FUNCTION_E"> E
						  // <GPIO_PIN_FUNCTION_F"> F
						  // <GPIO_PIN_FUNCTION_G"> G
						  // <GPIO_PIN_FUNCTION_H"> H
						  // <GPIO_PIN_FUNCTION_I"> I
						  // <GPIO_PIN_FUNCTION_J"> J
						  // <GPIO_PIN_FUNCTION_K"> K
						  // <GPIO_PIN_FUNCTION_L"> L
						  // <GPIO_PIN_FUNCTION_M"> M
						  // <GPIO_PIN_FUNCTION_N"> N
						  PINMUX_PB11I_SDHC0_SDCK);

	gpio_set_pin_direction(PA08,
						   // <y> Pin direction
						   // <id> pad_direction
						   // <GPIO_DIRECTION_OFF"> Off
						   // <GPIO_DIRECTION_IN"> In
						   // <GPIO_DIRECTION_OUT"> Out
						   GPIO_DIRECTION_OUT);

	gpio_set_pin_level(PA08,
					   // <y> Initial level
					   // <id> pad_initial_level
					   // <false"> Low
					   // <true"> High
					   false);

	gpio_set_pin_pull_mode(PA08,
						   // <y> Pull configuration
						   // <id> pad_pull_config
						   // <GPIO_PULL_OFF"> Off
						   // <GPIO_PULL_UP"> Pull-up
						   // <GPIO_PULL_DOWN"> Pull-down
						   GPIO_PULL_OFF);

	gpio_set_pin_function(PA08,
						  // <y> Pin function
						  // <id> pad_function
						  // <i> Auto : use driver pinmux if signal is imported by driver, else turn off function
						  // <PINMUX_PA08I_SDHC0_SDCMD"> Auto
						  // <GPIO_PIN_FUNCTION_OFF"> Off
						  // <GPIO_PIN_FUNCTION_A"> A
						  // <GPIO_PIN_FUNCTION_B"> B
						  // <GPIO_PIN_FUNCTION_C"> C
						  // <GPIO_PIN_FUNCTION_D"> D
						  // <GPIO_PIN_FUNCTION_E"> E
						  // <GPIO_PIN_FUNCTION_F"> F
						  // <GPIO_PIN_FUNCTION_G"> G
						  // <GPIO_PIN_FUNCTION_H"> H
						  // <GPIO_PIN_FUNCTION_I"> I
						  // <GPIO_PIN_FUNCTION_J"> J
						  // <GPIO_PIN_FUNCTION_K"> K
						  // <GPIO_PIN_FUNCTION_L"> L
						  // <GPIO_PIN_FUNCTION_M"> M
						  // <GPIO_PIN_FUNCTION_N"> N
						  PINMUX_PA08I_SDHC0_SDCMD);

	gpio_set_pin_direction(PA09,
						   // <y> Pin direction
						   // <id> pad_direction
						   // <GPIO_DIRECTION_OFF"> Off
						   // <GPIO_DIRECTION_IN"> In
						   // <GPIO_DIRECTION_OUT"> Out
						   GPIO_DIRECTION_OUT);

	gpio_set_pin_level(PA09,
					   // <y> Initial level
					   // <id> pad_initial_level
					   // <false"> Low
					   // <true"> High
					   false);

	gpio_set_pin_pull_mode(PA09,
						   // <y> Pull configuration
						   // <id> pad_pull_config
						   // <GPIO_PULL_OFF"> Off
						   // <GPIO_PULL_UP"> Pull-up
						   // <GPIO_PULL_DOWN"> Pull-down
						   GPIO_PULL_OFF);

	gpio_set_pin_function(PA09,
						  // <y> Pin function
						  // <id> pad_function
						  // <i> Auto : use driver pinmux if signal is imported by driver, else turn off function
						  // <PINMUX_PA09I_SDHC0_SDDAT0"> Auto
						  // <GPIO_PIN_FUNCTION_OFF"> Off
						  // <GPIO_PIN_FUNCTION_A"> A
						  // <GPIO_PIN_FUNCTION_B"> B
						  // <GPIO_PIN_FUNCTION_C"> C
						  // <GPIO_PIN_FUNCTION_D"> D
						  // <GPIO_PIN_FUNCTION_E"> E
						  // <GPIO_PIN_FUNCTION_F"> F
						  // <GPIO_PIN_FUNCTION_G"> G
						  // <GPIO_PIN_FUNCTION_H"> H
						  // <GPIO_PIN_FUNCTION_I"> I
						  // <GPIO_PIN_FUNCTION_J"> J
						  // <GPIO_PIN_FUNCTION_K"> K
						  // <GPIO_PIN_FUNCTION_L"> L
						  // <GPIO_PIN_FUNCTION_M"> M
						  // <GPIO_PIN_FUNCTION_N"> N
						  PINMUX_PA09I_SDHC0_SDDAT0);

	gpio_set_pin_direction(PA10,
						   // <y> Pin direction
						   // <id> pad_direction
						   // <GPIO_DIRECTION_OFF"> Off
						   // <GPIO_DIRECTION_IN"> In
						   // <GPIO_DIRECTION_OUT"> Out
						   GPIO_DIRECTION_OUT);

	gpio_set_pin_level(PA10,
					   // <y> Initial level
					   // <id> pad_initial_level
					   // <false"> Low
					   // <true"> High
					   false);

	gpio_set_pin_pull_mode(PA10,
						   // <y> Pull configuration
						   // <id> pad_pull_config
						   // <GPIO_PULL_OFF"> Off
						   // <GPIO_PULL_UP"> Pull-up
						   // <GPIO_PULL_DOWN"> Pull-down
						   GPIO_PULL_OFF);

	gpio_set_pin_function(PA10,
						  // <y> Pin function
						  // <id> pad_function
						  // <i> Auto : use driver pinmux if signal is imported by driver, else turn off function
						  // <PINMUX_PA10I_SDHC0_SDDAT1"> Auto
						  // <GPIO_PIN_FUNCTION_OFF"> Off
						  // <GPIO_PIN_FUNCTION_A"> A
						  // <GPIO_PIN_FUNCTION_B"> B
						  // <GPIO_PIN_FUNCTION_C"> C
						  // <GPIO_PIN_FUNCTION_D"> D
						  // <GPIO_PIN_FUNCTION_E"> E
						  // <GPIO_PIN_FUNCTION_F"> F
						  // <GPIO_PIN_FUNCTION_G"> G
						  // <GPIO_PIN_FUNCTION_H"> H
						  // <GPIO_PIN_FUNCTION_I"> I
						  // <GPIO_PIN_FUNCTION_J"> J
						  // <GPIO_PIN_FUNCTION_K"> K
						  // <GPIO_PIN_FUNCTION_L"> L
						  // <GPIO_PIN_FUNCTION_M"> M
						  // <GPIO_PIN_FUNCTION_N"> N
						  PINMUX_PA10I_SDHC0_SDDAT1);

	gpio_set_pin_direction(PA11,
						   // <y> Pin direction
						   // <id> pad_direction
						   // <GPIO_DIRECTION_OFF"> Off
						   // <GPIO_DIRECTION_IN"> In
						   // <GPIO_DIRECTION_OUT"> Out
						   GPIO_DIRECTION_OUT);

	gpio_set_pin_level(PA11,
					   // <y> Initial level
					   // <id> pad_initial_level
					   // <false"> Low
					   // <true"> High
					   false);

	gpio_set_pin_pull_mode(PA11,
						   // <y> Pull configuration
						   // <id> pad_pull_config
						   // <GPIO_PULL_OFF"> Off
						   // <GPIO_PULL_UP"> Pull-up
						   // <GPIO_PULL_DOWN"> Pull-down
						   GPIO_PULL_OFF);

	gpio_set_pin_function(PA11,
						  // <y> Pin function
						  // <id> pad_function
						  // <i> Auto : use driver pinmux if signal is imported by driver, else turn off function
						  // <PINMUX_PA11I_SDHC0_SDDAT2"> Auto
						  // <GPIO_PIN_FUNCTION_OFF"> Off
						  // <GPIO_PIN_FUNCTION_A"> A
						  // <GPIO_PIN_FUNCTION_B"> B
						  // <GPIO_PIN_FUNCTION_C"> C
						  // <GPIO_PIN_FUNCTION_D"> D
						  // <GPIO_PIN_FUNCTION_E"> E
						  // <GPIO_PIN_FUNCTION_F"> F
						  // <GPIO_PIN_FUNCTION_G"> G
						  // <GPIO_PIN_FUNCTION_H"> H
						  // <GPIO_PIN_FUNCTION_I"> I
						  // <GPIO_PIN_FUNCTION_J"> J
						  // <GPIO_PIN_FUNCTION_K"> K
						  // <GPIO_PIN_FUNCTION_L"> L
						  // <GPIO_PIN_FUNCTION_M"> M
						  // <GPIO_PIN_FUNCTION_N"> N
						  PINMUX_PA11I_SDHC0_SDDAT2);

	gpio_set_pin_direction(PB10,
						   // <y> Pin direction
						   // <id> pad_direction
						   // <GPIO_DIRECTION_OFF"> Off
						   // <GPIO_DIRECTION_IN"> In
						   // <GPIO_DIRECTION_OUT"> Out
						   GPIO_DIRECTION_OUT);

	gpio_set_pin_level(PB10,
					   // <y> Initial level
					   // <id> pad_initial_level
					   // <false"> Low
					   // <true"> High
					   false);

	gpio_set_pin_pull_mode(PB10,
						   // <y> Pull configuration
						   // <id> pad_pull_config
						   // <GPIO_PULL_OFF"> Off
						   // <GPIO_PULL_UP"> Pull-up
						   // <GPIO_PULL_DOWN"> Pull-down
						   GPIO_PULL_OFF);

	gpio_set_pin_function(PB10,
						  // <y> Pin function
						  // <id> pad_function
						  // <i> Auto : use driver pinmux if signal is imported by driver, else turn off function
						  // <PINMUX_PB10I_SDHC0_SDDAT3"> Auto
						  // <GPIO_PIN_FUNCTION_OFF"> Off
						  // <GPIO_PIN_FUNCTION_A"> A
						  // <GPIO_PIN_FUNCTION_B"> B
						  // <GPIO_PIN_FUNCTION_C"> C
						  // <GPIO_PIN_FUNCTION_D"> D
						  // <GPIO_PIN_FUNCTION_E"> E
						  // <GPIO_PIN_FUNCTION_F"> F
						  // <GPIO_PIN_FUNCTION_G"> G
						  // <GPIO_PIN_FUNCTION_H"> H
						  // <GPIO_PIN_FUNCTION_I"> I
						  // <GPIO_PIN_FUNCTION_J"> J
						  // <GPIO_PIN_FUNCTION_K"> K
						  // <GPIO_PIN_FUNCTION_L"> L
						  // <GPIO_PIN_FUNCTION_M"> M
						  // <GPIO_PIN_FUNCTION_N"> N
						  PINMUX_PB10I_SDHC0_SDDAT3);
}

void IO_BUS_CLOCK_init(void)
{
	hri_mclk_set_AHBMASK_SDHC0_bit(MCLK);
	hri_gclk_write_PCHCTRL_reg(GCLK, SDHC0_GCLK_ID, CONF_GCLK_SDHC0_SRC | (1 << GCLK_PCHCTRL_CHEN_Pos));
	hri_gclk_write_PCHCTRL_reg(GCLK, SDHC0_GCLK_ID_SLOW, CONF_GCLK_SDHC0_SLOW_SRC | (1 << GCLK_PCHCTRL_CHEN_Pos));
}

void IO_BUS_init(void)
{
	IO_BUS_CLOCK_init();
	mci_sync_init(&IO_BUS, SDHC0);
	IO_BUS_PORT_init();
}

void USB_DEVICE_INSTANCE_PORT_init(void)
{

	gpio_set_pin_direction(PA24,
						   // <y> Pin direction
						   // <id> pad_direction
						   // <GPIO_DIRECTION_OFF"> Off
						   // <GPIO_DIRECTION_IN"> In
						   // <GPIO_DIRECTION_OUT"> Out
						   GPIO_DIRECTION_OUT);

	gpio_set_pin_level(PA24,
					   // <y> Initial level
					   // <id> pad_initial_level
					   // <false"> Low
					   // <true"> High
					   false);

	gpio_set_pin_pull_mode(PA24,
						   // <y> Pull configuration
						   // <id> pad_pull_config
						   // <GPIO_PULL_OFF"> Off
						   // <GPIO_PULL_UP"> Pull-up
						   // <GPIO_PULL_DOWN"> Pull-down
						   GPIO_PULL_OFF);

	gpio_set_pin_function(PA24,
						  // <y> Pin function
						  // <id> pad_function
						  // <i> Auto : use driver pinmux if signal is imported by driver, else turn off function
						  // <PINMUX_PA24H_USB_DM"> Auto
						  // <GPIO_PIN_FUNCTION_OFF"> Off
						  // <GPIO_PIN_FUNCTION_A"> A
						  // <GPIO_PIN_FUNCTION_B"> B
						  // <GPIO_PIN_FUNCTION_C"> C
						  // <GPIO_PIN_FUNCTION_D"> D
						  // <GPIO_PIN_FUNCTION_E"> E
						  // <GPIO_PIN_FUNCTION_F"> F
						  // <GPIO_PIN_FUNCTION_G"> G
						  // <GPIO_PIN_FUNCTION_H"> H
						  // <GPIO_PIN_FUNCTION_I"> I
						  // <GPIO_PIN_FUNCTION_J"> J
						  // <GPIO_PIN_FUNCTION_K"> K
						  // <GPIO_PIN_FUNCTION_L"> L
						  // <GPIO_PIN_FUNCTION_M"> M
						  // <GPIO_PIN_FUNCTION_N"> N
						  PINMUX_PA24H_USB_DM);

	gpio_set_pin_direction(PA25,
						   // <y> Pin direction
						   // <id> pad_direction
						   // <GPIO_DIRECTION_OFF"> Off
						   // <GPIO_DIRECTION_IN"> In
						   // <GPIO_DIRECTION_OUT"> Out
						   GPIO_DIRECTION_OUT);

	gpio_set_pin_level(PA25,
					   // <y> Initial level
					   // <id> pad_initial_level
					   // <false"> Low
					   // <true"> High
					   false);

	gpio_set_pin_pull_mode(PA25,
						   // <y> Pull configuration
						   // <id> pad_pull_config
						   // <GPIO_PULL_OFF"> Off
						   // <GPIO_PULL_UP"> Pull-up
						   // <GPIO_PULL_DOWN"> Pull-down
						   GPIO_PULL_OFF);

	gpio_set_pin_function(PA25,
						  // <y> Pin function
						  // <id> pad_function
						  // <i> Auto : use driver pinmux if signal is imported by driver, else turn off function
						  // <PINMUX_PA25H_USB_DP"> Auto
						  // <GPIO_PIN_FUNCTION_OFF"> Off
						  // <GPIO_PIN_FUNCTION_A"> A
						  // <GPIO_PIN_FUNCTION_B"> B
						  // <GPIO_PIN_FUNCTION_C"> C
						  // <GPIO_PIN_FUNCTION_D"> D
						  // <GPIO_PIN_FUNCTION_E"> E
						  // <GPIO_PIN_FUNCTION_F"> F
						  // <GPIO_PIN_FUNCTION_G"> G
						  // <GPIO_PIN_FUNCTION_H"> H
						  // <GPIO_PIN_FUNCTION_I"> I
						  // <GPIO_PIN_FUNCTION_J"> J
						  // <GPIO_PIN_FUNCTION_K"> K
						  // <GPIO_PIN_FUNCTION_L"> L
						  // <GPIO_PIN_FUNCTION_M"> M
						  // <GPIO_PIN_FUNCTION_N"> N
						  PINMUX_PA25H_USB_DP);
}

/* The USB module requires a GCLK_USB of 48 MHz ~ 0.25% clock
 * for low speed and full speed operation. */
#if (CONF_GCLK_USB_FREQUENCY > (48000000 + 48000000 / 400)) || (CONF_GCLK_USB_FREQUENCY < (48000000 - 48000000 / 400))
#warning USB clock should be 48MHz ~ 0.25% clock, check your configuration!
#endif

void USB_DEVICE_INSTANCE_CLOCK_init(void)
{

	hri_gclk_write_PCHCTRL_reg(GCLK, USB_GCLK_ID, CONF_GCLK_USB_SRC | GCLK_PCHCTRL_CHEN);
	hri_mclk_set_AHBMASK_USB_bit(MCLK);
	hri_mclk_set_APBBMASK_USB_bit(MCLK);
}

void USB_DEVICE_INSTANCE_init(void)
{
	USB_DEVICE_INSTANCE_CLOCK_init();
	usb_d_init();
	USB_DEVICE_INSTANCE_PORT_init();
}

void system_init(void)
{
	init_mcu();

	gpio_set_pin_level(LED3,false);
	gpio_set_pin_direction(LED3, GPIO_DIRECTION_OUT);
	gpio_set_pin_function(LED3, GPIO_PIN_FUNCTION_OFF);

	gpio_set_pin_level(LED4,false);
	gpio_set_pin_direction(LED4, GPIO_DIRECTION_OUT);
	gpio_set_pin_function(LED4, GPIO_PIN_FUNCTION_OFF);

	gpio_set_pin_level(BUZZER, false);
	gpio_set_pin_direction(BUZZER,GPIO_DIRECTION_OUT);
	gpio_set_pin_function(BUZZER,GPIO_PIN_FUNCTION_OFF);

	IO_BUS_init();

	USB_DEVICE_INSTANCE_init();
}
