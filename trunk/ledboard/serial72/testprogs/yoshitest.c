/* Test program for the serial bus on yoshi */

#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/termios.h>
#include <sys/select.h>
#include <string.h>

int main(int argc, char **argv) {
  int fd;
  char buffer[16];
  struct termios options;
  fd_set rfd,wfd;
  int device= argv[1] ? atoi(argv[1]) : 1;
  
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
    select(fd+1, &rfd, &wfd, NULL, NULL);

    if (FD_ISSET(1,&rfd)) {
      memset(&buffer,0,sizeof(buffer));
      read(1,&buffer,1);
      buffer[1] = (device << 5) | 0x10 | (buffer[0] & 0x0F);
      buffer[0] = (device << 5) | ((buffer[0] & 0xF0)>>4);
      write(fd,&buffer,sizeof(buffer));
    }
    else if (FD_ISSET(fd,&wfd)) {
      memset(&buffer,0,sizeof(buffer));
      write(fd,&buffer,sizeof(buffer));
    }
  }

  return 0;
}
