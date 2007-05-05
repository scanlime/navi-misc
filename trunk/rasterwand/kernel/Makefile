ifneq ($(KERNELRELEASE),)

obj-m	:= rwand.o
CFLAGS += $(MODCFLAGS) -DKERNEL26

else

ifeq ($(PREFIX),)
PREFIX	:= /usr/local
endif

MODULE    := rwand
KVERS     := $(shell uname -r)
KDIR      := /lib/modules/$(KVERS)/build
INCLUDE   := -isystem $(KDIR)/include
MODCFLAGS := -DMODULE -D__KERNEL__ -Wall ${INCLUDE}

KMAKE	  := $(MAKE) -C $(KDIR) SUBDIRS=$(PWD)

modules: $(MODULE).o

insert_module: install
	modprobe -r rwand
	modprobe rwand

# build module

$(MODULE).o: $(MODULE).c
	$(KMAKE) modules


# Automake rules, as per "Third-Party Makefiles" in the automake manual

all: $(MODULE).o

install:
	$(KMAKE) modules_install
	[ -e /sbin/depmod ] && /sbin/depmod

install-data:
install-exec:
uninstall:

install-info:
installdirs:
check:
installcheck:
mostlyclean:
clean:
	rm -f rwand.ko
	rm -f rwand.o
	rm -f rwand.mod.o
	rm -f rwand.mod.c
distclean: clean
maintainer-clean:
dvi:
pdf:
info:
html:
tags:
ctags:

endif

