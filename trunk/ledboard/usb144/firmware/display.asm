;################################################################################
;
; display.asm - Implements the LED matrix driver itself
;
; The LED board project
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

#include <p16C765.inc>
#include "macros.inc"
#include "../include/ledboard_protocol.h"

	errorlevel -302		; supress "register not in bank0, check page bits" message

	global	display_poll
	global	display_init
	global	display_request_flip
	global	display_seq_write_byte
	global	display_seek

	global	back_buffer

	extern	temp
	extern	temp2

bank0	udata

write_pointer	res	1	; The current position in writing to the backbuffer

unbanked udata_shr

display_flags	res	1

#define FLAG_FLIP_REQUEST	display_flags, 0

bank1	udata
front_buffer	res	LEDBOARD_VRAM_SIZE
bank2	udata
back_buffer	res	LEDBOARD_VRAM_SIZE

	code


;*****************************************************************************************
;************************************************************************** Entry Points *
;*****************************************************************************************

display_init
	return


	; Called frequently from the main loop
display_poll
	clrwdt			; This should be the only place we clear the WDT!
	return


	; Request a page flip at our next opportunity
display_request_flip
	bsf	FLAG_FLIP_REQUEST
	banksel	write_pointer			; Reset the write_pointer
	clrf	write_pointer
	return


	; Set our write pointer to the given column number, form 'w'.
display_seek
	banksel	write_pointer
	movwf	write_pointer
	return


	; Sequentially write the byte given in 'w' to the backbuffer.
display_seq_write_byte
	movwf	temp
	banksel	write_pointer
	bankisel back_buffer

	movlw	LEDBOARD_VRAM_SIZE	; Test write_pointer - LEDBOARD_VRAM_SIZE
	subwf	write_pointer, w
	btfsc	STATUS, C
	return				; C=1, B=0, LEDBOARD_VRAM_SIZE <= write_pointer

	movf	write_pointer, w	; Write and increment
	addlw	back_buffer
	movwf	FSR
	movf	temp, w
	movwf	INDF
	incf	write_pointer, f
	return

	end

;### The End ###
