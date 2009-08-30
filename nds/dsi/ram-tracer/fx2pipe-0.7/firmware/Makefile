INCLUDES=-Iinclude

CC=sdcc -mmcs51

fx2pipe_static.cc: fx2pipe.ihx
	@echo "#include <stdio.h>  /* For NULL... */" > fx2pipe_static.cc
	@echo >> fx2pipe_static.cc
	@echo "const char *fx2pipe_static_firmware[]={" >> fx2pipe_static.cc
	@bash -c 'while read line ; do echo -e "\t\"$$line\"," >> fx2pipe_static.cc ; done < fx2pipe.ihx'
	@echo -e "\tNULL" >> fx2pipe_static.cc
	@echo "};" >> fx2pipe_static.cc
	@echo "Created fx2pipe_static.cc"

fx2pipe.ihx: fx2pipe.c
	$(CC) $(INCLUDES) fx2pipe.c
	ls -l fx2pipe.ihx

clean:
	rm -f fx2pipe.{asm,ihx,lnk,lst,map,mem,rel,rst,sym}
