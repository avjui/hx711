
#include "hx711.h"

HX711::HX711()
{
#ifdef DEBUG_HX711
    esp_log_level_set(MODUL_HX711, ESP_LOG_VERBOSE);
#else
    esp_log_level_set(MODUL_HX711, ESP_LOG_INFO);
#endif

    _pdsck = gpio_num_t(CONFIG_HX711_PIN_PDSCK);
    _dout = gpio_num_t(CONFIG_HX711_PIN_DOUT);
    read_times = CONFIG_HX711_SAMPLES;
    _gain = CONFIG_HX711_GAIN;
    _oload = 0.;
    offset = OFFSET;
    _scale = CONFIG_HX711_SCALE;
    _error = false;
    trys = 0;
    cellload = 0.00;

    // initial the pin configurtion
    init();

    // with power on we make also a reset to get in defined state
    poweron();

    ESP_LOGV(MODUL_HX711, "Pin PD_SCK: %d \t Pin DOUT: %d", _pdsck, _dout);
    return;
}

HX711::HX711(hx711_conf_t *conf_hx711)
{
#ifdef DEBUG_HX711
    esp_log_level_set(MODUL_HX711, ESP_LOG_VERBOSE);
#else
    esp_log_level_set(MODUL_HX711, ESP_LOG_INFO);
#endif

    _pdsck = (gpio_num_t)conf_hx711->pin_pdsck;
    _dout = (gpio_num_t)conf_hx711->pin_dout;
    read_times = conf_hx711->samples;
    _gain = conf_hx711->gain;
    _oload = 0.;
    offset = OFFSET;
    _scale = conf_hx711->scale;
    _error = false;
    trys = 0;
    cellload = 0.00;

    // initial the pin configurtion
    init();

    // with power on we make also a reset to get in defined state
    poweron();

    ESP_LOGV(MODUL_HX711, "Pin PD_SCK: %d \t Pin DOUT: %d", _pdsck, _dout);
    return;
}

float HX711::getOffset()
{
    return offset;
}

float HX711::getScale()
{
    return _scale;
}

void HX711::poweron()
{
    // reset befor beginn
    gpio_set_level(_pdsck, HIGH);
    ets_delay_us(80);
    gpio_set_level(_pdsck, LOW);
    ets_delay_us(WAIT_TIME);
    return;
}

void HX711::setGain(hx711_gain gain)
{
    _gain = gain;
    return;
}

void HX711::setOffset(float newoffset)
{
    offset = newoffset;
    return;
}

void HX711::setScale(float scaleFactor)
{
    _scale = scaleFactor;
    return;
}

void HX711::standby()
{
    gpio_set_level(_pdsck, LOW);
    ets_delay_us(WAIT_TIME);
    gpio_set_level(_pdsck, HIGH);
    ets_delay_us(WAIT_TIME);
    return;
}

void HX711::startTask()
{
    taskrun = true;
    xTaskCreate(hx711Task, "HX711 - Thread", 2048, this, 10, NULL);
}

void HX711::stopTask()
{
    taskrun = false;
}

void HX711::tare()
{
    /* Warm up to be more accurate. Set read_times to 10 so we
    *  have a warmup time between 3 and 6 seconds.)
    */

    int warmup = 3;
    float sum = 0;
    int temp_read_times = read_times;
    read_times = 10;
    ESP_LOGI(MODUL_HX711, "Warmup hx711 for calibration!");
    while(warmup > 0)
    {
        update();
        warmup--;
        vTaskDelay(100/portTICK_PERIOD_MS);
    }
    read_times = temp_read_times;

    // lets tare and set the offset
    sum = read_average(TARESAMPLES);
    offset = sum;
    ESP_LOGI(MODUL_HX711, "Offset set to : %f", offset);
}

void HX711::update()
{
    float t = read_average(read_times) - offset;
    cellload = t / _scale;
    if (_error)
    {
        cellload = -1;
    }
    return;
}

