main.hex: main.asm
	gpasm main.asm

install: main.hex
	ln -fs main.hex MAIN.HEX
	~/bin/xwisp write main.hex

run: install
	~/bin/xwisp run
	
