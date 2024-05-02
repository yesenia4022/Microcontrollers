#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/ledc.h"

// Define GPIO pin for servo control
#define SERVO_GPIO_1 GPIO_NUM_13
#define SERVO_GPIO_2 GPIO_NUM_12

#define SERVO_MIN_PULSE_MS  0.5 // Minimum pulse width in milliseconds
#define SERVO_MAX_PULSE_MS  2.5 // Maximum pulse width in milliseconds


// --------------------------------------------------------------------------------------------------------------


void configChannels(){
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


void configTimer(){
    ledc_timer_config_t timer_conf = {
        .duty_resolution = LEDC_TIMER_13_BIT, // Changed to 13-bit resolution for better servo control
        .freq_hz = 50,
        .speed_mode = LEDC_HIGH_SPEED_MODE,
        .timer_num = LEDC_TIMER_0,
        .clk_cfg = LEDC_AUTO_CLK,
    };
    ledc_timer_config(&timer_conf);
}


void setAngle(float angle_deg, ledc_channel_t channel) {
    // Convert angle to pulse width in milliseconds
    float pulse_ms = (angle_deg / 180.0) * (SERVO_MAX_PULSE_MS - SERVO_MIN_PULSE_MS) + SERVO_MIN_PULSE_MS;
    
    // Convert pulse width to PWM duty cycle
    uint32_t duty = (pulse_ms / 20.0) * ((1 << LEDC_TIMER_13_BIT) - 1);
    ledc_set_duty(LEDC_HIGH_SPEED_MODE, channel, duty);
    ledc_update_duty(LEDC_HIGH_SPEED_MODE, channel);
}


void setDuty(ledc_channel_t channel){
    ledc_set_duty_and_update(LEDC_HIGH_SPEED_MODE, channel, 0, 0); // Start with 0% duty
    ledc_update_duty(LEDC_HIGH_SPEED_MODE, channel); // Update duty
}


// Move Arm Segment 1 Down to Table
void setUpArm(){
    setAngle(90, LEDC_CHANNEL_0);
    vTaskDelay(1000 / portTICK_PERIOD_MS);
}


// Reset Arm Segment 1
void resetArm(){
    setAngle(0, LEDC_CHANNEL_0);
    vTaskDelay(1000 / portTICK_PERIOD_MS);
}


// Arm Segment 2 Begins Cleaning
void clean(int cycles){
    for (int i = 0; i < cycles; i++){
        // Swipe from Left -> Right (then return to position)
        setAngle(180, LEDC_CHANNEL_1);
        // General Delay
        vTaskDelay(200 / portTICK_PERIOD_MS);
    }
}


// void set_pwm_duty_cycle(float duty_ms) {
//     uint32_t duty = (duty_ms / 20.0) * ((1 << LEDC_TIMER_13_BIT) - 1);
//     ledc_set_duty(LEDC_HIGH_SPEED_MODE, LEDC_CHANNEL_0, duty);
//     ledc_update_duty(LEDC_HIGH_SPEED_MODE, LEDC_CHANNEL_0);
// }



// --------------------------------------------------------------------------------------------------------------



void app_main() {
    // Configure timer
    configTimer();

    // Configure channels
    configChannel();

    // Set up Fade
    ledc_fade_func_install(0);
    setDuty(LEDC_CHANNEL_0);
    setDuty(LEDC_CHANNEL_1);
    
    // Rotate servo counterclockwise to 90 degrees
    setAngle(90, LEDC_CHANNEL_0);

    // Wait for some time
    vTaskDelay(3000 / portTICK_PERIOD_MS);

    // Rotate servo counterclockwise to 0 degrees
    setAngle(0, LEDC_CHANNEL_1);

    // Wait for some time
    vTaskDelay(3000 / portTICK_PERIOD_MS);

    setAngle(180, LEDC_CHANNEL_1);

    vTaskDelay(3000 / portTICK_PERIOD_MS);
    // Stop PWM signal
    ledc_stop(LEDC_HIGH_SPEED_MODE, LEDC_CHANNEL_0, 0); // Stop PWM channel
    ledc_stop(LEDC_HIGH_SPEED_MODE, LEDC_CHANNEL_1, 0);
    ledc_fade_func_uninstall(); // Uninstall LEDC fade functions
}

