#include <zephyr/kernel.h>
#include <zephyr/drivers/uart.h>
#include <zephyr/sys/ring_buffer.h>
#include <zephyr/sys/printk.h>

#define BUF_SIZE 64

static const struct device *uart_dev = DEVICE_DT_GET(DT_NODELABEL(uart0));
// Define a ring buffer instance
static uint8_t buffer[BUF_SIZE];
static struct ring_buf ringbuf;

int main(void)
{
        uint8_t err;
        // Initialize the ring buffer
        ring_buf_init(&ringbuf, BUF_SIZE, buffer);
label:
        // Example usage: writing data to the ring buffer
        uint8_t data_write[] = {0};
        uint8_t data_read[BUF_SIZE];
        uint8_t received = 0;
        uint8_t i = 0;
        uart_fifo_read(uart_dev, &received, sizeof(received));
        if (err < 0)
        {
                printk("Error writting from ring buffer");
        }
        while (received != '\n')
        {
                err = uart_fifo_read(uart_dev, &received, sizeof(received));
                if (err < 0)
                {
                        printk("Error writting from ring buffer");
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

        int bytes_written = ring_buf_put(&ringbuf, data_write, strlen(data_write)-1);
        if (bytes_written < 0)
        {
                printk("Error writing to ring buffer\n");
        }
        else
        {
                printk("data_write: %s \n", data_write);
                printk("ring: %s \n", buffer);
        }

        // Example usage: reading data from the ring buffer

        int bytes_read = ring_buf_get(&ringbuf, data_read, BUF_SIZE);
        if (bytes_read < 0)
        {
                printk("Error reading from ring buffer\n");
        }
        else
        {
                printk("data_read: %s \n", data_read);
                printk("ring: %s \n", buffer);
        }

        goto label;
        return 0;
}
