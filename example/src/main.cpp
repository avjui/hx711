
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"

#include <hx711.h>

#define MAIN    "HX711 - Example"

extern "C" void app_main(void)
{
    HX711 load;
    ESP_LOGI(MAIN, "Starting tare the weight scale! Please be shure nothing is on the weight scale.");
    load.tare();
    ESP_LOGI(MAIN, "Finished! Let´s start the background task.");
    load.startTask();

    for(;;) 
    {

        ESP_LOGI(MAIN, "Weight : %.2f g", load.cellload);
        vTaskDelay(2000/portTICK_PERIOD_MS);
    }


};