ORIGINAL = VCKE-original.sav
OUTPUT   = VCKE.sav
OUTPUT64 = VCKE-64k.sav
CODE     = exploit

PYTHON   = python

.PSEUDO: all clean disassemble hexdump

all: $(OUTPUT) $(OUTPUT64)

clean:
	rm -f $(CODE).o $(CODE).bin $(OUTPUT)

disassemble: $(CODE).o
	arm-eabi-objdump -d $<

hexdump: $(OUTPUT)
	hexdump -C $<

# The actual savegame is 8 kB, but NDS_Backup_Tool_Wifi thinks
# the EEPROM is 64 kB. Make a 64 kB version by concatenating
# the 8kB version 8 times.

$(OUTPUT64): $(OUTPUT)
	cat $(OUTPUT) $(OUTPUT) $(OUTPUT) $(OUTPUT) \
	    $(OUTPUT) $(OUTPUT) $(OUTPUT) $(OUTPUT) > $@

$(OUTPUT): cookinject.py $(ORIGINAL) $(CODE).bin
	$(PYTHON) cookinject.py $(ORIGINAL) $(OUTPUT)

%.o: %.s
	arm-eabi-as $< -o $@

%.bin: %.o
	arm-eabi-objcopy $< -O binary $@
