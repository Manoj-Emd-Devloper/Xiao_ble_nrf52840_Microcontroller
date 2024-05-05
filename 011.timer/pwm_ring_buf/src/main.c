#include <zephyr/kernel.h>
#include <zephyr/sys/ring_buffer.h>
#include <zephyr/drivers/uart.h>
#include <zephyr/drivers/pwm.h>
#include <zephyr/logging/log.h>
#include <stdlib.h> // for atoi()
#include <string.h> // for sting function()

#define RING_BUF_SIZE 64
#define PWM_PERIOD_US 1000
#define PWM_LED0 DT_ALIAS(pwm_led0)

LOG_MODULE_REGISTER(pwm_ring_buf, LOG_LEVEL_DBG);

static const struct device *uart_dev = DEVICE_DT_GET(DT_NODELABEL(uart0));
static const struct pwm_dt_spec pwm_dev = PWM_DT_SPEC_GET(PWM_LED0);
static uint8_t buffer[RING_BUF_SIZE]; // Define a ring buffer instance
static struct ring_buf ringbuf;
uint8_t err;

void ring_rd_wt(uint8_t data_write[], uint8_t data_read[])
{
    int bytes_written = ring_buf_put(&ringbuf, data_write, (strlen(data_write) + 1));
    if (bytes_written < 0)
    {
        LOG_INF("Error writing to ring buffer");
    }

    // Example usage: reading data from the ring buffer

    int bytes_read = ring_buf_get(&ringbuf, data_read, RING_BUF_SIZE);
    if (bytes_read < 0)
    {
        LOG_INF("Error reading from ring buffer");
    }
}

void set_to_pwm(uint8_t data_read[])
{

    // Convert the data read from string to decimal
    int brightness = atoi((char *)data_read);

    // Adjust brightness value to fit within PWM range
    int duty_cycle_ticks = (brightness * PWM_PERIOD_US) / 100;
    LOG_INF("duty = %d", duty_cycle_ticks);

    // Set PWM duty cycle
    err = pwm_set_dt(&pwm_dev, PWM_PERIOD_US, duty_cycle_ticks);
    if (err)
    {
        LOG_ERR("Error setting PWM duty cycle: %d", err);
        return;
    }
    LOG_INF("Set PWM duty cycle to %d ticks", duty_cycle_ticks);
}

int main(void)
{   
    LOG_INF("Enter the percentage in between 0 to 100 to bright the led");
    // Initialize the PWM device
    if (!device_is_ready(pwm_dev.dev))
    {
        LOG_ERR("Error: Unable to bind to PWM device");
        return 0;
    }

    // Initialize the ring buffer
    ring_buf_init(&ringbuf, RING_BUF_SIZE, buffer);

    while (1)
    {
        // Example usage: writing data to the ring buffer
        uint8_t data_write[10] = {0};
        uint8_t data_read[RING_BUF_SIZE];
        uint8_t received = 0;
        uint8_t i = 0;
        err = uart_fifo_read(uart_dev, &received, sizeof(received));
        if (err < 0)
        {
            LOG_ERR("Error writting from ring buffer");
        }
        while (received != '\n')
        {
            err = uart_fifo_read(uart_dev, &received, sizeof(received));
            if (err < 0)
            {
                LOG_ERR("Error writting from ring buffer");
            }
            else if (err > 0)
            {

                if (received != '\n')
                {
                    data_write[i] = received;
                    ++i;
                }
            }
        }

        data_write[i] = '\0';

        ring_rd_wt(data_write, data_read);

        set_to_pwm(data_read);
    }

    return 0;
}
