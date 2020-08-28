# Build a Cypress PSoC 5LP/Cortex-M3 targetted bare metal binary and program to target :)

BIN= bin
GCC_PREFIX= arm-none-eabi-
CFLAGS= -mcpu=cortex-m3 -I.
LDSCRIPT= cortex-m3.ld
LDFLAGS= -Wl,-T,$(LDSCRIPT) -nostartfiles
OBJS= \
	$(BIN)/startup.o \
	$(BIN)/main.o

RTOS_CFLAGS= -mcpu=cortex-m3 -DFREE_RTOS -I. -IFreeRTOS-Kernel/include -IFreeRTOS-Kernel/portable/GCC/ARM_CM3
RTOS_OBJS = \
	$(BIN)/startup.o \
	$(BIN)/tasks.o \
	$(BIN)/queue.o \
	$(BIN)/list.o \
	$(BIN)/port.o \
	$(BIN)/heap_2.o \
	$(BIN)/rtosapp.o

all: $(BIN) $(BIN)/psocplay.bin

# FreeRTOS target, include FreeRTOS objects, set cflag..
rtos: $(BIN) $(BIN)/rtos.bin
rtos: CFLAGS=$(RTOS_CFLAGS)

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
	$(GCC_PREFIX)objcopy -O binary -R .bss -R .heap $< $@
	openocd -f openocd.cfg -c "init; halt; psoc5lp mass_erase 0; program $@ 0x0 verify reset exit"

$(BIN)/rtos.bin: $(BIN)/rtos.elf
	$(GCC_PREFIX)objcopy -O binary -R .bss -R .heap $< $@
	openocd -f openocd.cfg -c "init; halt; psoc5lp mass_erase 0; program $@ 0x0 verify reset exit"

$(BIN)/psocplay.elf: $(OBJS) $(LDSCRIPT)
	$(GCC_PREFIX)gcc -g $(CFLAGS) $(LDFLAGS) -o $@ $(OBJS)

$(BIN)/rtos.elf: $(RTOS_OBJS) $(LDSCRIPT)
	$(GCC_PREFIX)gcc -g $(RTOS_CFLAGS) $(LDFLAGS) -o $@ $(RTOS_OBJS)

$(BIN)/%.o: %.c
	$(GCC_PREFIX)gcc -g -c -o $@ $(CFLAGS) $<

$(BIN)/%.o: FreeRTOS-Kernel/%.c
	$(GCC_PREFIX)gcc -g -c -o $@ $(CFLAGS) $<

$(BIN)/%.o: FreeRTOS-Kernel/portable/GCC/ARM_CM3/%.c
	$(GCC_PREFIX)gcc -g -c -o $@ $(CFLAGS) $<

$(BIN)/%.o: FreeRTOS-Kernel/portable/MemMang/%.c
	$(GCC_PREFIX)gcc -g -c -o $@ $(CFLAGS) $<
