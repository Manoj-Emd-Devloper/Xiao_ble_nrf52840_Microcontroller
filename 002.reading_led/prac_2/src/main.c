#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/devicetree.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/sys/printk.h>

#define SW0_NODE DT_ALIAS(sw0)
#define LED0_NODE DT_ALIAS(led0)
#define DEBOUNCE_DURATION 50 // milliseconds

static struct gpio_callback button_cb_data;
static const struct gpio_dt_spec button = GPIO_DT_SPEC_GET(SW0_NODE, gpios);
static const struct gpio_dt_spec led = GPIO_DT_SPEC_GET(LED0_NODE, gpios);

static bool led_is_on = false;
uint32_t last_debounce_time = 0;

static void button_pressed(const struct device *dev, struct gpio_callback *cb, uint32_t pins)
{   
    printk("interrupt");
    uint32_t current_time = k_uptime_get_32();
    if ((current_time-last_debounce_time) > DEBOUNCE_DURATION)
    {
        led_is_on = !led_is_on; // Toggle the LED state
        gpio_pin_set_dt(&led, led_is_on); // Set the new LED state
        last_debounce_time = current_time;
    }
}

void main(void)
{
    gpio_pin_configure_dt(&led, GPIO_OUTPUT_INACTIVE);
    gpio_pin_configure_dt(&button, GPIO_INPUT);

    gpio_pin_interrupt_configure_dt(&button, GPIO_INT_EDGE_TO_ACTIVE);

    gpio_init_callback(&button_cb_data, button_pressed, BIT(button.pin));
    gpio_add_callback(button.port, &button_cb_data);

    while (1)
    {
    }
}
