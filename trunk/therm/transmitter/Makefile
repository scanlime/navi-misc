
all: therm-tc74.hex

install-tc74: therm-tc74.hex
	(echo "device 12f675"; echo "erase pic:"; echo "copy therm-tx.hex pic:") | pista

%.hex: %.asm *.inc
	gpasm $<

clean:
	rm -f *.lst *.cod *.hex