ifneq ($(KERNELRELEASE),)

EXTRA_CFLAGS += -I$(EFS_INCLUDEDIR)
obj-m	:= efs.o

else
KDIR	:= /lib/modules/$(shell uname -r)/build
PWD	:= $(shell pwd)
EFS_INCLUDEDIR := $(PWD)/../include

default:
	$(MAKE) -C $(KDIR) SUBDIRS=$(PWD) EFS_INCLUDEDIR=$(EFS_INCLUDEDIR) modules
endif