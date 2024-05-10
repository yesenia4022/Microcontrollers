#include "driver/i2c.h"
#include "esp_log.h"

#define MASTER_SDA_IO 21         // Master's SDA pin
#define MASTER_SCL_IO 22         // Master's SCL pin
#define I2C_MASTER_NUM I2C_NUM_0 // I2C port number for master dev
#define I2C_MASTER_FREQ_HZ 100000
#define TCA9548A_ADDRESS 0x72     // Address of the TCA9548A multiplexer
#define SLAVE_ADDR 0x28           // Slave address, adjust as needed
#define WRITE_BIT I2C_MASTER_WRITE 
#define ACK_CHECK_EN true

static const char *TAG = "master_control";

// Initialize I2C as master
void i2c_master_init(void) {
    i2c_config_t conf = {
        .mode = I2C_MODE_MASTER,
        .sda_io_num = MASTER_SDA_IO,
        .scl_io_num = MASTER_SCL_IO,
        .sda_pullup_en = GPIO_PULLUP_ENABLE,
        .scl_pullup_en = GPIO_PULLUP_ENABLE,
        .master.clk_speed = I2C_MASTER_FREQ_HZ,
    };
    ESP_ERROR_CHECK(i2c_param_config(I2C_MASTER_NUM, &conf));
    ESP_ERROR_CHECK(i2c_driver_install(I2C_MASTER_NUM, conf.mode, 0, 0, 0));
}

// Select the channel on the TCA9548A multiplexer
void tca9548a_select_channel(uint8_t channel_num) {
    uint8_t write_buf = 1 << channel_num;
    i2c_cmd_handle_t cmd = i2c_cmd_link_create();
    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, (TCA9548A_ADDRESS << 1) | WRITE_BIT, ACK_CHECK_EN);
    i2c_master_write_byte(cmd, write_buf, ACK_CHECK_EN);
    i2c_master_stop(cmd);
    esp_err_t ret = i2c_master_cmd_begin(I2C_MASTER_NUM, cmd, 1000 / portTICK_PERIOD_MS);
    i2c_cmd_link_delete(cmd);
    if (ret == ESP_OK) {
        ESP_LOGI(TAG, "Channel %d selected", channel_num);
    } else {
        ESP_LOGE(TAG, "Failed to select channel %d", channel_num);
    }
}

// Ping the slave to test communication
void ping_slave(void) {
    i2c_cmd_handle_t cmd = i2c_cmd_link_create();
    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, (SLAVE_ADDR << 1) | WRITE_BIT, ACK_CHECK_EN);
    i2c_master_write_byte(cmd, 0x00, ACK_CHECK_EN); // Ping command
    i2c_master_stop(cmd);
    esp_err_t ret = i2c_master_cmd_begin(I2C_MASTER_NUM, cmd, 1000 / portTICK_PERIOD_MS);
    i2c_cmd_link_delete(cmd);
    if (ret == ESP_OK) {
        ESP_LOGI(TAG, "Ping to the slave successful.");
    } else {
        ESP_LOGE(TAG, "Failed to ping the slave.");
    }
}

void app_main(void) {
    // Initialize I2C for master
    i2c_master_init();
    ESP_LOGI(TAG, "I2C Master initialized successfully");

    // Log the slave address being used by the master
    ESP_LOGI(TAG, "Configured I2C slave address to communicate: 0x%X", SLAVE_ADDR);

    // Select the channel connected to the slave ESP32
    tca9548a_select_channel(0); // Adjust this to the correct channel if necessary

    // Test I2C communication with the slave device using a ping test
    ESP_LOGI(TAG, "Testing I2C communication with the slave device.");
    ping_slave(); // Ping the slave

    ESP_LOGI(TAG, "hello");
    // Add delay or additional tests as needed
    vTaskDelay(pdMS_TO_TICKS(5000)); // Wait for 5 seconds for observation
}
