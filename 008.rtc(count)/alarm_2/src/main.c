#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/drivers/counter.h>
#include <autoconf.h>
#include <stdio.h>
#include <zephyr/sys/printk.h>

#define TIMER_NODE DT_NODELABEL(rtc2)
#define DELAY 4000000
#define ALARM_CHANNEL_ID 0

uint32_t ticks;
uint32_t seconds;
uint32_t hours = 3;
uint32_t minutes = 19;
uint32_t remaining_seconds;

const struct device *counter_dev = DEVICE_DT_GET(TIMER_NODE);
struct counter_alarm_cfg alarm_cfg;

static void test_counter_interrupt_fn(const struct device *counter_dev,
                                      uint8_t chan_id, uint32_t ticks,
                                      void *user_data)
{
        struct counter_alarm_cfg *config = user_data;
        uint32_t now_ticks;
        uint64_t now_usec;
        int now_sec;
        int err;

        err = counter_get_value(counter_dev, &now_ticks);
        if (err)
        {
                printk("Failed to read counter value (err %d)", err);
                return;
        }

        now_usec = counter_ticks_to_us(counter_dev, now_ticks);//give microsecond
        now_sec = (int)(now_usec / USEC_PER_SEC);//convert microsecond in second

        printk("!!! Alarm !!!\n");
        // printk("Now: %u\n", now_sec);

        /* Set a new alarm with a double length duration */
        // config->ticks = config->ticks * 2U;

        printk("Set alarm for %u sec (%u ticks)\n",
               (uint32_t)(counter_ticks_to_us(counter_dev,
                                              config->ticks) /
                          USEC_PER_SEC),
               config->ticks);

        //alarm disable after one short so enable to it
        err = counter_set_channel_alarm(counter_dev, ALARM_CHANNEL_ID,
                                        user_data); 
        if (err != 0)
        {
                printk("Alarm could not be set\n");
        }
}

void print_date_time(void)
{
        counter_get_value(counter_dev, &ticks);
        seconds = ticks / 32768; // 32768 ticks per second

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
        int err;
        k_msleep(5000);
        if (!device_is_ready(counter_dev) || counter_start(counter_dev) != 0)
        {
                printk("Failed to initialize counter device\n");
                return 0;
        }

        alarm_cfg.flags = 0;
        alarm_cfg.ticks = counter_us_to_ticks(counter_dev, DELAY);
        alarm_cfg.callback = test_counter_interrupt_fn;
        alarm_cfg.user_data = &alarm_cfg;

        err = counter_set_channel_alarm(counter_dev, ALARM_CHANNEL_ID,
                                        &alarm_cfg);
        printk("Set alarm for %u sec (%u ticks)\n",
               (uint32_t)(counter_ticks_to_us(counter_dev,
                                              alarm_cfg.ticks) /
                          USEC_PER_SEC),
               alarm_cfg.ticks);

        if (-EINVAL == err)
        {
                printk("Alarm settings invalid\n");
        }
        else if (-ENOTSUP == err)
        {
                printk("Alarm setting request not supported\n");
        }
        else if (err != 0)
        {
                printk("Error\n");
        }
        // Main loop to continuously print the date and time
        while (1)
        {
                print_date_time();
                k_sleep(K_SECONDS(1)); // Sleep for 1 second
        }
        return 0;
}
