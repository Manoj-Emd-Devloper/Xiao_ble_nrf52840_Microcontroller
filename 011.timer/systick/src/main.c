#include <nrfx_log.h>
#include <nrfx_systick.h>
#include <zephyr/sys/printk.h>

int main(void)
{
        nrfx_systick_init(); // Initialize the SysTick timer
        printk("hello\n\r");
        while (1)
        {
                nrfx_systick_state_t state;
                nrfx_systick_get(&state);
                // printk("%u \n\r",state.time);
                nrfx_systick_delay_ms(2000);
                if (nrfx_systick_test(&state, 2000)) //checks if 2 seconds have passed using the SysTick timer. (newstate - pre state >2000)
                {
                        printk("2 sec have passed\n");
                }
        }
}