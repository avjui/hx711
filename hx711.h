# pragma once

#ifdef __cplusplus
extern "C"
{
#endif

#include <stdio.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include "driver/gpio.h"
#include "esp_log.h"


#define MODUL_HX711           "HX711"

#define HIGH            1
#define LOW             0
#define SCALE           2200.f
#define OFFSET          46709.0
#define TARETIMES       20

#define GAIN_128        1  // Channel A, gain factor 128
#define GAIN_32         2  // Channel B, gain factor 32
#define GAIN_64         3  // Channel A, gain factor 64


typedef struct {
    int pin_pdsck;
    int pin_dout;
    float *load;
    int *read_times;
    int gain;
    float scale;
    double offset;
} hx711_conf_t;

class HX711 {
    public:
        void getLoad();
        void hxTask();
        void poweron();
        void tare();
        void setGain(int gain);
        void standby();
        bool isReady();
        void wait_ready(unsigned long delay_ms=1);
        HX711(hx711_conf_t *conf_hx711);
        ~HX711() { return; };

    private:
        void _readData();
        long read_average(uint64_t  times);
        uint8_t trys;

    private:
        gpio_num_t _pdsck;
        gpio_num_t _dout;
        int _gain;
        float *_load;
        float _oload;
        float _scale;
        uint8_t raw_data[3];
        gpio_config_t pdsck_conf;
        gpio_config_t dout_conf;
        double offset;
        int *read_times;
    };


#ifdef __cplusplus
}
#endif