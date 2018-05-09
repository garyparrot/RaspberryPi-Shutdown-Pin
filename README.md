# RaspberryPi-Shutdown-Pin
You can shutdown your raspberry pi by given voltage(HIGH or LOW) to specified pin

One day I block myself out of raspberry pi because I shutdown ssh daemon by accident.
And you know raspberry pi doesn't have a power button, so I do it on my own.

All these code is writing in C, it may require some permission to access gpiomem file and when you execute it.
Because of this program would shutdown your raspberry pi, you need to give it permission by sudo or thing like that.

## Configuration
The default shutdown pin is GPIO 21, To change specified pin, Modify S\_INPUT marco value.
Note that each pin only support one kind of pull resisitor.
It might be pull-up-resistor or pull-down-resistor, you need to look up BCM2835 datasheet(althougt it is old and buggy but still provide some useful info) in page 102 and 103.
```
Before:     #define S_INPUT 21
After:      #define S_INPUT <The GPIO pin you like>

If your specified pin only support pull-up resistor, make below changes.
Before:     int presistor = PULL_RESISTOR_DOWN;
After:      int presistor = PULL_RESISTOR_UP;
```

Because of some architecture difference, the I/O Peripherals physical address is different between pi 1 and pi 2/3.
For raspberry pi 1 user, make the change to source code as below demonstration.
```
Before:     #define BASE_IO_PREI 0x3F000000
After:      #define BASE_IO_PREI 0x20000000
```

If you don't have a file called "/dev/gpiomem", you need to make the changes.
```
Before:     #define MEMFILE "/dev/gpiomem"
After:      #define MEMFILE "/dev/mem"
```
