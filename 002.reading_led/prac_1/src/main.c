#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/devicetree.h>
#include <zephyr/drivers/gpio.h>

#define SLEEP_TIME_MS 100
#define SW0_NODE DT_ALIAS(sw0)
#define LED0_NODE DT_ALIAS(led0)

static const struct gpio_dt_spec button = GPIO_DT_SPEC_GET(SW0_NODE,gpios);
static const struct gpio_dt_spec led = GPIO_DT_SPEC_GET(LED0_NODE, gpios);

int main(void)
{
        int ret;

        if (!device_is_ready(button.port))
        {
                return -1;
        }

        if (!device_is_ready(led.port))
        {
                return -1;
        }

        ret = gpio_pin_configure_dt(&button, GPIO_INPUT);
        if (ret < 0)
        {
                return -1;
        }

        ret = gpio_pin_configure_dt(&led, GPIO_OUTPUT_ACTIVE);
        if (ret < 0)
        {
                return -1;
        }


        while(1){
                bool val = gpio_pin_get_dt(&button);

                gpio_pin_set_dt(&led,val);

                k_msleep(SLEEP_TIME_MS);
        }
}
