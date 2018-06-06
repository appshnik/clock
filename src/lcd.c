#include <lcd.h>

#ifdef OUT
#undef OUT
#endif

#ifndef CPU_F
#define CPU_F		20000000UL		/* oscillator frequency */
/** IMPORTANT!: CPU frequency must be greater or equal to 10 MHz otherwise
 *  __delay_us(us) macro will be equal to 0. If you use CPU_F less then 10 MHz
 *  you should modify this macro or just substitute it by __delay_ms(ms) macro
 *  (it will slow process down a little).
 */

/* function macros */
#define __delay_ms(ms)	(__delay_cycles((unsigned long)(CPU_F/1000UL * ms)))
#define __delay_us(us)	(__delay_cycles((unsigned long)(CPU_F/1000000UL * us)))

#endif /* CPU_F */

/************** LCD pinout settings *************************************/
#ifndef BUS_WIDTH
#define BUS_WIDTH	4	/* LCD's data bus width (8/4 bits) */
#endif

#define LCD_B_PRT	P2	/* MCU's port for LCD's data bus */
#define LCD_C_PRT	P2	/* MCU's port for LCD's control bits */

#define LCD_D0		7	/* port pin for D0 (set 7 if BUS_WIDTH == 4) */
#define LCD_D1		7	/* port pin for D1 (set 7 if BUS_WIDTH == 4) */
#define LCD_D2		7	/* port pin for D2 (set 7 if BUS_WIDTH == 4) */
#define LCD_D3		7	/* port pin for D3 (set 7 if BUS_WIDTH == 4) */
#define LCD_D4		4	/* port pin for D4 */
#define LCD_D5		5	/* port pin for D5 */
#define LCD_D6		6	/* port pin for D6 */
#define LCD_D7		7	/* port pin for D7 */

#define LCD_RS		0	/* port pin for RS */
#define LCD_RW		1	/* port pin for RW */
#define LCD_E		2	/* port pin for E */

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
#define _delay		15	/* us, delay for read/write routine timings
					if LCD doesn't work properly - try to
					increase this value */

/* LCD setting instructions (are used during initialization only) */
#define _function_set_i	0x38
#define _function_set	(BUS_WIDTH == 8) ? (0x38) : (0x28)

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

#define _RS(st)		((st) ? \
			(LCD_C_OUT |= 1<<LCD_RS) : \
			(LCD_C_OUT &= ~(1<<LCD_RS)))
#define _RW(st)		((st) ? \
			(LCD_C_OUT |= 1<<LCD_RW) : \
			(LCD_C_OUT &= ~(1<<LCD_RW)))
#define _E(st)		((st) ? \
			(LCD_C_OUT |= 1<<LCD_E) : \
			(LCD_C_OUT &= ~(1<<LCD_E)))

#define _READ_BF	_RS(0), _RW(1)
#define _WRITE_DT	_RS(1), _RW(0)
#define _WRITE_INS	_RS(0), _RW(0)

#define _CLEAR_BUS	LCD_B_OUT &= ~LCD_B_MSK

/* LCD initialization routine indicator */
static unsigned char init_flag;

