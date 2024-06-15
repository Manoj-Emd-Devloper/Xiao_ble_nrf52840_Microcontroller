#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/devicetree.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/logging/log.h>
#include "max30208.h"

LOG_MODULE_REGISTER(temp_max_xiao);

// I2C node
#define I2C_NODE DT_NODELABEL(max30208)
#define GPIO_NODE DT_NODELABEL(max_gpio0)

#define THREAD0_PRIORITY 6
#define STACKSIZE 1024

#define DEBOUNCE_TIME_MS 1000

static const struct i2c_dt_spec max30208 = I2C_DT_SPEC_GET(I2C_NODE);
static const struct gpio_dt_spec max30208_gpio = GPIO_DT_SPEC_GET(GPIO_NODE, gpios);

static struct gpio_callback max30208_gpio_cb_data;
volatile uint8_t flag = 0;

void triggred(const struct device *port, struct gpio_callback *cb, gpio_port_pins_t pins)
{
        static int64_t last_trigger_time = 0;
        int64_t current_time = k_uptime_get();

        if (current_time - last_trigger_time > DEBOUNCE_TIME_MS)
        {
                // LOG_INF("Temperature alarm triggered");
                // uint8_t PIN_state = gpio_pin_get_dt(&max30208_gpio);
                // LOG_INF("PS2 = %d", PIN_state);
                // gpio_pin_set_dt(&max30208_gpio, 0);
                flag = 1;

                // Update last trigger time
                last_trigger_time = current_time;
        }
}

int8_t gpio_trigger(void)
{
        int ret;

        if (!device_is_ready(max30208_gpio.port))
        {
                LOG_ERR("I2C bus is not ready!");
                return -1;
        }

        ret = gpio_pin_configure_dt(&max30208_gpio, GPIO_INPUT);
        if (ret < 0)
        {
                return -1;
        }

        ret = gpio_pin_interrupt_configure_dt(&max30208_gpio, GPIO_INT_EDGE_TO_INACTIVE); // falling edge
        if (ret < 0)
        {
                LOG_INF("Failed to configure interrupt");
                return -1;
        }

        gpio_init_callback(&max30208_gpio_cb_data, triggred, BIT(max30208_gpio.pin));

        ret = gpio_add_callback(max30208_gpio.port, &max30208_gpio_cb_data);
        if (ret < 0)
        {
                LOG_INF("Failed to add callback");
                return -1;
        }

        return 0;
}

int32_t max30208_get_AlarmHigh(const struct i2c_dt_spec *spec, uint8_t *temp)
{
        int32_t ret;

        ret = read_register(spec, ALARM_HIGH_MSB, temp, 2);
        if (ret < 0)
        {
                LOG_ERR("Failed to read ALARM_HIGH_MSB");
                return -1;
        }
        return 0;
}

int32_t max30208_set_AlarmHigh(const struct i2c_dt_spec *spec, uint8_t *temp)
{
        int32_t ret;
        uint8_t reg[2] = {ALARM_HIGH_MSB, temp[0]};
        ret = write_register(spec, reg, 2);
        if (ret < 0)
        {
                LOG_ERR("Failed to write ALARM_HIGH_MSB");
                return -1;
        }
        reg[0] = ALARM_HIGH_LSB;
        reg[1] = temp[1];
        ret = write_register(spec, reg, 2);
        if (ret < 0)
        {
                LOG_ERR("Failed to write ALARM_HIGH_MSB");
                return -1;
        }
        return 0;
}

int8_t alarm_high_setup(uint8_t *reg_AH)
{
        int ret;
        uint8_t ret_AH[2] = {0X00};
        ret = max30208_get_AlarmHigh(&max30208, ret_AH);
        if (ret < 0)
        {
                LOG_ERR("Failed to read alarm high");
                return -1;
        }
        LOG_INF("Alarm high: 0x%02x%02x", ret_AH[0], ret_AH[1]);
        ret = max30208_set_AlarmHigh(&max30208, reg_AH);
        if (ret < 0)
        {
                LOG_ERR("Failed to write alarm high");
                return -1;
        }

        ret = max30208_get_AlarmHigh(&max30208, ret_AH);
        if (ret < 0)
        {
                LOG_ERR("Failed to read alarm high");
                return -1;
        }
        LOG_INF("Alarm high: 0x%02x%02x", ret_AH[0], ret_AH[1]);
        return 0;
}

