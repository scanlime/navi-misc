;################################################################################
;
; commands.asm - Implementation of our vendor-specific commands
;
; The USB Electric Field Sensor project
; Copyright (C) 2004 Micah Dowty <micah@navi.cx>
;
;  This library is free software; you can redistribute it and/or
;  modify it under the terms of the GNU Lesser General Public
;  License as published by the Free Software Foundation; either
;  version 2.1 of the License, or (at your option) any later version.
;
;  This library is distributed in the hope that it will be useful,
;  but WITHOUT ANY WARRANTY; without even the implied warranty of
;  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
;  Lesser General Public License for more details.
;
;  You should have received a copy of the GNU Lesser General Public
;  License along with this library; if not, write to the Free Software
;  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
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

	extern	sensor_buffer

	code

;********************************************** Vendor-specific request table

defineRequest	macro	id,	handler
	movf	BufferData+bRequest,w
	xorlw	id
	pagesel	handler
	btfsc	STATUS,Z
	goto	handler
	endm

CheckVendor
	defineRequest	EFS_CTRL_SET_PARAM_BYTE,	request_setParamByte

	pagesel	wrongstate		; Not a recognized request
	goto	wrongstate

;********************************************** Request handlers

request_setParamByte
	banksel BufferData
	movf	BufferData+wIndex, w
	andlw	.63						; Limit the range to our buffer size
	bankisel sensor_buffer
	addlw	sensor_buffer
	movwf	FSR

	; Copy in the byte from wValue
	movf	BufferData+wValue, w
	movwf	INDF
	
	; Acknowledge the request
	pagesel	Send_0Len_pkt
	goto	Send_0Len_pkt

	end

;### The End ###
