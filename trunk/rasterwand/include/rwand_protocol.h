;//################################################################################
;//
;// rwand_protocol.h - Definitions describing the host to device protocol used by the
;//                    Raster Wand. This file is both valid C and assembly source, so it can be
;//                    shared by the firmware and kernel code.
;//
;// This file is part of the Raster Wand project.
;// Micah Dowty <micah@navi.cx>
;//
;//###############################################################################

#ifndef __RWAND_PROTOCOL_H
#define __RWAND_PROTOCOL_H


;//************************************************** Idenfification

;// The protocol version, stored in binary coded decimal.
;// This is available from the device in the bcdDevice field
;// of its DEVICE descriptor.
#define RWAND_PROTOCOL_VERSION  0x0100

;// Device vendor and product IDs.
#define RWAND_VENDOR_ID   0xE461
#define RWAND_PRODUCT_ID  0x0005


;//************************************************** Control Requests

;// Return an 8-byte packet with the current edge buffer (4 16-bit LE words)
#define RWAND_CTRL_READ_EDGES		0x01

;// Return a 5-byte packet containing:
;//   * Predicted period as a 16-bit Little Endian value in 16-cycle (2.66us) units
;//   * Predicted phase, same format as the period
;//   * Edge counter, 8-bit unsigned int. Increments once for each synchronization edge.
;//     This can be used to detect a stalled wand or other condition that would give bad
;//     predictions.
#define RWAND_CTRL_READ_PREDICTION	0x02

;// Set the phase to wValue and the period to wIndex
#define RWAND_CTRL_SET_PREDICTION	0x03

;// Set the coil driver to switch on when the predicted phase is between wValue and wIndex
#define RWAND_CTRL_SET_COIL_PHASE	0x04

;// Set the mode flags from the low byte of wValue
#define RWAND_CTRL_SET_MODES		0x05

;// Set the forward display phase (wValue) and reverse display phase (wIndex)
#define RWAND_CTRL_SET_DISPLAY_PHASE 0x06

;// Set the display's column width (wValue)
#define RWAND_CTRL_SET_COLUMN_WIDTH	0x07

;// Starting at the column given in the low byte of wValue, write
;// the values at wValue+1, wIndex, and wIndex+1 to the backbuffer.
#define RWAND_CTRL_RANDOM_WRITE3	0x08

;// Request a back->front page flip immediately preceeding the next display scan.
;// No more backbuffer writes should be performed until the flip has been
;// confirmed to be completed.
#define RWAND_CTRL_FLIP				0x09


;//************************************************** Mode bits

;// When this bit is on, coil phase and period are set automatically.
;// When this is off, edges aren't detected and no synchronization parameters will be updated.
#define RWAND_MODE_ENABLE_SYNC_BIT	0
#define RWAND_MODE_ENABLE_SYNC		0x01

;// When this bit is on, the coil drivers are run according to
;// the current angle predictor and coil drive window.
#define RWAND_MODE_ENABLE_COIL_BIT	1
#define RWAND_MODE_ENABLE_COIL		0x02

;// When this bit is on, the LEDs are driven from video memory
;// using the currently set display timings. When unset, the display is blanked.
#define RWAND_MODE_ENABLE_DISPLAY_BIT 2
#define RWAND_MODE_ENABLE_DISPLAY	0x04

;// When this bit is on, show the current coil driver status on the LEDs.
;// This must be off for normal display.
#define RWAND_MODE_COIL_DEBUG_BIT	3
#define RWAND_MODE_COIL_DEBUG		0x08

#endif

;//### The End ###

