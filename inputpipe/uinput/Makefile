ifneq ($(KERNELRELEASE),)

EXTRA_CFLAGS += -I$(LOCAL_INCLUDEDIR) -DDEBUG
obj-m	:= uinput.o

else
KDIR	:= /lib/modules/$(shell uname -r)/build
PWD	:= $(shell pwd)
LOCAL_INCLUDEDIR := $(PWD)

default:
	$(MAKE) -C $(KDIR) SUBDIRS=$(PWD) LOCAL_INCLUDEDIR=$(LOCAL_INCLUDEDIR) modules
endif