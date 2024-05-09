#include <stdio.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/i2c.h"
#include "esp_log.h"
#include "driver/gpio.h"
#include "sdkconfig.h"

#define I2C_SLAVE_SDA_IO    21  // GPIO21 connected to SDA on the multiplexer
#define I2C_SLAVE_SCL_IO    22  // GPIO22 connected to SCL on the multiplexer
#define I2C_SLAVE_NUM       I2C_NUM_0
#define I2C_SLAVE_FREQ_HZ   100000
#define I2C_SLAVE_ADDRESS   0x28  // Adjust if needed based on A0 pin setting
#define BLINK_GPIO          2     // GPIO number for LED
#define A0_PIN              19    // GPIO for A0 on the multiplexer

static const char *TAG = "i2c_slave_example";

// Initialize I2C as a slave device
void i2c_slave_init(void) {
    i2c_config_t i2c_config = {
        .sda_io_num = I2C_SLAVE_SDA_IO,
        .scl_io_num = I2C_SLAVE_SCL_IO,
        .sda_pullup_en = GPIO_PULLUP_ENABLE,
        .scl_pullup_en = GPIO_PULLUP_ENABLE,
        .mode = I2C_MODE_SLAVE,
        .slave = {
            .slave_addr = I2C_SLAVE_ADDRESS,
            .addr_10bit_en = 0,
        }
    };
    i2c_param_config(I2C_SLAVE_NUM, &i2c_config);
    i2c_driver_install(I2C_SLAVE_NUM, i2c_config.mode, 0, 0, 0);
}

void blink_gpio_init(void) {
    gpio_reset_pin(BLINK_GPIO);
    gpio_set_direction(BLINK_GPIO, GPIO_MODE_OUTPUT);
}

void i2c_slave_task(void *args) {
    uint8_t *data = (uint8_t *)malloc(128);
    if (!data) {
        ESP_LOGE(TAG, "Failed to allocate memory for receive buffer");
        vTaskDelete(NULL);
        return;
    }

    while (1) {
        memset(data, 0, 128);
        int size = i2c_slave_read_buffer(I2C_SLAVE_NUM, data, 128, pdMS_TO_TICKS(1000));
        if (size > 0) {
            ESP_LOGI(TAG, "Received data [%d]: %.*s", size, size, data);
            gpio_set_level(BLINK_GPIO, 1);   // Turn on the LED
            vTaskDelay(pdMS_TO_TICKS(100));  // Keep the LED on for 100ms
            gpio_set_level(BLINK_GPIO, 0);   // Turn off the LED
        }

        vTaskDelay(pdMS_TO_TICKS(10));
    }

    free(data);
    vTaskDelete(NULL);
}

void setup_a0_pin(void) {
    gpio_pad_select_gpio(A0_PIN);
    gpio_set_direction(A0_PIN, GPIO_MODE_OUTPUT);
    gpio_set_level(A0_PIN, 0); // Set A0 to high or low based on your configuration
}

void app_main(void) {
    ESP_LOGI(TAG, "Initializing I2C slave example");
    
    setup_a0_pin();  // Set up the A0 pin before initializing I2C
    blink_gpio_init();  // Initialize the GPIO for the LED
    i2c_slave_init();   // Set up the ESP32 as an I2C slave device

    // Create a task that will handle incoming I2C commands and blink an LED
    xTaskCreate(i2c_slave_task, "i2c_slave_task", 2048, NULL, 10, NULL);
}
