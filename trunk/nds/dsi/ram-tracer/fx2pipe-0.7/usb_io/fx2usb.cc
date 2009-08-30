/*
 * usb_io/fx2usb.cc
 * 
 * Low-level routines for data IO with Cypress FX2 chip. 
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

#include "../usb_io/fx2usb.h"
#include "../usb_io/cycfx2dev.h"  /* Needed for programming. */

#include <string.h>
#include <errno.h>

#include <assert.h>


FX2USBDevice::ErrorCode FX2USBDevice::_DownloadFirmware(
	struct usb_device *usbdev,const char *path,const char **static_firmware,
	const char *cfg_buf,size_t cfg_len,size_t cfg_adr)
{
	CypressFX2Device fx2dev;
	
	ErrorCode ec=ECFailure;
	do {
		if(fx2dev.open(usbdev)) break;
		
		// [none] means: Just reset. 
		char what='n';                          // none
		if(!path && static_firmware) what='b';  // builtin static firmware
		if(path && *path)            what='f';  // file
		
		fprintf(stderr,"Downloading firmware %s%s%s...\n",
			what=='f' ? "\"" : "",
			what=='f' ? path : what=='b' ? "[builtin]" : "[none]",
			what=='f' ? "\"" : "");
		
		if(fx2dev.FX2Reset(/*running=*/0)) break;
		
		if(what=='f' && fx2dev.ProgramIHexFile(path)) break;
		else if(what=='b' && fx2dev.ProgramStaticIHex(static_firmware)) break;
		
		if(cfg_buf && 
			fx2dev.WriteRAM(cfg_adr,(const uchar*)cfg_buf,cfg_len)) break;
		
		if(fx2dev.FX2Reset(/*running=*/1)) break;
		
		ec=ECSuccess;
	} while(0);
	
	// Important: Close device again since we need to re-open it using 
	//            our own code. 
	if(fx2dev.close())
	{  ec=ECFailure;  }
	
	if(ec)
	{
		// FIXME: We should be a bit more precise here...
		fprintf(stderr,"Failed to connect FX2 and download firmware.\n");
	}
	
	return(ec);
}


FX2USBDevice::ErrorCode FX2USBDevice::connect(
	int initial_vendor,int initial_product,int n_th,
	const char *firmware_path,const char **static_firmware,
	const char *cfg_buf,size_t cfg_len,size_t cfg_adr)
{
	// Be sure not to be connected. 
	disconnect();
	
	// *** Connect for the first time. ***
	struct usb_device *fx2_dev=NULL;
	if(!fx2_dev && initial_vendor>=0 && initial_product>=0)
	{  fx2_dev=USBFindDevice(/*vendor=*/initial_vendor,
		/*product=*/initial_product,n_th);  }
	// Search for unconfigured FX2: 
	if(!fx2_dev)
	{  fx2_dev=USBFindDevice(/*vendor=*/0x04b4,/*product=*/0x8613,n_th);  }
	if(!fx2_dev)
	{
		fprintf(stderr,"Device not attached to USB.\n");
		return(ECNoSuchDevice);
	}
	
	char *bus = strdup(fx2_dev->bus->dirname);
	char *dev = strdup(fx2_dev->filename);
	if(!bus || !dev)
	{  return(ECFailure);  }
	
	ErrorCode ec=_DownloadFirmware(fx2_dev,firmware_path,static_firmware,
		cfg_buf,cfg_len, cfg_adr);
	if(ec)
	{
		free(bus);
		free(dev);
		return(ec);
	}
	
	// Wait some time for FX2 CPU initialisation and maybe re-numeration. 
	// FIXME: Maybe this is not enough. 
	usleep(10000);
	
	// Re-connect using bus and device paths since these should not change. 
	ec=WWUSBDevice::connect(bus,dev);
	free(bus);
	free(dev);
	if(ec)
	{
		fprintf(stderr,"Failed to re-connect to (configured) FX2 (ec=%d)\n",ec);
		return(ec);
	}
	
	return(ECSuccess);
}


FX2USBDevice::FX2USBDevice() : 
	WWUSBDevice()
{
}

FX2USBDevice::~FX2USBDevice()
{
}
