;################################################################################
;
; commands.asm - Implementation of our vendor-specific commands
;
; USB Laser Projector
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
#include "hardware.inc"
#include "usb_defs.inc"
#include "../include/laser_protocol.h"

	errorlevel -302		; supress "register not in bank0, check page bits" message

	global	CheckVendor

	extern	USB_dev_req
	extern	BufferData
	extern	wrongstate
	extern	temp
	extern	Send_0Len_pkt
	extern	ServiceTimer

	extern	num_queued_events
	extern	buffer_head
	extern	x_buffer
	extern	y_buffer
	extern	duration_buffer
	extern	misc_buffer
	extern	tmr1_adj

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
	defineRequest	LASER_CTRL_ENQUEUE,      request_enqueue
	defineRequest	LASER_CTRL_SET_TIMEBASE, request_setTimebase

	pagesel	wrongstate		; Not a recognized request
	goto	wrongstate


;********************************************** Macros


	; Acknowledge the command with a zero-length packet
returnEmpty		macro
	pagesel	Send_0Len_pkt
	goto	Send_0Len_pkt
	endm


;********************************************** Request handlers

	; Queue up a new event:
	;   wValue low  -> x_buffer
	;   wValue high -> y_buffer
	;   wIndex low  -> duration_buffer
	;   wIndex high -> misc_buffer
request_enqueue
	
	movf	num_queued_events, w	; Make sure we have room in the buffer
	xorlw	EVENT_BUFFER_SIZE
	pagesel	buffer_space_available
	btfss	STATUS, Z
	goto	buffer_space_available
	
	pagesel	ServiceTimer			; No room? Wait until there is
	call	ServiceTimer
	pagesel	request_enqueue
	goto	request_enqueue

buffer_space_available
	banksel	BufferData
	bankisel x_buffer
	movlw	x_buffer
	addwf	buffer_head, w
	movwf	FSR
	movf	BufferData+wValue, w
	movwf 	INDF

	movlw	y_buffer
	addwf	buffer_head, w
	movwf	FSR
	movf	BufferData+(wValue+1), w
	movwf 	INDF

	bankisel duration_buffer
	movlw	duration_buffer
	addwf	buffer_head, w
	movwf	FSR
	movf	BufferData+wIndex, w
	movwf 	INDF

	movlw	misc_buffer
	addwf	buffer_head, w
	movwf	FSR
	movf	BufferData+(wIndex+1), w
	movwf 	INDF

	; Update buffer state
	incf	num_queued_events, f
	incf	buffer_head, f
	movf	buffer_head, w		; Wrap around if necessary
	xorlw	EVENT_BUFFER_SIZE
	btfsc	STATUS, Z
	clrf	buffer_head

	returnEmpty


	; Set the TMR1 adjustment, from wValue
request_setTimebase

	banksel	BufferData
	movf	BufferData+wValue, w
	banksel	tmr1_adj
	movwf 	tmr1_adj

	banksel	BufferData
	movf	BufferData+(wValue+1), w
	banksel	tmr1_adj
	movwf 	tmr1_adj+1

	returnEmpty

	end

;### The End ###
