main.hex: main.c
	rm -f main.hex
	sdcc -mpic14 main.c

install: main.hex
	ln -fs main.hex MAIN.HEX
	~/bin/xwisp write main.hex || true

run:
	../lib/test.py

# Type "0000hg" to start the program from this console
term:
	~/bin/xwisp dtr 0 term 19200

clean:
	rm -f main.asm main.cod main.d main.lnk main.lst main.p main.hex
