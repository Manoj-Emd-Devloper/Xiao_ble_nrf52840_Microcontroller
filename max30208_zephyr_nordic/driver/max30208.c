#include <zephyr/kernel.h>
#include <zephyr/drivers/i2c.h>
#include <zephyr/device.h>
#include <zephyr/devicetree.h>
#include <zephyr/logging/log.h>
#include "max30208.h"

LOG_MODULE_REGISTER(max30208);

static uint16_t skin_temperature;

uint16_t Get_Skin_Temperature(void)
{
	return skin_temperature;
}

uint8_t read_register(const struct i2c_dt_spec *spec, uint8_t reg, uint8_t *val, uint8_t rd_len)
{
	int ret = i2c_write_read_dt(spec, &reg, 1, val, rd_len);
	if (ret < 0)
	{
		LOG_ERR("Failed to read register 0x%02x", reg);
		return -1;
	}
	return 0;
}

uint8_t write_register(const struct i2c_dt_spec *spec, uint8_t *reg, uint8_t wd_len)
{
	int ret = i2c_write_dt(spec, reg, wd_len);
	if (ret < 0)
	{
		LOG_ERR("Failed to write register 0x%02x", reg[0]);
		return -1;
	}
	return 0;
}

int max30208_setmode(const struct i2c_dt_spec *spec)
{
	uint8_t reg[2] = {INTERRUPT_CONF, NORMAL_MODE};
	int ret = write_register(spec, reg, 2);
	if (ret < 0)
	{
		LOG_ERR("Failed to set mode");
		return -1;
	}
	return 0;
}

int read_part_id(const struct i2c_dt_spec *spec, uint8_t *data)
{
	int ret = read_register(spec, PART_ID_ADDR, data, 1);
	if (ret)
	{
		LOG_ERR("Failed to read part id");
		return -1;
	}
	k_msleep(50);
	return 0;
}

int32_t max30208_get_FIFOConfig1(const struct i2c_dt_spec *spec, uint8_t *value)
{
	int ret = read_register(spec, FIFO_CONFIGURATION1, value,1);
	if (ret)
	{
		LOG_ERR("Failed to get FIFOConfig1");
		return -1;
	}
	return 0;
}

int32_t max30208_set_FIFOConfig1(const struct i2c_dt_spec *spec, uint8_t config)
{
	uint8_t reg[2] = {FIFO_CONFIGURATION1, config};
	int ret = write_register(spec, reg,2);
	if (ret < 0)
	{
		LOG_ERR("Failed to set FIFOConfig1");
		return -1;
	}
	return 0;
}

int32_t max30208_set_FIFOConfig2(const struct i2c_dt_spec *spec, uint8_t config)
{
	uint8_t reg[2] = {FIFO_CONFIGURATION2, config};
	// if your use flush fifo the config should be 0x00
	int ret = write_register(spec, reg,2);
	if (ret < 0)
	{
		LOG_ERR("Failed to set FIFOConfig2");
		return -1;
	}
	return 0;
}

int max30208_init(const struct i2c_dt_spec *spec)
{
	int ret = reset_max30208(spec);
	if (ret < 0)
	{
		LOG_ERR("Reset error");
		return -1;
	}
	k_msleep(20);

	uint8_t reg_val = 0x1E, ret_val = 0;
	ret = max30208_get_FIFOConfig1(spec, &ret_val);
	if (ret != 0)
	{
		LOG_ERR("Failed to get FIFOConfig1");
		return -1;
	}

	ret = max30208_set_FIFOConfig1(spec, reg_val);
	if (ret != 0)
	{
		LOG_ERR("Failed to set FIFOConfig1");
		return -1;
	}
	ret = max30208_get_FIFOConfig1(spec, &ret_val);
	if (ret != 0)
	{
		LOG_ERR("Failed to get FIFOConfig1");
		return -1;
	}
	return 0;
}

