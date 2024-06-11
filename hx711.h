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
#include "sdkconfig.h"

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
    int read_times;
    int gain;
    float scale;
} hx711_conf_t;

class HX711 {
    public:
        
        /**
         * @brief Construct a new HX711 object
         * 
         */
        HX711();

        /**
         * @brief Construct a new HX711 object with config
         * 
         * @param conf_hx711 
         */
        HX711(hx711_conf_t *conf_hx711);
        
        ~HX711() { return; };
        
        /**
         * @brief Function to get the load value
         * 
         */
        void getLoad();

        /**
         * @brief Get the scale factor value
         * 
         * @return float 
         */
        float getScale();

        /**
         * @brief Function to power on the hx711 controller
         * 
         */
        void poweron();

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
         * @brief Set the scale factor
         * 
         * @param scaleFactor 
         */
        void setScale(float scaleFactor);

        /**
         * @brief Start background task
         * 
         */
        void startTask();

        /**
         * @brief Stop background task
         * 
         */
        void stopTask();

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
         * @brief Function to wait until hx711 is ready for shift out data
         * 
         * @details It will wait until PDOUT go low or trys bigger then 10
         * 
         * @param delay_ms  time wait between next call
         * @return true 
         * @return false 
         */
        bool wait_ready(unsigned long delay_ms=1);

        float cellload;

    private:

        /**
         * @brief Background Task for reading out hx711 value
         * 
         */
        static void hx711Task(void *pvParameter);

        void init();
        /**
         * @brief Function to shift out the value from hx711 modul.
         * 
         * @note In this function we disable all interrupts to for shifting
         * 
         */
        void _readData();

        /**
         * @brief Helper function to get the average of load.
         * 
         * @details This function will always be called when we read the value. It will take samples of 'n' times 
         *          to get a more accurate result. It will also remove the highest and lowest value.
         * 
         * @param times  number of reading value
         * @return float return the average sum of readings 
         */
        float read_average(uint8_t times);

    private:
        uint8_t trys;
        gpio_num_t _pdsck;
        gpio_num_t _dout;
        int _gain;
        float _oload;
        float _scale;
        uint8_t raw_data[3];
        gpio_config_t pdsck_conf;
        gpio_config_t dout_conf;
        float offset;
        int read_times;
        bool _error;
        float _loadsamples[TARETIMES];
        bool taskrun;
    };


#ifdef __cplusplus
}
#endif