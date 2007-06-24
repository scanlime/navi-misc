	;;
	;; Simple Playstation controller to RS-232 bridge, for use
	;; with the matching controller emulator FPGA.
	;; 
	;; Copyright (C) 2007 Micah Dowty <micah@navi.cx>
        ;;
        ;;   This program is free software; you can redistribute it and/or modify
        ;;   it under the terms of the GNU General Public License as published by
        ;;   the Free Software Foundation; either version 2 of the License, or
        ;;   (at your option) any later version.
        ;;

	;; ****************************************************
	;; Hardware Declarations
	;; ****************************************************
	
	list	p=16f877a
	errorlevel -302
	#include p16f877a.inc

	;; Oscillator:	 20 MHz XTAL

	__CONFIG _CP_OFF & _WDT_OFF & _BODEN_OFF & _PWRTE_ON & _HS_OSC & _WRT_OFF & _LVP_OFF & _DEBUG_OFF & _CPD_OFF

	#define SLOT1_LEDS	PORTB

	#define SLOT1_DAT	PORTC, 4
	#define SLOT1_CMD	PORTC, 0 ; Inverted
	#define SLOT1_SEL	PORTC, 1 ; Inverted
	#define SLOT1_CLK	PORTC, 2 ; Inverted
	#define SLOT1_ACK	PORTE, 2

	#define SLOT2_LEDS	PORTD

	#define SLOT2_DAT	PORTE, 1
	#define SLOT2_CMD	PORTC, 0 ; Inverted
	#define SLOT2_SEL	PORTC, 3 ; Inverted
	#define SLOT2_CLK	PORTC, 2 ; Inverted
	#define SLOT2_ACK	PORTE, 0

	;; 
	;; LED display values.
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
	#define LED_DIGIT_BLANK	0x00
	#define LED_DIGIT_DASH	0x80
	#define LED_DIGIT_DOT	0x08
	#define LED_DIGIT_0	0x77
	#define LED_DIGIT_1	0x14
	#define LED_DIGIT_2	0xB3
	#define LED_DIGIT_3	0xB6
	#define LED_DIGIT_4	0xD4
	#define LED_DIGIT_5	0xE6
	#define LED_DIGIT_6	0xE7
	#define LED_DIGIT_7	0x34
	#define LED_DIGIT_8	0xF7
	#define LED_DIGIT_9	0xF4
	#define LED_DIGIT_A	0xF5
	#define LED_DIGIT_B	0xC7
	#define LED_DIGIT_C	0x63
	#define LED_DIGIT_D	0x97
	#define LED_DIGIT_E	0xE3
	#define LED_DIGIT_F	0xE1


	;; ****************************************************
	;; PSX Controller Protocol definitions
	;; ****************************************************

	;;
	;; See http://docs.google.com/View?docid=ddbmmwds_5cw4pk3
	;;

	#define PSX_ADDRESS_CONTROLLER_1		0x01
	#define	PSX_PADDING				0x5A

	#define PSX_MODE_DIGITAL			0x40
	#define PSX_MODE_ANALOG				0x70
	#define PSX_MODE_ESCAPE				0xF0
	#define PSX_MODEMASK				0xF0

	#define PSX_CMD_INIT_PRESSURE			0x40
	#define	PSX_CMD_GET_AVAILABLE_POLL_RESULTS	0x41
	#define	PSX_CMD_POLL				0x42
	#define	PSX_CMD_ESCAPE				0x43
	#define PSX_CMD_SET_MAJOR_MODE			0x44
	#define PSX_CMD_READ_EXTENDED_STATUS_1		0x45
	#define PSX_CMD_READ_CONSTANT_1			0x46
	#define PSX_CMD_READ_CONSTANT_2			0x47
	#define PSX_CMD_READ_CONSTANT_3			0x4C
	#define PSX_CMD_SET_POLL_COMMAND_FORMAT		0x4D
	#define	PSX_CMD_SET_POLL_RESULT_FORMAT		0x4F

	
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

	#define NUM_SLOTS		.2
	#define	MAX_ACTUATORS_LEN	.2

	#define DEBOUNCE_CYCLE_COUNT	.10
	#define DEBOUNCE_DELAY_MS	.5

	;; Some controllers (Logitech wireless) are particularly
	;; sensitive to the interval between poll cycles. 13ms is
	;; close to what most games seem to use, and the Logitech
	;; controller is happy at this frequency.

	#define POLL_INTERVAL_MS	.13
	
	#define STATE_CONNECTED_BIT	0
	#define STATE_ANALOG_BIT	1
	#define STATE_DUALSHOCK_BIT	2
		
	;; Variables
	cblock	0x20
		temp
		temp2

		cmd_byte
		reply_byte

		current_slot
		next_state_on_error
		debounce_count

		slot_states:NUM_SLOTS
		slot_numbers:NUM_SLOTS
		slot_actuators:(NUM_SLOTS * MAX_ACTUATORS_LEN)
	endc

	;; ****************************************************
	;; Hardware Initialization
	;; ****************************************************

