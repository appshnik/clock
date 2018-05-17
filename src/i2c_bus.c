#include <common.h>
#include <i2c_bus.h>

signed char byte_num;
unsigned char *i2c_rx_buf;
unsigned char *i2c_tx_buf;

/*
 * This function initializes the USCI module for master-receive operation.
 */
void i2c_receiveinit(unsigned char slave_address, unsigned char prescale)
{
	P3SEL |= SDA_PIN + SCL_PIN;		/* Assign I2C pins to USCI_B0 */
	UCB0CTL1 = UCSWRST;			/* Enable SW reset */
	UCB0CTL0 = UCMST + UCMODE_3 + UCSYNC;	/* I2C Master, synchronous mode */

	UCB0CTL1 = UCSSEL_2 + UCSWRST;		/* Use SMCLK, keep SW reset */
	UCB0BR0 = prescale;			/* set prescaler */
	UCB0BR1 = 0;
	UCB0I2CSA = slave_address;		/* set slave address */
	UCB0CTL1 &= ~UCSWRST;			/* Clear SW reset and resume */
	UCB0I2CIE = UCNACKIE;
	IE2 = UCB0RXIE;				/* Enable RX interrupt */

}

/*
 * This function initializes the USCI module for master-transmit operation.
 */
void i2c_transmitinit(unsigned char slave_address, unsigned char prescale)
{
	P3SEL |= SDA_PIN + SCL_PIN;		/* Assign I2C pins to USCI_B0 */
	UCB0CTL1 = UCSWRST;			/* Enable SW reset */
	UCB0CTL0 = UCMST + UCMODE_3 + UCSYNC;	/* I2C Master, synchronous mode */
	UCB0CTL1 = UCSSEL_2 + UCSWRST;		/* Use SMCLK, keep SW reset*/
	UCB0BR0 = prescale;			/* set prescaler */
	UCB0BR1 = 0;
	UCB0I2CSA = slave_address;		/* Set slave address*/
	UCB0CTL1 &= ~UCSWRST;			/* Clear SW reset and resume */
	UCB0I2CIE = UCNACKIE;
	IE2 = UCB0TXIE;				/* Enable TX ready interrupt*/
}

/*
 * This function is used to start an I2C commuincation in master-receiver mode.
 */
void i2c_receive(unsigned char byteCount, const unsigned char *field)
{
	i2c_rx_buf = field;
	if ( byteCount == 1 ) {
		byte_num = 0 ;
		__disable_interrupt();
		UCB0CTL1 |= UCTXSTT;		/* I2C start condition*/
		while (UCB0CTL1 & UCTXSTT);	/* Start condition sent?*/
		UCB0CTL1 |= UCTXSTP;		/* I2C stop condition*/
		__enable_interrupt();
	} else if ( byteCount > 1 ) {
		byte_num = byteCount - 2;
		UCB0CTL1 |= UCTXSTT;		/* I2C start condition*/
	} else {
		 while (1);			/* illegal parameter*/
	}
}

/*
 * This function is used to start an I2C commuincation in master-transmit mode.
 */
void i2c_transmit(unsigned char byteCount, const unsigned char *field)
{
	i2c_tx_buf = field;
	byte_num = byteCount;
	UCB0CTL1 |= UCTR + UCTXSTT;		/* I2C TX, start condition */
}

/*
 * This function is used to look for a slave address on the I2C bus.
 */
unsigned char i2c_check_slave(unsigned char slave_address)
{
	unsigned char ie2_bak, slaveadr_bak, ucb0i2cie, returnValue;

	ucb0i2cie = UCB0I2CIE;			/* restore old UCB0I2CIE*/
	ie2_bak = IE2;				/* store IE2 register*/
	slaveadr_bak = UCB0I2CSA;		/* store old slave address*/
	UCB0I2CIE &= ~ UCNACKIE;		/* no NACK interrupt*/
	UCB0I2CSA = slave_address;		/* set slave address*/
	IE2 &= ~(UCB0TXIE + UCB0RXIE);		/* no RX or TX interrupts*/
	__disable_interrupt();
	UCB0CTL1 |= UCTR + UCTXSTT + UCTXSTP;	/* I2C TX, start condition*/
	while (UCB0CTL1 & UCTXSTP);		/* wait for STOP condition*/
	returnValue = !(UCB0STAT & UCNACKIFG);
	__enable_interrupt();
	IE2 = ie2_bak;				/* restore IE2*/
	UCB0I2CSA = slaveadr_bak;		/* restore old slave address*/
	UCB0I2CIE = ucb0i2cie;			/* restore old UCB0CTL1*/
	return returnValue;
}

/*
 * This function is used to check if there is communication in progress.
 */
unsigned char i2c_busy(void)
{
	return (UCB0STAT & UCBBUSY);
}


#pragma vector = USCIAB0RX_VECTOR
__interrupt void USCIAB0RX_ISR(void)
{
	if (UCB0STAT & UCNACKIFG) {		/* STOP if slave sends NACK*/
		UCB0CTL1 |= UCTXSTP;
		UCB0STAT &= ~UCNACKIFG;
	}
}

#pragma vector = USCIAB0TX_VECTOR
__interrupt void USCIAB0TX_ISR(void)
{
	if (IFG2 & UCB0RXIFG) {
		if (byte_num == 0) {
			UCB0CTL1 |= UCTXSTP;		/* I2C stop condition*/
			*i2c_rx_buf = UCB0RXBUF;
			i2c_rx_buf++;
		} else {
			*i2c_rx_buf = UCB0RXBUF;
			i2c_rx_buf++;
			byte_num--;
		}
	} else {
		if (byte_num == 0) {
			UCB0CTL1 |= UCTXSTP;		/* I2C stop condition*/
			IFG2 &= ~UCB0TXIFG;		/* Clear USCI_B0 TX */
		} else {
			UCB0TXBUF = *i2c_tx_buf;
			i2c_tx_buf++;
			byte_num--;
		}
	}
}
