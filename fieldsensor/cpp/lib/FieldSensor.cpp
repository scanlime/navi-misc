/*
 * FieldSensor.cpp - Implementation of electric field sensor interface
 *
 * Copyright (C) 2003 Micah Dowty <micahjd@users.sourceforge.net>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 * 
 * Contributors:
 * 
 * 
 * 
 */

#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/termios.h>

#include <FieldSensor.h>


FieldSensor::FieldSensor(const char *serialPort) {
  struct termios options;

  fd = open(serialPort, O_RDWRITE | O_NCTTY);

  tcgetattr(mouse_fd, &options);
  cfsetispeed(&options, B1200);                /* 19200 baud rates */
  options.c_cflag |= (CLOCAL | CREAD);         /* Enable receiver and set the local mode */
  options.c_cflag &= ~PARENB;                  /* None parity */
  options.c_cflag &= ~CSIZE;                   /* Mask the character bit size */
  options.c_cflag |= CS8;                      /* 8 data bits */
  options.c_cflag &= ~CSTOPB;                  /* 1 stop bits */
  options.c_cflag &= ~CRTSCTS;                 /* Disable hardware flow control */
  options.c_iflag &= ~(IXON | IXOFF | IXANY);  /* Disable software flow control */
  options.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG); /* Set raw input and output */
  options.c_oflag &= ~OPOST;
  tcsetattr(mouse_fd, TCSANOW, &options);

  reset();
}

FieldSensor::~FieldSensor(void) {
  close(fd);
}

void FieldSensor::reset(void) {
  ioctl(fd, TIOCMBIC, TIOCM_RTS);
  ioctl(fd, TIOCMBIS, TIOCM_DTR);
  usleep(5);
  ioctl(fd, TIOCMBIC, TIOCM_DTR);
}

std::vector<float> FieldSensor::read(void) {
}

/* The End */

  
