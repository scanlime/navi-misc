
all: cube64.hex

cube64.hex: cube64.asm
	gpasm cube64.asm

install: cube64.hex
	pista < pista.conf

clean:
	rm -f *.hex *.lst *.cod