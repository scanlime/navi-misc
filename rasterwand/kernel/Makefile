ifneq ($(KERNELRELEASE),)

EXTRA_CFLAGS += -I$(RWAND_INCLUDEDIR) -DDEBUG -DVERTICAL_FLIP
obj-m	:= rwand.o

else
KDIR	:= /lib/modules/$(shell uname -r)/build
PWD	:= $(shell pwd)
RWAND_INCLUDEDIR := $(PWD)/../include

default:
	$(MAKE) -C $(KDIR) SUBDIRS=$(PWD) RWAND_INCLUDEDIR=$(RWAND_INCLUDEDIR) modules
endif