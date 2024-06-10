# pragma once

#ifdef __cplusplus
extern "C"
{
#endif

#include <stdio.h>
#include <string.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include "driver/gpio.h"
#include <esp_timer.h>
#include "esp_log.h"
#if ESP_IDF_VERSION >= ESP_IDF_VERSION_VAL(5, 0, 0)
    #include <rom/ets_sys.h>
#endif


#define MODUL_HX711           "HX711"

#define HIGH            1
#define LOW             0
#define SCALE           491.f 
#define OFFSET          46709.0
#define TARETIMES       20
#define WAIT_TIME       20                             

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
        /**
         * @brief Function to get the load value
         * 
         */
        void getLoad();
        void hxTask();

        /**
         * @brief Function to power on the hx711 controller
         * 
         */
        void poweron();

        /**
         * @brief Function to tare the cell load
         * 
         * @details In this function we will read the sensor for the number 'TIMETARES' to get the offset.
         *          In the start of the function we will also read the load until 2 seconds to get a more 
         *          accurate result. 
         * 
         */
        void tare();

        /**
         * @brief Set the gain for hx711
         * 
         * @param gain 
         */
        void setGain(int gain);

        /**
         * @brief Function to bring the hx711 to standby mode
         * 
         */
        void standby();

        /**
         * @brief Function to get the ready status from hx711 to read values out
         * 
         * @return true 
         * @return false 
         */
        bool isReady();
        bool wait_ready(unsigned long delay_ms=1);
        /**
         * @brief Construct a new HX711 object
         * 
         * @param conf_hx711 
         */
        HX711(hx711_conf_t *conf_hx711);
        ~HX711() { return; };

    private:
        void _readData();

        /**
         * @brief Helper function to get the average of load.
         * 
         * @details This function will always be called when we read the value. It will take samples of 'n' times 
         *          to get a more accurate result. It will also remove the highest and lowest value.
         * 
         * @param times 
         * @return long 
         */
        float read_average(uint8_t times);

    private:
        uint8_t trys;
        gpio_num_t _pdsck;
        gpio_num_t _dout;
        int _gain;
        float *_load;
        float _oload;
        float _scale;
        uint8_t raw_data[3];
        gpio_config_t pdsck_conf;
        gpio_config_t dout_conf;
        float offset;
        int *read_times;
        bool _error;
        float _loadsamples[TARETIMES];
    };


#ifdef __cplusplus
}
#endif