startup
	banksel PORTA
	clrf	PORTA
	clrf	PORTB
	clrf	PORTC
	clrf	PORTD
	clrf	PORTE

	bcf	SLOT1_SEL	; Idle state for PSX busses
	bcf	SLOT2_SEL
	bcf	SLOT1_CMD
	bcf	SLOT1_CLK
	
	banksel	TRISA
	movlw	0x06		; RA1/RA2 input
	movwf	TRISA
	clrf	TRISB		; PORTB, all outputs
	movlw	0x90		; RC7/RC4 input
	movwf	TRISC
	clrf	TRISD		; PORTD, all outputs
	movlw	0x07		; PORTE, all inputs
	movwf	TRISE

	banksel	ADCON1
	movlw	0x02		; PORTA analog, PORTE digital
	movwf	ADCON1

	bankisel temp		; Clear the first bank of general purpose RAM
	movlw	0x20		; 0x20 through 0x7F
	movwf	FSR
mem_clear_loop
	clrf	INDF
	incf	FSR, f
	btfss	FSR, 7		; Stop at 0x80
	goto	mem_clear_loop

	banksel	PORTA		; Stay in the first bank by default

	
	;; ****************************************************
	;; Main Loop
	;; ****************************************************

main_loop
	movlw	POLL_INTERVAL_MS
	lcall	delay_ms

	clrf	current_slot
	lcall	controller_poll
	incf	current_slot, f
	lcall	controller_poll
	
	movf	slot_states, w	; XXX, display slot states
	call	led_digit_table	
	movwf	SLOT1_LEDS
	movf	slot_states+1, w
	call	led_digit_table	
	movwf	SLOT2_LEDS
			
	goto	main_loop


	;; ****************************************************
	;; Utility Macros
	;; ****************************************************


	;; ----------------------------------------------------
	;;
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


	;; ----------------------------------------------------
	;;
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
	;; LED Displays
	;; ****************************************************
	
	;; ----------------------------------------------------
	;;
	;; Display a byte, from 'w', in hexadecimal on the LED displays.
	;; 
	;; Side effects:
	;;	Modifies 'temp'.
	;;
display_hex_byte
	movwf	temp
	call	led_digit_table	
	movwf	SLOT2_LEDS

	swapf	temp, w
	call	led_digit_table	
	movwf	SLOT1_LEDS
	return
				

	;; ----------------------------------------------------
	;;
	;; Look-up table: hexadecimal digit to 7-segment display
	;; 
led_digit_table
	andlw	0x0F
	addwf	PCL, f
	retlw	LED_DIGIT_0
	retlw	LED_DIGIT_1
	retlw	LED_DIGIT_2
	retlw	LED_DIGIT_3
	retlw	LED_DIGIT_4
	retlw	LED_DIGIT_5
	retlw	LED_DIGIT_6
	retlw	LED_DIGIT_7
	retlw	LED_DIGIT_8
	retlw	LED_DIGIT_9
	retlw	LED_DIGIT_A
	retlw	LED_DIGIT_B
	retlw	LED_DIGIT_C
	retlw	LED_DIGIT_D
	retlw	LED_DIGIT_E
	retlw	LED_DIGIT_F


	;; ****************************************************
	;; Delay Loops
	;; ****************************************************

	;; ----------------------------------------------------
	;;
	;; Busy-loop for at least 'w' milliseconds.
	;;
	;; Side effects:
	;;	Modifies 'temp' and 'temp2'.
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

	
	;; ----------------------------------------------------
	;;
	;; Busy-loop for at least 'w' microseconds.
	;;
	;; Side effects:
	;;	Modifies 'temp' and 'temp2'.
	;; 
delay_us
	movwf	temp		; 5 MIPS * 1us = 5 cycles per iteration
