/* 
 * ledserver.c - Simple server process for LED Board 1
 *     
 *   This program runs a background thread to refresh the LED array,
 *   and accepts socket connections on port 3827 for modifying the
 *   controller's registers with the following commands:
 *         
 *   set <led number> <brightness>     - Set the given LED to a brightness between 0 and 15
 *   blit                              - Copy the LED brightness backbuffer to the registers
 *   write <text>                      - Write text to the VFD
 *   cr                                - Send a \n to the VFD
 *   lf                                - Send a \r to the VFD
 *   (more to come..)
 *
 * PicoGUI small and efficient client/server GUI
 * Copyright (C) 2000-2003 Micah Dowty <micahjd@users.sourceforge.net>
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
 */

#include <fcntl.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <netinet/in.h>
#include <sys/stat.h>
#include <dirent.h>
#include <signal.h>
#include <unistd.h>
#include <errno.h>
#include <stdio.h>
#include <sys/termios.h>
#include <sys/select.h>
#include <string.h>
#include <pthread.h>
#include <netinet/tcp.h>


#define TCP_PORT 3827
#define NUM_LEDS 72
#define NUM_REGS 42

int fd, sock, vfd;
unsigned char ledregs[NUM_REGS];
int ledbrights[NUM_LEDS];


/********************************************* LED board communications */

void write_another_byte(int device, int byte) {
  char buffer[2];
  buffer[0] = (device << 5) | 0x10 | (byte>>4);
  buffer[1] = (device << 5) | 0x10 | (byte&0x0F);
  write(fd,buffer,2);
}

void write_nop(void) {
  char buffer[8];
  memset(buffer,0,sizeof(buffer));
  write(fd,buffer,sizeof(buffer));
}


void write_byte(int device, int byte) {
  char buffer[2];
  buffer[0] = (device << 5)        | (byte >> 4);               /* Set LED board address */
  buffer[1] = (device << 5) | 0x10 | (byte & 0x0F);
  write(fd,buffer,2);
}

void write_string(int device, char *str) {
  while (*str) {
    write_byte(device,*str);
    write_nop();
    str++;
  }
}

void *refresh_thread(void *foo) {
  int i;
  fd_set wfd;

  while (1) {
    /* Synchronize with the serial port speed.
     * We can't just use blocking writes, since then
     * our data would come out in big spurts.
     */
    FD_ZERO(&wfd);
    FD_SET(fd,&wfd);
    select(fd+1,NULL,&wfd,NULL,NULL);
    write_nop();
    write_nop();
    write_nop();
    write_nop();

    /* Start at address 0 and write all registers */
    write_byte(5,0);
    for (i=0;i<sizeof(ledregs);i++)
      write_another_byte(5,ledregs[i]);
  }
  return NULL;
}

void clear_ledregs(void) {
  /* Initialize LED registers */
  memset(ledregs,0,sizeof(ledregs));
  ledregs[0x24] = 15;            /* Maximum PWM value */
  ledregs[0x28] = 1;             /* PWM increment value */
  ledregs[0x29] = 0x25;          /* maximum LED refresh index */
}

void init_led(void) {
  int i;
  struct termios options;
  pthread_t my_thread;

  fd = open("/dev/ttyS0",O_WRONLY | O_NOCTTY | O_NDELAY);
  if (fd<0) {
    perror("open");
  }
  
  //  fcntl(fd, F_SETFL, fcntl(fd, F_GETFL,0) & ~O_NONBLOCK);

  tcgetattr(fd, &options);
  cfsetispeed(&options, B57600);        /* 57600bps */
  options.c_cflag |= (CLOCAL | CREAD);  /* Enable receiver and set the local mode */
  options.c_cflag &= ~PARENB;           /* None parity */
  options.c_cflag &= ~CSIZE;            /* Mask the character bit size */
  options.c_cflag |= CS8;               /* 8 data bits */
  options.c_cflag &= ~CSTOPB;           /* 1 stop bits */
  options.c_cflag &= ~CRTSCTS;          /* Disable hardware flow control */
  options.c_iflag &= ~(IXON | IXOFF | IXANY); /* Disable software flow control */
  options.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG);  /* Set raw input and output */
  options.c_oflag &= ~OPOST;
  tcsetattr(fd, TCSANOW, &options);     /* set parameters */

  clear_ledregs();

  /* Fill buffer up until thread starts */
  for (i=0;i<50;i++)
    write_nop();

  pthread_create(&my_thread,NULL,refresh_thread,NULL);
}


