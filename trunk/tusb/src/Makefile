OBJS = uart_driver.rel usb_driver.rel util.rel
CC = sdcc
CFLAGS = -I ../include

all: $(OBJS)

%.rel: %.c ../include/*.h
	sdcc -o $@ -c $< $(CFLAGS)

clean:
	rm -f *.ihx *.lnk *.lst *.map *.mem *.rel *.rst *.sym *.asm
