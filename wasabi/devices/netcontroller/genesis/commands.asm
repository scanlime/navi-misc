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

	errorlevel -302		; suppress "register not in bank0, check page bits" message

	global	CheckVendor

	extern	USB_dev_req
	extern	BufferData
	extern	wrongstate
	extern	temp
	extern	Send_0Len_pkt

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

	psgoto	wrongstate		; Not a recognized request


;********************************************** Request handlers

	end

;### The End ###
