#include "I2C.h"
#include "esp_log.h"



#define I2C_MASTER_SCL_IO 22               /*!< gpio number for I2C master clock */
#define I2C_MASTER_SDA_IO 21               /*!< gpio number for I2C master data  */
#define I2C_MASTER_FREQ_HZ 100000        /*!< I2C master clock frequency */
#define I2C_MASTER_TX_BUF_DISABLE 0                           /*!< I2C master doesn't need buffer */
#define I2C_MASTER_RX_BUF_DISABLE 0                           /*!< I2C master doesn't need buffer */
#define SLAVE_ADDRESS 0x0A

#define WRITE_BIT I2C_MASTER_WRITE              /*!< I2C master write */
#define READ_BIT I2C_MASTER_READ                /*!< I2C master read */
#define ACK_CHECK_EN 0x1                        /*!< I2C master will check ack from slave*/
#define ACK_CHECK_DIS 0x0                       /*!< I2C master will not check ack from slave */
#define ACK_VAL 0x0                             /*!< I2C ack value */
#define NACK_VAL 0x1           

int i2c_init(int SDA_PIN, int SCL_PIN, int FREQ, i2c_mode MODE)
{
    if(MODE == I2C_MASTER_MODE)
    {
        i2c_config_t i2c_config =
        {
            .mode = I2C_MODE_MASTER,
            .scl_io_num = SCL_PIN,
            .sda_io_num = SDA_PIN,
            .master.clk_speed = FREQ,
            .scl_pullup_en = GPIO_PULLUP_ENABLE,
            .sda_pullup_en = GPIO_PULLUP_ENABLE,
        };
        esp_err_t get_ret;

        get_ret = i2c_param_config(I2C_PORT, &i2c_config);
        if(get_ret != ESP_OK) return get_ret;

        get_ret = i2c_driver_install(I2C_PORT, i2c_config.mode, 0 , 0, 0);
        if(get_ret != ESP_OK) return get_ret;
         ESP_LOGI("I2C MODE INIT", "I2C MODE: [MASTER]");   
        return ESP_OK;
    }

 
    i2c_config_t i2c_config =
    {
        .mode = I2C_MODE_SLAVE,
        .scl_io_num = SCL_PIN,
        .sda_io_num = SDA_PIN,
        .master.clk_speed = FREQ,
        .scl_pullup_en = GPIO_PULLUP_ENABLE,
        .sda_pullup_en = GPIO_PULLUP_ENABLE,
    };
    esp_err_t get_ret;

    get_ret = i2c_param_config(I2C_PORT, &i2c_config);
    if(get_ret != ESP_OK) return get_ret;
    ESP_LOGI("I2C MODE INIT", "I2C MODE: [SLAVE]");  
    return i2c_driver_install(I2C_PORT, i2c_config.mode, 255, 255, 0);
}

int i2c_send(const uint8_t *data, i2c_mode MODE)
{

    ESP_LOGI("MSG I2C", "Sending Message = %s", data);   

    i2c_cmd_handle_t cmd = i2c_cmd_link_create();
    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, 0x0A<< I2C_MASTER_WRITE , true);
    i2c_master_write(cmd, data, 5, true);
    i2c_master_stop(cmd);
    i2c_master_cmd_begin(I2C_PORT, cmd, 1000/ portTICK_PERIOD_MS);
    i2c_cmd_link_delete(cmd);
    return 0;
}

int i2c_read(uint8_t *buffer)
{
    i2c_slave_read_buffer(I2C_PORT, buffer, 6, 20/ portTICK_PERIOD_MS);
    i2c_reset_rx_fifo(I2C_PORT);
    ESP_LOGI("Dado recebido", "%s\n", buffer);
    return 0;
}