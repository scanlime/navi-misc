;################################################################################
;
; io.asm - Functions to manipulate general purpose IO pins using pin descriptors
;
; This file is part of the rcpod project. This file contains original code,
; released into the public domain.
;
; Micah Dowty <micah@picogui.org>
;
;###############################################################################

#include <p16C765.inc>

	errorlevel -302		; supress "register not in bank0, check page bits" message

	global	io_pin		; Current pin descriptor

	global	io_Assert	; Assert the pin descriptor passed in io_pin
	global	io_Deassert	; Invert the polarity of io_pin then assert it

bank0	udata
io_pin	res	1

	code

io_Deassert
	banksel	io_pin
	movlw	0x80		; Toggle the active low/high bit
	xorwf	io_pin, f	; and fall through to io_Assert...

io_Assert
	

	end

;### The End ###
