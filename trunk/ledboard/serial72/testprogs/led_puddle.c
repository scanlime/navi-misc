/* Test program for the serial bus on yoshi */

#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/termios.h>
#include <sys/select.h>
#include <string.h>
#include <math.h>

/* Tables used by the effect.. */

int dist_tab[8][5] = {
  { 4, 3, 3, 3, 4 },
  { 3, 2, 2, 2, 3 },
  { 2, 1, 1, 1, 2 },
  { 2, 1, 0, 1, 2 },
  { 2, 1, 0, 1, 2 },
  { 2, 1, 1, 1, 2 },
  { 3, 2, 2, 2, 3 },
  { 4, 3, 3, 3, 4 },
};

int brightness_function(int x, int y, int frame) {
  if (dist_tab[y][x]==4)
    return 0;

  return sin(dist_tab[y][x]*1.2 - frame*0.1)*2 + 4;
}

int main(int argc, char **argv) {
  int fd;
  char buffer[8];
  struct termios options;
  int i,j,frame;
  struct timeval tv;
  fd_set rfd,wfd;

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
  memset(buffer,0,sizeof(buffer));
  write(fd,buffer,sizeof(buffer));

  frame = 0;
  while (1) {
    FD_ZERO(&rfd);
    FD_SET(1,&rfd);
    FD_ZERO(&wfd);
    FD_SET(fd,&wfd);
    tv.tv_sec = 0;
    tv.tv_usec = 100;
    i = select(fd+1, &rfd, &wfd, NULL, &tv);

    if (i<=0) {
      frame++;
    
      buffer[0] = (5 << 5)        | 0;               /* Set LED board address */
      buffer[1] = (5 << 5) | 0x10 | 4;
      write(fd,buffer,2);
      
      for (j=0;j<20;j++) {
	buffer[0] = (5 << 5) | 0x10 | brightness_function(j>>2,(j&3)*2,frame);
	buffer[1] = (5 << 5) | 0x10 | brightness_function(j>>2,(j&3)*2+1,frame);
	buffer[2] = 0;
	write(fd,buffer,2);
      }
      tv.tv_sec = 0;
      tv.tv_usec = 100;
    }
    else if (FD_ISSET(fd,&wfd)) {
      memset(&buffer,0,sizeof(buffer));
      write(fd,&buffer,sizeof(buffer));
    }
  }

  return 0;
}
