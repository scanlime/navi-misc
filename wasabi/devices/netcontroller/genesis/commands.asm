;################################################################################
;
; commands.asm - Implementation of our vendor-specific commands
;
; Wasabi Net-controllers Project
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

	errorlevel -226		; suppress gpasm's silly include file warning

#include <p16C745.inc>
#include "usb_defs.inc"
#include "macros.inc"
#include "protocol.inc"
#include "hardware.inc"

	errorlevel -302		; suppress "register not in bank0, check page bits" message

	global	CheckVendor

	extern	USB_dev_req
	extern	BufferData
	extern	wrongstate
	extern	temp
	extern	Send_0Len_pkt
	extern	porta_select

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
	defineRequest GENESIS_CTRL_SET_STATUS,	request_setStatus

	psgoto	wrongstate		; Not a recognized request


;********************************************** Request handlers

request_setStatus
	banksel	BufferData	; Everything we're messing with except TRIS is in bank0

	copybit	STATUS_C1_UP,		PIN_C1_UP, 1
	copybit	STATUS_C1_DOWN,		PIN_C1_DOWN, 1
	copybit	STATUS_C1_LEFT,		PIN_C1_LEFT, 1
	copybit	STATUS_C1_RIGHT,	PIN_C1_RIGHT, 1

	copybit	STATUS_C2_UP,		PIN_C2_UP, 1
	copybit	STATUS_C2_DOWN,		PIN_C2_DOWN, 1
	copybit	STATUS_C2_LEFT,		PIN_C2_LEFT, 1
	copybit	STATUS_C2_RIGHT,	PIN_C2_RIGHT, 1

	;; Copy the multiplexed buttons to porta_select,
	;; they'll actually make it to PORTA next time the
	;; console toggles select.

	copybit	STATUS_C1_A,		porta_select+0, 0, 1
	copybit	STATUS_C1_A,		porta_select+2, 0, 1
	copybit	STATUS_C1_B,		porta_select+1, 0, 1
	copybit	STATUS_C1_B,		porta_select+3, 0, 1

	copybit	STATUS_C1_START,	porta_select+0, 1, 1
	copybit	STATUS_C1_START,	porta_select+2, 1, 1
	copybit	STATUS_C1_C,		porta_select+1, 1, 1
	copybit	STATUS_C1_C,		porta_select+3, 1, 1

	copybit	STATUS_C2_A,		porta_select+0, 3, 1
	copybit	STATUS_C2_A,		porta_select+1, 3, 1
	copybit	STATUS_C2_B,		porta_select+2, 3, 1
	copybit	STATUS_C2_B,		porta_select+3, 3, 1

	copybit	STATUS_C2_START,	porta_select+0, 4, 1
	copybit	STATUS_C2_START,	porta_select+1, 4, 1
	copybit	STATUS_C2_C,		porta_select+2, 4, 1
	copybit	STATUS_C2_C,		porta_select+3, 4, 1

	copybit	STATUS_POWER,		PIN_POWER, 0

	;; Copy RESET the hard way, since it crosses banks over to TRIS
	bsf	PIN_RESET
	pagesel	reset_on
	btfsc	STATUS_RESET
	goto	reset_on
	banksel	TRISA
	bsf	TRIS_RESET
	psgoto	reset_done
reset_on
	banksel	TRISA
	bcf	TRIS_RESET
reset_done

	psgoto	Send_0Len_pkt

	end

;### The End ###
