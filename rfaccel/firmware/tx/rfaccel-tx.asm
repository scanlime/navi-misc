;
; Wireless 3-axis accelerometer, transmitter side
; Copyright 2004, Micah Dowty <micah@navi.cx>
;
; Hardware description:
;      GP0: Power for transmitter and sensor package (sensor board pin 2)
;      GP1: DCLK output to sensor package (sensor board pin 3)
;      GP2: CS   output to sensor package (sensor board pin 4)
;      GP3: DOUT input from sensor package (sensor board pin 6)
;      GP4: DIN  output to sensor package (sensor board pin 5)
;      GP5: Transmitter data
;
; The sensor package includes an ADS8344 A/D converter,
; with sensors on inputs channels 4 through 7. Sensors
; are two ADXL311 dual-axis accelerometers from Analog
; devices, mounted at right angles and buffered by
; op-amps.
;
; The transmitter is a 916.5MHz module from Laipac.
; It's rated for 200kbps, but in practise doesn't seem to
; perform nearly that well. We currently use a really simple
; but fairly inefficient protocol. One 'unit delay' is currently
; 28.5us. There are three codes:
;
;           Zero:  1 unit high,  at least 1 unit low
;            One:  2 units high, at least 1 unit low
;    Packet flag:  3 units high, at least 1 unit low
;
; Multiple begin packet codes can be issued to let the
; receiver's DC bias settle. Any data between two packet
; flag codes may be a valid packet. The packet format is:
;
;   Bytes 0-7:  Data from the 4 16-bit sensors
;      Byte 8:  Button status
;      Byte 9:  Cheesy 8-bit checksum- computed such
;               that the sum of all bytes in the packet
;               is zero.
;

	list	p=12f675
	#include p12f675.inc

	errorlevel -302
	__CONFIG _CPD_OFF & _CP_OFF & _BODEN_ON & _MCLRE_OFF & _PWRTE_ON & _WDT_OFF & _INTRC_OSC_NOCLKOUT

;----------------------------------------------------- Constants

POWER_PIN   equ 0
DCLK_PIN    equ 1
CS_PIN      equ 2
DOUT_PIN    equ 3
DIN_PIN     equ 4
TX_PIN      equ 5


;----------------------------------------------------- Variables

	cblock 0x20
		IOLATCH, temp, delay_temp
		analog_high, analog_low, analog_channel
	endc


;----------------------------------------------------- Main program
	org 0

	; Initialize I/O
	banksel	IOLATCH		; Set initial latch values
	clrf	IOLATCH
	bsf	IOLATCH, POWER_PIN
	bcf	IOLATCH, DCLK_PIN
	bsf	IOLATCH, CS_PIN
	bcf	IOLATCH, DIN_PIN
	bcf	IOLATCH, TX_PIN
	movf	IOLATCH, w	; Apply the latch values
	movwf	GPIO
	banksel	TRISIO		; Set directions
	bcf	TRISIO, TX_PIN
	bcf	TRISIO, POWER_PIN
	bcf	TRISIO, DCLK_PIN
	bcf	TRISIO, CS_PIN
	bcf	TRISIO, DIN_PIN
	bsf	TRISIO, DOUT_PIN
	banksel	GPIO

main_loop

	call	tx_send_flag

	;; Send all analog channels
	movlw	0x04
	movwf	analog_channel
	call	send_analog_channel
	incf	analog_channel, f
	call	send_analog_channel
	incf	analog_channel, f
	call	send_analog_channel
	incf	analog_channel, f
	call	send_analog_channel

	goto	main_loop

;----------------------------------------------------- Protocol

	;; Read the current analog channel and send two
	;; bytes of data over the radio.
send_analog_channel
	call	analog16_Read
	movf	analog_high, w
	call	tx_send_byte
	movf	analog_low, w
	goto	tx_send_byte


;----------------------------------------------------- A/D Converter

clockBit macro
	bsf	IOLATCH, DCLK_PIN
	movf	IOLATCH, w
	movwf	GPIO
	bcf	IOLATCH, DCLK_PIN
	movf	IOLATCH, w
	movwf	GPIO
	endm

shiftOutBit macro reg
	bcf	IOLATCH, DIN_PIN
	rlf	reg, f
	btfsc	STATUS, C
	bsf	IOLATCH, DIN_PIN
	movf	IOLATCH, w
	movwf	GPIO
	clockBit
	endm

shiftOutByte macro reg
	shiftOutBit reg
	shiftOutBit reg
	shiftOutBit reg
	shiftOutBit reg
	shiftOutBit reg
	shiftOutBit reg
	shiftOutBit reg
	shiftOutBit reg
	endm

shiftInBit macro reg
	clockBit
	rlf	reg, f
	bcf	reg, 0
	btfsc	GPIO, DOUT_PIN
	bsf	reg, 0
	endm

shiftInByte macro reg
	shiftInBit reg
	shiftInBit reg
	shiftInBit reg
	shiftInBit reg
	shiftInBit reg
	shiftInBit reg
	shiftInBit reg
	shiftInBit reg
	endm

	;; Read analog channel given in analog_channel into analog_low and analog_high
analog16_Read
	bsf	IOLATCH, CS_PIN	; Reset the state of its serial bus
	bcf	IOLATCH, DOUT_PIN
	movf	IOLATCH, w
	movwf	GPIO
	bcf	IOLATCH, CS_PIN
	movf	IOLATCH, w
	movwf	GPIO

	movf	analog_channel, w
	andlw	0x07		; Make a control word with the channel number
	iorlw	0x78		; External clock mode
	movwf	temp
	swapf	temp, f
	shiftOutByte temp
	nop

	shiftInByte analog_high
	shiftInByte analog_low
	return

;----------------------------------------------------- Transmitter

	;; Turn on the transmitter pin
tx_pin_on macro
	bsf	IOLATCH, TX_PIN
	movf	IOLATCH, w
	movwf	GPIO
	endm

	;; Turn off the transmitter pin
tx_pin_off macro
	bcf	IOLATCH, TX_PIN
	movf	IOLATCH, w
	movwf	GPIO
	endm

	;; Send a "one" code
tx_send_one
	tx_pin_on
	call	unit_delay
	call	unit_delay
	tx_pin_off
	call	unit_delay
	return

	;; Send a "zero" code
tx_send_zero
	tx_pin_on
	call	unit_delay
	tx_pin_off
	call	unit_delay
	return

	;; Send a "flag" code
tx_send_flag
	tx_pin_on
	call	unit_delay
	call	unit_delay
	call	unit_delay
	tx_pin_off
	call	unit_delay
	return

	;; Send one byte, from 'w'
tx_send_byte
	movwf	temp
	call	tx_send_bit
	call	tx_send_bit
	call	tx_send_bit
	call	tx_send_bit
	call	tx_send_bit
	call	tx_send_bit
	call	tx_send_bit
	goto	tx_send_bit

	;; Rotate 'temp' left and send the shifted out bit
tx_send_bit
	rlf	temp, f
	btfsc	STATUS, C
	goto	tx_send_one
	goto	tx_send_zero

unit_delay
	movlw	0x05
	movwf	delay_temp
delay_loop
	decfsz	delay_temp, f
	goto	delay_loop
	return

	end


