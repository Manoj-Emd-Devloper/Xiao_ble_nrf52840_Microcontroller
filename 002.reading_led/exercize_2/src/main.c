#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/devicetree.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/sys/printk.h>

#define SLEEP_TIME_MS 10 * 60 * 1000
#define SW0_NODE DT_ALIAS(sw0)
#define LED0_NODE DT_ALIAS(led0)

static const struct gpio_dt_spec button = GPIO_DT_SPEC_GET(SW0_NODE, gpios);
static const struct gpio_dt_spec led = GPIO_DT_SPEC_GET(LED0_NODE, gpios);
static struct gpio_callback button_cb_data;

void button_pressed(const struct device *dev, struct gpio_callback *cb, uint32_t pins)
{       
        printk("interrupt\n\r");
        gpio_pin_toggle_dt(&led);
}


int main(void)
{
        int ret;

        if (!device_is_ready(led.port))
        {
                return -1;
        }

        if (!device_is_ready(button.port))
        {
                return -1;
        }

        ret = gpio_pin_configure_dt(&led, GPIO_OUTPUT_ACTIVE);
        if (ret < 0)
        {
                return -1;
        }
        
        ret = gpio_pin_interrupt_configure_dt(&button, GPIO_INT_EDGE_TO_ACTIVE );
        if (ret < 0)
        {
                return -1;
        }
        
        gpio_init_callback(&button_cb_data,button_pressed,BIT(button.pin));

        gpio_add_callback(button.port, &button_cb_data);

        while(1){
                k_msleep(SLEEP_TIME_MS);
        }
}
