
/*
TODO: This library works, but can only be used once. It needs to be re-writen as a C++ class that can be instantiated multiple times.
*/

#include "BMP388.hpp"

/***************** Internal macros ******************************/

/*! Power control settings */
#define POWER_CNTL UINT16_C(0x0006)
/*! Odr and filter settings */
#define ODR_FILTER UINT16_C(0x00F0)
/*! Interrupt control settings */
#define INT_CTRL UINT16_C(0x0708)
/*! Advance settings */
#define ADV_SETT UINT16_C(0x1800)

BMP388::BMP388(SPIClass *spi, int8_t cspin, const char *id) : Sensor(id)
{
	_spi = spi;
	_cs = cspin;
	_filterEnabled = _tempOSEnabled = _presOSEnabled = false;
}

bool BMP388::init()
{

	int8_t rslt = BMP3_OK;
	uint8_t read_chip_id = 0;

	rslt = get_regs(BMP3_CHIP_ID_ADDR, &read_chip_id, 1);
	/* Proceed if everything is fine until now */
	if (rslt == BMP3_OK)
	{
		/* Check for chip id validity */
		if (read_chip_id == BMP3_CHIP_ID)
		{
			chip_id = read_chip_id;
			/* Reset the sensor */
			rslt = soft_reset();
			if (rslt == BMP3_OK)
			{
				/* Read the calibration data */
				rslt = get_calib_data();
			}
		}
		else
		{
			rslt = BMP3_E_DEV_NOT_FOUND;
		}
	}

	setTemperatureOversampling(BMP3_NO_OVERSAMPLING);
	setPressureOversampling(BMP3_NO_OVERSAMPLING);
	setIIRFilterCoeff(BMP3_IIR_FILTER_DISABLE);

	// don't do anything till we request a reading
	settings.op_mode = BMP3_FORCED_MODE;

	return true;
}

/**************************************************************************/
/*!
    @brief Performs a full reading of all sensors in the BMP3XX.

    Assigns the internal BMP3xx#temperature & BMP3xx#pressure member variables

    @return True on success, False on failure
*/
/**************************************************************************/
BMP388::Data BMP388::readSensor(void)
{
	int8_t rslt;
	/* Used to select the settings user needs to change */
	uint16_t settings_sel = 0;
	/* Variable used to select the sensor component */
	uint8_t sensor_comp = 0;

	/* Variable used to store the compensated data */
	struct bmp3_data data = {0, 0};

	/* Select the pressure and temperature sensor to be enabled */
	settings.temp_en = BMP3_ENABLE;
	settings_sel |= BMP3_TEMP_EN_SEL;
	sensor_comp |= BMP3_TEMP;
	if (_tempOSEnabled)
	{
		settings_sel |= BMP3_TEMP_OS_SEL;
	}

	settings.press_en = BMP3_ENABLE;
	settings_sel |= BMP3_PRESS_EN_SEL;
	sensor_comp |= BMP3_PRESS;
	if (_presOSEnabled)
	{
		settings_sel |= BMP3_PRESS_OS_SEL;
	}

	if (_filterEnabled)
	{
		settings_sel |= BMP3_IIR_FILTER_SEL;
	}

	if (_ODREnabled)
	{
		settings_sel |= BMP3_ODR_SEL;
	}

	// set interrupt to data ready
	//settings_sel |= BMP3_DRDY_EN_SEL | BMP3_LEVEL_SEL | BMP3_LATCH_SEL;

	/* Set the desired sensor configuration */
#ifdef BMP3XX_DEBUG
	Serial.println("Setting sensor settings");
#endif
	rslt = set_sensor_settings(settings_sel);
	if (rslt != BMP3_OK)
		return data;

	/* Set the power mode */
	settings.op_mode = BMP3_FORCED_MODE;
#ifdef BMP3XX_DEBUG
	Serial.println("Setting power mode");
#endif
	rslt = set_op_mode();
	if (rslt != BMP3_OK)
		return data;

	/* Temperature and Pressure data are read and stored in the bmp3_data instance */
	rslt = get_sensor_data(sensor_comp, &data);

	/* Save the temperature and pressure data */

	if (rslt != BMP3_OK)
		return data;

	return data;
}

/**************************************************************************/
/*!
    @brief  Setter for Temperature oversampling
    @param  oversample Oversampling setting, can be BMP3_NO_OVERSAMPLING, BMP3_OVERSAMPLING_2X, BMP3_OVERSAMPLING_4X, BMP3_OVERSAMPLING_8X, BMP3_OVERSAMPLING_16X, BMP3_OVERSAMPLING_32X
    @return True on success, False on failure
*/
/**************************************************************************/

bool BMP388::setTemperatureOversampling(uint8_t oversample)
{
	if (oversample > BMP3_OVERSAMPLING_32X)
		return false;

	settings.odr_filter.temp_os = oversample;

	if (oversample == BMP3_NO_OVERSAMPLING)
		_tempOSEnabled = false;
	else
		_tempOSEnabled = true;

	return true;
}

