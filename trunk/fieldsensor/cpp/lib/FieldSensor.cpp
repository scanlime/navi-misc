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
#include <string.h>
#include <sys/termios.h>
#include <sys/ioctl.h>
#include <netinet/in.h>    /* For ntohs() */

#include <FieldSensor.h>


FieldSensor::FieldSensor(const char *serialPort, const char *netFile) 
  : net(netFile), rawData(8) {

  struct termios options;

  /* Initialize the serial port */
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

  /* Boot up our firmware on the board */
  reset();
  boot();

  /* Initial conditions for the Kalman filter */
  Q.set(0.001,0.001,0.001);        /* Process noise variance */
  R.set(0.7,0.7,0.7);              /* Measurement noise variance */
  x.set(0.5,0.5,0.5);              /* Initial estimate */
  P.set(1.0,1.0,1.0);              /* Initial variance estimate */

  pthread_create(&thread_id, NULL, &readerThread, this);
}

FieldSensor::~FieldSensor(void) {
  /* Use this to turn off the front panel lights, until 
   * we have full control of them from this software.
   */
  reset();

  pthread_kill(thread_id, 9);
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

void *FieldSensor::readerThread(void *pthis) {
  FieldSensor *sensor = (FieldSensor*) pthis;
  unsigned char c, theirChecksum, ourChecksum;
  union {
    unsigned short shorts[8];
    unsigned char bytes[16];
  } rawPacket;
  int i;
  VECTOR resultBuffer(8);
  unsigned char *packetPointer;
  int packetRemaining;

  while (1) {
    /* Synchronize to 0x80 synchronization byte */
    do {
      sensor->waitForData();
      read(sensor->fd, &c, 1);
    } while (c != 0x80);
    
    packetPointer = (unsigned char *) &rawPacket;
    packetRemaining = 16;
    while (packetRemaining) {
      sensor->waitForData();
      read(sensor->fd, packetPointer, 1);
      packetRemaining--;
      packetPointer++;
    }
    sensor->waitForData();
    read(sensor->fd, &theirChecksum, 1);

    /* Verify the checksum byte */
    ourChecksum = 0;
    for (i=0;i<16;i++)
      ourChecksum += rawPacket.bytes[i];
    if (ourChecksum != theirChecksum)
      continue;

    /* Convert the received big-endian integers to floating point */
    for (i=0;i<8;i++)
      resultBuffer[i] = ntohs(rawPacket.shorts[i]) / 65535.0;

    sensor->rawData = resultBuffer;
    VECTOR out = sensor->net.getOutput(resultBuffer);
    sensor->unfilteredPosition.set(out[0],out[1],out[2]);
    sensor->newData = true;
  }
}

void FieldSensor::sendSlowly(const char *str) {
  while (*str) {
    usleep(5000);
    write(fd, str, 1);
    str++;
  }
}

VECTOR FieldSensor::getRawData(void) {
  return rawData;
}

Vector3 FieldSensor::getPosition(void) {
  /* A simple 1D Kalman filter on each axis combined with a simple physics model */
  for (int i=0;i<3;i++) {
    /* Time update equations */
    x[i] += A[i];
    P[i] += Q[i];
    
    if (newData) {
      /* Measurement update equations */
      z = unfilteredPosition;
      K[i] = P[i]/(P[i]+R[i]);
      x[i] += K[i]*(z[i]-x[i]);
      P[i] *= 1-K[i];
    }
  }

  return x;
}

/* The End */

  
