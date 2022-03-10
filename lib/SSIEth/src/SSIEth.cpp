#include "SSIEth.hpp"

#include "hal_mac_async.h"

#include "hal_gpio.h"

#define PA12 GPIO(GPIO_PORTA, 12)
#define PA13 GPIO(GPIO_PORTA, 13)
#define PA14 GPIO(GPIO_PORTA, 14)
#define PA15 GPIO(GPIO_PORTA, 15)
#define PA17 GPIO(GPIO_PORTA, 17)
#define PA18 GPIO(GPIO_PORTA, 18)
#define PA19 GPIO(GPIO_PORTA, 19)
#define PB14 GPIO(GPIO_PORTB, 14)
#define PB15 GPIO(GPIO_PORTB, 15)
#define PC20 GPIO(GPIO_PORTC, 20)

void SSIEth::init(){

	hri_mclk_set_AHBMASK_GMAC_bit(MCLK);
	hri_mclk_set_APBCMASK_GMAC_bit(MCLK);

	mac_async_init(&COMMUNICATION_IO, GMAC);

	gpio_set_pin_function(PB14, PINMUX_PB14L_GMAC_GMDC);
	gpio_set_pin_function(PB15, PINMUX_PB15L_GMAC_GMDIO);
	gpio_set_pin_function(PA13, PINMUX_PA13L_GMAC_GRX0);
	gpio_set_pin_function(PA12, PINMUX_PA12L_GMAC_GRX1);
	gpio_set_pin_function(PC20, PINMUX_PC20L_GMAC_GRXDV);
	gpio_set_pin_function(PA15, PINMUX_PA15L_GMAC_GRXER);
	gpio_set_pin_function(PA18, PINMUX_PA18L_GMAC_GTX0);
	gpio_set_pin_function(PA19, PINMUX_PA19L_GMAC_GTX1);
	gpio_set_pin_function(PA14, PINMUX_PA14L_GMAC_GTXCK);
	gpio_set_pin_function(PA17, PINMUX_PA17L_GMAC_GTXEN);

    }

void SSIEth::test(){
	mac_async_enable(&COMMUNICATION_IO);
	mac_async_write(&COMMUNICATION_IO, (uint8_t *)"Hello World!", 12);
}