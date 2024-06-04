#include <zephyr/kernel.h>
#include "myfunction.h"

int main(void)
{
	int a = 3, b = 4;
	while(1){
		printk("The sum of %d and %d is %d\n\r", a, b, sum(a,b));
		k_msleep(1000);
	}
}
