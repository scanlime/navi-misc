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

	; If reg1 < reg2, jump to label (16-bit Little Endian)
jl_16	macro	reg1, reg2, label
	local 	no_jump

	movf	reg2+1, w			; Test high byte of reg1 - reg2
	subwf	reg1+1, w
	pagesel	label
	btfss	STATUS, C
	goto	label				; C=0, B=1, reg1 < reg2

	pagesel	no_jump
	btfss	STATUS, Z
	goto	no_jump				; reg1 != reg2 (implies reg1 > reg2)

	movf	reg2, w				; Test low byte of reg1 - reg2
	subwf	reg1, w
	pagesel	label
	btfss	STATUS, C
	goto	label				; C=0, B=1, reg1 < reg2
no_jump
	endm

;### The End ###
