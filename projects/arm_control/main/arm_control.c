#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/ledc.h"

// Define GPIO pin for servo control
#define SERVO_GPIO_1 GPIO_NUM_13
#define SERVO_GPIO_2 GPIO_NUM_14


void setAngle(ledc_channel_t channel, int angle){
    int32_t duty = (10 + angle) * (1023 - 0) / 180 + 0;
    // printf("Setting angle %d, duty cycle %ld\n", angle, duty);  // Debug output
    ledc_set_duty(LEDC_HIGH_SPEED_MODE, channel, duty);
    ledc_update_duty(LEDC_HIGH_SPEED_MODE, channel);
}

void app_main() {
    //Configure timer
    ledc_timer_config_t timer_conf = {
        .duty_resolution = LEDC_TIMER_12_BIT,  // resolution of PWM duty
        .freq_hz = 50,                         // frequency of PWM signal (ALWAYS 50!!)
        .speed_mode = LEDC_HIGH_SPEED_MODE,    // timer mode 
        .timer_num = LEDC_TIMER_0,             // timer index 
        .clk_cfg = LEDC_AUTO_CLK,              // Auto select the source clock 
    };
    ledc_timer_config(&timer_conf);

    //Configure channels
    ledc_channel_config_t ledc_conf_1 = {
        .gpio_num = SERVO_GPIO_1,                // GPIO pin for PWM output
        .speed_mode = LEDC_HIGH_SPEED_MODE,    // LEDC speed mode
        .channel = LEDC_CHANNEL_0,             // LEDC channel
        .intr_type = LEDC_INTR_DISABLE,        // disable interrupt
        .timer_sel = LEDC_TIMER_0,             // timer index
        .duty = 0,                             // initial duty cycle
        .hpoint = 0,                           // hpoint value
    };
    ledc_channel_config(&ledc_conf_1);

    ledc_channel_config_t ledc_conf_2 = {
        .gpio_num = SERVO_GPIO_2,                // GPIO pin for PWM output
        .speed_mode = LEDC_HIGH_SPEED_MODE,    // LEDC speed mode
        .channel = LEDC_CHANNEL_1,             // LEDC channel
        .intr_type = LEDC_INTR_DISABLE,        // disable interrupt
        .timer_sel = LEDC_TIMER_0,             // timer index
        .duty = 0,                             // initial duty cycle
        .hpoint = 0,                           // hpoint value
    };
    ledc_channel_config(&ledc_conf_2);

    //while (true) {
        // TESTING SERVO

    // Move the servo from 0 to 180 degrees
    for (int angle = 0; angle <= 180; angle++) {
        setAngle(LEDC_CHANNEL_0, angle);
        vTaskDelay(10 / portTICK_PERIOD_MS); // Delay for smooth movement
    }

    // Move the servo from 180 to 0 degrees
    for (int angle = 180; angle >= 0; angle--) {
        setAngle(LEDC_CHANNEL_1, angle);
        vTaskDelay(10 / portTICK_PERIOD_MS); // Delay for smooth movement
    }
    //}
    ledc_stop(LEDC_HIGH_SPEED_MODE, LEDC_CHANNEL_0, 0); // Set idle_level to 0 (low) after stopping PWM
    ledc_stop(LEDC_HIGH_SPEED_MODE, LEDC_CHANNEL_1, 0); // Set idle_level to 0 (low) after stopping PWM
}
