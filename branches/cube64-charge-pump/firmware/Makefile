
all: cube64.hex serial_bridge.hex

%.hex: %.asm *.inc
	gpasm $<

install: cube64.hex
	pista < pista.conf

clean:
	rm -f *.hex *.lst *.cod