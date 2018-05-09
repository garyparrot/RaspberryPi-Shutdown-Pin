# RaspberryPi-Shutdown-Pin

## Getter Started
You can shutdown your raspberry pi by given voltage(HIGH or LOW) to specified pin

One day I block myself out of raspberry pi because I shutdown ssh daemon by accident.
And you know raspberry pi doesn't have a power button, so I do it on my own.

All these code is writing in C, it may require some permission to access gpiomem file and when you execute it.
Because of this program would shutdown your raspberry pi, you need to give it permission by sudo or thing like that.

## Enviroment
A raspberry pi 2/3 installed Linux with a C compiler.
If you are using a pi 1, you need to modify some source code, Please follow section "Old raspberry pi configuration" steps.

## Setup
After download this repo, do these command in the same directory which contain those source code.
```
gcc GPIO.c shutdown-pin.c -Wall -o shutdown-pin
```
After this command, there should be a execute file whcih called shutdown-pin.

## Run it
Below command run this program, the default specified pin is GPIO 21, when you give Vcc(HIGH voltage) to this pin, raspberry should start shutdown.
```
sudo ./shutdown-pin
```

## Test
To test if this program work fine, execute following commands
```
sudo ./shutdown-pin --false-alarm
```
the "--false-alarm" tag won't reboot your raspberry pi if this flag is specified, but sending a "False alarm!" string to stdout
Now connect GPIO 21 to Vcc, wait for 10 seconds, there should be a string "False alarm!" show up in your terminal.

## Flags
* --false-alarm : Replace shutdown function for a string output.
* --no-persist  : This program would keep the specified pin in corresponding mode(Input, Pull resistor), use this flag to disable this function, NOTE THAT IF SOMETHING MODIFY YOU SPECIFIED PIN MODE, IT MIGHT BLOW YOUR RASPBERRY PI ONCE YOU CONNECT THAT PIN TO VCC OR GND!!!
* --quiet       : This program would stop sending any output.
* --pin n       : Specified a pin as shutdown trigger, n should be a number which between 0 and 53

## Old raspberry pi configuration
Because of some architecture difference, the I/O Peripherals physical address is different between pi 1 and pi 2/3.
For raspberry pi 1 user, make the change to source code as below demonstration.
```
Before:     #define BASE\_IO\_PREI 0x3F000000
After:      #define BASE\_IO\_PREI 0x20000000
```

If you don't have a file called "/dev/gpiomem", you need to make the changes.
```
Before:     #define MEMFILE "/dev/gpiomem"
After:      #define MEMFILE "/dev/mem"
```

Old raspberry pi doesn't have GPIO 21, which is my default pin.
You can modify default pin as below
```
Before:     #define DEFAULT_PIN 21
After:      #define DEFAULT_PIN <THE DEFAULT PIN YOU LIKE>
``` 

## Troubleshooting
If you running into any problem, Please contact with me.

