/*
 * usb_io/wwusb.h
 * 
 * USB async low-level routines. 
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

#ifndef _INCLUDE_USBIO_WWUSB_H_
#define _INCLUDE_USBIO_WWUSB_H_ 1

#include "../oconfig.h"
#include "../lib/linkedlist.h"

#include <string.h>
#include <usb.h>
#include <linux/usbdevice_fs.h>  /* usbdevfs_urb */


typedef unsigned char uchar;
struct usb_device;

/*
struct usbdevfs_urb {
	unsigned char type;
	unsigned char endpoint;
	int status;
	unsigned int flags;
	void __user *buffer;
	int buffer_length;  // <-- Max: MAX_USBFS_BUFFER_SIZE=16384
	int actual_length;
	int start_frame;
	int number_of_packets;
	int error_count;
	unsigned int signr;
	void *usercontext;
	struct usbdevfs_iso_packet_desc iso_frame_desc[0];
};
*/

/**
 * \short USB Device representation. 
 * \author Wolfgang Wieser ] wwieser (a) gmx <*> de [
 * 
 * The WWUSBDevice represents an USB device which can have several endpoints 
 * and communication in both directions (IN and OUT). 
 * 
 * The idea is to derive a class from this one and override some virtual 
 * functions to actually implement USB communication. 
 * 
 * NOTE: You canNOT use libusb routines in order to talk to a device while 
 *       having it opened using WWUSBDevice. Even not if the libusb code is 
 *       talking to a different endpoint of the device. The reason is that 
 *       libusb is likely to reap URBs which were submitted by us and vice 
 *       versa. So, be sure to know what you're doing. 
 * 
 * Not thread-safe. Only one thread should to all the USB actions. Otherwise 
 * a synchronisation layer must be added. 
 * 
 * Not "C++-safe", i.e. do not copy or assign; corresponding operators are 
 * made private by purpose. 
 */
class WWUSBDevice
{
	public:
		/// Error code enum. 
		/// Negative codes are private codes; positive ones are errno values. 
		enum ErrorCode
		{
			ECSuccess=0,      ///< No error. You can rely on this being 0. 
			ECConnected=-100, ///< (Already) connected. 
			ECNotConnected,   ///< Not connected to a device. 
			ECNoSuchDevice,   ///< No such USB device. 
			ECFailure,        ///< Unspecified failure. 
			ECTimeout,        ///< Timeout. 
			ECUserQuit,       ///< Event loop left by user request. 
			ECUserQuitFatal,  ///< Fatal version of ECUserQuit. 
			ECNoURBAvail,     ///< Internally used by _ReapURB(). 
		};
		
		/**
		 * \short User request block base struct. 
		 * 
		 * URB handling is somewhat ciritcal since we want little overhead 
		 * and hence need to be careful with buffer allocation to avoid 
		 * spurious copying. 
		 * 
		 * So, it works as follows: URBs are allocated via operator new 
		 * and deleted via operator delete. URBs are always allocated from 
		 * user code and then get an apropriate buffer attached. Deallocation 
		 * from within WWUSBDevice is done via WWUSBDevice::DeleteURB() which 
		 * can be overridden to provide URB caching, see below. Note that 
		 * kernel copies the URB data buffer of OUT URBs upon submission so 
		 * buffer re-use is possible. The URB destructor must be able to 
		 * correctly deallocate an attached buffer so that the URB 
		 * cancellation code does not introducing a mem leak when deleting 
		 * a pending URB. 
		 * 
		 * Note that it is recommended to use URB caching to avoid 
		 * (de)allocation overhead. For that, implement custom operators 
		 * for new and delete: Operator new dequeues the memory chunk from 
		 * a list and operator delete will put it back (after the destructor 
		 * has been called). To make sure that the correct operators are 
		 * called, WWUSBDevice uses WWUSBDevice::DeleteURB() for URB deletion 
		 * which should be overridden with a "delete" statement first applying 
		 * the apropriate type cast. 
		 */
		struct URB : LinkedListBase<URB>, usbdevfs_urb
		{
			/// 0 -> not cancelled; 1 -> cancellation failed; 2 -> cancelled. 
			int cancelled;
			
