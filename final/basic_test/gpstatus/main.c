#include <stdio.h>
#include "pico/stdlib.h"



int main()
{

    int pin1 = 26;
    gpio_init(pin1);
    gpio_set_dir(pin1, GPIO_IN);

    int pin2 = pin1+1;
    gpio_init(pin2);
    gpio_set_dir(pin2, GPIO_IN);

    stdio_init_all();

    while (1)
    {
        printf("pin1: %d   pin2: %d\n", gpio_get(pin1), gpio_get(pin2));
        sleep_ms(100);
        
    }
}
