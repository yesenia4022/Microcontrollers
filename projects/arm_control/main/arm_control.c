#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/ledc.h"

// Define GPIO pin for servo control
#define SERVO_GPIO_1 GPIO_NUM_13

#define SERVO_MIN_PULSE_MS  0.5 // Minimum pulse width in milliseconds
#define SERVO_MAX_PULSE_MS  2.5 // Maximum pulse width in milliseconds

void setAngle(ledc_channel_t channel, int angle){
    int32_t duty = (10 + angle) * (1023) / 180 + 0;
    ledc_set_duty(LEDC_HIGH_SPEED_MODE, channel, duty);
    ledc_update_duty(LEDC_HIGH_SPEED_MODE, channel);
}

void configChannel(){
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
}

void set_servo_position(float angle_deg, ledc_channel_t channel) {
    // Convert angle to pulse width in milliseconds
    float pulse_ms = (angle_deg / 180.0) * (SERVO_MAX_PULSE_MS - SERVO_MIN_PULSE_MS) + SERVO_MIN_PULSE_MS;
    
    // Convert pulse width to PWM duty cycle
    uint32_t duty = (pulse_ms / 20.0) * ((1 << LEDC_TIMER_13_BIT) - 1);
    ledc_set_duty(LEDC_HIGH_SPEED_MODE, channel, duty);
    ledc_update_duty(LEDC_HIGH_SPEED_MODE, channel);
}

// void set_pwm_duty_cycle(float duty_ms) {
//     uint32_t duty = (duty_ms / 20.0) * ((1 << LEDC_TIMER_13_BIT) - 1);
//     ledc_set_duty(LEDC_HIGH_SPEED_MODE, LEDC_CHANNEL_0, duty);
//     ledc_update_duty(LEDC_HIGH_SPEED_MODE, LEDC_CHANNEL_0);
// }

void setDuty(ledc_channel_t channel){
    ledc_set_duty_and_update(LEDC_HIGH_SPEED_MODE, channel, 0, 0); // Start with 0% duty
    ledc_update_duty(LEDC_HIGH_SPEED_MODE, channel); // Update duty
}

void app_main() {
    // Configure timer
    ledc_timer_config_t timer_conf = {
        .duty_resolution = LEDC_TIMER_13_BIT, // Changed to 13-bit resolution for better servo control
        .freq_hz = 50,
        .speed_mode = LEDC_HIGH_SPEED_MODE,
        .timer_num = LEDC_TIMER_0,
        .clk_cfg = LEDC_AUTO_CLK,
    };
    ledc_timer_config(&timer_conf);

    // Configure channels
    configChannel();

    // Set up Fade
    ledc_fade_func_install(0);
    setDuty(LEDC_CHANNEL_0);

    // Rotate servo counterclockwise to 90 degrees
    set_servo_position(90, LEDC_CHANNEL_0);

    // Wait for some time
    vTaskDelay(3000 / portTICK_PERIOD_MS);

    // Rotate servo counterclockwise to 0 degrees
    set_servo_position(0, LEDC_CHANNEL_0);

    // Wait for some time
    vTaskDelay(3000 / portTICK_PERIOD_MS);

    // Stop PWM signal
    ledc_stop(LEDC_HIGH_SPEED_MODE, LEDC_CHANNEL_0, 0); // Stop PWM channel
    ledc_fade_func_uninstall(); // Uninstall LEDC fade functions
}

