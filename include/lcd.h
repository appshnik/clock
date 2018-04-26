#ifndef LCD_H
#define LCD_H

/* LCD instructions */
#define _function_set_i	(0x38)
#define _function_set	(BUS_WIDTH == 8) ? (0x38) : (0x28)
#define _display_clear	(0x01)
#define _display_off	(0x08)
#define _display_on	(0x0C)
#define _cursor_on	(0x0F)
#define _entry_mode_set	(0x06)
#define _set_ddram(adr)	(0x80 | (adr))

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

