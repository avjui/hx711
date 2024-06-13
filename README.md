# HX711 Library

## Table of Contents

- [About](#about)
- [Getting Started](#getting_started)
- [Usage](#usage)
- [TODO](#todo)
- [Contributing](../CONTRIBUTING.md)

## About <a name = "about"></a>

This library is for the hx711 modul to read weight load.
It is build for esp-idf framework and written in C++! 

## Getting Started <a name = "getting_started"></a>

These instructions will get you a copy of the project up and running on your local machine for development and testing purposes. See [deployment](#deployment) for notes on how to deploy the project on a live system.


### Installing

A step by step series of examples that tell you how to get a development env running.

Say what the step will be

```
Give the example
```

And repeat

```
until finished
```

End with an example of getting some data out of the system or using it for a little demo.

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

## Todo <a name = "todo"></a>

 - add function to set scale factor