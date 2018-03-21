#ifndef LCD_DRV
#define LCD_DRV

#define LCD_BUS_MASK	(0xF0)						// four-bit bus mode
#define LCD_RS(st)	((st) ? (P1OUT |= (1<<0)) : (P1OUT &= ~(1<<0)))	// P1.0
#define LCD_RW(st)	((st) ? (P1OUT |= (1<<1)) : (P1OUT &= ~(1<<1)))	// P1.1
#define LCD_E(st)	((st) ? (P1OUT |= (1<<2)) : (P1OUT &= ~(1<<2)))	// P1.2

/* Makes strobe impulse to confirm LCD data/instruction transfer */
void strobe(void);
/* Waits while LCD is busy */
void lcd_busy(void);
/* Writes something to LCD 4-wire bus */
void lcd_wr(uint8_t);
/* Writes instruction to LCD */
void lcd_wr_ins(uint8_t);
/* Writes data to LCD */
void lcd_wr_dt(uint8_t);
/* Writes instruction to LCD in initialization mode */
void lcd_wr_init_ins(uint8_t);
/* Performs LCD initialization routine */
void lcd_init(void);
/* Writes string 'str' to LCD starting from 'ddram_ptr' address */
void lcd_str(const char *str, uint8_t ddram_ptr);
/* Clears LCD */
void lcd_clr(void);

#endif // LCD_DRV
