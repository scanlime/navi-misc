;
; Wireless temperature sensors, transmitter side
; Copyright 2004, Micah Dowty <micah@navi.cx>
;
; This samples one or more I2C temperature sensors
; (Microchip TC74) connected to the same bus, and sends back
; all results over a Laipac 315MHz ASK transmitter.
;
; Hardware description:
;      GP0: Battery voltage sensor (470Kohms to battery+, 470K to ground)
;      GP1: Transmitter data
;      GP2: Transmitter power
;      GP3: I2C data (4.7k pullup resistor)
;      GP4: I2C clock
;      GP5: I2C power
;
; The protocol is designed to be relatively radio-friendly, but still
; possible to receive using a UART designed for RS-232. Ideally, ones
; are transmitted as 156.3us high followed by 364.6us low, and zeroes
; are transmitted as 364.6us high followed by 156.3us low. This gives
; a constant bit period of 520.8us. Our data rate is 1920 bits per second,
; and it can be received or transmitted using a UART set to 19200 baud.
;
; Note that the radio signal is inverted from normal TTL-level RS-232,
; such that when idle it isn't transmitting. The receiver counts the number
; of bits set in each received byte to determine whether it's a zero or a one.
; This should provide a good amount of noise immunity.
;
; Packets are framed using an HDLC-like method. The bit sequence 011110
; is a 'flag', and signifies the beginning and/or end of a packet. When
; transmitting packet contents, bit stuffing ensures that the flag
; sequence never occurs. After any run of three "1" bits, a "0" is explicitly
; inserted. On the receiving side, after receiving a run of three "1" bits
; another "1" will indicate a flag but a "0" will be removed. All values
; are sent LSB-first.
;
; Packets are of the following form:
;
;      - 6-bit Flag sequence
;      - Bit-stuffed content:
;         - 2-bit protocol ID (0)
;         - 6-bit station ID
;         - 5-bit packet sequence number
;         - 10-bit battery voltage
;         - For each attached TC74 sensor:
;           - 8-bit sensor value
;         - 8-bit frame check sequence (CRC-8 of above)
;      - 6-bit Flag sequence
;

	list	p=12f675
	#include p12f675.inc

	errorlevel -302
	__CONFIG _CPD_OFF & _CP_OFF & _BODEN_ON & _MCLRE_OFF & _PWRTE_ON & _WDT_ON & _INTRC_OSC_NOCLKOUT

;----------------------------------------------------- Constants

BATT_VOLT_PIN  equ 0
TX_PIN         equ 1
TX_POWER_PIN   equ 2
SDA_PIN	       equ 3
SCL_PIN	       equ 4
I2C_POWER_PIN  equ 5

BATT_VOLT_MASK equ 0x01
TX_MASK        equ 0x02
TX_POWER_MASK  equ 0x04
SDA_MASK       equ 0x08
SCL_MASK       equ 0x10
I2C_POWER_MASK equ 0x20

TX_SHORT_DELAY equ .168		; 156.3us
TX_LONG_DELAY  equ .42		; 364.6us


;----------------------------------------------------- Variables

	cblock 0x20
		temp, consecutive_ones, crc_reg
	endc


;----------------------------------------------------- Initialization
	org 0

	; Initialize I/O
	banksel	GPIO			; Set initial latch values
	movlw	I2C_POWER_MASK | SDA_MASK | SCL_MASK | TX_POWER_MASK
	movwf	GPIO
	banksel	TRISIO
	movlw	BATT_VOLT_MASK		; Set pin directions (also turns on peripherals)
	movwf	TRISIO

	;; Initialize TMR0
	banksel	OPTION_REG
	bcf	OPTION_REG, T0CS
	clrf	TMR0
	bcf	INTCON, T0IF

	;; Initialize variables
	clrf	crc_reg
	clrf	consecutive_ones

	banksel	GPIO


;----------------------------------------------------- Main Program

	call	tx_begin_content

	movlw	0x42
	movwf	temp
	call	tx_content_8bit

	call	tx_end_content


	;; Go to sleep, turning off all peripherals
	clrf	GPIO
	sleep

