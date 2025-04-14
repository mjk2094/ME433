#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/spi.h"
#include <math.h>

// SPI Defines
// We are going to use SPI 0, and allocate it to the following GPIO pins
// Pins can be changed, see the GPIO function select table in the datasheet for information on GPIO assignments
#define SPI_PORT spi0
#define PIN_MISO 16
#define PIN_CS   17
#define PIN_SCK  18
#define PIN_MOSI 19

void init_spi() {
    // SPI initialisation. This example will use SPI at 1MHz.
    spi_init(SPI_PORT, 1000*1000);
    gpio_set_function(PIN_MISO, GPIO_FUNC_SPI);
    gpio_set_function(PIN_CS,   GPIO_FUNC_SIO);
    gpio_set_function(PIN_SCK,  GPIO_FUNC_SPI);
    gpio_set_function(PIN_MOSI, GPIO_FUNC_SPI);
    
    // Chip select is active-low, so we'll initialise it to a driven-high state
    gpio_set_dir(PIN_CS, GPIO_OUT);
    gpio_put(PIN_CS, 1);
    // For more examples of SPI use see https://github.com/raspberrypi/pico-examples/tree/master/spi
}

static inline void cs_select(uint cs_pin) {
    asm volatile("nop \n nop \n nop"); // FIXME
    gpio_put(cs_pin, 0);
    asm volatile("nop \n nop \n nop"); // FIXME
}

static inline void cs_deselect(uint cs_pin) {
    asm volatile("nop \n nop \n nop"); // FIXME
    gpio_put(cs_pin, 1);
    asm volatile("nop \n nop \n nop"); // FIXME
}

void write_spi(uint8_t* data, size_t len) {
    cs_select(PIN_CS);
    spi_write_blocking(SPI_PORT, data, len); // where data is a uint8_t array with length len
    cs_deselect(PIN_CS);
}

int main()
{
    stdio_init_all();
    init_spi();

    const int len = 2;
    uint8_t data[len];
    data[0] = 0b01110000;
    data[1] = 0b00000000;
    while(1) {
        /*
        data[0] = 0b01111111;
        data[1] = 0b11111111;
        write_spi(data,len);
        break;
        */

        for (uint16_t i=0; i < 1023; i++) {
            data[0] = 0b01110000;
            data[1] = 0b00000000;

            int16_t value = (512 * sin(2 * 3.141592653589793238 * i/1023)) + 511;
            data[1] = data[1] | value << 2;
            uint16_t masked = value & 0b0000001111000000;
            data[0] = data[0] | (masked >> 6);
            sleep_us(1000);
            write_spi(data, len);
        }
        


        
    }

}
