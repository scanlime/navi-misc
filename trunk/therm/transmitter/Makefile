
all: therm-tc74.hex therm-ds18b20.hex

install-tc74: therm-tc74.hex
	(echo device 12f675; echo erase pic:; echo copy $< pic:) | pista

install-ds18b20: therm-ds18b20.hex
	(echo device 12f675; echo erase pic:; echo copy $< pic:) | pista

%.hex: %.asm *.inc
	gpasm $<

clean:
	rm -f *.lst *.cod *.hex