/* Test program for the serial bus on yoshi */

#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/termios.h>
#include <sys/select.h>
#include <string.h>
#include <math.h>

int main(int argc, char **argv) {
  int fd;
  char buffer[8];
  struct termios options;
  fd_set rfd,wfd;
  int y,r,g,grid[5][8];
  int i,j;
  float t;
  struct timeval tv;

  /********** Init the serial port */

  fd = open("/dev/ttyS0",O_WRONLY | O_NOCTTY | O_NDELAY);
  if (fd<0) {
    perror("open");
    return 1;
  }

  tcgetattr(fd, &options);
  cfsetispeed(&options, B57600);       /* 1200 baud rates */
  options.c_cflag |= (CLOCAL | CREAD);  /* Enable receiver and set the local mode */
  options.c_cflag &= ~PARENB;           /* None parity */
  options.c_cflag &= ~CSIZE;            /* Mask the character bit size */
  options.c_cflag |= CS8;               /* 8 data bits */
  options.c_cflag &= ~CSTOPB;           /* 1 stop bits */
  options.c_cflag &= ~CRTSCTS;          /* Disable hardware flow control */
  options.c_iflag &= ~(IXON | IXOFF | IXANY); /* Disable software flow control */
  /* Set raw input and output */
  options.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG);
  options.c_oflag &= ~OPOST;
  tcsetattr(fd, TCSANOW, &options);     /* set parameters */

  /* Some zeros to get them synchronized */
  memset(&buffer,0,sizeof(buffer));
  write(fd,&buffer,sizeof(buffer));

  /********** Send data from keyboard to device */

  while (1) {
    FD_ZERO(&rfd);
    FD_SET(1,&rfd);
    FD_ZERO(&wfd);
    FD_SET(fd,&wfd);
    tv.tv_sec = 0;
    tv.tv_usec = 1000;
    i = select(fd+1, &rfd, &wfd, NULL, &tv);

    if (i<=0) {
      y = sin(t)*127 + 127;
      r = sin(t-2)*127 + 127;
      g = sin(t-4)*127 + 127;
      t += 0.1;

      memset(&buffer,0,sizeof(buffer));
      buffer[0] = (3 << 5) |        (y >> 4);        /* Set clock LEDs */
      buffer[1] = (3 << 5) | 0x10 | (y & 0x0F);
      buffer[2] = (3 << 5) | 0x10 | (r >> 4);
      buffer[3] = (3 << 5) | 0x10 | (r & 0x0F);
      buffer[4] = (3 << 5) | 0x10 | (g >> 4);
      buffer[5] = (3 << 5) | 0x10 | (g & 0x0F);
      buffer[6] = (5 << 5)        | 0;               /* Set LED board address to beginning of red grid */
      buffer[7] = (5 << 5) | 0x10 | 4;
      write(fd,&buffer,sizeof(buffer));

      for (i=0;i<5;i++)
	for (j=0;j<8;j+=2) {
	  int value = 0xFF; //(grid[i][j] << 4) | (grid[i][j+1] & 0x0F);
	  buffer[0] = (5 << 5) | 0x10 | (value >> 4);
	  buffer[1] = (5 << 5) | 0x10 | (value & 0x0F);
	  write(fd,&buffer,2);
	}

    }
    else if (FD_ISSET(fd,&wfd)) {
      memset(&buffer,0,sizeof(buffer));
      write(fd,&buffer,sizeof(buffer));
    }
  }

  return 0;
}
