#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "driver/mcpwm.h"
#include "soc/mcpwm_periph.h"

//Luis was here

// Motor 1 configuration
#define GPIO_MOTOR1_IN1 22
#define GPIO_MOTOR1_IN2 23
#define GPIO_MOTOR1_PWM 18 // Use one of the MCPWM pins for PWM

// Motor 2 configuration
#define GPIO_MOTOR2_IN1 21 
#define GPIO_MOTOR2_IN2 25 
#define GPIO_MOTOR2_PWM 19 // Changed to a pin that supports output and MCPWM if available

// Initialize GPIOs for both motors
static void mcpwm_example_gpio_initialize1(void) {
    printf("Initializing MCPWM motor1 control GPIO...\n");

    // Motor 1
    gpio_set_direction(GPIO_MOTOR1_IN1, GPIO_MODE_OUTPUT);
    gpio_set_direction(GPIO_MOTOR1_IN2, GPIO_MODE_OUTPUT);
    mcpwm_gpio_init(MCPWM_UNIT_0, MCPWM0A, GPIO_MOTOR1_PWM); // Set GPIO 18 as PWM0A for Motor 1

}
static void mcpwm_example_gpio_initialize2(void) {
    printf("Initializing MCPWM motor2 control GPIO...\n");

    // Motor 2
    gpio_set_direction(GPIO_MOTOR2_IN1, GPIO_MODE_OUTPUT);
    gpio_set_direction(GPIO_MOTOR2_IN2, GPIO_MODE_OUTPUT);
    mcpwm_gpio_init(MCPWM_UNIT_1, MCPWM1B, GPIO_MOTOR2_PWM); // Set GPIO 25 as PWM1A for Motor 2
}

// Function to control motor 1 direction and speed
static void brushed_motor1_forward(mcpwm_unit_t mcpwm_num, mcpwm_timer_t timer_num, float duty_cycle) {
    gpio_set_level(GPIO_MOTOR1_IN1, 1); // Set IN1 to high
    gpio_set_level(GPIO_MOTOR1_IN2, 0); // Set IN2 to low
    mcpwm_set_duty(mcpwm_num, timer_num, MCPWM_OPR_A, duty_cycle);
    mcpwm_set_duty_type(mcpwm_num, timer_num, MCPWM_OPR_A, MCPWM_DUTY_MODE_0);
}

// Function to control motor 2 direction and speed
static void brushed_motor2_forward(mcpwm_unit_t mcpwm_num, mcpwm_timer_t timer_num, float duty_cycle) {
    gpio_set_level(GPIO_MOTOR2_IN1, 1); // Set IN1 to high
    gpio_set_level(GPIO_MOTOR2_IN2, 0); // Set IN2 to low
    mcpwm_set_duty(mcpwm_num, timer_num, MCPWM_OPR_B, duty_cycle); // Use MCPWM_OPR_B for Motor 2
    mcpwm_set_duty_type(mcpwm_num, timer_num, MCPWM_OPR_B, MCPWM_DUTY_MODE_0);
}

void app_main(void) {
    // Initialize MCPWM GPIO for both motors
    mcpwm_example_gpio_initialize1();
    mcpwm_example_gpio_initialize2();

    // Configure MCPWM for both motors
    mcpwm_config_t pwm_config = {
        .frequency = 1000,  // frequency = 1kHz
        .cmpr_a = 0,        // duty cycle of PWMxA = 0% for Motor 1
        .cmpr_b = 0,        // duty cycle of PWMxB = 0% for Motor 2
        .counter_mode = MCPWM_UP_COUNTER,
        .duty_mode = MCPWM_DUTY_MODE_0
    };
    mcpwm_init(MCPWM_UNIT_0, MCPWM_TIMER_0, &pwm_config); // Initialize PWM0A for Motor 1
    mcpwm_init(MCPWM_UNIT_1, MCPWM_TIMER_1, &pwm_config); // Initialize PWM1A for Motor 2

    // Main loop
    while (1) {
        // Run motor 1 forward at 60% duty cycle
        brushed_motor1_forward(MCPWM_UNIT_0, MCPWM_TIMER_0, 80.0);

        // Run motor 2 forward at 100% duty cycle
        brushed_motor2_forward(MCPWM_UNIT_1, MCPWM_TIMER_1, 100.0);
        
        vTaskDelay(2000 / portTICK_PERIOD_MS); // Forward for 2000ms

        // Stop motor 1
        mcpwm_set_signal_low(MCPWM_UNIT_0, MCPWM_TIMER_0, MCPWM_OPR_A);

        // Stop motor 2
        mcpwm_set_signal_low(MCPWM_UNIT_1, MCPWM_TIMER_1, MCPWM_OPR_B);
        
        vTaskDelay(2000 / portTICK_PERIOD_MS); // Stop for 2000ms
    }
}

