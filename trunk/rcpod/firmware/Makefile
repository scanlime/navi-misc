BIN = rcpod.hex
LKR = 16c765.lkr
OBJ = usb_main.o usb_ch9.o descript.o commands.o io.o i2c.o
ASMFLAGS = -p p16c765

$(BIN): $(OBJ)
	gplink -s $(LKR) -o $@ $^

# Installing 'uses up' the serial number, since each chip
# should have a unique one.
install-765: $(BIN)
	rm -f serial.number
	pista < pista-765.conf

install-745: $(BIN)
	rm -f serial.number
	pista < pista-745.conf

%.o: %.asm *.inc
	gpasm $(ASMFLAGS) -c $<

descript.o: descript.asm *.inc serial.number
	gpasm $(ASMFLAGS) -c $<

serial.number:
	./serial-gen.py > serial.number

clean:
	rm -f *.o *.lst *.cod *.hex serial.number

