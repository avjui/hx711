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
#define calibration_factor  -3025.0 
#define OFFSET              46709
//#define calibration_factor  -7050.0 

#define GAIN_128        1  // Channel A, gain factor 128
#define GAIN_32         2  // Channel B, gain factor 32
#define GAIN_64         3  // Channel A, gain factor 64


class HX711 {
    public:
        bool calibrate();
        float getLoad(uint64_t times=1);
        void hxTask();
        void poweron();
        void tare(int times);
        void setCalibration(int cal);
        void setGain(int gain);
        void standby();
        bool isReady();
        void wait_ready(unsigned long delay_ms=1);
        HX711(int pdsck, int dout, int gain=GAIN_128);
        ~HX711(){return;};
    public:
         float scale = calibration_factor;

    private:
        void _readData();
        long read_average(uint64_t  times);

    private:
        gpio_num_t _pdsck;
        gpio_num_t _dout;
        int _gain;
        int _delta;
        float _load;
        uint32_t raw_data;
        gpio_config_t pdsck_conf;
        gpio_config_t dout_conf;
        int32_t offset;
    };


#ifdef __cplusplus
}
#endif