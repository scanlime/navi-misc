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
#include <sys/ioctl.h>
#include <netinet/in.h>

#include <FieldSensor.h>


FieldSensor::FieldSensor(const char *serialPort, const char *netFile) : net(netFile) {
  struct termios options;

  fd = open(serialPort, O_NOCTTY | O_RDWR);

  tcgetattr(fd, &options);
  cfsetspeed(&options, B19200);                /* 19200 baud rate */
  options.c_cflag |= (CLOCAL | CREAD);         /* Enable receiver and set the local mode */
  options.c_cflag &= ~PARENB;                  /* None parity */
  options.c_cflag &= ~CSIZE;                   /* Mask the character bit size */
  options.c_cflag |= CS8;                      /* 8 data bits */
  options.c_cflag &= ~CSTOPB;                  /* 1 stop bits */
  options.c_cflag &= ~CRTSCTS;                 /* Disable hardware flow control */
  options.c_iflag &= ~(IXON | IXOFF | IXANY);  /* Disable software flow control */
  options.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG); /* Set raw input and output */
  options.c_oflag &= ~OPOST;
  tcsetattr(fd, TCSANOW, &options);

  reset();
  boot();
}

FieldSensor::~FieldSensor(void) {
  reset();
  close(fd);
}

void FieldSensor::reset(void) {
  /* Reset the microcontroller */
  int x;
  x = TIOCM_RTS;
  ioctl(fd, TIOCMBIC, &x);
  x = TIOCM_DTR;
  ioctl(fd, TIOCMBIS, &x);
  usleep(5000);
  x = TIOCM_DTR;
  ioctl(fd, TIOCMBIC, &x);
}

void FieldSensor::boot(void) {
  /* Instruct the bootloader to run our code */
  sendSlowly("0000hg");
}

void FieldSensor::waitForData(void) {
  fd_set rfds;
  FD_ZERO(&rfds);
  FD_SET(fd, &rfds);
  select(fd+1, &rfds, NULL, NULL, NULL);
}

VECTOR FieldSensor::readPacket(void) {
  unsigned char c, theirChecksum, ourChecksum;
  union {
    unsigned short shorts[8];
    unsigned char bytes[16];
  } rawPacket;
  int i;
  VECTOR results;

  while (1) {
    /* Synchronize to 0x80 synchronization byte */
    do {
      waitForData();
      read(fd, &c, 1);
    } while (c != 0x80);
    
    waitForData();
    read(fd, &rawPacket, 16);
    waitForData();
    read(fd, &theirChecksum, 1);

    /* Verify the checksum byte */
    ourChecksum = 0;
    for (i=0;i<16;i++)
      ourChecksum += rawPacket.bytes[i];
    if (ourChecksum != theirChecksum)
      continue;

    /* Convert the received big-endian integers to floating point */
    for (i=0;i<8;i++)
      results.push_back(ntohs(rawPacket.shorts[i]) / 65535.0);
    return results;
  }
}

void FieldSensor::sendSlowly(const char *str) {
  while (*str) {
    usleep(5000);
    write(fd, str, 1);
    str++;
  }
}

VECTOR FieldSensor::readPosition(void) {
  VECTOR packet = readPacket();
  return net.getOutput(packet);
}

/* The End */

  
