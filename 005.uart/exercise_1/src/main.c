// Transfer the data using uart dma method
/* Controlling LEDs through UART. Press 1-3 on your keyboard to toggle LEDS 1-3 on your development kit */
#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/devicetree.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/drivers/uart.h>
#include <zephyr/logging/log.h>

#define SLEEP_TIME_OUT 1000
#define RECEIVE_BUFF_SIZE 10
#define RECEIVE_TIMEOUT 100

LOG_MODULE_REGISTER(PRAS_2, LOG_LEVEL_DBG);

const struct device *uart = DEVICE_DT_GET(DT_NODELABEL(uart0));
static const struct gpio_dt_spec led0 = GPIO_DT_SPEC_GET(DT_ALIAS(led0), gpios);
static const struct gpio_dt_spec led1 = GPIO_DT_SPEC_GET(DT_ALIAS(led1), gpios);
static const struct gpio_dt_spec led2 = GPIO_DT_SPEC_GET(DT_ALIAS(led2), gpios);
static uint8_t tx_buf[] = {"Press 1-3 on your keyboard to toggle LEDS 1-3 on your development kit\r\n"};
static uint8_t rx_buf[RECEIVE_BUFF_SIZE] = {0};

static void uart_cb(const struct device *dev, struct uart_event *evt, void *user_data)
{
        switch (evt->type)
        {
        case UART_RX_RDY:
                if ((evt->data.rx.len) == 1) // check the length of character send by serial monitor
                {
                        // pressed 1 for led 0
                        if (evt->data.rx.buf[evt->data.rx.offset] == '1')
                                gpio_pin_toggle_dt(&led0);
                        // pressed 2 for led 1
                        else if (evt->data.rx.buf[evt->data.rx.offset] == '2')
                                gpio_pin_toggle_dt(&led1);
                        // pressed 3 for led 2
                        else if (evt->data.rx.buf[evt->data.rx.offset] == '3')
                                gpio_pin_toggle_dt(&led2);
                        else if (evt->data.rx.buf[evt->data.rx.offset] > 3)
                                LOG_ERR("Press 1-3 on your keyboard to toggle LEDS 1-3 on your development kit\r\n");
                }
                break;
        case UART_RX_DISABLED:
                uart_rx_enable(dev, rx_buf, sizeof rx_buf, RECEIVE_TIMEOUT);
                break;

        default:
                break;
        }
}

int main(void)
{
        k_msleep(5000);
        int ret;

        if (!device_is_ready(uart))
        {
                LOG_INF("UART DEVICE NOT READY\r\n");
                return 1;
        }

        if (!device_is_ready(led0.port))
        {
                LOG_INF("GPIO device is not ready\r\n");
                return 1;
        }

        ret = gpio_pin_configure_dt(&led0, GPIO_OUTPUT_ACTIVE);
        if (ret < 0)
        {
                return 1;
        }
        ret = gpio_pin_configure_dt(&led1, GPIO_OUTPUT_ACTIVE);
        if (ret < 0)
        {
                return 1;
        }
        ret = gpio_pin_configure_dt(&led2, GPIO_OUTPUT_ACTIVE);
        if (ret < 0)
        {
                return 1;
        }

        ret = uart_callback_set(uart, uart_cb, NULL);
        if (ret)
        {
                return 1;
        }
        ret = uart_tx(uart, tx_buf, sizeof(tx_buf), SYS_FOREVER_US);
        if (ret)
        {
                return 1;
        }
        ret = uart_rx_enable(uart, rx_buf, sizeof(rx_buf), RECEIVE_TIMEOUT);
        if (ret)
        {
                return 1;
        }
        while (1)
        {
                k_msleep(SLEEP_TIME_OUT);
        }
        return 0;
}
