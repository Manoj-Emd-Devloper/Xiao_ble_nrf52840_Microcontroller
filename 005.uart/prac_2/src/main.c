#include <zephyr/kernel.h>
#include <zephyr/drivers/uart.h>
#include <zephyr/logging/log.h>

#define SLEEP_TIME_MS 1000
#define BUF_SIZE 15

LOG_MODULE_REGISTER(PRAS_2, LOG_LEVEL_DBG);

static const struct device *uart_dev = DEVICE_DT_GET(DT_NODELABEL(uart0));

int main(void)
{
        uint8_t rx_buffer[BUF_SIZE];
        uint8_t rx_index = 0;
        if (!device_is_ready(uart_dev))
        {
                LOG_ERR("uart_dev not ready \n\r");
                return 1;
        }

        while (1)
        {
                uint8_t received_byte;
                int bytes_read = uart_fifo_read(uart_dev, &received_byte, sizeof(received_byte));
                if (bytes_read > 0)
                {
                        /* Store the received byte in the buffer */
                        rx_buffer[rx_index++] = received_byte;
                        /* Check if the entire string is received */
                        if (received_byte == '\n' || rx_index >= BUF_SIZE - 1)
                        {
                                /* Null-terminate the string */
                                rx_buffer[rx_index] = '\0';

                                /* Print the received string */
                                LOG_INF("Received: %s\n\r", rx_buffer);

                                /* Reset buffer index for the next string */
                                rx_index = 0;
                        }
                }

                k_msleep(SLEEP_TIME_MS);
        }
        return 0;
}
