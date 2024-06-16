Quickstart
**********

| Here you can find a short example how to use the library in your project. 
| You can also find an example in `example`_ folder.

.. _example: https://github.com/avjui/hx711/tree/master/example

Installation
#############

ESP-IDF
^^^^^^^^

The library contains a `idf_compent.yml` file. So you can install it with the esp-idf packagemanager to be aviable in your project.

To integerate it to your project create a file named `idf_compent.yml` and put following lines in it.

.. code-block:: yml

    dependencies:
        idf: ">=4.4"

        hx711:
            git: https://github.com/avjui/hx711.git


The file was also aviable in the [example folder main dirctory](./example/main/).

After this installation is complete and you can build your project with th hx711 library.

Platformio
^^^^^^^^^^^^

To use this library in platformio you can add `lib_deps` to your configuration file `platformio.ini`

.. code-block:: ini

    lib_deps = 
        git+https://github.com/avjui/hx711

.. warning::
    Because platformio not regiester the `Kconfig` by him self you have to add `Kconfig` with editing `CMakeLists.txt`, or add the config variables to `platfomrio.ini`. 

- First option is to add following lines to `CMakeLists.txt` in the root directory.

.. code-block:: cmake

    get_filename_component(configName "${CMAKE_BINARY_DIR}" NAME)
    list(APPEND kconfigs "${CMAKE_SOURCE_DIR}/.pio/libdeps/${configName}/hx711/Kconfig")

- Second option is to add the config to `platformio.ini`.

.. code-block:: ini

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


Initialization and setup
#########################

At first we must include the header files

.. code-block:: cpp

    #include <hx711.h>

Constructer with default config
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

| After that we can create a Object with the standart config. 
| This means the configuration made by :code:`menuconfig` well be used. 

.. note:: There you can find a :code:`HX711 Configuration` section.

.. code-block:: cpp

    /* Init with default config 
    *  or config make with 'menuconfig' 
    */
    HX711 load;


Constructer with custom config
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
| If you want more control over the configuration you can use the overloaded constructer.
| Ther you can use the :code:`hx711_conf_t` struct.

.. code-block:: cpp

    /* Create custom config */
    hx711_conf_t conf_hx711 = {
        .pin_pdsck = 26,
        .pin_dout = 27,
        .samples = 10,
        .gain = GAIN_128,
        .scale = SCALE,
    };

    /* Init with custom config */
    HX711 load(&conf_hx711);


Tare the scale
^^^^^^^^^^^^^^^^

We can tare automaticly to get the right offset

.. note:: 
    You must be sure that you have nothing on the scale. Otherwise 0 gramm will be set
    within the weight staying on the scale when the tare function will be execute. 

.. code-block:: cpp

    /* Tare the scale */
    load.tare();


An other option is to set the offset manual. Maybe from stored value.

.. code-block:: cpp

    /* Set new offset */
    float newoffset = 46709.0;
    load.setOffset(newoffset);

Usage
######

Perform update of weight value
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

Here we have two options to update the :code:`cellload` value.

One option can be to start the background task.

.. code-block:: cpp

    /* Start background task to periodicly read out value from hx711 modul */
    load.startTask();

The other option is to poll it manuely. Be sure that you periodacly execute this
function to get a new value.

.. code-block:: cpp

    /* Perform read out value from hx711 modul */
    load.update();


Read load value
^^^^^^^^^^^^^^^^

To read the load you can use the variable :code:`cellload`. If you have started
the background task you can read the value, without doing anything else.

.. code-block:: cpp

    /* Read value and print */
    ESP_LOGI("MAIN", "Cell load: [ %.2f gram ] \n", load.cellload);

.. note:: 
    Default we read the value for 10 times to get an average to have a more accurate result.
    When the hx711 modul is set to 10Mhz we get all second one result 10MHz/10.
    After some testing it resuslts that it looks like the modul i testet has about 5Mhz.
    So we get a result every 2 seconds. You can change this behavier by change 
    the :code:`HX711_SAMPLES` variable or by using the constructer with custom config.


If you have choose to poll it manuel you must call :code:`update` before reading.

.. code-block:: cpp
    
    /* Update variable */
    load.update();

    /* Read value and print */
    ESP_LOGI("MAIN", "Cell load: [ %.2f gram ] \n", load.cellload);