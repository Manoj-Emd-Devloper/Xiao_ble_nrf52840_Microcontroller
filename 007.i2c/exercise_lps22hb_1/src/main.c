#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/devicetree.h>
#include <zephyr/drivers/i2c.h>
#include <zephyr/logging/log.h>

#define I2C1_NODE DT_NODELABEL(mysensor)
#define LPS22HB_REG_TEMP_OUT_L 0x2B
#define LPS22HB_REG_TEMP_OUT_H 0x2C
#define DELAY 1000

LOG_MODULE_REGISTER(PRAC_4, LOG_LEVEL_DBG);

static const struct i2c_dt_spec dev_i2c = I2C_DT_SPEC_GET(I2C1_NODE);
uint8_t temp_reading[2] = {0};
uint8_t sensor_regs[2] = {LPS22HB_REG_TEMP_OUT_L, LPS22HB_REG_TEMP_OUT_H};

int main(void)
{
        int ret;
        k_msleep(6000);
        if (!device_is_ready(dev_i2c.bus))
        {
                LOG_ERR("I2C bus %s is not ready!\n\r", dev_i2c.bus->name);
                return 0;
        }
        
        while (1)
        {
                ret = i2c_write_read_dt(&dev_i2c, &sensor_regs[0], 1, &temp_reading[0], 1);
                if (ret != 0)
                {
                        LOG_ERR("Failed to write/read I2C device address %x at Reg. %x \r\n", dev_i2c.addr, sensor_regs[0]);
                }
                ret = i2c_write_read_dt(&dev_i2c, &sensor_regs[1], 1, &temp_reading[1], 1);
                if (ret != 0)
                {
                        LOG_ERR("Failed to write/read I2C device address %x at Reg. %x \r\n", dev_i2c.addr, sensor_regs[1]);
                }

                int16_t raw_temperature = (temp_reading[1] << 8) | temp_reading[0];

                // Convert raw temperature data to temperature in degrees Celsius
                float temperature_celsius = (float)raw_temperature / 100.0f;

                // Print reading to console
                LOG_INF("Temperature in Celsius : %.2f C \n", temperature_celsius);
                k_msleep(DELAY);
        }
        return 0;
}
