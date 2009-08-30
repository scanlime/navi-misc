/*
 * cycfx2dev.cc - Cypress FX2 device class: low-level routines. 
 * 
 * Copyright (c) 2006 by Wolfgang Wieser ] wwieser (a) gmx <*> de [ 
 * 
 * This file may be distributed and/or modified under the terms of the 
 * GNU General Public License version 2 as published by the Free Software 
 * Foundation. (See COPYING.GPL for details.)
 * 
 * This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
 * WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
 * 
 */

#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <assert.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/time.h>

#include "cycfx2dev.h"


struct usb_device *USBFindDevice(const char *bus,const char *dev)
{
	for(usb_bus *b=usb_busses; b; b=b->next)
	{
		if(strcmp(b->dirname,bus))  continue;
		for(struct usb_device *d=b->devices; d; d=d->next)
		{
			if(strcmp(d->filename,dev))  continue;
			return(d);
		}
	}
	return(NULL);
}

struct usb_device *USBFindDevice(int vendor,int product,int nth)
{
	for(usb_bus *b=usb_busses; b; b=b->next)
	{
		for(struct usb_device *d=b->devices; d; d=d->next)
		{
			if(d->descriptor.idVendor==vendor && 
				d->descriptor.idProduct==product)
			{
				if(!nth--)
				{  return(d);  }
			}
		}
	}
	return(NULL);
}

//------------------------------------------------------------------------------

int CypressFX2Device::BlockRead(int endpoint,unsigned char *buf,size_t nbytes,
	char type)
{
	// FIXME: This function is somewhat bugged concerning reliable delivery 
	//        and correct handling of return values for short reads and . 
	//        timeout handling. Reason: Not sure what libusb is meant to 
	//        return and to do in non-standard cases. 
	
	if(!IsOpen())
	{  fprintf(stderr,"BlockRead: Not connected!\n");  return(-1);  }
	
	bool may_be_short=0;
	switch(type)
	{
		case 'b':  break;
		case 'i':  break;
		case 'B':  may_be_short=1;  type='b';  break;
		case 'I':  may_be_short=1;  type='i';  break;
		default:  assert(0);
	}
	
	int interface=0;
	int alt_interface=(type=='i' ? 2 : 1);
	if(usb_claim_interface(usbhdl,interface)<0)
	{  fprintf(stderr,"Failed to claim interface %d: %s\n",
		interface,usb_strerror());  return(-1);  }
	
	size_t chunk_size=nbytes;
	int error=0;
	size_t left=nbytes;
	do {
		if(usb_set_altinterface(usbhdl,alt_interface)<0)
		{
			fprintf(stderr,"Failed to set altinterface %d: %s\n",
				alt_interface,usb_strerror());
			++error;  break;
		}
		
		int ncalls=0;
		while(left)
		{
			size_t bs = left>chunk_size ? chunk_size : left;
			ssize_t rv;
			if(type=='i')
			{  rv=usb_interrupt_read(usbhdl,endpoint,(char*)buf,bs,
				/*timeout=*/1000/*msec*/);  }
			else
			{  rv=usb_bulk_read(usbhdl,endpoint,(char*)buf,bs,
				/*timeout=*/1000/*msec*/);  }
			++ncalls;
			if(rv<0)
			{
				fprintf(stderr,"Reading %u bytes from EP 0x%02x: "
					"USB: %s SYS: %s\n",
					bs,endpoint,usb_strerror(),strerror(-rv));
				++error;
				goto breakout;
			}
			assert(rv<=left);
			left-=rv;
			if(rv<bs)
			{
				if(may_be_short) goto breakout;
				
				// Not sure if rv=0 is a valid timeout indication...
				//if(!rv)
					fprintf(stderr,"Short read (%d/%d bytes)%s\n",rv,bs,
						rv==0 ? " (timeout?)" : "");
				if(rv==0)
				{  ++error;  goto breakout;  }
			}
		}
	} while(0); breakout:;
	
	usb_release_interface(usbhdl,interface);
	
	size_t read=nbytes-left;
	return(read ? read : (error ? -1 : 0));
}


