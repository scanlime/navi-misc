/*
 * uinput_mouse.c - A simple interface for emulating a mouse under linux using /dev/uinput
 *
 * Copyright (C) 2004 Micah Dowty <micah@navi.cx>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 */

#include "uinput_mouse.h"
#include <linux/input.h>
#include <linux/uinput.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

static int uinput_fd = 0;


void uinput_mouse_init(const char *dev_name) {
  struct uinput_user_dev dev;

  uinput_fd = open("/dev/uinput", O_WRONLY);
  if (uinput_fd <= 0) {
    perror("Error opening the uinput device\n");
    return;
  }

  memset(&dev, 0, sizeof(dev));
  strcpy(dev.name, dev_name);
  write(uinput_fd, &dev, sizeof(dev));

  /* Set the event bits to give us the bare minimum to count as a mouse-
   * two relative motion axes and one button.
   */
  ioctl(uinput_fd, UI_SET_EVBIT, EV_REL);
  ioctl(uinput_fd, UI_SET_RELBIT, REL_X);
  ioctl(uinput_fd, UI_SET_RELBIT, REL_Y);
  ioctl(uinput_fd, UI_SET_EVBIT, EV_KEY);
  ioctl(uinput_fd, UI_SET_KEYBIT, BTN_MOUSE);

  /* Register the device with linux's input subsystem */
  ioctl(uinput_fd, UI_DEV_CREATE, 0);
}

void uinput_mouse_move(int rel_x, int rel_y) {
  struct input_event event;

  if (uinput_fd <= 0)
    return;

  gettimeofday(&event.time, NULL);
  event.type = EV_REL;
  event.code = REL_X;
  event.value = rel_x;
  write(uinput_fd, &event, sizeof(event));

  event.type = EV_REL;
  event.code = REL_Y;
  event.value = rel_y;
  write(uinput_fd, &event, sizeof(event));

  event.type = EV_SYN;
  event.code = SYN_REPORT;
  event.value = 0;
  write(uinput_fd, &event, sizeof(event));
}

/* The End */
