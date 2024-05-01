#include <zephyr/kernel.h>
#include <zephyr/sys/printk.h>
#include <zephyr/drivers/uart.h>
#include <stdio.h>

#define SLEEP_TIME_MS 1000

static const struct device *uart_dev = DEVICE_DT_GET(DT_NODELABEL(uart0));
static uint8_t tx_buf[15]="Uart\n";
// static int tx_buf_length;
// static int counter = 0;

void main(void)
{
        if (!device_is_ready(uart_dev))
        {
                printk("uart_dev not ready \n\r");
                return;
        }
        while (1)
        {
                printk("hello world\n\r");
                // tx_buf_length = sprintf(tx_buf,"Counter = %d \n\r",counter);
                uart_tx(uart_dev,tx_buf,sizeof(tx_buf),100*USEC_PER_MSEC);
                // counter++;
                k_msleep(SLEEP_TIME_MS);
        }
}