delay_us_loop
	goto	$+1
	decfsz	temp, f
	goto	delay_us_loop
	return


	;; ****************************************************
	;; High level Playstation controller I/O
	;; ****************************************************

	;; ----------------------------------------------------
	;;
	;; Macros to send a byte (from 'w', a constant, or a file
	;; register) to the current controller slot, wait for an
	;; ACK, and jump to 'controller_error' on error. The
	;; 'expect' version also signals an error if the reply
	;; byte doesn't match the one provided.
	;;
	;; Inputs:
	;;	w, a file register, or a literal.
	;;
	;; Outputs:
	;;	reply_byte
	;;
controller_xfer_sub
	movwf	cmd_byte
	lcall	psx_xfer_byte
	lcall	psx_ack_wait
	return

controller_xfer_w	macro
	movwf	cmd_byte
	call	controller_xfer_sub
	btfss	STATUS, C
	goto	controller_error
	endm

controller_xfer		macro	literal
	movlw	literal
	controller_xfer_w
	endm

controller_xfer_no_ack	macro	literal
	movlw	literal
	movwf	cmd_byte
	lcall	psx_xfer_byte
	endm

controller_xfer_f	macro	file
	movf	file, w
	controller_xfer_w
	endm

controller_xfer_expect	macro	cmd_literal, reply_literal
	movlw	cmd_literal
	controller_xfer_w
	movlw	reply_literal
	xorwf	reply_byte, w
	btfss	STATUS, Z
	goto	controller_error
	endm


	;; ----------------------------------------------------
	;; 
	;; Macro to point INDF at the current slot's state.
	;; 
fsr_slot_state	macro
	bankisel slot_states
	movlw	slot_states
	addwf	current_slot, w
	movwf	FSR
	endm


	;; ----------------------------------------------------
	;;
	;; Detect the presence of a controller on the current slot.
	;; If we detect a controller successfully, probe for additional
	;; capabilities (like analog mode and pressure-sensitive buttons)
	;; that we can initialize.
	;;
	;; Inputs:
	;;	current_slot
	;;	slot_state
	;;
	;; Outputs:
	;;	slot_state
	;;
controller_detect

	;; 1. Detect presence: Send a POLL, verify that the
	;;    returned padding byte is correct. Do this several
	;;    times, in order to make sure the contacts aren't
	;;    still bouncing because the controller was just
	;;    inserted.

	clrf	next_state_on_error	; Disconnect on error

	movlw	DEBOUNCE_CYCLE_COUNT
	movwf	debounce_count
