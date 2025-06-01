#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/pwm.h"
#include "cam.h"

#define LEFT_PIN 16 // the built in LED on the Pico
#define RIGHT_PIN 17
#define WRAP 50000

void init_pwm() {
  gpio_set_function(LEFT_PIN, GPIO_FUNC_PWM);
  gpio_set_function(RIGHT_PIN, GPIO_FUNC_PWM);

  uint slice_num_A = pwm_gpio_to_slice_num(LEFT_PIN);
  uint slice_num_B = pwm_gpio_to_slice_num(RIGHT_PIN);
  float div = 3; // must be between 1-255
  pwm_set_clkdiv(slice_num_A, div); 
  pwm_set_clkdiv(slice_num_B, div); 

  uint16_t wrap = WRAP; // when to rollover, must be less than 65535
  pwm_set_wrap(slice_num_A, wrap);
  pwm_set_wrap(slice_num_B, wrap);

  pwm_set_enabled(slice_num_A, true); // turn on the PWM
  pwm_set_enabled(slice_num_B, true); // turn on the PWM

  pwm_set_gpio_level(LEFT_PIN, wrap / 2); // set the duty cycle to 50%
  pwm_set_gpio_level(RIGHT_PIN, wrap / 2); // set the duty cycle to 50%
}


int main()
{
    stdio_init_all();

    while (!stdio_usb_connected()) {
        sleep_ms(100);
    }
    printf("Hello, camera!\n");

    init_camera_pins();
    init_pwm();
 
    while (true) {
        // uncomment these and printImage() when testing with python 
        //char m[10];
        //scanf("%s",m);

        setSaveImage(1);
        while(getSaveImage()==1){}
        convertImage();
        int com = findLine(IMAGESIZEY/2); // calculate the position of the center of the ine
        setPixel(IMAGESIZEY/2,com,0,255,0); // draw the center so you can see it in python
        //printImage();
        printf("%d\r\n",com); // comment this when testing with python

        if (com > 75 || com < 1) {
            continue;
        }
        else if (com >= 38) { //VEERING LEFT
            uint8_t excess = com - 38;
            float extra_speed = ((excess * 0.2) + 1.0)/100.0;

            pwm_set_gpio_level(RIGHT_PIN, 1.0/100.0 * WRAP);
            pwm_set_gpio_level(LEFT_PIN, extra_speed * WRAP);
            //printf("LEFT, Extra speed: %f\n", extra_speed);
        }

        else if (com < 38) { //VEERING RIGHT
            uint8_t excess = 38 - com;
            float extra_speed = ((excess * 0.2) + 1.0)/100.0;

            pwm_set_gpio_level(LEFT_PIN, 1.0/100.0 * WRAP);
            pwm_set_gpio_level(RIGHT_PIN, extra_speed * WRAP);
            //printf("RIGHT, Extra speed: %f\n", extra_speed);
        }
        sleep_ms(100);
    }
}