			/// For debugging: serial number. 
			int urb_serial;
			static int urb_serial_counter;
			
			/// See destructor. 
			void _buffer_oops();
			
			/**
			 * \short URB constructor without buffer assignment. 
			 * 
			 * dir_ep is direction and endpoint (bit 7 = 0x80 set for IN). 
			 * Construcor will NUL out all the struct first. 
			 * 
			 * In order to allocate a new URB, use operator new or use 
			 * the URB cache (urbcache.h) which is most easily implemented 
			 * by overriding operators new and delete with URB cache's 
			 * get() and put() functions. 
			 */
			inline URB(uchar dir_ep,uchar _type=USBDEVFS_URB_TYPE_BULK) : 
				LinkedListBase<URB>()
				{
					usbdevfs_urb *u=this;  memset(u,0,sizeof(usbdevfs_urb));
					endpoint=dir_ep;  type=_type;  cancelled=0;
					urb_serial=urb_serial_counter++;
				}
			/// URB destructor of derived class MUST also free attached buffer. 
			virtual inline ~URB()
				{  if(buffer) _buffer_oops();  }
			
			// Get direction: -1 -> IN; +1 -> OUT. 
			inline int dir() const
				{  return((endpoint & 0x80) ? -1 : +1);  }
			// Get endpoint. 
			inline int ep() const
				{  return(endpoint & 0x7f);  }
		};
		
		/// Find n-th (nth) device with specified vendor and product ID. 
		static struct usb_device *USBFindDevice(
			int vendor,int product,int nth=0);
		/// Find device with specified vendor and device number. 
		static struct usb_device *USBFindDevice(
			const char *bus,const char *dev);
		
	protected:
		/// USB device descriptor from libusb. 
		struct usb_device *udev;
		/// USB device handle from libusb. 
		struct usb_dev_handle *udh;
		
		/**
		 * \short Notification of URB completion. 
		 * 
		 * This is called when an URB got processed and was just reaped. 
		 * 
		 * The URB will be deleted (using DeleteURB()) after returning 
		 * from the call to URBNotify() (so you need to detach the URB 
		 * buffer in URBNotify() if you still need it afterwards to prevent 
		 * it from being free'd in DeleteURB()). 
		 * 
		 * The return value should normally be 0; use ECUserQuit or 
		 * ECUserQuitFatal to exit the event loop. 
		 */
		virtual ErrorCode URBNotify(URB *u);
		
		/// Delete an URB. Never uses delete directly; see URB for details. 
		/// Default implementation will use operator delete directly. 
		virtual void DeleteURB(URB *u);
		
		/**
		 * \short Submit an URB. 
		 * 
		 * Call this to have an URB queued in the kernel. 
		 * 
		 * May be called from within URBNotify() but NOT with the 
		 * "original" URB (passed to URBNotify()). 
		 */
		ErrorCode SubmitURB(URB *u)
			{  return(_SubmitURB(u));  }
		/**
		 * \short Cancel a submitted URB. 
		 * 
		 * Used to get rid of an URB which may not yet be processed. 
		 * 
		 * May be called from within URBNotify() but NOT with the 
		 * "original" URB (passed to URBNotify()). 
		 * 
		 * NOTE: A canceled URB will still be reaped. So don't delete them 
		 *       unless you close the device afterwards anyways. 
		 */
		ErrorCode CancelURB(URB *u)
			{  return(_CancelURB(u));  }
		
		/**
		 * \short Cancel all pending URBs. 
		 * 
		 * May be called from within URBNotify(). 
		 * 
		 * If also_delete is set, the URBs are also reaped and deleted. 
		 * See CancelURB(). 
		 */
		void CancelAllPendingURBs(bool also_delete=0);
		
		/**
		 * \short Reap and URB. 
		 * 
		 * Will reap one URB (wait for it if may_wait it set, otherwise 
		 * return immediately) and remove it from the pending list. 
		 * Magic check is performed and errors are reported. 
		 * 
		 * Returns an URB or NULL. 
		 * If NULL, see *ec_rv for details; this may be an errno value or 
		 * ECNoURBAvail if currently no URB available (and may_wait was set 
		 * to 0), ECNotConnected if disconnected. 
		 */
		URB *_ReapURB(ErrorCode *ec_rv,bool may_wait);
		