/**************************************************************************/
/*!
    @brief  Setter for Pressure oversampling
    @param  oversample Oversampling setting, can be BMP3_NO_OVERSAMPLING, BMP3_OVERSAMPLING_2X, BMP3_OVERSAMPLING_4X, BMP3_OVERSAMPLING_8X, BMP3_OVERSAMPLING_16X, BMP3_OVERSAMPLING_32X
    @return True on success, False on failure
*/
/**************************************************************************/
bool BMP388::setPressureOversampling(uint8_t oversample)
{
	if (oversample > BMP3_OVERSAMPLING_32X)
		return false;

	settings.odr_filter.press_os = oversample;

	if (oversample == BMP3_NO_OVERSAMPLING)
		_presOSEnabled = false;
	else
		_presOSEnabled = true;

	return true;
}

/**************************************************************************/
/*!
    @brief  Setter for IIR filter coefficient
    @param filtercoeff Coefficient of the filter (in samples). Can be BMP3_IIR_FILTER_DISABLE (no filtering), BMP3_IIR_FILTER_COEFF_1, BMP3_IIR_FILTER_COEFF_3, BMP3_IIR_FILTER_COEFF_7, BMP3_IIR_FILTER_COEFF_15, BMP3_IIR_FILTER_COEFF_31, BMP3_IIR_FILTER_COEFF_63, BMP3_IIR_FILTER_COEFF_127
    @return True on success, False on failure

*/
/**************************************************************************/
bool BMP388::setIIRFilterCoeff(uint8_t filtercoeff)
{
	if (filtercoeff > BMP3_IIR_FILTER_COEFF_127)
		return false;

	settings.odr_filter.iir_filter = filtercoeff;

	if (filtercoeff == BMP3_IIR_FILTER_DISABLE)
		_filterEnabled = false;
	else
		_filterEnabled = true;

	return true;
}

/**************************************************************************/
/*!
    @brief  Setter for output data rate (ODR)
    @param odr Sample rate in Hz. Can be BMP3_ODR_200_HZ, BMP3_ODR_100_HZ, BMP3_ODR_50_HZ, BMP3_ODR_25_HZ, BMP3_ODR_12_5_HZ, BMP3_ODR_6_25_HZ, BMP3_ODR_3_1_HZ, BMP3_ODR_1_5_HZ, BMP3_ODR_0_78_HZ, BMP3_ODR_0_39_HZ, BMP3_ODR_0_2_HZ, BMP3_ODR_0_1_HZ, BMP3_ODR_0_05_HZ, BMP3_ODR_0_02_HZ, BMP3_ODR_0_01_HZ, BMP3_ODR_0_006_HZ, BMP3_ODR_0_003_HZ, or BMP3_ODR_0_001_HZ 
    @return True on success, False on failur
*/
/**************************************************************************/
bool BMP388::setOutputDataRate(uint8_t odr)
{
	if (odr > BMP3_ODR_0_001_HZ)
		return false;

	settings.odr_filter.odr = odr;

	_ODREnabled = true;

	return true;
}

int8_t BMP388::spi_read(uint8_t reg_addr, uint8_t *reg_data, uint16_t len)
{

	uint8_t buf[len + 1];

	memset(buf, 0x00, len + 1);

	buf[0] = reg_addr | 0x80;

	digitalWrite(_cs,LOW);
	_spi->transfer(buf, len + 1);
	digitalWrite(_cs,HIGH);

	memcpy(reg_data, buf + 1, len);

	return 0;
}

int8_t BMP388::spi_write(uint8_t reg_addr, uint8_t *reg_data, uint16_t len)
{
	uint8_t buf[len + 1];

	buf[0] = reg_addr;
	memcpy(buf + 1, reg_data, len);

	digitalWrite(_cs,LOW);
	_spi->transfer(buf, len + 1);
	digitalWrite(_cs,HIGH);

	return 0;
}

int8_t BMP388::get_regs(uint8_t reg_addr, uint8_t *reg_data, uint16_t len)
{
	int8_t rslt;
	uint16_t temp_len = len + 1;
	uint16_t i;
	uint8_t temp_buff[len + 1];

	reg_addr = reg_addr | 0x80;
	/* Read the data from the register */
	rslt = spi_read(reg_addr, temp_buff, temp_len);

	for (i = 0; i < len; i++)
		reg_data[i] = temp_buff[i + 1];
	/* Check for communication error */
	if (rslt != BMP3_OK)
		rslt = BMP3_E_COMM_FAIL;

	return rslt;
}

int8_t BMP388::set_regs(uint8_t *reg_addr, const uint8_t *reg_data, uint8_t len)
{
	int8_t rslt;
	uint8_t temp_buff[len * 2];
	uint16_t temp_len;
	uint8_t reg_addr_cnt;

	/* Check for arguments validity */
	if ((reg_addr != NULL) && (reg_data != NULL))
	{
		if (len != 0)
		{
			temp_buff[0] = reg_data[0];
			/* interface selected is SPI */
			for (reg_addr_cnt = 0; reg_addr_cnt < len; reg_addr_cnt++)
				reg_addr[reg_addr_cnt] = reg_addr[reg_addr_cnt] & 0x7F;
			/* Burst write mode */
			if (len > 1)
			{
				/* Interleave register address w.r.t data for
				burst write*/
				interleave_reg_addr(reg_addr, temp_buff, reg_data, len);
				temp_len = len * 2;
			}
			else
			{
				temp_len = len;
			}
			rslt = spi_write(reg_addr[0], temp_buff, temp_len);
			/* Check for communication error */
			if (rslt != BMP3_OK)
				rslt = BMP3_E_COMM_FAIL;
		}
		else
		{
			rslt = BMP3_E_INVALID_LEN;
		}
	}
	else
	{
		rslt = BMP3_E_NULL_PTR;
	}
	return rslt;
}

