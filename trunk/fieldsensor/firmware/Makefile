main.hex: main.c
	rm -f main.hex
	sdcc -mpic14 main.c

install: main.hex
	ln -fs main.hex MAIN.HEX
	~/bin/xwisp write main.hex || true

run:
	../python/raw_test.py

clean:
	rm -f main.asm main.cod main.d main.lnk main.lst main.p main.hex main.map main.ihx main.mem main.rel main.rst 