		/// Reset endpoint (if needed). 
		ErrorCode _ResetEP(uchar ep);
		
	protected:
		/// List of pending (i.e submitted but not yet completed) requests. 
		LinkedList<URB> pending;
		/// Number of pending URBs. 
		int npending;
		
	private:
		/// Internal function for error code "conversion". 
		ErrorCode _ErrorCodeRV(int errno_val);
		
		/// Connect to the passed USB device.  
		ErrorCode _DoConnect(struct usb_device *d);
		
		/// Submit URB and append it to pending on success. Usual return value. 
		ErrorCode _SubmitURB(URB *u);
		/// Cancel the URB. This will NOT remove it from the pending list 
		/// since cancelled URBs can still be reaped. 
		ErrorCode _CancelURB(URB *u);
		
	private:
		/// Do not use. 
		void operator=(const WWUSBDevice &);
		/// Do not use. 
		WWUSBDevice(const WWUSBDevice &);
	public:
		/// Constructor creates a non-set-up device. Use connect(). 
		WWUSBDevice();
		/// Destructor. Will also disconnect. 
		virtual ~WWUSBDevice();
		
		/// Get number of pending (not yet reaped) URBs. 
		inline int NPending() const
			{  return(npending);  }
		
		/**
		 * \short Connect to a (physical) USB device attached to the USB. 
		 * 
		 * If already connected, use disconnect() before. 
		 * 
		 * There are 2 versions available which differ only in the way 
		 * the device to connect is chosen, e.g. if the output of "lsusb" 
		 * is the following: 
		 * \code
		 * Bus 006 Device 002: ID 04b4:8613 Cypress CY7C68013 EZ-USB...
		 * \endcode
		 * then you can use vendor=0x04b4, product=0x8613 or you can use 
		 * bus="006", dev="002". The latter has the advantage that if 
		 * two identical devices are plugged into the box, they can be 
		 * uniquely addressed while the former version will choose the 
		 * one listed first (nth=0) or second (nth=1), etc. 
		 * 
		 * Return codes: 
		 *  ECSuccess\n
		 *  ECConnected (already connected to some device; disconnect() first)\n
		 *  ECNoSuchDevice (specified device is not present)\n
		 */
		/// \{
		ErrorCode connect(int vendor,int product,int nth=0);
		ErrorCode connect(const char *bus,const char *dev);
		/// \}
		
		/**
		 * \short Claim interface. 
		 * 
		 * Call after connect() to claim the specified interface (if>=0) and 
		 * use the alternate interface alt_interface (if >=0). 
		 */
		ErrorCode claim(int interface,int alt_interface);
		
		/**
		 * \short Disconnect from device. 
		 * 
		 * This will also cancel all URBs and free them. 
		 * 
		 * Will return ECSuccess if not connected so it's safe to call 
		 * more often than needed. 
		 */
		ErrorCode disconnect();
		
		/**
		 * \short Main (event) loop. 
		 * 
		 * Call this to have URBs reaped (URBNotify()). 
		 * 
		 * The loop will generally process as many URBs as possible. 
		 * Exiting the event loop can be achieved by returning 1 in 
		 * URBNotify(). 
		 * If you set max_delay>=0, the function will also return as soon 
		 * as an URB reap request will block for more than max_delay msec. 
		 * If max_delay<0, the function may block infinitely. 
		 * 
		 * NOTE: max_delay is the max delay between URBs, NOT the max. 
		 *       total delay spent in the function. 
		 * 
		 * Return value: \n
		 *   ECUserQuit: URBNotify() returned 1. \n
		 *   ECTimeout: max_delay timeout. \n
		 *   ECNotConnected: if disconnected in action \n
		 *   ECNoURBAvail: no more pending URBs \n
		 *   other errno codes
		 */
		ErrorCode ProcessEvents(int max_delay);
};

#endif  /* _INCLUDE_USBIO_WWUSB_H_ */
