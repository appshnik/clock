INCLUDES	= -Iinclude
DEFINES		= -D__STDC_VERSION__=199401L

CROSS_COMPILE	= msp430-
CC		= $(CROSS_COMPILE)gcc
LD		= $(CROSS_COMPILE)gcc
OBJ_DMP		= $(CROSS_COMPILE)objdump
MCU		= msp430g2553
MMCU		= -mmcu=$(MCU)

CFLAGS		= -Wall -Os
CFLAGS		+= -MD -pedantic -ansi -Werror -Wextra
CFLAGS		+= $(INCLUDES)
CFLAGS		+= $(DEFINES)

LDFLAGS		= -Wall $(MMCU)

OBJS		= src/board.o \
		  src/main.o \
		  src/lcd.o \
		  src/sb.o \
		  src/rtc.o \
		  src/TI_USCI_I2C_master.o

OUTELF		= clock.elf
OUTLST		= clock.lst
MCUDBG		= mspdebug
DRV		= rf2500

MSP430_INC	= -I/usr/msp430/include -I/usr/lib/gcc/msp430/4.6.3/include
MSP430_DEF	= -D__MSP430G2553__
MSP430_CFLAGS	= $(MSP430_INC) $(MSP430_DEF)
SPARSE_FLAGS	= -Wsparse-all $(MSP430_CFLAGS) $(CFLAGS)
CPPCHECK_FLAGS	= --enable=all $(MSP430_CFLAGS) $(INCLUDES) $(DEFINES)
CPPCHECK_FLAGS	+= --inline-suppr -D__STDC__

default: $(OBJS)
	@echo [LD] $(OUTELF)
	@$(LD) $(LDFLAGS) -o $(OUTELF) $^
	@echo [OBJ_DMP] $(OUTLST)
	@$(OBJ_DMP) -DS $(OUTELF) > $(OUTLST)

%.o: %.c
	@echo [CC] $<
	@$(CC) -c $< $(MMCU) -o $@ $(CFLAGS)

ctags:
	@ctags $(OBJS:.o=.c)

flash: $(OUTELF)
	$(MCUDBG) $(DRV) "prog $(OUTELF)"

clean:
	@-rm -rf $(OUTELF) $(OUTLST) tags
	@-rm -f $(OBJS)
	@-rm -f $(OBJS:.o=.d)

distclean:
	@make clean

_check_sparse:
	@echo "---> Checking with sparse..."
	@find . -type f -name '*.{c,h}' -exec sparse $(SPARSE_FLAGS) {} \;
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

.PHONY: default flash clean distclean ctags

-include $(OBJS:.o=.d)

