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
	;; See n64gc_comm.inc for code and documentation related to the protocol
	;; used between here, the N64, and the Gamecube.
	;;


	list	p=16f84a
	#include p16f84a.inc
	#include n64gc_comm.inc
	errorlevel -302

	__CONFIG   _CP_OFF & _PWRTE_OFF & _WDT_ON & _HS_OSC

	;; Hardware declarations
	#define N64_PIN		PORTB, 0
	#define N64_TRIS	TRISB, 0
	#define	GAMECUBE_PIN	PORTB, 1
	#define	GAMECUBE_TRIS	TRISB, 1

	;; Reset and interrupt vectors
	org 0
	goto	startup
	org 4
	retfie

	;; Variables
	cblock	0x0C
		byte_count
		bit_count
		temp
		gamecube_buffer:8
		n64_status_buffer:4
		n64_id_buffer:3
		n64_command
		n64_bus_buffer:34	; Must immediately follow n64_command
	endc

	;; *******************************************************************************
	;; ******************************************************  Initialization  *******
	;; *******************************************************************************

startup
	;; Initialize I/O ports- all unused pins output low, the two controller
	;; data pins initially tristated (they simulate open-collector outputs).
	bcf	STATUS, RP0
	clrf	PORTA
	clrf	PORTB
	bsf	STATUS, RP0
	clrf	TRISA
	movlw	0x03
	movwf	TRISB

	n64gc_init

	;; Initialize the ID buffer with values measured from an official nintendo
	;; controller with no rumble pak or memory pak attached. The page at
	;; http://www.mixdown.ca/n64dev/ mentioned the existence of this ID command,
	;; but I obtained this value myself. In the future this might be changed to
	;; support the N64 rumble pak via the gamecube controller's built-in motor.
	movlw	0x05
	movwf	n64_id_buffer+0
	movlw	0x00
	movwf	n64_id_buffer+1
	movlw	0x02
	movwf	n64_id_buffer+2

	;; We use the watchdog to implicitly implement controller probing.
	;; If we have no gamecube controller attached, gamecube_poll_status
	;; will never return. We therefore never get around to answering the
	;; N64's requests. This breaks down if we start up with the gamecube
	;; controller plugged in but it's unplugged while the N64 is idle, but
	;; the N64 should be polling us pretty constantly.
	;;
	;; The watchdog timeout therefore sets how often we retry in our search
	;; for a responding gamecube controller. The default is a little over
	;; 2 seconds. This changes the prescaler to 1:16, giving us a nominal
	;; watchdog timeout of 288ms.
	clrwdt
	movlw	0xFC
	bsf	STATUS, RP0
	movwf	OPTION_REG
	bcf	STATUS, RP0

main_loop
	call	gamecube_poll_status	; The gamecube poll takes place during the dead period
	call	n64_translate_status	;   between incoming N64 commands, hopefully
	call	n64_wait_for_command
	goto	main_loop


	;; *******************************************************************************
	;; ******************************************************  Button/Axis mapping ***
	;; *******************************************************************************

	;; Map one button to another button
map_button macro src_byte, src_bit, dest_byte, dest_bit
	btfsc	gamecube_buffer+src_byte, src_bit
	bsf	n64_status_buffer+dest_byte, dest_bit
	endm

	;; Map an 8-bit 0x80-centered axis to an 8-bit signed axis
map_axis macro src_byte, dest_byte
	movlw	0x80
	subwf	gamecube_buffer+src_byte, w
	movwf	n64_status_buffer+dest_byte
	endm

	;; Map an 8-bit 0x80-centered axis to two buttons, given lower and upper thresholds
map_button_axis macro axis_byte, lower_byte, lower_bit, upper_byte, upper_bit, lower_thresh, upper_thresh
	movlw	lower_thresh
	subwf	gamecube_buffer+axis_byte, w			; Axis - lower_thresh
	btfss	STATUS, C
	bsf	n64_status_buffer+lower_byte, lower_bit		; C=0, B=1, lower_thresh > axis
	movlw	upper_thresh+1
	subwf	gamecube_buffer+axis_byte, w			; Axis - (upper_thresh+1)
	btfsc	STATUS, C
	bsf	n64_status_buffer+upper_byte, upper_bit		; C=1, B=0, (upper_thresh+1) <= axis
	endm


map_start macro
	clrf	n64_status_buffer+0	; Start out with everything zeroed...
	clrf	n64_status_buffer+1
	clrf	n64_status_buffer+2
	clrf	n64_status_buffer+3
	endm

	;; Copy status from the gamecube buffer to the N64 buffer, The mapping is defined
	;; as a normal subroutine using the above macros.
