/*
 * usb_io/wwusb.cc
 * 
 * USB async low-level routines. 
 * 
 * Copyright (c) 2006 by Wolfgang Wieser ] wwieser (a) gmx <*> de [ 
 * 
 * The idea behind this code is inspired from fusb_linux.cc from the GNU 
 * radio project (Copyright 2003 Free Software Foundation, Inc.) and the 
 * first version of the code was based on it. 
 * However, this is a major re-write for the second version. 
 * 
 * This file may be distributed and/or modified under the terms of the 
 * GNU General Public License version 2 as published by the Free Software 
 * Foundation. (See COPYING.GPL for details.)
 * 
 * This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
 * WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
 * 
 */

#include "wwusb.h"

#include <string.h>
#include <errno.h>
#include <sys/ioctl.h>
#include <sys/poll.h>

#include <assert.h>

// Initialize static data: 
int WWUSBDevice::URB::urb_serial_counter=1;

#define DEBUG_ASYNC_IO 0


// Totally evil and fragile extraction of file descriptor from
// guts of libusb.  They don't install usbi.h, which is what we'd need
// to do this nicely.
//
// FIXME if everything breaks someday in the future, look here...
static inline int fd_from_usb_dev_handle(usb_dev_handle *udh)
{
	return( *((int*)udh) );
}

// This function is from SSRP: Actually, we don't need it any more. 
// Danger, big, fragile KLUDGE.  The problem is that we want to be
// able to get from a usb_dev_handle back to a usb_device, and the
// right way to do this is buried in a non-installed include file.
static inline struct usb_device *dev_handle_to_dev(usb_dev_handle *udh)
{
	struct usb_dev_handle_kludge {
		int fd;
		struct usb_bus *bus;
		struct usb_device *device;
	};
	
	return(((struct usb_dev_handle_kludge *) udh)->device);
}

//==============================================================================

struct usb_device *WWUSBDevice::USBFindDevice(int vendor,int product,int nth)
{
	// We re-scan now to make sure not to miss devices which were plugged 
	// in in the meantime...
	// FIXME: The only problem is that this may make our udev pointer invalid 
	//        if the corresponding device was unplugged in the mean time (?). 
	//        You may see crashes here...
	usb_find_busses();
	usb_find_devices();
	
	for(usb_bus *b=usb_busses; b; b=b->next)
	{
		for(struct usb_device *d=b->devices; d; d=d->next)
		{
			if(d->descriptor.idVendor==vendor && 
			   d->descriptor.idProduct==product )
			{
				if(!nth--)
				{  return(d);  }
			}
		}
	}
	
	return(NULL);
}

