	;; ledboard.asm - Firmware for controlling 72 LEDs and an LCD
	;;                over a serial port. The protocol is extensible,
	;;                so one write-only serial bus may be shared between
	;;                multiple peripherals.
	;;
	;; ------------------------------------------------------------------
	;; 
	;; Copyright (C) 2002 Micah Dowty <micahjd@users.sourceforge.net>
	;; 
	;; This program is free software; you can redistribute it and/or
	;; modify it under the terms of the GNU General Public License
	;; as published by the Free Software Foundation; either version 2
	;; of the License, or (at your option) any later version.
	;; 
	;; This program is distributed in the hope that it will be useful,
	;; but WITHOUT ANY WARRANTY; without even the implied warranty of
	;; MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	;; GNU General Public License for more details.
	;; 
	;; You should have received a copy of the GNU General Public License
	;; along with this program; if not, write to the Free Software
	;; Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
	;;
	;; ------------------------------------------------------------------
	;;
	;; 20mHz oscillator, PIC16F84A
	;; 
	;; Clock-speed-sensitive code:
	;;  - delay_ms
	;;  - lcd_tinydelay
	;;  - All the cycle counting in get_char
	;;  - Animation speed of standby mode
	;; 
	;; The hardware for this project consists of a PIC16F84 running at 20mHz,
	;; 72 LEDs, 9 74HCT164 shift registers, and a total of 72 LEDs arranged in
	;; 9 banks of 8 each. The LEDs are connected to the outputs of the shift
	;; registers. Since there is no output enable or double buffering, the LED
	;; data will be invalid when loading the shift registers. This is made as
	;; unnoticeable as possible by loading the shift registers very fast. The PC
	;; software can adjust the refresh rate, balancing flicker and the shift
	;; register 'trails'.
	;;
	;; ---------- Ports:
	;; 
	;; Port A:
	;;  0. R/S for LCD, and data for the 9th shift register
	;;  1. LCD clock
	;;  2. Shift register clock
	;;  3. Shift register reset
	;;  4. Serial data in
	;;
	;; Port B:
	;;  8-bit bus shared between LCD data and shift register data
	;;
	;; ---------- LED banks:
	;;
	;;  1-5. Grid of 5x8 red LEDs, from left to right
	;;  6. A row of green LEDs along the bottom of the board, in an arc
	;;  7. A row of yellow LEDs along the top of the board, in an arc
	;;  8. A blue and white LED in each corner
	;;  9. An external board mounted on the side of the case
	;;
	;; ---------- Protocol:
	;;
	;; The serial protocol for this project is a little weird... It has the
	;; following goals:
	;;  1. Allow multiple devices to share the same bus
	;;  2. Make the PIC software simpler
	;;  3. Recover from errors, and allow hotplugging to the serial stream
	;;
	;; To make handling PWM and serial input easier on the PIC, the PC
	;; sends a continuous stream of data to the PIC. Any packets that are
	;; not directed at it are ignored, and the rest are decoded. If the stream
	;; of data is interrupted, the device goes back into a standby mode until it
	;; is resumed. The PC software should be designed to progressively resynchronize
	;; the device with the PC's representation of it. With each update, the PC software
	;; can send a different chunk of data that the device should already have, but maybe
	;; doesn't due to line noise or disconnection.
	;; 
	;; 57600bps, 8-N-1
	;; 
	;; Byte format:
	;;   IIISDDDD
	;; I: device ID
	;; S: 0 here indicates the first byte of a packet, 1 indicates continuation
	;; D: a nibble of data to build the packet
	;;
	;; The device ID is specified in the defines below, and allows unique identification
	;; of a device on this serial line. There may be more devices IDs than physical
	;; devices, if one microcontroller controls multiple different peripherals.
	;;
	;; Data in the packet is sent high nibble first, the content is device-dependent.
	;;
	;; ---------- Devices implemented:
	;;
	;; Device 0:	No-op. Packets sent here are ignored
	;; 
	;; Device 2:	Control bits. (Shared with the clock device)
	;;			0. LCD data mode
	;; 
	;; Device 4:	16x2 top-panel LCD data. Packets should be 1 byte,
	;;		sent directly to the LCD. Rate should
	;;		be limited to one byte per millisecond
	;; 
	;; Device 5:	LED register set packet. The first byte is the address
	;;		to write to, the following bytes are written to the LED registers
	;;		starting at that address.
	;;
	;;	LED registers:
	;;		0x00-0x23:	LED brightnesses, packed 2 per byte
	;;		     0x24:	Maximum value for the PWM register
	;;		     0x25:	Reset value of the PWM register
	;;		     0x26:	This is added to all LED brightness values before use
	;;		     0x27:	The PWM register itself
	;;		     0x28:	The amount to increment the PWM register by
	;;		     0x29:	Maximum LED refresh index (zero to disable LED refresh)
			
	;; ------------------------------------------------ Defines
	
	PROCESSOR 16F84
	#include "p16f84.inc"
	
	__CONFIG _CP_OFF & _WDT_OFF & _PWRTE_ON & _HS_OSC

