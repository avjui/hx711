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
#define CAL_FAC         -3025
#define OFFSET          46709.0
#define TARETIMES       10
//#define calibration_factor  -7050.0 

#define GAIN_128        1  // Channel A, gain factor 128
#define GAIN_32         2  // Channel B, gain factor 32
#define GAIN_64         3  // Channel A, gain factor 64


typedef struct {
    int pin_pdsck;
    int pin_dout;
    float *load;
    int read_times;
    int gain;
    int cal_fac;
    double offset;
} hx711_conf_t;

class HX711 {
    public:
        bool calibrate();
        void getLoad(uint64_t times=1);
        void hxTask();
        void poweron();
        void tare(int times=TARETIMES);
        void setCalibration(int cal);
        void setGain(int gain);
        void standby();
        bool isReady();
        void wait_ready(unsigned long delay_ms=1);
        HX711(hx711_conf_t *conf_hx711);
        HX711(int pdsck, int dout, int gain=GAIN_128);
        ~HX711() { return; };
    public:
         float scale = CAL_FAC;

    private:
        void _readData();
        long read_average(uint64_t  times);
        uint8_t trys;

    private:
        gpio_num_t _pdsck;
        gpio_num_t _dout;
        int _gain;
        float _delta;
        float *_load;
        float _oload;
        uint32_t raw_data;
        gpio_config_t pdsck_conf;
        gpio_config_t dout_conf;
        double offset;
        int read_times;
    };


#ifdef __cplusplus
}
#endif