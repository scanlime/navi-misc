;################################################################################
;
; io.asm - Functions to manipulate general purpose IO pins using pin descriptors
;
; The RCPOD project
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

	errorlevel -226		; suppress the crazy include file warnings on gpasm

#include <p16C765.inc>
#include "usb_defs.inc"

	errorlevel -302		; supress "register not in bank0, check page bits" message

	global	io_pin		; Current pin descriptor

	global	io_Assert	; Assert the pin descriptor passed in io_pin
	global	io_Deassert	; Invert the polarity of io_pin then assert it
	global	io_Read		; Test the value of the pin descriptor io_pin, return it in 'w'
	global	io_Init

	;; It's important that these variables are in bank1, this isn't relocatable.
bank1	udata

io_pin	res	1	; Current pin number, supplied by other modules
io_tmp	res	1

LATA	res	1	; Latch registers, holding the current output values of all ports.
LATB	res	1	;    These are used to avoid read-modify-write errors on the GPIOs.
LATC	res	1
LATD	res	1
LATE	res	1

	code

io_Init
	banksel	LATA
	clrf	LATA
	clrf	LATB
	clrf	LATC
	clrf	LATD
	clrf	LATE
	return


io_Deassert
	banksel	io_pin
	movlw	0x80		; Toggle the active low/high bit
	xorwf	io_pin, f	; and fall through to io_Assert...

io_Assert
	banksel	io_pin		; Select a half of the table, load PCLATH
	movlw	high io_assert_table_1
	btfsc	io_pin, 7
	movlw	high io_assert_table_2
	movwf	PCLATH

	rlf	io_pin, w	; Jump to the entry corresponding with io_pin*2
	andlw	0xFE
	movwf	PCL


	; Test the current pin descriptor, return its value in w
io_Read
	banksel	io_pin		; Select a half of the table, load PCLATH
	movlw	high io_read_table_1
	btfsc	io_pin, 6
	movlw	high io_read_table_2
	movwf	PCLATH

	rlf	io_pin, w	; Jump to the entry corresponding with io_pin*4
	movwf	io_tmp
	rlf	io_tmp, w
	andlw	0xFC

	btfss	io_pin, 6	; Without disturbing 'w', switch to bank0 for read_table_1
	bcf	STATUS, RP0

	movwf	PCL


	;; Since pin descriptors are at the heart of many higher-level I/O
	;; operations, we take a rather extreme memory/speed tradeoff here
	;; to keep pin descriptors speedy. We have separate code for each
	;; and every possible pin descriptor, and we just perform a calculated
	;; jump to the proper fragment.
	;;
	;; Pin descriptor assertions are each two instructions- a bsf/bcf
	;; followed by a return. Pin descriptor reads are each 4 instructions-
	;; a btfss, two returns, and a nop. However, only descriptors 0 through
	;; 127 are implemented in the table for reads, since the result is
	;; easily inverted.
	;;
	;; This gives us a total of 4 256-byte pages full of pin descriptor tables.
	;; We reserve all of page 3 for these tables, since our EPROM is really
	;; a lot bigger than it needs to be for the RCPOD ;)

iotables code

assert_bits_8	macro	inst, reg
	inst	reg, 0
	return
	inst	reg, 1
	return
	inst	reg, 2
	return
	inst	reg, 3
	return
	inst	reg, 4
	return
	inst	reg, 5
	return
	inst	reg, 6
	return
	inst	reg, 7
	return
	endm

	;; In our assertion tables that write through a latch, jump
	;; to a common routine afterwards that copies our latches back
	;; onto our port registers
assertj_bits_8	macro	inst, reg, lbl
	inst	reg, 0
	goto	lbl
	inst	reg, 1
	goto	lbl
	inst	reg, 2
	goto	lbl
	inst	reg, 3
	goto	lbl
	inst	reg, 4
	goto	lbl
	inst	reg, 5
	goto	lbl
	inst	reg, 6
	goto	lbl
	inst	reg, 7
	goto	lbl
	endm

assert_nop_8	macro
	return	; 0
	return
	return	; 1
	return
	return	; 2
	return
	return	; 3
	return
	return	; 4
	return
	return	; 5
	return
	return	; 6
	return
	return	; 7
	return
	endm

	;; First half of the assertion table (0x00 to 0x7F)