int CypressFX2Device::BlockWrite(int endpoint,const unsigned char *buf,
	size_t nbytes,char type)
{
	// FIXME: This function is somewhat bugged concerning reliable delivery 
	//        and correct handling of return values for short writes and . 
	//        timeout handling. Reason: Not sure what libusb is meant to 
	//        return and to do in non-standard cases. 
	
	if(!IsOpen())
	{  fprintf(stderr,"BlockWrite: Not connected!\n");  return(-1);  }
	
	switch(type)
	{
		case 'b':  break;
		case 'i':  break;
		default:  assert(0);
	}
	
	int interface=0;
	int alt_interface=(type=='i' ? 2 : 1);
	if(usb_claim_interface(usbhdl,interface)<0)
	{  fprintf(stderr,"Failed to claim interface %d: %s\n",
		interface,usb_strerror());  return(-1);  }
	
	size_t chunk_size=nbytes;
	int error=0;
	size_t left=nbytes;
	do {
		if(usb_set_altinterface(usbhdl,alt_interface)<0)
		{
			fprintf(stderr,"Failed to set altinterface %d: %s\n",
				alt_interface,usb_strerror());
			++error;  break;
		}
		
		int ncalls=0;
		while(left)
		{
			size_t bs = left>chunk_size ? chunk_size : left;
			ssize_t rv;
			if(type=='i')
			{  rv=usb_interrupt_write(usbhdl,endpoint,(char*)buf,bs,
				/*timeout=*/1000/*msec*/);  }
			else
			{  rv=usb_bulk_write(usbhdl,endpoint,(char*)buf,bs,
				/*timeout=*/1000/*msec*/);  }
			++ncalls;
			if(rv<0)
			{
				fprintf(stderr,"Writing %u bytes to EP 0x%02x: "
					"USB: %s SYS: %s\n",
					bs,endpoint,usb_strerror(),strerror(-rv));
				++error;
				goto breakout;
			}
			assert(rv<=left);
			left-=rv;
			if(rv<bs)
			{
				// Not sure if rv=0 is a valid timeout indication...
				//if(!rv)
					fprintf(stderr,"Short write (%d/%d bytes)%s\n",rv,bs,
						rv==0 ? " (timeout?)" : "");
				if(rv==0)
				{  ++error;  goto breakout;  }
			}
		}
	} while(0); breakout:;
	
	usb_release_interface(usbhdl,interface);
	
	size_t read=nbytes-left;
	return(read ? read : (error ? -1 : 0));
}


int CypressFX2Device::BenchBlockRead(int endpoint,size_t nbytes,
	size_t chunk_size,char type)
{
	if(!IsOpen())
	{  fprintf(stderr,"BenchBlockRead: Not connected!\n");  return(1);  }
	
	assert(type=='b' || type=='i');
	
	int interface=0;
	int alt_interface=(type=='i' ? 2 : 1);
	if(usb_claim_interface(usbhdl,interface)<0)
	{  fprintf(stderr,"Failed to claim interface %d: %s\n",
		interface,usb_strerror());  return(1);  }
	
	int error=0;
	char *buf=(char*)malloc(chunk_size);
	assert(buf);
	do {
		if(usb_set_altinterface(usbhdl,alt_interface)<0)
		{
			fprintf(stderr,"Failed to set altinterface %d: %s\n",
				alt_interface,usb_strerror());
			++error;  break;
		}
		
		// Start benchmark: 
		timeval start_tv,end_tv;
		gettimeofday(&start_tv,NULL);
		
		size_t left=nbytes;
		int ncalls=0;
		while(left)
		{
			size_t bs = left>chunk_size ? chunk_size : left;
			ssize_t rv;
			if(type=='i')
			{  rv=usb_interrupt_read(usbhdl,endpoint,buf,bs,
				/*timeout=*/1000/*msec*/);  }
			else
			{  rv=usb_bulk_read(usbhdl,endpoint,buf,bs,
				/*timeout=*/1000/*msec*/);  }
			++ncalls;
			if(rv<0)
			{
				fprintf(stderr,"Reading %u bytes from EP 0x%02x: "
					"USB: %s SYS: %s\n",
					bs,endpoint,usb_strerror(),strerror(-rv));
				++error;
				goto breakout;
			}
			if(rv<bs)
			{
				// Not sure if rv=0 is a valid timeout indication...
				//if(!rv)
					fprintf(stderr,"Short read (%d/%d bytes)%s\n",rv,bs,
						rv==0 ? " (timeout?)" : "");
				if(rv==0)
				{  ++error;  goto breakout;  }
			}
			assert(rv<=left);
			left-=rv;
		}
		
		// End benchmark: 
		gettimeofday(&end_tv,NULL);
		
		double seconds = 
			double(end_tv.tv_sec-start_tv.tv_sec)+
			double(end_tv.tv_usec-start_tv.tv_usec)/1000000.0;
		printf("Read %u bytes in %5d msec (chunk size %6u): "
			"%6.3f Mb/sec (%5d calls, %6u bytes/call)\n",
			nbytes,(int)(seconds*1000+0.5),
			chunk_size,nbytes/seconds/1024/1024,
			ncalls,nbytes/ncalls);
		
	} while(0); breakout:;
	
	if(buf)
	{  free(buf);  buf=NULL;  }
	usb_release_interface(usbhdl,interface);
	
	return(error ? -1 : 0);
}


