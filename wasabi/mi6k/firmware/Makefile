BIN = mi6k.hex
LKR = 16c745.lkr
OBJ = usb_main.o usb_ch9.o descript.o
ASMFLAGS = -p p16c745

$(BIN): $(OBJ)
	gplink -s $(LKR) -o $@ $^

%.o: %.asm
	gpasm $(ASMFLAGS) -c $<

clean:
	rm -f *.o *.lst *.cod *.hex

