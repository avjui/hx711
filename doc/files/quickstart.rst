Quickstart
**********

| Here you can find a short example how to use the library in your project. 
| You can also find an example in `example`_ folder.

.. _example: https://github.com/avjui/hx711/tree/master/example

Initialization and setup
^^^^^^^^^^^^^^^^^^^^^^^^^^

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