int CypressFX2Device::FX2Reset(bool running)
{
	// Reset is accomplished by writing a 1 to address 0xE600. 
	// Start running by writing a 0 to that address. 
	const size_t reset_addr=0xe600;
	unsigned char val = running ? 0 : 1;
	
	return(WriteRAM(reset_addr,&val,1));
}


int CypressFX2Device::WriteRAM(size_t addr,const unsigned char *data,
	size_t nbytes)
{
	if(!IsOpen())
	{  fprintf(stderr,"WriteRAM: Not connected!\n");  return(1);  }
	
	int n_errors=0;
	
	const size_t chunk_size=16;
	const unsigned char *d=data;
	const unsigned char *dend=data+nbytes;
	while(d<dend)
	{
		size_t bs=dend-d;
		if(bs>chunk_size)  bs=chunk_size;
		size_t dl_addr=addr+(d-data);
		int rv=usb_control_msg(usbhdl,0x40,0xa0,
			/*addr=*/dl_addr,0,
			/*buf=*/(char*)d,/*size=*/bs,
			/*timeout=*/1000/*msec*/);
		if(rv<0)
		{  fprintf(stderr,"Writing %u bytes at 0x%x: %s\n",
			bs,dl_addr,usb_strerror());  ++n_errors;  }
		d+=bs;
	}
	
	return(n_errors);
}


int CypressFX2Device::ReadRAM(size_t addr,unsigned char *data,size_t nbytes)
{
	if(!IsOpen())
	{  fprintf(stderr,"ReadRAM: Not connected!\n");  return(1);  }
	
	int n_errors=0;
	
	const size_t chunk_size=16;
	
	unsigned char *d=data;
	unsigned char *dend=data+nbytes;
	while(d<dend)
	{
		size_t bs=dend-d;
		if(bs>chunk_size)  bs=chunk_size;
		size_t rd_addr=addr+(d-data);
		int rv=usb_control_msg(usbhdl,0xc0,0xa0,
			/*addr=*/rd_addr,0,
			/*buf=*/(char*)d,/*size=*/bs,
			/*timeout=*/1000/*msec*/);
		if(rv<0)
		{  fprintf(stderr,"Reading %u bytes at 0x%x: %s\n",
			bs,rd_addr,usb_strerror());  ++n_errors;  }
		d+=bs;
	}
	
	return(n_errors);
}


