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
	;; This file doesn't implement all of the N64 controller protocol, but
	;; it should correctly emulate an official N64 controller with rumble pak.
	;; It might not respond to unusual circumstances the same as a real N64
	;; controller would, both due to potential gaps in the reverse engineering,
	;; and due to corners cut in the algorithm implementation to fit it on
	;; this microcontroller. In particular, only a partial implementation of
	;; the CRC algorithm is used and the address check bits are ignored.
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
		bus_byte_count
		flags
		virtual_button

		;; These four items must be contiguous.
		;; Note that n64_command and n64_bus_address point to the same memory.
		;; This is explained in the bus write receive code near n64_wait_for_command.
		n64_command:0
		n64_bus_address:2
		n64_bus_packet:.32
		n64_crc

		gamecube_buffer:8
		n64_status_buffer:4
		n64_id_buffer:3
	endc

	#define	FLAG_RUMBLE_MOTOR_ON	flags, 0


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

	clrf	flags

	movlw	.34			; Reset bus_byte_count to 34. Keeping this set beforehand
	movwf	bus_byte_count		;   saves a few precious cycles in receiving bus writes.

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

	;; Button IDs. These are a superset of the gamecube and N64, without any correspondance
	;; with the wire protocol used by either. They're used as intermediate values when
	;; translating from gamecube to N64. These IDs are used as indices into the remapping
	;; table stored in our on-chip EEPROM.

	#define	BTN_A		0x00
	#define	BTN_B		0x01
	#define	BTN_X		0x02
	#define	BTN_Y		0x03
	#define	BTN_Z		0x04
	#define	BTN_R		0x05
	#define	BTN_L		0x06
	#define	BTN_START	0x07
	#define	BTN_D_LEFT	0x08
	#define	BTN_D_RIGHT	0x09
	#define	BTN_D_DOWN	0x0A
	#define	BTN_D_UP	0x0B
	#define	BTN_C_LEFT	0x0C
	#define	BTN_C_RIGHT	0x0D
	#define	BTN_C_DOWN	0x0E
	#define	BTN_C_UP	0x0F

	;; Map a gamecube button to a virtual button, and eventually to an N64 button
map_button_from macro src_byte, src_bit, virtual
	movlw	virtual
	btfsc	gamecube_buffer+src_byte, src_bit
	call	set_virtual_button
	endm

	;; Map a virtual button to an N64 button. If the indicated button is the one
	;; currently in virtual_button, sets the corresponding N64 bit and returns.
map_button_to	macro virtual, dest_byte, dest_bit
	local	next
	movf	virtual_button, w
	xorlw	virtual
	btfss	STATUS, Z
	goto	next
	bsf	n64_status_buffer+dest_byte, dest_bit
	return
next
	endm

	;; Map an 8-bit 0x80-centered axis to an 8-bit signed axis.
	;; Doesn't allow any remapping.
map_axis macro src_byte, dest_byte
	movlw	0x80
	subwf	gamecube_buffer+src_byte, w
	movwf	n64_status_buffer+dest_byte
	endm

	;; Map an 8-bit 0x80-centered axis to two virtual buttons,
	;; given lower and upper thresholds.
map_button_axis macro axis_byte, lower_virtual, upper_virtual, lower_thresh, upper_thresh
	movlw	lower_thresh
	subwf	gamecube_buffer+axis_byte, w		; Axis - lower_thresh
	movlw	lower_virtual
	btfss	STATUS, C
	call	set_virtual_button			; C=0, B=1, lower_thresh > axis
	movlw	upper_thresh+1
	subwf	gamecube_buffer+axis_byte, w		; Axis - (upper_thresh+1)
	movlw	upper_virtual
	btfsc	STATUS, C
	call	set_virtual_button			; C=1, B=0, (upper_thresh+1) <= axis
	endm


map_start macro
	clrf	n64_status_buffer+0	; Start out with everything zeroed...
	clrf	n64_status_buffer+1
	clrf	n64_status_buffer+2
	clrf	n64_status_buffer+3
	endm

	;; Copy status from the gamecube buffer to the N64 buffer. This first
	;; stage maps all axes, and maps gamecube buttons to virtual buttons.
