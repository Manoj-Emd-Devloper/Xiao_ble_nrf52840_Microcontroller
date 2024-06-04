#include <zephyr/kernel.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/device.h>
#include <zephyr/devicetree.h>
#include <zephyr/sys/printk.h>

// Define the stack size and scheduling priority of the two threads
#define STACKSIZE 1024
#define THREAD0_PRIORITY 7
#define THREAD1_PRIORITY 6 //for low priority level thread priory must be high

static const struct gpio_dt_spec led0 = GPIO_DT_SPEC_GET(DT_NODELABEL(led0), gpios);
static const struct gpio_dt_spec led1 = GPIO_DT_SPEC_GET(DT_NODELABEL(led1), gpios);

volatile bool led0_state = false;
volatile bool led1_state = true;
// thread entry functions for the two threads (thread0 and thread1)
void thread0(void)
{
        while (1)
        {
                printk("===========\nled0!\n");
                if (led0_state == 0)
                {
                        gpio_pin_set(led0.port, led0.pin, led0_state);
                        led0_state = 1;
                }
                else if (led0_state == 1)
                {
                        gpio_pin_set(led0.port, led0.pin, led0_state);
                        led0_state = 0;
                }

                /*function call causes the current thread to execute a do-nothing loop for a specified time in microseconds.*/
                k_busy_wait(1000000);
        }
}

void thread1(void)
{
        while (1)
        {
                printk("led1!\n");
                if (led1_state == 1)
                {
                        gpio_pin_set(led1.port, led1.pin, led1_state);
                        led1_state = 0;
                }
                else if (led1_state == 0)
                {
                        gpio_pin_set(led1.port, led1.pin, led1_state);
                        led1_state = 1;
                }

                /*function call causes the current thread to execute a do-nothing loop for a specified time in microseconds.*/
                k_busy_wait(1000000);
        }
}

// we can define the two threads, thread0 and thread1, using K_THREAD_DEFINE()
K_THREAD_DEFINE(thread0_id, STACKSIZE, thread0, NULL, NULL, NULL,
                THREAD0_PRIORITY, 0, 0);
K_THREAD_DEFINE(thread1_id, STACKSIZE, thread1, NULL, NULL, NULL,
                THREAD1_PRIORITY, 0, 0);

void main(void)
{
        int ret;
        if (!device_is_ready(led0.port))
        {
                return;
        }
        if (!device_is_ready(led1.port))
        {
                return;
        }
        ret = gpio_pin_configure_dt(&led0, GPIO_OUTPUT_ACTIVE);
        if (ret < 0)
        {
                return;
        }
        ret = gpio_pin_configure_dt(&led1, GPIO_OUTPUT_ACTIVE);
        if (ret < 0)
        {
                return;
        }
}