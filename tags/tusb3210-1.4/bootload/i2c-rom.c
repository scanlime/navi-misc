/*  
 * TUSB2136/3210 i2c ROM read/write (through bootloader code)
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
#include <string.h>
#include <errno.h>
#include <getopt.h>
#include <usb.h>
#include "tusb.h"

#ifndef min
#define min(a,b) (((a)<(b))?(a):(b))
#endif

#define verb(format, args...) \
	do { if(verbose) printf(format, ## args); } while(0)

int main(int argc, char **argv)
{
  FILE *fh;
  int i;
  struct stat s;
  int len;
  unsigned char rom[65536];   // maximum size of ROM

  int c;
  int getopt_index;
  static struct option long_opts[] = {
    { "read", 0, NULL, 'r' },
    { "write", 0, NULL, 'w' },
    { "addr", 1, NULL, 'a' },
    { "type", 1, NULL, 't' },
    { "speed", 1, NULL, 's' },
    { "size", 1, NULL, 'z' },
    { "delay", 1, NULL, 'd' },
    { "pad", 1, NULL, 'p' },
    { "quiet", 0, NULL, 'q' },
    { "help", 0, NULL, 'h' },
  };
  int help=0;
  int verbose=1;
  enum { rom_none, rom_read, rom_write } rw=rom_none;
  int addr = 0;
  int type = 3;
  int speed = 400;
  int size = 8192;
  int delay = 5;
  int pad = -1;
  u16 parms;
  u8 x;

  while((c=getopt_long(argc,argv,"rwa:t:s:z:d:p:qh",
		       long_opts,&getopt_index))!=-1) {
    switch(c) {
      case 'r': 
	if(rw==rom_write) {
	  fprintf(stderr,"error: can't specify both --read and --write\n");
	  help=1;
	} else rw=rom_read;
	break;
      case 'w': 
	if(rw==rom_read) {
	  fprintf(stderr,"error: can't specify both --read and --write\n");
	  help=1;
	} else rw=rom_write;
	break;
      case 'a':
	addr=strtol(optarg,(char **)NULL,0);
	if(addr<0 || addr>7) {
	  fprintf(stderr,"error: address must be between 0 and 7\n");
	  help=1;
	}
	break;
      case 't':
	type=strtol(optarg,(char **)NULL,0);
	if(type<1 || type>3) {
	  fprintf(stderr,"error: type must be between 1 and 3\n");
	  help=1;
	} 
	break;
      case 's':
	speed=strtol(optarg,(char **)NULL,0);
	if(speed!=100 && speed!=400) {
	  fprintf(stderr,"error: speed must be either 100 or 400\n");
	  help=1;
	} 
	break;
      case 'z': 
	size=strtol(optarg,(char **)NULL,0);
	if(size<16 || size>65536) {
	  fprintf(stderr,"error: size must be between 16 and 65536\n");
	  help=1;
	} else if(size & (size-1))
	  fprintf(stderr,"warning: size %d is not a power of two\n",size);
	break;
      case 'd':
	delay=strtol(optarg,(char **)NULL,0);
	if(delay<0) {
	  fprintf(stderr,"error: delay must be non-negative\n");
	  help=1;
	}
	break;
      case 'p':
	pad=strtol(optarg,(char **)NULL,0);
	if(pad<0x00 || pad>0xff) {
	  fprintf(stderr,"error: pad value must be between 0x00 and 0xFF\n");
	  help=1;
	}
	break;	
      case 'q':
	verbose=0;
	break;
      case 'h':
      default:
	help=1;
	break;
    }
  }
  if(optind!=(argc-1)) {
    fprintf(stderr,"error: must specify exactly one filename!\n");
    help=1;
  }
  if(help) {
    fprintf(stderr,"TUSB2136/3210 i2c ROM read/write by jim@jtan.com\n");
    fprintf(stderr,"usage: %s [options] filename\n",*argv);
    fprintf(stderr,"  -r, --read      read ROM, save to filename\n");
    fprintf(stderr,"  -w, --write     write filename to ROM (default)\n");
    fprintf(stderr,"  -a, --addr=n    i2c memory address (0-7, default 0)\n");
    fprintf(stderr,"  -t, --type=n    memory type (1-3, default 3)\n");
    fprintf(stderr,"  -s, --speed=n   speed in kHz (100/400, default 400)\n");
    fprintf(stderr,"  -z, --size=n    ROM size, in bytes "
	    "(16-65536, default 8192)\n");
    fprintf(stderr,"  -d, --delay=n   when writing, delay this many ms between"
            " bytes (default 5)\n");
    fprintf(stderr,"  -p, --pad=n     when writing, pad with this value "
	    "(default is no padding)\n");
    fprintf(stderr,"  -q, --quiet     be less verbose\n");
    return(1);
  } 
  if(rw==rom_none) rw=rom_write;

  verb("initializing TUSB\n");
  if((i=tusb_init())!=0) {
    fprintf(stderr,"Error initializing USB\n");
    return i;
  }

  /* From datasheet */
  parms = (u16) ((addr<<8) | type | ((speed==400)?0x80:0x00));

  switch(rw) {
    case rom_write:
      /* Open the file */
      verb("writing %s to EEPROM\n",argv[optind]);
      if((fh=fopen(argv[optind],"r"))==NULL) {
	perror(argv[optind]);
	return 1;
      }      
      if(fstat(fileno(fh),&s)<0) {
	perror(argv[optind]);
	fclose(fh);
	return 1;
      }
      if(s.st_size>size) {
	fprintf(stderr, "error: input file (%d bytes) "
		"is larger than ROM (%d bytes)\n",(int)s.st_size,size);
	fclose(fh);
	return 1;
      }

      /* Read the ROM image into memory */
      len=fread(rom,1,s.st_size,fh);
      fclose(fh);
      if(len==0) {
	fprintf(stderr,"error: input file is empty!\n");
	return 1;
      } else if(len<0) {
	perror("error reading input file");
	return 1;
      }
      
      /* Do a "dummy" read to set the device parameters */
      verb("setting i2c device parameters\n");
      if(usb_control_msg(tusb,
			 USB_RECIP_DEVICE | USB_TYPE_VENDOR | USB_ENDPOINT_IN,
			 BTC_I2C_MEMORY_READ,
			 parms,
			 0x0000,
			 &x, 1,
			 500)<0) {
	fprintf(stderr,"error setting i2c device parameters\n");
	return 1;
      }
      
      if(pad!=-1) {
	for(i=len;i<size;i++) 
	  rom[i]=(unsigned char)pad;
	len=size;
      }

      /* Now write the ROM */
      printf("writing to ROM...\n");
      for(i=0;i<len;i++) {
	if(verbose && (i & 0x0f)==0) {
	  printf("%d/%d\r",i,len);
	  fflush(stdout);
	}
	if(usb_control_msg(tusb,
			   USB_RECIP_DEVICE | USB_TYPE_VENDOR,
			   BTC_I2C_MEMORY_WRITE,
			   (u16)((addr<<8) | rom[i]),
			   i,
			   NULL, 0,
			   500)<0) {
	  fprintf(stderr,"i2c write failed at byte %d\n",i);
	  return 1;
	}
	if(delay>0) 
	  usleep(delay * 1000);
      }
      verb("%d/%d\ndone!\n",len,len);
      break;

    case rom_read:
      /* Create the output file */
      verb("reading from EEPROM, saving to %s\n",argv[optind]);
      if((fh=fopen(argv[optind],"w"))==NULL) {
	perror(argv[optind]);
	return 1;
      }

      /* Read from the ROM */
      printf("reading from ROM...\n");
      for(i=0;i<size;i++) {
	if(verbose && (i & 0x0f)==0) {
	  printf("%d/%d\r",i,size);
	  fflush(stdout);
	}
	if(usb_control_msg(tusb,
			   USB_RECIP_DEVICE | USB_TYPE_VENDOR |USB_ENDPOINT_IN,
			   BTC_I2C_MEMORY_READ,
			   parms,
			   i,
			   &rom[i], 1,
			   500)<0) {
	  fprintf(stderr,"i2c read failed at byte %d\n",i);
	  fclose(fh);
	  return 1;
	}
      }
      verb("%d/%d\ndone!\n",size,size);
      if(fwrite(rom,1,size,fh)!=size)
	fprintf(stderr,"error writing ROM image to disk\n");
      fclose(fh);
      break;

    default:
      fprintf(stderr,"Strange.\n");
      return 1;
  }
  return 0;
}
