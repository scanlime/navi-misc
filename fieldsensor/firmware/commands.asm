;################################################################################
;
; commands.asm - Implementation of our vendor-specific commands
;
; This file is part of the USB Electric Field Sensor project. This file contains
; original code, released into the public domain.
;
; Micah Dowty <micah@picogui.org>
;
;###############################################################################

#include <p16C745.inc>
#include "usb_defs.inc"
#include "../include/efs_protocol.h"

	errorlevel -302		; supress "register not in bank0, check page bits" message

	global	CheckVendor

	extern	BufferData
	extern	wrongstate
	extern	temp
	extern	Send_0Len_pkt

bank1	udata

	code

; *********************************************************************
; Vendor Specific calls
; control is transferred here when bmRequestType bits 5 & 6 = 10 indicating
; the request is a vendor specific request.  This function then would
; interpret the bRequest field to determine what action is required.
; The end of each vendor specific command should be terminated with a
; return.
; *********************************************************************
CheckVendor
	movf	BufferData+bRequest,w
	xorlw	EFS_CTRL_SET_FREQUENCY
	pagesel	SetFrequencyRequest
	btfsc	STATUS,Z
	goto	SetFrequencyRequest

	pagesel	wrongstate		; Not a recognized request
	goto	wrongstate

	;********************* Request to set one channel's modulation frequency
	; Channel number in wIndex, PR2 value in wValue.
SetFrequencyRequest
	banksel BufferData
	movf	BufferData+wValue, w
	banksel	PR2
	movwf	PR2

	; Acknowledge the request
	pagesel	Send_0Len_pkt
	call	Send_0Len_pkt
	return

	end

;### The End ###
