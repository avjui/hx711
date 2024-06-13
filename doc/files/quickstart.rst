Quickstart
**********

Here you can find a short example to use the library.

Initialization
===============

At first we must include the header files

.. code-block:: cpp

    #include <hx711.h>

After that we can create a Object with the standart config. That means the 
configuration made by :code:`menuconfig` well be used.

.. code-block:: cpp

    HX711 load;

If you want more control over the configuration you can use the overloaded constructer.
Ther you can use the :code:`hx711_conf_t` struct.

.. code-block:: cpp

    int average_reads = 10;
    hx711_conf_t conf_hx711 = {
        .pin_pdsck = 26,
        .pin_dout = 27,
        .read_times = average_reads,
        .gain = GAIN_128,
        .scale = SCALE,
    };
    HX711 load(&conf_hx711);

First we tare to get automaticly the right offset

.. code-block:: cpp

    load.tare();

After that we can start the background task.

.. code-block:: cpp

    load.startTask();

Read Load
==============

To read the load you can use the variable :code:`cellload`.

.. note:: 
    Note we read the value for 10 times to get an average to have a more accurate result.
    When the hx711 modul is set to 10Mhz we get all second one result 10MHz/10.
    After some testing it resuslts that it looks like the modul i testet has about 5Mhz.
    So we get a result every 2 seconds. 

.. code-block:: cpp

    printf("Cell load: [ %fg ] \n",load.cellload);
