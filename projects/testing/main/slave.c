#include <stdio.h>
#include <string.h>
#include "driver/i2c.h"
#include "esp_log.h"
#include "sdkconfig.h"
#include "esp_timer.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "driver/mcpwm.h"
#include "soc/mcpwm_periph.h"

#define I2C_SLAVE_SCL_IO             22  /*!< gpio number for I2C slave clock */
#define I2C_SLAVE_SDA_IO             21  /*!< gpio number for I2C slave data */
#define I2C_SLAVE_NUM                I2C_NUM_0 /*!< I2C port number for slave dev */
#define I2C_SLAVE_TX_BUF_LEN         1024 /*!< I2C slave tx buffer size */
#define I2C_SLAVE_RX_BUF_LEN         1024 /*!< I2C slave rx buffer size */
#define I2C_SLAVE_ADDR               0x04  /*!< I2C slave address, 7-bit */
#define MOTOR_START_CMD              0x00  /*!< Command to start the motors */
#define TRIG_PIN 5
#define ECHO_PIN 27
#define SPEED_OF_SOUND 343.0 // The speed of sound in m/s

// Global flag to indicate whether the motor should be running
volatile bool motor_should_run = false;
volatile bool motors_running = false; 

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

void ultrasonic_init() {
    gpio_set_direction(TRIG_PIN, GPIO_MODE_OUTPUT);
    gpio_set_direction(ECHO_PIN, GPIO_MODE_INPUT);
    gpio_set_level(TRIG_PIN, 0);
}

double get_distance() {
    // Send trig pulse
    gpio_set_level(TRIG_PIN, 1);
    esp_rom_delay_us(10); // Send a 10-microsecond pulse
    gpio_set_level(TRIG_PIN, 0);

    // Wait for echo
    while(gpio_get_level(ECHO_PIN) == 0) {
        ; // Loop until the echo starts
    }
    int64_t start_time = esp_timer_get_time();

    while(gpio_get_level(ECHO_PIN) == 1) {
        ; // Loop until the echo ends
    }
    int64_t end_time = esp_timer_get_time();

    // Calculate distance
    double distance = (double)(end_time - start_time) * SPEED_OF_SOUND / (2 * 1000000); // Convert from us to seconds

    return distance;
}

void start_motors() {
    printf("Motors started!\n");
    // Here you would add the code to physically start the motors.
}

void stop_motors() {
    printf("Motors stopped!\n");
    // Here you would add the code to physically stop the motors.
}

void app_main() {
    i2c_slave_init();
    ultrasonic_init();
    printf("I2C slave and Ultrasonic sensor initialized successfully\n");

    uint8_t *data = (uint8_t *)malloc(I2C_SLAVE_RX_BUF_LEN);
    int size;

    bool motors_running = false; // Add this line to declare the variable

    while (1) {
        memset(data, 0, I2C_SLAVE_RX_BUF_LEN); // Clear the buffer before reading
        size = i2c_slave_read_buffer(I2C_SLAVE_NUM, data, I2C_SLAVE_RX_BUF_LEN, 1000 / portTICK_PERIOD_MS);

        if (size > 0) {
            if (data[0] == MOTOR_START_CMD) {
                printf("Received start motor command\n");
                motor_should_run = true; // Enable motor control
            }
        }

        // Motor control logic
        if (motor_should_run) {
            double distance = get_distance();
            if (distance < 0.5) {
                if (motors_running) {
                    stop_motors();
                    motors_running = false; // Update state
                    printf("Motors stopped due to obstacle.\n");
                }
            } else {
                if (!motors_running) {
                    start_motors();
                    motors_running = true; // Update state
                }
                printf("Motors are moving!\n"); // This line will now always print when motors are supposed to run
            }
        }
        // Reduced delay for faster response
        // Be cautious with values lower than 10ms
        //vTaskDelay(pdMS_TO_TICKS(50));
        vTaskDelay(pdMS_TO_TICKS(100)); // Delay to prevent a busy loop
    }

    free(data);
}