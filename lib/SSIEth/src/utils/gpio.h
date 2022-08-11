#pragma once

#define GPIO_PIN(n) (((n)&0x1Fu) << 0)
#define GPIO_PORT(n) ((n) >> 5)
#define GPIO(port, pin) ((((port)&0x7u) << 5) + ((pin)&0x1Fu))
#define GPIO_PIN_FUNCTION_OFF 0xffffffff

/**
 * \brief PORT pull mode settings
 */
enum gpio_pull_mode { GPIO_PULL_OFF, GPIO_PULL_UP, GPIO_PULL_DOWN };

/**
 * \brief PORT direction settins
 */
enum gpio_direction { GPIO_DIRECTION_OFF, GPIO_DIRECTION_IN, GPIO_DIRECTION_OUT };

/**
 * \brief PORT group abstraction
 */

enum gpio_port { GPIO_PORTA, GPIO_PORTB, GPIO_PORTC, GPIO_PORTD, GPIO_PORTE };


/**
 * \brief Set gpio pin function
 */
static inline void gpio_set_pin_function(const uint32_t gpio, const uint32_t function)
{
	uint8_t port = GPIO_PORT(gpio);
	uint8_t pin  = GPIO_PIN(gpio);

	if (function == GPIO_PIN_FUNCTION_OFF) {
		hri_port_write_PINCFG_PMUXEN_bit(PORT, port, pin, false);

	} else {
		hri_port_write_PINCFG_PMUXEN_bit(PORT, port, pin, true);

		if (pin & 1) {
			// Odd numbered pin
			hri_port_write_PMUX_PMUXO_bf(PORT, port, pin >> 1, function & 0xffff);
		} else {
			// Even numbered pin
			hri_port_write_PMUX_PMUXE_bf(PORT, port, pin >> 1, function & 0xffff);
		}
	}
}