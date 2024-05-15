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
#define MUX_CHANNEL_2                0x02 // Channel for the servo control ESP32
#define MUX_CHANNEL_3                0x04 // Channel for the camera control ESP32
#define SLAVE_ADDR1                  0x04 // I2C address of the motor control slave ESP32
#define SLAVE_ADDR2                  0x05 // I2C address of the servo control slave ESP32
#define SLAVE_ADDR3                  0x06 // I2C address of the camera control slave ESP32
#define MOTOR_START_CMD              0x00 // Command to start the motors
#define SERVO1_CONTROL_CMD           0x03 // Command to control the first servo
#define SERVO2_CONTROL_CMD           0x04 // Command to control the second servo
#define CAMERA_START_CMD             0x05 // Command to start the camera
#define EXTENDED_TIME_CMD            0x06 // New command for 12 seconds
#define SERVO3_CONTROL_CMD           0x08 // Command to control the 1st servo
//#define HELLO_WORLD_CMD              0x09 // Command to test communication
//#define GOODBYE_WORLD_CMD            0x0A // Command to test communication

typedef enum {
    STATE_MOTOR_CONTROL = 0,
    STATE_SERVO1_CONTROL,
    STATE_SERVO2_CONTROL,
    STATE_SERVO3_CONTROL,
    STATE_BEFORE_CAMERA,
    STATE_END,
    STATE_AFTER_CAMERA,
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

void send_command_to_slave(uint8_t slave_addr, uint8_t command) {
    i2c_cmd_handle_t cmd = i2c_cmd_link_create();
    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, (slave_addr << 1) | I2C_MASTER_WRITE, true);
    i2c_master_write_byte(cmd, command, true);
    i2c_master_stop(cmd);
    esp_err_t ret = i2c_master_cmd_begin(I2C_MASTER_NUM, cmd, I2C_MASTER_TIMEOUT_MS / portTICK_PERIOD_MS);
    if (ret == ESP_OK) {
        printf("Command %d sent successfully to slave %d\n", command, slave_addr);
    } else {
        printf("Failed to send command %d to slave %d\n", command, slave_addr);
    }
    i2c_cmd_link_delete(cmd);
}

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
        printf("Failed to read from slave %d: %d\n", slave_addr, ret);
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
                send_command_to_slave(SLAVE_ADDR1, MOTOR_START_CMD);
                if (read_slave_signal(SLAVE_ADDR1) == 0x01) {
                    printf("Task completed by Motor Control Slave.\n");
                    current_state = STATE_SERVO1_CONTROL;
                }
                break;
            case STATE_SERVO1_CONTROL:
                printf("In Servo 1 Control State\n");
                select_mux_channel(MUX_CHANNEL_2);
                send_command_to_slave(SLAVE_ADDR2, SERVO1_CONTROL_CMD);
                //send_command_to_slave(SLAVE_ADDR2, SERVO3_CONTROL_CMD);
                if (read_slave_signal(SLAVE_ADDR2) == 0x01) {
                    printf("Servo 1 operation completed.\n");
                    //current_state = STATE_SERVO2_CONTROL;
                    current_state = STATE_BEFORE_CAMERA;
                }
                break;
            case STATE_BEFORE_CAMERA:
                printf("In Before Camera Control State\n");
                select_mux_channel(MUX_CHANNEL_3);
                send_command_to_slave(SLAVE_ADDR3, CAMERA_START_CMD);
                if (read_slave_signal(SLAVE_ADDR3) == 0x01) {
                    printf("Took picture before cleaning the surface.\n");
                    vTaskDelay(pdMS_TO_TICKS(1000));
                    current_state = STATE_SERVO2_CONTROL;
                }
                break;
            case STATE_SERVO2_CONTROL:
                printf("In Servo 2 Control State\n");
                select_mux_channel(MUX_CHANNEL_2);
                send_command_to_slave(SLAVE_ADDR2, SERVO2_CONTROL_CMD);
                if (read_slave_signal(SLAVE_ADDR2) == 0x01) {
                    printf("Servo 2 operation completed.\n");
                    //current_state = STATE_SERVO3_CONTROL;
                    current_state = STATE_AFTER_CAMERA;
                }
                break;
            case STATE_AFTER_CAMERA:
                printf("In After Camera Control State\n");
                select_mux_channel(MUX_CHANNEL_3);
                send_command_to_slave(SLAVE_ADDR3, EXTENDED_TIME_CMD);
                if (read_slave_signal(SLAVE_ADDR3) == 0x01) {
                    printf("Took picture after cleaning the surface.\n");
                    //current_state = STATE_END;
                    current_state = STATE_SERVO3_CONTROL;
                }
                break;
            case STATE_SERVO3_CONTROL:
                printf("In Servo 3 Control State\n");
                select_mux_channel(MUX_CHANNEL_2);
                send_command_to_slave(SLAVE_ADDR2, SERVO3_CONTROL_CMD);
                if (read_slave_signal(SLAVE_ADDR2) == 0x01) {
                    printf("Servo 3 operation completed.\n");
                    //current_state = STATE_AFTER_CAMERA;
                    current_state = STATE_END;
                    
                }
                break;
            case STATE_END:
                printf("All 5 States completed successfully!\n");
                break;
            default:
                printf("Unknown state\n");
                break;
        }
        vTaskDelay(pdMS_TO_TICKS(1000)); // Main loop delay
    }
}