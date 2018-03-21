#include "clib.h"
#include "lcd_drv.h"


/* LCD strobe */
void strobe(void)
{
	LCD_E(1);
	__delay_us(1);
	LCD_E(0);	
}

/* LCD busy check */
void lcd_busy(void)
{
	int busy_flag;
	/* configure LCD port as input */
	P2DIR &= ~(LCD_BUS_MASK);
	/* switch LCD to read mode */
	LCD_RS(0);
	LCD_RW(1);

	do {
		strobe();
		busy_flag = P2IN & (1<<7);
		__delay_us(2);
	} while (busy_flag);	
}

/* LCD write */
void lcd_wr(uint8_t data)
{
	int i = 0; 
	/* configure LCD port as output */
	P2DIR |= LCD_BUS_MASK;
	/* swith LCD to write mode */
	LCD_RW(0);
	/* transfer data (MS nible first) */
	while (i <= 4) {
		P2OUT = 0x0;
		P2OUT = (data << i) & LCD_BUS_MASK;
		strobe();
		i += 4;
	}
	
}

/* LCD write instruction*/
void lcd_wr_ins(uint8_t ins_code)
{
	lcd_busy();
	LCD_RS(0);
	lcd_wr(ins_code);
}
/* LCD write data*/
void lcd_wr_dt(uint8_t data)
{
	lcd_busy();
	LCD_RS(1);
	lcd_wr(data);
}
/* LCD write init instruction */
void lcd_wr_init_ins(uint8_t cmd)
{
	P2OUT &= 0x0;
	P2OUT |= cmd;
	strobe();
}
/* LCD initialize */
void lcd_init(void)
{
	/* configure LCD port as output */
	P2DIR |= LCD_BUS_MASK;
	/* configure RS and RW bits */
	LCD_RS(0);
	LCD_RW(0);
	/* start inintialization routine */
	__delay_ms(20);
	lcd_wr_init_ins(0x30);
	__delay_ms(5);
	lcd_wr_init_ins(0x30);
	__delay_us(150);
	lcd_wr_init_ins(0x30);
	/* set interface to 4-bits long */
	lcd_wr_init_ins(0x20);
	/* set number of lines and character font */
	lcd_wr_init_ins(0xC0);
	/* display on */
	lcd_wr_init_ins(0x00);
	lcd_wr_init_ins(0xF0);
	/* display clear */
	lcd_wr_init_ins(0x00);
	lcd_wr_init_ins(0x10);
	/* entry mode set */
	lcd_wr_init_ins(0x00);
	lcd_wr_init_ins(0x60);
}
/* LCD write string */
void lcd_str(const char *str, uint8_t ddram_ptr)
{
	uint8_t symb = 0;
	/* set DDRAM address to write from */
	lcd_wr_ins(ddram_ptr);
	while (*str) {
		symb = *str++;
		lcd_wr_dt(symb);
	}
}
/* clear LCD */
void lcd_clr(void)
{
	lcd_wr_ins(0x01);
}
