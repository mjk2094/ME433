#include <stdio.h>
#include "stdlib.h"
#include "pico/stdlib.h"
#include "hardware/pwm.h"


#define A_PIN 14 // the built in LED on the Pico
#define B_PIN 17
#define WRAP 50000

void init_pwm() {
  gpio_set_function(A_PIN, GPIO_FUNC_PWM);
  gpio_set_function(B_PIN, GPIO_FUNC_PWM);

  uint slice_num_A = pwm_gpio_to_slice_num(A_PIN);
  uint slice_num_B = pwm_gpio_to_slice_num(B_PIN);
  float div = 3; // must be between 1-255
  pwm_set_clkdiv(slice_num_A, div); 
  pwm_set_clkdiv(slice_num_B, div); 

  uint16_t wrap = WRAP; // when to rollover, must be less than 65535
  pwm_set_wrap(slice_num_A, wrap);
  pwm_set_wrap(slice_num_B, wrap);

  pwm_set_enabled(slice_num_A, true); // turn on the PWM
  pwm_set_enabled(slice_num_B, true); // turn on the PWM

  pwm_set_gpio_level(A_PIN, wrap / 2); // set the duty cycle to 50%
  pwm_set_gpio_level(B_PIN, wrap / 2); // set the duty cycle to 50%
}

int main()
{
    stdio_init_all();
    while (!stdio_usb_connected()) {
        sleep_ms(1000);
    }
    init_pwm();
    float cut = 0;
    int32_t duty_cycle;

    while (true) {
        char command[100];
        
        printf("Enter + or - to adjust duty cycle: ");
        //if (fgets(command, sizeof(command), stdin) != NULL) {
            //printf("You entered: %s\n", command);
        //}
        scanf("%c",&command);

        if (command[0] == '+') {
            cut += 1;
            duty_cycle = (cut / 100.0) * WRAP;
        }
        else if (command[0] == '-') {
            cut -= 1;
            duty_cycle = (cut / 100.0) * WRAP;
        }
        cut = (cut > 100) ? 100 : (cut < -100) ? -100 : cut;
        printf("Cut: %f Duty: %d",cut, duty_cycle);
        printf("\n");
        if (cut < 0) {
            pwm_set_gpio_level(A_PIN, abs(duty_cycle));
            pwm_set_gpio_level(B_PIN, 0);
        }
        else if (cut > 0) {
            pwm_set_gpio_level(A_PIN, 0);
            pwm_set_gpio_level(B_PIN, abs(duty_cycle));
        }
        else {
            pwm_set_gpio_level(A_PIN, 0);
            pwm_set_gpio_level(B_PIN, 0);
        }
    }
}
