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
; perform nearly that well. We're currently just using
; 19200 baud RS-232 out of laziness ;)
;
; Packets are of the following form:
;
;      Byte 0:	Literal "U", for synchronization
;   Bytes 1-8:  Data from the 4 16-bit sensors
;      Byte 9:  Button status
;     Byte 10:  Cheesy 8-bit checksum- computed such
;               that the sum of all bytes in the packet
;               is zero.
;
; There is no packet framing, the receiver must pass a sliding window
; over the incoming data and detect all valid packets.
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
		IOLATCH, temp, delay_temp, checksum
		analog_channel
		analog_buffer:8
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

	;; Initialize TMR0
	banksel	OPTION_REG
	bcf	OPTION_REG, T0CS
	clrf	TMR0
	bcf	INTCON, T0IF

	banksel	GPIO
main_loop

	;; Read all analog channels
	movlw	0x04
	movwf	analog_channel
	movlw	analog_buffer
	movwf	FSR
	call	analog16_Read
	call	analog16_Read
	call	analog16_Read
	call	analog16_Read

	clrf	checksum	; Prepare for another transmission...
	call	reset_bit_timer

	movlw	'U'		; Sync byte
	call	tx_send_byte

	;; Send all analog channels
	movf	analog_buffer+0, w
	call	tx_send_byte
	movf	analog_buffer+1, w
	call	tx_send_byte
	movf	analog_buffer+2, w
	call	tx_send_byte
	movf	analog_buffer+3, w
	call	tx_send_byte
	movf	analog_buffer+4, w
	call	tx_send_byte
	movf	analog_buffer+5, w
	call	tx_send_byte
	movf	analog_buffer+6, w
	call	tx_send_byte
	movf	analog_buffer+7, w
	call	tx_send_byte

	;; Send buttons (not implemented yet)
	movlw	0xAA
	call	tx_send_byte

	;; Send checksum
	movf	checksum, w
	call	tx_send_byte

	goto	main_loop


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

	;; Read analog channel given in analog_channel into *FSR.
	;; Increments the current channel and leaves FSR pointed after
	;; the data we just wrote.
analog16_Read
	bsf	IOLATCH, CS_PIN	; Reset the state of its serial bus
	bcf	IOLATCH, DOUT_PIN
	movf	IOLATCH, w
	movwf	GPIO
	bcf	IOLATCH, CS_PIN
	movf	IOLATCH, w
	movwf	GPIO

	movf	analog_channel, w
	incf	analog_channel, f
	andlw	0x07		; Make a control word with the channel number
	iorlw	0x78		; External clock mode
	movwf	temp
	swapf	temp, f
	shiftOutByte temp
	nop

	shiftInByte INDF
	incf	FSR, f
	shiftInByte INDF
	incf	FSR, f
	return

;----------------------------------------------------- Transmitter

reset_bit_timer
	movlw	.255
	movwf	TMR0
	;; Fall through to wait_next_bit...

	;; Use TMR0 to busy-wait until the next bit period
wait_next_bit
	btfss	INTCON, T0IF
	goto	wait_next_bit
	movlw	.228		; 19200 baud
	addwf	TMR0, f
	bcf	INTCON, T0IF
	return

	;; Turn on the transmitter pin
tx_pin_on
	bsf	IOLATCH, TX_PIN
	movf	IOLATCH, w
	movwf	GPIO
	return

	;; Turn off the transmitter pin
tx_pin_off
	bcf	IOLATCH, TX_PIN
	movf	IOLATCH, w
	movwf	GPIO
	return

	;; Send one byte, from 'w'. This also updates the checksum
tx_send_byte
	movwf	temp
	subwf	checksum, f
	call	wait_next_bit	; Start bit
	call	tx_pin_on

	call	tx_send_bit
	call	tx_send_bit
	call	tx_send_bit
	call	tx_send_bit
	call	tx_send_bit
	call	tx_send_bit
	call	tx_send_bit
	call	tx_send_bit

	call	wait_next_bit	; Stop bit
	goto	tx_pin_off

	;; Rotate 'temp' left and send the shifted out bit
tx_send_bit
	call	wait_next_bit
	rrf	temp, f
	btfsc	STATUS, C
	goto	tx_pin_off
	goto	tx_pin_on

	end


