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

#define MUX_ADDRESS                  0x72 // Replace with your actual mux's I2C address
#define MUX_CHANNEL_1                0x01 // Replace with the command or value to select channel 1 on the mux
#define SLAVE_ADDR                   0x04 // The I2C address of the slave ESP32
#define MOTOR_START_CMD              0x00 // The command byte to start the motors

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
    i2c_driver_install(I2C_MASTER_NUM, conf.mode,
                       I2C_MASTER_RX_BUF_DISABLE,
                       I2C_MASTER_TX_BUF_DISABLE, 0);
}

void select_mux_channel(uint8_t channel) {
    i2c_cmd_handle_t cmd = i2c_cmd_link_create();
    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, (MUX_ADDRESS << 1) | I2C_MASTER_WRITE, true);
    i2c_master_write_byte(cmd, channel, true);
    i2c_master_stop(cmd);
    esp_err_t ret = i2c_master_cmd_begin(I2C_MASTER_NUM, cmd, I2C_MASTER_TIMEOUT_MS / portTICK_PERIOD_MS);
    if (ret == ESP_OK) {
        printf("Multiplexer channel selected successfully\n");
    } else {
        printf("Failed to select multiplexer channel\n");
    }
    i2c_cmd_link_delete(cmd);
}

void send_motor_start_command() {
    i2c_cmd_handle_t cmd = i2c_cmd_link_create();
    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, (SLAVE_ADDR << 1) | I2C_MASTER_WRITE, true);
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

void app_main() {
    i2c_master_init();
    printf("Master initialized, waiting to send start command...\n");

    // Wait for some event or condition to be true before sending the command.
    // This could be a delay, a button press, or some initialization completion.
    // For example, you could use vTaskDelay() to wait for a few seconds:
    vTaskDelay(pdMS_TO_TICKS(5000));  // Wait for 5 seconds

    printf("Sending start motor command...\n");
    select_mux_channel(MUX_CHANNEL_1); // Select the mux channel connected to the slave
    send_motor_start_command(); // Send the start command to the slave
}