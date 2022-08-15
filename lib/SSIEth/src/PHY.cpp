#include "PHY.h"
#include <utils/utils_assert.h>
#include "ieee8023_mii_standard_register.h"

/**
 * @brief Construct a new PHY object
 *
 * @param mac
 * @param addr the PHY address, unsigned int 0-31
 */
PHY::PHY(EthMAC &mac, uint16_t addr) : mac(mac), addr(addr)
{
	ASSERT(addr <= 0x1F);
}

/**
 * \brief Read PHY Register value.
 * \note For conformance with the 802.3 specification, MDC must not exceed
 *       2.5 MHz (MDC is only active during MDIO read and write operations).
 *       The function execution time depend on MDC frequency.
 * \param[in]  reg   Register address
 * \param[out] val   Register value
 * \return Operation result.
 */
result_t PHY::read_reg(uint16_t reg, uint16_t &val)
{
	return mac.read_phy_reg(addr, reg, &val);
}

/**
 * \brief Write PHY Register value.
 * \note For conformance with the 802.3 specification, MDC must not exceed
 *       2.5 MHz (MDC is only active during MDIO read and write operations).
 *       The function execution time depend on MDC frequency.
 * \param[in]  reg   Register address
 * \param[out] val   Register value
 *
 * \return Operation result.
 */
result_t PHY::write_reg(uint16_t reg, uint16_t val)
{
	return mac.write_phy_reg(addr, reg, val);
}

/**
 * \brief Setting bit for a PHY Register
 * \param[in] reg   Register address.
 * \param[in] ofst  Register bit mask.
 * \return Operation result.
 */
result_t PHY::set_reg_bit(uint16_t reg, uint16_t ofst)
{
	result_t rst;
	uint16_t val;

	ASSERT(reg <= 0x1F);

	rst = mac.read_phy_reg(addr, reg, &val);
	if (rst == RET::OK)
	{
		val |= ofst;
		rst = mac.write_phy_reg(addr, reg, val);
	}
	return rst;
}

/**
 * \brief Clear bit for a PHY Register
 * \param[in] reg   Register address.
 * \param[in] ofst  Register bit mask.
 * \return Operation result.
 */
result_t PHY::clear_reg_bit(uint16_t reg, uint16_t ofst)
{
	result_t rst;
	uint16_t val;

	ASSERT(reg <= 0x1F);

	rst = mac.read_phy_reg(addr, reg, &val);
	if (rst == RET::OK)
	{
		val &= ~ofst;
		rst = mac.write_phy_reg(addr, reg, val);
	}
	return rst;
}

/**
 * \brief Set PHY low-power consumption state.
 * The specific behavior of a PHY in the power-down state is implementation
 * specific. While in the power-down state, the PHY shall respond to management
 * transactions. During the transition to the power-down state and while in the
 * power-down state, the PHY shall not generate spurious signals on the MII or
 * GMII.
 * \param[in] state The state of the power-down mode.
 * \return Operation result.
 */
result_t PHY::set_powerdown(bool state)
{
	if (state)
		return set_reg_bit(MDIO_REG0_BMCR, MDIO_REG0_BIT_POWER_DOWN);
	else
		return clear_reg_bit(MDIO_REG0_BMCR, MDIO_REG0_BIT_POWER_DOWN);
}

/**
 * \brief Set PHY electrically isolate state.
 * When the PHY is isolated from the MII or RMII it shall not respond to the
 * data bundle.
 * \param[in] state The state of the isolate mode.
 * \return Operation result.
 */
result_t PHY::set_isolate(bool state)
{
	if (state)
		return set_reg_bit(MDIO_REG0_BMCR, MDIO_REG0_BIT_ISOLATE);
	else
		return clear_reg_bit(MDIO_REG0_BMCR, MDIO_REG0_BIT_ISOLATE);
}

/**
 * @brief Restart an auto negotiation of the PHY.
 *
 * @return result_t
 */
result_t PHY::restart_autoneg()
{
	return set_reg_bit(MDIO_REG0_BMCR, MDIO_REG0_BIT_RESTART_AUTONEG);
}

/**
 * \brief Set PHY placed in a loopback mode of operation.
 * When in loopback mode, the PHY shall accept data from the MII/RMII transmit
 * data path and return it to the MII/RMII receive data path.
 * \param[in] state State of the loopback mode.
 * @return result_t
 */
result_t PHY::set_loopback(bool state)
{
	if (state)
		return set_reg_bit(MDIO_REG0_BMCR, MDIO_REG0_BIT_LOOPBACK);
	else
		return clear_reg_bit(MDIO_REG0_BMCR, MDIO_REG0_BIT_LOOPBACK);
}

/**
 * @brief Get PHY link status
 *
 * @param status
 * @return result_t
 */
result_t PHY::get_link_status(bool &status)
{
	result_t rst;
	uint16_t val;

	rst = read_reg(MDIO_REG1_BMSR, val);
	if (rst == RET::OK)
		status = (val & MDIO_REG1_BIT_LINK_STATUS) ? true : false;
	return rst;
}

/**
 * @brief Reset PHY.
 * Resetting PHY, this action set all the status and control register to their
 * default states. As a consequence this action may change the internal state
 * of the PHY and the state of the physical link associated with the PHY. The
 * reset process shall be completed within 0.5 second.
 * @return result_t
 */
result_t PHY::reset()
{
	return set_reg_bit(MDIO_REG0_BMCR, MDIO_REG0_BIT_RESET);
}