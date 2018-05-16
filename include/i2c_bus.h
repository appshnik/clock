#ifndef USCI_LIB
#define USCI_LIB

#define SDA_PIN	0x02	/* msp430x261x UCB0SDA pin */
#define SCL_PIN	0x04	/* msp430x261x UCB0SCL pin */

void i2c_receiveinit(unsigned char slave_address, unsigned char prescale);
void i2c_transmitinit(unsigned char slave_address, unsigned char prescale);
void i2c_receive(unsigned char byteCount, const unsigned char *field);
void i2c_transmit(unsigned char byteCount, const unsigned char *field);
unsigned char i2c_check_slave(unsigned char slave_address);
unsigned char i2c_busy(void);

#endif /* USCI_LIB */
