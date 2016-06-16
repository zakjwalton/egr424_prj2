CC := arm-none-eabi-gcc
CCFLAGS := -Wall -O3 -march=armv7-m -mcpu=cortex-m3 -mthumb -mfix-cortex-m3-ldrd

STELLARIS := /home/l337/egr424/StellarisWare

INCLUDE := -I$(STELLARIS) -I$(STELLARIS)/boards/ek-lm3s6965
LIBS := -L$(STELLARIS)/driverlib/gcc-cm3
LDFLAGS := -ldriver-cm3 -T hello.ld --entry ResetISR -Wl,--gc-sections
PROGRAM := hello.elf

SRCDIR = src
BLDDIR = build
BINDIR = bin
SYMDIR = symbols

SRCS = $(wildcard $(SRCDIR)/*.c)
OBJS = $(patsubst $(SRCDIR)%.c,$(BLDDIR)%.o, $(SRCS))
SYMS = $(patsubst $(SRCDIR)%.c,$(SYMDIR)%.s, $(SRCS))

all: $(BINDIR)/$(PROGRAM)

debug: CCFLAGS += -DDEBUG -g
debug: $(BINDIR)/$(PROGRAM)

symbols: $(SYMS)

dump: $(BINDIR)/$(PROGRAM)
	openocd -f program.cfg

test:
	sudo minicom /dev/ttyUSB0

$(BLDDIR)/%.o: $(SRCDIR)/%.c
	@mkdir -p $(dir $@)
	$(CC) -c -o $@ $^ $(INCLUDE) $(CCFLAGS)

$(BINDIR)/$(PROGRAM): $(OBJS)
	@mkdir -p $(dir $@)
	$(CC) -o $@ $^ $(LIBS) $(LDFLAGS)

$(SYMDIR)/%.s: $(SRCDIR)/%.c
	@mkdir -p $(dir $@)
	$(CC) -S -o $@ $^ $(INCLUDE) $(CCFLAGS)

clean:
	rm -rf $(BINDIR)/ $(BLDDIR)/ $(SYMDIR)/