LCD_RS		EQU 0	; Port A pins
SR9		EQU 0
LCD_CLK		EQU 1
SR_CLK		EQU 2
SR_RST		EQU 3
SERIAL_PIN	EQU 4

	;; LCD commands
clear_lcd	EQU 0x01	; Clear Display
return_home	EQU 0x02	; Cursor to Home position
entry_mode	EQU 0x06	;  Normal entry mode
entry_mode_shift EQU 0x07	; - with shift
system_set_8_bit EQU 0x38	; 8 bit data mode 2 line ( 5x7 font )
system_set_4_bit EQU 0x28	; 4 bit data mode 2 line ( 5x7 font )
display_on	EQU 0x0c	; Switch ON Display
display_off	EQU 0x08	; Cursor plus blink
set_dd_ram	EQU 0x80	; Line 1 position 1
set_cg_ram	EQU 0x40	; Beginning of CGRAM
cursor_on	EQU 0x0E	; Switch Cursor ON
cursor_off	EQU 0x0C	; Switch Cursor OFF

	;; ------------------------------------------------ Variables

	;; We have 68 general-purpose registers, starting at 0x0C.
	
	cblock 0x0C		
		;; First we have the LED registers documented above.
		;; This accounts for 42 bytes
		ledreg_brights:24 
		ledreg_pwm_max
		ledreg_pwm_min
		ledreg_add
		ledreg_pwm
		ledreg_increment
		ledreg_refresh
	
		;; We have room for 10 bytes here before we hit the buffers below.
		;; Variables on the same line here share the same memory space.
		led_refresh_index
		delay_tmp1:0,	ser_tmp1:0,	pkt_tmp:0,	led_bit_tmp:0,	led_sine_result
		delay_tmp2:0,	ser_bit_tmp:0,	standby_led_index
		led_tmp
		char_count:0,	ser_byte
		pkt_byte:0,	standby_frame
		pkt_nibbles:0,	standby_msg_timer
		led_reg_addr:0, standby_frame_timer
		control_packet:0,	lcd_msg_addr
		flags:0
	
		;; Two buffers to hold LED samples before shifting them out
		buffer_a:8
		buffer_b:8
	endc

	;; Some flags...
	#define led_high_nibble	flags,7
	#define led_sine_lsb	flags,6
		
	;; ------------------------------------------------ Entry points
	
	org 0x00		; Reset vector
	goto main
	
	org 0x04		; Interrupt vector
	goto main

	
	;; ------------------------------------------------ Initialization
		
main
	bsf	STATUS,RP0	; Initialize port direction
	movlw	0xF0		; Serial data is an input,
	movwf	TRISA		; everything else is output.
	clrf	TRISB
	movlw	b'11001000'	; TMR0 on, running at the instruction cycle speed
	movwf	OPTION_REG
	bcf	STATUS,RP0
	clrf	PORTA		; Clear ports (holding LEDs in reset)
	clrf	PORTB
	movlw	0x0C		; Clear data memory
	movwf	FSR
memclr
	clrf	INDF
	incf	FSR,F
	movlw	0x30
	subwf	FSR,W
	btfss	STATUS,Z
	goto	memclr

	call	lcd_init
	call	led_init
	
	goto	standby_mode

	;; ------------------------------------------------ LCD utilities
	
	;; Initialize our 16x2 LCD module
