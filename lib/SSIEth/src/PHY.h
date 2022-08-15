
#pragma once

#include "return.h"
#include "compiler.h"
#include "MAC.h"

class PHY
{
public:
	PHY(MAC &mac, uint16_t addr);
	result_t read_reg(uint16_t reg, uint16_t &val);
	result_t write_reg(uint16_t reg, uint16_t val);
	result_t set_reg_bit(uint16_t reg, uint16_t ofst);
	result_t clear_reg_bit(uint16_t reg, uint16_t ofst);
	result_t set_powerdown(bool state);
	result_t set_isolate(bool state);
	result_t restart_autoneg();
	result_t set_loopback(bool state);
	result_t get_link_status(bool &status);
	result_t reset();

private:
	MAC &mac;
	uint16_t addr; /* PHY address, defined by IEEE802.3 section 22.2.4.5.5 */
};