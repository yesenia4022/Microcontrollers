#include "driver/i2c.h"

#define I2C_MASTER_SCL_IO    22    /*!< gpio number for I2C master clock */
#define I2C_MASTER_SDA_IO    21    /*!< gpio number for I2C master data  */
#define I2C_MASTER_NUM       I2C_NUM_0 /*!< I2C port number for master dev */
#define I2C_MASTER_FREQ_HZ   100000     /*!< I2C master clock frequency */
#define I2C_MASTER_TX_BUF_DISABLE   0   /*!< I2C master doesn't need buffer */
#define I2C_MASTER_RX_BUF_DISABLE   0   /*!< I2C master doesn't need buffer */
#define I2C_MASTER_TIMEOUT_MS       1000

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

void app_main() {
    i2c_master_init();
    printf("I2C master initialized successfully\n");

    // Example of sending data to a slave device
    // Here we just send a single byte to a slave with address 0x04
    int ret;
    i2c_cmd_handle_t cmd = i2c_cmd_link_create();
    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, (0x04 << 1) | I2C_MASTER_WRITE, true);
    i2c_master_write_byte(cmd, 0xAA, true); // Example byte to send
    i2c_master_stop(cmd);
    ret = i2c_master_cmd_begin(I2C_MASTER_NUM, cmd, I2C_MASTER_TIMEOUT_MS / portTICK_RATE_MS);
    if (ret == ESP_OK) {
        printf("Write successfully\n");
    } else {
        printf("Write failed\n");
    }
    i2c_cmd_link_delete(cmd);
}
