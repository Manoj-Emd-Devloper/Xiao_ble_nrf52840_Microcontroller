#include <zephyr/kernel.h>
#include <zephyr/drivers/gpio.h>
#include <hal/nrf_gpio.h>

#define SLEEP_TIME_MS 1000
#define GPIO_PIN NRF_GPIO_PIN_ΜΑΡ(0, 27)

int main(void)
{

        nrf_gpio_cfg_output(GPIO_PIN); // set pin as output

        while (1)
        {
                nrf_gpio_pin_set(GPIO_PIN); // set pin to HIGH (LED off)

                k_msleep(SLEEP_TIME_MS);

                nrf_gpio_pin_clear(GPIO_PIN); // set pin to LOW (LED on)

                k_msleep(SLEEP_TIME_MS);
        }

        return 0;
}