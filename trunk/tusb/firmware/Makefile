hello: hello.ihx
	objcopy -I ihex -O binary hello.ihx hello

install: hello
	../bootload/bootload hello

hello.ihx: hello.c
	sdcc -o hello.ihx -I ../include hello.c

clean:
	rm -f *.ihx *.lnk *.lst *.map *.mem *.rel *.rst *.sym
	rm -f hello hello.asm
