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

	extern	sensor_sampler

bank1	udata

sensor_params	res	8

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
	defineRequest	EFS_CTRL_TAKE_READING,		request_takeReading

	pagesel	wrongstate		; Not a recognized request
	goto	wrongstate

;********************************************** Request handlers

request_setParamByte
	; Point IRP:FSR at the applicable sensor_params byte (no bounds checking yet)
	banksel BufferData
	movf	BufferData+wIndex, w
	bankisel sensor_params
	addlw	sensor_params
	movwf	FSR

	; Copy in the byte from wValue
	movf	BufferData+wValue, w
	movwf	INDF
	
	; Acknowledge the request
	pagesel	Send_0Len_pkt
	goto	Send_0Len_pkt


request_takeReading
	bankisel sensor_params
	movlw	sensor_params
	movwf	FSR
	pagesel	sensor_sampler
	call	sensor_sampler

	movwf   temp

	banksel BD0IAL
	movf    low BD0IAL,w    ; get address of buffer
	movwf   FSR
	bsf     STATUS,IRP      ; indirectly to banks 2-3

	movf    temp, w         ;  Write the saved byte to our buffer
	movwf   INDF
	banksel BD0IBC
	movlw   0x01
	movwf   BD0IBC          ; set byte count to 1
	movlw   0xc8            ; DATA1 packet, DTS enabled
	movwf   BD0IST          ; give buffer back to SIE
	return

	end

;### The End ###
