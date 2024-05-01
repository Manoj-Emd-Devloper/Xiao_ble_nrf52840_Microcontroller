#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/devicetree.h>
#include <zephyr/drivers/i2c.h>
#include <zephyr/logging/log.h>

#define I2C_NODE DT_NODELABEL(i2c1)

LOG_MODULE_REGISTER(PRAC_1, LOG_LEVEL_DBG);

static const struct device *i2c_dev = DEVICE_DT_GET(I2C_NODE);

int main(void)
{
        int ret;
        k_msleep(5000);
        if (!device_is_ready(i2c_dev))
        {
                LOG_ERR("I2C bus is not ready!\n\r");
                return -1;
        }

        LOG_INF("Scanning I2C bus...\n");
        uint8_t address;
        for (address = 1; address < 127; address++)
        {
                struct i2c_msg msgs[1];
                uint8_t dummy_data = 0;

                msgs[0].buf = &dummy_data;
                msgs[0].len = 0U;
                msgs[0].flags = I2C_MSG_WRITE | I2C_MSG_STOP;

                ret = i2c_transfer(i2c_dev, &msgs[0], 1, address);
                if (ret == 0)
                {
                        LOG_INF("Device found at address 0x%x\n", address);
                }

        }
        if (ret == -ENODEV)
        {
                printf("No device found at address 0x%x\n", address);
        }
        return 0;
}
