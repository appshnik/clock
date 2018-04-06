# clock
kitchen clock project

This project is made for small kitchen clock design. Main purpose of kitchen
clock is to show user a current time (and temperature) and let him to set
a timer that reports user when the time expired.

Device consists of:
	1) MSP430G2553 controller;
	2) RTC module;
	3) LCD module;
	4) keyboard (it's made of four buttons);
	5) DHT22 humidity/temperature sensor.



Program flow:
initialization - 	GPIO init
			WDT init
			I2C init
			MCLK init

main loop -		mode control
			read from RTC
			read from TS
			check if timer has expired

interrupts -		timer ISR (for changing device mode)
			GPIO isr (for keyboard events)

