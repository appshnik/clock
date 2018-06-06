#ifndef LCD_H
#define LCD_H

#include <msp430.h>

/* LCD instructions */
#define _display_clear	0x01
#define _display_off	0x08
#define _display_on	0x0C
#define _cursor_on	0x0F
#define _entry_mode_set	0x06
#define _set_ddram(adr)	(0x80 | (adr))

#ifndef CPU_F
#define CPU_F		20000000UL		/* oscillator frequency */

/* function macros */
#define __delay_ms(ms)	(__delay_cycles((unsigned long)(CPU_F/1000UL * ms)))
#define __delay_us(us)	(__delay_cycles((unsigned long)(CPU_F/1000000UL * us)))

#endif /* CPU_F */

/* function definitions */
unsigned char lcd_read(void);
void lcd_write(unsigned char val);
void lcd_check_busy(void);
void lcd_wr_instr(unsigned char cmd);
void lcd_wr_symb(unsigned char symb);
void lcd_wr_scr(char *s, unsigned char ddram);
void lcd_wr_str(char *s, unsigned char ddram);
void lcd_init(void);

#endif /* LCD_H */
