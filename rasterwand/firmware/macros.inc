;################################################################################
;
; macros.inc - Random utility macros
;
; This file is part of the Raster Wand project.
; Micah Dowty <micah@navi.cx>
;
;###############################################################################

movlf	macro	literal, register
	movlw	literal
	banksel	register
	movwf	register
	endm

;### The End ###