int8_t BMP388::soft_reset()
{
	int8_t rslt;
	uint8_t reg_addr = BMP3_CMD_ADDR;
	/* 0xB6 is the soft reset command */
	uint8_t soft_rst_cmd = 0xB6;
	uint8_t cmd_rdy_status;
	uint8_t cmd_err_status;

	/* Check for command ready status */
	rslt = get_regs(BMP3_SENS_STATUS_REG_ADDR, &cmd_rdy_status, 1);
	/* Device is ready to accept new command */
	if ((cmd_rdy_status & BMP3_CMD_RDY) && (rslt == BMP3_OK))
	{
		/* Write the soft reset command in the sensor */
		rslt = set_regs(&reg_addr, &soft_rst_cmd, 1);
		/* Proceed if everything is fine until now */
		if (rslt == BMP3_OK)
		{
			/* Wait for 2 ms */
			//delay_ms(2); //TODO FIX!
			/* Read for command error status */
			rslt = get_regs(BMP3_ERR_REG_ADDR, &cmd_err_status, 1);
			/* check for command error status */
			if ((cmd_err_status & BMP3_CMD_ERR) || (rslt != BMP3_OK))
			{
				/* Command not written hence return
            error */
				rslt = BMP3_E_CMD_EXEC_FAILED;
			}
		}
	}
	else
	{
		rslt = BMP3_E_CMD_EXEC_FAILED;
	}

	return rslt;
}

int8_t BMP388::set_op_mode()
{
	int8_t rslt;
	uint8_t last_set_mode;
	uint8_t curr_mode = settings.op_mode;

	rslt = get_op_mode(&last_set_mode);
	/* If the sensor is not in sleep mode put the device to sleep
		   mode */
	if (last_set_mode != BMP3_SLEEP_MODE)
	{
		/* Device should be put to sleep before transiting to
			   forced mode or normal mode */
		rslt = put_device_to_sleep();
		/* Give some time for device to go into sleep mode */
		//TODO FIX! delay_ms(5);
	}
	/* Set the power mode */
	if (rslt == BMP3_OK)
	{
		if (curr_mode == BMP3_NORMAL_MODE)
		{
			/* Set normal mode and validate
				   necessary settings */
			rslt = set_normal_mode();
		}
		else if (curr_mode == BMP3_FORCED_MODE)
		{
			/* Set forced mode */
			rslt = write_power_mode();
		}
	}

	return rslt;
}

int8_t BMP388::write_power_mode()
{
	int8_t rslt;
	uint8_t reg_addr = BMP3_PWR_CTRL_ADDR;
	uint8_t op_mode = settings.op_mode;
	/* Temporary variable to store the value read from op-mode register */
	uint8_t op_mode_reg_val;

	/* Read the power mode register */
	rslt = get_regs(reg_addr, &op_mode_reg_val, 1);
	/* Set the power mode */
	if (rslt == BMP3_OK)
	{
		op_mode_reg_val = BMP3_SET_BITS(op_mode_reg_val, BMP3_OP_MODE, op_mode);
		/* Write the power mode in the register */
		rslt = set_regs(&reg_addr, &op_mode_reg_val, 1);
	}

	return rslt;
}

int8_t BMP388::set_normal_mode()
{
	int8_t rslt;
	uint8_t conf_err_status;

	rslt = validate_normal_mode_settings();
	/* If osr and odr settings are proper then write the power mode */
	if (rslt == BMP3_OK)
	{
		rslt = write_power_mode();
		/* check for configuration error */
		if (rslt == BMP3_OK)
		{
			/* Read the configuration error status */
			rslt = get_regs(BMP3_ERR_REG_ADDR, &conf_err_status, 1);
			/* Check if conf. error flag is set */
			if (rslt == BMP3_OK)
			{
				if (conf_err_status & BMP3_CONF_ERR)
				{
					/* Osr and odr configuration is
					   not proper */
					rslt = BMP3_E_CONFIGURATION_ERR;
				}
			}
		}
	}

	return rslt;
}

int8_t BMP388::validate_normal_mode_settings()
{
	int8_t rslt;

	rslt = get_odr_filter_settings();

	if (rslt == BMP3_OK)
		rslt = validate_osr_and_odr_settings();

	return rslt;
}

int8_t BMP388::get_odr_filter_settings()
{
	int8_t rslt;
	uint8_t reg_data[4];

	/* Read data beginning from 0x1C register */
	rslt = get_regs(BMP3_OSR_ADDR, reg_data, 4);
	/* Parse the read data and store it in dev structure */
	parse_odr_filter_settings(reg_data, &settings.odr_filter);

	return rslt;
}

void BMP388::parse_odr_filter_settings(const uint8_t *reg_data, struct bmp3_odr_filter_settings *settings)
{
	uint8_t index = 0;

	/* Odr and filter settings index starts from one (0x1C register) */
	settings->press_os = BMP3_GET_BITS_POS_0(reg_data[index], BMP3_PRESS_OS);
	settings->temp_os = BMP3_GET_BITS(reg_data[index], BMP3_TEMP_OS);

	/* Move index to 0x1D register */
	index++;
	settings->odr = BMP3_GET_BITS_POS_0(reg_data[index], BMP3_ODR);

	/* Move index to 0x1F register */
	index = index + 2;
	settings->iir_filter = BMP3_GET_BITS(reg_data[index], BMP3_IIR_FILTER);
}

