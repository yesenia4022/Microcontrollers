#include "driver/i2c.h"
#include "esp_log.h"
#include <stdio.h>
#include <string.h>

#define SLAVE_SCL_IO    22   // GPIO number for I2C slave clock
#define SLAVE_SDA_IO    21   // GPIO number for I2C slave data
#define SLAVE_NUM       I2C_NUM_0  // I2C port number for the slave device
#define SLAVE_ADDR      0x05  // I2C address for the slave device
#define COMPLETION_SIGNAL 0x01  // Signal to indicate completion
#define HELLO_WORLD_CMD 0x02  // Command to print Hello World
#define I2C_SLAVE_RX_BUF_LEN         1024

void i2c_slave_init(void) {
    i2c_config_t i2c_config = {
        .mode = I2C_MODE_SLAVE,
        .sda_io_num = SLAVE_SDA_IO,
        .scl_io_num = SLAVE_SCL_IO,
        .sda_pullup_en = GPIO_PULLUP_ENABLE,
        .scl_pullup_en = GPIO_PULLUP_ENABLE,
        .slave.addr_10bit_en = 0,
        .slave.slave_addr = SLAVE_ADDR
    };
    i2c_param_config(SLAVE_NUM, &i2c_config);
    i2c_driver_install(SLAVE_NUM, i2c_config.mode, 1024, 1024, 0);
}

void app_main(void) {
    i2c_slave_init();
    printf("Slave device initialized successfully\n");

    while (1) {
        printf("inside of while loop\n");
        uint8_t *data = (uint8_t *)malloc(I2C_SLAVE_RX_BUF_LEN);
        int size_luis;
        memset(data, 0, I2C_SLAVE_RX_BUF_LEN); // Clear the buffer before reading
        size_luis = i2c_slave_read_buffer(SLAVE_NUM, data, I2C_SLAVE_RX_BUF_LEN, 1000 / portTICK_PERIOD_MS);
        if (size_luis > 0) {
            if (true) {
                printf("second slave is guchi\n");
                break;
            }
            free(data); // Free the data after processing
        }
    }
}
