#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/drivers/sensor.h>
#include <zephyr/logging/log.h>
#include <zephyr/sys/util.h>

LOG_MODULE_REGISTER(lps22hb_prac_1, LOG_LEVEL_DBG);

static unsigned int obs;
struct sensor_value pressure, temp;

static void process_sample(const struct device *dev)
{

	if (sensor_sample_fetch(dev) < 0)
	{
		LOG_ERR("Sensor sample update error\n");
		return;
	}

	if (sensor_channel_get(dev, SENSOR_CHAN_PRESS, &pressure) < 0)
	{
		LOG_ERR("Cannot read LPS22HB pressure channel\n");
		return;
	}

	if (sensor_channel_get(dev, SENSOR_CHAN_AMBIENT_TEMP, &temp) < 0)
	{
		LOG_ERR("Cannot read LPS22HB temperature channel\n");
		return;
	}

	++obs;
	LOG_INF("Observation:%u\n", obs);

	/* display pressure */
	LOG_INF("Pressure:%.1f kPa\n", sensor_value_to_double(&pressure));

	/* display temperature */
	LOG_INF("Temperature:%.1f C\n", sensor_value_to_double(&temp));

	LOG_INF("=========================\n\r");
}

int main(void)
{
	const struct device *const dev = DEVICE_DT_GET_ONE(st_lps22hb_press);

	if (!device_is_ready(dev))
	{
		LOG_INF("Device %s is not ready\n", dev->name);
		return 0;
	}

	while (true)
	{
		process_sample(dev);
		k_sleep(K_MSEC(2000));
	}
	return 0;
}