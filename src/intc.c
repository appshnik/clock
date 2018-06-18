#include <common.h>
#include <intc.h>
#include <ow.h>

#define CB_ARR_LEN	8

struct cb_t {
	isr_cb_t cb;	/* function to be executed */
	void *priv;	/* parameters for the function */
};

static struct cb_t p1_isr_cb[CB_ARR_LEN];
static struct cb_t p2_isr_cb[CB_ARR_LEN];

/* sets callback */
void intc_p1_add_cb(uint8_t pin, isr_cb_t cb, void *priv)
{
	p1_isr_cb[pin].cb = cb;
	p1_isr_cb[pin].priv = priv;
}

void intc_p2_add_cb(uint8_t pin, isr_cb_t cb, void *priv)
{
	p2_isr_cb[pin].cb = cb;
	p2_isr_cb[pin].priv = priv;
}

/* port 1 interrupt service routine */
__attribute__((interrupt(PORT1_VECTOR)))
void p1_isr(void)
{
	uint8_t i;
	for (i = 0; i < CB_ARR_LEN; i++) {
		if (P1IFG & BIT(i) && p1_isr_cb[i].cb != NULL)
			p1_isr_cb[i].cb(p1_isr_cb[i].priv);
	}
	P1IFG = 0x00;
}

/* port 2 interrupt service routine */
__attribute__((interrupt(PORT2_VECTOR)))
void p2_isr(void)
{
	uint8_t i;
	for (i = 0; i < CB_ARR_LEN; i++) {
		if (P2IFG & BIT(i) && p2_isr_cb[i].cb != NULL)
			p2_isr_cb[i].cb(p2_isr_cb[i].priv);
	}
	P2IFG = 0x00;
}