int max30208_takeDataMeasurement(const struct i2c_dt_spec *spec)
{
	int ret;
	uint8_t temp_config = 0x00, ret_val;
	ret = read_register(spec, TEMP_SET, &temp_config,1);
	if (ret)
	{
		LOG_ERR("Failed to read TEMP_SET");
		return -1;
	}
	temp_config = 0xFF;
	uint8_t reg[2] = {TEMP_SET, temp_config};
	ret = write_register(spec, reg,2);
	if (ret < 0)
	{
		LOG_ERR("Failed to write TEMP_SET");
		return -1;
	}
	ret = read_register(spec, TEMP_SET, &ret_val,1);
	if (ret < 0)
	{
		LOG_ERR("Failed to read TEMP_SET");
		return -1;
	}
	return 0;
}

int8_t MAX30208_Trigger_Conversion(const struct i2c_dt_spec *spec)
{
	int ret;
	uint8_t val = 0x10;
	ret = max30208_set_FIFOConfig2(spec, val);
	if (ret != 0)
	{
		LOG_ERR("Failed to set FIFOConfig2");
		return -1;
	}
	ret = max30208_takeDataMeasurement(spec);
	if (ret != 0)
	{
		LOG_ERR("Failed to take data measurement");
		return -1;
	}
	return 0;
}

int max30208_get_FifoData(const struct i2c_dt_spec *spec, uint8_t value[2])
{
	uint8_t buf[1] = {FIFO_DATA};
	int ret = i2c_write_read_dt(spec, &buf[0], 1, value, 2);
	if (ret < 0)
	{
		LOG_ERR("Failed to read register 0x%02x", FIFO_DATA);
		return -1;
	}
	return 0;
}

int read_temp(const struct i2c_dt_spec *spec, float *Temp)
{
	int ret;
	uint8_t raw_data[2] = {0x00};
	uint16_t raw = 0x00;

	ret = max30208_get_FifoData(spec, raw_data);
	if (ret < 0)
	{
		LOG_ERR("Failed to get FIFOData");
		return -1;
	}
	raw = ((raw_data[0] << 8) + raw_data[1]);
	*Temp = max30208_toCelsius(raw);
	return 0;
}

int8_t read_status(const struct i2c_dt_spec *spec, uint8_t *data)
{
	int8_t ret;
	uint8_t val[1] = {0x00};
	ret = read_register(spec, STATUS_REG, val,1);
	if (ret < 0)
	{
		LOG_ERR("Device ID error");
		return -1;
	}
	*data = val[0];
	return 0;
}

int reset_max30208(const struct i2c_dt_spec *spec)
{
	uint8_t reg[2] = {MAX30208_SYSTEM_REG, 0x01};
	int ret = write_register(spec, reg,2);
	if (ret < 0)
	{
		LOG_ERR("Reset error");
		return -1;
	}
	return 0;
}

/**
 * @brief Convert Raw Sensor Data to degrees Celisus
 * @param rawTemp - 16 bit raw temperature data
 * @return Returns the converted Celsius Temperature
 */
float max30208_toCelsius(uint16_t rawTemp)
{
	float celsius;
	celsius = 0.005 * rawTemp;
	return celsius;
}

/**
 * @brief Convert Celsius Temperature to Fahrenheit
 * @param temperatureC - Temperature in degrees Celsius that will be converted
 * @return Returns the converted Fahrenheit temperature
 */
float max30208_toFahrenheit(float temperatureC)
{
	float temperatureF;
	temperatureF = (temperatureC * 1.8F) + 32.0f;
	return temperatureF;
}

int32_t max30208_get_Status(const struct i2c_dt_spec *spec, uint8_t *value)
{
	int32_t ret;

	ret = (read_register(spec, STATUS_REG, value,1));
	if (ret < 0)
	{
		LOG_INF("Failed to read register 0x%02x", STATUS_REG);
		return -1;
	}
	return 0;
}

int32_t max30208_get_Overflow(const struct i2c_dt_spec *spec, uint8_t *value)
{
	int32_t ret;

	ret = read_register(spec, FIFO_OVERFLOW_COUNTER, value,1);
	if (ret < 0)
	{
		LOG_INF("Failed to read register 0x%02x", FIFO_OVERFLOW_COUNTER);
		return -1;
	}
	return 0;
}