int8_t BMP388::validate_osr_and_odr_settings()
{
	int8_t rslt;
	uint16_t meas_t = 0;
	/* Odr values in milli secs  */
	uint32_t odr[18] = {5, 10, 20, 40, 80, 160, 320, 640, 1280, 2560, 5120, 10240,
						20480, 40960, 81920, 163840, 327680, 655360};

	if (settings.press_en)
	{
		/* Calculate the pressure measurement duration */
		meas_t = calculate_press_meas_time();
	}
	if (settings.temp_en)
	{
		/* Calculate the temperature measurement duration */
		meas_t += calculate_temp_meas_time();
	}
	rslt = verify_meas_time_and_odr_duration(meas_t, odr[settings.odr_filter.odr]);

	return rslt;
}

uint16_t BMP388::calculate_temp_meas_time()
{
	uint16_t temp_meas_t;
	struct bmp3_odr_filter_settings odr_filter = settings.odr_filter;
#ifdef BMP3_DOUBLE_PRECISION_COMPENSATION
	float base = 2.0;
	float partial_out;
#else
	uint8_t base = 2;
	uint32_t partial_out;
#endif /* BMP3_DOUBLE_PRECISION_COMPENSATION */

	partial_out = bmp3_pow(base, odr_filter.temp_os);
	temp_meas_t = (uint16_t)(BMP3_TEMP_SETTLE_TIME + partial_out * BMP3_ADC_CONV_TIME);
	/* convert into mill seconds */
	temp_meas_t = temp_meas_t / 1000;

	return temp_meas_t;
}

int8_t BMP388::verify_meas_time_and_odr_duration(uint16_t meas_t, uint32_t odr_duration)
{
	int8_t rslt;

	if (meas_t < odr_duration)
	{
		/* If measurement duration is less than odr duration
		   then osr and odr settings are fine */
		rslt = BMP3_OK;
	}
	else
	{
		/* Osr and odr settings are not proper */
		rslt = BMP3_E_INVALID_ODR_OSR_SETTINGS;
	}

	return rslt;
}

uint16_t BMP388::calculate_press_meas_time()
{
	uint16_t press_meas_t;
	struct bmp3_odr_filter_settings odr_filter = settings.odr_filter;
#ifdef BMP3_DOUBLE_PRECISION_COMPENSATION
	double base = 2.0;
	double partial_out;
#else
	uint8_t base = 2;
	uint32_t partial_out;
#endif /* BMP3_DOUBLE_PRECISION_COMPENSATION */

	partial_out = bmp3_pow(base, odr_filter.press_os);
	press_meas_t = (uint16_t)(BMP3_PRESS_SETTLE_TIME + partial_out * BMP3_ADC_CONV_TIME);
	/* convert into mill seconds */
	press_meas_t = press_meas_t / 1000;

	return press_meas_t;
}

int8_t BMP388::put_device_to_sleep()
{
	int8_t rslt;
	uint8_t reg_addr = BMP3_PWR_CTRL_ADDR;
	/* Temporary variable to store the value read from op-mode register */
	uint8_t op_mode_reg_val;

	rslt = get_regs(BMP3_PWR_CTRL_ADDR, &op_mode_reg_val, 1);

	if (rslt == BMP3_OK)
	{
		/* Set the power mode */
		op_mode_reg_val = op_mode_reg_val & (~(BMP3_OP_MODE_MSK));

		/* Write the power mode in the register */
		rslt = set_regs(&reg_addr, &op_mode_reg_val, 1);
	}

	return rslt;
}

int8_t BMP388::get_op_mode(uint8_t *op_mode)
{
	int8_t rslt;

	/* Read the power mode register */
	rslt = get_regs(BMP3_PWR_CTRL_ADDR, op_mode, 1);
	/* Assign the power mode in the device structure */
	*op_mode = BMP3_GET_BITS(*op_mode, BMP3_OP_MODE);

	return rslt;
}

int8_t BMP388::get_sensor_data(uint8_t sensor_comp, struct bmp3_data *comp_data)
{
	int8_t rslt;
	/* Array to store the pressure and temperature data read from
	the sensor */
	uint8_t reg_data[BMP3_P_T_DATA_LEN] = {0};
	struct bmp3_uncomp_data uncomp_data = {0};

	/* Check for null pointer in the device structure*/

	if (comp_data != NULL)
	{
		/* Read the pressure and temperature data from the sensor */
		rslt = get_regs(BMP3_DATA_ADDR, reg_data, BMP3_P_T_DATA_LEN);

		if (rslt == BMP3_OK)
		{
			/* Parse the read data from the sensor */
			parse_sensor_data(reg_data, &uncomp_data);
			/* Compensate the pressure/temperature/both data read
			   from the sensor */
			rslt = compensate_data(sensor_comp, &uncomp_data, comp_data, &calib_data);
		}
	}
	else
	{
		rslt = BMP3_E_NULL_PTR;
	}

	return rslt;
}

