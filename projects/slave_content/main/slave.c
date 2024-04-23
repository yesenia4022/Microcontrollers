#include "driver/i2c.h"
#include "esp_log.h"
#include "sdkconfig.h"
#include <stdio.h>
#include <string.h> // Include string.h for memset

#define I2C_SLAVE_SCL_IO             22  /*!< gpio number for I2C slave clock */
#define I2C_SLAVE_SDA_IO             21  /*!< gpio number for I2C slave data */
#define I2C_SLAVE_NUM                I2C_NUM_0 /*!< I2C port number for slave dev */
#define I2C_SLAVE_TX_BUF_LEN         1024 /*!< I2C slave tx buffer size */
#define I2C_SLAVE_RX_BUF_LEN         1024 /*!< I2C slave rx buffer size */
#define I2C_SLAVE_ADDR               0x04  /*!< I2C slave address, 7-bit */
#define MOTOR_START_CMD              0x00  /*!< Command to start the motors */

void i2c_slave_init() {
    i2c_config_t conf_slave = {
        .sda_io_num = I2C_SLAVE_SDA_IO,
        .scl_io_num = I2C_SLAVE_SCL_IO,
        .sda_pullup_en = GPIO_PULLUP_ENABLE,
        .scl_pullup_en = GPIO_PULLUP_ENABLE,
        .mode = I2C_MODE_SLAVE,
        .slave.addr_10bit_en = 0,
        .slave.slave_addr = I2C_SLAVE_ADDR,
    };
    ESP_ERROR_CHECK(i2c_param_config(I2C_SLAVE_NUM, &conf_slave));
    ESP_ERROR_CHECK(i2c_driver_install(I2C_SLAVE_NUM, conf_slave.mode,
                                       I2C_SLAVE_RX_BUF_LEN,
                                       I2C_SLAVE_TX_BUF_LEN, 0));
}

void start_motors() {
    // Code to activate the motors.
    printf("Motors started!\n");
    // Actual motor control code should be here
}

void app_main() {
    i2c_slave_init();
    printf("I2C slave initialized successfully\n");

    uint8_t *data = (uint8_t *)malloc(I2C_SLAVE_RX_BUF_LEN);
    int size;

    while (1) {
        printf("Waiting for I2C data...\n");
        memset(data, 0, I2C_SLAVE_RX_BUF_LEN); // Clear the buffer before reading
        // Attempt to read with a timeout instead of portMAX_DELAY
        size = i2c_slave_read_buffer(I2C_SLAVE_NUM, data, I2C_SLAVE_RX_BUF_LEN, 1000 / portTICK_PERIOD_MS);
        if (size > 0) {
            printf("Buffer received: ");
            for (int i = 0; i < size; i++) {
                printf("%02X ", data[i]);
            }
            printf("\n");

            if (data[0] == MOTOR_START_CMD) {
                printf("Received start motor command\n");
                start_motors();
            } else {
                printf("Received unknown command: %02X\n", data[0]);
            }
        } else if (size == 0) {
            printf("I2C buffer empty, no data received\n");
        } else {
            printf("I2C read error: %d\n", size);
        }
        vTaskDelay(pdMS_TO_TICKS(100)); // Add a small delay to prevent a busy loop
    }

    free(data);
}