n64_translate_status
	map_start
	map_button_from	GC_A,		BTN_A
	map_button_from	GC_B,		BTN_B
	map_button_from	GC_Z,		BTN_Z
	map_button_from	GC_R,		BTN_R
	map_button_from	GC_L,		BTN_L
	map_button_from	GC_START,	BTN_START

	map_button_from	GC_X,		BTN_X
	map_button_from	GC_Y,		BTN_Y

	map_button_from	GC_D_LEFT,	BTN_D_LEFT
	map_button_from	GC_D_RIGHT,	BTN_D_RIGHT
	map_button_from	GC_D_UP,	BTN_D_UP
	map_button_from	GC_D_DOWN,	BTN_D_DOWN

	map_axis	GC_JOYSTICK_X,	N64_JOYSTICK_X
	map_axis	GC_JOYSTICK_Y,	N64_JOYSTICK_Y

	map_button_axis	GC_CSTICK_X,    BTN_C_LEFT,	BTN_C_RIGHT,	0x50, 0xB0
	map_button_axis	GC_CSTICK_Y,    BTN_C_DOWN,	BTN_C_UP,	0x50, 0xB0
	return


	;; This is called for each virtual button set. It translates the virtual
	;; button through our remapping table in EEPROM, then maps that into an N64 button.
set_virtual_button
	movwf	virtual_button

	map_button_to	BTN_D_RIGHT,	N64_D_RIGHT
	map_button_to	BTN_D_LEFT,	N64_D_LEFT
	map_button_to	BTN_D_DOWN,	N64_D_DOWN
	map_button_to	BTN_D_UP,	N64_D_UP

	map_button_to	BTN_X,		N64_C_DOWN
	map_button_to	BTN_Y,		N64_B

	map_button_to	BTN_START,	N64_START
	map_button_to	BTN_Z,		N64_Z
	map_button_to	BTN_B,		N64_B
	map_button_to	BTN_A,		N64_A
	map_button_to	BTN_R,		N64_R
	map_button_to	BTN_L,		N64_L

	map_button_to	BTN_C_RIGHT,	N64_C_RIGHT
	map_button_to	BTN_C_LEFT,	N64_C_LEFT
	map_button_to	BTN_C_DOWN,	N64_C_DOWN
	map_button_to	BTN_C_UP,	N64_C_UP
	return


	;; *******************************************************************************
	;; ******************************************************  N64 Interface *********
	;; *******************************************************************************

	;; Service commands coming in from the N64
n64_wait_for_command
	call	n64_wait_for_idle	; Ensure the line is idle first, so we don't jump in to the middle of a command

	movlw	n64_command		; Receive 1 command byte
	movwf	FSR
	call	n64_rx_command

	;; We need to handle controller pak writes very fast because there's no pause
	;; between the initial command byte and the 34 bytes following. Every
	;; extra instuction here increases the probability of missing the first bit.
	;;
	;; FSR is already pointing at the right buffer- n64_rx will leave FSR pointing
	;; at the last byte it read, which is n64_command. We've overlaid n64_command
	;; onto the same memory as our address buffer, and the data buffer is immediately
	;; after our address buffer.
	;;
	;; Since n64_rx must be called as soon as possible, this skips n64_rx if we're not
	;; doing a write_bus command, and after the fact detects that and looks for other commands.
	;;
	movlw	N64_COMMAND_WRITE_BUS
	xorwf	n64_command, w
	btfsc	STATUS, Z
	call	n64_rx_bus		; Do another receive if this was a write_bus command.

	movlw	n64_command		; n64_command itself might be invalid now. If FSR changed,
	xorwf	FSR, w			;   n64_command is invalid and we're doing a bus write.
	btfss	STATUS, Z
	goto	n64_bus_write

	movf	n64_command, w		; Detect other applicable commands...
	xorlw	N64_COMMAND_READ_BUS
	btfsc	STATUS, Z
	goto	n64_bus_read

	movf	n64_command, w
	xorlw	N64_COMMAND_IDENTIFY
	btfsc	STATUS, Z
	goto	n64_send_id

	movf	n64_command, w
	xorlw	N64_COMMAND_STATUS
	btfsc	STATUS, Z
	goto	n64_send_status

	goto	n64_wait_for_command	; Ignore unimplemented commands


	;; The N64 requested a 32-byte write to our controller pak bus.
	;; The start address is given in the high 11 bits of n64_bus_address.
	;; The low 5 bits are to verify the address- the algorithm for this is
	;; known, but to save time they are currently ignored.
	;; Addresses from 0x0000 to 0x7FFF are only used by the memory pak's RAM.
	;; To emulate a rumble pak, we should only need to respond to configuration
	;; writes at 0x8000 and rumble pak motor writes at 0xC000.
	;;
	;; Since all the packets we'll get while emulating a rumble pak are copies
	;; of the same byte, we assume this is always true and use a table storing
	;; the checksums of all such packets. We always negate the checksum to indicate
	;; that a controller pak has been detected and initialized properly.
