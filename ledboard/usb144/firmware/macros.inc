;################################################################################
;
; macros.inc - Random utility macros
;
; This file is part of the LED board project.
; Micah Dowty <micah@navi.cx>
;
;###############################################################################


movlf	macro	literal, register
	movlw	literal
	banksel	register
	movwf	register
	endm

	;; Swap two file registers (in the current bank) with no temp variable
swapff	macro	reg1, reg2
	movf	reg1, w
	xorwf	reg2, f
	movf	reg2, w
	xorwf	reg1, f
	movf	reg1, w
	xorwf	reg2, f
	endm

	;; Store a new value to a 2-byte function pointer
fpset	macro	ptr, lbl
	banksel	ptr
	movlw	high lbl
	movwf	ptr
	movlw	low lbl
	movwf	ptr+1
	endm


	;; Goto a label from a function pointer
fpgoto	macro	ptr
	banksel	ptr
	movf	ptr, w
	movwf	PCLATH
	movf	ptr+1, w
	movwf	PCL
	endm


	;; Call a function stored in a function pointer
fpcall	macro	ptr
	local	sub
	local	finished
	pagesel	sub
	call	sub
	pagesel	finished
	goto	finished
sub
	fpgoto	ptr
finished
	endm


	;; Effectively wait until a function pointer is called,
	;; then continue execution. This works by saving the instruction
	;; after this one in the function pointer then returning,
	;; but it can be used to create state machines that look
	;; an awful lot like threads.
fpsleep	macro	ptr
	local	next
	fpset	ptr, next
	return
next
	endm


	;; Sleep on a function pointer until a bit is in a particular state
fpsleep_until macro ptr, reg, bit, state
	local	loop
loop
	fpsleep	ptr
	banksel	reg
	if state
	btfss	reg, bit
	else
	btfsc	reg, bit
	endif
	goto	loop
	endm


	;; The assembler already has lgoto/lcall mnemonics
	;; that should do exactly this, but they're buggy!
	;; These mnemonics stand for pagesel+goto and pagesel+call
psgoto	macro	lbl
	pagesel	lbl
	goto	lbl
	endm

pscall	macro	lbl
	pagesel	lbl
	call	lbl
	endm


	;; Zero an area of memory starting at the given literal
	;; address and with a constant length. Requires the 'temp'
	;; register to be available and in the current bank.
bzero	macro	ptr, length
	local	loop
	movlw	ptr
	movwf	FSR
	movlw	length
	movwf	temp
	pagesel	loop
loop
	clrf	INDF
	incf	FSR, f
	decfsz	temp, f
	goto	loop
	endm

;### The End ###
