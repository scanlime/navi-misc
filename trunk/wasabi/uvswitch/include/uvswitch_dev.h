/*
 * USB Video Switch userspace interface
 *
 * This header describes the interface between the uvswitch kernel module and
 * any userspace applications wishing to use the device.
 *
 * Copyright(c) 2003 Micah Dowty <micah@picogui.org>
 *
 *	This program is free software; you can redistribute it and/or modify
 *	it under the terms of the GNU General Public License as published by
 *	the Free Software Foundation; either version 2 of the License, or
 *	(at your option) any later version.
 */

#ifndef __UVSWITCH_DEV_H
#define __UVSWITCH_DEV_H

#include <asm/types.h>
#include "uvswitch_protocol.h"

/* We have 8 video/audio channels. This doesn't count the bypass switch input */
#define UVSWITCH_CHANNELS  8

/************************************************** Device node ***************/

/* Information about the uvswitch's device node in /dev/usb/ */
#define UVSWITCH_DEV_NAMEFORMAT   "uvswitch%d"
#define UVSWITCH_MINOR_BASE       220


/************************************************** ioctl()s ******************/

struct uvswitch_calibration {
	/* Number of reads to ignore for precharging the ADC hold capacitor. Between 1 and 255 */
	int precharge_reads;

	/* Number of reads to accumulate for each packet sent from uvswitch to host. Between 1 and 255 */
	int integration_reads;

	/* Number of packets to accumulate before applying the threshold */
	int integration_packets;

	/* Threshold to test the final accumulated value against to check for active video inputs */
	int threshold;
};

/* Send a uvswitch_calibration structure */
#define UVSWITCHIO_ADC_CALIBRATE  0x3901

/* Read the values being used for each channel before threshold testing into an array of UVSWITCH_CHANNELS integers */
#define UVSWITCHIO_ADC_READ_RAW   0x3902

/* Set the default ADC calibration */
#define UVSWITCHIO_ADC_RESET      0x3903


/************************************************** read/write ******************/

/* Write an ASCII string of one of the following forms to set the switch:
 *
 *    <video/audio channel>
 *    <video/audio channel> <bypass enable>
 *    <video channel> <bypass enable> <audio channel>
 *    <video channel> <bypass enable> <white audio channel> <red audio channel>
 *
 */

/* Reading from the device yields a space-separated list of active channel numbers.
 * The line is resent every time it changes.
 */

#endif /* __UVSWITCH_DEV_H */

/* The End */
