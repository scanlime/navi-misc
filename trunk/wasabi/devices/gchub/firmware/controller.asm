;################################################################################
;
; controller.asm - Initialization, detection, sampling, and force feedback for
;                  multiple Nintendo Gamecube controllers. The low-level protocol
;                  is implemented in n64gc_comm.inc
;
; The Gamecube Hub project
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

	errorlevel -226

#include <p16C745.inc>
#include "../include/gchub_protocol.h"
#include "multitasking.inc"

#define N64GC_CLOCK_24
#include "n64gc_comm.inc"

	errorlevel -302			; Disable register page warnings

	global  controller_init
	global	controller_sample_next
	global	controller_buffer
	global	rumble_bits

;************************************************** Variables

	;; Note that the transmitter has to hang around in bank1
	;; since that's where TRISA is, while the receiver uses bank0.
	;; To simplify, all shared iterators are made unbanked.

unbanked	udata_shr

byte_count	res 1
bit_count	res 1
parameter_iter	res 1

bank0	udata

poll_state	res 2

controller_buffer res 8
rumble_bits	res 1

gamecube_buffer	res 8


controller	code

;************************************************** Macros

clearBuffer	macro	address, size
	local	clearLoop
	movlw	size
	banksel	parameter_iter
	movwf	parameter_iter
	bankisel address
	movlw	address
	movwf	FSR
	pagesel	clearLoop
clearLoop
	clrf	INDF
	incf	FSR, f
	decfsz	parameter_iter, f
	goto	clearLoop
	endm

map_byte	macro	gc_src, packet_dest
	movf	gamecube_buffer + gc_src, w
	movwf	controller_buffer + packet_dest
	endm

	;; Poll one controller, fpsleep'ing for each packet it generates
poll_controller	macro	port_number
	local	timeout_handler
	local	done

	banksel	gamecube_buffer
	movlw	0x40			; Put 0x400300 in the gamecube_buffer
	movwf	gamecube_buffer+0
	movlw	0x03
	movwf	gamecube_buffer+1
	movlw	0x00
	movwf	gamecube_buffer+2

	btfsc	rumble_bits, port_number ; Set the low bit of our gamecube command to turn on rumble
	bsf	gamecube_buffer+2, 0

	call	controller_io_reset	; Transmit the gamecube_buffer
	bankisel gamecube_buffer
	movlw	gamecube_buffer
	movwf	FSR
	movlw	3
	banksel	TRISA			; To bank 1
	n64gc_tx_buffer	TRISA, port_number, 0

	movlw	gamecube_buffer		; Receive the controller's 8-byte status packet
	movwf	FSR
	movlw	8
	bcf	STATUS, RP0		; Back to bank 0
	n64gc_rx_buffer	PORTA, port_number, timeout_handler

	;; Send back analog status
	banksel	controller_buffer
	movlw	GCHUB_PACKET_ANALOG | port_number
	movwf	controller_buffer
	map_byte 2, 1
	map_byte 3, 2
	map_byte 4, 3
	map_byte 5, 4
	map_byte 6, 5
	map_byte 7, 6
	fpsleep	poll_state

	;; Then send button status
	banksel	controller_buffer
	movlw	GCHUB_PACKET_BUTTONS | GCHUB_SYNC | port_number
	movwf	controller_buffer
	map_byte 0, 1
	map_byte 1, 2
	fpsleep	poll_state

	;; Send a disconnect packet for this controller if it timed out
	pagesel	done
	goto	done
timeout_handler
	movlw	GCHUB_PACKET_DISCONNECT | GCHUB_SYNC | port_number
	banksel	controller_buffer
	movwf	controller_buffer
	fpsleep	poll_state

done
	endm


;************************************************** Utilities

	;; Reset latch state and pin direction on controllers.
	;; We must do this before transmitting, since read-modify-write
	;; operations on the same I/O port could put the output latches
	;; in the wrong state.
controller_io_reset
	banksel	PORTA
	clrf	PORTA
	banksel TRISA
	movlw	0x0F
	movwf	TRISA
	banksel PORTA
	return


;************************************************** Frontend

controller_init
	banksel	ADCON1		; Configure all pins as digital rather than analog
	movlw	0x07
	movwf	ADCON1

	banksel	PORTA		; All outputs low by default
	clrf	PORTA
	clrf	PORTB
	clrf	PORTC
	banksel TRISA
	movlw	0x0F		; Tristate the four controller ports by default
	movwf	TRISA
	clrf	TRISB		; Everything else is an output
	clrf	TRISC
	banksel PORTA

	clearBuffer	controller_buffer, 8

	;; Start at the beginning of the polling loop
	fpset	poll_state, controller_poll_loop
	return

	;; Every time we need another sample for our interrupt endpoint,
	;; this runs the polling loop until it hits the next 'fpsleep'.
controller_sample_next
	fpgoto	poll_state


;************************************************** Polling loop

controller_poll_loop
	poll_controller	0
	poll_controller	1
	poll_controller	2
	poll_controller	3
	goto	controller_poll_loop

	end

;### The End ###
