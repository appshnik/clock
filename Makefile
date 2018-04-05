CROSS_COMPILE	= msp430-
CC		= $(CROSS_COMPILE)gcc
LD		= $(CROSS_COMPILE)gcc
MMCU		= -mmcu=msp430g2553
CCFLAGS		= -Wall -Os -Iinclude -MD
LDFLAGS		= -Wall
SRC_FOL		= src/

OBJS		= clib.o \
		  main.o \
		  lcd_drv.o \
		  sb_drv.o

OUTELF		= clock.elf
OUTLST		= clock.lst
OUTHEX		= clock.hex
MCUDBG		= mspdebug
DRV		= rf2500

.PHONY: default flash clean distclean

%.o: $(SRC_FOL)%.c
	@echo [CC] $<
	@$(CC) -c $< $(MMCU) -o $@ $(CCFLAGS)

default: $(OBJS)
	@echo [LD] $(OUTELF)
	@$(LD) $(MMCU) -o $(OUTELF) $^ $(LDFLAGS)
	msp430-objdump -DS $(OUTELF) > $(OUTLST)
	msp430-objcopy -O ihex $(OUTELF) $(OUTHEX)
	cp $(OUTELF) $(OUTLST) $(OUTHEX) /home/share/

flash: $(OUTELF)
	$(MCUDBG) $(DRV) "prog $(OUTELF)"

clean:
	rm -rf $(OUTELF) $(OUTLST) $(OUTHEX) *.d *.o

distclean:
	make clean

-include $(OBJS:.o=.d)

