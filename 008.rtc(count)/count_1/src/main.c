#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/drivers/counter.h>
#include <autoconf.h>
#include <zephyr/sys/printk.h>

#define TICKS_TO_SECONDS(ticks) ((ticks) / CONFIG_SYS_CLOCK_TICKS_PER_SEC)
#define SECONDS_TO_TICKS(seconds) ((seconds) * CONFIG_SYS_CLOCK_TICKS_PER_SEC)

#define TIMER_NODE DT_NODELABEL(rtc2)

void print_date_time(void)
{
        const struct device *counter_dev = DEVICE_DT_GET(TIMER_NODE);
        if (!device_is_ready(counter_dev))
        {
                printk("Counter device not found\n");
                return;
        }

        uint32_t ticks;
        counter_get_value(counter_dev, &ticks);
        uint32_t seconds = TICKS_TO_SECONDS(ticks);

        // Example: Print the time in HH:MM:SS format
        uint32_t hours = seconds / 3600;
        uint32_t minutes = (seconds / 60) % 60;
        uint32_t remaining_seconds = seconds % 60;
        printk("Current time: %02u:%02u:%02u\n", hours, minutes, remaining_seconds);
}

int main(void)
{
        // Initialize the counter device
        const struct device *counter_dev = DEVICE_DT_GET(TIMER_NODE);
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