int32_t max30208_get_AlarmLow(const struct i2c_dt_spec *spec, uint8_t *value)
{
        int8_t ret;
        ret = read_register(spec, ALARM_LOW_MSB, value, 2);
        if (ret < 0)
        {
                LOG_ERR("Failed to read ALARM_LOW_MSB");
                return -1;
        }
        return 0;
}

int32_t max30208_set_AlarmLow(const struct i2c_dt_spec *spec, uint8_t *temp)
{
        int32_t ret;
        uint8_t reg[2] = {ALARM_LOW_MSB, temp[0]};
        ret = write_register(spec, reg, 2);
        if (ret < 0)
        {
                LOG_ERR("Failed to write ALARM_LOW_MSB");
                return -1;
        }
        reg[0] = ALARM_LOW_LSB;
        reg[1] = temp[1];
        ret = write_register(spec, reg, 2);
        if (ret < 0)
        {
                LOG_ERR("Failed to write ALARM_LOW_MSB");
                return -1;
        }
        return 0;
}

int8_t alarm_low_setup(uint8_t *reg_AL)
{
        int8_t ret;
        uint8_t ret_AL[2] = {0X00};
        ret = max30208_get_AlarmLow(&max30208, ret_AL);
        if (ret < 0)
        {
                LOG_ERR("Failed to read alarm low");
                return -1;
        }
        ret = max30208_set_AlarmLow(&max30208, reg_AL);
        if (ret < 0)
        {
                LOG_ERR("Failed to write alarm low");
                return -1;
        }
        ret = max30208_get_AlarmLow(&max30208, ret_AL);
        if (ret < 0)
        {
                LOG_ERR("Failed to read alarm low");
                return -1;
        }
        return 0;
}

int32_t max30208_get_enable_interrupts(const struct i2c_dt_spec *spec, uint8_t *value)
{
        int32_t ret;
        ret = read_register(spec, INTERRUPT_CONF, value, 1);
        if (ret < 0)
        {
                LOG_ERR("Failed to read ALARM_LOW_MSB");
                return -1;
        }
        return 0;
}

int32_t max30208_enable_interrupts(const struct i2c_dt_spec *spec)
{
        int32_t ret;
        uint8_t value = 0;
        ret = max30208_get_enable_interrupts(spec, &value);
        if (ret < 0)
        {
                LOG_ERR("Failed to read ALARM_LOW_MSB");
                return -1;
        }
        LOG_INF("int : 0x%02x ", value);
        uint8_t reg[2] = {INTERRUPT_CONF, 0x02}; // Enable TEMP_LO and TEMP_HI interrupts
        ret = write_register(spec, reg, 2);
        if (ret < 0)
        {
                LOG_ERR("Failed to enable interrupts");
                return -1;
        }
        return 0;
}

int32_t max30208_get_GPIOSetup(const struct i2c_dt_spec *spec, uint8_t *value)
{
        int32_t ret;
        ret = read_register(spec, GPIO_SETUP, value, 1);
        if (ret < 0)
        {
                LOG_ERR("Failed to read ALARM_LOW_MSB");
                return -1;
        }
        return 0;
}

