/* Test program for the serial bus on yoshi */

#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/termios.h>
#include <sys/select.h>
#include <string.h>
#include <math.h>
#include <time.h>

int main(int argc, char **argv) {
  int fd;
  char buffer[8];
  struct termios options;
  fd_set rfd,wfd;
  int device=3;
  int y,r,g;
  int i;
  int randnum;
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

  srand(time(NULL));
  
  tv.tv_sec = 0;
  tv.tv_usec = 0;
  while (1) {
    FD_ZERO(&rfd);
    FD_SET(1,&rfd);
    FD_ZERO(&wfd);
    FD_SET(fd,&wfd);
    i = select(fd+1, &rfd, &wfd, NULL, &tv);

    if (i<=0) {
      randnum = rand();
      y = randnum & 4 ? 255 : 0;
      r = randnum & 2 ? 255 : 0;
      g = randnum & 1 ? 255 : 0;

      memset(&buffer,0,sizeof(buffer));
      buffer[0] = (device << 5) |        (y >> 4);
      buffer[1] = (device << 5) | 0x10 | (y & 0x0F);
      buffer[2] = (device << 5) | 0x10 | (r >> 4);
      buffer[3] = (device << 5) | 0x10 | (r & 0x0F);
      buffer[4] = (device << 5) | 0x10 | (g >> 4);
      buffer[5] = (device << 5) | 0x10 | (g & 0x0F);
      write(fd,&buffer,sizeof(buffer));
  
      tv.tv_sec = 0;
      tv.tv_usec = 200000;
    }
    else if (FD_ISSET(fd,&wfd)) {
      memset(&buffer,0,sizeof(buffer));
      write(fd,&buffer,sizeof(buffer));
    }
  }

  return 0;
}
