BIN = rcpod.hex
LKR = 16c765.lkr
OBJ = usb_main.o usb_ch9.o descript.o commands.o io.o i2c.o
ASMFLAGS = -p p16c765

$(BIN): $(OBJ)
	gplink -s $(LKR) -o $@ $^

install-765: $(BIN)
	pista < pista-765.conf

install-745: $(BIN)
	pista < pista-745.conf

%.o: %.asm
	gpasm $(ASMFLAGS) -c $<

clean:
	rm -f *.o *.lst *.cod *.hex