controller_debounce_loop

	;; Note: We'd like to be able to controller_xfer_expect here,
	;;       but some controllers (like the Guitar Hero controller)
	;;       don't respond using the typical values.

	lcall	psx_begin
	controller_xfer		PSX_ADDRESS_CONTROLLER_1
	controller_xfer		PSX_CMD_POLL
	controller_xfer		0x00
	lcall	psx_end_wait

	movlw	DEBOUNCE_DELAY_MS
	lcall	delay_ms
	decfsz	debounce_count, f
	goto	controller_debounce_loop
		
	;; 2. At this point, we know that at the very least
	;;    we have a functioning digital controller. Set CONNECTED.

	fsr_slot_state
	bsf	INDF, STATE_CONNECTED_BIT
	movf	INDF, w
	movwf	next_state_on_error
			
	;; 3. Try to enter analog mode.

	movlw	0x01			; Enter escape mode
	lcall	controller_escape

	lcall	psx_begin		; Set up analog mode
 	controller_xfer_expect	PSX_ADDRESS_CONTROLLER_1, 0xFF
 	controller_xfer_expect	PSX_CMD_SET_MAJOR_MODE, PSX_MODE_ESCAPE | 0x03
 	controller_xfer_expect	0x00, PSX_PADDING
 	controller_xfer		0x01
 	controller_xfer		0x03
 	controller_xfer		0x00
 	controller_xfer		0x00
	controller_xfer		0x00
 	controller_xfer_no_ack	0x00
	lcall	psx_end_wait

	movlw	0x00			; Exit escape mode
	lcall	controller_escape

	;; 4. Test analog mode. If we poll now, the reply length and mode should
	;;    indicate that we're in analog mode. If so, set ANALOG.

 	lcall	psx_begin
 	controller_xfer		PSX_ADDRESS_CONTROLLER_1
 	controller_xfer		PSX_CMD_POLL
 	lcall	psx_end_wait

 	movf	reply_byte, w
 	andlw	PSX_MODEMASK
 	xorlw	PSX_MODE_ANALOG
 	btfss	STATUS, Z
 	goto	controller_error
	
	fsr_slot_state
	bsf	INDF, STATE_ANALOG_BIT
	movf	INDF, w
	movwf	next_state_on_error

	;; 5. Enable force feedback. What this actually does it to specify a
	;;    mapping from bytes in our polling packet to force feedback actuators
	;;    on the controller. This code assumes 2 actuators, which is the case
	;;    with Dual Shock controllers.
	
	movlw	0x01			; Enter escape mode
	lcall	controller_escape

	lcall	psx_begin
 	controller_xfer_expect	PSX_ADDRESS_CONTROLLER_1, 0xFF
 	controller_xfer_expect	PSX_CMD_SET_POLL_COMMAND_FORMAT, PSX_MODE_ESCAPE | 0x03
 	controller_xfer_expect	0x00, PSX_PADDING
 	controller_xfer		0x00	; Byte 0 => Actuator 0
 	controller_xfer		0x01	; Byte 1 => Actuator 1
 	controller_xfer		0xFF	; Byte 2 (Disabled)
 	controller_xfer		0xFF	; Byte 3 (Disabled)
 	controller_xfer		0xFF	; Byte 4 (Disabled)
 	controller_xfer_no_ack	0xFF	; Byte 5 (Disabled)
	lcall	psx_end_wait

	movlw	0x00			; Exit escape mode
	lcall	controller_escape



	;; We've made it all the way! Identify this controller with
	;; the DUALSHOCK bit, and return.
	
	fsr_slot_state
	bsf	INDF, STATE_DUALSHOCK_BIT
	movf	INDF, w
	return


	;; ----------------------------------------------------
	;;
	;; Enter/leave the controller's escape mode. All commands
	;; other than POLL and ESCAPE are only valid in escape
	;; mode.
	;;
	;; Escape mode probably exists because the controller
	;; must be notified ahead of time that the length of the
	;; reply must change. (Reply length and command ID overlap
	;; in the Playstation's protocol)
	;;
	;; See http://docs.google.com/View?docid=ddbmmwds_5cw4pk3
	;; 
	;; Inputs:
	;;	w=0, exit escape mode. w=1, enter escape mode.
	;;
	;; Side effects:
	;;	Modifies 'temp' and 'temp2'.
	;;
controller_escape
	movwf	temp2
	lcall	psx_begin
	controller_xfer		PSX_ADDRESS_CONTROLLER_1
	controller_xfer		PSX_CMD_ESCAPE
	controller_xfer		0x00
	controller_xfer_f	temp2
	controller_xfer		0x00
	lcall	psx_end_wait
	return
	
	
	;; ----------------------------------------------------
	;; 
	;; Poll the current controller. Sends all actuator
	;; changes to the controller, while reading current
	;; status.
	;;
	;; Inputs:
	;;	current_slot
	;;	slot_actuators
	;;	slot_state
	;;
	;; Outputs:
	;;	slot_state
	;; 
controller_poll
	fsr_slot_state			; Not connected? Try to connect.
	btfss	INDF, STATE_CONNECTED_BIT
	call	controller_detect

	fsr_slot_state			; Still not connected? Give up.
	btfss	INDF, STATE_CONNECTED_BIT
	return

	clrf	next_state_on_error	; Disconnect on error
	lcall	psx_begin

	controller_xfer		0x01	; [0] Address a controller at port 1
 	controller_xfer		0x42	; [1] Polling command
	controller_xfer		0x00	; [2] Header padding byte

 	movlw	0x00
 	movwf	cmd_byte
 	lcall	psx_xfer_byte	; [3] Data byte 0
 	lcall	psx_ack_wait
	btfss	STATUS, C
	goto	controller_error

 	movlw	0x00
 	movwf	cmd_byte
 	lcall	psx_xfer_byte	; [4] Data byte 1
 	lcall	psx_ack_wait

 	lcall	psx_end
	return

	
	;; ----------------------------------------------------
	;;
	;; Controller error handler. De-selects the
	;; controller, and reverts it to 'next_state_on_error'.
	;;
	;; Inputs:
	;;	current_slot
	;;	next_state_on_error
	;;
	;; Outputs:
	;;	slot_state
	;;
controller_error
	lcall	psx_end_wait
	fsr_slot_state
	movf	next_state_on_error, w
	movwf	INDF
	return


	;; ****************************************************
	;; Low level Playstation port I/O
	;; ****************************************************

	;; ----------------------------------------------------
	;; 
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
psx_xfer_bit_m	macro	cmd_p, cmd_b, dat_p, dat_b, clk_p, clk_b

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


	;; ----------------------------------------------------
	;;
	;; Transfer one byte in/out via one of the Playstation ports.
	;;
	;; Inputs:
	;;     cmd_byte
	;;
	;; Outputs:
	;;     reply_byte
	;; 
psx_xfer_byte_m	macro	cmd_p, cmd_b, dat_p, dat_b, clk_p, clk_b
	psx_xfer_bit_m	cmd_p, cmd_b, dat_p, dat_b, clk_p, clk_b
	psx_xfer_bit_m	cmd_p, cmd_b, dat_p, dat_b, clk_p, clk_b
	psx_xfer_bit_m	cmd_p, cmd_b, dat_p, dat_b, clk_p, clk_b
	psx_xfer_bit_m	cmd_p, cmd_b, dat_p, dat_b, clk_p, clk_b
	psx_xfer_bit_m	cmd_p, cmd_b, dat_p, dat_b, clk_p, clk_b
	psx_xfer_bit_m	cmd_p, cmd_b, dat_p, dat_b, clk_p, clk_b
	psx_xfer_bit_m	cmd_p, cmd_b, dat_p, dat_b, clk_p, clk_b
	psx_xfer_bit_m	cmd_p, cmd_b, dat_p, dat_b, clk_p, clk_b
	endm

	
	;; ----------------------------------------------------
	;;
	;; Wait for an ACK on one of the Playstation ports.
	;; 
	;; On official controllers, the ACK is generally 3us long,
	;; beginning 8us after the last rising edge of CLK. In this
	;; implementation, we poll every 1us for an ACK pulse and
	;; give up after 20us.
	;;
	;; Outputs:
	;;	C=1 on success, C=0 on failure
	;; 
	;; Side effects:
	;;	Modifies 'temp'.
	;; 
psx_ack_wait_m macro   ack_p, ack_b    
	local   done
	local   keep_waiting
 
	setc			; Assume success for now
	movlw	.20		; Timeout, in microseconds
	movwf	temp
keep_waiting
	btfss	ack_p, ack_b	; Poll every 5 cycles (1us)
	goto	done
	decfsz	temp, f
	goto	keep_waiting
	clrc			; Timed out
done
	endm


	;; ----------------------------------------------------
	;;
	;; Begin a command on one of the Playstation ports
	;; 
	;; Side effects:
	;;	Modifies 'temp'.
	;;
psx_begin_m	macro	sel_p, sel_b
	bsf	sel_p, sel_b
	movlw	.20		; Controller needs 20us after SEL to wake up
	lcall	delay_us
	endm


	;; ----------------------------------------------------
	;;
	;; End a command on one of the Playstation ports
	;; 
psx_end_m	macro	sel_p, sel_b
	bcf	sel_p, sel_b
	endm


	;; ----------------------------------------------------
	;; 
	;; Macro instantiations for PSX slots 1 and 2.
	;;
	;; The low-level Playstation I/O code is implemented as a
	;; set of macros, parameterized by I/O pins. This is necessary
	;; for performance. This layer wraps those low-level macros
	;; and provides a set of subroutines that respect the
	;; 'current_slot' variable.
	;; 

psx_xfer_byte
	pagesel	pxb_slot2
	btfsc	current_slot, 0
	goto	pxb_slot2
	psx_xfer_byte_m	SLOT1_CMD, SLOT1_DAT, SLOT1_CLK
	return

psx_begin
	pagesel	pb_slot2
	btfsc	current_slot, 0
	goto	pb_slot2
	psx_begin_m	SLOT1_SEL
	return

psx_end
	pagesel	pe_slot2
	btfsc	current_slot, 0
	goto	pe_slot2
	psx_end_m	SLOT1_SEL
	return

psx_ack_wait
	pagesel	paw_slot2
	btfsc	current_slot, 0
	goto	paw_slot2
	psx_ack_wait_m	SLOT1_ACK
	return

pxb_slot2
	psx_xfer_byte_m	SLOT2_CMD, SLOT2_DAT, SLOT2_CLK
	return

pb_slot2
	psx_begin_m	SLOT2_SEL
	return

pe_slot2
	psx_end_m	SLOT2_SEL
	return

paw_slot2
	psx_ack_wait_m	SLOT2_ACK
	return


	;; ----------------------------------------------------
	;; 
	;; End a Playstation command, and delay enough that the
	;; controller should be ready to accept the next command.
	;;
	;; This is a recommended wrapper to use around psx_end
	;; in all situations other than normal controller polling.
	;;
psx_end_wait
	lcall	psx_end
	movlw	.2
	lgoto	delay_ms

	end
