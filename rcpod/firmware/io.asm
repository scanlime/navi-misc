;################################################################################
;
; io.asm - Functions to manipulate general purpose IO pins by number. This mapping
;          uses pin numbers for the PIC16C765 in a 40-pin DIP package.
;
; This file is part of the rcpod project. This file contains original code,
; released into the public domain.
;
; Micah Dowty <micah@picogui.org>
;
;###############################################################################

#include <p16C765.inc>

	errorlevel -302		; supress "register not in bank0, check page bits" message

	global	io_pin

	global	io_Output	; Set the pin indicated by io_pin as an output
	global	io_Input	; Set the pin indicated by io_pin as an input
	global	io_High		; Set the indicated pin as an output and pull it high
	global	io_Low		; Set the indicated pin as an output and pull it low
	global	io_Read		; Set the indicated pin as an input and return its value in w

bank0	udata
io_pin	res	1

	code

	;; ------------------------ Macros

mapPin	macro	pin, op, port, bit
	local	next
	movlw	pin
	xorwf	io_pin, w
	pagesel	next
	btfss	STATUS, Z
	goto	next
	banksel	port
	op	port, bit
	return
next
	endm

readPin	macro	pin, port, bit
	local	next
	movlw	pin
	xorwf	io_pin, w
	pagesel	next
	btfss	STATUS, Z
	goto	next
	banksel	port
	btfss	port, bit
	retlw	0
	retlw	1
	return
next
	endm

	;; ------------------------ Functions

io_Output
	banksel	io_pin
	mapPin	.2,	bcf,	TRISA,	0
	mapPin	.3,	bcf,	TRISA,	1
	mapPin	.4,	bcf,	TRISA,	2
	mapPin	.5,	bcf,	TRISA,	3
	mapPin	.6,	bcf,	TRISA,	4
	mapPin	.7,	bcf,	TRISA,	5
	mapPin	.8,	bcf,	TRISE,	0
	mapPin	.9,	bcf,	TRISE,	1
	mapPin	.10,	bcf,	TRISE,	2
	mapPin	.15,	bcf,	TRISC,	0
	mapPin	.16,	bcf,	TRISC,	1
	mapPin	.17,	bcf,	TRISC,	2
	mapPin	.19,	bcf,	TRISD,	0
	mapPin	.20,	bcf,	TRISD,	1
	mapPin	.21,	bcf,	TRISD,	2
	mapPin	.22,	bcf,	TRISD,	3
	mapPin	.25,	bcf,	TRISC,	6
	mapPin	.26,	bcf,	TRISC,	7
	mapPin	.27,	bcf,	TRISD,	4
	mapPin	.28,	bcf,	TRISD,	5
	mapPin	.29,	bcf,	TRISD,	6
	mapPin	.30,	bcf,	TRISD,	7
	mapPin	.33,	bcf,	TRISB,	0
	mapPin	.34,	bcf,	TRISB,	1
	mapPin	.35,	bcf,	TRISB,	2
	mapPin	.36,	bcf,	TRISB,	3
	mapPin	.37,	bcf,	TRISB,	4
	mapPin	.38,	bcf,	TRISB,	5
	mapPin	.39,	bcf,	TRISB,	6
	mapPin	.40,	bcf,	TRISB,	7
	return

io_Input
	banksel	io_pin
	mapPin	.2,	bsf,	TRISA,	0
	mapPin	.3,	bsf,	TRISA,	1
	mapPin	.4,	bsf,	TRISA,	2
	mapPin	.5,	bsf,	TRISA,	3
	mapPin	.6,	bsf,	TRISA,	4
	mapPin	.7,	bsf,	TRISA,	5
	mapPin	.8,	bsf,	TRISE,	0
	mapPin	.9,	bsf,	TRISE,	1
	mapPin	.10,	bsf,	TRISE,	2
	mapPin	.15,	bsf,	TRISC,	0
	mapPin	.16,	bsf,	TRISC,	1
	mapPin	.17,	bsf,	TRISC,	2
	mapPin	.19,	bsf,	TRISD,	0
	mapPin	.20,	bsf,	TRISD,	1
	mapPin	.21,	bsf,	TRISD,	2
	mapPin	.22,	bsf,	TRISD,	3
	mapPin	.25,	bsf,	TRISC,	6
	mapPin	.26,	bsf,	TRISC,	7
	mapPin	.27,	bsf,	TRISD,	4
	mapPin	.28,	bsf,	TRISD,	5
	mapPin	.29,	bsf,	TRISD,	6
	mapPin	.30,	bsf,	TRISD,	7
	mapPin	.33,	bsf,	TRISB,	0
	mapPin	.34,	bsf,	TRISB,	1
	mapPin	.35,	bsf,	TRISB,	2
	mapPin	.36,	bsf,	TRISB,	3
	mapPin	.37,	bsf,	TRISB,	4
	mapPin	.38,	bsf,	TRISB,	5
	mapPin	.39,	bsf,	TRISB,	6
	mapPin	.40,	bsf,	TRISB,	7
	return

