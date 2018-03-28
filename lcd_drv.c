#include "lcd_drv.h"

#ifndef CPU_F
#define CPU_F		16000000UL	// MCLK frequency
#endif
/* LCD initialization routine indicator */
unsigned char init_flag;

/* reads data from LCD */
unsigned char read(void)
{	
	unsigned char ret_val = 0x00;
	if (BUS_WIDTH == 8) {
		LCD_B_DIR &= ~LCD_B_MSK;
		_CLEAR_BUS;
		_delay_us(_delay);
		_E(1);
		_delay_us(_delay);
		ret_val = LCD_B_IN & LCD_B_MSK;
		_E(0);
		_delay_us(_delay);
	}
	else if (BUS_WIDTH == 4) {
		LCD_B_DIR &= ~LCD_B_MSK;
		_CLEAR_BUS;
		_delay_us(_delay);
		_E(1);
	_delay_us(_delay);
		ret_val = LCD_B_IN & LCD_B_MSK;
		_E(0);
		_delay_us(_delay);
		_E(1);
		_delay_us(_delay);
		ret_val |= ((LCD_B_IN>>4) & ~(LCD_B_MSK));
		_E(0);
		_delay_us(_delay);
	}
	return ret_val;	
}
/* writes any data to LCD */
void write(unsigned char val)
{
	LCD_B_DIR |= LCD_B_MSK;
	if ((BUS_WIDTH == 8) ||	(init_flag)) {
		_CLEAR_BUS;
		_delay_us(_delay);
		_E(1);
		if (BUS_WIDTH == 8)
		LCD_B_OUT |=  ((((val & (1<<7))?(1):(0))<<LCD_D7 | \
			 	((val & (1<<6))?(1):(0))<<LCD_D6 | \
			 	((val & (1<<5))?(1):(0))<<LCD_D5 | \
			 	((val & (1<<4))?(1):(0))<<LCD_D4 | \
			 	((val & (1<<3))?(1):(0))<<LCD_D3 | \
			 	((val & (1<<2))?(1):(0))<<LCD_D2 | \
			 	((val & (1<<1))?(1):(0))<<LCD_D1 | \
			 	((val & (1<<0))?(1):(0))<<LCD_D0) & LCD_B_MSK);
		else if (BUS_WIDTH == 4)
		LCD_B_OUT |=  ((((val & (1<<7))?(1):(0))<<LCD_D7 | \
			 	((val & (1<<6))?(1):(0))<<LCD_D6 | \
			 	((val & (1<<5))?(1):(0))<<LCD_D5 | \
			 	((val & (1<<4))?(1):(0))<<LCD_D4 ) & LCD_B_MSK);


		_delay_us(_delay);
		_E(0);
		_delay_us(_delay);
	}
	else if (BUS_WIDTH == 4) {
		_CLEAR_BUS;
		_delay_us(_delay);
		_E(1);
		LCD_B_OUT |=  ((((val & (1<<7))?(1):(0))<<LCD_D7 | \
			 	((val & (1<<6))?(1):(0))<<LCD_D6 | \
			 	((val & (1<<5))?(1):(0))<<LCD_D5 | \
			 	((val & (1<<4))?(1):(0))<<LCD_D4) & LCD_B_MSK);

		_delay_us(_delay);
		_E(0);
		_delay_us(_delay);
		_CLEAR_BUS;
		_delay_us(_delay);
		_E(1);
		LCD_B_OUT |=  ((((val & (1<<3))?(1):(0))<<LCD_D7 | \
			 	((val & (1<<2))?(1):(0))<<LCD_D6 | \
			 	((val & (1<<1))?(1):(0))<<LCD_D5 | \
			 	((val & (1<<0))?(1):(0))<<LCD_D4) & LCD_B_MSK);

		_delay_us(_delay);
		_E(0);
		_delay_us(_delay);
	}
}
/* waits while LCD's controller is busy */
void check_busy(void)
{	
	unsigned char res = 0x00;
	_READ_BF;
	do {
		res = read();
	} while (res & (1<<LCD_D7));
}
/* writes instruction to LCD */
void wr_instr(unsigned char cmd)
{
	if (init_flag)
		_delay_ms(5);
	else
		check_busy();
	_WRITE_INS;
	write(cmd);
}
/* writes data (symbol) to LCD */
void wr_symb(unsigned char symb)
{
	_WRITE_DT;
	write(symb);
}
/* clears whole LCD and writes string to address*/
void wr_scr(char *s, unsigned char ddram)
{
	wr_instr(_display_clear);
	wr_instr(_set_ddram(ddram));
	while (*s) {
		wr_symb(*s++);
	}
}

/* clears only line to be written and writes string to address*/
void wr_str(char *s, unsigned char ddram)
{
	unsigned char i;
	wr_instr(_set_ddram(ddram & 0xF0));
	for (i = 0; i < 16; i++)
		wr_symb(' ');
	wr_instr(_set_ddram(ddram));
	while (*s) {
		wr_symb(*s++);
	}
}
/* LCD initialization routine */
void lcd_init(void)
{
	// GPIO initialization
	LCD_B_DIR |= LCD_B_MSK;
	LCD_C_DIR |= LCD_C_MSK;

	LCD_B_OUT &= ~LCD_B_MSK;
	LCD_C_OUT &= ~LCD_C_MSK;

	LCD_B_SEL &= ~LCD_B_MSK;	
	LCD_C_SEL &= ~LCD_C_MSK;
	
	LCD_B_SEL2 &= ~LCD_B_MSK;
	LCD_C_SEL2 &= ~LCD_C_MSK;
	// initialization routine
	init_flag = 1;
	_delay_ms(50);
	wr_instr(_function_set_i);
	_delay_ms(10);
	wr_instr(_function_set_i);
	_delay_us(150);
	wr_instr(_function_set_i);
	
	_delay_ms(10);
	wr_instr(_function_set);
	_delay_ms(10);
	init_flag = 0;
// function set (if interface is 4-bit)
	if (BUS_WIDTH == 4) {
		wr_instr(_function_set);
	}
// display off
	wr_instr(_display_off);
	_delay_ms(10);
// display clear
	wr_instr(_display_clear);
	_delay_ms(10);
// entry mode set
	wr_instr(_entry_mode_set);
	_delay_ms(10);
// display on
	wr_instr(_display_on);	
	_delay_ms(10);
// set ddram address
	wr_instr(_set_ddram(0x00));	
	_delay_ms(10);
}
