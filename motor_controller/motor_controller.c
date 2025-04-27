#include <stdio.h>
#include "pico/stdlib.h"

#define pwm_pin 22
#define A_pin 16
#define B_pin 17

void init_pins() {
    gpio_init(A_pin);
    gpio_init(B_pin);
    gpio_init(pwm_pin);
    gpio_set_dir(A_pin, GPIO_OUT);
    gpio_set_dir(B_pin, GPIO_OUT);
    gpio_set_dir(pwm_pin, GPIO_OUT);
}

int main()
{
    stdio_init_all();
    sleep_ms(1000);
    init_pins();
    gpio_put(A_pin, 1);
    gpio_put(B_pin,0);
    gpio_put(pwm_pin,1);
    uint32_t period_ms = 1000;
    /*
    while(1) {
        gpio_put(pwm_pin,0);
        sleep_ms(2*period_ms);
        gpio_put(pwm_pin,1);
        sleep_ms(period_ms);
    }
        */
    while(1) {
        gpio_put(A_pin,1);
        sleep_ms(2);
        gpio_put(A_pin,0);
        sleep_ms(18);

    }

}
