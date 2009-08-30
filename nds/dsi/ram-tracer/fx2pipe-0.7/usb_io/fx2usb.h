/*
 * usb_io/fx2usb.h
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

#ifndef _INCLUDE_USBIO_FX2USB_H_
#define _INCLUDE_USBIO_FX2USB_H_ 1

#include "../oconfig.h"
#include "../usb_io/wwusb.h"


/**
 * \short FX2 USB Device representation. 
 * \author Wolfgang Wieser ] wwieser (a) gmx <*> de [
 * 
 * Derived from WWUSBDevice; provides a simple extension for certain 
 * things like firmware downloading. 
 */
class FX2USBDevice : public WWUSBDevice
{
	private:
		
		/// Internally used by connect() to download firmware. 
		ErrorCode _DownloadFirmware(struct usb_device *usbdev,const char *path,
			const char **static_firmware,
			const char *cfg_buf,size_t cfg_len,size_t cfg_adr);
	private:
		/// Do not use. 
		void operator=(const FX2USBDevice &);
		/// Do not use. 
		FX2USBDevice(const FX2USBDevice &);
	public:
		/// Create not-set-up FX2 device. 
		FX2USBDevice();
		/// Destructor...
		virtual ~FX2USBDevice();
		
		/**
		 * \short Connect to FX2 and dowload firmware. 
		 * 
		 * NOTE: When connecting an FX2 device, do not use one of the 
		 *       connect() calls from WWUSBDevice. Instead use this one. 
		 * 
		 * This will first find the FX2 device to use, download the firmware, 
		 * disconnect [to give the FX2 a chance for its re-numeration which is 
		 * probably pointless in this case] and then connect again. 
		 * 
		 * Step-by-step: 
		 * 
		 * Will first trigger a disconnect() first to be sure. 
		 * 
		 * Then, will look for a device with vendor and product IDs 
		 * initial_vendor,initial_product or (if not found) for an 
		 * unconfigured FX2. (Use -1,-1 to switch off.) 
		 * Actually, the code will look for the n_th match; use 0 for the 
		 * first device, 1 for the second, etc. 
		 * 
		 * Then, will connect, download firmware and disconnect. 
		 * The firmware is taken from the file firmware_path which must be 
		 * an Intel HEX file. If firmware_path is NULL, an alternate 
		 * firmware can be specified as static_firmware which is a 
		 * NULL-terminated array of strings; each element being one line of an 
		 * Intel HEX file. If firmware_path is an empty string, no firmware 
		 * will be downloaded. 
		 * 
		 * You can pass an additional config in cfg_buf of size cfg_len 
		 * which will be put at address cfg_adr before taking the FX2 out 
		 * of the reset. If unused, use NULL,0,0. 
		 * 
		 * Then, will connect to the device again. We use bus and device 
		 * paths ("IDs") since these will not change even if the USB IDs 
		 * change due to re-numeration. 
		 * 
		 * Errors are written to stderr. 
		 */
		ErrorCode connect(int initial_vendor,int initial_product,int n_th,
			const char *firmware_path,const char **static_firmware,
			const char *cfg_buf,size_t cfg_len,size_t cfg_adr);
};

#endif  /* _INCLUDE_USBIO_FX2USB_H_ */