io_High
	pagesel	io_Output
	call	io_Output
	banksel	io_pin
	mapPin	.2,	bsf,	PORTA,	0
	mapPin	.3,	bsf,	PORTA,	1
	mapPin	.4,	bsf,	PORTA,	2
	mapPin	.5,	bsf,	PORTA,	3
	mapPin	.6,	bsf,	PORTA,	4
	mapPin	.7,	bsf,	PORTA,	5
	mapPin	.8,	bsf,	PORTE,	0
	mapPin	.9,	bsf,	PORTE,	1
	mapPin	.10,	bsf,	PORTE,	2
	mapPin	.15,	bsf,	PORTC,	0
	mapPin	.16,	bsf,	PORTC,	1
	mapPin	.17,	bsf,	PORTC,	2
	mapPin	.19,	bsf,	PORTD,	0
	mapPin	.20,	bsf,	PORTD,	1
	mapPin	.21,	bsf,	PORTD,	2
	mapPin	.22,	bsf,	PORTD,	3
	mapPin	.25,	bsf,	PORTC,	6
	mapPin	.26,	bsf,	PORTC,	7
	mapPin	.27,	bsf,	PORTD,	4
	mapPin	.28,	bsf,	PORTD,	5
	mapPin	.29,	bsf,	PORTD,	6
	mapPin	.30,	bsf,	PORTD,	7
	mapPin	.33,	bsf,	PORTB,	0
	mapPin	.34,	bsf,	PORTB,	1
	mapPin	.35,	bsf,	PORTB,	2
	mapPin	.36,	bsf,	PORTB,	3
	mapPin	.37,	bsf,	PORTB,	4
	mapPin	.38,	bsf,	PORTB,	5
	mapPin	.39,	bsf,	PORTB,	6
	mapPin	.40,	bsf,	PORTB,	7
	return

io_Low
	pagesel	io_Output
	call	io_Output
	banksel	io_pin
	mapPin	.2,	bcf,	PORTA,	0
	mapPin	.3,	bcf,	PORTA,	1
	mapPin	.4,	bcf,	PORTA,	2
	mapPin	.5,	bcf,	PORTA,	3
	mapPin	.6,	bcf,	PORTA,	4
	mapPin	.7,	bcf,	PORTA,	5
	mapPin	.8,	bcf,	PORTE,	0
	mapPin	.9,	bcf,	PORTE,	1
	mapPin	.10,	bcf,	PORTE,	2
	mapPin	.15,	bcf,	PORTC,	0
	mapPin	.16,	bcf,	PORTC,	1
	mapPin	.17,	bcf,	PORTC,	2
	mapPin	.19,	bcf,	PORTD,	0
	mapPin	.20,	bcf,	PORTD,	1
	mapPin	.21,	bcf,	PORTD,	2
	mapPin	.22,	bcf,	PORTD,	3
	mapPin	.25,	bcf,	PORTC,	6
	mapPin	.26,	bcf,	PORTC,	7
	mapPin	.27,	bcf,	PORTD,	4
	mapPin	.28,	bcf,	PORTD,	5
	mapPin	.29,	bcf,	PORTD,	6
	mapPin	.30,	bcf,	PORTD,	7
	mapPin	.33,	bcf,	PORTB,	0
	mapPin	.34,	bcf,	PORTB,	1
	mapPin	.35,	bcf,	PORTB,	2
	mapPin	.36,	bcf,	PORTB,	3
	mapPin	.37,	bcf,	PORTB,	4
	mapPin	.38,	bcf,	PORTB,	5
	mapPin	.39,	bcf,	PORTB,	6
	mapPin	.40,	bcf,	PORTB,	7
	return

io_Read
	pagesel	io_Input
	call	io_Input
	banksel	io_pin
	readPin	.2,	PORTA,	0
	readPin	.3,	PORTA,	1
	readPin	.4,	PORTA,	2
	readPin	.5,	PORTA,	3
	readPin	.6,	PORTA,	4
	readPin	.7,	PORTA,	5
	readPin	.8,	PORTE,	0
	readPin	.9,	PORTE,	1
	readPin	.10,	PORTE,	2
	readPin	.15,	PORTC,	0
	readPin	.16,	PORTC,	1
	readPin	.17,	PORTC,	2
	readPin	.19,	PORTD,	0
	readPin	.20,	PORTD,	1
	readPin	.21,	PORTD,	2
	readPin	.22,	PORTD,	3
	readPin	.25,	PORTC,	6
	readPin	.26,	PORTC,	7
	readPin	.27,	PORTD,	4
	readPin	.28,	PORTD,	5
	readPin	.29,	PORTD,	6
	readPin	.30,	PORTD,	7
	readPin	.33,	PORTB,	0
	readPin	.34,	PORTB,	1
	readPin	.35,	PORTB,	2
	readPin	.36,	PORTB,	3
	readPin	.37,	PORTB,	4
	readPin	.38,	PORTB,	5
	readPin	.39,	PORTB,	6
	readPin	.40,	PORTB,	7
	return

	end

;### The End ###
