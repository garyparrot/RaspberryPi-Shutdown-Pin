# RaspberryPi-Shutdown-Pin
You can shutdown your raspberry pi by given voltage(HIGH or LOW) to specified pin

One day I block myself out of raspberry pi because I shutdown ssh daemon by accident.
And you know raspberry pi doesn't have a power button, so I do it on my own.

All these code is writing in C, it may require some permission to access gpiomem file when you execute it.

## Configuration
The default shutdown pin is GPIO 21, if you want to change the speicied shutdown pin number, find below source code and modify it.
And remember that the avaliable Pull up/down resistor is different between each pin, you need to look up BCM2835 datasheet(althougt it is old and buggy but still provide some useful info) in page 102 and 103.
```
Before:     #define S_INPUT (21)
After:      #define S_INPUT (<The GPIO pin you like>)

If your specified pin only support pull-up resistor, make below changes.
Before:     GPPUD = PULL_RESISTOR_DOWN
After:      GPPUD = PULL_RESISTOR_UP
```

Because of some architecture difference, the GPIO position is different between 1 and 2/3.
For raspberry pi 1 user, make the change to source code as below demonstration.
```
Before:     #define BASE_IO_PREI 0x3F000000
After:      #define BASE_IO_PREI 0x20000000
```

If you don't have a file called "/dev/gpiomem", you need to make the changes.
But remember that with the permssion of access "/dev/mem", this program got the ability to access physical memory, this might cause some security problem.
```
Before:     #define MEMFILE ("/dev/gpiomem")
After:      #define MEMFILE ("/dev/mem")
```
