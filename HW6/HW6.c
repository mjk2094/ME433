#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/i2c.h"

// I2C defines
// This example will use I2C0 on GPIO8 (SDA) and GPIO9 (SCL) running at 400KHz.
// Pins can be changed, see the GPIO function select table in the datasheet for information on GPIO assignments
#define I2C_PORT i2c0
#define I2C_SDA 8
#define I2C_SCL 9

#define ADDR 0b0100000

#define REGISTER_IODIR 0x00
#define REGISTER_GPIO 0x09

void init_pins() {
    gpio_init(PICO_DEFAULT_LED_PIN);
    gpio_set_dir(PICO_DEFAULT_LED_PIN, GPIO_OUT);

}

int main()
{
    stdio_init_all();
    init_pins();

    i2c_init(I2C_PORT, 400*1000);
    
    gpio_set_function(I2C_SDA, GPIO_FUNC_I2C);
    gpio_set_function(I2C_SCL, GPIO_FUNC_I2C);
    //gpio_pull_up(I2C_SDA);
    //gpio_pull_up(I2C_SCL);

    uint8_t turn_on[2] = {REGISTER_GPIO, 0b10000000};
    uint8_t turn_off[2] = {REGISTER_GPIO, 0};

    i2c_write_blocking(i2c_default, ADDR, turn_off, 2, false);
    
    while (true) {
        gpio_put(PICO_DEFAULT_LED_PIN, 1);
        sleep_ms(17);
        gpio_put(PICO_DEFAULT_LED_PIN, 0);
        sleep_ms(17);
        
        uint8_t init_gpio_dir[2] = {REGISTER_IODIR, 0b00000001};
        i2c_write_blocking(i2c_default, ADDR, init_gpio_dir, 2, false);
        //i2c_write_blocking(i2c_default, ADDR, test, 2, false);

        uint8_t src = REGISTER_GPIO;
        uint8_t dst;

        i2c_write_blocking(i2c_default, ADDR, &src, 1, true);  // true to keep master control of bus
        i2c_read_blocking(i2c_default, ADDR, &dst, 1, false);  // false - finished with bus

        printf("%d\n",dst);
        if (dst & 0b00000001) {
            i2c_write_blocking(i2c_default, ADDR, turn_off, 2, false);
        }
        else {
            i2c_write_blocking(i2c_default, ADDR, turn_on, 2, false);
        }


    }
}
