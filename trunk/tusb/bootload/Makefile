# TUSB2136/3210 bootloader
# See http://www-s.ti.com/sc/psheets/sllu025a/sllu025a.pdf
#
# Copyright (c) 2001 Jim Paris <jim@jtan.com>
#
# This program is free software; you can redistribute it and/or
# modify it under the terms of version 2 of the GNU General Public
# License, as published by the Free Software Foundation.  This
# program is distributed WITHOUT ANY WARRANTY; without even the
# implied warranty of merchantability or fitness for a particular
# purpose.  You should have received a copy of version 2 of the GNU
# General Public License along with this program; if not, write to
# the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
# Boston, MA 02111-1307, USA.

CFLAGS += -Wall $(shell libusb-config --cflags)
LDFLAGS += $(shell libusb-config --libs)

all: 	bootload i2c-rom i2c-header

tusb.o: tusb.c tusb.h

bootload.o: bootload.c tusb.h

bootload: bootload.o tusb.o

i2c-rom.o: i2c-rom.c tusb.h

i2c-rom: i2c-rom.o tusb.o

i2c-header.o: i2c-header.c tusb.h

%: %.o
	$(CC) -o $@ $^ $(LDFLAGS)

clean:
	rm -f bootload i2c-rom i2c-header *.o