/* reads data from LCD */
unsigned char lcd_read(void)
{
	unsigned char ret_val = 0x00;
	if (BUS_WIDTH == 8) {
		LCD_B_DIR &= ~LCD_B_MSK;
		_CLEAR_BUS;
		__delay_us(_delay);
		_E(1);
		__delay_us(_delay);
		ret_val = LCD_B_IN & LCD_B_MSK;
		_E(0);
		__delay_us(_delay);
	}
	else if (BUS_WIDTH == 4) {
		LCD_B_DIR &= ~LCD_B_MSK;
		_CLEAR_BUS;
		__delay_us(_delay);
		_E(1);
		__delay_us(_delay);
		ret_val = LCD_B_IN & LCD_B_MSK;
		_E(0);
		__delay_us(_delay);
		_E(1);
		__delay_us(_delay);
		ret_val |= ((LCD_B_IN>>4) & ~(LCD_B_MSK));
		_E(0);
		__delay_us(_delay);
	}
	return ret_val;
}
/* writes any data to LCD */
void lcd_write(unsigned char val)
{
	LCD_B_DIR |= LCD_B_MSK;
	if ((BUS_WIDTH == 8) ||	(init_flag)) {
		_CLEAR_BUS;
		__delay_us(_delay);
		_E(1);
		if (BUS_WIDTH == 8)
			LCD_B_OUT |=  ((((val & (1<<7))?(1):(0))<<LCD_D7 | \
			 		((val & (1<<6))?(1):(0))<<LCD_D6 | \
				 	((val & (1<<5))?(1):(0))<<LCD_D5 | \
				 	((val & (1<<4))?(1):(0))<<LCD_D4 | \
				 	((val & (1<<3))?(1):(0))<<LCD_D3 | \
				 	((val & (1<<2))?(1):(0))<<LCD_D2 | \
				 	((val & (1<<1))?(1):(0))<<LCD_D1 | \
				 	((val & (1<<0))?(1):(0))<<LCD_D0) & \
					LCD_B_MSK);
		else if (BUS_WIDTH == 4)
			LCD_B_OUT |=  ((((val & (1<<7))?(1):(0))<<LCD_D7 | \
				 	((val & (1<<6))?(1):(0))<<LCD_D6 | \
				 	((val & (1<<5))?(1):(0))<<LCD_D5 | \
				 	((val & (1<<4))?(1):(0))<<LCD_D4 ) & \
					LCD_B_MSK);
		__delay_us(_delay);
		_E(0);
		__delay_us(_delay);
	}
	else if (BUS_WIDTH == 4) {
		_CLEAR_BUS;
		__delay_us(_delay);
		_E(1);
		LCD_B_OUT |=  ((((val & (1<<7))?(1):(0))<<LCD_D7 | \
			 	((val & (1<<6))?(1):(0))<<LCD_D6 | \
			 	((val & (1<<5))?(1):(0))<<LCD_D5 | \
			 	((val & (1<<4))?(1):(0))<<LCD_D4) & LCD_B_MSK);

		__delay_us(_delay);
		_E(0);
		__delay_us(_delay);
		_CLEAR_BUS;
		__delay_us(_delay);
		_E(1);
		LCD_B_OUT |=  ((((val & (1<<3))?(1):(0))<<LCD_D7 | \
			 	((val & (1<<2))?(1):(0))<<LCD_D6 | \
			 	((val & (1<<1))?(1):(0))<<LCD_D5 | \
			 	((val & (1<<0))?(1):(0))<<LCD_D4) & LCD_B_MSK);

		__delay_us(_delay);
		_E(0);
		__delay_us(_delay);
	}
}
/* waits while LCD's controller is busy */
void lcd_check_busy(void)
{
	unsigned char res = 0x00;
	_READ_BF;
	do {
		res = lcd_read();
	} while (res & (1<<LCD_D7));
}
/* writes instruction to LCD */
void lcd_wr_instr(unsigned char cmd)
{
	if (init_flag)
		__delay_ms(5);
	else
		lcd_check_busy();
	_WRITE_INS;
	lcd_write(cmd);
}
/* writes data (symbol) to LCD */
void lcd_wr_symb(unsigned char symb)
{
	_WRITE_DT;
	lcd_write(symb);
}
/* clears whole LCD and writes string to address*/
void lcd_wr_scr(char *s, unsigned char ddram)
{
	lcd_wr_instr(_display_clear);
	lcd_wr_instr(_set_ddram(ddram));
	while (*s) {
		lcd_wr_symb(*s++);
	}
}

/* clears only line to be written and writes string to address*/
void lcd_wr_str(char *s, unsigned char ddram)
{
	unsigned char i;
	lcd_wr_instr(_set_ddram(ddram & 0xF0));
	for (i = 0; i < 16; i++)
		lcd_wr_symb(' ');
	lcd_wr_instr(_set_ddram(ddram));
	while (*s) {
		lcd_wr_symb(*s++);
	}
}
/* LCD initialization routine */
void lcd_init(void)
{
	/* GPIO initialization */
	LCD_B_DIR |= LCD_B_MSK;
	LCD_C_DIR |= LCD_C_MSK;

	LCD_B_OUT &= ~LCD_B_MSK;
	LCD_C_OUT &= ~LCD_C_MSK;

	LCD_B_SEL &= ~LCD_B_MSK;
	LCD_C_SEL &= ~LCD_C_MSK;

	LCD_B_SEL2 &= ~LCD_B_MSK;
	LCD_C_SEL2 &= ~LCD_C_MSK;
	/* initialization routine */
	init_flag = 1;
	__delay_ms(50);
	lcd_wr_instr(_function_set_i);
	__delay_ms(10);
	lcd_wr_instr(_function_set_i);
	__delay_us(150);
	lcd_wr_instr(_function_set_i);

	__delay_ms(10);
	lcd_wr_instr(_function_set);
	__delay_ms(10);
	init_flag = 0;
	/* function set (if interface is 4-bit) */
	if (BUS_WIDTH == 4) {
		lcd_wr_instr(_function_set);
	}
	/* display off */
	lcd_wr_instr(_display_off);
	__delay_ms(10);
	/* display clear */
	lcd_wr_instr(_display_clear);
	__delay_ms(10);
	/* entry mode set */
	lcd_wr_instr(_entry_mode_set);
	__delay_ms(10);
	/* display on */
	lcd_wr_instr(_display_on);
	__delay_ms(10);
	/* set ddram address */
	lcd_wr_instr(_set_ddram(0x00));
	__delay_ms(10);
}
