#include <string.h>
#include <stdio.h>
#include "driver/i2c.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/ledc.h"

#define SLAVE_SCL_IO    22
#define SLAVE_SDA_IO    21
#define SLAVE_NUM       I2C_NUM_0
#define SLAVE_ADDR      0x06
#define COMPLETION_SIGNAL 0x01
#define CAMERA_START_CMD 0x05
#define I2C_SLAVE_RX_BUF_LEN 1024

volatile uint8_t task_status = 0;

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

void update_task_status(uint8_t status) {
    task_status = status;
    i2c_slave_write_buffer(SLAVE_NUM, &task_status, sizeof(task_status), portMAX_DELAY);
}

void delay_and_update() {
    for (int i = 1; i <= 10; i++) {
        printf("%d sec has passed\n", i);
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
    update_task_status(COMPLETION_SIGNAL);
    printf("Action completed, status updated.\n");
}

void app_main(void) {
    i2c_slave_init();
    printf("Slave device initialized successfully\n");

    while (1) {
        uint8_t *data = (uint8_t *)malloc(I2C_SLAVE_RX_BUF_LEN);
        memset(data, 0, I2C_SLAVE_RX_BUF_LEN);
        int size = i2c_slave_read_buffer(SLAVE_NUM, data, I2C_SLAVE_RX_BUF_LEN, 1000 / portTICK_PERIOD_MS);

        if (size > 0 && data[0] == CAMERA_START_CMD) {
            printf("CAMERA START command received, performing action...\n");
            delay_and_update();  // This is called only after receiving the CAMERA_START_CMD
        }
        free(data);  // free the data
    }
}