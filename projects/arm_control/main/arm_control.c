// #include <stdio.h>
// #include "freertos/FreeRTOS.h"
// #include "freertos/task.h"
// #include "driver/gpio.h"
// #include "driver/ledc.h"

// #define GPIO_SERVO_PIN  GPIO_NUM_2  // Example GPIO pin number (you can change this as needed)
// #define LEDC_TIMER        LEDC_TIMER_0
// #define LEDC_CHANNEL      LEDC_CHANNEL_0
// #define LEDC_DUTY_RESOLUTION   LEDC_TIMER_13_BIT  // 13-bit resolution for duty cycle
// #define PWM_FREQUENCY     50  // PWM frequency in Hz (typical for servo motors)

// void configure_gpio_output() {
//     esp_rom_gpio_pad_select_gpio(GPIO_SERVO_PIN);  // Select the GPIO pin
//     gpio_set_direction(GPIO_SERVO_PIN, GPIO_MODE_OUTPUT);  // Set the GPIO pin as an output
// }

// void configure_pwm_output() {
//     // Configure LEDC timer and channel
//     ledc_timer_config_t ledc_timer = {
//         .duty_resolution = LEDC_DUTY_RESOLUTION,
//         .freq_hz = PWM_FREQUENCY,
//         .speed_mode = LEDC_HIGH_SPEED_MODE,
//         .timer_num = LEDC_TIMER
//     };
//     ledc_timer_config(&ledc_timer);

//     ledc_channel_config_t ledc_channel = {
//         .channel = LEDC_CHANNEL,
//         .duty = 0,
//         .gpio_num = GPIO_SERVO_PIN,
//         .speed_mode = LEDC_HIGH_SPEED_MODE,
//         .timer_sel = LEDC_TIMER
//     };
//     ledc_channel_config(&ledc_channel);
// }

// void set_servo_angle(int angle) {
//     int duty = (angle * 8191) / 180;  // Calculate duty cycle based on angle (0-180 degrees)
//     ledc_set_duty(LEDC_HIGH_SPEED_MODE, LEDC_CHANNEL, duty);
//     ledc_update_duty(LEDC_HIGH_SPEED_MODE, LEDC_CHANNEL);
// }

// void app_main() {
//     configure_pwm_output();  // Initialize PWM configuration for servo

//     while(1) {
//         ;
//     }
// }

#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/ledc.h"

// Define GPIO pin for servo control
#define SERVO_GPIO GPIO_NUM_2

void app_main() {
    // Configure the LEDC peripheral
    ledc_timer_config_t timer_conf = {
        .duty_resolution = LEDC_TIMER_12_BIT, // resolution of PWM duty
        .freq_hz = 50,                         // frequency of PWM signal
        .speed_mode = LEDC_HIGH_SPEED_MODE,    // timer mode
        .timer_num = LEDC_TIMER_0,             // timer index
        .clk_cfg = LEDC_AUTO_CLK,              // Auto select the source clock
    };
    ledc_timer_config(&timer_conf);

    ledc_channel_config_t ledc_conf = {
        .gpio_num = SERVO_GPIO,                // GPIO pin for PWM output
        .speed_mode = LEDC_HIGH_SPEED_MODE,    // LEDC speed mode
        .channel = LEDC_CHANNEL_0,             // LEDC channel
        .intr_type = LEDC_INTR_DISABLE,        // disable interrupt
        .timer_sel = LEDC_TIMER_0,             // timer index
        .duty = 0,                             // initial duty cycle
        .hpoint = 0,                           // hpoint value
    };
    ledc_channel_config(&ledc_conf);

    // Set the servo angle continuously
    while (1) {
        // Move the servo from 0 to 180 degrees
        for (int angle = 0; angle <= 180; angle++) {
            uint32_t duty = (10 + angle) * (1023 - 0) / 180 + 0; // Map angle to duty cycle (10-170 degrees)
            ledc_set_duty(LEDC_HIGH_SPEED_MODE, LEDC_CHANNEL_0, duty);
            ledc_update_duty(LEDC_HIGH_SPEED_MODE, LEDC_CHANNEL_0);
            vTaskDelay(10 / portTICK_PERIOD_MS); // Delay for smooth movement
        }

        // Move the servo from 180 to 0 degrees
        for (int angle = 180; angle >= 0; angle--) {
            uint32_t duty = (10 + angle) * (1023 - 0) / 180 + 0; // Map angle to duty cycle (10-170 degrees)
            ledc_set_duty(LEDC_HIGH_SPEED_MODE, LEDC_CHANNEL_0, duty);
            ledc_update_duty(LEDC_HIGH_SPEED_MODE, LEDC_CHANNEL_0);
            vTaskDelay(10 / portTICK_PERIOD_MS); // Delay for smooth movement
        }
    }

    // Stop the PWM (This code is unreachable in this example as the loop runs continuously)
    ledc_stop(LEDC_HIGH_SPEED_MODE, LEDC_CHANNEL_0, 0); // Set idle_level to 0 (low) after stopping PWM
}

