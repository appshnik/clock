# Kitchen alarm clock #

This project is made for small kitchen clock design. Main purpose of kitchen
clock is to show user a current time (and temperature) and let him to set
a timer that reports user when the time expired.

## Hardware structure ##

Device consists of:
1. MSP430G2553 controller
2. RTC module
3. LCD 16x2 module
4. keyboard
5. AM2302 humidity/temperature sensor

### MSP430 controller ###

Controller performs data exchange between modules, provides user with HMI by
LCD module.

### RTC module ###

Module holds current date and time regardless of power supply due to it has
it's own primary power cell. It also performs alarm clock counting.

### LCD module ###

LCD shows user current signal values. It is 1602A module connected by 4-wire
data bus. Suitable description can be found [here][1].

### Keyboard ###

Keyboard is built of four push-buttons which performs mode and screen switching
and value adjustment.

### AM2302 sensor ###

Sensor is used to measure current relative humidity and temperature values.
Sensor description can be found [here][2]


[1]: https://www.newhavendisplay.com/app_notes/SPLC780D.pdf
[2]: https://akizukidenshi.com/download/ds/aosong/AM2302.pdf