struct usb_device *WWUSBDevice::USBFindDevice(const char *bus,const char *dev)
{
	// We re-scan now to make sure not to miss devices which were plugged 
	// in in the meantime...
	::usb_find_busses();
	::usb_find_devices();

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

//==============================================================================

WWUSBDevice::ErrorCode WWUSBDevice::_ErrorCodeRV(int e)
{
	assert(e>=0);
	return((ErrorCode)e);
}


WWUSBDevice::ErrorCode WWUSBDevice::_DoConnect(struct usb_device *d)
{
	if(udev || udh)
	{  return(ECConnected);  }
	
	udev=d;
	udh=::usb_open(udev);
	if(!udh)
	{  udev=NULL;  return(_ErrorCodeRV(errno));  }
	
	assert(dev_handle_to_dev(udh)==udev);
	
	return(ECSuccess);
}


WWUSBDevice::ErrorCode WWUSBDevice::connect(int vendor,int product,int nth)
{
	if(udev || udh)
	{  return(ECConnected);  }
	
	struct usb_device *ud=USBFindDevice(vendor,product,nth);
	if(!ud)
	{  return(ECNoSuchDevice);  }
	
	return(_DoConnect(ud));
}


WWUSBDevice::ErrorCode WWUSBDevice::connect(const char *bus,const char *dev)
{
	if(udev || udh)
	{  return(ECConnected);  }
	
	struct usb_device *ud=USBFindDevice(bus,dev);
	if(!ud)
	{  return(ECNoSuchDevice);  }
	
	return(_DoConnect(ud));
}


WWUSBDevice::ErrorCode WWUSBDevice::claim(int interface,int alt_interface)
{
	if(!udh)
	{  return(ECNotConnected);  }
	
	if(interface>=0)
	{
		if(::usb_claim_interface(udh,/*interface=*/interface)<0)
		{  return(_ErrorCodeRV(errno));  }
	}
	
	if(alt_interface>=0)
	{
		if(::usb_set_altinterface(udh,/*alt_interface=*/alt_interface)<0)
		{  return(_ErrorCodeRV(errno));  }
	}
	
	return(ECSuccess);
}


void WWUSBDevice::CancelAllPendingURBs(bool also_delete)
{
	int cnt=0;
	for(URB *_u=pending.last(); _u; cnt++)
	{
		URB *u=_u;
		_u=_u->prev;
		
		ErrorCode ec=_CancelURB(u);
		// We get errno=EINVAL when cancelling an URB which is already 
		// completed but not yet reaped. Ignore that. 
		if(ec && ec!=EINVAL)
		{
			fprintf(stderr,"Failed to cancel URB: ec=%d (%s)\n",
				ec,strerror(ec));
			// In case we get ENODEV (after a hot disconnect), the 
			// URB was already killed in kernel space. 
			if(ec==ENODEV)
			{
				pending.dequeue(u);
				--npending;
				DeleteURB(u);
			}
		}
	}
	
	if(cnt)
	{  fprintf(stderr,"Cancelled %d pending URBs (npending=%d)%s",
		cnt,npending,(also_delete && udh) ? ", reaping..." : "\n");  }
	
	if(also_delete && udh)
	{
		// Reap as much as we can. 
		int nreap=0;
		for(;;)
		{
			ErrorCode ec=ECSuccess;
			URB *u=_ReapURB(&ec,/*may_wait=*/0);
			if(u)
			{  ++nreap;  continue;  }
			if(ec==ECNoURBAvail)  break;
			// In all other cases, we break as well. Most notably 
			// when we get ECNotConnected...
			break;
		}
		
		if(cnt)
		{  fprintf(stderr," done reaping (%d): npending=%d\n",
			nreap,npending);  }
	}
	
	if(also_delete)
	{  assert(npending==0);  }
}


WWUSBDevice::ErrorCode WWUSBDevice::disconnect()
{
	// Cancel everything, free requests, close everything...
	CancelAllPendingURBs(/*also_delete=*/1);
	
	if(udh)
	{  ::usb_close(udh);  udh=NULL;  }
	udev=NULL;
	
	return(ECSuccess);
}


WWUSBDevice::ErrorCode WWUSBDevice::_SubmitURB(URB *u)
{
#if DEBUG_ASYNC_IO
	fprintf(stderr,"S(%d) ",u->urb_serial);
#endif
	
	usbdevfs_urb *dev_u=u;
	u->usercontext=u;  // <-- This is just some "magic" for checks...
	int rv=ioctl(fd_from_usb_dev_handle(udh),USBDEVFS_SUBMITURB,dev_u);
	if(rv<0)
	{  return(_ErrorCodeRV(errno));  }
	
	pending.append(u);
	++npending;
	//fprintf(stderr,"<<%d>>",npending);
	
	return(ECSuccess);
}


WWUSBDevice::ErrorCode WWUSBDevice::_CancelURB(URB *u)
{
#if DEBUG_ASYNC_IO
	fprintf(stderr,"C(%d) ",u->urb_serial);
#endif
	
	if(u->cancelled==2)  return(ECSuccess);
	
	usbdevfs_urb *dev_u=u;
	int rv=ioctl(fd_from_usb_dev_handle(udh),USBDEVFS_DISCARDURB,dev_u);
	if(rv<0)
	{
		u->cancelled=1;
		return(_ErrorCodeRV(errno));
	}
	
	u->cancelled=2;
	return(ECSuccess);
}


WWUSBDevice::URB *WWUSBDevice::_ReapURB(ErrorCode *ec_rv,bool may_wait)
{
	int fd=fd_from_usb_dev_handle(udh);
	usbdevfs_urb *dev_u=NULL;
	int reap_rv = may_wait ? 
		ioctl(fd,USBDEVFS_REAPURB,&dev_u) : 
		ioctl(fd,USBDEVFS_REAPURBNDELAY,&dev_u);
	int errn=errno;
	
	if(reap_rv<0)
	{
		if(errn==EAGAIN)
		{
			// Currently no URB to reap available. 
			*ec_rv=ECNoURBAvail;
		}
		else if(errn==ENODEV)
		{
			fprintf(stderr,"USB hot disconnect... What'chew doin' dude?\n");
			*ec_rv=ECNotConnected;
		}
		else
		{
			*ec_rv=_ErrorCodeRV(errn);
			fprintf(stderr,"reap_rv=%d, errno=%s\n",reap_rv,strerror(errn));
		}
		return(NULL);
	}
	
	assert(dev_u);
	URB *u=static_cast<URB*>(dev_u);
	assert(u->usercontext==u);  // <-- Here's the magic check. 
	
	// If this assert fails, you probably messed around with libusb...
	assert(pending.search(u));
	pending.dequeue(u);
	--npending;  assert(npending>=0);
	
#if DEBUG_ASYNC_IO
	fprintf(stderr,"R(%d) ",u->urb_serial);
#endif
	
	if(u->status && !(u->status==-ENOENT && u->cancelled==2))
	{
		fprintf(stderr,"URB (just reaped) status=%d (%s), cancelled=%d\n",
			u->status,strerror(u->status),u->cancelled);
	}
	
	*ec_rv=ECSuccess;
	return(u);
}


WWUSBDevice::ErrorCode WWUSBDevice::_ResetEP(uchar dir_ep)
{
	unsigned int ep=dir_ep;
	int rv=ioctl(fd_from_usb_dev_handle(udh),USBDEVFS_RESETEP,&ep);
	if(rv<0)
	{
		int errn=errno;
		fprintf(stderr,"Reset EP 0x%02x: %s\n",(int)ep,strerror(errn));
		_ErrorCodeRV(errn);
	}
	else  {  fprintf(stderr,"Reset EP 0x%02x: OK\n",(int)ep);  }
	
	return(ECSuccess);
}


WWUSBDevice::ErrorCode WWUSBDevice::ProcessEvents(int max_delay)
{
	if(!udh)
	{  return(ECNotConnected);  }
	
	int fd=fd_from_usb_dev_handle(udh);
	//fprintf(stderr,"fd=%d\n",fd);
	
	// First, see what we can get without waiting. 
	bool gather_phase=1;
	
	ErrorCode ec=ECSuccess;
	int errn=0;
	for(;;)
	{
		if(!npending)
		{  return(ECNoURBAvail);  }
		
//fprintf(stderr,"loop: gather_phase=%d\n",gather_phase);
		usbdevfs_urb *dev_u=NULL;
		bool may_wait;
		if(gather_phase || max_delay==0)
		{  may_wait=0;  }
		else if(max_delay>0)
		{
			struct pollfd pfd[1];
			pfd[0].fd=fd;
			pfd[0].events=POLLIN|POLLHUP;
			pfd[0].revents=0;
			int poll_rv=::poll(pfd,1,max_delay);
// FIXME!!! This does not work!!!!!!!!!!!!!!
fprintf(stderr,"fd=%d, poll_rv=%d, revents=0x%x \n",
	pfd[0].fd,poll_rv,pfd[0].revents);
			if(poll_rv<0)
			{
				fprintf(stderr,"poll: rv=%d (%s)\n",poll_rv,strerror(errno));
				// Actually, this may make the actual delay longer than 
				// max_delay (e.g. by periodically sending a singal to 
				// the process) but is not supposed to be a real problem. 
				continue;  // FIXME: This may be dangerous (except for EINTR). 
			}
			else if(poll_rv==0)
			{
				// Timeout. 
				ec=ECTimeout;
				break;
			}
			else
			{  may_wait=0;  }
		}
		else // max_delay<0. 
		{  may_wait=1;  }
		
		URB *u=_ReapURB(&ec,may_wait);
		if(!u)
		{
			if(ec==ECNoURBAvail)
			{
				// Currently no URB to reap available. 
				if((max_delay>0 && !gather_phase) || 
				   (max_delay==0 && gather_phase) )
				{
					ec=ECTimeout;
					break;
				}
				gather_phase=0;
				continue;
			}
			else if(ec==ECNotConnected)
			{  return(ECNotConnected);  }
			
			gather_phase=0;
			break;
		}
		
		// Tell user. 
		ErrorCode urv=URBNotify(u);
		
		// Now free the URB. 
		DeleteURB(u);
		
		if(urv)
		{
			ec = urv==ECUserQuitFatal ? ECUserQuitFatal : ECUserQuit;
			break;
		}
	}
	
	return(ec);
}


WWUSBDevice::ErrorCode WWUSBDevice::URBNotify(URB *)
{
	assert(!"Not overridden.");
	return(ECSuccess);
}


void WWUSBDevice::DeleteURB(URB *u)
{
	// Hope that's right...
	delete u;
}


WWUSBDevice::WWUSBDevice() : 
	udev(NULL),
	udh(NULL),
	pending(),
	npending(0)
{
	// Nothing to do...
}

WWUSBDevice::~WWUSBDevice()
{
	disconnect();
}

//------------------------------------------------------------------------------

void WWUSBDevice::URB::_buffer_oops()
{
	assert(buffer==NULL);
}
