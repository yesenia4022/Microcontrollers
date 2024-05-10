#include "driver/i2c.h"

#define SLAVE_ADDR 0x05
#define I2C_SLAVE_SCL_IO 22
#define I2C_SLAVE_SDA_IO 21
#define I2C_SLAVE_NUM I2C_NUM_0

void i2c_slave_init() {
    i2c_config_t i2c_config = {
        .mode = I2C_MODE_SLAVE,
        .sda_io_num = I2C_SLAVE_SDA_IO,
        .scl_io_num = I2C_SLAVE_SCL_IO,
        .sda_pullup_en = GPIO_PULLUP_ENABLE,
        .scl_pullup_en = GPIO_PULLUP_ENABLE,
        .slave.addr_10bit_en = 0,
        .slave.slave_addr = SLAVE_ADDR
    };
    ESP_ERROR_CHECK(i2c_param_config(I2C_SLAVE_NUM, &i2c_config));
    ESP_ERROR_CHECK(i2c_driver_install(I2C_SLAVE_NUM, i2c_config.mode, 512, 512, 0));
}

void app_main() {
    i2c_slave_init();
    // Additional logic for handling I2C communications can be added here.
}