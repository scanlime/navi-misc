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

bank0	udata

poll_state	res 2

controller_buffer res 8
rumble_bits	res 1

gamecube_buffer	res 8

byte_count	res 1
bit_count	res 1
parameter_iter	res 1

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


	;; Poll one controller, fpsleep'ing for each packet it generates
poll_controller	macro	port_number
	local	timeout
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
	movlw	gamecube_buffer
	movwf	FSR
	movlw	3
	n64gc_tx_buffer	TRISA, port_number, 0

	movlw	gamecube_buffer		; Receive the controller's 8-byte status packet
	movwf	FSR
	movlw	8
	n64gc_rx_buffer	PORTA, port_number, timeout

	;; Send back analog status
	movlw	GCHUB_PACKET_ANALOG | port_number
	movwf	controller_buffer
	fpsleep	poll_state

	;; Send a timeout packet for this controller
	pagesel	done
	goto	done
timeout
	movlw	GCHUB_PACKET_DISCONNECT | port_number
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