void BMP388::parse_sensor_data(const uint8_t *reg_data, struct bmp3_uncomp_data *uncomp_data)
{
	/* Temporary variables to store the sensor data */
	uint32_t data_xlsb;
	uint32_t data_lsb;
	uint32_t data_msb;

	/* Store the parsed register values for pressure data */
	data_xlsb = (uint32_t)reg_data[0];
	data_lsb = (uint32_t)reg_data[1] << 8;
	data_msb = (uint32_t)reg_data[2] << 16;
	uncomp_data->pressure = data_msb | data_lsb | data_xlsb;

	/* Store the parsed register values for temperature data */
	data_xlsb = (uint32_t)reg_data[3];
	data_lsb = (uint32_t)reg_data[4] << 8;
	data_msb = (uint32_t)reg_data[5] << 16;
	uncomp_data->temperature = data_msb | data_lsb | data_xlsb;
}

int8_t BMP388::compensate_data(uint8_t sensor_comp, const struct bmp3_uncomp_data *uncomp_data,
							   struct bmp3_data *comp_data, struct bmp3_calib_data *calib_data)
{
	int8_t rslt = BMP3_OK;

	if ((uncomp_data != NULL) && (comp_data != NULL) && (calib_data != NULL))
	{
		/* If pressure or temperature component is selected */
		if (sensor_comp & (BMP3_PRESS | BMP3_TEMP))
		{
			/* Compensate the temperature data */
			comp_data->temperature = compensate_temperature(uncomp_data, calib_data);
		}
		if (sensor_comp & BMP3_PRESS)
		{
			/* Compensate the pressure data */
			comp_data->pressure = compensate_pressure(uncomp_data, calib_data);
		}
	}
	else
	{
		rslt = BMP3_E_NULL_PTR;
	}

	return rslt;
}

int8_t BMP388::get_calib_data()
{
	int8_t rslt;
	uint8_t reg_addr = BMP3_CALIB_DATA_ADDR;
	/* Array to store calibration data */
	uint8_t calib_data[BMP3_CALIB_DATA_LEN] = {0};

	/* Read the calibration data from the sensor */
	rslt = get_regs(reg_addr, calib_data, BMP3_CALIB_DATA_LEN);
	/* Parse calibration data and store it in device structure */
	parse_calib_data(calib_data);

	return rslt;
}

int8_t BMP388::set_sensor_settings(uint32_t desired_settings)
{
	int8_t rslt;

	if (are_settings_changed(POWER_CNTL, desired_settings))
	{
		/* Set the power control settings */
		rslt = set_pwr_ctrl_settings(desired_settings);
	}
	if (are_settings_changed(ODR_FILTER, desired_settings) && (!rslt))
	{
		/* Set the over sampling, odr and filter settings*/
		rslt = set_odr_filter_settings(desired_settings);
	}
	if (are_settings_changed(INT_CTRL, desired_settings) && (!rslt))
	{
		/* Set the interrupt control settings */
		rslt = set_int_ctrl_settings(desired_settings);
	}
	if (are_settings_changed(ADV_SETT, desired_settings) && (!rslt))
	{
		/* Set the advance settings */
		rslt = set_advance_settings(desired_settings);
	}

	return rslt;
}

void BMP388::fill_osr_data(uint32_t _settings, uint8_t *addr, uint8_t *reg_data, uint8_t *len)
{
	struct bmp3_odr_filter_settings osr_settings = settings.odr_filter;

	if (_settings & (BMP3_PRESS_OS_SEL | BMP3_TEMP_OS_SEL))
	{
		/* Pressure over sampling settings check */
		if (_settings & BMP3_PRESS_OS_SEL)
		{
			/* Set the pressure over sampling settings in the
			  register variable */
			reg_data[*len] = BMP3_SET_BITS_POS_0(reg_data[0], BMP3_PRESS_OS, osr_settings.press_os);
		}
		/* Temperature over sampling settings check */
		if (_settings & BMP3_TEMP_OS_SEL)
		{
			/* Set the temperature over sampling settings in the
			   register variable */
			reg_data[*len] = BMP3_SET_BITS(reg_data[0], BMP3_TEMP_OS, osr_settings.temp_os);
		}
		/* 0x1C is the register address of over sampling register */
		addr[*len] = BMP3_OSR_ADDR;
		(*len)++;
	}
}

/*!
 * @brief This internal API fills the register address and register data of
 * the odr settings for burst write operation.
 */
void BMP388::fill_odr_data(uint8_t *addr, uint8_t *reg_data, uint8_t *len)
{
	struct bmp3_odr_filter_settings *osr_settings = &settings.odr_filter;

	/* Limit the ODR to 0.001525879 Hz*/
	if (osr_settings->odr > BMP3_ODR_0_001_HZ)
		osr_settings->odr = BMP3_ODR_0_001_HZ;
	/* Set the odr settings in the register variable */
	reg_data[*len] = BMP3_SET_BITS_POS_0(reg_data[1], BMP3_ODR, osr_settings->odr);
	/* 0x1D is the register address of output data rate register */
	addr[*len] = 0x1D;
	(*len)++;
}

/*!
 * @brief This internal API fills the register address and register data of
 * the filter settings for burst write operation.
 */
