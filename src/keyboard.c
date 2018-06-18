#include <common.h>
#include <keyboard.h>

void debounce(void)
{
	;
}

void prev_key_pressed(void *priv)
{

	void *loc_priv = priv;
	loc_priv = (int *)(loc_priv) + 1;
	debounce();
/* this shit is to be moved to screen file */
#if 0
 	if (c_scr%2 == 0)
		c_scr = (c_scr == HT_SCR)?(T_SCR):(c_scr - 2);
	else {
		if (c_ind == 0) {
			c_scr -= 1;
			lcd_wr_instr(_display_on);
		}
		else
			c_ind -= 1;
	}
#endif
}

void up_key_pressed(void *priv)
{
	void *loc_priv = priv;
	loc_priv = (int *)(loc_priv) + 1;
	debounce();
#if 0
 	if (c_scr%2 == 0) {
		c_scr += 1;
		c_ind = 0;
		lcd_wr_instr(_cursor_on);
	}
	else if (c_scr == DTS_SCR) {
		*(dt_ptr[c_ind]) += 1;	/* increment parameter */
		if (c_ind <= 2)
			dt_ch |= DS_CHANGED;
		else
			dt_ch |= TS_CHANGED;
	}
	else if (c_scr == TS_SCR) {
		*(t_ptr[c_ind]) += 1;	/* increment parameter */
		if (c_ind == 3)
			dt_ch |= TMS_CHANGED;
	}
#endif
}

void down_key_pressed(void *priv)
{
	void *loc_priv = priv;
	loc_priv = (int *)(loc_priv) + 1;
	debounce();
#if 0
	if (c_scr%2 == 0) {
		c_scr += 1;
		c_ind = 0;
		lcd_wr_instr(_cursor_on);
	}
	else if (c_scr == DTS_SCR) {
		*(dt_ptr[c_ind]) -= 1;	/* decrement parameter */
		if (c_ind <= 2)
			dt_ch |= DS_CHANGED;
		else
			dt_ch |= TS_CHANGED;
	}
	else if (c_scr == TS_SCR) {
		*(t_ptr[c_ind]) -= 1;	/* decrement parameter */
		if (c_ind == 3)
			dt_ch |= STOP_ALARM;
	}
#endif
}

void next_key_pressed(void *priv)
{
	void *loc_priv = priv;
	loc_priv = (int *)(loc_priv) + 1;
	debounce();
#if 0
	if (c_scr%2 == 0)
		c_scr = (c_scr == T_SCR)?(HT_SCR):(c_scr + 2);
	else {
		if (c_ind == max_ind) {
			c_scr -= 1;
			lcd_wr_instr(_display_on);
		}
		else
			c_ind += 1;
	}

#endif
}
