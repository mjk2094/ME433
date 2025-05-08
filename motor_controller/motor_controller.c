#include <stdio.h>
#include "pico/stdlib.h"

#define pwm_pin 2
#define A_pin 0
#define B_pin 1

#define MIN_DUTY_CYCLE_US 500
#define MAX_DUTY_CYCLE_US 2500
#define PERIOD_US 100

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
    uint64_t pulse_width_us = 25000;
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
        sleep_us(pulse_width_us);
        gpio_put(A_pin,0);
        sleep_us(PERIOD_US - pulse_width_us);
    }

}
