;################################################################################
;
; descript.asm - USB descriptors for the LED board project
;
; This file is part of the LED board project. This file is based
; on the revision 2.00 USB firmware distributed by Microchip for use with the
; PIC16C745 and PIC16C765 microcontrollers. New code added for this project
; is placed in the public domain.
;
; LED board modifications done by Micah Dowty <micah@navi.cx>
;
;###############################################################################
;
; The original license agreement and author information for the USB firmware follow:
;
;                            Software License Agreement
;
; The software supplied herewith by Microchip Technology Incorporated (the "Company")
; for its PICmicro(r) Microcontroller is intended and supplied to you, the Company's
; customer, for use solely and exclusively on Microchip PICmicro Microcontroller
; products.
;
; The software is owned by the Company and/or its supplier, and is protected under
; applicable copyright laws. All rights are reserved. Any use in violation of the
; foregoing restrictions may subject the user to criminal sanctions under applicable
; laws, as well as to civil liability for the breach of the terms and conditions of
; this license.
;
; THIS SOFTWARE IS PROVIDED IN AN "AS IS" CONDITION. NO WARRANTIES, WHETHER EXPRESS,
; IMPLIED OR STATUTORY, INCLUDING, BUT NOT LIMITED TO, IMPLIED WARRANTIES OF
; MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE APPLY TO THIS SOFTWARE. THE
; COMPANY SHALL NOT, IN ANY CIRCUMSTANCES, BE LIABLE FOR SPECIAL, INCIDENTAL OR
; CONSEQUENTIAL DAMAGES, FOR ANY REASON WHATSOEVER.
;
;	Author:			Dan Butler and Reston Condit
;	Company:		Microchip Technology Inc
;
;################################################################################

	errorlevel -226		; suppress gpasm's silly include file warning

#include <p16c765.inc>
#include "usb_defs.inc"
#include "../include/ledboard_protocol.h"

descr_page	code
	global	Config_desc_index
	global	Descriptions
	global	DeviceDescriptor
	global	String0
	global	StringDescriptions
	global	GetStringIndex

	extern	EP0_start
	extern	temp		; temp var used in get config index
	extern	temp2 		; another temp, in bank2

; ******************************************************************
; Given a configuration descriptor index, returns the beginning address
; of the descriptor within the descriptions table
; ******************************************************************
Config_desc_index
	movwf	temp
	movlw	HIGH CDI_start
	movwf	PCLATH
	movlw	low CDI_start
	addwf	temp,w
	btfsc	STATUS,C
	incf	PCLATH,f
	movwf	PCL
CDI_start			; this table calculates the offsets for each
	retlw	low  Config1	; configuration descriptor from the beginning
	retlw	high Config1	; of the table
	; more configurations can be added here
	; retlw   low Config2
	; retlw   high Config2
	; etc....

; ******************************************************************
; This table is polled by the host immediately after USB Reset has been released.
; This table defines the maximum packet size EP0 can take.
; See section 9.6.1 of the Rev 1.0 USB specification.
; These fields are application DEPENDENT. Modify these to meet
; your specifications.
; the offset is passed in P0 and P1 (P0 is low order byte).
; ******************************************************************
Descriptions
	banksel	EP0_start
	movf	EP0_start+1,w
	movwf	PCLATH
	movf	EP0_start,w
	movwf	PCL

DeviceDescriptor
StartDevDescr
	retlw	0x12		; bLengthLength of this descriptor
	retlw	0x01		; bDescType This is a DEVICE descriptor
	retlw	0x10		; bcdUSBUSB revision 1.10 (low byte)
	retlw	0x01		; high byte
	retlw	0x00		; bDeviceClasszero means each interface operates independently
	retlw	0x00		; bDeviceSubClass
	retlw	0x00		; bDeviceProtocol
	retlw	0x08		; bMaxPacketSize0 - inited in UsbInit()
	retlw	LEDBOARD_VENDOR_ID & 0xFF		; idVendor
	retlw	LEDBOARD_VENDOR_ID >> 8
	retlw	LEDBOARD_PRODUCT_ID & 0xFF		; idProduct
	retlw	LEDBOARD_PRODUCT_ID >> 8
	retlw	LEDBOARD_PROTOCOL_VERSION & 0xFF	; bcdDevice
	retlw	LEDBOARD_PROTOCOL_VERSION >> 8
	retlw	0x01		; iManufacturer
	retlw	0x02		; iProduct
	retlw	0x00		; iSerialNumber - 3
	retlw	NUM_CONFIGURATIONS	; bNumConfigurations

