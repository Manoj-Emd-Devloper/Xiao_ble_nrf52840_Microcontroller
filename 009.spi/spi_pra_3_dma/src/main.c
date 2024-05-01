#include <zephyr/kernel.h>
#include <zephyr/devicetree.h>
#include <zephyr/drivers/spi.h>

#define SPI1_NODE DT_NODELABEL(spi2)

static const struct spi_config spi_cfg = {
    .operation = SPI_OP_MODE_MASTER | SPI_WORD_SET(8) | SPI_TRANSFER_MSB |
                 SPI_MODE_CPOL | SPI_MODE_CPHA,
    .frequency = 4000000,
    .slave = 0,
};
static const struct device *spi_dev = DEVICE_DT_GET(SPI1_NODE);
uint8_t tx_buffer[4] = {1, 2, 3, 4};
static uint8_t rx_buffer[4] = {0};
volatile static uint8_t flag = 0;

const struct spi_buf tx_buf = {
    .buf = tx_buffer,
    .len = sizeof(tx_buffer)};

const struct spi_buf_set tx = {
    .buffers = &tx_buf,
    .count = 1};

struct spi_buf rx_buf = {
    .buf = rx_buffer,
    .len = sizeof(rx_buf),
};
const struct spi_buf_set rx = {
    .buffers = &rx_buf,
    .count = 1};

void callback(const struct device *dev, int result, void *data)
{
    flag = 1;
    return;
}

int main(void)
{
    int err;

    if (!device_is_ready(spi_dev))
    {
        printk("SPI device is not ready \n\r");
        return 0;
    }

    while (1)
    {
        err = spi_transceive_cb(spi_dev, &spi_cfg, &tx, &rx, callback, NULL);
        if (err < 0)
        {
            printk("callback function is not trigger \n\r");
            return 0;
        }

        if (flag == 1)
        {
            err = spi_transceive(spi_dev, &spi_cfg, &tx, &rx);
            if (err)
            {
                printk("SPI error: %d\n", err);
            }

            else
            {
                printk("TRANSMITTED : \n\r");
                for (int i = 0; i < sizeof(tx_buffer); i++)
                {
                    printk("%d ", tx_buffer[i]);
                }
                printk("\n\rRECEIVE : \n\r");
                for (int i = 0; i < sizeof(rx_buffer); i++)
                {
                    printk("%d ", rx_buffer[i]);
                }
                printk("\n\r===============\n\r");
                flag = 0;
                k_msleep(4000);
            }
        }
    }
    return 0;
}