	;; clock.asm - Firmware for the wallclock module, with a 40x4 LCD,
	;;             beeper, and three LEDs.
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
	;; 10mHz oscillator, PIC16F84A
	;; 
	;; Clock-speed-sensitive code:
	;;  - delay_ms
	;;  - lcd_tinydelay
	;;  - led_decay constants
	;;  - All the cycle counting in get_char
	;; 
	;; ---------- Ports:
	;; 
	;; Port A:
	;;  0. Beeper (driven with a constant high, not a square wave)
	;;  1. Green LED (right)
	;;  2. Yellow LED (left)
	;;  3. Red LED (middle)
	;;
	;; Port B:
	;;  0. LCD data 0
	;;  1. LCD data 1
	;;  2. LCD data 2
	;;  3. LCD data 3
	;;  4. Enable 1 (top half)
	;;  5. Enable 2 (bottom half)
	;;  6. R/S
	;;  7. Serial data in
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
	;; Device 1:	40x4 clock LCD data. Packets should be 1 byte,
	;;		sent directly to the LCD in data mode. Rate should
	;;		be limited to one byte per millisecond
	;; 
	;; Device 2:	Control bits. Packet should be one nibble, with these bits:
	;;			0. LCD data mode
	;;			1. Beeper
	;;			2. Enable top half LCD
	;;			3. Enable bottom half LCD
	;; 
	;; Device 3:	Clock LEDs, Packets should be 3 bytes:
	;;			0. Yellow LED brightness
	;;			1. Red LED brightness
	;;			2. Green LED brightness
	
		
	;; ------------------------------------------------ Defines
	
	PROCESSOR 16F84
	#include "p16f84.inc"
	
	__CONFIG _CP_OFF & _WDT_OFF & _PWRTE_ON & _HS_OSC

BEEP_PIN	EQU 0		; Port A pins
LED_GREEN_PIN	EQU 1
LED_YELLOW_PIN	EQU 2
LED_RED_PIN	EQU 3

E1_PIN		EQU 4		; Port B pins
E2_PIN		EQU 5
RS_PIN		EQU 6
SERIAL_PIN	EQU 7

E1		EQU 0x10	; Enables used in emask
E2		EQU 0x20

LED_GREEN	EQU 0x02
LED_YELLOW	EQU 0x04
LED_RED		EQU 0x08
LED_ALL		EQU 0x0E
	
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

led_decay_speed	EQU 1		; Speed of LED decay during fading effect
led_decay_length EQU 40		; Smaller numbers give faster side-to-side flashing
led_decay_brightness EQU 0xFF	; Maximum brightness during the effect

	;; ------------------------------------------------ Variables
			
	cblock 0x0C
		emask		; Mask of enable pins to use
		led_yellow_pwm	; PWM settings for LEDs
		led_red_pwm
		led_green_pwm
		pwm_index
		string_number	; Indexes for the string table
		string_index
		ser_byte	; Byte recieved from serial port
		led_decay_drive	; Mask of LEDs to drive continuously during decay
		led_fx_index	; Index into led_fx_table
		pkt_byte	; Byte being assembled from packets
		pkt_nibbles	; Number of nibbles recieved so far 
		pkt_high	; High nibble from packet recieved (in low nibble position)
		pkt_low		; Low nibble from packet recieved
	
		;; Temporary vars
		lcd_tmp1
		lcd_tmp2
		lcd_tmp3
		delay_index
		delay_tmp1
		delay_tmp2
		led_decay1
		led_decay2
		led_decay3
		led_tmp1
		ser_bit_tmp
		pkt_tmp
	endc

	
	;; ------------------------------------------------ Entry points
		
	org 0x00		; Reset vector
	goto main

	org 0x04		; Interrupt handler
	goto main
	
	;; ------------------------------------------------ Data tables

	;; Table of LEDs to drive during the nifty flashing effects
led_fx_table
	movlw	0x00
	movwf	PCLATH
	movf	led_fx_index,W
	addwf	PCL,F
	dt	0,LED_YELLOW,0,LED_RED,0,LED_GREEN,0,0,0,0,LED_GREEN
	dt	0,LED_RED,0,LED_YELLOW,0,0,0,0,LED_YELLOW,0,LED_RED,0
	dt	LED_GREEN,0,0,0,0,LED_GREEN,0,LED_RED,0,LED_YELLOW,0
	dt	0,0,0,0,0,0,LED_RED,0,LED_GREEN | LED_YELLOW,0,0,0,0,0
	dt	LED_GREEN,LED_RED,LED_YELLOW,0,0,LED_YELLOW,LED_RED
	dt	LED_GREEN,0,0,LED_GREEN,LED_RED,LED_YELLOW,0,0,LED_YELLOW
	dt	LED_RED,LED_GREEN,0,0,0,0,LED_YELLOW | LED_RED | LED_GREEN
	dt	0,0,0,0,LED_GREEN,0,0,0,LED_YELLOW,0,0,0,LED_RED,0,0,0,0,0	
	dt	0xFF

	;; String jump table
