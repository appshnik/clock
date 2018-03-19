#ifndef HDR
#define HDR

#include <msp430.h>
/* constants definition */
#define OSC_FR	(32768)			// oscilator frequency
#define MAIN_MD	(1)			// MAIN clock mode
#define SEL_MD	(2)			// select clock mode
#define C_SET_MD(4)			// clock SET mode
#define T_SET_MD(3)			// timer SET mode
/* IO definition */
#define SET_B	(1<<0)			// "set" button
#define UP_B	(1<<1)			// "set" button
#define DOWN_B	(1<<2)			// "set" button
#define NEXT_B	(1<<3)			// "set" button
/* event definition */
#define SET	(~P2IN & SET_B)		// "set" button is pressed
#define UP	(~P2IN & UP_B)		// "up" button is pressed
#define DOWN	(~P2IN & DOWN_B)	// "down" button is pressed
#define NEXT	(~P2IN & NEXT_B)	// "next" button is pressed



#endif // HDR
