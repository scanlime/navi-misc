	;;
	;; Gamecube to N64 converter: use your gamecube controllers with an N64 console
	;; Copyright (C) 2004 Micah Dowty <micah@navi.cx>
	;;
	;;   This program is free software; you can redistribute it and/or modify
	;;   it under the terms of the GNU General Public License as published by
	;;   the Free Software Foundation; either version 2 of the License, or
	;;   (at your option) any later version.
	;;
	;; This firmware is designed to run on a PIC16F84A microcontroller
	;; clocked at 20 MHz. The gamecube's bidirectional data line
	;; (with 1k pullup) is on RB1, the N64's bidirectional data line
	;; is on RB0/INT. The PIC16F84 was chosen due to its relatively high
	;; speed and wide availability, but this code should be portable
	;; to other PICs at the same clock speed without much effort.
	;;
	;; This wouldn't have been possible without all the effort already
	;; poured into reverse engineering the N64 and Gamecube by others.
	;; The author found these resources particularly helpful:
	;;
	;;   http://www.int03.co.uk/crema/hardware/gamecube/gcpad1.cpp
	;;   http://www.gc-linux.org/docs/yagcd/chap8.html
	;;   http://instruct1.cit.cornell.edu/courses/ee476/FinalProjects/s2002/jew17/lld.html
	;;

	list	p=16f84a
	#include p16f84a.inc
	errorlevel -302

	__CONFIG   _CP_OFF & _PWRTE_ON & _WDT_OFF & _HS_OSC

	;; Hardware declarations
	#define N64_PIN		PORTB, 0 ; Also acts as an interrupt source
	#define N64_TRIS	TRISB, 0
	#define	GAMECUBE_PIN	PORTB, 1
	#define	GAMECUBE_TRIS	TRISB, 1

	;; Reset and interrupt vectors
	org 0
	goto	startup
	org 4
	goto	startup

	;; Variables
	cblock	0x0C
		temp
		tx_byte_count
		tx_bit_count
		gc_buffer:8
	endc

	;; *****************************************************************************
	;; ******************************************************  Comm Macros *********
	;; *****************************************************************************

	;; Transmit 'w' bytes of data over the indicated pin, starting at IRP:FSR.
	;; The data is modified as it is transmitted, as we don't have enough time
	;; to copy each byte to a temporary location. Returns when done.
tx_buffer macro port, bit
	local	bit_loop
	local	not_last_byte
	local	not_last_bit

	banksel	TRISA		; Stow our count and start looping over bytes
	movwf	tx_byte_count

	movlw	8		; Set the bit count for the first time. Subsequently
	movwf	tx_bit_count	;   it gets set in the spare cycles below.

	;; Each bit always takes 4us, but there are several possible control paths...
	;; Every line has been numbered with the number of microseconds since the
	;; beginning of the current bit. There isn't enough time to use traditional
	;; nested loops for bits and bytes, so we use the time during a bit to prepare
	;; for the next bit. This is implemented in the form of three alternate endings
	;; for the loop.
bit_loop
	rlf	INDF, f		; 3.8us  Rotate the next bit into C
	bcf	port, bit	; 0.0us  The falling edge begins another bit
	goto	$+1		; 0.2us
	nop			; 0.6us
	btfsc	STATUS, C	; 0.8us  Our bit is in the C flag...
	bsf	port, bit	; 1.0us  Rising edge for 1 bits
	decfsz	tx_bit_count, f	; 1.2us  Is this the last bit?
	goto	not_last_bit	; 1.4us  We have an alternate ending for the last bit...

	decfsz	tx_byte_count,f	; 1.6us  Is this the last byte?
	goto	not_last_byte	; 1.8us	 Yet another alternate ending for the last byte...

	goto	$+1		; 2.0us
	goto	$+1		; 2.4us
	btfss	STATUS, C	; 2.8us
	bsf	port, bit	; 3.0us  Rising edge for 0 bits
	goto	$+1		; 3.2us
	goto	$+1		; 3.6us
	bcf	port, bit	; 0.0us  Begin a stop bit
	goto	$+1		; 0.2us
	goto	$+1		; 0.6us
	bsf	port, bit	; 1.0us  Rising edge for 1 bits
	goto	finished	; 1.2us  Done for now. The first instruction after the
				;        macro is at 1.6us after the beginning of the
				;        stop bit, so it ends 2.4us after the macro does.

not_last_byte
	incf	FSR, f		; 2.2us  Point at the next byte
	movlw	8		; 2.4us  Reinitialize the bit counter
	movwf	tx_bit_count	; 2.6us
	btfss	STATUS, C	; 2.8us
	bsf	port, bit	; 3.0us  Rising edge for 0 bits
	nop			; 3.2us
	goto	bit_loop	; 3.4us

not_last_bit
	goto	$+1		; 1.8us
	goto	$+1		; 2.2us
	nop			; 2.6us
	btfss	STATUS, C	; 2.8us
	bsf	port, bit	; 3.0us  Rising edge for 0 bits
	nop			; 3.2us
	goto	bit_loop	; 3.4us  Next bit...

finished
	endm


	;; *****************************************************************************
	;; ******************************************************  Initialization  *****
	;; *****************************************************************************

startup
	;; Initialize I/O ports- all unused pins output low, the two controller
	;; data pins initially tristated (they simulate open-collector outputs).
	banksel	PORTA
	clrf	PORTA
	clrf	PORTB
	banksel	TRISA
	clrf	TRISA
	movlw	0x03
	movwf	TRISB
	banksel	PORTA

loop_foo

	;; 0x400300
	movlw	0x40
	movwf	gc_buffer+0
	movlw	0x03
	movwf	gc_buffer+1
	movlw	0x00
	movwf	gc_buffer+2

	bankisel gc_buffer
	movlw	gc_buffer
	movwf	FSR
	movlw	3
	tx_buffer GAMECUBE_TRIS

	movlw	0xFF
	banksel temp
	movwf	temp
delay_loop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	decfsz	temp, f
	goto	delay_loop

	goto	loop_foo

	end