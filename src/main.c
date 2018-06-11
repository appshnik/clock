#include <common.h>
#include <board.h>
#include <lcd.h>
#include <sb.h>
#include <itrc.h>
#include <one_wire.h>


int main(void)
{
	char str[17];
	uint8_t sb_rec_oper;	/* data reading is active */
	struct hutemp *ht_data;
	int i = 0;

	/* initialization */

	init_device();
	lcd_init();
	lcd_wr_str("Kitchen clock", 0x01);
	lcd_wr_str("Initialization", 0x40);
	__delay_ms(500UL);

	__disable_interrupt();
	/* init callbacks */
	set_cb(&p2_isr_cb, one_wire_get_bit, NULL);
	/* main program loop */
	while (1) {

		sb_rec_oper = 1;
		sprintf(str, "Attempt #%d", ++i);
		lcd_wr_scr(str, 0x00);
		__delay_ms(1500UL);
		/* data reading from HT sensor */
		if (sb_rec_oper) {
			ht_data = sb_receive();
			sprintf(str, "hum: %d", *(int*)(ht_data));
/*			lcd_wr_str(str, 0x00);
*/
		}
	}
	return 0;
}
