OBJS = hello.rel hello_descript.rel uart_driver.rel usb_driver.rel util.rel
CC = sdcc
CFLAGS = -I ../include

hello: hello.ihx
	objcopy -I ihex -O binary hello.ihx hello

install: hello
	../bootload/bootload hello

hello.ihx: $(OBJS)
	sdcc -o hello.ihx $(OBJS)

%.rel: %.c *.h ../include/*.h
	sdcc -o $@ -c $< $(CFLAGS)

clean:
	rm -f *.ihx *.lnk *.lst *.map *.mem *.rel *.rst *.sym *.asm
	rm -f hello