n64_translate_status
	map_start
	map_button	GC_A,		N64_A		; Shared buttons
	map_button	GC_B,		N64_B
	map_button	GC_Z,		N64_Z
	map_button	GC_R,		N64_R
	map_button	GC_L,		N64_L
	map_button	GC_START,	N64_START

	map_button	GC_X,		N64_C_DOWN	; Intuitive-ish mappings for the new buttons
	map_button	GC_Y,		N64_B

	map_button	GC_D_LEFT,	N64_D_LEFT	; D-Pad
	map_button	GC_D_RIGHT,	N64_D_RIGHT
	map_button	GC_D_UP,	N64_D_UP
	map_button	GC_D_DOWN,	N64_D_DOWN

	map_axis	GC_JOYSTICK_X,	N64_JOYSTICK_X	; Joystick
	map_axis	GC_JOYSTICK_Y,	N64_JOYSTICK_Y

	map_button_axis	GC_CSTICK_X,    N64_C_LEFT,	N64_C_RIGHT,	0x60, 0xA0 ; C-Button emulation
	map_button_axis	GC_CSTICK_Y,    N64_C_DOWN,	N64_C_UP,	0x60, 0xA0
	return


	;; *******************************************************************************
	;; ******************************************************  N64 Interface *********
	;; *******************************************************************************

	;; Service commands coming in from the N64
n64_wait_for_command
	call	n64_wait_for_idle	; Ensure the line is idle first, so we don't jump in to the middle of a command

	movlw	n64_command		; Receive 1 command byte
	movwf	FSR
	movlw	1
	call	n64_rx_command

	;; We need to handle controller pak writes very fast because there's no pause
	;; between the initial command byte and the 34 bytes following. Every
	;; extra instuction here decreases the sampling quality in the receive,
	;; so we include our own copy of the receive macro. This also lets us disable
	;; watchdog clearing, so we won't get stalled in here if the N64 gives up.
	movlw	N64_COMMAND_WRITE_BUS
	xorwf	n64_command, w
	btfss	STATUS, Z
	goto	not_bus_write
	movlw	.34
	call	n64_rx			; FSR is already pointing at our buffer, it's after n64_command
	;; Fall through...

n64_bus_write
	movlw	0x1E			; Always indicate that we have no controller pak
	movwf	n64_command
	movlw	n64_command
	movwf	FSR
	movlw	1
	goto	n64_tx_widestop		; I don't know why, but these replies use a funky stop bit

n64_bus_read				; Not implemented yet...
	goto	n64_wait_for_command

n64_send_status
	movlw	n64_status_buffer	; Transmit the status buffer
	movwf	FSR
	movlw	4
	goto	n64_tx

n64_send_id
	movlw	n64_id_buffer		; Transmit the ID buffer
	movwf	FSR
	movlw	3
	goto	n64_tx

not_bus_write
	movf	n64_command, w
	xorlw	N64_COMMAND_IDENTIFY
	btfsc	STATUS, Z
	goto	n64_send_id

	movf	n64_command, w
	xorlw	N64_COMMAND_STATUS
	btfsc	STATUS, Z
	goto	n64_send_status

	movf	n64_command, w
	xorlw	N64_COMMAND_READ_BUS
	btfsc	STATUS, Z
	goto	n64_bus_read

	goto	n64_wait_for_command	; Ignore other commands


	;; Don't return until the N64 data line has been idle long enough to ensure
	;; we aren't in the middle of a packet already.
n64_wait_for_idle
	movlw	0x10
	movwf	temp
keep_waiting_for_idle
	btfss	N64_PIN
	goto	n64_wait_for_idle
	decfsz	temp, f
	goto	keep_waiting_for_idle
	return

	;; Before transmitting, we explicitly force the output latch low- it may have
	;; been left high by a read-modify-write operation elsewhere.
n64_tx
	bcf	N64_PIN
	n64gc_tx_buffer N64_TRIS, 0
n64_tx_widestop
	bcf	N64_PIN
	n64gc_tx_buffer N64_TRIS, 1
n64_rx
	n64gc_rx_buffer N64_PIN, 0
n64_rx_command
	n64gc_rx_buffer N64_PIN, 1		; Clear the watchdog while waiting for commands


	;; *******************************************************************************
	;; ******************************************************  Gamecube Interface  ***
	;; *******************************************************************************

	;; Poll the gamecube controller's state by transmitting a magical
	;; poll command (0x400300) then receiving 8 bytes of status.
gamecube_poll_status
	movlw	0x40			; Put 0x400300 in the gamecube_buffer
	movwf	gamecube_buffer+0
	movlw	0x03
	movwf	gamecube_buffer+1
	movlw	0x00
	movwf	gamecube_buffer+2

	movlw	gamecube_buffer		; Transmit the gamecube_buffer
	movwf	FSR
	movlw	3
	call	gamecube_tx

	movlw	gamecube_buffer		; Receive 8 status bytes
	movwf	FSR
	movlw	8
	call	gamecube_rx
	return


gamecube_tx
	bcf	GAMECUBE_PIN
	n64gc_tx_buffer GAMECUBE_TRIS, 0
gamecube_rx
	n64gc_rx_buffer GAMECUBE_PIN, 0

	end
