
ifneq ($(KERNELRELEASE),)
# We're in the kernel build system

EXTRA_CFLAGS += -I$(GCHUB_INCLUDEDIR) -DDEBUG
obj-m	:= gchub.o

# Compatibility with pre-2.5 build systems, graciously pilfered from hostap
ifeq ($(TOPDIR)/Rules.make,$(wildcard $(TOPDIR)/Rules.make))
include $(TOPDIR)/Rules.make

ifdef CONFIG_MODVERSIONS

VERFILES=gchub.ver
gchub.o: $(VERFILES)
INCLUDES_MODVER=-include gchub.ver

GENKSYMS=/sbin/genksyms
ifdef CONFIG_SMP
        GENKSYMS += -p smp_
endif

gchub.ver: gchub.c
	$(CC) $(INCLUDES) $(CFLAGS) -E -D__GENKSYMS__ $^ | \
		$(GENKSYMS) -k $(VERSION).$(PATCHLEVEL).$(SUBLEVEL) > $@

endif # CONFIG_MODVERSIONS
endif # pre-2.5

else
# Outside the kernel build system, enter it

KDIR	:= /lib/modules/$(shell uname -r)/build
PWD	:= $(shell pwd)
GCHUB_INCLUDEDIR := $(PWD)/../include

default:
	$(MAKE) -C $(KDIR) SUBDIRS=$(PWD) GCHUB_INCLUDEDIR=$(GCHUB_INCLUDEDIR) modules
endif