/*  
 * TUSB2136/3210 - support routines for bootcode
 * See http://www-s.ti.com/sc/psheets/sllu025a/sllu025a.pdf
 *
 * Copyright (C) 2001 Jim Paris <jim@jtan.com>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of version 2 of the GNU General Public
 * License, as published by the Free Software Foundation.  This
 * program is distributed WITHOUT ANY WARRANTY; without even the
 * implied warranty of merchantability or fitness for a particular
 * purpose.  You should have received a copy of version 2 of the GNU
 * General Public License along with this program; if not, write to
 * the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 * Boston, MA 02111-1307, USA.
 */

#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <usb.h>
#include "tusb.h"

#define die(x) do { perror(x); return 1; } while(0)

usb_dev_handle *tusb = NULL;

void close_tusb(void) {
  if(tusb!=NULL)
    usb_close(tusb);
}

int tusb_init(void) {
  struct usb_bus *bus;
  struct usb_device *dev;
  struct usb_device *tusb_dev = NULL;

  usb_init();
  if(usb_find_busses()<0)
    die("Bus scan");
  if(usb_find_devices()<0)
    die("Device scan");

  for (bus = usb_busses; bus; bus = bus->next) {
    for (dev = bus->devices; dev; dev = dev->next) {
      if((dev->descriptor.idVendor==0xE461 ||
          dev->descriptor.idVendor==0x0451) &&       /* TI */
	 (dev->descriptor.idProduct==0x0012 ||       /* TUSB2316/3210 */
	  dev->descriptor.idProduct==0x3410)) {      /* TUSB3410 */
	if(tusb_dev!=NULL) {
	  /* More than one, but there's no way of knowing which one to
	     program, so give up */
	  fprintf(stderr,"Multiple TUSB2316/3210s found, bailing out.\n");
	  return 1;
	}
	tusb_dev=dev;
      }
    }
  }

  if(tusb_dev==NULL) {
    fprintf(stderr,"TUSB2316/3210 not found.\n");
    return 1;
  }
  if((tusb=usb_open(tusb_dev))==NULL) {
    fprintf(stderr,"Error opening USB device.\n");
    return 1;
  }
  atexit(close_tusb);

  /* Setting the configuration and interface isn't really necessary 
     since there are only one of each in the boot ROM, but it's a good
     way to make sure we can send data to the device. */

  if(usb_set_configuration(tusb, 1)) {
    perror("Can't set configuration");
    fprintf(stderr,"(make sure you have write access to the device)\n");
    return 1;
  }
  if(usb_claim_interface(tusb, 0))
    die("Can't claim interface");

  return 0;
}

