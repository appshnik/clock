CC	= msp430-gcc
MMCU	= -mmcu=msp430g2553
CFLAGS	= -Wall -O2 -I.
SRC	= clib.c main.c lcd_drv.c
HDR	= clib.h lcd_drv.h
DEPS 	= $(SRC) $(HDR)
OUT	= clock.fwr
MCUDBG	= mspdebug
DRV	= rf2500

default: $(DEPS)
	$(CC) $(MMCU) -o $(OUT) $(SRC) $(CFLAGS)
load: $(OUT)
	$(MCUDBG) $(DRV)