string_table
	movlw	0x00
	movwf	PCLATH
	movf	string_number,W
	addwf	PCL,F
lcd_welcome_line1	EQU 0
	goto	lcd_welcome_line1_table
lcd_welcome_line2	EQU 1
	goto	lcd_welcome_line2_table
lcd_welcome_line3	EQU 2
	goto	lcd_welcome_line3_table
lcd_welcome_line4	EQU 3
	goto	lcd_welcome_line4_table
	
	;; Welcome message for LCD
lcd_welcome_line1_table
	movf	string_index,W
	addwf	PCL,F
	dt	"LCD Clock Interface v2, Micah Dowty",0
lcd_welcome_line2_table
	movf	string_index,W
	addwf	PCL,F
	dt	"    Original design June 2000,",0
lcd_welcome_line3_table
	movf	string_index,W
	addwf	PCL,F
	dt	"            Revised June 2002",0
lcd_welcome_line4_table
	movf	string_index,W
	addwf	PCL,F
	dt	"Awaiting connection...",0

	
	;; ------------------------------------------------ Initialization

	org	0x100
			
main
	bsf	STATUS,RP0	; Initialize port direction
	movlw	b'10000000'	; Serial data is an input,
	movwf	TRISB		; everything else is output.
	clrf	TRISA
	movlw	b'11001000'	; TMR0 on, running at the instruction cycle speed
	movwf	OPTION_REG
	bcf	STATUS,RP0	; Back to register page 0
	clrf	PORTA		; Clear ports
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

	bsf	PORTA,BEEP_PIN	; Short startup beep
	movlw	50
	call	delay_ms
	bcf	PORTA,BEEP_PIN

	movlw	E1 | E2		; Init both LCDs at the same time
	movwf	emask
	call	lcd_init
	
standby_mode
	bcf	PORTA,BEEP_PIN	; Beeper off
	call	lcd_welcome	; Identify ourselves
	goto	led_fx		; And now for something completely different..
	
	;; ------------------------------------------------ LCD utilities

	
	;; Initialize our 40x4 LCD module (actually two 20x2 modules)
lcd_init
	;; First a reset
	movlw	50		; Waste 50 ms
	call	delay_ms
	movlw	system_set_4_bit >> 4 ; Send 4-bit bus initialization
	call	lcd_nibble
	movlw	5		; Wait 5 ms
	call	delay_ms
	movlw	system_set_4_bit >> 4 ; Repeat a few times...
	call	lcd_nibble
	movlw	5
	call	delay_ms
	movlw	system_set_4_bit >> 4
	call	lcd_nibble
	movlw	5
	call	delay_ms

	;; Now initialize
	movlw	system_set_4_bit
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
	bsf	PORTB,RS_PIN
	return


	;; Send one nibble on the 4-bit bus to the LCD
	;; Note that a faster version of this is included
	;; directly in the packet decoding code, since time is
	;; very tight there.
lcd_nibble
	andlw	0x0F		; Take the lower nibble of W
	movwf	lcd_tmp1	; Save the parameter for later
	movlw	0xF0
	andwf	PORTB,W		; Mask out the lower nibble
	iorwf	lcd_tmp1,W	; Replace it with our parameter
	movwf	PORTB		; Send it to port
	call	lcd_tinydelay
	movf	emask,W		; Turn on the emask bits in PORTB
	iorwf	PORTB,F
	call	lcd_tinydelay
	movlw	~(E1 | E2)	; Turn E1 and E2 back off
	andwf	PORTB,F
	return


	;; Send one character on the 4-bit bus, one nibble at a time
lcd_char
	movwf	lcd_tmp2	; Store it for later
	movwf	lcd_tmp3
	rrf	lcd_tmp2,F	; Get the high nibble first
	rrf	lcd_tmp2,F
	rrf	lcd_tmp2,F
	rrf	lcd_tmp2,W
	call	lcd_nibble
	movf	lcd_tmp3,W
	call	lcd_nibble
	return


	;; Send a character, but in command mode not data mode
