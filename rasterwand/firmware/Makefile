BIN = rasterwand.hex
LKR = 16c765.lkr
OBJ = commands.o descript.o display.o usb_ch9.o usb_main.o
ASMFLAGS = -p 16c745

$(BIN): $(OBJ)
	gplink -s $(LKR) -o $@ $^

install: $(BIN)
	pista < pista.conf

%.o: %.asm *.inc
	gpasm $(ASMFLAGS) -c $<

clean:
	rm -f *.o *.lst *.cod *.hex


