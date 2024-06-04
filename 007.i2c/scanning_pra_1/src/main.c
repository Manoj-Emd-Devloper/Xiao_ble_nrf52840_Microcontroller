#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/drivers/i2c.h>
#include <zephyr/devicetree.h>
#include <zephyr/sys/printk.h>

const struct device *i2c_dev = DEVICE_DT_GET(DT_NODELABEL(i2c0));

void i2c_scan_touch()
{
        int ret;
        uint8_t i2c_addr;
        if (!device_is_ready(i2c_dev))
        {
                printk("Cannot get I2C device\n");
                return;
        }

        printf("Scanning I2C bus...\n");
        // in nrf52840, Scan I2C bus from 0x08 to 0x78
        for (i2c_addr = 0x08; i2c_addr < 0x78; i2c_addr++)
        {
                struct i2c_msg msgs[1];
                uint8_t data = 0; // Dummy data, not used for scanning

                msgs[0].buf = &data;
                msgs[0].len = sizeof(data);
                msgs[0].flags = I2C_MSG_WRITE | I2C_MSG_STOP;

                ret = i2c_transfer(i2c_dev, msgs, 1, i2c_addr);
                if (ret == 0)
                {
                        printk("Found device at address: 0x%02X\n", i2c_addr);
                        return;
                }
        }
        if (ret == -EIO)
        {
                // No ACK received, device not present
                printk("Error scanning I2C bus: %d\n", ret);
        }
}

int main(void)
{
        i2c_scan_touch();
        return 0;
}