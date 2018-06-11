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
		__delay_ms(2500UL);
		sb_rec_oper = 1;

		sprintf(str, "bus_st: %x", P2OUT);
		lcd_wr_str(str, 0x00);
		/* data reading from HT sensor */
		if (sb_rec_oper) {
			sb_receive();

			sb_rec_oper = 0;
		}
	}
	return 0;
}