int32_t max30208_configure_gpio(const struct i2c_dt_spec *spec)
{

        int32_t ret;
        uint8_t ret_GPCL[1] = {0X00};
        ret = read_register(&max30208, GPIO_SETUP, ret_GPCL, 1);
        if (ret < 0)
        {
                LOG_ERR("Failed to read GPIO control");
                return -1;
        }
        LOG_INF("GPIO con: 0x%02x", ret_GPCL[0]);
        uint8_t reg[2] = {GPIO_SETUP, 0xAB}; // Configure GPIO0 as interrupt output
        ret = write_register(spec, reg, 2);
        if (ret < 0)
        {
                LOG_ERR("Failed to configure GPIO");
                return -1;
        }
        ret = read_register(&max30208, GPIO_SETUP, ret_GPCL, 1);
        if (ret < 0)
        {
                LOG_ERR("Failed to read GPIO control");
                return -1;
        }
        LOG_INF("GPIO con: 0x%02x", ret_GPCL[0]);
        return 0;
}

int32_t max30208_set_GPIOControl(const struct i2c_dt_spec *spec)
{
        int32_t ret;
        uint8_t ret_GPCL[1] = {0X00};
        ret = read_register(&max30208, GPIO_CONTROL, ret_GPCL, 1);
        if (ret < 0)
        {
                LOG_ERR("Failed to read GPIO control");
                return -1;
        }
        LOG_INF("GPIO Control: 0x%02x", ret_GPCL[0]);

        uint8_t reg[2] = {GPIO_CONTROL, 0x01};
        ret = write_register(spec, reg, 2);
        if (ret < 0)
        {
                LOG_ERR("Failed to set GPIO control");
                return -1;
        }

        ret = read_register(&max30208, GPIO_CONTROL, ret_GPCL, 1);
        if (ret < 0)
        {
                LOG_ERR("Failed to read GPIO control");
                return -1;
        }
        LOG_INF("GPIO Control: 0x%02x", ret_GPCL[0]);
        return 0;
}

int main(void)
{
        int8_t ret;
        uint8_t part_id;
        uint8_t pin_state;
        float temp;
        float temperatureF;
        uint8_t htem_status = 0x00;
        uint8_t int_value = 0;
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

        ret = gpio_trigger();
        if (ret < 0)
        {
                LOG_ERR("Failed to configure GPIO");
                return -1;
        }

        // Initialize sensor
        ret = max30208_init(&max30208);
        if (ret < 0)
        {
                LOG_ERR("Failed to initialize MAX30208");
                return -1;
        }

        ret = max30208_enable_interrupts(&max30208);
        if (ret < 0)
        {
                LOG_ERR("Failed to enable interrupts");
                return -1;
        }

        ret = max30208_configure_gpio(&max30208);
        if (ret < 0)
        {
                LOG_ERR("Failed to configure GPIO");
                return -1;
        }

        uint8_t reg_AL[2] = {0x19, 0x00};
        ret = alarm_low_setup(reg_AL);
        if (ret < 0)
        {
                LOG_ERR("Failed to set alarm low");
                return -1;
        }

        uint8_t reg_AH[2] = {0x1A, 0x90};
        ret = alarm_high_setup(reg_AH);
        if (ret < 0)
        {
                LOG_ERR("Failed to set alarm high");
                return -1;
        }

        // ret = max30208_set_GPIOControl(&max30208);
        // if (ret < 0)
        // {
        //         LOG_ERR("Failed to set GPIO control");
        //         return -1;
        // }

        while (1)
        {
                // ret = max30208_get_enable_interrupts(&max30208, &int_value);
                // if (ret < 0)
                // {
                //         LOG_ERR("Failed to read ALARM_LOW_MSB");
                //         return -1;
                // }
                // LOG_INF("After int : 0x%02x ", int_value);

                ret = read_status(&max30208, &htem_status);
                if (ret < 0)
                {
                        LOG_ERR("Failed to read status");
                        return -1;
                }
                // LOG_INF("Status: 0x%02x", htem_status);

                // pin_state = gpio_pin_get_dt(&max30208_gpio);
                // LOG_INF("ps = %d", pin_state);

                LOG_INF("HELLO!");
                if (flag == 1)
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
                }

                // Wait before next measurement
                k_msleep(DELAY);
        }

        return 0;
}
