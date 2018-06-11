#include <common.h>
#include <itrc.h>

struct cb_t p2_isr_cb;	/* port 2 ISR callback */
#if 0
static struct cb_t p1_isr_cb;	/* port 1 ISR callback */
static struct cb_t ta0_isr_cb;	/* timer A0 ISR callback */
static struct cb_t wdt_isr_cb;	/* WDT ISR callback */
#endif

/*8888888888888888888888*/
#include <lcd.h>
#include <stdio.h>
char deb_str[17];
/*8888888888888888888888*/

/* sets callback */
void set_cb(struct cb_t *isr_cb, isr_cb_t cb, void *param)
{
	isr_cb->cb = cb;
	isr_cb->param = param;
}

/* executes callback */
void callback(struct cb_t *isr_cb)
{
	isr_cb->cb(isr_cb->param);
}

/* port 2 interrupt service routine */
__attribute__((interrupt(PORT2_VECTOR)))
/* cppcheck-suppress unusedFunction */
void p2_isr(void)
{
	if (P2IFG & 0x08)
		callback(&p2_isr_cb);
	P2IFG = 0x00;
}
