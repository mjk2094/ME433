/**
 * Copyright (c) 2020 Raspberry Pi (Trading) Ltd.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <stdio.h>
#include "pico/stdlib.h"
#include "pico/multicore.h"
#include "hardware/adc.h"

#define FLAG_VALUE 123
#define READ_VOLTAGE 0
#define LED_ON 1
#define LED_OFF 2

#define LED_PIN 15

void core1_init() {
    gpio_init(LED_PIN);
    gpio_set_dir(LED_PIN, GPIO_OUT);

    adc_init();
    adc_gpio_init(26);  
    adc_select_input(0);
}

void core1_entry() {

    core1_init();
    multicore_fifo_push_blocking(FLAG_VALUE);

    while(1) {

        uint32_t g = multicore_fifo_pop_blocking();

        if (g == READ_VOLTAGE) {
            printf("0\n");
            uint16_t v = adc_read();
            float voltage = (v*3.3) / 4095.0;
            printf("Voltage: %f\n",voltage);
        }
        if (g == LED_ON) {
            printf("1\n");
            gpio_put(LED_PIN,1);
        }
        if (g == LED_OFF) {
            printf("2\n");
            gpio_put(LED_PIN,0);
        }
        multicore_fifo_push_blocking(FLAG_VALUE);
    }
    //while (1)
        //tight_loop_contents();
}

int main() {
    stdio_init_all();
    while (!stdio_usb_connected()) {
        sleep_ms(100);
    }
    
    printf("Hello, multicore!\n");

    /// \tag::setup_multicore[]

    multicore_launch_core1(core1_entry);

    // Wait for it to start up

    uint32_t g = multicore_fifo_pop_blocking();

    uint8_t input;
    while(1) {
        printf("Enter a number (0,1,2): ");
        scanf("%d",&input);
        printf("\n");
        multicore_fifo_push_blocking(input);
        g = multicore_fifo_pop_blocking();
        while(1) {
            if (g==FLAG_VALUE) {
                break;
            }
        }
    }

    /// \end::setup_multicore[]
}
