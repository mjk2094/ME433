#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/spi.h"
#include <math.h>

// SPI Defines
// We are going to use SPI 0, and allocate it to the following GPIO pins
// Pins can be changed, see the GPIO function select table in the datasheet for information on GPIO assignments
#define SPI_PORT spi0

#define SPI_PIN_MISO 16
#define SPI_PIN_SCK  18
#define SPI_PIN_MOSI 19

#define DAC_CS 17
#define RAM_CS 13

#define pi 3.1415

union FloatInt {
    float f;
    uint32_t i;
};

void init_spi() {
    // SPI initialisation. This example will use SPI at 1MHz.
    spi_init(SPI_PORT, 1000*1000);
    gpio_init(RAM_CS);
    gpio_init(DAC_CS);
    gpio_set_function(SPI_PIN_MISO, GPIO_FUNC_SPI);
    gpio_set_function(SPI_PIN_SCK,  GPIO_FUNC_SPI);
    gpio_set_function(SPI_PIN_MOSI, GPIO_FUNC_SPI);

    //gpio_set_function(RAM_CS,   GPIO_FUNC_SIO);
    //gpio_set_function(DAC_CS, GPIO_FUNC_SIO);
    
    // Chip select is active-low, so we'll initialise it to a driven-high state
    gpio_set_dir(RAM_CS, GPIO_OUT);
    gpio_set_dir(DAC_CS, GPIO_OUT);
    gpio_put(RAM_CS, 1);
    gpio_put(DAC_CS, 1);
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

void write_spi(uint8_t port, uint8_t* data, size_t len) {
    cs_select(port);
    spi_write_blocking(SPI_PORT, data, len); // where data is a uint8_t array with length len
    cs_deselect(port);
}


void time_math() {
    const int loops = 200;
    volatile float f1, f2;
    f1 = 7;
    f2 = 7;
    volatile float f_add, f_sub, f_mult, f_div;
    unsigned long long t[4];
    for (uint8_t j=0; j<4; j++) {
        absolute_time_t t1 = get_absolute_time();
        for (uint8_t i=0; i < loops; i++) {
            //printf("Enter two floats to use:");
            //scanf("%f %f", &f1, &f2);
            switch (j) {
                case 0:
                    f_add = f1+f2;
                    break;
                case 1:
                    f_sub = f1-f2;
                    break;
                case 2:
                    f_mult = f1*f2;
                    break;
                case 3:
                    f_div = f1/f2;
                    //printf("DIVISON\n");
                    break;
            }
            //printf("\nResults %d: \n%f+%f=%f \n%f-%f=%f \n%f*%f=%f \n%f/%f=%f\n", i, f1,f2,f_add, f1,f2,f_sub, f1,f2,f_mult, f1,f2,f_div);
        }
        absolute_time_t t2 = get_absolute_time();
        float rosie = f_add+f_sub+f_mult+f_div;
        double time1 = to_us_since_boot(t1);
        double time2 = to_us_since_boot(t2);
        double total_time = time2 - time1;
        double loop_time = total_time / loops;
        t[j] = loop_time / 0.006667;
    }
    printf("Add: %llu\n", t[0]);
    printf("Sub: %llu\n", t[1]);
    printf("Mult: %llu\n", t[2]);
    printf("Div: %llu\n", t[3]);
}

double* generate_sin_wave() {
    static double x[1000];
    for (uint8_t i = 0; i < 1000; i++) {
        x[i] = sin(2 * pi * i / 1000);
    }
    return x;
}

void init_spi_ram() {
    uint8_t data[2];
    data[0] = 0b00000001;
    data[1] = 0b01000000;
    write_spi(RAM_CS, data, 2);
}

float read_spi_ram(uint32_t address) {
    uint8_t write[3];
    uint8_t read[4];
    write[0] = 0b00000011;
    write[1] = (address >> 8) & 0xFF;
    write[2] = address & 0xFF;
    cs_select(RAM_CS);
    spi_write_blocking(SPI_PORT, write, 3);
    spi_read_blocking(SPI_PORT, 0, read, 4);
    cs_deselect(RAM_CS);

    union FloatInt val;
    val.i = (read[0] << 24) | (read[1] << 16) | (read[2] << 8) | read[3];
    return val.f;
    
}

void write_spi_ram(union FloatInt* data, size_t len) {

    uint8_t data_to_write[7];
    data_to_write[0] = 0b00000010;
    for (uint32_t i = 0; i < len; i++) {
        uint32_t address = i * 4;
        uint32_t word = data[i].i;

        data_to_write[1] = (address >> 8) & 0xFF;
        data_to_write[2] = address & 0xFF;
        data_to_write[3] = (word >> 24) & 0xFF;
        data_to_write[4] = (word >> 16) & 0xFF;
        data_to_write[5] = (word >> 8) & 0xFF;
        data_to_write[6] = word & 0xFF;

        write_spi(RAM_CS, data_to_write, 7);
    }

    /*uint8_t data_to_write[7];
    data_to_write[0] = 0b00000010;
    for (uint32_t i=0; i < len; i++) {
        uint32_t address = i * 4;
        data_to_write[1] = (address >> 8) & 0xFF;
        data_to_write[2] = address & 0xFF;
        union FloatInt current_float = data[i];
        //printf("%f\n", current_float.f);
        uint32_t current_int = current_float.i;
        for (uint8_t j=0; j < 4; j++) {
            data_to_write[3+j] = (current_int >> (8*(3-j))) & 0xFF;
        }
        write_spi(RAM_CS, data_to_write, 7);
        printf("%f\n", (data_to_write[3] << 24) | (data_to_write[4] << 16) | (data_to_write[5] << 8) | data_to_write[6]);
        //printf("%d %d %d %d\n", data_to_write[3], data_to_write[4], data_to_write[5], data_to_write[6] );
    }*/
    
}

int main() {
    stdio_init_all();
    while (!stdio_usb_connected()) {
        sleep_ms(1000);
    }
    init_spi();
    init_spi_ram();

    const int len = 1000;
    union FloatInt x[len];
    for (uint16_t i = 0; i < len; i++) {
        float z = (float)sin(2 * pi * i / len);
        float y = (z * 1.65) + 1.65;
        //printf("%f\n", y);
        x[i] = (union FloatInt)y;
        //printf("%f     %d\n",x[i].f, x[i].i);
    }
    //printf("WRITING SPI TO RAM\n");
    write_spi_ram(x,len);

    while(1) {
        float buffer[len];
        for (uint32_t i=0; i<len; i++) {
            uint32_t address = i*4;
            buffer[i] = read_spi_ram(address);
            printf("%f\n", buffer[i]);
            uint16_t converted = (uint16_t)((buffer[i] /3.3) * 1000);
            //printf("%d\n", converted);

            uint8_t data[2];
            data[0] = 0b01110000;
            data[1] = 0b00000000;

            data[1] = data[1] | converted << 2;
            uint16_t masked = converted & 0b0000001111000000;
            data[0] = data[0] | (masked >> 6);

            write_spi(DAC_CS, data, 2);
            //printf("%d\n", integer);
            sleep_ms(1);
        }
    }
}
