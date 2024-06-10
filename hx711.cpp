
#include "hx711.h"

HX711::HX711(hx711_conf_t *conf_hx711)
{
#ifdef DEBUG_HX711
    esp_log_level_set(MODUL_HX711, ESP_LOG_VERBOSE);
#else
    esp_log_level_set(MODUL_HX711, ESP_LOG_INFO);
#endif

    _pdsck = (gpio_num_t)conf_hx711->pin_pdsck;
    _dout = (gpio_num_t)conf_hx711->pin_dout;
    read_times = conf_hx711->read_times;
    _gain = conf_hx711->gain;
    _load = conf_hx711->load;
    _oload = 0.;
    offset = conf_hx711->offset;
    _scale = conf_hx711->scale;
    _error = false;
    
    ESP_LOGV(MODUL_HX711, "Pin PD_SCK: %d \t Pin DOUT: %d", _pdsck, _dout);

    // pdsck config
    pdsck_conf.intr_type = GPIO_INTR_DISABLE;
    pdsck_conf.mode = GPIO_MODE_OUTPUT;
    pdsck_conf.pin_bit_mask = (1ULL << conf_hx711->pin_pdsck);
    pdsck_conf.pull_down_en = GPIO_PULLDOWN_DISABLE;
    pdsck_conf.pull_up_en = GPIO_PULLUP_ENABLE;

    // dout config
    dout_conf.intr_type = GPIO_INTR_DISABLE;
    dout_conf.mode = GPIO_MODE_INPUT;
    dout_conf.pin_bit_mask = (1ULL << conf_hx711->pin_dout);
    dout_conf.pull_down_en = GPIO_PULLDOWN_DISABLE;
    dout_conf.pull_up_en = GPIO_PULLUP_ENABLE;

    gpio_config(&pdsck_conf);
    gpio_config(&dout_conf);

    trys = 0;

    // with power on we make also a reset to get in defined state
    poweron();
    return;
}

void HX711::tare()
{
    double sum = 0;
    sum = read_average(TARETIMES);
    offset = sum;
    ESP_LOGI(MODUL_HX711, "Offset set to : %f", offset);
}

void HX711::getLoad()
{

    *_load = (read_average(*read_times) - offset) / _scale;

    if(_error)
    {
        *_load = -1;
    }
    return;
}

/**
 * @brief power on the hx711
 *
 */
void HX711::poweron()
{
    // reset befor beginn
    gpio_set_level(_pdsck, HIGH);
    ets_delay_us(80);
    gpio_set_level(_pdsck, LOW);
    ets_delay_us(WAIT_TIME);
    return;
}

void HX711::setGain(int gain)
{
    _gain = gain;
    return;
}
/**
 * @brief set the hx711 to standby
 *
 * PDSCK must set to high for more then 60us
 *
 */
void HX711::standby()
{
    gpio_set_level(_pdsck, LOW);
    ets_delay_us(WAIT_TIME);
    gpio_set_level(_pdsck, HIGH);
    ets_delay_us(WAIT_TIME);
    return;
}

bool HX711::isReady()
{
    return !gpio_get_level(_dout);
}
/**
 * @brief read data from hx711
 *
 *
 */
void HX711::_readData(void)
{
    uint8_t raw_data[3] = {0};
    uint32_t data = 0x00 << 24;
    portMUX_TYPE mux = portMUX_INITIALIZER_UNLOCKED;
    if (wait_ready())
    {
        portENTER_CRITICAL(&mux);
        for (int j = 2; j >= 0; j--)
        {

            for (int i = 0; i < 8; i++)
            {
                gpio_set_level(_pdsck, HIGH);
                ets_delay_us(WAIT_TIME);
                raw_data[j] |= gpio_get_level(_dout) << (7 - i);
                gpio_set_level(_pdsck, LOW);
                ets_delay_us(WAIT_TIME);
            }
        }

        // set channel and gain for next read
        for (int i = 0; i < _gain; i++)
        {
            // ets_delay_us(1);
            gpio_set_level(_pdsck, HIGH);
            ets_delay_us(WAIT_TIME);
            gpio_set_level(_pdsck, LOW);
            ets_delay_us(WAIT_TIME);
        }
        portEXIT_CRITICAL(&mux);

        // combine data
        data = 0x00 << 24 | raw_data[2] << 16 | raw_data[1] << 8 | raw_data[0];

        ESP_LOGV(MODUL_HX711, "DATA: %lu", data);
        _oload = ((int32_t)data);
        return;
    }
    else
    {
        ESP_LOGE(MODUL_HX711, "Error HX711 dont get ready! Please check your wires.");
        return;
    }
}

bool HX711::wait_ready(unsigned long delay_ms)
{

    trys = 0;
    // Wait for the chip to become ready.
    // This is a blocking implementation and will
    while (!isReady() || trys < 10)
    {
        ESP_LOGV(MODUL_HX711, "Waiting to be ready");
        vTaskDelay(portTICK_PERIOD_MS);
        trys++;

        if (trys >= 10)
        {
            _error = true;
            return false;
        }
    }
    _error = false;
    return true;
}

long HX711::read_average(uint64_t times)
{
    float sum = 0.;

    while (!_error)
    {
        for (uint64_t i = 0; i < times; i++)
        {
            _readData();
            sum += _oload;
        }
    }
    return (sum / times);
}