lcd_init
	;; First a reset
	movlw	50		 ; Waste 50 ms
	call	delay_ms
	movlw	system_set_8_bit ; Send 8-bit bus initialization
	call	lcd_char
	movlw	5		 ; Wait 5 ms
	call	delay_ms
	movlw	system_set_8_bit ; Repeat a few times...
	call	lcd_char
	movlw	5
	call	delay_ms
	movlw	system_set_8_bit
	call	lcd_char
	movlw	5
	call	delay_ms

	;; Now initialize
	movlw	system_set_8_bit
	call	lcd_char
	call	lcd_char_delay
	movlw	display_off
	call	lcd_char
	call	lcd_char_delay
	movlw	entry_mode
	call	lcd_char
	call	lcd_char_delay
	movlw	display_on
	call	lcd_char
	call	lcd_char_delay
	movlw	set_dd_ram
	call	lcd_char
	call	lcd_char_delay

	;;  Enter data mode
	bsf	PORTA,LCD_RS
	bsf	control_packet,3
	return

	;; Send one character to the LCD (in W)
lcd_char
	movwf	PORTB
	call	lcd_tinydelay
	bsf	PORTA,LCD_CLK
	call	lcd_tinydelay
	bcf	PORTA,LCD_CLK
	return

	;; Send a character, but in command mode not data mode
lcd_cmd
	bcf	PORTA,LCD_RS
	call	lcd_char
	bsf	PORTA,LCD_RS
	return

	;; Tiny delay for LCD clock
lcd_tinydelay
	nop
	nop
	nop
	nop
	return

	;; Delay to use between characters
lcd_char_delay
	movlw	1
	goto	delay_ms

	;; Display the next message on the LCD
lcd_message
	movlw	3		; Table starts at 0x300
	movwf	PCLATH
	movlw	set_dd_ram	; Go to line 1
	call	lcd_cmd
	call	lcd_char_delay
	call	lcd_message_line
	movlw	set_dd_ram+40	; Go to line 2
	call	lcd_cmd
	call	lcd_char_delay
	call	lcd_message_line
	return
	
	;; Display one line of the message, at the current location
	;; (used internally by lcd_message)
lcd_message_line
	movlw	0x10
	movwf	char_count
lcd_message_char
	movf	lcd_msg_addr,W
	call	lcd_message_table ; Get a char
	call	lcd_char	; Display it
	call	lcd_char_delay
	incf	lcd_msg_addr,F
	decfsz	char_count,F	; Next
	goto	lcd_message_char
	return
		
	;; Make a lookup from the LCD message table (stored at 0x300)
	;; Assumes PCLATH is already set to 3
lcd_message_table
	movwf	PCL
	
	;; ------------------------------------------------ General-purpose utilities
	
	;; Delay for W milliseconds, assuming 20mhz processor speed
delay_ms
	movwf	delay_tmp1
delay_loop1
	movlw	0xAC
	movwf	delay_tmp2
	nop
	nop
delay_loop2
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	decfsz	delay_tmp2,F
	goto	delay_loop2
	nop
	decfsz	delay_tmp1,F
	goto	delay_loop1
	return

	;; ------------------------------------------------ Standby-mode tables

	;; First, a data table to define the distance of each LED from the center.
	;; This is in the same order the red LEDs are in memory, with nibble-wide
	;; distances packed into bytes.
dist_table
	clrf	PCLATH
	addwf	PCL,F
	dt	0x43, 0x22, 0x22, 0x34
	dt	0x32, 0x11, 0x11, 0x23
	dt	0x32, 0x10, 0x01, 0x23
	dt	0x32, 0x11, 0x11, 0x23
	dt	0x43, 0x22, 0x22, 0x34
	
	;; Now the table of brightnesses indexed by distance + frame.
	;; Two values packed into each byte
bright_table
	clrf	PCLATH
	addwf	PCL,F
        dt      0x33, 0x33, 0x33, 0x33
        dt      0x33, 0x34, 0x44, 0x44
        dt      0x44, 0x44, 0x44, 0x44
        dt      0x44, 0x44, 0x44, 0x44
        dt      0x44, 0x44, 0x44, 0x44
        dt      0x44, 0x44, 0x44, 0x44
        dt      0x44, 0x44, 0x44, 0x33
        dt      0x33, 0x33, 0x33, 0x33
        dt      0x32, 0x22, 0x22, 0x22
        dt      0x22, 0x21, 0x11, 0x11
        dt      0x11, 0x11, 0x11, 0x11
        dt      0x11, 0x11, 0x11, 0x11
        dt      0x11, 0x11, 0x11, 0x11
        dt      0x11, 0x11, 0x11, 0x11
        dt      0x11, 0x11, 0x11, 0x22
        dt      0x22, 0x22, 0x22, 0x22
	
	;; ------------------------------------------------ LED driver routines

	org 0x100		; Keep within this segment, since we do a lot of jump tables...
	
	;; Supply default values for all the LED registers