lcd_cmd
	bcf	PORTB,RS_PIN
	call	lcd_char
	bsf	PORTB,RS_PIN
	return


	;; Tiny delay for LCD clock
lcd_tinydelay
	nop
	nop
	return

	;; Set the cursor to the beginning of a line
lcd_line1
	movlw	E1
	movwf	emask
	movlw	set_dd_ram
	call	lcd_cmd
	return	
lcd_line2
	movlw	E1
	movwf	emask
	movlw	set_dd_ram+40
	call	lcd_cmd
	return	
lcd_line3	
	movlw	E2
	movwf	emask
	movlw	set_dd_ram
	call	lcd_cmd
	return
lcd_line4
	movlw	E2
	movwf	emask
	movlw	set_dd_ram+40
	call	lcd_cmd
	return

	;; Print a message to the LCD from the string index given in W
lcd_print
	movwf	string_number
	clrf	string_index
lcd_print_loop
	call	string_table
	addlw	0		; Return on zero
	btfsc	STATUS,Z
	return
	call	lcd_char
	call	lcd_char_delay
	incf	string_index,F
	goto	lcd_print_loop

	;; Print the welcome message on the LCD
lcd_welcome
	movlw	clear_lcd
	call	lcd_cmd
	call	lcd_char_delay
	call	lcd_line1
	call	lcd_char_delay
	movlw	lcd_welcome_line1
	call	lcd_print
	call	lcd_line2
	call	lcd_char_delay
	movlw	lcd_welcome_line2
	call	lcd_print
	call	lcd_line3
	call	lcd_char_delay
	movlw	lcd_welcome_line3
	call	lcd_print
	call	lcd_line4
	call	lcd_char_delay
	movlw	lcd_welcome_line4
	call	lcd_print
	return

	;; Delay to use between characters
lcd_char_delay
	movlw	1
	goto	delay_ms

	
	;; ------------------------------------------------ LED utilities
	
	;; Do one PWM cycle using the current values for all LEDs
	;; This takes 22 cycles
led_refresh
	movf	PORTA,W		; Make a copy of PORTA...
	andlw	~LED_ALL	; ...with all LEDs off...
	movwf	led_tmp1	; ...in led_tmp1
	movf	pwm_index,W	; PWM for yellow LED
	subwf	led_yellow_pwm,W
	btfsc	STATUS,C
	bsf	led_tmp1,LED_YELLOW_PIN
	movf	pwm_index,W	; PWM for green LED
	subwf	led_green_pwm,W
	btfsc	STATUS,C
	bsf	led_tmp1,LED_GREEN_PIN
	movf	pwm_index,W	; PWM for red LED
	subwf	led_red_pwm,W
	btfsc	STATUS,C
	bsf	led_tmp1,LED_RED_PIN
	movf	led_tmp1,W	; Copy back to PORTA
	movwf	PORTA
	incf	pwm_index,F	; Next PWM cycle
	btfsc	STATUS,Z	; Skip cycle 0
	incf	pwm_index,F
	return

	;; Do several PWM cycles, and decrease the brightness of all LEDs.
	;; Prevent decay of LEDs in the bitmask led_decay_drive
led_decay
	movlw	led_decay_length
	movwf	led_decay3
led_decay_repeat
	movlw	led_decay_brightness	; Drive the LEDs in led_decay_drive 
	btfsc	led_decay_drive,LED_RED_PIN
	movwf	led_red_pwm
	btfsc	led_decay_drive,LED_GREEN_PIN
	movwf	led_green_pwm
	btfsc	led_decay_drive,LED_YELLOW_PIN
	movwf	led_yellow_pwm

	movf	led_red_pwm,F		; Decrease red brightness
	btfss	STATUS,Z
	decf	led_red_pwm,F
	movf	led_green_pwm,F		; Decrease green brightness
	btfss	STATUS,Z
	decf	led_green_pwm,F
	movf	led_yellow_pwm,F	; Decrease yellow brightness
	btfss	STATUS,Z
	decf	led_yellow_pwm,F

	movlw	led_decay_speed
	movwf	led_decay1
	clrf	led_decay2
led_decay_loop
	call	led_refresh
	
	btfss	PORTB,SERIAL_PIN ; If we notice serial activity, act on it
	goto	no_serial_activity
	movlw	E1 | E2
	movwf	emask
	btfss	PORTB,SERIAL_PIN ; Just a glitch?
	goto	no_serial_activity
	clrf	led_red_pwm	; Clean up a little
	clrf	led_green_pwm
	clrf	led_yellow_pwm
	goto	get_char	; Start recieving
