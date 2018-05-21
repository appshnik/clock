#include <common.h>
#include <sb.h>

#ifdef OUT
#undef OUT
#endif

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

struct hutemp ht_data;		/* humidity and temperature data */
uint8_t bit_count;		/* number of already received bits */
signed char ht_res;		/* result of reading:	-1 - sensor error
							>0 - wrong data
							 0 - ok */

static uint16_t tar_val;	/* microseconds */

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

void sb_start(void)
{
	_OCCUPY_BUS;
	_SET_SB_OUT(0);
	__delay_ms(25);
	_SET_SB_OUT(1);
	__delay_us(30);
	_RELEASE_BUS;
}

uint8_t sb_resp(void)
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

signed char sb_receive(void)
{
	uint8_t itr;		/* holder for previous interrupt settings */
	static uint8_t failed;	/* counter for invalid data transitions */
	int8_t sign;		/* sign for temperature value */

	itr = SB_IE;
	bit_count = 0;

	/* enable sufficient interrupt */
	TACTL = TACTL & ~TAIE;
	SB_IE = SB_MSK;
	/* wait while reading */
	__delay_ms(100);
	/* restore interrupt settings */
	SB_IE = itr;
	TACTL = TACTL | TAIE;

	/* data validation */
	if (((uint8_t)(ht_data.hum_h + ht_data.hum_l + \
		ht_data.temp_h + ht_data.temp_l)) != (uint8_t)ht_data.ch_sum)
		return ++failed;
	/* calculate temp and humidity data */
	ht_data.hum = ((ht_data.hum_h)<<8 | ht_data.hum_l);
	sign = (ht_data.temp_h & 0x80) ? (-1) : (1);
	ht_data.temp = sign * (((ht_data.temp_h)<<8 | ht_data.temp_l) & 0x7FFF);
	return (failed = 0);
}

/**
  * Function is used to receive next bit from sensor. It is called in port ISR.
  */
signed char sb_get_bit(void)
{
	/* set TimerA to zero */
	TACTL |= TACLR;
	/* wait until high-low transition */
	while (SB_IN & SB_MSK);
	/* compare TAR with predefined values and return corresponding value */
	tar_val = (unsigned int)(TAR) / TA_FAC;
	if ( tar_val >= ONE_MIN_TIME && tar_val <= ONE_MAX_TIME)
		return 1;
	else if ( tar_val >= ZERO_MIN_TIME && tar_val <= ZERO_MAX_TIME)
		return 0;
	else
		return -1;
}

/* Data receiving routine */
signed char sb_read_data(void)
{
	/* send start signal to sensor */
	sb_start();
	/* check if sensor has responded */
	if (sb_resp())
		/* start bit reading sequence */
		return sb_receive();
	else
		return -1;
}