;----------------------------------------------------- Protocol: Frame check sequence

	;; Reinitialize the CRC8 and send a flag
tx_begin_content
	clrf	crc_reg
	goto	tx_send_flag

	;; A macro to shift a new bit into the CRC
crc_shift macro value
	rlf	crc_reg, f
	if value
	 bsf	crc_reg, 0
	else
	 bcf	crc_reg, 0
	endif
	movlw	0x07
	btfsc	STATUS, C	; XOR with the polynomial if 1 came out
	xorwf	crc_reg, f
	endm

	;; Send the CRC8 followed by a flag. Note that this first has to push
	;; 8 zeroes through the CRC8 shift register to pad the message, as
	;; per the CRC8 specification.
tx_end_content
	crc_shift 0
	crc_shift 0
	crc_shift 0
	crc_shift 0
	crc_shift 0
	crc_shift 0
	crc_shift 0
	crc_shift 0
	movf	crc_reg, w	; Send the CRC
	movwf	temp
	call	tx_content_8bit
	goto	tx_send_flag	; Send the flag and return

	;; Send a bit-stuffed "1" that is included in the CRC8
tx_content_one
	crc_shift 1
	goto	tx_stuffed_one

	;; Send a bit-stuffed "0" that is included in the CRC8
tx_content_zero
	crc_shift 0
	goto	tx_stuffed_zero

	;; Send out one bit of content, right-shifted out of 'temp'
tx_content_bit
	rrf	temp, f
	btfsc	STATUS, C
	goto	tx_content_one
	goto	tx_content_zero

	;; Send out multiple bits of content from 'temp'
tx_content_8bit
	call	tx_content_bit
tx_content_7bit
	call	tx_content_bit
tx_content_6bit
	call	tx_content_bit
tx_content_5bit
	call	tx_content_bit
tx_content_4bit
	call	tx_content_bit
tx_content_3bit
	call	tx_content_bit
tx_content_2bit
	call	tx_content_bit
	goto	tx_content_bit


;----------------------------------------------------- Protocol: Bit stuffing and flags

tx_send_flag
	call	tx_send_zero
	call	tx_send_one
	call	tx_send_one
	call	tx_send_one
	call	tx_send_one
	call	tx_send_zero
	clrf	consecutive_ones
	return

	;; Send a bit-stuffed "1"
tx_stuffed_one
	incf	consecutive_ones, f	; Send the one
	call	tx_send_one
	movlw	.3			; Has this just been a run of three ones?
	subwf	consecutive_ones, w
	btfss	STATUS, Z
	return				; Return if not
	;; Fall through...

	;; Send a bit-stuffed "0"
tx_stuffed_zero
	clrf	consecutive_ones
	goto	tx_send_zero


;----------------------------------------------------- Protocol: Low level bit timing

	;; Set TMR0 to the given value and reset its overflow flag
tx_set_timer macro value
	movlw	value
	movwf	TMR0
	bcf	INTCON, T0IF
	endm

	;; Wait for TMR0 to overflow
tx_wait_timer macro
	btfss	INTCON, T0IF
	goto	$-1
	endm

	;; Start a new transmission
tx_send_start
	tx_set_timer 0xFF
	return

	;; Bring the transmit pin high
tx_pin_high macro
	movlw	I2C_POWER_MASK | SDA_MASK | SCL_MASK | TX_POWER_MASK | TX_MASK
	movwf	GPIO
	endm

	;; Bring the transmit pin low
tx_pin_low macro
	movlw	I2C_POWER_MASK | SDA_MASK | SCL_MASK | TX_POWER_MASK
	movwf	GPIO
	endm

	;; Send a '1' bit
tx_send_one
	tx_wait_timer		; Let the previous bit end
	tx_set_timer	TX_LONG_DELAY
	tx_pin_high
	tx_wait_timer
	tx_set_timer	TX_SHORT_DELAY
	tx_pin_low
	return

	;; Send a '0' bit
tx_send_zero
	tx_wait_timer		; Let the previous bit end
	tx_set_timer	TX_SHORT_DELAY
	tx_pin_high
	tx_wait_timer
	tx_set_timer	TX_LONG_DELAY
	tx_pin_low
	return

	end