void BMP388::fill_filter_data(uint8_t *addr, uint8_t *reg_data, uint8_t *len)
{
	struct bmp3_odr_filter_settings osr_settings = settings.odr_filter;

	/* Set the iir settings in the register variable */
	reg_data[*len] = BMP3_SET_BITS(reg_data[3], BMP3_IIR_FILTER, osr_settings.iir_filter);
	/* 0x1F is the register address of iir filter register */
	addr[*len] = 0x1F;
	(*len)++;
}

int8_t BMP388::set_odr_filter_settings(uint32_t desired_settings)
{
	int8_t rslt;
	/* No of registers to be configured is 3*/
	uint8_t reg_addr[3] = {0};
	/* No of register data to be read is 4 */
	uint8_t reg_data[4];
	uint8_t len = 0;

	rslt = get_regs(BMP3_OSR_ADDR, reg_data, 4);

	if (rslt == BMP3_OK)
	{
		if (are_settings_changed((BMP3_PRESS_OS_SEL | BMP3_TEMP_OS_SEL), desired_settings))
		{
			/* Fill the over sampling register address and
			register data to be written in the sensor */
			fill_osr_data(desired_settings, reg_addr, reg_data, &len);
		}
		if (are_settings_changed(BMP3_ODR_SEL, desired_settings))
		{
			/* Fill the output data rate register address and
			register data to be written in the sensor */
			fill_odr_data(reg_addr, reg_data, &len);
		}
		if (are_settings_changed(BMP3_IIR_FILTER_SEL, desired_settings))
		{
			/* Fill the iir filter register address and
			register data to be written in the sensor */
			fill_filter_data(reg_addr, reg_data, &len);
		}
		if (settings.op_mode == BMP3_NORMAL_MODE)
		{
			/* For normal mode, osr and odr settings should
			   be proper */
			rslt = validate_osr_and_odr_settings();
		}
		if (rslt == BMP3_OK)
		{
			/* Burst write the over sampling, odr and filter
			   settings in the register */
			rslt = set_regs(reg_addr, reg_data, len);
		}
	}

	return rslt;
}

int8_t BMP388::set_int_ctrl_settings(uint32_t desired_settings)
{
	int8_t rslt;
	uint8_t reg_data;
	uint8_t reg_addr;
	struct bmp3_int_ctrl_settings int_settings;

	reg_addr = BMP3_INT_CTRL_ADDR;
	rslt = get_regs(reg_addr, &reg_data, 1);

	if (rslt == BMP3_OK)
	{
		int_settings = settings.int_settings;

		if (desired_settings & BMP3_OUTPUT_MODE_SEL)
		{
			/* Set the interrupt output mode bits */
			reg_data = BMP3_SET_BITS_POS_0(reg_data, BMP3_INT_OUTPUT_MODE, int_settings.output_mode);
		}
		if (desired_settings & BMP3_LEVEL_SEL)
		{
			/* Set the interrupt level bits */
			reg_data = BMP3_SET_BITS(reg_data, BMP3_INT_LEVEL, int_settings.level);
		}
		if (desired_settings & BMP3_LATCH_SEL)
		{
			/* Set the interrupt latch bits */
			reg_data = BMP3_SET_BITS(reg_data, BMP3_INT_LATCH, int_settings.latch);
		}
		if (desired_settings & BMP3_DRDY_EN_SEL)
		{
			/* Set the interrupt data ready bits */
			reg_data = BMP3_SET_BITS(reg_data, BMP3_INT_DRDY_EN, int_settings.drdy_en);
		}

		rslt = set_regs(&reg_addr, &reg_data, 1);
	}

	return rslt;
}

int8_t BMP388::set_advance_settings(uint32_t desired_settings)
{
	int8_t rslt;
	uint8_t reg_addr;
	uint8_t reg_data;
	struct bmp3_adv_settings adv_settings = settings.adv_settings;

	reg_addr = BMP3_IF_CONF_ADDR;
	rslt = get_regs(reg_addr, &reg_data, 1);

	if (rslt == BMP3_OK)
	{
		if (desired_settings & BMP3_I2C_WDT_EN_SEL)
		{
			/* Set the i2c watch dog enable bits */
			reg_data = BMP3_SET_BITS(reg_data, BMP3_I2C_WDT_EN, adv_settings.i2c_wdt_en);
		}
		if (desired_settings & BMP3_I2C_WDT_SEL_SEL)
		{
			/* Set the i2c watch dog select bits */
			reg_data = BMP3_SET_BITS(reg_data, BMP3_I2C_WDT_SEL, adv_settings.i2c_wdt_sel);
		}

		rslt = set_regs(&reg_addr, &reg_data, 1);
	}

	return rslt;
}

int8_t BMP388::set_pwr_ctrl_settings(uint32_t desired_settings)
{
	int8_t rslt;
	uint8_t reg_addr = BMP3_PWR_CTRL_ADDR;
	uint8_t reg_data;

	rslt = get_regs(reg_addr, &reg_data, 1);

	if (rslt == BMP3_OK)
	{
		if (desired_settings & BMP3_PRESS_EN_SEL)
		{
			/* Set the pressure enable settings in the
			register variable */
			reg_data = BMP3_SET_BITS_POS_0(reg_data, BMP3_PRESS_EN, settings.press_en);
		}
		if (desired_settings & BMP3_TEMP_EN_SEL)
		{
			/* Set the temperature enable settings in the
			register variable */
			reg_data = BMP3_SET_BITS(reg_data, BMP3_TEMP_EN, settings.temp_en);
		}
		/* Write the power control settings in the register */
		rslt = set_regs(&reg_addr, &reg_data, 1);
	}

	return rslt;
}

