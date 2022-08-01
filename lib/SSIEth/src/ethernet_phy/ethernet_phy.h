
#pragma once

#include "return.h"
#include "compiler.h"
#include "hal_mac_async.h"
#include "ieee8023_mii_standard_register.h"

class PHY {
	public:
		PHY(struct mac_async_descriptor *mac, uint16_t addr);
		return_t read_reg(uint16_t reg, uint16_t &val);
		return_t write_reg(uint16_t reg, uint16_t val);
		return_t set_reg_bit(uint16_t reg, uint16_t ofst);
		return_t clear_reg_bit(uint16_t reg, uint16_t ofst);
		return_t set_powerdown(bool state);
		return_t set_isolate(bool state);
		return_t restart_autoneg();
		return_t set_loopback(bool state);
		return_t get_link_status(bool &status);
		return_t reset();

	private:
		struct mac_async_descriptor *mac;  /* MAC descriptor handler */
		uint16_t addr; /* PHY address, defined by IEEE802.3 section 22.2.4.5.5 */
};