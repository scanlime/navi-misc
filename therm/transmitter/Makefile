BIN = therm-tx.hex

all: $(BIN)

install: $(BIN)
	pista < pista.conf

%.hex: %.asm *.inc
	gpasm $<

clean:
	rm -f *.lst *.cod *.hex