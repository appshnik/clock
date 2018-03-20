#ifndef LCD_DRV
#define LCD_DRV

#define LCD_BUS_MASK	(0xF0)						// four-bit bus mode
#define LCD_RS(st)	((st) ? (P1OUT |= (1<<0)) : (P1OUT &= ~(1<<0)))	// P1.0
#define LCD_RW(st)	((st) ? (P1OUT |= (1<<1)) : (P1OUT &= ~(1<<1)))	// P1.1
#define LCD_E(st)	((st) ? (P1OUT |= (1<<2)) : (P1OUT &= ~(1<<2)))	// P1.2

void strobe(void);
void lcd_busy(void);
void lcd_wr(unsigned short int);
void lcd_wr_ins(unsigned short int);
void lcd_wr_dt(unsigned short int);
void lcd_wr_init_ins(unsigned short int);
void lcd_init(void);

#endif // LCD_DRV
