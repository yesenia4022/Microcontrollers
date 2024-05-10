#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "driver/i2c.h"
#include "driver/mcpwm.h"
#include "soc/mcpwm_periph.h"
#include "esp_timer.h"
#include <string.h>
// I2C configuration for the ESP32 used as I2C slave
#define I2C_SLAVE_SCL_IO             22  // GPIO number for I2C slave clock
#define I2C_SLAVE_SDA_IO             21  // GPIO number for I2C slave data
#define I2C_SLAVE_NUM                I2C_NUM_0 // I2C port number for the slave device
#define I2C_SLAVE_TX_BUF_LEN         1024 // I2C slave tx buffer size
#define I2C_SLAVE_RX_BUF_LEN         1024 // I2C slave rx buffer size
#define I2C_SLAVE_ADDR               0x04  // I2C address for the slave device

#define SLAVE_REG_STATUS 0x01

// Motor Control Configuration
#define GPIO_MOTOR1_IN1              25  // Motor 1 IN1 pin
#define GPIO_MOTOR1_IN2              27  // Motor 1 IN2 pin //not used
#define GPIO_MOTOR1_PWM              26  // Motor 1 PWM pin (must support MCPWM)
#define GPIO_MOTOR2_IN1              14  // Motor 2 IN1 pin
#define GPIO_MOTOR2_IN2              12  // Motor 2 IN2 pin //not used
#define GPIO_MOTOR2_PWM              13  // Motor 2 PWM pin (must support MCPWM)
#define MOTOR_START_CMD              0x00  // Command to start the motors
#define MOTOR_STOP_CMD               0x02  // Command to stop the motors

// Ultrasonic Sensor Configuration
#define TRIG_PIN 5
#define ECHO_PIN 18
#define SPEED_OF_SOUND 343.0 // The speed of sound in m/s

// Global flag to indicate whether the motor should be running
volatile bool motor_should_run = false;
volatile bool motors_running = false; 

volatile uint8_t sensor_status = 0;  // 0: no obstacle, 1: obstacle detected
volatile uint8_t task_status = 0; 

// Setup I2C as a slave
void i2c_slave_init() {
    i2c_config_t conf = {
        .mode = I2C_MODE_SLAVE,
        .sda_io_num = I2C_SLAVE_SDA_IO,
        .scl_io_num = I2C_SLAVE_SCL_IO,
        .sda_pullup_en = GPIO_PULLUP_ENABLE,
        .scl_pullup_en = GPIO_PULLUP_ENABLE,
        .slave.addr_10bit_en = 0,
        .slave.slave_addr = I2C_SLAVE_ADDR,
    };
    ESP_ERROR_CHECK(i2c_param_config(I2C_SLAVE_NUM, &conf));
    ESP_ERROR_CHECK(i2c_driver_install(I2C_SLAVE_NUM, conf.mode, 1024, 1024, 0));
    i2c_slave_write_buffer(I2C_SLAVE_NUM, &task_status, sizeof(task_status), portMAX_DELAY);
}

// Initialize GPIOs for both motors
static void mcpwm_example_gpio_initialize1(void) {
    printf("Initializing MCPWM motor1 control GPIO...\n");

    // Motor 1
    gpio_set_direction(GPIO_MOTOR1_IN1, GPIO_MODE_OUTPUT);
    gpio_set_direction(GPIO_MOTOR1_IN2, GPIO_MODE_OUTPUT);
    mcpwm_gpio_init(MCPWM_UNIT_0, MCPWM0A, GPIO_MOTOR1_PWM);

}

static void mcpwm_example_gpio_initialize2(void) {
    printf("Initializing MCPWM motor2 control GPIO...\n");

    // Motor 2
    gpio_set_direction(GPIO_MOTOR2_IN1, GPIO_MODE_OUTPUT);
    gpio_set_direction(GPIO_MOTOR2_IN2, GPIO_MODE_OUTPUT);
    mcpwm_gpio_init(MCPWM_UNIT_1, MCPWM1B, GPIO_MOTOR2_PWM); 
}

// Handler to process I2C events
static esp_err_t i2c_slave_read_event_handler(i2c_port_t i2c_num, uint8_t* data, size_t size) {
    uint8_t reg_address = data[0];  // Assume first byte is the register address
    switch (reg_address) {
        case SLAVE_REG_STATUS:
            data[1] = sensor_status;  // Write the status to the data buffer
            return ESP_OK;
        default:
            return ESP_ERR_INVALID_ARG;
    }
}

// Function to update task status and write it to I2C buffer
void update_task_status(uint8_t status) {
    task_status = status;
    i2c_slave_write_buffer(I2C_SLAVE_NUM, &task_status, sizeof(task_status), portMAX_DELAY);
}

// Initialize ultrasonic sensor and GPIOs
void ultrasonic_init() {
    gpio_set_direction(TRIG_PIN, GPIO_MODE_OUTPUT);
    gpio_set_direction(ECHO_PIN, GPIO_MODE_INPUT);
    gpio_set_level(TRIG_PIN, 0);
}

