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
	extern	parameter_buffers
	extern	sensor_sample_all

bank1	udata
	src_ptr			res	1
	dest_ptr		res	1
	byte_iterator	res	1	

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
	defineRequest	EFS_CTRL_GET_PARAM_BLOCK,	request_getParamBlock
	defineRequest	EFS_CTRL_READ_SENSORS,		request_readSensors

	pagesel	wrongstate		; Not a recognized request
	goto	wrongstate


;********************************************** Macros


	; Acknowledge the command with a zero-length packet
returnEmpty		macro
	pagesel	Send_0Len_pkt
	goto	Send_0Len_pkt
	endm


	; Return 'length' bytes of data copied from buffer[w]
returnBuffer	macro	buffer, length
	local	loop

	addlw	buffer
	banksel	src_ptr
	movwf	src_ptr

	banksel	BD0IAL
	movf	low BD0IAL,w	; Get the address of the EP0 IN buffer
	banksel	dest_ptr
	movwf	dest_ptr		; Start a buffer pointer we'll increment...

	movlw	length
	movwf	byte_iterator
loop

	bankisel buffer
	movf	src_ptr, w
	movwf	FSR

	movf	INDF, w			; Save the referenced byte in temp
	movwf	temp

	movf	dest_ptr,w		; get address of buffer
	movwf	FSR
	bsf 	STATUS,IRP		; indirectly to banks 2-3

	movf	temp, w			;  Write the saved byte to our buffer
	movwf	INDF

	incf	src_ptr, f		; Next...
	incf	dest_ptr, f
	banksel	byte_iterator
	decfsz	byte_iterator, f
	goto	loop

	banksel	BD0IBC
	bsf 	STATUS, RP0
	movlw	length
	movwf	BD0IBC			; set byte count
	movlw	0xc8			; DATA1 packet, DTS enabled
	movwf	BD0IST			; give buffer back to SIE
	return
	endm


;********************************************** Request handlers

request_setParamByte
	banksel BufferData
	movf	BufferData+wIndex, w
	andlw	.63						; Limit the range to our buffer size
	bankisel parameter_buffers
	addlw	parameter_buffers
	movwf	FSR

	; Copy in the byte from wValue
	movf	BufferData+wValue, w
	movwf	INDF
	returnEmpty


	; Return the wIndex'th parameter block in an 8-byte packet
request_getParamBlock
	banksel BufferData
	movf	BufferData+wIndex, w
	andlw	.7						; Limit the range to our 8 blocks
	movwf	temp					; Left-shift to convert from block number to byte offset
	rlf		temp, f
	rlf		temp, f
	rlf		temp, w
	returnBuffer	parameter_buffers, 8

request_readSensors
	pagesel	sensor_sample_all
	call	sensor_sample_all
	clrw
	returnBuffer	sensor_buffer, 8

	end

;### The End ###
