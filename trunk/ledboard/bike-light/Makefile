
all: bike-light.hex

install: bike-light.hex
	(echo device 12f675; echo erase pic:; echo copy $< pic:) | pista

%.hex: %.asm *.inc
	gpasm $<

clean:
	rm -f *.lst *.cod *.hex