led_init
	movlw	ledreg_brights	; Clear the brightness registers
	movwf	FSR
led_init_brights_loop
	clrf	INDF
	incf	FSR,F
	movlw	ledreg_pwm_max
	subwf	FSR,W
	btfss	STATUS,Z
	goto	led_init_brights_loop
	movlw	0x00		; Set up PWM parameters
	movwf	ledreg_pwm_min
	movlw	0x0F
	movwf	ledreg_pwm_max
	clrf	ledreg_add
	clrf	ledreg_pwm
	movlw	0x01
	movwf	ledreg_increment
	movlw	0x25
	movwf	ledreg_refresh
	return
	
	;; This is called while waiting for bits in get_char. Use it
	;; to refresh the LED grid. We only have about 80 cycles here, so
	;; do it in small pieces.
	;;
	;; We will generate buffer_a and buffer_b from all the ledreg_* settings
	;; two LEDs at a time, then after that's done with, blast buffer_a and
	;; buffer_b out to the shift registers.
led_refresh
	movf	ledreg_refresh,W ; Is refresh disabled?
	btfsc	STATUS,Z
	return
	
	subwf	led_refresh_index,W ; See if it's time to finish up this cycle
	btfsc	STATUS,C
	goto	led_refresh_fin	; If led_refresh_index > ledreg_refresh

	movlw	0x24		; If we're past the end of our LED table, ignore this refresh
	subwf	led_refresh_index,W
	btfsc	STATUS,C
	goto	refresh_done

	btfss	led_high_nibble
	goto	low_nibble
	
	movf	led_refresh_index,W ; Get the high nibble of this brightness byte
	addlw	ledreg_brights
	movwf	FSR
	swapf	INDF,W
	call	led_refresh_one
	bcf	led_high_nibble	; Now do the next byte
refresh_done
	incf	led_refresh_index,F
	return
	
low_nibble
	movf	led_refresh_index,W ; Now the low nibble
	addlw	ledreg_brights
	movwf	FSR
	movf	INDF,W
	bcf	led_high_nibble
	call	led_refresh_one
	bsf	led_high_nibble	; Now the high nibble
	return


	;; Take care of refreshing one LED. Brightness is specified in W, LED byte
	;; in led_refresh_index, and nibble in led_high_nibble.
led_refresh_one
	andlw	0x0F
	movwf	led_tmp		; Store the part of the brightness we need

	movlw	4		; The first four nibbles are for buffer_a, the rest for buffer_b
	subwf	led_refresh_index,W
	btfss	STATUS,C
	goto	led_refresh_buffer_a
	goto	led_refresh_buffer_b

	
	;; Refresh one LED in buffer_a. Brightness is in led_tmp,
	;; LED byte is in led_refresh_index, and nibble in led_high_nibble.
led_refresh_buffer_a
	rlf	led_refresh_index,W ; Calculate which buffer_a byte this is
	movwf	FSR
	bcf	FSR,0
	btfss	led_high_nibble
	bsf	FSR,0
	movlw	buffer_a
	addwf	FSR,F

	movlw	0x08		; Normal bits for PORTA
	movwf	INDF

	movf	led_tmp,W
	addwf	ledreg_add,W
	subwf	ledreg_pwm,W	; Now compare with the PWM register
	btfss	STATUS,C
	bsf	INDF,SR9
	return


	;; Same as above, but for one LED in buffer_b
	;; After subtracting 4, the low two bits of led_refresh_index, and
	;; led_high_nibble specify which byte in buffer_b, the rest specifies
	;; which bit. This gives a layout consistent with what we have in buffer_a,
	;; with banks of 8 LEDs grouped together.
