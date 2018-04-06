#include <msp430.h>
#include <clib.h>

#ifndef SB_H
#define SB_H

#ifdef OUT
#undef OUT
#endif

#ifndef CPU_F
#define CPU_F		20000000UL	/* MCLK frequency */
#endif


/************** Single-bus pinout settings *************************************/
#define SB_PRT		P2		/* MCU's port for LCD's data bus */
#define SDA		3		/* port pin for SDA */

#define SB_MSK     	(1<<SDA)	/* port's pins for data bus */
/*********************************************************************/

#define MAX_RESP_TIME	32000
#define ONE_MIN_TIME	65	/* us */
#define ONE_MAX_TIME	75	/* us */
#define ZERO_MIN_TIME	20	/* us */
#define ZERO_MAX_TIME	30	/* us */


#ifndef _delay_ms
#define _delay_ms(ms)	(__delay_cycles((unsigned long)(CPU_F/1000UL * ms)))
#endif

#ifndef _delay_us
#define _delay_us(us)	(__delay_cycles((unsigned long)(CPU_F/1000000UL * us)))
#endif

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

struct hutemp {
	uint8_t hum_h;		/* humidity high byte */
	uint8_t hum_l;		/* humidity low byte */
	uint8_t temp_h;		/* temperature high byte */
	uint8_t temp_l;		/* temperature low byte */
	uint8_t ch_sum;		/* check sum */
	int hum;		/* current humidity */
	int temp;		/* current humidity */
};

void sb_start(void);
uint8_t sb_resp(void);
signed char sb_receive(void);
signed char sb_get_bit(void);
signed char sb_read_data(void);

#endif /* SB_H */