int CypressFX2Device::_ProgramIHexLine(const char *buf,
	const char *path,int line)
{
	const char *s=buf;
	if(*s!=':')
	{  fprintf(stderr,"%s:%s: format violation (1)\n",path,line);
		return(1);  }
	++s;
	
	unsigned int nbytes=0,addr=0,type=0;
	if(sscanf(s,"%02x%04x%02x",&nbytes,&addr,&type)!=3)
	{  fprintf(stderr,"%s:%s: format violation (2)\n",path,line);
		return(1);  }
	s+=8;
	
	if(type==0)
	{
		//printf("  Writing nbytes=%d at addr=0x%04x\n",nbytes,addr);
		assert(nbytes>=0 && nbytes<256);
		unsigned char data[nbytes];
		unsigned char cksum=nbytes+addr+(addr>>8)+type;
		for(unsigned int i=0; i<nbytes; i++)
		{
			unsigned int d=0;
			if(sscanf(s,"%02x",&d)!=1)
			{  fprintf(stderr,"%s:%s: format violation (3)\n",path,line);
				return(1);  }
			s+=2;
			data[i]=d;
			cksum+=d;
		}
		unsigned int file_cksum=0;
		if(sscanf(s,"%02x",&file_cksum)!=1)
		{  fprintf(stderr,"%s:%s: format violation (4)\n",path,line);
			return(1);  }
		if((cksum+file_cksum)&0xff)
		{  fprintf(stderr,"%s:%s: checksum mismatch (%u/%u)\n",
			cksum,file_cksum);  return(1);  }
		if(WriteRAM(addr,data,nbytes))
		{  return(1);  }
	}
	else if(type==1)
	{
		// EOF marker. Oh well, trust it. 
		return(-1);
	}
	else
	{
		fprintf(stderr,"%s:%s: Unknown entry type %d\n",type);
		return(1);
	}

	return(0);
}


int CypressFX2Device::ProgramIHexFile(const char *path)
{
	if(!IsOpen())
	{  fprintf(stderr,"ProgramIHexFile: Not connected!\n");  return(1);  }
	
	FILE *fp=fopen(path,"r");
	if(!fp)
	{  fprintf(stderr,"Failed to open %s: %s\n",path,strerror(errno));
		return(2);  }
	
	int n_errors=0;
	
	const size_t buflen=1024;  // Hopefully much too long for real life...
	char buf[buflen];
	int line=1;
	for(;;++line)
	{
		*buf='\0';
		if(!fgets(buf,buflen,fp))
		{
			if(feof(fp))
			{  break;  }
			fprintf(stderr,"Reading %s (line %d): %s\n",path,line,
				strerror(ferror(fp)));
			fclose(fp);  fp=NULL;
			return(3);
		}
		
		int rv=_ProgramIHexLine(buf,path,line);
		if(rv<0)  break;
		if(rv)
		{  ++n_errors;  }
	}
	
	if(fp)
	{  fclose(fp);  }
	
	return(n_errors ? -1 : 0);
}


int CypressFX2Device::ProgramStaticIHex(const char **ihex)
{
	int n_errors=0;
	
	for(int line=0; ihex[line]; line++)
	{
		int rv=_ProgramIHexLine(ihex[line],"[builtin]",line+1);
		if(rv<0)  break;
		if(rv)
		{  ++n_errors;  }
	}
	
	return(n_errors ? -1 : 0);
}


int CypressFX2Device::ProgramBinFile(const char *path,size_t start_addr)
{
	if(!IsOpen())
	{  fprintf(stderr,"ProgramIHexFile: Not connected!\n");  return(1);  }
	
	int fd=::open(path,O_RDONLY);
	if(fd<0)
	{  fprintf(stderr,"Failed to open %s: %s\n",path,strerror(errno));
		return(2);  }
	
	int n_errors=0;
	const size_t buflen=1024;
	char buf[buflen];
	size_t addr=start_addr;
	for(;;)
	{
		ssize_t rd=read(fd,buf,buflen);
		if(!rd)  break;
		if(rd<0)
		{
			fprintf(stderr,"Reading %s: %s\n",path,strerror(errno));
			::close(fd);  fd=-1;
			return(3);
		}
		if(WriteRAM(addr,(const unsigned char*)buf,rd))
		{  ++n_errors;  }
		addr+=rd;
	}
	
	if(fd>=0)
	{  ::close(fd);  fd=-1;  }
	
	return(n_errors ? -1 : 0);
}


int CypressFX2Device::open(struct usb_device *_usbdev)
{
	close();
	usbdev=_usbdev;
	usbhdl=usb_open(usbdev);
	if(!usbhdl)
	{  fprintf(stderr,"Failed to open device: %s\n",usb_strerror());
		return(1);  }
	return(0);
}


int CypressFX2Device::close()
{
	int rv=0;
	if(usbhdl)
	{
		rv=usb_close(usbhdl);  usbhdl=NULL;
		if(rv)
		{  fprintf(stderr,"closing USB device: %s\n",usb_strerror());  }
	}
	usbdev=NULL;
	return(rv);
}

