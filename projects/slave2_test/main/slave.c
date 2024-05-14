#include <string.h>
#include <stdio.h>
#include "driver/i2c.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/ledc.h"

#define SLAVE_SCL_IO    22   // GPIO number for I2C slave clock
#define SLAVE_SDA_IO    21   // GPIO number for I2C slave data
#define SLAVE_NUM       I2C_NUM_0  // I2C port number for the slave device
#define SLAVE_ADDR      0x05  // I2C address for the slave device
#define COMPLETION_SIGNAL 0x01  // Signal to indicate completion
#define SERVO1_CONTROL_CMD 0x03  // Command to control the first servo
#define SERVO2_CONTROL_CMD 0x04  // Command to control the second servo
#define SERVO3_CONTROL_CMD 0x08  // Command to control the third servo
#define I2C_SLAVE_RX_BUF_LEN 1024

// Define GPIO pins for servo control
#define SERVO_GPIO_1 GPIO_NUM_13
#define SERVO_GPIO_2 GPIO_NUM_12

// Servo control configurations
#define SERVO_MIN_PULSE_MS  0.5 // Minimum pulse width in milliseconds
#define SERVO_MAX_PULSE_MS  2.5 // Maximum pulse width in milliseconds
#define LEDC_TIMER_13_BIT   13

volatile uint8_t task_status = 0;  // Add a global variable to hold the task status

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

void configChannels() {
    // Configure first channel
    ledc_channel_config_t ledc_conf_1 = {
        .gpio_num = SERVO_GPIO_1,
        .speed_mode = LEDC_HIGH_SPEED_MODE,
        .channel = LEDC_CHANNEL_0,
        .intr_type = LEDC_INTR_DISABLE,
        .timer_sel = LEDC_TIMER_0,
        .duty = 0,
        .hpoint = 0,
    };
    ledc_channel_config(&ledc_conf_1);

    // Configure second channel
    ledc_channel_config_t ledc_conf_2 = {
        .gpio_num = SERVO_GPIO_2,
        .speed_mode = LEDC_HIGH_SPEED_MODE,
        .channel = LEDC_CHANNEL_1,
        .intr_type = LEDC_INTR_DISABLE,
        .timer_sel = LEDC_TIMER_0,
        .duty = 0,
        .hpoint = 0,
    };
    ledc_channel_config(&ledc_conf_2);
}

void configTimer() {
    ledc_timer_config_t timer_conf = {
        .duty_resolution = LEDC_TIMER_13_BIT,
        .freq_hz = 50,
        .speed_mode = LEDC_HIGH_SPEED_MODE,
        .timer_num = LEDC_TIMER_0,
        .clk_cfg = LEDC_AUTO_CLK,
    };
    ledc_timer_config(&timer_conf);
}

void update_task_status(uint8_t status) {
    task_status = status;
    i2c_slave_write_buffer(SLAVE_NUM, &task_status, sizeof(task_status), portMAX_DELAY);
}

void setAngle(float angle_deg, ledc_channel_t channel) {
    float pulse_ms = (angle_deg / 180.0) * (SERVO_MAX_PULSE_MS - SERVO_MIN_PULSE_MS) + SERVO_MIN_PULSE_MS;
    uint32_t duty = (pulse_ms / 20.0) * ((1 << LEDC_TIMER_13_BIT) - 1);
    ledc_set_duty(LEDC_HIGH_SPEED_MODE, channel, duty);
    ledc_update_duty(LEDC_HIGH_SPEED_MODE, channel);
}

void setDuty(ledc_channel_t channel) {
    ledc_set_duty_and_update(LEDC_HIGH_SPEED_MODE, channel, 0, 0); // Start with 0% duty
    ledc_update_duty(LEDC_HIGH_SPEED_MODE, channel);
}

void setUpArm() {
    //setAngle(0, LEDC_CHANNEL_0);
    //vTaskDelay(100 / portTICK_PERIOD_MS);
    setAngle(90, LEDC_CHANNEL_0);
    vTaskDelay(100 / portTICK_PERIOD_MS);
}

void resetArm() {
    setAngle(0, LEDC_CHANNEL_0);
    vTaskDelay(1000 / portTICK_PERIOD_MS);
}

void clean(int cycles) {
    //setAngle(0, LEDC_CHANNEL_1);
    for (int i = 0; i < cycles; i++) {
        setAngle(180, LEDC_CHANNEL_1);
        //printf("CLEAN ANGLE 180");
        vTaskDelay(200 / portTICK_PERIOD_MS);
        setAngle(0, LEDC_CHANNEL_1); // Optionally reset position after moving
        //printf("CLEAN ANGLE 0");
        vTaskDelay(200 / portTICK_PERIOD_MS);
    }
}

void app_main(void) {
    printf("Before i2c_init()\n");
    i2c_slave_init();
    printf("Before configTimer()\n");
    configTimer();
    printf("Before configChannels()\n");
    configChannels();
    //printf("Setting Duty for Channel 0\n");
    setDuty(LEDC_CHANNEL_0);
    //printf("Setting Duty for Channel 1\n");
    setDuty(LEDC_CHANNEL_1);
    ledc_update_duty(LEDC_HIGH_SPEED_MODE, LEDC_CHANNEL_0);
    vTaskDelay(50 / portTICK_PERIOD_MS);
    ledc_update_duty(LEDC_HIGH_SPEED_MODE, LEDC_CHANNEL_1);
    vTaskDelay(50 / portTICK_PERIOD_MS);

    uint8_t *data = (uint8_t *)malloc(I2C_SLAVE_RX_BUF_LEN);
    printf("Before While Loop\n");
    while (1) {
        printf("Inside While Loop, Before memset()\n");
        memset(data, 0, I2C_SLAVE_RX_BUF_LEN);
        int size = i2c_slave_read_buffer(SLAVE_NUM, data, I2C_SLAVE_RX_BUF_LEN, 1000 / portTICK_PERIOD_MS);
        printf("Inside While Loop, before size check\n");
        if (size > 0) {
            switch (data[0]) {
                case SERVO1_CONTROL_CMD:
                    setUpArm(); // Only move the first servo
                    update_task_status(COMPLETION_SIGNAL);
                    printf("First servo operation completed and master notified\n");
                    break;
                case SERVO2_CONTROL_CMD:
                    clean(5); // Perform 5 cleaning cycles for the second servo
                    update_task_status(COMPLETION_SIGNAL);
                    printf("Second servo operation completed and master notified\n");
                    break;
                case SERVO3_CONTROL_CMD:
                    resetArm(); // Perform 5 cleaning cycles for the second servo
                    update_task_status(COMPLETION_SIGNAL);
                    printf("Third servo operation completed and master notified\n");
                    break;
            }
        }
        vTaskDelay(pdMS_TO_TICKS(100));
    }
    free(data);
}
