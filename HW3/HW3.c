#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/adc.h"

#define ADC_PIN 26
#define BUTTON_PIN 2
#define LED_PIN 3

void init_pins() {
    gpio_init(BUTTON_PIN);
    gpio_set_dir(BUTTON_PIN, GPIO_IN);
    gpio_init(LED_PIN);
    gpio_set_dir(LED_PIN, GPIO_OUT);

    adc_init();
    adc_gpio_init(26);  
    adc_select_input(0); 
}

int main()
{
    stdio_init_all();

    while (true) {
        while (!stdio_usb_connected()) {
            sleep_ms(1000);
        }
        init_pins();
        gpio_put(LED_PIN, 1);
        while(1) {
            if (gpio_get(BUTTON_PIN)) {
                break;
            }
        }
        gpio_put(LED_PIN, 0);

        while(1) {
            int num_samples;
            printf("Please enter a number of samples (1-100): ");
            scanf("%d", &num_samples);
            for (uint8_t i = 0; i < num_samples; i++) {
                uint16_t voltage = adc_read();
                double volts = (voltage / 4095.00) * 3.3;
                //printf("%d %d", voltage, volts);
                printf("%f Volts\n", volts);
            }
            sleep_ms(1000);
        }


    }
}
