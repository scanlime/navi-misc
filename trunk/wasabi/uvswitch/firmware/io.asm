;################################################################################
;
; io.asm - Functions to manipulate the uvswitch's hardware
;
; This file is part of the uvswitch project. This file contains original code,
; released into the public domain.
;
; Micah Dowty <micah@picogui.org>
;
;###############################################################################

#include <p16C765.inc>

	errorlevel -302		; supress "register not in bank0, check page bits" message

bank0	udata
io_pin	res	1

	code

	;; ------------------------ Functions

	end

;### The End ###
