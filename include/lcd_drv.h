#include <msp430.h>

#ifndef LCD_H
#define LCD_H

#ifdef OUT
#undef OUT
#endif

#ifndef CPU_F
#define CPU_F		16000000UL	/* MCLK frequency */
#endif


/************** LCD pinout settings *************************************/
#define BUS_WIDTH	4		/* LCD's data bus width (8/4 bits) */
#define LCD_B_PRT	P2		/* MCU's port for LCD's data bus */
#define LCD_C_PRT	P2		/* MCU's port for LCD's control bits */

#define LCD_D0		7		/* port pin for D0 (set 7 if BUS_WIDTH == 4) */
#define LCD_D1		7		/* port pin for D1 (set 7 if BUS_WIDTH == 4) */
#define LCD_D2		7		/* port pin for D2 (set 7 if BUS_WIDTH == 4) */
#define LCD_D3		7		/* port pin for D3 (set 7 if BUS_WIDTH == 4) */
#define LCD_D4		4		/* port pin for D4 */
#define LCD_D5		5		/* port pin for D5 */
#define LCD_D6		6		/* port pin for D6 */
#define LCD_D7		7		/* port pin for D7 */

#define LCD_RS		0		/* port pin for RS */
#define LCD_RW		1		/* port pin for RW */
#define LCD_E		2		/* port pin for E */

#define LCD_B_MSK      (1<<LCD_D0 | \
			1<<LCD_D1 | \
			1<<LCD_D2 | \
			1<<LCD_D3 | \
			1<<LCD_D4 | \
			1<<LCD_D5 | \
			1<<LCD_D6 | \
			1<<LCD_D7)	/* port's pins for data bus */

#define LCD_C_MSK      (1<<LCD_RS | \
			1<<LCD_RW | \
			1<<LCD_E)	/* port's pins for control bits */
/*********************************************************************/
#define _delay		15		/* us, delay for read/write routine timings
						if LCD doesn't work properly - try
						to increase this value */

#ifndef _delay_ms
#define _delay_ms(ms)	(__delay_cycles((unsigned long)(CPU_F/1000UL * ms)))
#endif

#ifndef _delay_us
#define _delay_us(us)	(__delay_cycles((unsigned long)(CPU_F/1000000UL * us)))
#endif

#define _function_set_i	(0x38)
#define _function_set	(BUS_WIDTH == 8) ? (0x38) : (0x28)
#define _display_clear	(0x01)
#define _display_off	(0x08)
#define _display_on	(0x0C)
#define _entry_mode_set	(0x06)
#define _set_ddram(adr)	(0x80 | (adr))

#define cat(f, s)	f ## s
#define xcat(a, b)	cat(a, b)

#define LCD_B_IN	xcat(LCD_B_PRT, IN)
#define LCD_B_OUT	xcat(LCD_B_PRT, OUT)
#define LCD_C_OUT	xcat(LCD_C_PRT, OUT)
#define LCD_B_DIR	xcat(LCD_B_PRT, DIR)
#define LCD_C_DIR	xcat(LCD_C_PRT, DIR)
#define LCD_B_SEL	xcat(LCD_B_PRT, SEL)
#define LCD_C_SEL	xcat(LCD_C_PRT, SEL)
#define LCD_B_SEL2	xcat(LCD_B_PRT, SEL2)
#define LCD_C_SEL2	xcat(LCD_C_PRT, SEL2)

#define _RS(st)		((st) ? (LCD_C_OUT |= 1<<LCD_RS) : (LCD_C_OUT &= ~(1<<LCD_RS)))
#define _RW(st)		((st) ? (LCD_C_OUT |= 1<<LCD_RW) : (LCD_C_OUT &= ~(1<<LCD_RW)))
#define _E(st)		((st) ? (LCD_C_OUT |= 1<<LCD_E) : (LCD_C_OUT &= ~(1<<LCD_E)))

#define _READ_BF	_RS(0), _RW(1)
#define _WRITE_DT	_RS(1), _RW(0)
#define _WRITE_INS	_RS(0), _RW(0)

#define _CLEAR_BUS	LCD_B_OUT &= ~LCD_B_MSK

unsigned char read(void);
void write(unsigned char val);
void check_busy(void);
void wr_instr(unsigned char cmd);
void wr_symb(unsigned char symb);
void wr_scr(char *s, unsigned char ddram);
void wr_str(char *s, unsigned char ddram);
void lcd_init(void);

#endif /* LCD_H */
