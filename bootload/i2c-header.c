/*  
 * TUSB2136/3210 i2c ROM header generator
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

/*
 * The header format on the i2c ROM begins with a signature, 
 * and contains any number of "descriptors" that include USB info,
 * firmware, or other stuff.  For the bootcode built into the
 * TUSB2316/3210, the format is further restricted to reduce code size:
 *
 * - i2c signature
 * - descriptor 1 is parsed only if it is a USB Basic Info block
 * - descriptor 2 is parsed only if it is a Firmware block
 * 
 * The useful combinations that this allows are
 * - just the USB Basic Info, which will give the TUSB your own
 *   VID/PID but still execute the builtin bootcode
 * - just the firmware, which can manually set up the USB
 * - both, in which case the USB info gets automatically filled
 *   (but the firmware still needs to manually connect to the bus)
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <string.h>
#include <errno.h>
#include <getopt.h>
#include "tusb.h"

#ifndef min
#define min(a,b) (((a)<(b))?(a):(b))
#endif

#define verb(format, args...) \
	do { if(verbose) printf(format, ## args); } while(0)

int main(int argc, char **argv)
{
  FILE *fh;
  struct stat s;
  int len, i;
  unsigned char firmware[65536];   // maximum size of firmware
  int firmware_len=0;
  int tmp;

  int c;
  int getopt_index;
  static struct option long_opts[] = {
    {     "info", 0, NULL, 'i' },
    {      "bit", 1, NULL, 'b' },
    {      "vid", 1, NULL, 'v' },
    {     "hpid", 1, NULL, 'h' },
    {      "pid", 1, NULL, 'p' },
    {     "potg", 1, NULL, 'g' },
    {  "current", 1, NULL, 'c' },
    { "firmware", 1, NULL, 'f' },
    {      "rev", 1, NULL, 'r' },
    {    "quiet", 0, NULL, 'q' },
    {     "help", 0, NULL, '?' },
  };
  int write_info=0;
  u8 bit = 0x81;
  u16 vid = 0xffff;
  u16 hpid = 0x0000;
  u16 pid = 0x1234;
  u8 potg = 0x32;    /* in units of 2ms */
  u8 current = 0x64; /* in units of 1ma */
  int write_firmware=0;
  char *firmware_file=NULL;
  u16 rev = 0x0100;
  int verbose=1;
  int help=0;

  while((c=getopt_long(argc,argv,"ib:v:h:p:g:c:f:r:q?",
		       long_opts,&getopt_index))!=-1) {
    switch(c) {
      case 'i':
	write_info=1;
	break;
      case 'b':
	tmp=strtol(optarg,(char **)NULL,0);
	if(tmp<0x00 || tmp>0xff) {
	  fprintf(stderr,"error: bit setting must be between 0x00 and 0xFF\n");
	  help=1;
	} else bit=(u8)tmp;
	break;
      case 'v':
	tmp=strtol(optarg,(char **)NULL,0);
	if(tmp<0x0000 || tmp>0xffff) {
	  fprintf(stderr,"error: VID must be between 0x0000 and 0xFFFF\n");
	  help=1;
	} else vid=(u16)tmp;
	break;
      case 'h':
	tmp=strtol(optarg,(char **)NULL,0);
	if(tmp<0x0000 || tmp>0xffff) {
	  fprintf(stderr,"error: hub PID must be between 0x0000 and 0xFFFF\n");
	  help=1;
	} else hpid=(u16)tmp;
	break;
      case 'p':
	tmp=strtol(optarg,(char **)NULL,0);
	if(tmp<0x0000 || tmp>0xffff) {
	  fprintf(stderr,"error: PID must be between 0x0000 and 0xFFFF\n");
	  help=1;
	} else pid=(u16)tmp;
	break;
      case 'g':
	/* power on to power good is in units of 2ms, so round up and divide */
	tmp=(strtol(optarg,(char **)NULL,0)+1)/2;
	if(tmp<0x00 || tmp>0xff) {
	  fprintf(stderr,"error: power on to power good must be "
		  "between 0 and 510 ms\n");
	  help=1;
	} else potg=(u8)tmp;
	break;
      case 'c':
	tmp=strtol(optarg,(char **)NULL,0);
	if(tmp<0x00 || tmp>0xff) {
	  fprintf(stderr,"error: current must be between 0 and 255 mA\n");
	  help=1;
	} else current=(u8)tmp;
	break;
      case 'f':
	write_firmware=1;
	firmware_file=optarg;
	break;
      case 'r':
	tmp=strtol(optarg,(char **)NULL,0);
	if(tmp<0x0000 || tmp>0xffff) {
	  fprintf(stderr,"error: revision must be between 0x0000 "
		  "and 0xFFFF\n");
	  help=1;
	} else rev=(u16)tmp;
	break;
      case 'q':
	verbose=0;
	break;
      case '?':
      default:
	help=1;
	break;
    }
  }
  if(!help) {
    if(!write_info && !write_firmware) {
      fprintf(stderr,"error: you must write a USB Basic Info and/or Firmware"
	      " block\n");
      help=1;
    }
    if(optind!=(argc-1)) {
      fprintf(stderr,"error: you must specify an output filename!\n");
      help=1;
    }
  }
  if(help) {
    fprintf(stderr,"TUSB2136/3210 i2c ROM header generator by jim@jtan.com\n");
    fprintf(stderr,"usage: %s [options] output-filename\n",*argv);
    fprintf(stderr," -i, --info           include USB Basic Info block:\n");
    fprintf(stderr,"     -b, --bit=n        bit settings (default 0x81):\n");
    fprintf(stderr,"                          bit 7: hub power switching\n");
    fprintf(stderr,"                          bit 6: group hub power\n");
    fprintf(stderr,"                          bit 0: self powered\n");
    fprintf(stderr,"     -v, --vid=n        vendor ID (default 0xffff)\n");
    fprintf(stderr,"     -h, --hpid=n       product ID for hub "
	    "(default 0x0000)\n");
    fprintf(stderr,"     -p, --pid=n        product ID (default 0x1234)\n");
    fprintf(stderr,"     -g, --potg=n       hub power on to power good time, "
	    "in ms (default 100)\n");
    fprintf(stderr,"     -c, --current=n    hub current, in mA "
	    "(default 100)\n");
    fprintf(stderr," -f, --firmware=file  include Firmware block, read "
	    "firmware from \"file\"\n");
    fprintf(stderr,"     -r, --rev=n        firmware revision "
	    "(default 0x0100)\n");
    return(1);
  } 

  if(write_firmware) {
    /* Read the firmware into memory, prepending the revision */
    firmware[0] = (u8)(rev & 0xFF);
    firmware[1] = (u8)((rev & 0xFF00) >> 8);

    verb("reading firmware from %s\n",firmware_file);
    if((fh=fopen(firmware_file,"r"))==NULL) {
      perror(argv[optind]);
      return 1;
    }
    if(fstat(fileno(fh),&s)<0) {
      perror(argv[optind]);
      fclose(fh);
      return 1;
    }
    if(s.st_size>65534) {
      fprintf(stderr, "error: firmware file is too large! (>65534 bytes)\n");
      fclose(fh);
      return 1;
    }
    /* Read the ROM image into memory */
    len=fread(&firmware[2],1,s.st_size,fh);
    fclose(fh);
    if(len==0) {
      fprintf(stderr,"error: firmware file is empty!\n");
      return 1;
    } else if(len<0) {
      perror("error reading firmware file");
      return 1;
    }
    firmware_len = len + 2;
  }

  /* Open the output file */
  verb("creating output file %s\n",argv[optind]);
  if((fh=fopen(argv[optind],"w"))==NULL) {
    perror(argv[optind]);
    return 1;
  }

  /* This code gets a bit ugly, but I want to take care that the 
     bytes are written one at a time to prevent endian issues */

  len=0;

