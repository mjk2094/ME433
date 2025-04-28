#include <stdio.h>
#include "string.h"
#include "pico/stdlib.h"
#include "hardware/i2c.h"
#include "ssd1306.h"
#include "font.h"
#include "hardware/adc.h"

// I2C defines
// This example will use I2C0 on GPIO8 (SDA) and GPIO9 (SCL) running at 400KHz.
// Pins can be changed, see the GPIO function select table in the datasheet for information on GPIO assignments
#define I2C_PORT i2c0
#define I2C_SDA 8
#define I2C_SCL 9

void write_char(uint8_t buf, uint8_t x, uint8_t y) {
    uint8_t character = buf - 0x20;
    for (uint8_t i=0; i<5; i++) {
        for (uint8_t j=0; j<8; j++) {
            uint8_t pix = (ASCII[character][i] >> (j-1) ) & 0x1;
            ssd1306_drawPixel(x+i,y+j,pix);
            //printf("%d at (%d, %d)\n",pix,x+i,y+j);
        }
    }
    ssd1306_update();
}

void write_string(uint8_t* buf, uint8_t x, uint8_t y) {
    uint8_t i = 0;
    while(true) {
        if(buf[i] == 0)
            break;
        write_char(buf[i], x+(5*i), y);
        x++;
        i++;
    }
}

int main()
{
    stdio_init_all();
    while (!stdio_usb_connected()) {
        sleep_ms(1000);
    }
    i2c_init(I2C_PORT, 400*1000);
    gpio_set_function(I2C_SDA, GPIO_FUNC_I2C);
    gpio_set_function(I2C_SCL, GPIO_FUNC_I2C);
    ssd1306_setup();

    adc_init();
    adc_gpio_init(26);  
    adc_select_input(0); 

    uint8_t adc_output[100];
    uint8_t fps[100];
    uint8_t time_elapsed[100];
    uint8_t str[100];
    uint8_t str2[100];
    sprintf(str,"How many cats are you");
    sprintf(str2,"gonna have");

    unsigned int t1=0;
    unsigned int t2=0;
    double time=0;
    double frames = 0;
    sleep_ms(2000);
    write_string(str,1,1);
    write_string(str2,1,10);

    uint8_t cats[100];

    uint8_t i=1;
    while(1) {
        sprintf(cats,"%d",i);
        write_string(cats,1,20);
        i++;
    }

    while (true) {
        t1 = to_us_since_boot(get_absolute_time());
        time = (t1 - t2);
        frames = 1000000.0 / time;

        t2 = to_us_since_boot(get_absolute_time());
        sprintf(time_elapsed,"%.3f",frames);
        //printf("%f\n",frames);
        write_string(time_elapsed,90,24);

        uint16_t voltage = adc_read();
        double volts = (voltage / 4095.00) * 3.3;
        sprintf(adc_output, "%.4f",volts);        
        write_string(adc_output,1,24);
        sprintf(fps,"FPS: ");
        write_string(fps,65,24);
        //write_string(time_elapsed,100,24);

        
    }
}
