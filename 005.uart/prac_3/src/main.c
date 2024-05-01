// receive the data using uart interrupt method
/* Controlling LEDs through UART. Press 1-3 on your keyboard to toggle LEDS on your development kit */
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

static void uart_cb(const struct device *dev, void *user_data)
{       
        uint8_t data;
        static uint8_t toggle = 0;
        if(uart_irq_update(dev) && uart_irq_rx_ready(dev))
        {
                if (uart_fifo_read(dev, &data, sizeof(data))) // check the length of character send by serial monitor
                {
                        gpio_pin_toggle_dt(&led0);
                }
                if(toggle==1){
                        LOG_INF("Led is on");
                        toggle = 0;
                }
                else if(toggle==0){
                        LOG_INF("Led is off");
                        toggle = 1;
                }
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
        ret = uart_irq_callback_user_data_set(uart, uart_cb,NULL);
        if (ret)
        {
                return 1;
        }
        uart_irq_rx_enable(uart);

        while (1)
        {       
                k_msleep(SLEEP_TIME_OUT);
        }
        return 0;
}
