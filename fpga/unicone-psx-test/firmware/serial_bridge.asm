	;;
	;; Simple Playstation controller to RS-232 bridge, for use
	;; with the matching controller emulator FPGA.
	;; 
	;; Copyright (C) 2007 Micah Dowty <micah@navi.cx>
	;;
	;; This firmware is designed to run on a PIC16F877A microcontroller at 20 MHz.
	;; 

	;; ****************************************************
	;; Hardware Declarations
	;; ****************************************************
	
	list	p=16f877a
	errorlevel -302
	#include p16f877a.inc

	__CONFIG _CP_OFF & _WDT_OFF & _BODEN_OFF & _PWRTE_ON & _HS_OSC & _WRT_OFF & _LVP_OFF & _DEBUG_OFF & _CPD_OFF

	#define PORT1_LED	PORTB
	#define PORT1_ACTIVITY	PORTB, 3

	#define PORT1_DAT	PORTC, 4
	#define PORT1_CMD	PORTC, 0 ; Inverted
	#define PORT1_SEL	PORTC, 1 ; Inverted
	#define PORT1_CLK	PORTC, 2 ; Inverted
	#define PORT1_ACK	PORTE, 2

	#define PORT2_LED	PORTD
	#define PORT2_ACTIVITY	PORTD, 3

	#define PORT2_DAT	PORTE, 1
	#define PORT2_CMD	PORTC, 0 ; Inverted
	#define PORT2_SEL	PORTC, 3 ; Inverted
	#define PORT2_CLK	PORTC, 2 ; Inverted
	#define PORT2_ACK	PORTE, 0

	;; ****************************************************
	;; Reset/Interrupt Vectors
	;; ****************************************************

	org 0
	goto	startup
	org 4
	retfie

	;; ****************************************************
	;; Variables and Constants
	;; ****************************************************

	;; Variables
	cblock	0x20
		temp
		temp2
		cmd_byte
		reply_byte
		digit
		buffer:.40
	endc

	;; ****************************************************
	;; I/O Initialization
	;; ****************************************************

startup
	;; Initialize I/O ports
	banksel PORTA
	clrf	PORTA
	clrf	PORTB
	clrf	PORTC
	clrf	PORTD
	clrf	PORTE

	bcf	PORT1_SEL
	bcf	PORT2_SEL
	bcf	PORT1_CMD
	bcf	PORT1_CLK
	
	banksel	TRISA
	movlw	0x06		; RA1/RA2 input
	movwf	TRISA
	clrf	TRISB		; PORTB, all outputs
	movlw	0x90		; RC7/RC4 input
	movwf	TRISC
	clrf	TRISD		; PORTD, all outputs
	movlw	0x07		; PORTE, all inputs
	movwf	TRISE

	banksel PORTA

	;; ****************************************************
	;; Main Loop
	;; ****************************************************

main_loop
	movlw	.13
	call	delay_ms

	bsf	PORT1_SEL

	movlw	.1
	call	delay_ms

	movlw	0x01
	movwf	cmd_byte
	call	psx_send_recv_byte_port1

	movlw	0x42
	movwf	cmd_byte
	call	psx_send_recv_byte_port1

	movlw	0x00
	movwf	cmd_byte
	call	psx_send_recv_byte_port1

	movlw	0x00
	movwf	cmd_byte
	call	psx_send_recv_byte_port1

	movlw	0x00
	movwf	cmd_byte
	call	psx_send_recv_byte_port1

	bcf	PORT1_SEL
	
	movf	reply_byte, w
	call	display_hex_byte
			
	goto	main_loop


	;; ****************************************************
	;; Copy the carry flag to a pin
	;;
	;; This sets/clears a pin according to the carry
	;; flag, without any glitches. Always takes 5
	;; clock cycles. Optionally negates the carry flag.
	;;
mov_c_to_pin	macro	p_port, p_bit, negate
	local	bit_1
	local done

	if negate
	btfss	STATUS, C
	else
	btfsc	STATUS, C	; 1 2
	endif

	goto	bit_1		; 3 -
	bcf	p_port, p_bit  	; - 3
	goto	done		; - 5 
bit_1
	bsf	p_port, p_bit	; 4 -
	nop			; 5 -
done
	endm


	;; ****************************************************
	;; Copy a pin to the carry flag
	;;
	;; This sets/clears the carry flag according to the value
	;; of a pin. Always takes 3 clock cycles.
	;;
mov_pin_to_c	macro	p_port, p_bit, negate
	clrc
	if negate
	btfss	p_port, p_bit
	else
	btfsc	p_port, p_bit
	endif
	setc
	endm

	
	;; ****************************************************
	;; Transfer one bit in/out via one of the Playstation ports.
	;;
	;; To achieve the nominal bit rate of 250 Khz with a
	;; clock frequency of 20 MHz, this code should be timed such
	;; that each CLK half-cycle is 10 instruction cycles.
	;; 
	;; Nominally we should sample input bits on the rising
	;; edge and set output bits on the falling edge.
	;; 
	;; Inputs:
	;;     cmd_byte
	;;
	;; Outputs:
	;;     reply_byte
	;; 