led_refresh_buffer_b
	movlw	4		; Start calculating the address...
	subwf	led_refresh_index,W
	movwf	FSR
	rlf	FSR,F
	bcf	FSR,0
	btfss	led_high_nibble
	bsf	FSR,0		; Now FSR holds the LED number

	rrf	FSR,W		; Isolate the bit number times two into led_bit_tmp
	movwf	led_bit_tmp
	rrf	led_bit_tmp,W
	andlw	0x0E
	movwf	led_bit_tmp

	movlw	7		; Isolate the byte number and index into buffer_b
	andwf	FSR,W
	addlw	buffer_b
	movwf	FSR

	movlw	0x01		; Set page bits for this jump table
	movwf	PCLATH

	movf	led_tmp,W
	addwf	ledreg_add,W
	subwf	ledreg_pwm,W	; Now compare with the PWM register
	btfss	STATUS,C
	goto	buffer_b_set

	movf	led_bit_tmp,W	; Index into our table with the bit number * 2
	addwf	PCL,F		; Clear bits in this table...
	bcf	INDF,0
	return
	bcf	INDF,1
	return
	bcf	INDF,2
	return
	bcf	INDF,3
	return
	bcf	INDF,4
	return
	bcf	INDF,5
	return
	bcf	INDF,6
	return
	bcf	INDF,7
	return

buffer_b_set		
	movf	led_bit_tmp,W	; Index into our table with the bit number * 2
	addwf	PCL,F		; Set bits in this table...
	bsf	INDF,0
	return
	bsf	INDF,1
	return
	bsf	INDF,2
	return
	bsf	INDF,3
	return
	bsf	INDF,4
	return
	bsf	INDF,5
	return
	bsf	INDF,6
	return
	bsf	INDF,7
	return
		
	
	;; Finish one cycle of LED refresh
led_refresh_fin
	call	led_shift8	; Update the LEDs
	clrf	led_refresh_index
	bcf	led_high_nibble
	movf	ledreg_increment,W ; Step the PWM register forward...
	addwf	ledreg_pwm,F
	movf	ledreg_pwm_max,W ; Check the PWM register upper limit
	subwf	ledreg_pwm,W
	btfss	STATUS,C
	return
	movf	ledreg_pwm_min,W ; Reset it
	movwf	ledreg_pwm
	return


	;; Shift in one value to all shift registers.
	;; Takes 5 cycles
led_shift1 macro i
	movf	buffer_a+i,W	; Copy from BufferA to PORTA
	movwf	PORTA
	movf	buffer_b+i,W	; Copy from buffer_b to PORTB
	movwf	PORTB
	bsf	PORTA,SR_CLK	; Set the clock (it was cleared by writing to PORTA earlier)
	endm


	;; Shift in all values to all shift registers, from buffer_a and buffer_b
	;; This takes 43 cycles
led_shift8
	bcf	PORTA,SR_RST	; Reset the shift registers first
	led_shift1 0
	led_shift1 1
	led_shift1 2
	led_shift1 3
	led_shift1 4
	led_shift1 5
	led_shift1 6
	led_shift1 7
	return

	
	;; ------------------------------------------------ Serial reciever
	
	;; This serial reciever is tuned for 57600bps using a 20mHz crystal

	org	0x200
		
get_char
	clrf	ser_tmp1
start_bit_wait
	btfsc	PORTA,SERIAL_PIN ; Wait for start bit
	goto	got_start_bit
	incfsz	ser_tmp1,F
	goto	start_bit_wait
	goto	standby_mode	; If we don't recieve it in a while, go to standby

got_start_bit	
	movlw	8		; Prepare to recieve a byte
	movwf	ser_bit_tmp
	clrf	ser_byte

	movlw	0x0C		; Sample bits in the middle-
	movwf	ser_tmp1	; Waste about 40 cycles
bit_middle_delay
	decfsz	ser_tmp1,F
	goto	bit_middle_delay
	