double get_distance() {
    int64_t start_time = 0;
    int64_t end_time = 0;
    int64_t time_to_wait = 1000000L; // 1 second in microseconds

    // Ensure the trigger pin is low before starting the process
    gpio_set_level(TRIG_PIN, 0);
    vTaskDelay(pdMS_TO_TICKS(2));

    // Send trig pulse
    gpio_set_level(TRIG_PIN, 1);
    esp_rom_delay_us(10); // Send a 10-microsecond pulse
    gpio_set_level(TRIG_PIN, 0);

    // Wait for echo high
    int64_t max_wait = esp_timer_get_time() + time_to_wait;
    while(gpio_get_level(ECHO_PIN) == 0 && esp_timer_get_time() < max_wait) {
        // Waiting for echo
    }
    if(esp_timer_get_time() < max_wait) {
        start_time = esp_timer_get_time();
    } else {
        printf("Echo signal high wait timed out\n");
        return -1;
    }

    // Wait for echo low
    max_wait = esp_timer_get_time() + time_to_wait;
    while(gpio_get_level(ECHO_PIN) == 1 && esp_timer_get_time() < max_wait) {
        // Waiting for echo to end
    }
    if(esp_timer_get_time() < max_wait) {
        end_time = esp_timer_get_time();
    } else {
        printf("Echo signal low wait timed out\n");
        return -1;
    }

    // Calculate distance
    double distance = (double)(end_time - start_time) * SPEED_OF_SOUND / (2 * 1000000); // Convert from us to seconds

    printf("Measured distance: %.2f meters\n", distance);

    return distance;
}

void brushed_motor1_forward(float duty_cycle) {
    printf("Motor 1 forward at %f%% duty cycle\n", duty_cycle);
    gpio_set_level(GPIO_MOTOR1_IN1, 1); // Set IN1 to high
    gpio_set_level(GPIO_MOTOR1_IN2, 1); // Set IN2 to low
    mcpwm_set_duty(MCPWM_UNIT_0, MCPWM_TIMER_0, MCPWM_OPR_A, duty_cycle);
    mcpwm_set_duty_type(MCPWM_UNIT_0, MCPWM_TIMER_0, MCPWM_OPR_A, MCPWM_DUTY_MODE_0);
}

void brushed_motor2_forward(float duty_cycle) {
    printf("Motor 2 forward at %f%% duty cycle\n", duty_cycle);
    gpio_set_level(GPIO_MOTOR2_IN1, 1); // Set IN1 to high
    gpio_set_level(GPIO_MOTOR2_IN2, 1); // Set IN2 to low
    mcpwm_set_duty(MCPWM_UNIT_1, MCPWM_TIMER_1, MCPWM_OPR_B, duty_cycle);
    mcpwm_set_duty_type(MCPWM_UNIT_1, MCPWM_TIMER_1, MCPWM_OPR_B, MCPWM_DUTY_MODE_0);
}

void brushed_motor_stop(mcpwm_unit_t mcpwm_num, mcpwm_timer_t timer_num) {
    printf("Stopping motors\n");
    // Stop Motor 1
    gpio_set_level(GPIO_MOTOR1_IN1, 0);
    gpio_set_level(GPIO_MOTOR1_IN2, 0);
    mcpwm_set_signal_low(mcpwm_num, timer_num, MCPWM_OPR_A);

    // Stop Motor 2
    gpio_set_level(GPIO_MOTOR2_IN1, 0);
    gpio_set_level(GPIO_MOTOR2_IN2, 0);
    mcpwm_set_signal_low(mcpwm_num, timer_num, MCPWM_OPR_B);
}
//uint8_t *data = (uint8_t *)malloc(I2C_SLAVE_RX_BUF_LEN);
//int size;

void app_main() {
    i2c_slave_init();
    ultrasonic_init();
    mcpwm_example_gpio_initialize1();
    mcpwm_example_gpio_initialize2();
    printf("I2C slave and Ultrasonic sensor initialized successfully\n");

    while (1) {
        printf("inside of while loop\n");
        uint8_t *data = (uint8_t *)malloc(I2C_SLAVE_RX_BUF_LEN);
        int size_luis;
        bool motors_running = false; // This flag will be set true when motors are supposed to be running
        memset(data, 0, I2C_SLAVE_RX_BUF_LEN); // Clear the buffer before reading
        size_luis = i2c_slave_read_buffer(I2C_SLAVE_NUM, data, I2C_SLAVE_RX_BUF_LEN, 1000 / portTICK_PERIOD_MS);

        if (size_luis > 0) {
            if (data[0] == MOTOR_START_CMD) {
                motor_should_run = true; // Enable motor control
                brushed_motor1_forward(80.0); // Start Motor 1 with 80% duty cycle
                brushed_motor2_forward(100.0); // Start Motor 2 with 100% duty cycle
                motors_running = true;
                printf("Motors started!\n");
            }
            free(data); // Free the data after processing
        }
        double distance = get_distance();
        if (distance < 0.5) {
            printf("inside while then if\n");
            if (motors_running) {
                printf("insdie of motor if\n");
                brushed_motor_stop(MCPWM_UNIT_0, MCPWM_TIMER_0);
                brushed_motor_stop(MCPWM_UNIT_1, MCPWM_TIMER_1);
                motors_running = false;
                motor_should_run = false;
                update_task_status(1);  // Update status to indicate obstacle detected
                printf("Motors stopped due to obstacle.\n");
            }
        } else {
            if (!motors_running && motor_should_run) {
                brushed_motor1_forward(80.0);
                brushed_motor2_forward(100.0);
                motors_running = true;
                update_task_status(0);  // Update status to indicate no obstacle
                printf("Motors are moving\n");
            }
        }
        vTaskDelay(pdMS_TO_TICKS(100));
    }
    printf("out side of while loop\n");
}