#include <stdio.h>
#include <string.h>
#include "pico/stdlib.h"
#include "hardware/i2c.h"
#include "ssd1306.h"
#include "font.h"
#include "math.h"
#include "HW7.h"



// I2C defines
// This example will use I2C0 on GPIO8 (SDA) and GPIO9 (SCL) running at 400KHz.
// Pins can be changed, see the GPIO function select table in the datasheet for information on GPIO assignments
#define I2C_PORT i2c0
#define I2C_SDA 8
#define I2C_SCL 9

#define IMU_ADDR 0b1101000 
//#define IMU_ADDR 0b1100010
#define PIX_ADDR 0b0111100

void init_pins() {
    i2c_init(I2C_PORT, 400*1000);
    
    gpio_set_function(I2C_SDA, GPIO_FUNC_I2C);
    gpio_set_function(I2C_SCL, GPIO_FUNC_I2C);
    //gpio_pull_up(I2C_SDA);
    //gpio_pull_up(I2C_SCL);

    ssd1306_setup();
}

void init_imu() {
    uint8_t src1[2] = {PWR_MGMT_1, 0b11100000};
    i2c_write_blocking(i2c_default, IMU_ADDR, src1, 2, false);
    uint8_t src2[2] = {ACCEL_CONFIG, 0b00000000};
    i2c_write_blocking(i2c_default, IMU_ADDR, src2, 2, false);
    uint8_t src3[2] = {GYRO_CONFIG, 0b11111000};
    i2c_write_blocking(i2c_default, IMU_ADDR, src3, 2, false);
}

void read_imu(int8_t* dst) {
    uint8_t src = ACCEL_XOUT_H;
    i2c_write_blocking(i2c_default, IMU_ADDR, &src, 1, true);
    i2c_read_blocking(i2c_default, IMU_ADDR, &dst[0], 14, false);
    //return dst;
}

float* process_imu_data(int8_t* dst) {
    static float p[7];
    for (int8_t i = 0; i < 7; i++) {
        p[i] = (dst[2*i] << 8) | dst[(2*i)+1];
    }
    p[0] *= 0.000061;
    p[1] *= 0.000061;
    p[2] *= 0.000061;

    p[3] = (p[3] / 340.00) + 36.53;

    p[4] *= 0.007630;
    p[5] *= 0.007630;
    p[6] *= 0.007630;

    return p;
}


int main()
{
    stdio_init_all();
    while(!stdio_usb_connected()) {
        sleep_ms(1000);
    }
    init_pins();
    init_imu();

    //uint8_t src = WHO_AM_I;
    //uint8_t dst;
    //i2c_write_blocking(i2c_default, IMU_ADDR, &src, 1,true);
    //i2c_read_blocking(i2c_default, IMU_ADDR, &dst, 1, false);
    //printf("%d\n", dst);

    uint8_t dst[14];
    float* processed_data;

    uint8_t accel[100];
    uint8_t temp[100];
    uint8_t gyro[100];

    while (true) {
        //sleep_ms(10);
        read_imu(dst);
        processed_data = process_imu_data(dst);

        for (uint8_t i = 0; i < 7; i++) {
            printf("%.3f, ",processed_data[i]);
        }
        printf("\n");

        sprintf(accel, "%.2f, %.2f, %.2f", fabs(processed_data[0]), fabs(processed_data[1]), fabs(processed_data[2]));
        sprintf(temp, "%.2f", processed_data[3]);
        sprintf(gyro,"%.2f, %.2f, %.2f", fabs(processed_data[4]), fabs(processed_data[5]), fabs(processed_data[6]));
        write_string(accel,1,1);
        write_string(temp,1,10);
        write_string(gyro,1,19);
    }

}