no_serial_activity
	
	decfsz	led_decay2,F
	goto	led_decay_loop
	decfsz	led_decay1,F
	goto	led_decay_loop

	decfsz	led_decay3,F
	goto	led_decay_repeat
	return
		

	;; Nifty LED special effects while we wait. For each iteration,
	;; drive the LEDs indicated in the table while fading the rest,
	;; using the led_decay function above.
	;;
	;; This function never returns!
	;; Normally it would be running while the serial interrupt handler
	;; is waiting for data, and it would be exited by the serial interrupt
	;; handler.
led_fx_restart
	clrf	led_fx_index
led_fx
	movf	led_fx_index,W
	call	led_fx_table	; Load an entry from the table
	movwf	led_decay_drive
	movlw	0xFF
	subwf	led_decay_drive,W
	btfsc	STATUS,Z
	goto	led_fx_restart	; Restart if the table returns 0xFF
	call	led_decay	; Do this decay cycle
	incf	led_fx_index,F
	goto	led_fx		; Repeat...
		
			
	;; ------------------------------------------------ General-purpose utilities

	
	;; Delay for W milliseconds, assuming 10mhz processor speed
delay_ms
	movwf	delay_tmp1
delay_loop1
	movlw	0x56
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

	
	;; ------------------------------------------------ Serial reciever
	
	;; This serial reciever is tuned for 57600bps using a 10mHz crystal

	org	0x300
		
get_char
	clrf	delay_index
start_bit_wait
	btfsc	PORTB,SERIAL_PIN ; Wait for start bit
	goto	got_start_bit
	incfsz	delay_index,F
	goto	start_bit_wait
	goto	standby_mode	; If we don't recieve it in a while, go to standby

got_start_bit	
	movlw	8		; Prepare to recieve a byte
	movwf	ser_bit_tmp
	clrf	ser_byte

	movlw	6		; Sample bits in the middle-
	movwf	delay_index	; Waste about 20 cycles
bit_middle_delay
	decfsz	delay_index,F
	goto	bit_middle_delay
	
