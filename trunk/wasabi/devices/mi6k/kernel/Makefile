ifneq ($(KERNELRELEASE),)

EXTRA_CFLAGS += -I$(MI6K_INCLUDEDIR) -I$(LIRC_INCLUDEDIR) -DDEBUG
obj-m	:= mi6k.o

else
KDIR	:= /lib/modules/$(shell uname -r)/build
PWD	:= $(shell pwd)
MI6K_INCLUDEDIR := $(PWD)/../include
LIRC_INCLUDEDIR := /usr/src/linux/drivers/char/lirc

default:
	$(MAKE) -C $(KDIR) SUBDIRS=$(PWD) MI6K_INCLUDEDIR=$(MI6K_INCLUDEDIR) LIRC_INCLUDEDIR=$(LIRC_INCLUDEDIR) modules
endif