# Build a Cypress PSoC 5LP/Cortex-M3 targetted bare metal binary and program to target :)

BIN= bin
GCC-PREFIX= arm-none-eabi-
CFLAGS= -mcpu=cortex-m3
LDSCRIPT= cortex-m3.ld
LDFLAGS= -Wl,-T,$(LDSCRIPT)
OBJS= $(BIN)/psocplay.o

all: $(BIN) $(BIN)/psocplay.bin

prog: all

debug: all
	gdb-multiarch $(BIN)/psocplay.elf --command gdbinit

clean:
	rm -rf $(BIN)

$(BIN):
	mkdir -p $@

# Programming voodoo from: http://www.psoctools.org/psoc5lp-tools.html
# NB: we always program the device if rebuilding the .bin file..
$(BIN)/psocplay.bin: $(BIN)/psocplay.elf
	$(GCC-PREFIX)objcopy -O binary $< $@
	openocd -f openocd.cfg -c "init; halt; psoc5lp mass_erase 0; program bin/psocplay.bin 0x0 verify reset exit"

$(BIN)/psocplay.elf: $(OBJS) $(LDSCRIPT)
	$(GCC-PREFIX)gcc -g -nostartfiles $(CFLAGS) $(LDFLAGS) -o $@ $(OBJS)

$(BIN)/%.o: %.c
	$(GCC-PREFIX)gcc -g -c -o $@ $(CFLAGS) $<