uint8_t BMP388::are_settings_changed(uint32_t sub_settings, uint32_t desired_settings)
{
	uint8_t settings_changed = FALSE;

	if (sub_settings & desired_settings)
	{
		/* User wants to modify this particular settings */
		settings_changed = TRUE;
	}
	else
	{
		/* User don't want to modify this particular settings */
		settings_changed = FALSE;
	}

	return settings_changed;
}

int8_t BMP388::get_sensor_settings()
{
	int8_t rslt;
	uint8_t settings_data[BMP3_GEN_SETT_LEN];

	rslt = get_regs(BMP3_INT_CTRL_ADDR, settings_data, BMP3_GEN_SETT_LEN);
	if (rslt == BMP3_OK)
	{
		/* Parse the settings data */
		parse_sett_data(settings_data);
	}

	return rslt;
}

void BMP388::parse_sett_data(const uint8_t *reg_data)
{
	/* Parse interrupt control settings and store in device structure */
	parse_int_ctrl_settings(&reg_data[0], &settings.int_settings);
	/* Parse advance settings and store in device structure */
	parse_advance_settings(&reg_data[1], &settings.adv_settings);
	/* Parse power control settings and store in device structure */
	parse_pwr_ctrl_settings(&reg_data[2], &settings);
	/* Parse odr and filter settings and store in device structure */
	parse_odr_filter_settings(&reg_data[3], &settings.odr_filter);
}

void BMP388::parse_int_ctrl_settings(const uint8_t *reg_data, struct bmp3_int_ctrl_settings *settings)
{
	settings->output_mode = BMP3_GET_BITS_POS_0(*reg_data, BMP3_INT_OUTPUT_MODE);
	settings->level = BMP3_GET_BITS(*reg_data, BMP3_INT_LEVEL);
	settings->latch = BMP3_GET_BITS(*reg_data, BMP3_INT_LATCH);
	settings->drdy_en = BMP3_GET_BITS(*reg_data, BMP3_INT_DRDY_EN);
}

void BMP388::parse_advance_settings(const uint8_t *reg_data, struct bmp3_adv_settings *settings)
{
	settings->i2c_wdt_en = BMP3_GET_BITS(*reg_data, BMP3_I2C_WDT_EN);
	settings->i2c_wdt_sel = BMP3_GET_BITS(*reg_data, BMP3_I2C_WDT_SEL);
}

/*!
 * @brief This internal API parse the power control(power mode, pressure enable
 * and temperature enable) settings and store in the device structure.
 */
void BMP388::parse_pwr_ctrl_settings(const uint8_t *reg_data, struct bmp3_settings *settings)
{
	settings->op_mode = BMP3_GET_BITS(*reg_data, BMP3_OP_MODE);
	settings->press_en = BMP3_GET_BITS_POS_0(*reg_data, BMP3_PRESS_EN);
	settings->temp_en = BMP3_GET_BITS(*reg_data, BMP3_TEMP_EN);
}

void BMP388::interleave_reg_addr(const uint8_t *reg_addr, uint8_t *temp_buff, const uint8_t *reg_data, uint8_t len)
{
	uint8_t index;

	for (index = 1; index < len; index++)
	{
		temp_buff[(index * 2) - 1] = reg_addr[index];
		temp_buff[index * 2] = reg_data[index];
	}
}

double BMP388::compensate_temperature(const struct bmp3_uncomp_data *uncomp_data,
									  struct bmp3_calib_data *calib_data)
{
	uint32_t uncomp_temp = uncomp_data->temperature;
	double partial_data1;
	double partial_data2;

	partial_data1 = (double)(uncomp_temp - calib_data->quantized_calib_data.par_t1);
	partial_data2 = (double)(partial_data1 * calib_data->quantized_calib_data.par_t2);
	/* Update the compensated temperature in calib structure since this is
	   needed for pressure calculation */
	calib_data->quantized_calib_data.t_lin = partial_data2 + (partial_data1 * partial_data1) * calib_data->quantized_calib_data.par_t3;

	/* Returns compensated temperature */
	return calib_data->quantized_calib_data.t_lin;
}

/*!
 * @brief This internal API is used to compensate the raw pressure data and
 * return the compensated pressure data in double data type.
 */
