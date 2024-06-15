#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/devicetree.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/logging/log.h>
#include "max30208.h"

LOG_MODULE_REGISTER(temp_max_xiao);

// I2C node
#define I2C_NODE DT_NODELABEL(max30208)

static const struct i2c_dt_spec max30208 = I2C_DT_SPEC_GET(I2C_NODE);

int main(void)
{
        int8_t ret;
        uint8_t part_id;
        float temp;
        float temperatureF;
        if (!device_is_ready(max30208.bus))
        {
                LOG_ERR("I2C bus is not ready!");
                return -1;
        }

        // Check Part ID
        ret = read_part_id(&max30208, &part_id);
        if (ret < 0 || part_id != PART_IDENTIFIER_VAL)
        {
                LOG_ERR("Failed to read part ID or ID mismatch. Expected: 0x%02x, Read: 0x%02x", PART_IDENTIFIER_VAL, part_id);
                return -1;
        }
        LOG_INF("Part ID: 0x%02x", part_id);

        // Initialize sensor
        ret = max30208_init(&max30208);
        if (ret < 0)
        {
                LOG_ERR("Failed to initialize MAX30208");
                return -1;
        }

        while (1)
        {

                // Trigger temperature conversion
                ret = MAX30208_Trigger_Conversion(&max30208);
                if (ret < 0)
                {
                        LOG_ERR("Failed to trigger conversion");
                        return -1;
                }
                // Delay to allow conversion to complete
                k_msleep(100);

                // Read temperature
                ret = read_temp(&max30208, &temp);
                if (ret < 0)
                {
                        LOG_ERR("Failed to read temperature");
                        return -1;
                }
                // Convert to Fahrenheit
                temperatureF = max30208_toFahrenheit(temp);
                LOG_INF("Temperaturec: %.2f °C\t||\tTemperaturef: %.2f °F", temp, temperatureF);
                flag = 0;

                // Wait before next measurement
                k_msleep(DELAY);
        }

        return 0;
}