; ******************************************************************
; This table is retrieved by the host after the address has been set.
; This table defines the configurations available for the device.
; See section 9.6.2 of the Rev 1.0 USB specification (page 184).
; These fields are application DEPENDENT.
; Modify these to meet your specifications.
; ******************************************************************
Config1
	retlw	0x09		; bLengthLength of this descriptor
	retlw	0x02		; bDescType2 = CONFIGURATION
	retlw	EndConfig1 - Config1
	retlw	0x00
	retlw	0x01		; bNumInterfacesNumber of interfaces
	retlw	0x01		; bConfigValueConfiguration Value
	retlw	0x00		; iConfigString Index for this config = none
	retlw	0xA0		; bmAttributesattributes - bus powered
	retlw	0x32		; MaxPowerself-powered draws 100 mA from the bus.
Interface1
	retlw	0x09		; length of descriptor
	retlw	INTERFACE
	retlw	0x00		; number of interface, 0 based array
	retlw	0x00		; alternate setting
	retlw	0x00		; number of endpoints used in this interface
	retlw	0xFF		; interface class - vendor specific
	retlw	0xFF		; vendor specific subclass (not a boot device)
	retlw	0xFF		; vendor specific protocol
	retlw	0x00		; index to string descriptor that describes this interface = none
EndConfig1


; ******************************************************************
; Given a string descriptor index, returns the beginning address
; of the descriptor within the descriptions table
; ******************************************************************
GetStringIndex	; langid in W reg, string offset in EP0_start
	movwf	temp
	movlw	HIGH StringIndexTable
	movwf	PCLATH
	movlw	LOW StringIndexTable
	addwf	temp,w
	btfsc	STATUS,C
	incf	PCLATH,f
	movwf	PCL

StringIndexTable
	retlw	low  String0	; LangIDs
	retlw	high String0
	retlw	low  String1
	retlw	high String1
	retlw	low  String2
	retlw	high String2

StringDescriptions
	banksel	EP0_start
	movf	EP0_start+1,w
	movwf	PCLATH
	movf	EP0_start,w
	movwf	PCL

String0
	retlw	low (String1 - String0)	; length of string
	retlw	0x03		; descriptor type 3?
	retlw	0x09		; language ID (as defined by MS 0x0409)
	retlw	0x04
String1
	retlw	String2-String1	; length of string
	retlw	0x03		; string descriptor type 3
	retlw	'M'
	retlw	0x00
	retlw	'i'
	retlw	0x00
	retlw	'c'
	retlw	0x00
	retlw	'a'
	retlw	0x00
	retlw	'h'
	retlw	0x00
	retlw	' '
	retlw	0x00
	retlw	'D'
	retlw	0x00
	retlw	'o'
	retlw	0x00
	retlw	'w'
	retlw	0x00
	retlw	't'
	retlw	0x00
	retlw	'y'
	retlw	0x00
	retlw	' '
	retlw	0x00
	retlw	'<'
	retlw	0x00
	retlw	'm'
	retlw	0x00
	retlw	'i'
	retlw	0x00
	retlw	'c'
	retlw	0x00
	retlw	'a'
	retlw	0x00
	retlw	'h'
	retlw	0x00
	retlw	'@'
	retlw	0x00
	retlw	'n'
	retlw	0x00
	retlw	'a'
	retlw	0x00
	retlw	'v'
	retlw	0x00
	retlw	'i'
	retlw	0x00
	retlw	'.'
	retlw	0x00
	retlw	'c'
	retlw	0x00
	retlw	'x'
	retlw	0x00
	retlw	'>'
	retlw	0x00
String2
	retlw	String3-String2
	retlw	0x03
	retlw	'L'
	retlw	0x00
	retlw	'E'
	retlw	0x00
	retlw	'D'
	retlw	0x00
	retlw	' '
	retlw	0x00
	retlw	'b'
	retlw	0x00
	retlw	'o'
	retlw	0x00
	retlw	'a'
	retlw	0x00
	retlw	'r'
	retlw	0x00
	retlw	'd'
	retlw	0x00
	retlw	' '
	retlw	0x00
	retlw	'U'
	retlw	0x00
	retlw	'S'
	retlw	0x00
	retlw	'B'
	retlw	0x00
	retlw	'/'
	retlw	0x00
	retlw	'1'
	retlw	0x00
	retlw	'4'
	retlw	0x00
	retlw	'4'
	retlw	0x00
String3
	end

;### The End ###
