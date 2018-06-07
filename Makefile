INCLUDES	= -Iinclude
DEFINES		= -D__STDC_VERSION__=199401L

CROSS_COMPILE	?= msp430-
CC		= $(CROSS_COMPILE)gcc
LD		= $(CROSS_COMPILE)gcc
OBJDUMP		= $(CROSS_COMPILE)objdump
MCU		= msp430g2553

CFLAGS		= -Wall -Os
CFLAGS		+= -MD -pedantic -ansi -Werror -Wextra
CFLAGS		+= $(INCLUDES)
CFLAGS		+= $(DEFINES)

LDFLAGS		= -mmcu=$(MCU)

OBJS		= src/board.o \
<<<<<<< HEAD
		  src/common.o \
		  src/i2c_bus.o \
		  src/lcd.o \
		  src/main.o \
		  src/one_wire.o \
=======
>>>>>>> 407aa6d2d0a41791b9d5ee03d9d428707aa2743e
		  src/i2c_bus.o \
		  src/lcd.o \
		  src/main.o \
		  src/rtc.o \
		  src/sb.o

OUTELF		= clock.elf
OUTLST		= clock.lst

MSP430_INC	= -I/usr/msp430/include -I/usr/lib/gcc/msp430/4.6.3/include
MSP430_DEF	= -D__MSP430G2553__
MSP430_CFLAGS	= $(MSP430_INC) $(MSP430_DEF)
SPARSE_FLAGS	= -Wsparse-all $(MSP430_CFLAGS) $(CFLAGS)
CPPCHECK_FLAGS	= --enable=all $(MSP430_CFLAGS) $(INCLUDES) $(DEFINES)
CPPCHECK_FLAGS	+= --inline-suppr -D__STDC__

default: $(OBJS)
	@echo LD $(OUTELF)
	@$(LD) $(LDFLAGS) -o $(OUTELF) $^

%.o: %.c
	@echo CC $<
	@$(CC) -c $< -mmcu=$(MCU) -o $@ $(CFLAGS)

ctags:
	@ctags $(OBJS:.o=.c)

dump: $(OUTELF)
	@echo OBJDUMP $(OUTLST)
	@$(OBJDUMP) -DS $(OUTELF) > $(OUTLST)

flash: $(OUTELF)
	mspdebug rf2500 "prog $(OUTELF)"

clean:
	@-rm -rf $(OUTELF) $(OUTLST) tags
	@-rm -f $(OBJS)
	@-rm -f $(OBJS:.o=.d)

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

.PHONY: default flash clean distclean ctags

-include $(OBJS:.o=.d)