bool HX711::wait_ready(unsigned long delay_ms)
{

    // Wait for the chip to become ready.
    // This is a blocking implementation and will

    gpio_set_level(_pdsck, LOW);
    while (gpio_get_level(_dout) || trys < 30)
    {
        if (trys >= 30)
        {
            trys = 0;
            _error = true;
            return false;
        }
        trys++;
        // give some time to get ready
        ESP_LOGV(MODUL_HX711, "Waiting to be ready");
        vTaskDelay(10 / portTICK_PERIOD_MS);
    }
    trys = 0;
    _error = false;
    return true;
}

void HX711::hx711Task(void *pvParameter)
{
    HX711 *thisPtr = (HX711 *)pvParameter;
#ifdef DEBUG_HX711
    esp_log_level_set(MODUL_HX711, ESP_LOG_VERBOSE);
#else
    esp_log_level_set(MODUL_HX711, ESP_LOG_INFO);
#endif
    ESP_LOGI(MODUL_HX711, "########## Starting HX711 Task ##########");
    while (thisPtr->taskrun)
    {
        thisPtr->update();
        vTaskDelay(100 / portTICK_PERIOD_MS);
    }
    // When we are here we stopped the task.
    ESP_LOGI(MODUL_HX711, "########## Stoping HX711 Task ##########");

}

void HX711::init()
{
    // pdsck config
    pdsck_conf.intr_type = GPIO_INTR_DISABLE;
    pdsck_conf.mode = GPIO_MODE_OUTPUT;
    pdsck_conf.pin_bit_mask = (1ULL << _pdsck);
    pdsck_conf.pull_down_en = GPIO_PULLDOWN_DISABLE;
    pdsck_conf.pull_up_en = GPIO_PULLUP_ENABLE;

    // dout config
    dout_conf.intr_type = GPIO_INTR_DISABLE;
    dout_conf.mode = GPIO_MODE_INPUT;
    dout_conf.pin_bit_mask = (1ULL << _dout);
    dout_conf.pull_down_en = GPIO_PULLDOWN_DISABLE;
    dout_conf.pull_up_en = GPIO_PULLUP_DISABLE;

    gpio_config(&pdsck_conf);
    gpio_config(&dout_conf);

    return;
}

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
                gpio_set_level(_pdsck, LOW);
                raw_data[j] |= gpio_get_level(_dout) << (7 - i);
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

#if ESP_IDF_VERSION < ESP_IDF_VERSION_VAL(5, 0, 0)
        ESP_LOGV(MODUL_HX711, "DATA: %u", data);
#else
        ESP_LOGV(MODUL_HX711, "DATA: %lu", data);
#endif
        _oload = ((int32_t)data);
        return;
    }
    else
    {
        ESP_LOGE(MODUL_HX711, "Error HX711 dont get ready! Please check your wires.");
        return;
    }
}

float HX711::read_average(uint8_t times)
{
    // variabel init
    for (int i = 0; i < times; i++)
    {
        _loadsamples[i] = 0.0;
    }

    float sum = 0.;
    uint8_t _rmnumber = 0;
    float high_data = 0.f;
    float low_data = 0.f;

    for (uint8_t i = 0; i <= times; i++)
    {
        _readData();
        _loadsamples[i] = _oload;
        // sum += _oload;
    }
    if (_error)
    {
        ESP_LOGE(MODUL_HX711, "Can not read data!!!");
        return 0;
    }

    //only used when we have 3 and more samples
    if (read_times > 2)
    {
        // find highest value
        for (uint8_t i = 0; i < times; i++)
        {
            if (_loadsamples[i] > high_data)
            {
                high_data = _loadsamples[i];
            }
        }

        // find the lowest value
        low_data = high_data;
        for (uint8_t i = 0; i < times; i++)
        {
            if (_loadsamples[i] < low_data)
            {
                low_data = _loadsamples[i];
            }
        }
    }
    // summit it without the highest and lowest value
    for (uint8_t i = 0; i < times; i++)
    {
        if ((_loadsamples[i] == low_data) || (_loadsamples[i] == high_data))
        {
            _rmnumber++;
            continue;
        }
        sum += _loadsamples[i];
    }

    if ((sum / (times - _rmnumber)) > 0)
    {
        ESP_LOGV(MODUL_HX711, " Sum average: %f", sum);
        return sum / (times - _rmnumber);
    }
    else
    {
        return 0.0;
    }
}