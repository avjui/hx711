# HX711 Library

## Table of Contents

- [About](#about)
- [Getting Started](#getting_started)
- [Usage](#usage)
- [TODO](#todo)
- [License](./LICENSE)

## About <a name = "about"></a>

This library is for the hx711 modul to read weight load.
It is build for esp-idf framework and written in C++! 

## Getting Started <a name = "getting_started"></a>

Goal of this project is to provide an easy to use library for the hx711 modul written in C++! 

The main featers are:
 - Easy to use
 - Reading value from hx711 modul in background task
 - Create average value from `x` samples to get more accurate results
 - Remove highest and lowest value to get more accurate results


### Installing

#### ESP-IDF

The library contains a `idf_compent.yml` file. So you can install it with the esp-idf packagemanager to be aviable in your project.

To integerate it to your project create a file named `idf_compent.yml` and put following lines in it.

```
dependencies:
  idf: ">=4.4"

  hx711:
    git: https://gitlab.renejuen.at/av_jui/hx711.git
```
:file_folder: The file was also aviable in the [example folder main dirctory](./example/main/).

After this installation is complete and you can build your project with th hx711 library.

##### Platformio

To use this library in platformio you can add `lib_deps` to your configuration file `platformio.ini`

```
lib_deps = 
    git+https://gitlab.renejuen.at/av_jui/hx711
```
:warning: Because platformio not regiester the `Kconfig` by him self you also must put following lines to your `CMakeLists.txt` in the root directory. 

```
get_filename_component(configName "${CMAKE_BINARY_DIR}" NAME)
list(APPEND kconfigs "${CMAKE_SOURCE_DIR}/.pio/libdeps/${configName}/hx711/Kconfig")
```

## Usage <a name = "usage"></a>

Add notes about how to use the system.

```
#include <hx711.h>
```

After the we declare it

```
HX711 load();
```

Then we can tare the weight scale

```
load.tare()
```

Then we can start the background task and read the value

```
load.startTask();
for(;;) 
{
    ESP_LOGI("MAIN", "Weight : %.2f g", load.getLoad());
    vTaskDelay(2000/portTICK_PERIOD_MS);
}

```

You can find an example in the [example folder](./example)

## Todo <a name = "todo"></a>

 - add function to set scale factor