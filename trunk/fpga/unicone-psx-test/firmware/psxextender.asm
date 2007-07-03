	;;
	;; Remote unit for the Playstation controller extender.
	;;
	;; This is a Playstation controller to serial bridge. It has
	;; two controller ports, a 7-segment display (indicating channel
	;; assignment and status) for each port, and an RJ45 uplink.
	;;
	;; The uplink consists of two asynchronous serial signals,
	;; each with a dedicated twisted pair. Electrically they
	;; are basically RS-485, with an 8-N-1 RS-232-equivalent
	;; data format. Mark (1) is + > -, Space (0) is - > +. As
	;; with RS-232, the signals should be Mark when idle.
	;; 
	;;   - From base unit to remote, a 19200 baud signal
	;;     encodes force-feedback information and LED display
	;;     updates.
	;; 
	;;     The differential signal is received using one of the
	;;     PIC's built-in comparators and the USART, and it is
	;;     processed by an interrupt handler.
	;;
	;;     Packet format:
	;;       - Start-of-packet flag (0x5A)
	;;         (This may be doubled to ensure the receiver
	;;          isn't in escape mode when the packet starts)
	;; 
	;;       - The rest of the packet is byte-stuffed. An extra
	;;         0xFE byte must be inserted by the transmitter
	;;         prior to any 0xFE or 0x5A bytes. The receiver
	;;         always interprets bytes following 0xFE as data.
	;; 
	;;          - For each slot:
	;;		- 1 byte LED status
	;;		- 2 byte actuator status
	;; 
	;;   - From remote to base unit, a 250 kbaud signal
	;;     provides fast low-latency controller updates.
	;; 
	;;     This signal is bit-banged synchronously with the
	;;     clock used on the Playstation bus. It has ~1 bit
	;;     of latency above the latency imposed by the
	;;     controller itself and our polling schedule.
	;; 
	;;     Packet format:
	;;       - Start-of-packet flag (0x50) OR'ed with slot number
	;;       - State bits
	;;       - Data from controller (0 to 18 bytes, length depending only on state)
	;;
	;;     There is no byte stuffing on this signal, but as
	;;     the data is expected to be bursty the receiver
	;;     can use an inter-packet timeout (of about 1ms)
	;;     to recover from noisy packets.
	;; 
	;; The remote unit has a free-running update clock, sending
	;; back update packets as often as possible.
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

	__CONFIG _CP_OFF & _WDT_ON & _BODEN_ON & _PWRTE_ON & _HS_OSC & _WRT_OFF & _LVP_OFF & _DEBUG_OFF & _CPD_OFF

	;; All PSX outputs (SEL, CLK, CMD) are inverted.
	
	#define SLOT1_LEDS	PORTB
	
	#define SLOT1_DAT	PORTA, 2
	#define SLOT1_SEL	PORTC, 1 ; Inverted
	#define SLOT1_ACK	PORTE, 2
	
	#define SLOT2_LEDS	PORTD

	#define SLOT2_DAT	PORTE, 1
	#define SLOT2_SEL	PORTC, 3 ; Inverted
	#define SLOT2_ACK	PORTE, 0

	;; CLK and CMD are shared between all ports. They must
	;; be on the same output port as TX+/TX-.
	
	#define PSX_PORT	PORTC
	#define	CMD_MASK	0x01	; Inverted
	#define	CLK_MASK	0x04	; Inverted
	#define CLK_BIT		2
	#define TX_SPACE_MASK	0x20	; TX- = 1
	#define TX_MARK_MASK	0x10	; TX+ = 1 
	#define PSX_OTHER_MASK  0x9A	; All pins not listed here
	
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

	#define LED_DOT_BIT	3


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
	;; Variables and Constants
	;; ****************************************************

	#define TX_START_OF_PACKET	0x50
	#define RX_START_OF_PACKET	0x5A
	#define RX_ESCAPE		0xFE

	#define RX_ESCAPE_FLAG		rx_flags, 0
	#define RX_PACKET_DATA_FLAG	rx_flags, 1

	#define ACTIVITY_FLAG           gen_flags, 0
		
	#define NUM_SLOTS		.2
	#define	MAX_ACTUATORS_LEN	.18
	#define NUM_PRESSURE_BUTTONS	.12

	#define DEBOUNCE_CYCLE_COUNT	.50

	;; Longer-period timing is based on TMR0 with a 1:16 prescaler.
	;; At 20 MHz, this gives us about 1221 Hz.

	#define TMR0_HZ			.1221
	
	;; Some controllers (Logitech wireless) are particularly
	;; sensitive to the interval between poll cycles.  This
	;; constant is the target polling rate that our per-slot
	;; accumulators try to hit. 
	;;
	;; On average, each slot will poll at TMR0_HZ / POLL_INTERVAL_TICKS Hz.
	
	#define POLL_INTERVAL_TICKS	.15	; 81.4 Hz

	;; How many TMR0 ticks can elapse between RX packets?

	#define RX_WATCHDOG_TICKS	TMR0_HZ	; 1 sec
	
	;; How long to make our activity blink. This is a power of two specified
	;; in terms of POLL_INTERVAL.

	#define LOG_ACTIVITY_BLINK_LENGTH	.3	; 98ms 

	;; How long controllers are inactive before we put them in low-power mode.
	;; This is a 16-bit value, specified in terms of POLL_INTERVAL. Note that
	;; force feedback does not work in low-power mode.

	#define POWERDOWN_INACTIVITY_TIME	(.4884 * .5) ; 5 minutes

	;; Which major mode do we use for powerdown?
	;; 0x01 uses analog mode (without fancy dual-shock features). This
	;; is enough to get the wireless controllers to turn themselves off
	;; eventually. You can set this to 0x00 if you want to use plain
	;; digital mode. This might save a little extra power.

	#define POWERDOWN_MODE			0x01
	
	;; Per-slot state bits. These are transmitted over the wire.

	#define STATE_CONNECTED_BIT	0
	#define STATE_ANALOG_BIT	1
	#define STATE_DUALSHOCK_BIT	2
	#define STATE_POWERDOWN_BIT	3
			
	cblock	0x20
		;; General purpose
		temp
		temp2
		temp3
		gen_flags

		;; Low-level I/O 
		cmd_byte
		reply_byte
		psx_port_latch

		;; Controller I/O
		current_slot
		next_state_on_error
		debounce_count
		packet_iter
		byte_count

		slot_states:NUM_SLOTS
		slot_mapping:NUM_SLOTS
		slot_actuators:(NUM_SLOTS * MAX_ACTUATORS_LEN)
		slot_inactivity_low:NUM_SLOTS
		slot_inactivity_high:NUM_SLOTS
		slot_activity_blink_timer:NUM_SLOTS
	
		;; Interrupt handler registers
		slot_poll_timers:NUM_SLOTS
		rx_byte
		rx_flags
		rx_byte_count
		rx_watchdog_low
		rx_watchdog_high
	endc

	;; Bank-free RAM
	cblock	0x70
		w_temp
		status_temp
		pclath_temp
	endc


	;; ****************************************************
	;; Reset/Interrupt Vectors
	;; ****************************************************

	org 0
	pagesel	startup
	goto	startup

	;; Warning: If you're using a PIC16F877A bootloader,
	;;          it might not copy words 0x0004 through 0x0007.
	;;          You may need to use a traditional PIC programmer
	;;          to download the firmware once before using a
	;;          bootloader, otherwise the interrupt handler
	;;          will not save CPU state correctly and you'll
	;;          end up with unpredictable failures later on!
	org 4

	;; ----------------------------------------------------
	;;
	;; Save registers
	;;

	movwf	w_temp
	swapf	STATUS, w
	clrf	STATUS
	movwf	status_temp
	movf	PCLATH, w
	movwf	pclath_temp
	clrf	PCLATH

	;; ----------------------------------------------------
	;;
	;; Detect interrupt source
	;;

	btfsc	INTCON, TMR0IF
	goto	tmr0_interrupt
end_tmr0_interrupt
	
	btfsc	PIR1, RCIF
	goto	usart_interrupt
end_usart_interrupt
	
	;; ----------------------------------------------------
	;;
	;; Restore registers
	;;
	
	movf	pclath_temp, w
	movwf	PCLATH
	swapf	status_temp, w
	movwf	STATUS
	swapf	w_temp, f
	swapf	w_temp, w
	retfie


	;; ----------------------------------------------------
	;;
	;; Timer 0 interrupt handler
	;;
tmr0_interrupt
	bcf	INTCON, TMR0IF

	;; Decrement the two polling timers
	decf	slot_poll_timers+0, f
	decf	slot_poll_timers+1, f

	;; Is the RX watchdog already expired?
	movf	rx_watchdog_low, w
	iorwf	rx_watchdog_high, w
	btfsc	STATUS, Z
	goto	end_tmr0_interrupt
	
	;; Decrement the RX watchdog...
	decf	rx_watchdog_low, f
	incf	rx_watchdog_low, w	; Did it just roll over?
	btfsc	STATUS, Z
	decf	rx_watchdog_high, f

	;; Did it just become zero?
	movf	rx_watchdog_low, w
	iorwf	rx_watchdog_high, w
	btfss	STATUS, Z
	goto	end_tmr0_interrupt

	;; The RX watchdog has expired. Turn off LEDs and motors.
	movlw	LED_DIGIT_DOT
	andwf	SLOT1_LEDS, f
	andwf	SLOT2_LEDS, f
	clrf	slot_actuators + 0
	clrf	slot_actuators + 1
	clrf	slot_actuators + MAX_ACTUATORS_LEN + 0
	clrf	slot_actuators + MAX_ACTUATORS_LEN + 1

	goto end_tmr0_interrupt

	
	;; ----------------------------------------------------
	;;
	;; USART Receive interrupt: Packet framing
	;;
usart_interrupt
	movf	RCREG, w
	movwf	rx_byte

	btfsc	RX_ESCAPE_FLAG		; Was the previous byte an escape?
	goto	rx_data_byte		; If so, this byte must be data

	movlw	RX_START_OF_PACKET	; Check for start-of-packet
	xorwf	rx_byte, w
	btfsc	STATUS, Z
	goto	rx_start_of_packet

	movlw	RX_ESCAPE		; Check for escape
	xorwf	rx_byte, w
	btfsc	STATUS, Z
	goto	rx_escape

	;; Fall through...

	;; ----------------------------------------------------
	;;
	;; Handler for USART packet data bytes
	;;
rx_data_byte
	bcf	RX_ESCAPE_FLAG
	btfss	RX_PACKET_DATA_FLAG	; Stop receiving if this isn't valid packet data
	goto	end_usart_interrupt

	movf	rx_byte_count, w	; Next byte...
	incf	rx_byte_count, f
	
	addwf	PCL, f			; Save some CPU by using a jump table for each packet byte
	goto	rx_slot0_led
	goto	rx_slot0_actuator0
	goto	rx_slot0_actuator1
	goto	rx_slot1_led
	goto	rx_slot1_actuator0
	goto	rx_slot1_actuator1

	bcf	RX_PACKET_DATA_FLAG	; End of packet
	goto	end_usart_interrupt

	;; Macro to set the value of an LED display.
	;; Copies all bits except LED_DIGIT_DOT.
	;; Modifies 'from'.
copy_led_state_m	macro	from, to
	movlw	~LED_DIGIT_DOT
	andwf	from, f
	movlw	LED_DIGIT_DOT
	andwf	to, w
	iorwf	from, w
	movwf	to
	endm
	
rx_slot0_led
	copy_led_state_m rx_byte, SLOT1_LEDS
	goto	end_usart_interrupt

rx_slot1_led
	copy_led_state_m rx_byte, SLOT2_LEDS
	goto	end_usart_interrupt

rx_slot0_actuator0
	movf	rx_byte, w
	movwf	slot_actuators+0
	goto	end_usart_interrupt

rx_slot0_actuator1
	movf	rx_byte, w
	movwf	slot_actuators+1
	goto	end_usart_interrupt

rx_slot1_actuator0
	movf	rx_byte, w
	movwf	slot_actuators+MAX_ACTUATORS_LEN+0
	goto	end_usart_interrupt
	
rx_slot1_actuator1
	movf	rx_byte, w
	movwf	slot_actuators+MAX_ACTUATORS_LEN+1
	goto	end_usart_interrupt
		
			
	;; ----------------------------------------------------
	;;
	;; Handler for USART start-of-packet bytes
	;;
rx_start_of_packet
	clrf	rx_byte_count
	bsf	RX_PACKET_DATA_FLAG

	movlw	low RX_WATCHDOG_TICKS
	movwf	rx_watchdog_low
	movlw	high RX_WATCHDOG_TICKS
	movwf	rx_watchdog_high

	goto	end_usart_interrupt

	
	;; ----------------------------------------------------
	;;
	;; Handler for USART escape bytes
	;;
rx_escape
	bsf	RX_ESCAPE_FLAG
	goto	end_usart_interrupt


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

	movlw	TX_MARK_MASK	; Transmit a mark when idle
	movwf	PSX_PORT

	movlw	.100		; Power-on settling delay. This also makes it really
	lcall	delay_ms	;   obvious if we get reset by the WDT (LEDs go dark briefly)
	
	movlw	LED_DIGIT_DOT	; Display "." on the LEDs when idle, to indicate power
	movwf	SLOT1_LEDS
	movwf	SLOT2_LEDS
	
	banksel	TRISA
	movlw	0x0D		; RA0, RA2, RA3 inputs
	movwf	TRISA
	clrf	TRISB		; PORTB, all outputs
	movlw	0x80		; RC7 input
	movwf	TRISC
	clrf	TRISD		; PORTD, all outputs
	movlw	0x07		; PORTE, all inputs
	movwf	TRISE

	banksel	ADCON1
	movlw	0x04		; AN0, AN1, and AN3 are analog
	movwf	ADCON1

	;; Clear the first bank of general purpose RAM
	bankisel temp
	movlw	0x20		; 0x20 through 0x7F
	movwf	FSR
mem_clear_loop
	clrf	INDF
	incf	FSR, f
	btfss	FSR, 7		; Stop at 0x80
	goto	mem_clear_loop

	banksel	OPTION_REG
	movlw	0x03		; Give TMR0 a 1:16 prescaler
	movwf	OPTION_REG	;
	banksel	TMR0		; ... and clear it 
	clrf	TMR0

	banksel	CMCON		; Set up a comparator (for USART RX):
	movlw	0x01		;   (RA3 > RA0) => RA4 
	movwf	CMCON

	banksel SPBRG		; 19200 Baud (20MHz/BRGH=1)
	movlw	.64
	movwf	SPBRG
	banksel	TXSTA		; USART Transmit disabled
	movlw	0x04		;   (BRGH=1)
	movwf	TXSTA
	banksel	RCSTA		; USART Receive enabled
	movlw	0x90
	movwf	TXSTA

	banksel	PIE1		; Enable TMR0 and USART receive interrupts
	movlw	0x20
	movwf	PIE1
	movlw	0xE0 
	movwf	INTCON
		
	banksel	PORTA

	
	;; ****************************************************
	;; Main Loop
	;; ****************************************************

	;; ----------------------------------------------------
	;;
	;; The main loop uses two timers, decremented
	;; by the TMR0 interrupt handler, to control
	;; the rate at which we poll each controller.
	;; Each slot gets its own timer, so that changes
	;; in one controller's state don't affect the
	;; other controller's poll frequency.
	;;
	;; This macro divides up the main loop's per-slot work.
	;; It contains higher-level functionality for timers and
	;; LEDs, where pretty much everything is parameterized
	;; by slot.
	;;
main_loop_slot_m	macro	slot, leds
	local	done
	local	not_connected
	local	connected
	local	active
	local	inactive
	local	not_blinking
	
	btfss	slot_poll_timers + slot, 7	; Check for timer underflow
	goto	done
	movlw	POLL_INTERVAL_TICKS		; Reset the timer
	addwf	slot_poll_timers + slot, f
	
	movlw	slot				; Make this slot current, and poll it 
	movwf	current_slot 
	lcall	controller_poll

	;; Is this slot's controller connected?
	btfss	slot_states + slot, STATE_CONNECTED_BIT
	goto	not_connected
connected

	;; Is the controller active?
	btfsc	ACTIVITY_FLAG
	goto	active

	;; Connected but not active: Light the LED continuously,
	;; and update the inactivity counter. If it gets high enough,
	;; enter powerdown mode.
inactive
	bsf	leds, LED_DOT_BIT

	btfsc	slot_states + slot, STATE_POWERDOWN_BIT
	goto	done				; Already in powerdown mode
	
	incf	slot_inactivity_low + slot, f	; Increment the 16-bit inactivity counter
	btfsc	STATUS, Z
	incf	slot_inactivity_high + slot, f

	movf	slot_inactivity_high + slot, w	; Have we hit POWERDOWN_INACTIVITY_TIME?
	xorlw	high POWERDOWN_INACTIVITY_TIME
	btfss	STATUS, Z
	goto	done
	movf	slot_inactivity_low + slot, w
	xorlw	low POWERDOWN_INACTIVITY_TIME
	btfss	STATUS, Z
	goto	done

	lcall	controller_enter_powerdown	; Yes. Enter power-down mode.
	lgoto	done

	;; Connected and active. We want to produce a blink that lasts
	;; ACTIVITY_BLINK_LENGTH just after the controller first becomes active.
active
	pagesel	controller_exit_powerdown
	btfsc	slot_states + slot, STATE_POWERDOWN_BIT
	call	controller_exit_powerdown
	
	movf	slot_inactivity_low + slot, w	; Was controller already active? 
	iorwf	slot_inactivity_high + slot, w	;   (slot_inactivity == 0) 
	btfss	STATUS, Z
	clrf	slot_activity_blink_timer + slot ; Just became active. Clear blink timer. 

	clrf	slot_inactivity_low + slot	; Reset inactivity timer
	clrf	slot_inactivity_high + slot

	pagesel	not_blinking
	btfsc	slot_activity_blink_timer + slot, LOG_ACTIVITY_BLINK_LENGTH
	goto	not_blinking
	incf	slot_activity_blink_timer + slot, f
	bcf	leds, LED_DOT_BIT
	goto	done
not_blinking
	bsf	leds, LED_DOT_BIT
	goto	done
	
	;; Not connected: Clear this slot's LED dot, and we're done.
not_connected
	bcf	leds, LED_DOT_BIT
done
	endm
	

main_loop
	clrwdt
	main_loop_slot_m 0, SLOT1_LEDS
	main_loop_slot_m 1, SLOT2_LEDS
	goto	main_loop


	;; ****************************************************
	;; LED Displays
	;; ****************************************************
	
	;; ----------------------------------------------------
	;;
	;; Display a byte, from 'w', in hexadecimal on the LED displays.
	;; (For debugging)
	;; 
	;; Side effects:
	;;	Modifies 'temp'.
	;;
display_hex_byte
	movwf	temp

	movlw	high led_digit_table
	movwf	PCLATH
	
	movf	temp, w	
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
	clrwdt			; 17
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
	clrwdt
	nop
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
	bcf	INTCON, GIE
	lcall	psx_xfer_byte
	lcall	psx_ack_wait
	bsf	INTCON, GIE
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
	bcf	INTCON, GIE
	lcall	psx_xfer_byte
	bsf	INTCON, GIE
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

	;; 1. Detect presence: Send a POLL, verify that40/f3 the
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

	decfsz	debounce_count, f
	goto	controller_debounce_loop
		
	;; 2. At this point, we know that at the very least
	;;    we have a functioning digital controller. Set CONNECTED,
	;;    and clear other flags like POWERDOWN.
	;;
	;;    This also happens to be a secondary entry point used
	;;    to take a controller out of power-down mode.
cdetect_try_analog

	fsr_slot_state
	movlw	(1 << STATE_CONNECTED_BIT)
	movwf	INDF
	movwf	next_state_on_error
			
	;; 3. Try to enter analog mode.

	movlw	0x01
	lcall	controller_set_major_mode

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
	
	;; 6. Specify the polling result format to include all channels on the
	;;    Dual Shock controller. We can actually ask the controller to give us
	;;    a bitmap of supported channels (with CMD_GET_AVAILABLE_POLL_RESULTS)
	;;    but that's kind of pointless for us since we wouldn't know what to
	;;    do with any extra channels if the controller reported any.
	;;
	;;    This enables the first 18 reply channels, which includes everything
	;;    the Dual Shock controller has (including all pressure-sensitive buttons)

	lcall	psx_begin
 	controller_xfer_expect	PSX_ADDRESS_CONTROLLER_1, 0xFF
 	controller_xfer_expect	PSX_CMD_SET_POLL_RESULT_FORMAT, PSX_MODE_ESCAPE | 0x03
 	controller_xfer_expect	0x00, PSX_PADDING
 	controller_xfer		0xFF	; Bitmap: enable first 18 responses
 	controller_xfer		0xFF	;         ...
 	controller_xfer		0x03	;         ...
 	controller_xfer		0x00	;         ...
 	controller_xfer		0x00	;         ...
 	controller_xfer_no_ack	0x00	; (Unused)
	lcall	psx_end_wait

	;; 7. Initialize the pressure-sensitive buttons. This consists of sending one
	;;    command per button, but the command contents are not yet understood.
	;;
	;;    Note that this command sequence is not actually necessary in order to
	;;    use the pressure-sensitive buttons, but we might as well issue it in
	;;    order to make sure any internal parameters for the pressure sensitive
	;;    buttons are at least in a known state.

	clrf	packet_iter
pressure_sensor_init_loop

	lcall	psx_begin
 	controller_xfer_expect	PSX_ADDRESS_CONTROLLER_1, 0xFF
 	controller_xfer_expect	PSX_CMD_INIT_PRESSURE, PSX_MODE_ESCAPE | 0x03
 	controller_xfer_expect	0x00, PSX_PADDING
 	controller_xfer_f	packet_iter	; Button number 
 	controller_xfer		0xFF		; (unknown)
 	controller_xfer		0x02		; (unknown)
 	controller_xfer		0x00		; (unknown)
 	controller_xfer		0x00		; (unknown)
 	controller_xfer_no_ack	0x00		; (unknown)
	lcall	psx_end_wait

	incf	packet_iter, f		; Loop over all buttons
	movlw	NUM_PRESSURE_BUTTONS
	xorwf	packet_iter, w
	btfss	STATUS, Z
	goto	pressure_sensor_init_loop

	;; 8. Test all the setup work so far by issuing another poll and checking
	;;    the mode and reply length. We should have a reply length of 18 bytes
	;;    (0x09) and the mode PSX_MODE_ANALOG.
	
	movlw	0x00			; Exit escape mode
	lcall	controller_escape

	lcall	psx_begin
 	controller_xfer_expect	PSX_ADDRESS_CONTROLLER_1, 0xFF
 	controller_xfer_expect	PSX_CMD_POLL, PSX_MODE_ANALOG | 0x09
 	controller_xfer_expect	0x00, PSX_PADDING
	lcall	psx_end_wait

	;; 9. We've made it all the way! Identify this controller with
	;;    the DUALSHOCK bit, and return.
	
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
	;; Set the controller's major mode (analog/digital).
	;;
	;; This has the side-effect of resetting most controller
	;; state, such as the pressure-sensitive buttons and
	;; force feedback.
	;; 
	;; Inputs:
	;;	w=0x00 for digital, 0x01 for analog.
	;;
	;; Side effects:
	;;	Modifies 'temp', 'temp2', and 'temp3'
	;;
controller_set_major_mode
	movwf	temp3

	movlw	0x01			; Enter escape mode
	lcall	controller_escape

	lcall	psx_begin		; Set up analog mode
 	controller_xfer_expect	PSX_ADDRESS_CONTROLLER_1, 0xFF
 	controller_xfer_expect	PSX_CMD_SET_MAJOR_MODE, PSX_MODE_ESCAPE | 0x03
 	controller_xfer_expect	0x00, PSX_PADDING
 	controller_xfer_f	temp3
 	controller_xfer		0x03	; (unknown)
 	controller_xfer		0x00	; (unknown)
 	controller_xfer		0x00	; (unknown)
	controller_xfer		0x00	; (unknown)
 	controller_xfer_no_ack	0x00	; (unknown)
	lcall	psx_end_wait

	movlw	0x00			; Exit escape mode
	lgoto	controller_escape
	

	;; ----------------------------------------------------
	;; 
	;; Enter low-power mode. This switches the controller
	;; into normal analog mode without force-feedback or
	;; pressure-sensitive buttons. This is necessary in order
	;; for Logitech's wireless controllers to power themselves
	;; off.
	;;
	;; If the controller is not dual-shock, powerdown mode has
	;; no effect other than setting STATE_POWERDOWN_BIT.
	;;
	;; Note that controller_exit_powerdown is actually part
	;; of controller_detect. It performs most of the controller
	;; detection procedure, aside from the time consuming
	;; debouncing.
	;;
	;; Inputs:
	;;	current_slot
	;;
	;; Outputs:
	;;	slot_state
	;; 
controller_enter_powerdown
	fsr_slot_state
	bsf	INDF, STATE_POWERDOWN_BIT
	btfss	INDF, STATE_DUALSHOCK_BIT
	return

	clrf	next_state_on_error
	bcf	INDF, STATE_DUALSHOCK_BIT
	if	POWERDOWN_MODE == 0x00
	 bcf	INDF, STATE_ANALOG_BIT
	endif
	
	lcall	psx_end_wait		; Delay after polling

	movlw	POWERDOWN_MODE
	lgoto	controller_set_major_mode


	;; ----------------------------------------------------
	;; 
	;; Exit low-power mode.
	;;
	;; Inputs:
	;;	current_slot
	;;
	;; Outputs:
	;;	slot_state
	;; 
controller_exit_powerdown
	lcall	psx_end_wait		; Delay after polling
	goto	cdetect_try_analog

		
	;; ----------------------------------------------------
	;; 
	;; Poll the current controller.
	;; 
	;; Sends current actuator values to the controller, and
	;; generates a packet on the TX line containing the
	;; controller's current status. This TX packet has very
	;; low latency (~1 bit period).
	;; 
	;; Automatically connects/disconnects the controller as
	;; necessary.
	;;
	;; Inputs:
	;;	current_slot
	;;	slot_actuators
	;;	slot_state
	;;
	;; Outputs:
	;;	slot_state
	;;      ACTIVITY_FLAG
	;; 
controller_poll
	bcf	ACTIVITY_FLAG
	
	;; Not connected? Try to connect.
	fsr_slot_state
	btfss	INDF, STATE_CONNECTED_BIT
	call	controller_detect

	;; Send the TX packet's header
	movf	current_slot, w	
	iorlw	TX_START_OF_PACKET
	lcall	tx_byte				; 1. Start of packet + slot number
	fsr_slot_state
	movf	INDF, w
	lcall	tx_byte				; 2. Current state

	;; We're done if the controller is disconnected
	fsr_slot_state
	btfss	INDF, STATE_CONNECTED_BIT
	return

	;; Figure out how many bytes to expect
	movlw	.2				; Plain digital controller
	btfsc	INDF, STATE_ANALOG_BIT
	movlw	.6				; Plain analog controller
	btfsc	INDF, STATE_DUALSHOCK_BIT
	movlw	.18				; Dual shock controller
	movwf	byte_count
	clrf	packet_iter			; Data byte counter, for activity detect
	
	;; Set up our error handling behaviour, (disconnect on error) and begin the PSX packet
	clrf	next_state_on_error
	lcall	psx_begin

	;; Send the PSX polling command
	controller_xfer		PSX_ADDRESS_CONTROLLER_1
	controller_xfer		PSX_CMD_POLL
	controller_xfer		0x00

	bankisel slot_actuators			; Point INDF at the first actuator for this slot 
	movlw	slot_actuators
	btfsc	current_slot, 0
	addlw	MAX_ACTUATORS_LEN
	movwf	FSR

	
cpoll_byte_loop

	movf	INDF, w				; Next actuator byte
	movwf	cmd_byte
	incf	FSR, f
	
	bcf	INTCON, GIE
	lcall	psx_xfer_tx_byte
	
	decfsz	byte_count, f			; Done yet?
	goto	cpoll_check_for_ack
	goto	cpoll_done

cpoll_check_for_ack
	lcall	psx_ack_wait			; Only check for ACK if this isn't the last byte
	bsf	INTCON, GIE
	btfss	STATUS, C
	goto	controller_error

	;; While we're here, detect button activity. If either of the first
	;; two bytes aren't 0xFF, a button is down. If a button is down, reset
	;; the controller's inactivity timer.
cpoll_activity_detect
	btfsc	packet_iter, 1			; First two bytes?
	goto	cpoll_byte_loop			;  ... No. 
	incf	packet_iter, f

	incf	reply_byte, w			; Is a button down?
	btfss	STATUS, Z
	bsf	ACTIVITY_FLAG			; Yes. Set ACTIVITY_FLAG.
	goto	cpoll_byte_loop			; Done with activity check.

cpoll_done	
	bsf	INTCON, GIE
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
	;; Low level bit-banged I/O
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
	;;	cmd_byte
	;;	psx_port_latch (must be (PSX_PORT & PSX_OTHER_MASK) + CLK_MASK)
	;;
	;; Outputs:
	;;	reply_byte
	;; 
psx_xfer_bit_m	macro	dat_p, dat_b

	movf	psx_port_latch, w	; H(7)   Prepare new PSX_PORT value
	rrf	cmd_byte, f		; H(8)   Transfer bit from cmd_byte to CMD_MASK 
	btfss	STATUS, C		; H(9)     (inverted)
	iorlw	CMD_MASK		; H(10) 
	movwf	PSX_PORT		; L(1)   Falling edge of CLK, output CMD bit
	goto	$+1			; L(3) 
	goto	$+1			; L(5)
	goto	$+1			; L(7) 
	goto	$+1			; L(9) 
	clrc				; L(10) 
	bcf	PSX_PORT, CLK_BIT	; H(1)   Rising edge of CLK
	btfsc	dat_p, dat_b		; H(2)   Sample DAT bit into C
	setc				; H(3) 
	rrf	reply_byte, f		; H(4)   Transfer bit from C into reply_byte

	clrwdt					; H(5)
	nop					; H(6) 
	
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
psx_xfer_byte_m	macro	dat_p, dat_b
	psx_xfer_bit_m	dat_p, dat_b
	psx_xfer_bit_m	dat_p, dat_b
	psx_xfer_bit_m	dat_p, dat_b
	psx_xfer_bit_m	dat_p, dat_b
	psx_xfer_bit_m	dat_p, dat_b
	psx_xfer_bit_m	dat_p, dat_b
	psx_xfer_bit_m	dat_p, dat_b
	psx_xfer_bit_m	dat_p, dat_b
	endm


	;; ----------------------------------------------------
	;; 
	;; Transfer one bit in/out via one of the Playstation ports,
	;; while simultaneously outputting a copy of it on the differential-drive
	;; TX port.
	;;
	;; This macro has all the timing constraints of psx_xfer_bit_m,
	;; with the additional constraint that the TX signal must be
	;; a valid asynchronous serial (8-N-1) waveform at 250 kbaud.
	;; This macro can basically choose an arbitrary point in time
	;; to output its TX bit, as long as that is consistent with the
	;; timing of our start and stop bits.
	;;
	;; Note that this macro actually transmits the *previous* bit
	;; via TX. This means that when we're receiving bit 0, we're
	;; transmitting the start bit. After all bits have been received,
	;; we still need to transmit both bit 7 and the stop bit.
	;; 
	;; Inputs:
	;;	cmd_byte
	;;	psx_port_latch (must be (PSX_PORT & PSX_OTHER_MASK) + CLK_MASK)
	;;
	;; Outputs:
	;;	reply_byte
	;; 
psx_xfer_tx_bit_m	macro	dat_p, dat_b

	movf	psx_port_latch, w		; H(7)   Prepare new PSX_PORT value
	rrf	cmd_byte, f			; H(8)   Transfer bit from cmd_byte to CMD_MASK 
	btfss	STATUS, C			; H(9)     (inverted)
	iorlw	CMD_MASK			; H(10) 
	movwf	PSX_PORT			; L(1)   Falling edge of CLK, output CMD bit

	andlw	~(TX_MARK_MASK | TX_SPACE_MASK)	; L(2)   Convert the previous DAT bit to a TX mark/space 
	btfss	reply_byte, 7			; L(3) 
	iorlw	TX_SPACE_MASK			; L(4)
	btfsc	reply_byte, 7			; L(5) 
	iorlw	TX_MARK_MASK			; L(6)
	movwf	PSX_PORT			; L(7)   Transmit the TX bit. Doesn't affect CLK/CMD. 

	andlw	~CMD_MASK			; L(8)   Update psx_port_latch with the new TX bits 
	movwf	psx_port_latch			; L(9)
	
	clrc					; L(10) 
	bcf	PSX_PORT, CLK_BIT		; H(1)   Rising edge of CLK
	btfsc	dat_p, dat_b			; H(2)   Sample DAT bit into C
	setc					; H(3) 
	rrf	reply_byte, f			; H(4)   Transfer bit from C into reply_byte

	clrwdt					; H(5)
	nop					; H(6) 

	endm

	
	;; ----------------------------------------------------
	;;
	;; Transfer one byte in/out via one of the Playstation ports,
	;; while simultaneously outputting a copy on the differential-drive
	;; TX port. This macro generates framing bits which mesh with
	;; the cycle timing used in psx_xfer_tx_bit_m.
	;;
	;; Inputs:
	;;	cmd_byte
	;;	reply_byte (Must be 0, for start bit)
	;;	psx_port_latch (must be (PSX_PORT & PSX_OTHER_MASK) + CLK_MASK)
	;;
	;; Outputs:
	;;	reply_byte
	;; 
psx_xfer_tx_byte_m	macro	dat_p, dat_b	
	psx_xfer_tx_bit_m	dat_p, dat_b	; Read bit 0, TX start bit 
	psx_xfer_tx_bit_m	dat_p, dat_b	; Read bit 1, TX bit 0
	psx_xfer_tx_bit_m	dat_p, dat_b	; Read bit 2, TX bit 1
	psx_xfer_tx_bit_m	dat_p, dat_b	; Read bit 3, TX bit 2
	psx_xfer_tx_bit_m	dat_p, dat_b	; Read bit 4, TX bit 3
	psx_xfer_tx_bit_m	dat_p, dat_b	; Read bit 5, TX bit 4
	psx_xfer_tx_bit_m	dat_p, dat_b	; Read bit 6, TX bit 5
	psx_xfer_tx_bit_m	dat_p, dat_b	; Read bit 7, TX bit 6

	;; TX bit 7, using a copy of psx_xfer_tx_bit with PSX I/O removed

	movf	psx_port_latch, w		; H(7)   Prepare new PSX_PORT value
	andlw	~CLK_MASK			; H(8)   Keep CLK high
	goto	$+1				; H(10)
	nop					; L(1)

	andlw	~(TX_MARK_MASK | TX_SPACE_MASK)	; L(2)   Convert the previous DAT bit to a TX mark/space 
	btfss	reply_byte, 7			; L(3) 
	iorlw	TX_SPACE_MASK			; L(4)
	btfsc	reply_byte, 7			; L(5) 
	iorlw	TX_MARK_MASK			; L(6)
	movwf	PSX_PORT			; L(7)   Transmit the TX bit (Also brings CMD high)

	andlw	~(CMD_MASK | CLK_MASK)		; L(8)   Update psx_port_latch with the new TX bits 
	movwf	psx_port_latch			; L(9)      (Let it clear CLK this time, we want it high)
	
	clrwdt 					; L(10) 
	goto	$+1				; H(2)
	goto	$+1				; H(4)
	goto	$+1				; H(6)

	;; TX stop bit, using a copy of psx_xfer_tx_bit with PSX I/O
	;; removed and which always outputs a Mark.
	
	goto	$+1				; H(8)   Keep CLK high
	goto	$+1				; H(10)
	movf	psx_port_latch, w		; L(1)   Prepare new PSX_PORT value

	andlw	~(TX_MARK_MASK | TX_SPACE_MASK)	; L(2)   Always output a Mark
	iorlw	TX_MARK_MASK			; L(3)
	goto	$+1				; L(5)
	nop					; L(6) 
	movwf	PSX_PORT			; L(7)   Transmit the TX bit
		
	endm


	;; ----------------------------------------------------
	;;
	;; Transfer one byte out via the TX port, without touching
	;; the Playstation ports. This is used to send header bytes
	;; on our high-speed serial line.
	;;
	;; Inputs:
	;;	Byte to send in 'w'
	;;
	;; Side effects:
	;;	Modifies psx_port_latch
	;;	Modifies 'temp' and 'temp2'
	;;
tx_byte
	movwf	temp2
	bcf	INTCON, GIE		; Interrupts off
	
	movf	PSX_PORT, w		; Prepare psx_port_latch for use
	andlw	~(TX_MARK_MASK | TX_SPACE_MASK)
	movwf	psx_port_latch

	clrc				; Put start bit into C
	
	movlw	.10			; Loop over 10 bits total (start, 8 data, stop)
	movwf	temp
tx_bit_loop				; This loop must be 20 cycles, but that's our only constraint.

	movf	psx_port_latch, w	; (1) Convert C to a mark/space 
	btfss	STATUS, C		; (2)
	iorlw	TX_SPACE_MASK		; (3)
	btfsc	STATUS, C		; (4) 
	iorlw	TX_MARK_MASK		; (5)
	movwf	PSX_PORT		; (6) Output the bit 

	goto	$+1			; (8)
	goto	$+1			; (10)
	goto	$+1			; (12)
	goto	$+1			; (14)
	clrwdt				; (15)

	setc				; (16) This will become our stop bit
	rrf	temp2, f		; (17) Next data/stop bit...
	decfsz	temp, f			; (18) 
	goto	tx_bit_loop		; (20) 

	bsf	INTCON, GIE		; Interrupts on
	return

			
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
	;; The psx_xfer_byte macro also initializes psx_port_latch.
	;;

psx_xfer_byte
	movf	PSX_PORT, w			; Prepare psx_port_latch
	andlw	PSX_OTHER_MASK
	iorlw	CLK_MASK | TX_MARK_MASK
	movwf	psx_port_latch
	
	pagesel	pxb_slot2
	btfsc	current_slot, 0
	goto	pxb_slot2
	psx_xfer_byte_m	SLOT1_DAT
	return

psx_xfer_tx_byte
	movf	PSX_PORT, w			; Prepare psx_port_latch
	andlw	PSX_OTHER_MASK
	iorlw	CLK_MASK | TX_MARK_MASK
	movwf	psx_port_latch

	clrf	reply_byte			; Clear reply_byte to initialize start bit
	
	pagesel	pxtb_slot2
	btfsc	current_slot, 0
	goto	pxtb_slot2
	psx_xfer_tx_byte_m	SLOT1_DAT
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
	psx_xfer_byte_m	SLOT2_DAT
	return

pxtb_slot2
	psx_xfer_tx_byte_m	SLOT2_DAT
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
