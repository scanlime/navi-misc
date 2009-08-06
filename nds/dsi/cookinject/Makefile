#
# Makefile for cookinject
#

ORIGINAL = VCKE-original.sav
OUTPUT   = VCKE.sav
CODE     = exploit
STAGE2   = stage2

SPIME    = spime
PYTHON   = python

ifeq ($(strip $(DEVKITARM)),)
$(error "Please set DEVKITARM in your environment. export DEVKITARM=<path to>devkitARM")
endif
ifeq ($(strip $(DEVKITPRO)),)
$(error "Please set DEVKITPRO in your environment. export DEVKITPRO=<path to>devkitPro")
endif
include $(DEVKITARM)/base_rules

LIBNDS   = $(DEVKITPRO)/libnds

CFLAGS   = -Os -DARM9
LDFLAGS  = -ffreestanding -nostdlib -Wl,-T,inject.ld
INCLUDES = -I$(LIBNDS)/include
LIBS     = -L$(LIBNDS)/lib -lnds9 -lc

STAGE2_SRC = start.s logo.s inject.c spime.c


###########################################
# Top-level targets

.PSEUDO: all clean disassemble hexdump load

all: $(OUTPUT)

clean:
	rm -f *.bin *.o *.elf $(OUTPUT) logo.h logo.s

disassemble: $(CODE).o
	$(PREFIX)objdump -d $<

hexdump: $(OUTPUT)
	hexdump -C $<

# Load the savegame onto the spi-mem-emulator FPGA
load: $(OUTPUT)
	$(SPIME) --load $<


###########################################
# Build rules

# Inject the exploit code into a save file, then append the second-stage code
$(OUTPUT): cookinject.py $(ORIGINAL) $(CODE).bin $(STAGE2).bin
	$(PYTHON) cookinject.py $(ORIGINAL) $(OUTPUT)
	cat $(STAGE2).bin >> $(OUTPUT)

$(STAGE2).elf: inject.ld $(STAGE2_SRC)
	$(CC) $(CFLAGS) $(LDFLAGS) $(INCLUDES) -o $@ $(STAGE2_SRC) $(LIBS)

logo.s: logo.png logo.grit
	grit logo.png -fts -ff logo.grit

%.o: %.s
	$(AS) $< -o $@

%.bin: %.o
	$(OBJCOPY) $< -O binary $@

%.bin: %.elf
	arm-eabi-objcopy $< -O binary $@
