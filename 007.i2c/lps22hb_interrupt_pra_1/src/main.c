#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/drivers/sensor.h>
#include <zephyr/logging/log.h>
#include <zephyr/sys/util.h>

LOG_MODULE_REGISTER(lps22hb_prac_1, LOG_LEVEL_DBG);

const struct device *const dev = DEVICE_DT_GET_ONE(st_lps22hb_press);
static uint8_t obs;
struct sensor_value pressure, temp;

static void process_sample(void)
{

    ++obs;
    LOG_INF("Observation:%u\n", obs);

    /* display pressure */
    LOG_INF("Pressure:%.1f kPa\n", sensor_value_to_double(&pressure));

    /* display temperature */
    LOG_INF("Temperature:%.1f C\n"
            "=========================\n\r", sensor_value_to_double(&temp));
}

void timer_handler(struct k_timer *timer_id)
{
    process_sample();
}

K_TIMER_DEFINE(timer, timer_handler, NULL);

int main(void)
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

    k_timer_start(&timer, K_SECONDS(5), K_SECONDS(5));

    while (true)
    {
        k_sleep(K_FOREVER);
    }
    return 0;
}