n64_bus_write
	goto	$+1			; We have about 3us to kill here, we don't
	goto	$+1			;   want to begin transmitting before the stop bit is over.
	goto	$+1
	goto	$+1

	movlw	.34			; Reset bus_byte_count to 34. Keeping this set beforehand
	movwf	bus_byte_count		;   saves a few precious cycles in receiving bus writes.

	call	get_repeated_crc	; Get our limited CRC from the lookup table
	xorlw	0xFF			; Negate the CRC, we emulate a rumble pak
	movwf	n64_crc			; Send back the CRC in a 1-byte transmission
	movlw	n64_crc
	movwf	FSR
	movlw	1
	call	n64_tx_widestop		; We need a 2us stop bit after all CRCs

	movf	n64_bus_address, w	; Is this a write to the rumble pak?
	xorlw	0xC0			; (only check the top 8 bits. This excludes a few address bits and all check bits)
	btfss	STATUS, Z
	return				; Nope, return. We ignore the initialization writes to 0x8000

	bcf	FLAG_RUMBLE_MOTOR_ON	; Set the rumble flag from the low bit of the first data byte
	btfsc	n64_bus_packet+0, 0
	bsf	FLAG_RUMBLE_MOTOR_ON
	return


	;; The N64 requested a 32-byte read from our controller pak bus.
	;; If all is well, this should only happen at address 0x8000, where it
	;; tries to identify what type of controller pak we have. Always
	;; indicate we have a rumble pak by sending all 0x80s.
	;;
	;; Since we're assuming the address is 0x8000, don't even bother scrambling
	;; to start receiving it in time. From the end of the command byte to when
	;; we start transmitting should be about 67us, or 335 cycles.
n64_bus_read
	movlw	.32			; Fill the buffer with 0x80
	movwf	byte_count
	movlw	n64_bus_packet
	movwf	FSR
pak_identify_fill_loop
	movlw	0x80
	movwf	INDF
	incf	FSR, f
	goto	$+1			; Padding to make this loop take most of the
	goto	$+1			;    335 cycles we need to waste.
	decfsz	byte_count, f
	goto	pak_identify_fill_loop

	movlw	0xB8			; 0xB8 is the inverted CRC of a packet with all 0x80s
	movwf	n64_crc

	movlw	n64_bus_packet		; Send back the data and CRC
	movwf	FSR
	movlw	.33
	goto	n64_tx_widestop		; We need a 2us stop bit after all CRCs


	;; The N64 asked for our button and joystick status
n64_send_status
	movlw	n64_status_buffer	; Transmit the status buffer
	movwf	FSR
	movlw	4
	goto	n64_tx


	;; The N64 asked for our identity. Report that we're an
	;; N64 controller with the cotnroller pak slot occupied.
n64_send_id
	movlw	0x05
	movwf	n64_id_buffer+0
	movlw	0x00
	movwf	n64_id_buffer+1
	movlw	0x01
	movwf	n64_id_buffer+2

	movlw	n64_id_buffer		; Transmit the ID buffer
	movwf	FSR
	movlw	3
	goto	n64_tx


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
	bsf	STATUS, RP0
	bsf	N64_TRIS
	bcf	STATUS, RP0
	bcf	N64_PIN
	n64gc_tx_buffer N64_TRIS, 0

n64_tx_widestop
	bsf	STATUS, RP0
	bsf	N64_TRIS
	bcf	STATUS, RP0
	bcf	N64_PIN
	n64gc_tx_buffer N64_TRIS, 1

n64_rx_bus
	n64gc_rx_buffer N64_PIN, bus_byte_count, 0

n64_rx_command
	movlw	.1
	movwf	byte_count
	n64gc_rx_buffer N64_PIN, byte_count, 1		; Clear the watchdog while waiting for commands


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

	btfsc	FLAG_RUMBLE_MOTOR_ON	; Set the low bit of our gamecube command to turn on rumble
	bsf	gamecube_buffer+2, 0

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
	movwf	byte_count
	n64gc_rx_buffer GAMECUBE_PIN, byte_count, 0


	;; *******************************************************************************
	;; ******************************************************  Lookup Tables  ********
	;; *******************************************************************************

	;; This is a table of the resulting CRCs
	;; for every message consisting of 32 copies
	;; of a single byte. It's cheesy, but our
	;; microcontroller is a bit on the slow side
	;; to implement the full CRC in the allotted time,
	;; and this is all we need for initialization
	;; and rumble pak support.
	;;
	;; It was generated using the small-table
	;; implementation of the CRC, in notes/crc.py
get_repeated_crc
	movlw	high crc_table
	movwf	PCLATH
	movf	n64_bus_packet, w
	movwf	PCL

	org 0x300
crc_table
	#include repeated_crc_table.inc

	end
