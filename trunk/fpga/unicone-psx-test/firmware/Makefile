BIN = psxextender.hex

all: $(BIN)

%.hex: %.asm
	gpasm $<

#
# This assumes you're using the PIC16F877A bootloader here:
# http://www.microchipc.com/PIC16bootload/
#
install: $(BIN)
	grep -v :02000004 $(BIN) > $(BIN).2
	picdl -P/dev/ttyUSB1 $(BIN).2
	rm -f $(BIN).2

clean:
	rm -f *.hex *.lst *.cod *.hex.2