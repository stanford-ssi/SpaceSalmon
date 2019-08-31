#pragma once

extern "C"
{
#include "bmp3_defs.h"
#include <hal_spi_m_os.h>
#include <hal_gpio.h>
#include <math.h>
#include <string.h>
#include <hal_delay.h>
}


class BMP388
{
  public:

    typedef bmp3_data Data;

    BMP388(struct spi_m_os_descriptor *spi, int8_t cspin);

    bool begin();
    bool setTemperatureOversampling(uint8_t os);
    bool setPressureOversampling(uint8_t os);
    bool setIIRFilterCoeff(uint8_t fs);
    bool setOutputDataRate(uint8_t odr);

    
    Data readSensor(void); // Perform a reading

    void write();

  private:
    int8_t _cs;
    struct spi_m_os_descriptor *_spi;


    /*! Chip Id */
	  uint8_t chip_id;
	  /*! Device Id */
  	uint8_t dev_id;

    struct bmp3_calib_data calib_data;
	/*! Sensor Settings */
    struct bmp3_settings settings;
    /*! Sensor and interrupt status flags */
    struct bmp3_status status;
    /*! FIFO data and settings structure */
    struct bmp3_fifo *fifo;

    bool _filterEnabled, _tempOSEnabled, _presOSEnabled, _ODREnabled;

    unsigned long _meas_end;

    int8_t spi_read(uint8_t reg_addr, uint8_t *reg_data, uint16_t len);
    int8_t spi_write(uint8_t reg_addr, uint8_t *reg_data, uint16_t len);

    int8_t get_regs(uint8_t reg_addr, uint8_t *reg_data, uint16_t len);
    int8_t set_regs(uint8_t *reg_addr, const uint8_t *reg_data, uint8_t len);

    int8_t soft_reset();

    int8_t set_op_mode();
    int8_t get_op_mode(uint8_t *op_mode);

    int8_t get_calib_data();
    
    int8_t set_sensor_settings(uint32_t desired_settings);

    int8_t get_sensor_data(uint8_t sensor_comp, struct bmp3_data *comp_data);

    void parse_sensor_data(const uint8_t *reg_data, struct bmp3_uncomp_data *uncomp_data);

    int8_t compensate_data(uint8_t sensor_comp, const struct bmp3_uncomp_data *uncomp_data, struct bmp3_data *comp_data, struct bmp3_calib_data *calib_data);

    void interleave_reg_addr(const uint8_t *reg_addr, uint8_t *temp_buff, const uint8_t *reg_data, uint8_t len);

    int8_t put_device_to_sleep();
    int8_t set_normal_mode();

    int8_t validate_normal_mode_settings();
    int8_t validate_osr_and_odr_settings();

    int8_t write_power_mode();
    int8_t get_odr_filter_settings();

    void parse_odr_filter_settings(const uint8_t *reg_data, struct bmp3_odr_filter_settings *settings);

    int8_t get_sensor_settings();

    void parse_sett_data(const uint8_t *reg_data);

    uint16_t calculate_press_meas_time();

    uint16_t calculate_temp_meas_time();

    int8_t verify_meas_time_and_odr_duration(uint16_t meas_t, uint32_t odr_duration);

    double bmp3_pow(double base, uint8_t power);
    double compensate_pressure(const struct bmp3_uncomp_data *uncomp_data,
					const struct bmp3_calib_data *calib_data);
    double compensate_temperature(const struct bmp3_uncomp_data *uncomp_data,
						struct bmp3_calib_data *calib_data);

    uint8_t are_settings_changed(uint32_t sub_settings, uint32_t desired_settings);

    void parse_calib_data(const uint8_t *reg_data);

    int8_t set_pwr_ctrl_settings(uint32_t desired_settings);
    int8_t set_odr_filter_settings(uint32_t desired_settings);
    int8_t set_int_ctrl_settings(uint32_t desired_settings);

    int8_t set_advance_settings(uint32_t desired_settings);

    void fill_filter_data(uint8_t *addr, uint8_t *reg_data, uint8_t *len);
    void fill_osr_data(uint32_t settings, uint8_t *addr, uint8_t *reg_data, uint8_t *len);
    void fill_odr_data(uint8_t *addr, uint8_t *reg_data, uint8_t *len);

    void parse_pwr_ctrl_settings(const uint8_t *reg_data, struct bmp3_settings *settings);
    void parse_advance_settings(const uint8_t *reg_data, struct bmp3_adv_settings *settings);
    void parse_int_ctrl_settings(const uint8_t *reg_data, struct bmp3_int_ctrl_settings *settings);
};