/********************************************* VFD */

void init_vfd(void) {
  int i;
  struct termios options;
  pthread_t my_thread;

  vfd = open("/dev/ttyS1",O_WRONLY | O_NOCTTY);
  if (vfd<0) {
    perror("open vfd");
  }
  
  tcgetattr(vfd, &options);
  cfsetispeed(&options, B19200);        /* 19200bps */
  options.c_cflag |= (CLOCAL | CREAD);  /* Enable receiver and set the local mode */
  options.c_cflag &= ~PARENB;           /* None parity */
  options.c_cflag &= ~CSIZE;            /* Mask the character bit size */
  options.c_cflag |= CS8;               /* 8 data bits */
  options.c_cflag &= ~CSTOPB;           /* 1 stop bits */
  options.c_cflag &= ~CRTSCTS;          /* Disable hardware flow control */
  options.c_iflag &= ~(IXON | IXOFF | IXANY); /* Disable software flow control */
  options.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG);  /* Set raw input and output */
  options.c_oflag &= ~OPOST;
  tcsetattr(vfd, TCSANOW, &options);     /* set parameters */
}

void reset_vfd(void) {
  char reset = 0x14, cursor_off = 0x0E;
  write(vfd, &reset, 1);
  write(vfd, &cursor_off, 1);
}


/********************************************* Socket communications */

void init_socket(void) {
  int tmp;
  struct sockaddr_in server_sockaddr;
    
  sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
  if (sock == -1)
    perror("socket");

  tmp = 1;
  setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, (void *)&tmp,sizeof(tmp));
  tmp = 1;
  setsockopt(sock,6 /*PROTO_TCP*/,TCP_NODELAY,(void *)&tmp,sizeof(tmp));
  
  server_sockaddr.sin_family = AF_INET;
  server_sockaddr.sin_port = htons(TCP_PORT);
  server_sockaddr.sin_addr.s_addr = htonl(INADDR_ANY);

  if (bind(sock, (struct sockaddr *)&server_sockaddr, sizeof(server_sockaddr)) == -1)
    perror("bind");
  if (listen(sock, 5) == -1)
    perror("listen");
}

void accept_client(void) {
  int client;
  struct sockaddr_in ec;
  FILE *f;
  char buf[4096];
  int a,b;
  socklen_t len = sizeof(ec);

  client = accept(sock, (void*) &ec, &len);
  if (client == -1)
    return;
  f = fdopen(client, "rw");

  /* Read lines... */
  while (fgets(buf, sizeof(buf)-1, f)) {
    buf[sizeof(buf)-1] = 0;
    
    if (sscanf(buf, "set %d %d\n", &a, &b) == 2) {
      if (a<NUM_LEDS)
	ledbrights[a] = b;
    }
    else if (!strcmp(buf, "blit\n")) {
      int i;
      for (i=0;i<NUM_LEDS/2;i++)
	ledregs[i] = ((ledbrights[i<<1]&0x0F) << 4) |
	  (ledbrights[(i<<1)+1]&0x0F);
    }
    else if (!strncmp(buf, "write ", 6)) {
      write(vfd,buf+6,strlen(buf)-7);
    }
    else if (!strcmp(buf, "cr\n")) {
      char nl = '\r';
      write(vfd, &nl, 1);
    }
    else if (!strcmp(buf, "lf\n")) {
      char nl = '\n';
      write(vfd, &nl, 1);
    }
  }

  fclose(f);
}


/********************************************* Main program */

int main(void) {
  init_led();
  init_vfd();
  init_socket();
  while (1) {
    clear_ledregs();
    reset_vfd();
    accept_client();
  }

  return 0;
}

/* The End */
