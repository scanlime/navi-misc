	;;
	;; A reverse engineering and debugging utility that converts commands
	;; sent over RS-232 to commands encoded in the N64/Gamecube low-level format.
	;; Copyright (C) 2004 Micah Dowty <micah@navi.cx>
	;;
	;;   This program is free software; you can redistribute it and/or modify
	;;   it under the terms of the GNU General Public License as published by
	;;   the Free Software Foundation; either version 2 of the License, or
	;;   (at your option) any later version.
	;;
	;; This firmware is designed to run on a PIC16F84A microcontroller
	;; clocked at 20 MHz. The Nintendo device's bidirectional data line
	;; is on RB0, serial receive is on RB1, serial transmit is on RB2.
	;;
	;; This accepts commands of the following form over the serial port,
	;; which is configured for 38400 baud, 8-N-1:
	;;
	;;   Byte   Function
	;; ----------------------
	;;   0      Always 0x7E
	;;   1      Number of bytes to transmit (up to 40)
	;;   2      Number of bytes to receive afterwards (up to 40)
	;;   3-n    Data to transmit
	;;

	list	p=16f84a
	errorlevel -302
	#include p16f84a.inc

	#include n64gc_comm.inc
	#include rs232_comm.inc

	__CONFIG   _CP_OFF & _PWRTE_OFF & _WDT_ON & _HS_OSC

	;; Hardware declarations
	#define NINTENDO_PIN	PORTB, 0
	#define NINTENDO_TRIS	TRISB, 0
	#define RX232_PIN	PORTB, 1
	#define TX232_PIN	PORTB, 2

	;; Reset and interrupt vectors
	org 0
	goto	startup
	org 4
	retfie

	;; Variables
	cblock	0x0C
		temp
		bit_count
		byte_count
		serial_byte
		buffer:40
	endc

startup
	;; Initialize I/O ports- all unused pins output low, nintendo pin and serial receive
	;; are initially tristated.
	bcf	STATUS, RP0
	clrf	PORTA
	clrf	PORTB
	bsf	STATUS, RP0
	clrf	TRISA
	movlw	0x03
	movwf	TRISB

	n64gc_init

main_loop
	call	serial_rx
	movwf	buffer

	movlw	buffer
	movwf	FSR
	movlw	1
	call	nintendo_tx

	goto	main_loop


serial_rx
	rs232_rx_byte	RX232_PIN, B38400, POLARITY_INVERTED, serial_byte
	movf	serial_byte, w
	return

nintendo_tx
	bcf	NINTENDO_PIN
	n64gc_tx_buffer NINTENDO_TRIS, 0

nintendo_rx
	n64gc_rx_buffer NINTENDO_PIN, 0

	end