int32_t max30208_get_DataCounter(const struct i2c_dt_spec *spec, uint8_t *value)
{
	int32_t ret;

	ret = read_register(spec, FIFO_DATA_COUNTER, value,1);
	if (ret < 0)
	{
		LOG_INF("Failed to read register 0x%02x", FIFO_DATA_COUNTER);
		return -1;
	}
	return 0;
}

int32_t MAX30208_GetRawData(const struct i2c_dt_spec *spec, uint16_t *data)
{
	int32_t ret = 0;
	float Temp;
	uint16_t raw = 0x0000, *raw_ptr = NULL;
	uint8_t raw_data[2] = {0x00}, no_of_samples = 0, overflow = 0, status;

	raw_ptr = (uint16_t *)data;

	ret = max30208_get_Status(spec, &status);

	ret = max30208_get_Overflow(spec, &overflow);

	if (overflow == 0)
	{
		/* Number of samples available in the FIFO */
		ret = max30208_get_DataCounter(spec, &no_of_samples);
	}
	else
	{
		no_of_samples = 32;
	}

	LOG_DBG("MAX30208 FIFO COUNTER : %d ", no_of_samples);

#ifndef USE_DO_WHILE

	while (no_of_samples != 0)
	{
		/* Read FIFO. */
		ret = max30208_get_FifoData(spec, raw_data);

		if (ret == 0)
		{
			raw = ((raw_data[0] << 8) + raw_data[1]);
#ifdef MAX30208_DEBUG
			LOG_DBG("Skin_Raw \t:%X", raw);
#endif
			Temp = max30208_toCelsius(raw);
#ifdef MAX30208_DEBUG
			LOG_INF("Skin\t: %.2f c", Temp);
#endif
		}
		no_of_samples--;
	};

#else

	do
	{
		/* Read FIFO. */
		ret = max30208_get_FifoData(spec, raw_data);

		if (ret == 0)
		{
			raw = ((raw_data[0] << 8) + raw_data[1]);
#ifdef MAX30208_DEBUG
			LOG_INF("Skin_Raw \t:%X", raw);
#endif
			Temp = max30208_toCelsius(raw);
#ifdef MAX30208_DEBUG
			LOG_INF("Skin\t: %.2f c", Temp);

#endif
		}
		no_of_samples--;

	} while (no_of_samples != 0);

#endif

	*raw_ptr = raw;

	skin_temperature = raw >> 1;

	return 0;
}

// #define MAX30208_DEFINE(inst)                                                                      \
//                                                                                                    \
// 	static struct max30208_data max30208_data_##inst;                                          \
//                                                                                                    \
// 	static const struct max30208_config max30208_config_##inst = {                             \
// 		.spi = SPI_DT_SPEC_INST_GET(inst, SPI_MODE_CPHA | SPI_WORD_SET(8), 0),             \
// 		.resistance_at_zero = DT_INST_PROP(inst, resistance_at_zero),                      \
// 		.resistance_reference = DT_INST_PROP(inst, resistance_reference),                  \
// 		.conversion_mode = false,                                                          \
// 		.one_shot = true,                                                                  \
// 		.three_wire = DT_INST_PROP(inst, maxim_3_wire),                                    \
// 		.fault_cycle = MAX31865_FAULT_DETECTION_NONE,                                      \
// 		.filter_50hz = DT_INST_PROP(inst, filter_50hz),                                    \
// 		.low_threshold = DT_INST_PROP(inst, low_threshold),                                \
// 		.high_threshold = DT_INST_PROP(inst, high_threshold),                              \
// 	};                                                                                         \
//                                                                                                    \
// 	SENSOR_DEVICE_DT_INST_DEFINE(inst, max31865_init, NULL, &max31865_data_##inst,             \
// 			      &max31865_config_##inst, POST_KERNEL, CONFIG_SENSOR_INIT_PRIORITY,   \
// 			      &max31865_api_funcs);

// /* Create the struct device for every status "okay" node in the devicetree. */
// DT_INST_FOREACH_STATUS_OKAY(MAX30208_DEFINE)