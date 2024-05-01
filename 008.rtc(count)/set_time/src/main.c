#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/drivers/counter.h>
#include <autoconf.h>
#include <stdio.h>
#include <zephyr/sys/printk.h>

#define TIMER_NODE DT_NODELABEL(rtc2)

uint32_t ticks;
uint32_t seconds;
uint32_t hours = 14;
uint32_t minutes = 01;
uint32_t remaining_seconds;

const struct device *counter_dev = DEVICE_DT_GET(TIMER_NODE);

void print_date_time(void)
{
        counter_get_value(counter_dev, &ticks);
        seconds = ticks / 32768;

        // Example: Print the time in HH:MM:SS format
        remaining_seconds = seconds % 60;
        if (remaining_seconds == 0)
        {
                minutes++;
                seconds = 0;
        }
        if (minutes > 59)
        {
                hours++;
                minutes = 0;
        }

        if (hours >= 13)
        {
                hours = 1;
        }
        printk("Current time: %02u:%02u:%02u\n", hours, minutes, remaining_seconds);
}

int main(void)
{
        if (!device_is_ready(counter_dev) || counter_start(counter_dev) != 0)
        {
                printk("Failed to initialize counter device\n");
                return 0;
        }

        // Main loop to continuously print the date and time
        while (1)
        {
                print_date_time();
                k_sleep(K_SECONDS(1)); // Sleep for 1 second
        }
        return 0;
}
