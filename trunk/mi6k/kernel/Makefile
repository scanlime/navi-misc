WARN	:=
INCLUDE := -isystem /lib/modules/`uname -r`/build/include -I../include
CFLAGS  := -O2 -DMODULE -D__KERNEL__ ${WARN} ${INCLUDE} -DDEBUG
CC      := gcc
OBJS    := ${patsubst %.c, %.o, ${wildcard *.c}}

all: ${OBJS}

clean:
	rm -rf *.o

test: mi6k.o
	rmmod mi6k || true
	insmod mi6k.o
