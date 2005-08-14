run: c64.ihx
	sudo fxload -t an21 -I $<

%.ihx: %.c
	sdcc $<

clean:
	rm -f *.asm *.ihx *.lnk *.lst *.map *.mem *.rel *.rst *.sym
