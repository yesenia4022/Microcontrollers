#include "driver/i2c.h"

#define I2C_SLAVE_SCL_IO     22    /*!< gpio number for I2C slave clock */
#define I2C_SLAVE_SDA_IO     21    /*!< gpio number for I2C slave data */
#define I2C_SLAVE_NUM        I2C_NUM_0 /*!< I2C port number for slave dev */
#define I2C_SLAVE_TX_BUF_LEN 1024  /*!< I2C slave tx buffer size */
#define I2C_SLAVE_RX_BUF_LEN 1024  /*!< I2C slave rx buffer size */
#define I2C_SLAVE_ADDR       0x04  /*!< I2C slave address */

void i2c_slave_init() {
    i2c_config_t conf_slave = {
        .sda_io_num = I2C_SLAVE_SDA_IO,
        .scl_io_num = I2C_SLAVE_SCL_IO,
        .sda_pullup_en = GPIO_PULLUP_ENABLE,
        .scl_pullup_en = GPIO_PULLUP_ENABLE,
        .mode = I2C_MODE_SLAVE,
        .slave.addr_10bit_en = 0,
        .slave.slave_addr = I2C_SLAVE_ADDR,
    };
    i2c_param_config(I2C_SLAVE_NUM, &conf_slave);
    i2c_driver_install(I2C_SLAVE_NUM, conf_slave.mode,
                       I2C_SLAVE_RX_BUF_LEN,
                       I2C_SLAVE_TX_BUF_LEN, 0);
}

void app_main() {
    i2c_slave_init();
    printf