io_assert_table_1

	assert_nop_8				; port 0
	assertj_bits_8	bcf, LATA, copy_LATA	; port 1
	assertj_bits_8	bcf, LATB, copy_LATB	; port 2
	assertj_bits_8	bcf, LATC, copy_LATC	; port 3
	assertj_bits_8	bcf, LATD, copy_LATD	; port 4
	assertj_bits_8	bcf, LATE, copy_LATE	; port 5
	assert_nop_8				; port 6
	assert_nop_8				; port 7

	assert_nop_8				; port 0
	assert_bits_8	bcf, TRISA		; port 1
	assert_bits_8	bcf, TRISB		; port 2
	assert_bits_8	bcf, TRISC		; port 3
	assert_bits_8	bcf, TRISD		; port 4
	assert_bits_8	bcf, TRISE		; port 5
	assert_nop_8				; port 6
	assert_nop_8				; port 7

	;; Second half of the assertion table (0x80 to 0xFF)
io_assert_table_2

	assert_nop_8				; port 0
	assertj_bits_8	bsf, LATA, copy_LATA	; port 1
	assertj_bits_8	bsf, LATB, copy_LATB	; port 2
	assertj_bits_8	bsf, LATC, copy_LATC	; port 3
	assertj_bits_8	bsf, LATD, copy_LATD	; port 4
	assertj_bits_8	bsf, LATE, copy_LATE	; port 5
	assert_nop_8				; port 6
	assert_nop_8				; port 7

	assert_nop_8				; port 0
	assert_bits_8	bsf, TRISA		; port 1
	assert_bits_8	bsf, TRISB		; port 2
	assert_bits_8	bsf, TRISC		; port 3
	assert_bits_8	bsf, TRISD		; port 4
	assert_bits_8	bsf, TRISE		; port 5
	assert_nop_8				; port 6
	assert_nop_8				; port 7

read_bits_8	macro	reg
	movlw	0
	btfsc	reg, 0
	movlw	1
	goto	finish_read
	movlw	0
	btfsc	reg, 1
	movlw	1
	goto	finish_read
	movlw	0
	btfsc	reg, 2
	movlw	1
	goto	finish_read
	movlw	0
	btfsc	reg, 3
	movlw	1
	goto	finish_read
	movlw	0
	btfsc	reg, 4
	movlw	1
	goto	finish_read
	movlw	0
	btfsc	reg, 5
	movlw	1
	goto	finish_read
	movlw	0
	btfsc	reg, 6
	movlw	1
	goto	finish_read
	movlw	0
	btfsc	reg, 7
	movlw	1
	goto	finish_read
	endm

read_nop_8	macro
	retlw 0	; 0
	retlw 0
	retlw 0
	retlw 0
	retlw 0	; 1
	retlw 0
	retlw 0
	retlw 0
	retlw 0	; 2
	retlw 0
	retlw 0
	retlw 0
	retlw 0	; 3
	retlw 0
	retlw 0
	retlw 0
	retlw 0	; 4
	retlw 0
	retlw 0
	retlw 0
	retlw 0	; 5
	retlw 0
	retlw 0
	retlw 0
	retlw 0	; 6
	retlw 0
	retlw 0
	retlw 0
	retlw 0	; 7
	retlw 0
	retlw 0
	retlw 0
	endm

	;; First half of the read table (0x00 to 0x3F)
io_read_table_1

	read_nop_8		; port 0
	read_bits_8	PORTA	; port 1
	read_bits_8	PORTB	; port 2
	read_bits_8	PORTC	; port 3
	read_bits_8	PORTD	; port 4
	read_bits_8	PORTE	; port 5
	read_nop_8		; port 6
	read_nop_8		; port 7

	;; Second half of the read table (0x40 to 0xFF)
io_read_table_2

	read_nop_8		; port 0
	read_bits_8	TRISA	; port 1
	read_bits_8	TRISB	; port 2
	read_bits_8	TRISC	; port 3
	read_bits_8	TRISD	; port 4
	read_bits_8	TRISE	; port 5
	read_nop_8		; port 6
	read_nop_8		; port 7

	;; After the read tables but in the same page, we
	;; have small routines that copy each latch register back
	;; onto its corresponding PORT register.

copy_latch macro latch_reg, port_reg
	movf	latch_reg, w
	bcf	STATUS, RP0
	movwf	port_reg
	return
	endm

copy_LATA
	copy_latch LATA, PORTA

copy_LATB
	copy_latch LATB, PORTB

copy_LATC
	copy_latch LATC, PORTC

copy_LATD
	copy_latch LATD, PORTD

copy_LATE
	copy_latch LATE, PORTE

	;; A similar bit of common code for valid reads. We must invert
	;; the result if our pin descriptor is active-low
finish_read
	bsf	STATUS, RP0	; If we were in read_table_1, we were in bank 0
	btfss	io_pin, 7
	xorlw	1
	return

	end

;### The End ###
