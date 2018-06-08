#include <one_wire.h>

#ifdef OUT
#undef OUT
#endif

#ifndef CPU_F
#define CPU_F		20000000UL		/* oscillator frequency */
#define TA_FAC		(CPU_F/1000000UL)	/* ticks per microsecond */
/** IMPORTANT!: CPU frequency must be greater or equal to 10 MHz otherwise
  * __delay_us(us) macro will be equal to 0. If you use CPU_F less then 10 MHz
  * you should modify this macro or just substitute it by __delay_ms(ms) macro
  * (it will slow process down a little).
  */

/* function macros */
#define __delay_ms(ms)	(__delay_cycles((unsigned long)(CPU_F/1000UL * ms)))
#define __delay_us(us)	(__delay_cycles((unsigned long)(CPU_F/1000000UL * us)))

#endif /* CPU_F */

/************** Single-bus pinout settings ***************************/
#define SB_PRT		P2		/* MCU's port for LCD's data bus */
#define SDA		3		/* port pin for SDA */

#define SB_MSK     	(1<<SDA)	/* port's pins for data bus */
/*********************************************************************/

#define MAX_RESP_TIME	32000
#define ONE_MIN_TIME	65	/* us */
#define ONE_MAX_TIME	75	/* us */
#define ZERO_MIN_TIME	20	/* us */
#define ZERO_MAX_TIME	30	/* us */

#define cat(f, s)	f ## s
#define xcat(a, b)	cat(a, b)

#define SB_IN		xcat(SB_PRT, IN)
#define SB_OUT		xcat(SB_PRT, OUT)
#define SB_DIR		xcat(SB_PRT, DIR)
#define SB_SEL		xcat(SB_PRT, SEL)
#define SB_SEL2		xcat(SB_PRT, SEL2)
#define SB_IE		xcat(SB_PRT, IE)

#define _RELEASE_BUS	SB_DIR &= ~SB_MSK
#define _OCCUPY_BUS	SB_DIR |= SB_MSK
#define _SET_SB_OUT(x)	(x) ? (SB_OUT |= SB_MSK) : (SB_OUT &= ~SB_MSK)
#define _SB_STATE	(SB_IN & SB_MSK)


static uint16_t bit_count;	/* number of already received bits */
uint16_t *p_bit_count = &bit_count;

static bool buffer[8*16];	/* buffer for received data */

/*8888888888888888888888*/
#include <lcd.h>
#include <stdio.h>
char deb_str[17];
/*8888888888888888888888*/

/* interrupt service routine for SB pin */
/**
  * This ISR serves to calculate time periods of 1's and 0's
  * during data transition through single-bus interface. ISR starts
  * with low-high transition and counts time period. After signal's
  * high-low transition it checks whether time period corresponds to "1"
  * or "0" and writes result to corresponding destination.
  */

/* main idea */
/**
  * User's function initiates data reading through single-bus interface.
  * After start signal it disables any interrupts on this port and enables
  * the one that belongs to desired port pin. Function starts a cycle which is
  * to be performed while data transfer isn't finished (some flag is switched).
  * Then interrupts settings are restored (all data receiving/storing job is
  * implemented in ISR).
  */

/* Generates START condition on the bus */
void one_wire_start(void)
{
	_OCCUPY_BUS;
	_SET_SB_OUT(0);
	__delay_ms(25);
	_SET_SB_OUT(1);
	__delay_us(30);
	_RELEASE_BUS;
}

uint8_t one_wire_resp(void)
{
	unsigned char res;
	int i;
	res = 1;
	/* wait until sensor responses "low" */
	i = MAX_RESP_TIME;
	while (_SB_STATE && i--);
	if (i <= 0)
		return 0;
	/* wait until sensor responses "high" */
	i = MAX_RESP_TIME;
	while (!_SB_STATE && i--);
	if (i <= 0)
		return 0;
	/* wait until sensor starts data transfer */
	i = MAX_RESP_TIME;
	while (_SB_STATE && i--);
	if (i <= 0)
		return 0;
	return res;
}

/**
  * Function is used to receive next bit from sensor. It is called in port ISR.
  */
void one_wire_get_bit(void *param)
{
	char *par = param;
	signed char bit;
	uint16_t tar_val;	/* microseconds expired */

	*par += 1;

	/* set TimerA to zero */
	TACTL |= TACLR;
	/* wait until high-low transition */
	while (SB_IN & SB_MSK);
	/* compare TAR with predefined values and return corresponding value */
	tar_val = (unsigned int)(TAR) / TA_FAC;
	if (tar_val >= ONE_MIN_TIME && tar_val <= ONE_MAX_TIME)
		bit = 1;
	else if (tar_val >= ZERO_MIN_TIME && tar_val <= ZERO_MAX_TIME)
		bit = 0;
	else
		bit = -1;
	/* ignore first short impulse from sensor */
	if (bit < 0)
		goto _end;
	if (bit >= 0) {
		buffer[*p_bit_count] = bit;
	}
_end:
	*p_bit_count = *p_bit_count + 1;
}

/* Data receiving routine */
bool *one_wire_read_data(uint16_t bit_number)
{
	/* struct to hold previous interrupts settings */
	struct p_itr_set {
		uint8_t p1;
		uint8_t p2;
		uint8_t wdt;
		uint8_t ta0;
	} itr;
	struct p_itr_set *p_itr = &itr;
	memset(p_itr, 0, sizeof(itr));

	bit_count = 0;

	__disable_interrupt();
	/* save interrupt settings and disable unwanted one's */
	p_itr->p1 = P1IE;
	P1IE = 0x0;
	p_itr->p2 = P2IE;
	P2IE = SB_MSK;
	p_itr->wdt = IE1;
	IE1 = 0x0;

/*
	sprintf(deb_str, "%x", (int)TACTL);
	lcd_wr_str(deb_str, 0x00);
*/
	/* send start signal to sensor */
	one_wire_start();
	/* check if sensor has responded */
	if (one_wire_resp()) {
		/* start bit reading sequence */
		__enable_interrupt();
/*		while (*p_bit_count < bit_number);
*/
		{unsigned long int i;
		for (i = 0; i <= 4200000000UL; i++);}
	} else {
		return NULL;
	}

	bit_number += 0;
	__disable_interrupt();
	sprintf(deb_str, "P2IE: %x", P2IE);
	lcd_wr_str(deb_str, 0x00);
	sprintf(deb_str, "bit_count   %d", (int)bit_count);
	lcd_wr_str(deb_str, 0x40);
	/* restore interrupt settings */
	P2IE = p_itr->p2 & ~SB_MSK;
	P1IE = p_itr->p1;
	IE1 = p_itr->wdt;
	TACTL = p_itr->ta0;

	return buffer;
}
