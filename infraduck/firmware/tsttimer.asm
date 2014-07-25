;
; Timer program to assist in testing the transmitter-
; Pulse the GP5 pin low at 10hz.
;

	list	p=12c509a
	#include p12c509a.inc


;----------------------------------------------------- Per-device configuration

;--- Values for transmitter/receiver test breadboard
oscCalValue	equ	054


;----------------------------------------------------- Constants

; I/O
tristateMask	equ	b'00000000'
outputBit	equ	5


;----------------------------------------------------- Variables

	cblock 7
	  delayTemp1, delayTemp2
	endc


;----------------------------------------------------- Main program
	org 0

	; Initialize oscillator (must come first)
	ifdef oscCalValue
	movlw	oscCalValue
	endif
	movwf	OSCCAL
	
	; Initialize OPTION register:

	; Initialize I/O
	clrf	GPIO
	movlw	tristateMask
	tris	GPIO
	bsf	GPIO, outputBit

mainLoop

	; Wait about 100ms
	movlw	080
	movwf	delayTemp1
delayLoop1
	movlw	0FF
	movwf	delayTemp2
delayLoop2
	decfsz	delayTemp2, f
	goto	delayLoop2
	decfsz	delayTemp1, f
	goto	delayLoop1

	; Pulse the output low
	bcf	GPIO, outputBit
	nop
	nop
	bsf	GPIO, outputBit

	goto	mainLoop	
	end