bit_loop			; This loop must always total 96 cycles
	clrf	TMR0		; Start counting cycles (3 cycles into our total)

	;; Do something useful while waiting (Safe to add more
	;; here as long as it's all 84 cycles or less)
	call	led_refresh
	
	;; We now need to waste time until our counter hits 85. This is our target of 96
	;; cycles, minus the 3 it takes to load the timer, the 6 for the actual serial code
	;; below, and 3 cycles of overhead for this calculated jump.
	
	movlw	0x02
	movwf	PCLATH
	movf	TMR0,W
	addwf	PCL,F
	
	;; Big table of 83 nop's... If we're low on ROM space this could easily be converted
	;; into a delay loop, as long as it can have exact timing.
nop8	macro
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	endm

nop32	macro
	nop8
	nop8
	nop8
	nop8
	endm	

	nop32
	nop32
	nop8
	nop8
	nop
	nop
	nop
	
	;; 6 Cycles for loop and serial stuff
	rrf	ser_byte,F	; Transfer the serial port bit to ser_byte
	btfss	PORTA,SERIAL_PIN
	bsf	ser_byte,7
	decfsz	ser_bit_tmp,F
	goto	bit_loop
	
	;; Fall through to packet decode
	
	;; ------------------------------------------------ Packet decode
	
	;; Now decode the packet, during the stop bit. This should take no
	;; more than about 60 cycles because we must still be in the stop bit when
	;; this ends.
	
	btfss	ser_byte,4	; Is it a continuation packet?
	clrf	pkt_nibbles	; If not, begin a new packet
	movlw	0x0F		; Mask off the data from this incoming byte
	andwf	ser_byte,W
	iorwf	pkt_byte,F	; Store it in the low nibble of pkt_byte
	incf	pkt_nibbles,F	; Increment count of nibbles recieved
	btfss	pkt_nibbles,0	; Rotate into high nibble if it's an odd nibble
	goto	no_nibble_rotate
	swapf	pkt_byte,F
no_nibble_rotate
	
	rrf	ser_byte,W	; Mask off the device number
	movwf	pkt_tmp
	swapf	pkt_tmp,W
	andlw	0x07		; Mask off lower 3 bits
	addwf	PCL,F		; Jump table, indexed by device number
	goto	decode_done	; 0. No-op
	goto	decode_done	; 1.
	goto	pkt_control	; 2. control bits
	goto	decode_done	; 3.
	goto	pkt_lcd		; 4. LCD data
	goto	pkt_led		; 5. LED register set
	goto	decode_done	; 6.
	goto	decode_done	; 7.

	;; We return here after packet decoding. We should be inside
	;; the stop bit by now, so make sure it's a stop bit. If not,
	;; wait for the beginning of the stop bit to resynchronize.
	;; This works for synchronizing to streams of zero bytes, which
	;; is what we'll be recieving most of the time.
decode_done
	btfss	PORTA,SERIAL_PIN
	goto	get_char	; Yep, we have a stop bit
	clrf	ser_tmp1
stop_bit_wait
	btfss	PORTA,SERIAL_PIN ; Wait for stop bit
	goto	get_char	; Got it
	incfsz	ser_tmp1,F
	goto	stop_bit_wait
	goto	standby_mode	; If we don't recieve it in a while, go to standby

	
	;; Decode a one-byte LCD data packet
pkt_lcd
	btfss	pkt_nibbles,1	; Wait until we have 2 nibbles
	goto	decode_done
	clrf	pkt_nibbles	; Reset packet nibble counter
	bcf	PORTA,LCD_RS	; Set the RS bit according to our control packet
	btfsc	control_packet,3
	movf	pkt_byte,W
	clrf	pkt_byte
	bsf	PORTA,LCD_RS
	call	lcd_char
	goto	decode_done

	;; Save it for later when we output to the LCD
pkt_control
	movf	ser_byte,W
	movwf	control_packet
	goto	decode_done	

	;; Output one or more bytes to the LED registers
pkt_led
	btfsc	pkt_nibbles,0	; Skip partial bytes
	goto	decode_done

	movlw	2
	subwf	pkt_nibbles,W	; If this is the first byte, set the address
	btfsc	STATUS,Z
	goto	pkt_led_address

	movlw	0x2A		; If it's past the end, reset to zero
	subwf	led_reg_addr,W
	btfsc	STATUS,C
	clrf	led_reg_addr
	
	movlw	0x0C		; Index into the LED registers
	addwf	led_reg_addr,W
	movwf	FSR
	movf	pkt_byte,W
	movwf	INDF
	
	incf	led_reg_addr,F
	clrf	pkt_byte
	goto	decode_done

	;; Handle the first byte of an LED packet- set the address
pkt_led_address
	movf	pkt_byte,W
	movwf	led_reg_addr
	clrf	pkt_byte
	goto	decode_done

	
	 ;; ------------------------------------------------ Standby-mode code

	;; This function performs the per-LED animation calculations for the standby-mode effect.
	;; The distance from LED to center is supplied in W, the resulting brightness should
	;; be returned in led_sine_result. The animation frame is in standby_frame.
led_sine
	movwf	led_sine_result	; Use this as temp space for the distance
	swapf	led_sine_result,F ; Scale the distance * 16, decreasing the sine wave period
	movf	standby_frame,W	; Add the frame number
	subwf	led_sine_result,F

	bcf	led_sine_lsb	; Copy the least significant bit for later
	btfsc	led_sine_result,0
	bsf	led_sine_lsb
	
	rrf	led_sine_result,W ; Get the byte number in our table
	andlw	0x3F		; Loop inside the 128-entry table
	call	bright_table
	movwf	led_sine_result
	
	btfss	led_sine_lsb	; Get the nibble we want
	swapf	led_sine_result,F ; The caller will mask it to 4 bits
	return
	
	
	;; This is the actual standby loop logic
	;; 
standby_mode
	clrf	lcd_msg_addr	; Initialize the LEDs and LCDs to the state we want
	clrf	standby_frame
	call	led_init
	movlw	0xF0
	movwf	ledreg_brights+1 ; Bottom blue LED on side panel on
	movwf	ledreg_brights+2 ; Middle blue LED on side panel on
	movlw	0xFF
	movwf	ledreg_brights	; White LED and bottom red LED on
	movwf	ledreg_brights+3 ; Top blue LED and top red LED on

standby_msg_loop		; Loop once every several seconds, to put a new message on the LCD
	call	lcd_message
	movlw	0x20
	movwf	standby_msg_timer

standby_animation_loop		; Loop pretty frequently to animate the LEDs
	movlw	0x0A		; Control the animation speed
	movwf	standby_frame_timer
	clrf	standby_led_index
standby_animation_led		; Inner loop for calculating the value of two LEDs (one byte)
		
	movf	standby_led_index,W
	call	dist_table	; Look up the distance for these two LEDs
	movwf	led_tmp

	movf	standby_led_index,W
	addlw	ledreg_brights+4
	movwf	FSR		; Set up INDF to point to our LED brightness register
	clrf	INDF
	
	swapf	led_tmp,W
	andlw	0x0F
	call	led_sine	; Do the sine table thing for the first LED
	swapf	led_sine_result,W
	andlw	0xF0		; Insert it into the first LED brightness nibble
	iorwf	INDF,F

	movf	led_tmp,W
	andlw	0x0F
	call	led_sine	; And again for the second LED..
	movf	led_sine_result,W
	andlw	0x0F
	iorwf	INDF,F
	
	incf	standby_led_index,F ; Loop to the next LED pair
	movlw	0x14
	subwf	standby_led_index,W
	btfss	STATUS,C
	goto	standby_animation_led	

standby_refresh_loop		; Inner loop to refresh LEDs
	call	led_refresh
	
	btfss	PORTA,SERIAL_PIN ; If we notice serial activity, act on it
	goto	no_serial_activity
	call	lcd_tinydelay
	btfsc	PORTA,SERIAL_PIN ; Just a glitch?
	goto	exit_standby
no_serial_activity
	
	movf	led_refresh_index,W ; Perform one complete refresh cycle
	btfss	STATUS,Z
	goto	standby_refresh_loop

	decfsz	standby_frame_timer,F ; Animate the LEDs
	goto	standby_refresh_loop
	incfsz	standby_frame,F
	goto	standby_animation_loop

	decfsz	standby_msg_timer,F ; Change messages on the LCD every so often
	goto	standby_animation_loop
	goto	standby_msg_loop
	
exit_standby
	clrf	lcd_msg_addr	; Reinitialize again before the serial loop
	call	led_init
	clrf	pkt_byte
	goto	get_char
			
	;; ------------------------------------------------ LCD text messages

	;; The last page in memory is exclusively for LCD text messages,
	;; cycled through during standby mode.
	;;
	;; At 32 characters each, we have room for exactly 8 messages.
	;;
	
	org	0x300

	dt	"---= Yoshi! =---"
	dt	"Waiting for data"
	
	dt	" ALL YOUR BASE  "
	dt	"ARE BELONG TO US"

	dt	" Someone set up "
	dt	"  us the bomb!  "

	dt	"     Got        "
	dt	"     Root?      "

	dt	"Linux, the best "
	dt	"kind of nukes.  "

	dt	"It's cute.. WHAT"
	dt	"THE HELL IS IT? "

	dt	"You may or may  "
	dt	"not have mail   "

	dt	"One ping to rule"
	dt	"them all.       "

	;; ------------------------------------------------ The End!
	end
	
