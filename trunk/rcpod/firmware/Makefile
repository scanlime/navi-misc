BIN = rcpod.hex
LKR = 16c745.lkr
OBJ = usb_main.o usb_ch9.o descript.o commands.o
ASMFLAGS = -p p16c745

$(BIN): $(OBJ)
	gplink -s $(LKR) -o $@ $^

install: $(BIN)
	picp /dev/ttyS0 16C745 -s 80 -wp $(BIN)

%.o: %.asm
	gpasm $(ASMFLAGS) -c $<

clean:
	rm -f *.o *.lst *.cod *.hex

