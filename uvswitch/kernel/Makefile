ifneq ($(KERNELRELEASE),)

EXTRA_CFLAGS += -I$(UVSWITCH_INCLUDEDIR) -DDEBUG
obj-m	:= uvswitch.o

else
KDIR	:= /lib/modules/$(shell uname -r)/build
PWD	:= $(shell pwd)
UVSWITCH_INCLUDEDIR := $(PWD)/../include

default:
	$(MAKE) -C $(KDIR) SUBDIRS=$(PWD) UVSWITCH_INCLUDEDIR=$(UVSWITCH_INCLUDEDIR) modules
endif