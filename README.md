# HX711 Library

<img src="doc/_static/logo.png" width="200" height="200">

![platformio build](https://github.com/avjui/hx711/actions/workflows/build.yml/badge.svg)  ![GitHub Issues or Pull Requests](https://img.shields.io/github/issues/avjui/hx711)  [![License: GPL v3](https://img.shields.io/badge/License-GPLv3-blue.svg)](https://www.gnu.org/licenses/gpl-3.0)

## Table of Contents

- [HX711 Library](#hx711-library)
  - [Table of Contents](#table-of-contents)
  - [About ](#about-)
  - [Getting Started ](#getting-started-)
    - [Installing ](#installing-)
      - [ESP-IDF ](#esp-idf-)
      - [Platformio ](#platformio-)
  - [Usage ](#usage-)
  - [Documentation ](#documentation-)
  - [Todo ](#todo-)

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


### Installing <a name = "installing"></a>

#### ESP-IDF <a name = "esp-idf"></a>

The library contains a `idf_compent.yml` file. So you can install it with the esp-idf packagemanager to be aviable in your project.

To integerate it to your project create a file named `idf_compent.yml` and put following lines in it.

```
dependencies:
  idf: ">=4.4"

  hx711:
    git: https://github.com/avjui/hx711.git
```
:file_folder: The file was also aviable in the [example folder main dirctory](./example/main/).

After this installation is complete and you can build your project with th hx711 library.

#### Platformio <a name = "platformio"></a>

To use this library in platformio you can add `lib_deps` to your configuration file `platformio.ini`

```
lib_deps = 
    git+https://github.com/avjui/hx711
```
:warning: Because platformio not regiester the `Kconfig` by him self you have to add `Kconfig` with editing `CMakeLists.txt`, or add the config variables to `platfomrio.ini`.

- First option is to add following lines to `CMakeLists.txt` in the root directory.

```
get_filename_component(configName "${CMAKE_BINARY_DIR}" NAME)
list(APPEND kconfigs "${CMAKE_SOURCE_DIR}/.pio/libdeps/${configName}/hx711/Kconfig")
```

- An other option is to add the config to `platformio.ini`.

```
[hx711]
; Build flags for hx711 library
build_flags=
    -D CONFIG_HX711_PIN_PDSCK=26 ; clock pin
    -D CONFIG_HX711_PIN_PDOUT=25 ; out pin
    -D CONFIG_HX711_SCALE=491    ; scale factor
    -D CONFIG_HX711_TARETIME=20  ; samples we use for tare       
    -D CONFIG_HX711_SAMPLES=10   ; samples we use for avarge value
    -D CONFIG_HX711_WAIT_TIME=2  ; time to wait when toggle pins
    -D CONFIG_HX711_GAIN=1       ; Gain channel A 128=1, 32=3, channel B 64=2 

    [env:...]
    ....

    build_flags = 
        ${hx711.build_flags}


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

## Documentation <a name = "documentation"></a>

For more information you can have a look at the [documentation](https://avjui.github.io/hx711/).


## Todo <a name = "todo"></a>

 - add function to set scale factor