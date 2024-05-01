// transmit the data using uart interrupt method
#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/devicetree.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/drivers/uart.h>
#include <zephyr/logging/log.h>

#define SLEEP_TIME_OUT 1000
#define RECEIVE_BUFF_SIZE 10
#define RECEIVE_TIMEOUT 100

LOG_MODULE_REGISTER(PRAC_4, LOG_LEVEL_DBG);

const struct device *uart = DEVICE_DT_GET(DT_NODELABEL(uart0));
static const struct gpio_dt_spec led0 = GPIO_DT_SPEC_GET(DT_ALIAS(led0), gpios);
static uint8_t tx_buf[20];

static void uart_cb(const struct device *dev, void *user_data)
{
        if (uart_irq_update(dev) && uart_irq_tx_ready(dev))
        {
                uart_fifo_fill(dev,tx_buf,sizeof(tx_buf));
        }
}

int main(void)
{
        k_msleep(5000);
        int ret;

        if (!device_is_ready(uart))
        {
                LOG_ERR("UART DEVICE NOT READY\r\n");
                return 1;
        }

        if (!device_is_ready(led0.port))
        {
                LOG_ERR("GPIO device is not ready\r\n");
                return 1;
        }

        ret = gpio_pin_configure_dt(&led0, GPIO_OUTPUT_ACTIVE);
        if (ret < 0)
        {
                return 1;
        }
        ret = uart_irq_callback_user_data_set(uart, uart_cb, NULL);
        if (ret)
        {
                return 1;
        }
        for (int i = 0; i < 20; i++)
        {
                tx_buf[i] = 'x';
        }
        uart_irq_tx_enable(uart);

        while (1)
        {
                gpio_pin_toggle_dt(&led0);
                k_msleep(SLEEP_TIME_OUT);
        }
        return 0;
}
