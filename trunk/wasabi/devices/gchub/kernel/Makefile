ifneq ($(KERNELRELEASE),)

EXTRA_CFLAGS += -I$(GCHUB_INCLUDEDIR) -DDEBUG
obj-m	:= gchub.o

else
KDIR	:= /lib/modules/$(shell uname -r)/build
PWD	:= $(shell pwd)
GCHUB_INCLUDEDIR := $(PWD)/../include

default:
	$(MAKE) -C $(KDIR) SUBDIRS=$(PWD) GCHUB_INCLUDEDIR=$(GCHUB_INCLUDEDIR) modules
endif