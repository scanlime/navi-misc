;################################################################################
;
; controller.asm - Initialization, detection, sampling, and force feedback for
;                  multiple Nintendo Gamecube controllers. The low-level protocol
;                  is implemented in n64gc_comm.inc
;
; The Gamecube Hub project
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
#include "../include/gchub_protocol.h"

	errorlevel -302			; Disable register page warnings

	global  controller_init
	global	controller_sample_next
	global	controller_buffer
	global	rumble_bits

;************************************************** Variables

bank0	udata

	; Single-packet result buffer
controller_buffer res 8

	; Controller request and reply buffers
request_buffer	res 4
reply_buffer	res 8

rumble_bits	res 1

	code

;************************************************** Macros

clearBuffer	macro	address, size
	local	clearLoop
	movlw	size
	banksel	parameter_iter
	movwf	parameter_iter
	bankisel address
	movlw	address
	movwf	FSR
	pagesel	clearLoop
clearLoop
	clrf	INDF
	incf	FSR, f
	decfsz	parameter_iter, f
	goto	clearLoop
	endm

;************************************************** Frontend

controller_init
	return

controller_sample_next

	end

;### The End ###
