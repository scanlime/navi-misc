/*  
 * TUSB2136/3210 bootloader (for ROM-less operation)
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

#ifndef min
#define min(a,b) (((a)<(b))?(a):(b))
#endif

#define die(x) do { perror(x); return 1; } while(0)

int main(int argc, char *argv[])
{
  FILE *in;
  int i;
  struct stat s;
  int len;
  unsigned char firmware[8195];

  if(argc!=2) {
    fprintf(stderr,"Usage: %s filename\n",*argv);
    fprintf(stderr,"Downloads a file to the TUSB2316/3210 and executes it\n");
    return 1;
  }

  if((in=fopen(argv[1],"rb"))==NULL)
    die(argv[1]);

  if(fstat(fileno(in),&s)<0) 
    die(argv[1]);

  if(s.st_size>8192) {
    fprintf(stderr,"Error: input file is too large (max 8192 bytes)\n");
    fclose(in);
    return 1;
  }

  if((i=tusb_init())!=0) {
    fprintf(stderr,"Error initializing USB\n");
    fclose(in);
    return i;
  }

  /* When sending firmware directly to endpoint 1:
     - the first two bytes specify the length (LSB first); 
     - the third byte specifies the checksum
     - the firmware follows
     - each packet can be up to 255 bytes.
  */

  /* Read the data */
  len=fread(&firmware[3],1,8192,in);
  fclose(in);
  if(len==0) {
    fprintf(stderr,"Error: input file is empty!\n");
    return 1;
  } else if(len<0) {
    die("Error reading input file");
  }

  /* Store the length */
  firmware[0] = (unsigned char)(len & 0xff);
  firmware[1] = (unsigned char)((len & 0xff00) >> 8);

  /* Calculate the checksum */
  firmware[2]=0;
  for(i=0;i<len;i++)
    firmware[2] += firmware[3+i];

  /* Send the header and data to endpoint 1 in 255-byte chunks. */
  len+=3;
  for(i=0;i<len;i+=255)
    if(usb_bulk_write(tusb, 1, &firmware[i], min(255,len-i), 500) < 0)
      die("Error downloading firmware");

  printf("Wrote %d bytes.  Firmware should be running now.\n",len);

  return 0;
}