double BMP388::compensate_pressure(const struct bmp3_uncomp_data *uncomp_data,
								   const struct bmp3_calib_data *calib_data)
{
	const struct bmp3_quantized_calib_data *quantized_calib_data = &calib_data->quantized_calib_data;
	/* Variable to store the compensated pressure */
	double comp_press;
	/* Temporary variables used for compensation */
	double partial_data1;
	double partial_data2;
	double partial_data3;
	double partial_data4;
	double partial_out1;
	double partial_out2;

	partial_data1 = quantized_calib_data->par_p6 * quantized_calib_data->t_lin;
	partial_data2 = quantized_calib_data->par_p7 * bmp3_pow(quantized_calib_data->t_lin, 2);
	partial_data3 = quantized_calib_data->par_p8 * bmp3_pow(quantized_calib_data->t_lin, 3);
	partial_out1 = quantized_calib_data->par_p5 + partial_data1 + partial_data2 + partial_data3;

	partial_data1 = quantized_calib_data->par_p2 * quantized_calib_data->t_lin;
	partial_data2 = quantized_calib_data->par_p3 * bmp3_pow(quantized_calib_data->t_lin, 2);
	partial_data3 = quantized_calib_data->par_p4 * bmp3_pow(quantized_calib_data->t_lin, 3);
	partial_out2 = uncomp_data->pressure *
				   (quantized_calib_data->par_p1 + partial_data1 + partial_data2 + partial_data3);

	partial_data1 = bmp3_pow((double)uncomp_data->pressure, 2);
	partial_data2 = quantized_calib_data->par_p9 + quantized_calib_data->par_p10 * quantized_calib_data->t_lin;
	partial_data3 = partial_data1 * partial_data2;
	partial_data4 = partial_data3 + bmp3_pow((double)uncomp_data->pressure, 3) * quantized_calib_data->par_p11;
	comp_press = partial_out1 + partial_out2 + partial_data4;

	return comp_press;
}

/*!
 * @brief This internal API is used to calculate the power functionality for
 * double precision floating point values.
 */
double BMP388::bmp3_pow(double base, uint8_t power)
{
	double pow_output = 1;

	while (power != 0)
	{
		pow_output = base * pow_output;
		power--;
	}

	return pow_output;
}

void BMP388::parse_calib_data(const uint8_t *reg_data)
{
	/* Temporary variable to store the aligned trim data */
	struct bmp3_reg_calib_data *reg_calib_data = &calib_data.reg_calib_data;
	struct bmp3_quantized_calib_data *quantized_calib_data = &calib_data.quantized_calib_data;
	/* Temporary variable */
	double temp_var;

	/* 1 / 2^8 */
	temp_var = 0.00390625f;
	reg_calib_data->par_t1 = BMP3_CONCAT_BYTES(reg_data[1], reg_data[0]);
	quantized_calib_data->par_t1 = ((double)reg_calib_data->par_t1 / temp_var);

	reg_calib_data->par_t2 = BMP3_CONCAT_BYTES(reg_data[3], reg_data[2]);
	temp_var = 1073741824.0f;
	quantized_calib_data->par_t2 = ((double)reg_calib_data->par_t2 / temp_var);

	reg_calib_data->par_t3 = (int8_t)reg_data[4];
	temp_var = 281474976710656.0f;
	quantized_calib_data->par_t3 = ((double)reg_calib_data->par_t3 / temp_var);

	reg_calib_data->par_p1 = (int16_t)BMP3_CONCAT_BYTES(reg_data[6], reg_data[5]);
	temp_var = 1048576.0f;
	quantized_calib_data->par_p1 = ((double)(reg_calib_data->par_p1 - (16384)) / temp_var);

	reg_calib_data->par_p2 = (int16_t)BMP3_CONCAT_BYTES(reg_data[8], reg_data[7]);
	temp_var = 536870912.0f;
	quantized_calib_data->par_p2 = ((double)(reg_calib_data->par_p2 - (16384)) / temp_var);

	reg_calib_data->par_p3 = (int8_t)reg_data[9];
	temp_var = 4294967296.0f;
	quantized_calib_data->par_p3 = ((double)reg_calib_data->par_p3 / temp_var);

	reg_calib_data->par_p4 = (int8_t)reg_data[10];
	temp_var = 137438953472.0f;
	quantized_calib_data->par_p4 = ((double)reg_calib_data->par_p4 / temp_var);

	reg_calib_data->par_p5 = BMP3_CONCAT_BYTES(reg_data[12], reg_data[11]);
	/* 1 / 2^3 */
	temp_var = 0.125f;
	quantized_calib_data->par_p5 = ((double)reg_calib_data->par_p5 / temp_var);

	reg_calib_data->par_p6 = BMP3_CONCAT_BYTES(reg_data[14], reg_data[13]);
	temp_var = 64.0f;
	quantized_calib_data->par_p6 = ((double)reg_calib_data->par_p6 / temp_var);

	reg_calib_data->par_p7 = (int8_t)reg_data[15];
	temp_var = 256.0f;
	quantized_calib_data->par_p7 = ((double)reg_calib_data->par_p7 / temp_var);

	reg_calib_data->par_p8 = (int8_t)reg_data[16];
	temp_var = 32768.0f;
	quantized_calib_data->par_p8 = ((double)reg_calib_data->par_p8 / temp_var);

	reg_calib_data->par_p9 = (int16_t)BMP3_CONCAT_BYTES(reg_data[18], reg_data[17]);
	temp_var = 281474976710656.0f;
	quantized_calib_data->par_p9 = ((double)reg_calib_data->par_p9 / temp_var);

	reg_calib_data->par_p10 = (int8_t)reg_data[19];
	temp_var = 281474976710656.0f;
	quantized_calib_data->par_p10 = ((double)reg_calib_data->par_p10 / temp_var);

	reg_calib_data->par_p11 = (int8_t)reg_data[20];
	temp_var = 36893488147419103232.0f;
	quantized_calib_data->par_p11 = ((double)reg_calib_data->par_p11 / temp_var);
}