#define put(x) do { fputc(x,fh); len++; } while(0)

  /* Write the i2c signature */
  put(0x36);
  put(0x21);
  
  if(write_info) {
    u8 data_type = 0x01;
    u8 data_size_low = 0x09;
    u8 data_size_high = 0x00;
    u8 vid_low = (u8)(vid & 0xFF);
    u8 vid_high = (u8)((vid & 0xFF00) >> 8);
    u8 hpid_low = (u8)(hpid & 0xFF);
    u8 hpid_high = (u8)((hpid & 0xFF00) >> 8);
    u8 pid_low = (u8)(pid & 0xFF);
    u8 pid_high = (u8)((pid & 0xFF00) >> 8);
    u8 checksum = (u8)(bit + vid_low + vid_high + hpid_low + hpid_high +
		       pid_low + pid_high + potg + current);

    verb("writing USB Basic Info block\n");

    put(data_type);
    put(data_size_low); put(data_size_high);
    put(checksum);
    put(bit);
    put(vid_low);       put(vid_high);
    put(hpid_low);      put(hpid_high);
    put(pid_low);       put(pid_high);
    put(potg);
    put(current);

  } else {
    /* Include a "dummy" block so that firmware can be in position 2 */
    u8 data_type = 0xff;
    u8 data_size_low = 0x00;
    u8 data_size_high = 0x00;
    u8 checksum = 0x00;

    verb("writing dummy block\n");

    put(data_type);
    put(data_size_low); put(data_size_high);
    put(checksum);
  }

  if(write_firmware) {
    u8 data_type = 0x02;
    u8 data_size_low = (u8)(firmware_len & 0xFF);
    u8 data_size_high = (u8)((firmware_len & 0xFF00) >> 8);
    u8 checksum = 0;

    verb("writing firmware block\n");

    for(i=0;i<firmware_len;i++)
      checksum += firmware[i];

    put(data_type);
    put(data_size_low); put(data_size_high);
    put(checksum);
    fwrite(firmware,1,firmware_len,fh);
    len+=firmware_len;    
  }

  /* End of descriptor marker */
  put(0x00);

  fclose(fh);

  verb("%d bytes written\n",len);

  return 0;
}
