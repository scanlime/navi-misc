OBJS = hello.rel hello_descript.rel
CFLAGS = -I ../include
LDFLAGS = --xram-loc 0xF800 --xram-size 0x500 -L ../src -ltusb

all: hello hello_test

hello_test:
	gcc -o hello_test hello_test.c -lusb

hello: hello.ihx
	objcopy -I ihex -O binary hello.ihx hello

install: hello
	../bootload/bootload hello

hello.ihx: $(OBJS)
	sdcc -o hello.ihx $(OBJS) $(LDFLAGS)

%.rel: %.c ../include/*.h
	sdcc -o $@ -c $< $(CFLAGS)

clean:
	rm -f *.ihx *.lnk *.lst *.map *.mem *.rel *.rst *.sym *.asm
	rm -f hello hello_test
