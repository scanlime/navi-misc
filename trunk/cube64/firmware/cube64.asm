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
	;;   http://www.int03.co.uk/crema/hardware/gamecube/gc-control.htm
	;;   http://www.gc-linux.org/docs/yagcd/chap8.html
	;;   http://instruct1.cit.cornell.edu/courses/ee476/FinalProjects/s2002/jew17/lld.html
	;;   http://www.mixdown.ca/n64dev/
	;;
	;; Unfortunately, the above sources still don't tell you everything you
	;; need to know to trick an N64 into thinking you're a controller. The rest
	;; of this information was reverse engineered by the author.
	;;
	;; The serial format is the same as that used by the gamecube: zeroes
	;; are 1us off 3us on, ones are 3us off 1us on. All packets are a multiple
	;; of 8 bits long not including the single '1' bit used as a stop bit.
	;;
	;; The N64 initiates communications by sending a 1-byte command packet. The
	;; following commands have been observed:
	;;
	;;   0x00 :   Asks the controller to identify itself. Returns a 24-bit word.
	;;            An official N64 controller will respond with 0x050002 if it has
	;;            nothing in the expansion slot, 0x050001 if it has a memory or
	;;            rumble pak installed.
	;;
	;;   0x01 :   Returns a 32-bit status word including all button and axis state.
	;;            The format of this status word is documented in the sources above.
	;;
	;;   0x02 :   Read 32 bytes from the controller pak bus. The N64 follows the 0x02
	;;            with two bytes that have an unknown relation to the address on the
	;;            controller bus. The controller returns 32 bytes of data.
	;;
	;;   0x03 :   Write 32 bytes to the controller pak bus. The 0x03 is followed by
	;;            two address bytes using the same encoding as the read command,
	;;            then 32 data bytes. The controller responds with a 1-byte status
	;;            code. 0x1E indicates that no controler pak is attached, 0xE1 seems
	;;            to indicate success.
	;;
	;; Most games work fine if only the 0x00 and 0x01 commands are implemented,
	;; but some games will attempt to use the controller pak read/write commands
	;; and report that no controller is found if they fail. This includes Ocarina of
	;; Time and Majora's Mask.
	;;
	;; The exact meanings of the address and status bytes in the controller pak
	;; reads/writes are still unknown. At startup, the N64 writes 0xFE bytes to
	;; addresses 0x0000 through 0x003F using the command 03 80 01. It then reads back
	;; the same addresses using 02 80 01. The Rumble Pak returns all 0x80 bytes, while
	;; the memory pak returns all 0x00. It's still unknown why the memory pak doesn't
	;; return 0xFE instead. It's possible that some bits in the address word and one
	;; of the still unknown pins on the controller pak connector are used as a chip
	;; select for the RAM.
	;;


	list	p=16f84a
	#include p16f84a.inc
	errorlevel -302

	__CONFIG   _CP_OFF & _PWRTE_OFF & _WDT_ON & _HS_OSC

	;; Hardware declarations
	#define N64_PIN		PORTB, 0
	#define N64_TRIS	TRISB, 0
	#define	GAMECUBE_PIN	PORTB, 1
	#define	GAMECUBE_TRIS	TRISB, 1

	#define DEBUG1_PIN	PORTA, 1
	#define DEBUG2_PIN	PORTA, 2
	#define DEBUG3_PIN	PORTA, 3

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

	;; Put us back in bank 0 with a bit_count of 8, preconditions for rx_buffer
	bcf	STATUS, RP0
	movlw	8
	movwf	bit_count

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

	;; Gamecube buttons
	#define	GC_A		0, 0
	#define	GC_B		0, 1
	#define	GC_X		0, 2
	#define	GC_Y		0, 3
	#define	GC_START	0, 4
	#define	GC_D_LEFT	1, 0
	#define	GC_D_RIGHT	1, 1
	#define	GC_D_DOWN	1, 2
	#define	GC_D_UP		1, 3
	#define	GC_Z		1, 4
	#define	GC_R		1, 5
	#define	GC_L		1, 6

	;; Gamecube axes
	#define GC_JOYSTICK_X	2
	#define GC_JOYSTICK_Y	3
	#define GC_CSTICK_X	4
	#define GC_CSTICK_Y	5
	#define GC_L_ANALOG	6
	#define GC_R_ANALOG	7

	;; N64 buttons
	#define	N64_D_RIGHT	0, 0
	#define	N64_D_LEFT	0, 1
	#define	N64_D_DOWN	0, 2
	#define	N64_D_UP	0, 3
	#define	N64_START	0, 4
	#define	N64_Z		0, 5
	#define	N64_B		0, 6
	#define	N64_A		0, 7
	#define	N64_C_RIGHT	1, 0
	#define	N64_C_LEFT	1, 1
	#define	N64_C_DOWN	1, 2
	#define	N64_C_UP	1, 3
	#define	N64_R		1, 4
	#define	N64_L		1, 5

	;; N64 axes
	#define N64_JOYSTICK_X	2
	#define N64_JOYSTICK_Y	3


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
	;; ******************************************************  Low-level Comm  *******
	;; *******************************************************************************

	;; Transmit 'w' bytes of data over the indicated pin, starting at IRP:FSR.
	;; The data is modified as it is transmitted, as we don't have enough time
	;; to copy each byte to a temporary location. Returns when done. This macro
	;; works with the gamecube or N64, as they both use the same low-level protocol.
