# ArduinoTaikoController

Sketch for Arduino based taiko game controller circuit

## Circuit Setup

For most of the times, pluging the sensors directly into Arduino's analog pins will work.
But for most of the times you'd better connect a resistor in parallel or an RC low-pass circuit for each sensor.

For different types of sensors, you have to determine the amplitute of output level on arduino.
By using a maximum reference voltage and enabling debug info you can see the input voltage level in 1/1024 of the reference voltage.
Then you should choose a reference voltage wisely, using either pre-built 5V/1.1V or external voltage sources.

## Algorithm

This controller program uses a dynamic threshold of sensor input levels to trigger an input.
Everytime a sensor gives a significantly higher input level, the algorithm sets the threshold of this sensor to that input level, and gradually lowers the threshold.
To avoid picking up inputs from other sensors erroneously, when a sensor is recognized as triggered, the threshold level of all other sensors is raised as well.
Also, a cooldown length will be added to all sensors so to ignore the unstable levels near a drum hit.

## Parameters (with suggested values)

#### min_threshold = 20
The minimum level that a trigger is recognized for all sensors.

To determine an optimal value for this level, try enabling debug info.
Usually, this value is only used to ignore sensor noises, but you can use this level as a sensitivity level.

#### cd_length = 8000
The cooldown length of the triggered sensor, in microseconds (=1x10^-6s).

While a sensor is in its cooldown period, no input will be generated for it, but the threshold level would still be updated.
During the cooldown period, the corresponding key of the sensor is kept pressed. When it ended, the key is released.

#### cd_antireso_length = 8000
The cooldown length of sensors other than the triggered one, in microseconds.

When there's a drum hit on one part of the drum, other parts will still have fluctuations in sensor levels.
By using cooldown periods for sensors other than the triggered one, we are able to let these sensors settle down and also get their threshold level updated,
so that there will not be any mistrigger after the cooldown period expires.

#### k_antireso = 0.85
How much the thresholds of every sensor other than the triggered one are raised to, in ratio to the threshold of the triggered sensor.

#### k_decay = 0.96
How fast every threshold level decays, in ratio.

For approximately every millisecond, the threshold value of every channel is multiplied by k_decay.

#### pin[4] = {A3, A0, A2, A1}
The analog input pins of four sensors.

#### key[4] = {'d', 'f', 'j', 'k'}
The key mapping of four sensors.

## Debug Info

If the macro ```DEBUG_OUTPUT``` is enabled, there will be debug info printed via serial. Take a look at your serial monitor.

The first 4 rows indicates current vibration level of the four sensors, and the last 4 rows indicates the minimum level for a sensor to trigger a input;
the symbols in the middle shows current status of the sensors, # for input triggered and * for anti-resonance state.

A typical output could be:

```
0  3  13 63 |         | 0  0  0  0
51 2  11 58 | * * * # | 53 53 53 63
83 5  9  24 | # * * # | 83 70 70 70
```

