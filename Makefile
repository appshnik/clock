CC	= msp430-gcc
MMCU	= -mmcu=msp430g2553
CFLAGS	= -Wall -O2 -I.
PSRC	= pain.c
SRC	= clib.c main.c lcd_drv.c sb_drv.c
HDR	= clib.h lcd_drv.h sb_drv.h
DEPS 	= $(SRC) $(HDR)
OUTBIN	= clock.fwr
OUTELF	= ./hex/clock.elf
OUTLST	= ./hex/clock.lst
OUTHEX	= ./hex/clock.hex
MCUDBG	= mspdebug
DRV	= rf2500

default: $(DEPS)
	$(CC) $(MMCU) -o $(OUTBIN) $(SRC) $(CFLAGS)
hux: $(DEPS)
	$(CC) -C $(MMCU) -o $(OUTELF) $(SRC)
	msp430-objdump -DS $(OUTELF) > $(OUTLST)
	msp430-objcopy -O ihex $(OUTELF) $(OUTHEX)
	cp -r hex/ /home/share/	
load: $(OUT)
	$(MCUDBG) $(DRV)