tx_buffer macro port, bit, wide_stop_bit
	local	bit_loop
	local	not_last_byte
	local	not_last_bit

	bsf	STATUS, RP0	; Stow our count and start looping over bytes
	movwf	byte_count

	movlw	8		; Set the bit count for the first time. Subsequently
	movwf	bit_count	;   it gets set in the spare cycles below.

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
	decfsz	bit_count, f	; 1.2us  Is this the last bit?
	goto	not_last_bit	; 1.4us  We have an alternate ending for the last bit...

	decfsz	byte_count,f	; 1.6us  Is this the last byte?
	goto	not_last_byte	; 1.8us	 Yet another alternate ending for the last byte...

	goto	$+1		; 2.0us
	goto	$+1		; 2.4us
	btfss	STATUS, C	; 2.8us
	bsf	port, bit	; 3.0us  Rising edge for 0 bits
	goto	$+1		; 3.2us
	goto	$+1		; 3.6us
	bcf	port, bit	; 0.0us  Begin a stop bit
	bcf	STATUS, RP0	; 0.2us  Always leave in bank 0
	nop			; 0.4us
	movlw	8		; 0.6us  Setup for the next receive
	movwf	bit_count	; 0.8us

	if	wide_stop_bit
	goto	$+1		; Use a 2us stop bit rather than 1us, waste 5 cycles
	goto	$+1
	nop
	endif

	bsf	port, bit	; 1.0us  Rising edge for the stop bit
	return			; 1.2us  Done for now. The first instruction after the
				;        macro is at 1.6us after the beginning of the
				;        stop bit, so it ends 2.4us after we return.

not_last_byte
	incf	FSR, f		; 2.2us  Point at the next byte
	movlw	8		; 2.4us  Reinitialize the bit counter
	movwf	bit_count	; 2.6us
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
	endm


	;; Receive 'w' bytes of data over the indicated pin, starting at IRP:FSR.
	;; Again, this macro works with the GC or the N64. This could block indefinitely
	;; waiting for data. If this happens, the watchdog will reset us.
	;;
	;; Since receives often have to be started extremely quickly after a transmit
	;; ends, some of the setup work for this loop occurs inside tx_buffer above,
	;; during otherwise wasted cycles.
rx_buffer macro port, bit, clear_watchdog
	local	bit_loop
	local	not_last_bit
	local	not_last_byte
	local	edge_det_loop

	movwf	byte_count	; Stow our byte count

	;; We poll for the low state signalling the beginning of a bit, time out 2us
	;; (putting us right in the middle of the bit) and then sample. This probably isn't
	;; how the GC/N64 hardware decodes the received data, but it's close enough.
	;;
	;; The overhead from sampling to the beginning of the next poll needs to
	;; be less than 2us (10 cycles)
bit_loop
	btfss	port, bit	; Before looking for the falling edge, make sure the line is 1.
	goto	bit_loop	; This is necessary if the loop below is speedy enough to finish
				; before the end of the low portion of a bit.

edge_det_loop
	if	clear_watchdog
	clrwdt
	endif

	btfsc	port, bit	; 0.0us  Poll for the beginning of the bit, max 0.6us jitter
	goto	edge_det_loop	; 0.2us
	rlf	INDF, f		; 0.4us  Make room for the new bit
	bcf	INDF, 0		; 0.6us  Assume it's 0 to begin with
	decfsz	bit_count, f	; 0.8us  Is this the last bit?
	goto	not_last_bit	; 1.0us  We have an alternate ending for the last bit...

	decfsz	byte_count,f	; 1.2us  Is this the last byte?
	goto	not_last_byte	; 1.4us	 Yet another alternate ending for the last byte...

	movlw	8		; 1.6us  Reset bit count
	movwf	bit_count	; 1.8us
	btfsc	port, bit	; 2.0us  Sample the incoming bit
	bsf	INDF, 0		; 2.2us
	incf	FSR, f		; 2.4us  Leave FSR pointing after the end of our buffer, for further receiving
	return

not_last_byte
	movlw	8		; 1.8us
	btfsc	port, bit	; 2.0us  Sample the incoming bit
	bsf	INDF, 0		; 2.2us
	incf	FSR, f		; 2.4us  Next byte...
	movwf	bit_count	; 2.6us  Reset bit count
	goto	bit_loop	; 2.8us

not_last_bit
	nop			; 1.4us
	goto	$+1		; 1.6us
	btfsc	port, bit	; 2.0us  Sample the incoming bit
	bsf	INDF, 0		; 2.2us
	goto	bit_loop	; 2.4us
	endm


	;; Implementations of the above macros for particular devices.
	;;
	;; Before transmitting, we explicitly force the output latch low- it may have
	;; been left high by a read-modify-write operation elsewhere.
gamecube_tx
	bcf	GAMECUBE_PIN
	tx_buffer GAMECUBE_TRIS, 0
gamecube_rx
	rx_buffer GAMECUBE_PIN, 0

n64_tx
	bcf	N64_PIN
	tx_buffer N64_TRIS, 0
n64_tx_widestop
	bcf	N64_PIN
	tx_buffer N64_TRIS, 1
n64_rx
	rx_buffer N64_PIN, 0
n64_rx_command
	rx_buffer N64_PIN, 1		; Clear the watchdog while waiting for commands


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
	movlw	0x03
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
	btfsc	STATUS, Z
	goto	n64_send_id		; 0x00 sends a controller identification packet
	xorlw	0x01
	btfsc	STATUS, Z
	goto	n64_send_status		; 0x01 sends a status packet
	movf	n64_command, w
	xorlw	0x02
	btfsc	STATUS, Z
	goto	n64_bus_read		; 0x02 is a read from the controller pak bus
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

	end
