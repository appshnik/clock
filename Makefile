INCLUDES	= -Iinclude
DEFINES		= -D__STDC_VERSION__=199401L

CROSS_COMPILE	= msp430-
CC		= $(CROSS_COMPILE)gcc
LD		= $(CROSS_COMPILE)gcc
MCU		= msp430g2553
MMCU		= -mmcu=$(MCU)

CFLAGS		= -Wall -Os
CFLAGS		+= -MD -pedantic -ansi -Werror -Wextra
CFLAGS		+= $(INCLUDES)
CFLAGS		+= $(DEFINES)

LDFLAGS		= -Wall $(MMCU)

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

MSP430_INC	= -I/usr/msp430/include -I/usr/lib/gcc/msp430/4.6.3/include
MSP430_DEF	= -D__MSP430G2553__
MSP430_CFLAGS	= $(MSP430_INC) $(MSP430_DEF)
SPARSE_FLAGS	= -Wsparse-all $(MSP430_CFLAGS) $(CFLAGS)
CPPCHECK_FLAGS	= --enable=all $(MSP430_CFLAGS) $(INCLUDES) $(DEFINES)
CPPCHECK_FLAGS	+= -D__STDC__

.PHONY: default flash clean distclean ctags

%.o: $(SRC_FOL)%.c
	@echo [CC] $<
	@$(CC) -c $< $(MMCU) -o $@ $(CFLAGS)

default: $(OBJS)
	@echo [LD] $(OUTELF)
	@$(LD) $(LDFLAGS) -o $(OUTELF) $^
	$(CROSS_COMPILE)objdump -DS $(OUTELF) > $(OUTLST)
	$(CROSS_COMPILE)objcopy -O ihex $(OUTELF) $(OUTHEX)
	cp $(OUTELF) $(OUTLST) $(OUTHEX) /home/share/

ctags:
	ctags $(SRC_FOL)*

flash: $(OUTELF)
	$(MCUDBG) $(DRV) "prog $(OUTELF)"

clean:
	rm -rf $(OUTELF) $(OUTLST) $(OUTHEX) *.d *.o tags

distclean:
	make clean

_check_sparse:
	@echo "---> Checking with sparse..."
	@find . -type f -name '*.[ch]' -exec sparse $(SPARSE_FLAGS) {} \;
	@echo

_check_cppcheck:
	@echo "---> Checking with cppcheck..."
	@cppcheck $(CPPCHECK_FLAGS) .
	@echo

__check_clang: $(OBJS)

_check_clang:
	@echo "---> Checking with clang..."
	@$(MAKE) clean >/dev/null 2>/dev/null
	@$(MAKE) CFLAGS="$(MSP430_CFLAGS) $(CFLAGS)" \
		CROSS_COMPILE="scan-build $(CROSS_COMPILE)" __check_clang
	@$(MAKE) clean >/dev/null 2>/dev/null
	@echo

check: _check_sparse _check_cppcheck _check_clang

-include $(OBJS:.o=.d)

