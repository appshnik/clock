# Kitchen alarm clock #

This project is made for small kitchen clock design. Main purpose of kitchen
clock is to show user a current time (and temperature) and let him to set
a timer that reports user when the time expired.

## How do you build and work with project ##

To build the project you should have installed "msp430-gcc" package. It
contains all necessary libraries. If you have already got it - the default
rule of Makefile will build the project. Just type "make" command in your
terminal.

To download your firmware to microcontroller you should have installed
"mspdebug" package. Then check the launchpad is connected to your PC: type
"lsusb" command and find "TexasInstruments..." among output (if it isn't
there - just try to unplug usb-cable and plug it in once again). Type
"make flash" and your program will be transferred to microcontroller.

## Hardware structure ##

![](https://github.com/appshnik/clock/blob/master/kitchen_clock_schem.jpg)

Device consists of:
1. MSP430G2553 controller
2. DS3231 RTC module
3. 1602a LCD 16x2 module
4. keyboard
5. AM2302 humidity/temperature sensor

Controller performs data exchange between modules, provides user with HMI by
LCD module.

RTC module holds current date and time regardless of power supply due to it
has it's own primary power cell. It also performs alarm clock counting.
User manual can be found [here][1]

LCD shows user current signal values. It is 1602A module connected by 4-wire
data bus. Suitable description can be found [here][2].

Keyboard is built of four push-buttons which performs mode and screen
switching and value adjustment.

AM2302 is used to measure current relative humidity and temperature values.
Sensor description can be found [here][3]


[1]: https://datasheets.maximintegrated.com/en/ds/DS3231.pdf
[2]: https://www.newhavendisplay.com/app_notes/SPLC780D.pdf
[3]: https://akizukidenshi.com/download/ds/aosong/AM2302.pdf
