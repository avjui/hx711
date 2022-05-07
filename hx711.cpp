
#include "hx711.h"

/**
 * @brief initialize hx711 driver
 * 
 */
HX711::HX711(int pdsck, int dout, int gain)
{
#ifdef DEBUG_HX711
    esp_log_level_set(MODUL_HX711, ESP_LOG_DEBUG);
#endif

    _pdsck = (gpio_num_t)pdsck;
    _dout = (gpio_num_t)dout;
    _gain = gain;
    _delta = 0;
    _load = 0;
    offset = OFFSET;
    ESP_LOGI(MODUL_HX711, "Pin PD_SCK: %d \t Pin DOUT: %d", _pdsck, _dout);

    
    //pdsck config
    pdsck_conf.intr_type = GPIO_INTR_DISABLE;
    pdsck_conf.mode = GPIO_MODE_OUTPUT;
    pdsck_conf.pin_bit_mask = (1ULL << pdsck);
    pdsck_conf.pull_down_en = GPIO_PULLDOWN_DISABLE;
    pdsck_conf.pull_up_en = GPIO_PULLUP_ENABLE;

    //dout config
    dout_conf.intr_type = GPIO_INTR_DISABLE;
    dout_conf.mode = GPIO_MODE_INPUT;
    dout_conf.pin_bit_mask = (1ULL << dout);
    dout_conf.pull_down_en = GPIO_PULLDOWN_DISABLE;
    dout_conf.pull_up_en = GPIO_PULLUP_ENABLE;

    gpio_config(&pdsck_conf);
    gpio_config(&dout_conf);
    
    //with power on we make also a reset to get in defined state
    poweron();
    return;
}

bool HX711::calibrate()
{
    _readData();
    _delta = _load;
    if (_delta)
    {
        return true;
    }
    return false;
}

void HX711::tare(int times) {
	double sum = read_average(times);
	offset = sum;
}


float HX711::getLoad(uint64_t  times)
{
    //float load = read_average(times) / scale;
    float load = (read_average(times)  - offset) / 2280.f;
    // convert _load to readable value
    return load;
}

/**
 * @brief power on the hx711
 * 
 */
void HX711::poweron()
{
    //reset befor beginn
    gpio_set_level(_pdsck, HIGH);
    ets_delay_us(80);
    gpio_set_level(_pdsck, LOW);
    ets_delay_us(2);
    return;
}

void HX711::setCalibration(int cal)
{
    _delta = cal;
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
    ets_delay_us(1);
    gpio_set_level(_pdsck, HIGH);
    ets_delay_us(1);
    return;
}

bool HX711::isReady(){
    return !gpio_get_level(_dout);
}
/**
 * @brief read data from hx711
 * 
 * 
 */
void HX711::_readData(void)
{
    uint32_t raw_data = 0;
    portMUX_TYPE mux = portMUX_INITIALIZER_UNLOCKED;
    //gpio_set_level(_pdsck, LOW);
    wait_ready();
    portENTER_CRITICAL(&mux);
    for (int i = 0; i < 24; i++)
    {
        ets_delay_us(1);
        gpio_set_level(_pdsck, HIGH);
        ets_delay_us(1);       
        raw_data |= gpio_get_level(_dout) << (23 - i);
        gpio_set_level(_pdsck, LOW);
    }

    //set channel and gain for next read
    for (int i = 0; i < _gain; i++)
    {
        ets_delay_us(1);
        gpio_set_level(_pdsck, HIGH);
        ets_delay_us(1);
        gpio_set_level(_pdsck, LOW);
    }
    portEXIT_CRITICAL(&mux);

    /*
    The output 24 bits of data is in 2’s complement
    format. When input differential signal goes out of
    the 24 bit range, the output data will be saturated
    at 800000h (MIN) or 7FFFFFh (MAX), until the
    input signal comes back to the input range.
    */

    /*if(raw_data & 0x80)
    {
        _load = -((raw_data ^ 0xffffff) + 1);
    }
    */
   /*
   raw_data = raw_data ^ 0x800000; // flip the 24th bit
    if (raw_data > 0xFFFFFF)
    {
        ESP_LOGW(MODUL_HX711, "Data out of range!!!\n");
        return;
    }
    else{
    _load = (int32_t)raw_data;
    }
    */
    if (raw_data & 0x800000){
        raw_data |= 0xff0000;
        //raw_data |= 0xff000000;
    }
    ESP_LOGI(MODUL_HX711, "RAW_DATA: %d", raw_data);
    _load = ((int32_t)raw_data);
    return;
}


void HX711::wait_ready(unsigned long delay_ms) {
	// Wait for the chip to become ready.
	// This is a blocking implementation and will
	while (!isReady()) {
        //ESP_LOGI(MODUL_HX711,"Waiting");
	    vTaskDelay(portTICK_PERIOD_MS);
	}
}

long HX711::read_average(uint64_t  times) {
	long sum = 0;
	for (uint64_t i = 0; i < times; i++) {
		_readData();
        sum += _load;
		//ets_delay_us(1);
	}
	return (sum / times);
}