bit_loop			; This loop must always total 43 cycles
	clrf	TMR0		; Start counting cycles (3 cycles into our total)

	;; Do something useful while waiting (Safe to add more
	;; here as long as it's all 29 cycles or less)
	call	led_refresh
	
	;; We now need to waste time until our counter hits 29. This is our target of 43
	;; cycles, minus the 3 it takes to load the timer, the 6 for the actual serial code
	;; below, and 5 cycles of overhead for this calculated jump.
	
	movlw	0x03
	movwf	PCLATH
	movf	TMR0,W
	addwf	PCL,F
	
	nop			; Big table of 31 nop's...
	nop
	nop
	nop
	nop
	nop
	nop
	nop			; 8

	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop			; 16

	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop			; 24

	nop
	nop
	nop
	nop
	nop			; 29

	;; 6 Cycles for loop and serial stuff
	rrf	ser_byte,F	; Transfer the serial port bit to ser_byte
	btfss	PORTB,SERIAL_PIN
	bsf	ser_byte,7
	decfsz	ser_bit_tmp,F
	goto	bit_loop
	
	;; Fall through to packet decode
	
	;; ------------------------------------------------ Packet decode
	
	;; Now decode the packet, during the stop bit. This should take no
	;; more than about 30 cycles because we must still be in the stop bit when
	;; this ends, so timing is very tight.
	
	btfss	ser_byte,4	; Is it a continuation packet?
	clrf	pkt_nibbles	; If not, begin a new packet
	movlw	0x0F		; Mask off the data from this incoming byte
	andwf	ser_byte,W
	movwf	pkt_low
	iorwf	pkt_byte,F	; Store it in the low nibble of pkt_byte
	incf	pkt_nibbles,F	; Increment count of nibbles recieved
	btfss	pkt_nibbles,0	; Rotate into high nibble if it's an odd nibble
	goto	no_nibble_rotate
	movf	pkt_low,W
	movwf	pkt_high
	rlf	pkt_byte,F
	rlf	pkt_byte,F
	rlf	pkt_byte,F
	rlf	pkt_byte,F
no_nibble_rotate		; So far, we've used between 10 and 15 cycles
	
	;; So far we've used between 10 and 15 cycles to assemble the nibbles.
	;; The following device decode will use 11 cycles. This leaves very little
	;; time for the actual device handlers, only about 10 cycles.
			
	rrf	ser_byte,W	; Mask off the device number
	movwf	pkt_tmp
	rrf	pkt_tmp,F
	rrf	pkt_tmp,F
	rrf	pkt_tmp,F
	rrf	pkt_tmp,W	; Rotate a total of 5 times
	andlw	0x07		; Mask off lower 3 bits
	addwf	PCL,F		; Jump table, indexed by device number
	goto	decode_done	; 0. No-op
	goto	pkt_lcd		; 1. LCD data
	goto	pkt_control	; 2. control bits
	goto	pkt_led		; 3. LEDs
	goto	decode_done	; 4.
	goto	decode_done	; 5.
	goto	decode_done	; 6.
	goto	decode_done	; 7.

	;; We return here after packet decoding. We should be inside
	;; the stop bit by now, so make sure it's a stop bit. If not,
	;; wait for the beginning of the stop bit to resynchronize.
	;; This works for synchronizing to streams of zero bytes, which
	;; is what we'll be recieving most of the time.
decode_done
	btfss	PORTB,SERIAL_PIN
	goto	get_char	; Yep, we have a stop bit
	clrf	delay_index
stop_bit_wait
	btfss	PORTB,SERIAL_PIN ; Wait for stop bit
	goto	get_char	; Got it
	incfsz	delay_index,F
	goto	stop_bit_wait
	goto	standby_mode	; If we don't recieve it in a while, go to standby

	;; Decode a single-nibble packet with LCD and beeper control bits
pkt_control
	bcf	PORTB,RS_PIN	; Turn everything off first
	bcf	PORTA,BEEP_PIN
	clrf	emask
	btfsc	ser_byte,0	; Turn on what's indicated
	bsf	PORTB,RS_PIN
	btfsc	ser_byte,1
	bsf	PORTA,BEEP_PIN
	btfsc	ser_byte,2
	bsf	emask,E1_PIN
	btfsc	ser_byte,3
	bsf	emask,E2_PIN
	goto	decode_done	

	;; Decode a one-byte LCD data packet
	;; We could just send each nibble to the LCD as we get it,
	;; but then a communications error could get the LCD one
	;; nibble out of sync very easily.
pkt_lcd
	btfss	pkt_nibbles,1	; Wait until we have 2 nibbles
	goto	decode_done
	clrf	pkt_nibbles	; Reset packet nibble counter

	movlw	0xF0		; Mask out lower nibble of PORTB
	andwf	PORTB,W
	iorwf	pkt_high,W	; Stick in our high nibble
	movwf	PORTB		; Send off
	call	lcd_tinydelay
	movf	emask,W		; Turn on emask bits in PORTB
	iorwf	PORTB,F
	call	lcd_tinydelay
	movlw	~(E1 | E2)	; Turn back off
	andwf	PORTB,F

	movlw	0xF0		; Mask out lower nibble of PORTB
	andwf	PORTB,W
	iorwf	pkt_low,W	; Stick in our low nibble
	movwf	PORTB		; Send off
	call	lcd_tinydelay
	movf	emask,W		; Turn on emask bits in PORTB
	iorwf	PORTB,F
	call	lcd_tinydelay
	movlw	~(E1 | E2)	; Turn back off
	andwf	PORTB,F
	
	goto	decode_done

	;; Decode a 3-byte packet that sets the LED's PWM values
pkt_led
	movf	pkt_nibbles,W	; Which byte is it?
	addwf	PCL,F
	goto	decode_done	; None? Shouldn't happen...
	goto	decode_done	; 1 nibble
	goto	pkt_led_1	; 2 nibbles
	goto	decode_done	; 3 nibbles
	goto	pkt_led_2	; 4 nibbles
	goto	decode_done	; 5 nibbles
	goto	pkt_led_3	; 6 nibbles
	clrf	pkt_nibbles	; Any more than that, and we reset...
	goto	decode_done

pkt_led_1			; First byte of LED packet
	movf	pkt_byte,W
	movwf	led_yellow_pwm
	clrf	pkt_byte	; Clear packet pyte to prepare for another
	goto	decode_done

pkt_led_2			; Second byte of LED packet
	movf	pkt_byte,W
	movwf	led_red_pwm
	clrf	pkt_byte	; Clear packet pyte to prepare for another
	goto	decode_done

pkt_led_3			; Third byte of LED packet
	movf	pkt_byte,W
	movwf	led_green_pwm
	clrf	pkt_byte	; Clear packet pyte to prepare for another
	goto	decode_done

	;; ------------------------------------------------ The End!			
	end