psx_send_recv_bit	macro	cmd_p, cmd_b, dat_p, dat_b, clk_p, clk_b

	rrf		cmd_byte, f	; H 6
	mov_c_to_pin	cmd_p, cmd_b, 1	; H 11
	bsf		clk_p, clk_b	; L 1

	goto		$+1		; L 3 
	goto		$+1		; L 5
	goto		$+1		; L 7 
	goto		$+1		; L 9

	bcf		clk_p, clk_b	; H 1
	mov_pin_to_c	dat_p, dat_b, 0	; H 4
	rrf		reply_byte, f	; H 5
	
	endm


	;; ****************************************************
	;; Transfer one byte in/out via one of the Playstation ports.
	;;
	;; Inputs:
	;;     cmd_byte
	;;
	;; Outputs:
	;;     reply_byte
	;; 
psx_send_recv_byte	macro	cmd_p, cmd_b, dat_p, dat_b, clk_p, clk_b
	local	delay_loop
	
	psx_send_recv_bit	cmd_p, cmd_b, dat_p, dat_b, clk_p, clk_b
	psx_send_recv_bit	cmd_p, cmd_b, dat_p, dat_b, clk_p, clk_b
	psx_send_recv_bit	cmd_p, cmd_b, dat_p, dat_b, clk_p, clk_b
	psx_send_recv_bit	cmd_p, cmd_b, dat_p, dat_b, clk_p, clk_b
	psx_send_recv_bit	cmd_p, cmd_b, dat_p, dat_b, clk_p, clk_b
	psx_send_recv_bit	cmd_p, cmd_b, dat_p, dat_b, clk_p, clk_b
	psx_send_recv_bit	cmd_p, cmd_b, dat_p, dat_b, clk_p, clk_b
	psx_send_recv_bit	cmd_p, cmd_b, dat_p, dat_b, clk_p, clk_b

	;; XXX:	Wait for ACK
	movlw	.250
	movwf	temp
delay_loop
	decfsz	temp, f
	goto	delay_loop

	endm

psx_send_recv_byte_port1
	psx_send_recv_byte	PORT1_CMD, PORT1_DAT, PORT1_CLK
	return

	
	;; ****************************************************
	;; Display a byte, from 'w', in hexadecimal on the LED displays.
	;; 
	;; Side effects:
	;;     Modifies 'temp'.
	;;
display_hex_byte
	movwf	temp
	call	led_digit_table	
	movwf	PORT2_LED

	swapf	temp, w
	call	led_digit_table	
	movwf	PORT1_LED
	return
				

	;; ****************************************************
	;; Look-up table: hexadecimal digit to 7-segment display
	;; 
	;; Bit positions:
	;; 
	;;      5
	;;     ---
	;;   6| 7 |4
	;;     ---
	;;   0|   |2
	;;     ---   *
	;;      1    3
	;; 
led_digit_table
	andlw	0x0F
	addwf	PCL, f
	retlw	0x77		; 0
	retlw	0x14		; 1
	retlw	0xB3		; 2
	retlw	0xB6		; 3
	retlw	0xD4		; 4
	retlw	0xE6		; 5
	retlw	0xE7		; 6
	retlw	0x34		; 7
	retlw	0xF7		; 8
	retlw	0xF4		; 9
	retlw	0xF5		; A
	retlw	0xC7		; B
	retlw	0x63		; C
	retlw	0x97		; D
	retlw	0xE3		; E
	retlw	0xE1		; F


	;; ****************************************************
	;; Busy-loop for at least 'w' milliseconds.
	;;
	;; Side effects:
	;;     Modifies 'temp' and 'temp2'.
	;; 
delay_ms
	movwf	temp2
delay_ms_loop
	movlw	.250		; 5 MIPS * 1ms = 5000 cycles = 250 iterations
	movwf	temp
delay_ms_loop2
	goto	$+1		; 2
	goto	$+1		; 4
	goto	$+1		; 6
	goto	$+1		; 8
	goto	$+1		; 10
	goto	$+1		; 12
	goto	$+1		; 14
	goto	$+1		; 16
	nop			; 17
	decfsz	temp, f		; 18
	goto	delay_ms_loop2	; 20 cycles/iteration
	decfsz	temp2, f
	goto	delay_ms_loop
	return

	
	;; ****************************************************
	;; Busy-loop for at least 'w' microseconds.
	;;
	;; Side effects:
	;;     Modifies 'temp' and 'temp2'.
	;; 
delay_us
	movwf	temp		; 5 MIPS * 1us = 5 cycles per iteration
delay_us_loop
	goto	$+1
	decfsz	temp, f
	goto	delay_us_loop
	return
			
	end
