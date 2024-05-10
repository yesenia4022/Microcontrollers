#include <stdio.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/i2c.h"
#include "esp_log.h"

//Luis was here TODAY

// Motor 1 configuration
#define GPIO_MOTOR1_IN1 22
#define GPIO_MOTOR1_IN2 23
#define GPIO_MOTOR1_PWM 18 // Use one of the MCPWM pins for PWM
#define I2C_SLAVE_SDA_IO 21       // SDA pin is connected to SD1 of the multiplexer
#define I2C_SLAVE_SCL_IO 22       // SCL pin is connected to SC1 of the multiplexer
#define I2C_SLAVE_NUM I2C_NUM_0   // I2C port number for the slave device
#define I2C_SLAVE_FREQ_HZ 100000  // I2C slave clock frequency
#define I2C_SLAVE_ADDRESS 0x28    // The I2C address for the slave ESP32
>>>>>>> Stashed changes

static const char *TAG = "slave_device";

// Initialize I2C as a slave
void i2c_slave_init(void) {
    i2c_config_t conf = {
        .sda_io_num = I2C_SLAVE_SDA_IO,
        .scl_io_num = I2C_SLAVE_SCL_IO,
        .sda_pullup_en = GPIO_PULLUP_ENABLE,
        .scl_pullup_en = GPIO_PULLUP_ENABLE,
        .mode = I2C_MODE_SLAVE,
        .slave.addr_10bit_en = 0,
        .slave.slave_addr = I2C_SLAVE_ADDRESS,
    };
    ESP_ERROR_CHECK(i2c_param_config(I2C_SLAVE_NUM, &conf));
    ESP_ERROR_CHECK(i2c_driver_install(I2C_SLAVE_NUM, conf.mode, 1024, 1024, 0));
}

// Task to handle incoming I2C commands
void i2c_slave_task(void *args) {
    uint8_t data[1]; // Buffer for received data
    while (1) {
        int size = i2c_slave_read_buffer(I2C_SLAVE_NUM, data, sizeof(data), portMAX_DELAY);
        if (size == 0) {
            // No data received, loop again
            vTaskDelay(pdMS_TO_TICKS(10));
            continue;
        }
        // Log the received data
        ESP_LOGI(TAG, "Received data: 0x%X", data[0]);
        // Implement any actions based on the received data if necessary
        // For now, just log the reception
        // Add a delay to prevent a tight loop in case of continuous data.
        vTaskDelay(pdMS_TO_TICKS(10));
    }
}

void app_main(void) {
    ESP_LOGI(TAG, "Slave device initialization...");
    i2c_slave_init();

    ESP_LOGI(TAG, "Starting I2C slave task...");
    xTaskCreate(i2c_slave_task, "i2c_slave_task", 2048, NULL, 10, NULL);
}
