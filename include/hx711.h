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
#include "esp_log.h"
#include "sdkconfig.h"

#if ESP_IDF_VERSION >= ESP_IDF_VERSION_VAL(5, 0, 0)
    #include <rom/ets_sys.h>
#endif


#define MODUL_HX711           "HX711"

#define HIGH            1
#define LOW             0

/**
 * \defgroup Defaults 
 * 
 * 
 * @{ 
 */
#define SCALE           491.f 
#define OFFSET          46709.0
#define TARESAMPLES       20
#define WAIT_TIME       20
/**@}*/



/**
 * @enum hx711_gain
 * 
 * @brief Enum for set gain to hx711 modul
 * @var hx711_gain::GAIN_128
 * gain factor 128 for channel A
 * 
 * @var hx711_gain::GAIN_64
 * gain factor 64 for channel A
 * 
 * @var hx711_gain::GAIN_32
 * gain factor 32 for channel B
 * 
 */
typedef enum {
    GAIN_128 = 1, 
    GAIN_32 = 2,   
    GAIN_64 = 3,   
} hx711_gain;


/**
 * @struct hx711_conf_t
 * @brief Struct for configuration
 * 
 * @var hx711_conf_t::pin_pdsck 
 * assign clockpin
 * @var hx711_conf_t::pin_dout 
 * assign data out pin
 * @var hx711_conf_t::samples
 * set read times for average read
 * @var hx711_conf_t::gain 
 * set the gain
 * @var hx711_conf_t::scale 
 * set the scale factor
 * 
 */
typedef struct {
    int pin_pdsck;
    int pin_dout;
    uint8_t samples;
    hx711_gain gain;
    float scale;
} hx711_conf_t;

class HX711 {
    public:
        
        /**
         * @brief Construct a new HX711 object 
         * 
         * @note  The constructer can be used without any parameter. If no parameter is
         *        given it will use the standart pins set by `menuconfig`.
         * 
         */
        HX711();

        /**
         * @brief Construct a new HX711 object with config
         * 
         * @details If you want to setup the hx711 from programm you can use this function.
         * 
         * @param conf_hx711 Config struct for hx711 modul. See ::hx711_conf_t.
         */
        HX711(hx711_conf_t *conf_hx711);
        
        ~HX711() { return; };
        
        /**
         * @brief Get the the offset of the hx711 modul.
         * 
         * @return Return the offset value.
         */
        float getOffset();

        /**
         * @brief Get the scale factor value
         * 
         * @return Return the scale factor
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
         * @details Here you can set the gain. There are 3 option aviable.
         *          See ::hx711_gain for more information.
         * 
         * @param gain Set the gain 
         */
        void setGain(hx711_gain gain);

        /**
         * @brief Set the offset for hx711 modul
         * 
         * @param newoffset Offset value
         */
        void setOffset(float newoffset);

        /**
         * @brief Function to set the scale factor.
         * 
         * @details This factor will be used to get right value based on which load cell is
         *          connected to the hx711
         * 
         * @param scaleFactor Set the scale factor
         */
        void setScale(float scaleFactor);

        /**
         * @brief Function to bring the hx711 to standby mode
         * 
         */
        void standby();

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
         * @details In this function we will read the sensor for the number ::TARESAMPLES to get the offset.
         *          In the start of the function we will also read the load until 2 seconds to get a more 
         *          accurate result. 
         * 
         */
        void tare();

        /**
         * @brief Function to manuel trigger value read
         * 
         * @details This function can be used to trigger the read. It will be used when no
         *          background task running. This function will trigger ::wait_ready.
         * 
         */
        void update();

        /**
         * @brief Function to wait until hx711 is ready for shift out data
         * 
         * @details It will wait until PDOUT went LOW or it will be more the 10 trys.
         *          In this case ::_error will get true and it will stop to read value.
         *          In the case ::_error will get true maybe there is a problem with your wiring.
         * 
         * @note Note we read the value for 10 times to get an average to have a more accurate result.
         *       When the hx711 modul is set to 10Mhz we get all second one result 10MHz/10.
         *       After some testing it resuslts that it looks like the modul i testet has about 5Mhz.
         *       So we get a result every 2 seconds. 
         * 
         * @param delay_ms Time wait between next call
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
         * @details This function will always be called when we read the value. It will take samples of `n` times 
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
        float _loadsamples[TARESAMPLES];
        bool taskrun;
    };


#ifdef __cplusplus
}
#endif