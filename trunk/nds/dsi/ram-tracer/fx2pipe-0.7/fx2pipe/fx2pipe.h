/*
 * usb_io/fx2pipe.h
 * 
 * Cypress FX2 pipe IO main class. 
 * 
 * Copyright (c) 2006--2009 by Wolfgang Wieser ] wwieser (a) gmx <*> de [ 
 * 
 * This file may be distributed and/or modified under the terms of the 
 * GNU General Public License version 2 as published by the Free Software 
 * Foundation. (See COPYING.GPL for details.)
 * 
 * This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
 * WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
 * 
 */

#ifndef _INCLUDE_USBIO_FX2PIPE_H_
#define _INCLUDE_USBIO_FX2PIPE_H_ 1

#include "../oconfig.h"
#include "../lib/linkedlist.h"
#include "../usb_io/fx2usb.h"
#include "../usb_io/urbcache.h"

#include <sys/time.h>


// This is defined somewhere in main.cc
extern volatile int caught_sigint;
// Builtin firmware from ../firmware/fx2pipe_static.cc: 
extern const char *fx2pipe_static_firmware[];

/**
 * \short Cypress FX2 pipe IO main class. 
 * \author Wolfgang Wieser ] wwieser (a) gmx <*> de [
 * 
 * Not C++-safe. Not thread-safe. 
 */
class FX2Pipe : public FX2USBDevice
{
	public:
		/// URB used by USBIOThread. 
		struct MyURB : FX2Pipe::URB
		{
			/// URB cache against allocation overhead. 
			static URBCache urb_cache;
			
			/// Allocate buffer and attach it. Must be done before submission. 
			void AllocBuffer(size_t size);
			
			MyURB(uchar dir_ep,uchar _type=USBDEVFS_URB_TYPE_BULK);
			~MyURB();
			
			/** Custom operators new and delete for URB caching. */
			/// \{
			void *operator new(size_t size)
				{  return(urb_cache.get(size));  }
			void operator delete(void *ptr,size_t size)
				{  urb_cache.put(ptr,size);  }
			/// \}
		};
	
	private:
		/// Number of successive reaped URBs with errors. 
		/// May only be accessed by IO thread. 
		int successive_error_urbs;
		
		/// Error counter for exit value. 
		int x_errors;
		
		/// Start time when we submitted the initial URBs. 
		timeval starttime;
		/// End time when everything's done. 
		timeval endtime;
		/// Time of last status display update. 
		timeval last_update_time;
		/// Number of bytes transferred at last update. 
		int64 last_update_transferred;
		
		/// Submitted bytes: Number of bytes submitted but not all of them 
		/// are yet transferred. 
		int64 submitted_bytes;
		/// Transferred bytes: 
		/// Number of bytes successfully transferred. 
		int64 transferred_bytes;
		
		/// EOF on stdio (1) or transfer limit reached (2). 
		int stdio_eof;
		
		/// See FirwareConfig. 
		static const int FirmwareConfigAdr=0x1003;
		
		/// Write transferred bytes, time and transfer rate. 
		void _DisplayTransferStatistics(const timeval *endtime,int final);
		
		/// Initialize/connect to USB device. Also download firmware. 
		/// Returns error code. 
		int _ConnectAndInitUSB();
		/// Counterpart of ConnectAndInitUSB: cleanup. 
		void _CleanupUSB();
		
		/// Submit one URB. 0 on success; 1 on failure. 
		int _SubmitOneURB();
		/// Submit a bunch of URBs initially to fill the pipeline. 
		int _SubmitInitialURBs();
		/// Cancel all the pending osci data URBs. 
		void _CancelAllPendingDataURBs();
		
		/// Overriding virtual from WWUSBDevice. 
		ErrorCode URBNotify(URB *u);
		/// Overriding virtual from WWUSBDevice. 
		void DeleteURB(URB *u);
		
	public:
		/// Some config: 
		/// This config is meant to be set in the beginning and not to be 
		/// changed afterwards. 
		int n_th_usb_dev;  /// Connect to n-th matching USB device. Start with 0.
		
		/// Max number of bytes to transfer; negative for unlimited. 
		int64 transfer_limit;
		
		/// IO block size. 
		uint io_block_size;
		/// Pipeline size (number of URBs). 
		int pipeline_size;
		
		/// Direction: -1 -> IN (default); +1 -> OUT
		int dir;
		
		/// Don't use stdio but throw away data / write NUL data. 
		int no_stdio;
		
		/// SCHED_OTHER or SCHED_FIFO or SCHED_RR
		int schedule_policy;
		/// Priority; used if schedule_policy!=SCHED_OTHER. 
		int schedule_priority;
		
		/// Path to firmware IHX file. NULL for builtin firmware. 
		const char *firmware_hex_path;
		
		/// This is at address FirmwareConfigAdr in the FX2. 
		struct FirwareConfig
		{
			uchar FC_DIR;         // [0] 0x12 or 0x21
			uchar FC_IFCONFIG;    // [1]
			uchar FC_EPCFG;       // [2]
			uchar FC_EPFIFOCFG;   // [3]
			uchar FC_CPUCS;       // [4]
		}__attribute__((__packed__)) fc;
		
	private:
		/// Do not use. 
		FX2Pipe(const FX2Pipe &);
		/// Do not use. 
		void operator=(const FX2Pipe &);
	public:
		FX2Pipe();
		~FX2Pipe();
		
		/// Actually run the IO code...
		int run();
};

#endif  /* _INCLUDE_USBIO_FX2PIPE_H_ */
