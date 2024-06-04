#include <zephyr/kernel.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/device.h>
#include <zephyr/devicetree.h>
#include <zephyr/sys/printk.h>

// Define the stack size and scheduling priority of the two threads
#define STACKSIZE 1024
#define THREAD0_PRIORITY 7
#define THREAD1_PRIORITY 7

static const struct gpio_dt_spec led0 = GPIO_DT_SPEC_GET(DT_NODELABEL(led0), gpios);
static const struct gpio_dt_spec led1 = GPIO_DT_SPEC_GET(DT_NODELABEL(led1), gpios);

volatile bool led0_state = false;
volatile bool led1_state = true;
// thread entry functions for the two threads (thread0 and thread1)
void thread0(void)
{
        while (1)
        {
                /*better option can be for the threads to sleep rather than yield. When sleeping,
                threads are put in the “Non-runnable” state and do very little processing*/
                k_msleep(2000);
                printk("led0!\n");
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

                /* thread normally yields when it either has nothing else to do or wants to give other equal
                or higher priority threads a chance to run*/
                k_yield();
        }
}

void thread1(void)
{
        while (1)
        {
                k_msleep(2000);
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

                k_yield();
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