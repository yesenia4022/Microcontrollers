#include "driver/i2c.h"
#include "esp_log.h"
#include <stdio.h>

#define I2C_MASTER_SCL_IO            22
#define I2C_MASTER_SDA_IO            21
#define I2C_MASTER_NUM               I2C_NUM_0
#define I2C_MASTER_FREQ_HZ           100000
#define I2C_MASTER_TX_BUF_DISABLE    0
#define I2C_MASTER_RX_BUF_DISABLE    0
#define I2C_MASTER_TIMEOUT_MS        1000

#define MUX_ADDRESS                  0x72 // Actual mux's I2C address
#define MUX_CHANNEL_1                0x01 // Channel for the motor control ESP32
#define MUX_CHANNEL_2                0x02 // Channel for the next state ESP32
#define SLAVE_ADDR1                  0x04 // I2C address of the motor control slave ESP32
#define SLAVE_ADDR2                  0x05 // I2C address of the second ESP32 for state 2
#define MOTOR_START_CMD              0x00 // Command to start the motors

typedef enum {
    STATE_MOTOR_CONTROL = 0,
    STATE_END,
    NUM_STATES
} State;

State current_state = STATE_MOTOR_CONTROL;

void i2c_master_init() {
    i2c_config_t conf = {
        .mode = I2C_MODE_MASTER,
        .sda_io_num = I2C_MASTER_SDA_IO,
        .scl_io_num = I2C_MASTER_SCL_IO,
        .sda_pullup_en = GPIO_PULLUP_ENABLE,
        .scl_pullup_en = GPIO_PULLUP_ENABLE,
        .master.clk_speed = I2C_MASTER_FREQ_HZ,
    };
    i2c_param_config(I2C_MASTER_NUM, &conf);
    i2c_driver_install(I2C_MASTER_NUM, conf.mode, I2C_MASTER_RX_BUF_DISABLE, I2C_MASTER_TX_BUF_DISABLE, 0);
}

void select_mux_channel(uint8_t channel) {
    i2c_cmd_handle_t cmd = i2c_cmd_link_create();
    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, (MUX_ADDRESS << 1) | I2C_MASTER_WRITE, true);
    i2c_master_write_byte(cmd, channel, true);
    i2c_master_stop(cmd);
    esp_err_t ret = i2c_master_cmd_begin(I2C_MASTER_NUM, cmd, I2C_MASTER_TIMEOUT_MS / portTICK_PERIOD_MS);
    if (ret == ESP_OK) {
        printf("Multiplexer channel %d selected successfully\n", channel);
    } else {
        printf("Failed to select multiplexer channel %d\n", channel);
    }
    i2c_cmd_link_delete(cmd);
}

void send_motor_start_command() {
    i2c_cmd_handle_t cmd = i2c_cmd_link_create();
    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, (SLAVE_ADDR1 << 1) | I2C_MASTER_WRITE, true);
    i2c_master_write_byte(cmd, MOTOR_START_CMD, true);
    i2c_master_stop(cmd);
    esp_err_t ret = i2c_master_cmd_begin(I2C_MASTER_NUM, cmd, I2C_MASTER_TIMEOUT_MS / portTICK_PERIOD_MS);
    if (ret == ESP_OK) {
        printf("Start motor command sent successfully\n");
    } else {
        printf("Failed to send start motor command\n");
    }
    i2c_cmd_link_delete(cmd);
}

void send_second_start_command() {
    i2c_cmd_handle_t cmd = i2c_cmd_link_create();
    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, (SLAVE_ADDR2 << 1) | I2C_MASTER_WRITE, true);
    i2c_master_write_byte(cmd, MOTOR_START_CMD, true);
    i2c_master_stop(cmd);
    esp_err_t ret = i2c_master_cmd_begin(I2C_MASTER_NUM, cmd, I2C_MASTER_TIMEOUT_MS / portTICK_PERIOD_MS);
    if (ret == ESP_OK) {
        printf("Second State command sent successfully\n");
    } else {
        printf("Failed to send Second State command\n");
    }
    i2c_cmd_link_delete(cmd);
}

// Read a byte from the specified slave address
uint8_t read_slave_signal(uint8_t slave_addr) {
    uint8_t signal = 0;
    i2c_cmd_handle_t cmd = i2c_cmd_link_create();
    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, (slave_addr << 1) | I2C_MASTER_READ, true);
    i2c_master_read_byte(cmd, &signal, I2C_MASTER_NACK);  // I2C_MASTER_LAST_NACK might be used depending on SDK version
    i2c_master_stop(cmd);
    esp_err_t ret = i2c_master_cmd_begin(I2C_MASTER_NUM, cmd, pdMS_TO_TICKS(I2C_MASTER_TIMEOUT_MS));
    i2c_cmd_link_delete(cmd);
    if (ret != ESP_OK) {
        printf("Failed to read from slave: %d\n", ret);
        return 0xFF; // An error code or invalid signal value if read fails
    }
    return signal;
}

void app_main() {
    i2c_master_init();
    printf("Master initialized, setting initial state...\n");

    while (1) {
        switch(current_state) {
            case STATE_MOTOR_CONTROL:
                printf("In Motor Control State\n");
                select_mux_channel(MUX_CHANNEL_1);
                send_motor_start_command(); // Send start command to the motor control slave
                printf("before if statement\n");
                if (read_slave_signal(SLAVE_ADDR1) == 0x01) { // Assuming 0x01 is the completion signal
                    printf("Task completed by Motor Control Slave.\n");
                    current_state = STATE_END;
                }
                printf("after if statement\n");
                break;
            case STATE_END:
                printf("In End State - operation completed.\n");
                select_mux_channel(MUX_CHANNEL_2);
                send_second_start_command();// Send start command to the second ESP32 slave
                if (read_slave_signal(SLAVE_ADDR2) == 0x01) { // Assuming 0x01 is the completion signal
                    printf("Task completed by Second ESP32 Slave.\n");
                    current_state = STATE_END;
                }
                break;
                
            default:
                printf("Unknown state\n");
                break;
        }
        vTaskDelay(pdMS_TO_TICKS(1000)); // Main loop delay
    }
}