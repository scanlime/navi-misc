;
; Wireless temperature sensors, transmitter side
; Copyright 2004, Micah Dowty <micah@navi.cx>
;
; This samples an I2C temperature sensor (Microchip TC74) and sends back
; all results over a Laipac 315MHz ASK transmitter. Currently it only supports
; a single temperature sensor, but the protocol is extensible.
;
; Hardware description:
;      GP0: Battery voltage sensor (470Kohms to battery+, 470K to ground)
;      GP1: Transmitter data
;      GP2: Transmitter and I2C power
;      GP3: Unused input-only pin, external 4.7k pullup resistor to I2C power
;      GP4: I2C data, 4.7k pullup resistor to VCC
;      GP5: I2C clock
;
; The protocol is designed to be relatively radio-friendly, but still
; possible to receive using a UART designed for RS-232. Ideally, ones
; are transmitted as 312.5us high followed by 729.2us low, and zeroes
; are transmitted as 729.9us high followed by 312.5us low. This gives
; a constant bit period of 1.0417ms. Our data rate is 960 bits per second,
; and it can be received or transmitted using a UART set to 9600 baud.
;
; Note that the radio signal is inverted from normal TTL-level RS-232,
; such that when idle it isn't transmitting. The receiver tests each received
; byte against the two ideal bit patterns, selecting the closest match and
; using the number of differences as a measure of signal strength.
;
; Packets are framed using an HDLC-like method. The bit sequence 011110
; is a 'flag', and signifies the beginning and/or end of a packet. When
; transmitting packet contents, bit stuffing ensures that the flag
; sequence never occurs. After any run of three "1" bits, a "0" is explicitly
; inserted. On the receiving side, after receiving a run of three "1" bits
; another "1" will indicate a flag but a "0" will be removed. All values
; are sent LSB-first unless otherwise indicated.
;
; Packets are of the following form:
;
;      - 6-bit Flag sequence
;      - Bit-stuffed content:
;         - 2-bit protocol ID (0)
;         - 6-bit station ID
;         - 5-bit packet sequence number
;         - 10-bit battery voltage
;         - 8-bit sensor value(s), MSB first (streamed directly from I2C)
;         - 8-bit frame check sequence (CRC-8 of above)
;      - 6-bit Flag sequence
;

	list	p=12f675
	#include p12f675.inc

	errorlevel -302
	__CONFIG _CPD_OFF & _CP_OFF & _BODEN_ON & _MCLRE_OFF & _PWRTE_ON & _WDT_ON & _INTRC_OSC_NOCLKOUT


;----------------------------------------------------- Device configuration

STATION_ID	equ	1	; A unique ID between 0 and 63
TC74_ADDR	equ	7	; The address marked on this station's TC74
SLEEP_TIME	equ	1	; Time to sleep between readings, in 2.3 second units


;----------------------------------------------------- Constants

BATT_VOLT_PIN	equ 0
TX_PIN		equ 1
POWER_PIN	equ 2
UNUSED_PIN	equ 3
SDA_PIN		equ 4
SCL_PIN		equ 5

BATT_VOLT_MASK	equ 0x01
TX_MASK		equ 0x02
POWER_MASK	equ 0x04
UNUSED_MASK	equ 0x08
SDA_MASK	equ 0x10
SCL_MASK	equ 0x20

TC74_ADDR_READ	equ (0x91 | (TC74_ADDR << 1))
TC74_ADDR_WRITE	equ (0x90 | (TC74_ADDR << 1))

PREAMBLE_LENGTH	equ .64

TX_SHORT	equ .312	; Transmit timings, in microseconds
TX_LONG		equ .730

TX_SHORT_DELAY	equ (.258 - (TX_SHORT / 4))
TX_LONG_DELAY	equ (.258 - (TX_LONG / 4))


;----------------------------------------------------- Variables

	cblock 0x20
		temp, consecutive_ones, crc_reg, temp2, packet_seq
		iolatch
	endc


;----------------------------------------------------- Initialization
	org 0
start

	; Initialize I/O
	movlw	POWER_MASK | SCL_MASK
	movwf	iolatch
	movwf	GPIO
	movlw	BATT_VOLT_MASK | UNUSED_MASK | SDA_MASK
	bsf	STATUS, RP0
	movwf	TRISIO
	bcf	STATUS, RP0

	;; Set up the A/D converter, start an acquisition on channel 0
	bsf	STATUS, RP0
	movlw	0x31		; RC osc, only AN0 is analog
	movwf	ANSEL
	bcf	STATUS, RP0
	movlw	0x81		; Right justified, VDD reference, channel 0, converter on
	movwf	ADCON0

	;; Calibrate the oscillator
	bsf	STATUS, RP0
	call	0x3FF
	movwf	OSCCAL
	bcf	STATUS, RP0

	;; Initialize TMR0 and the watchdog timer. Initially, we assign
	;; a 1:4 prescaler to TMR0 so we can time delays long enough for our
	;; slow bits. Before sleeping we swap that over to the WDT.
	clrwdt
	movlw	0xD1
	bsf	STATUS, RP0
	movwf	OPTION_REG
	bcf	STATUS, RP0
	clrf	TMR0
	clrf	INTCON
	clrwdt

	;; Initialize variables
	clrf	crc_reg
	clrf	consecutive_ones


;----------------------------------------------------- Temperature Sampling

	;; Turn on the TC74
	call	i2c_start
	movlw	TC74_ADDR_WRITE	; Address it
	movwf	temp
	call	i2c_send_byte
	movlw	0x01		; Select the CONFIG register
	movwf	temp
	call	i2c_send_byte
	movlw	0x00		; Out of standby mode
	movwf	temp
	call	i2c_send_byte
	call	i2c_stop

	;; Send the preamble while waiting for a sample
	call	tx_send_preamble

	;; Point at the TEMP register
	call	i2c_start
	movlw	TC74_ADDR_WRITE	; Address it
	movwf	temp
	call	i2c_send_byte
	movlw	0x00		; Select the TEMP register
	movwf	temp
	call	i2c_send_byte
	call	i2c_stop

	;; Get ready to read the temperature below
	call	i2c_start
	movlw	TC74_ADDR_READ
	movwf	temp
	call	i2c_send_byte

;----------------------------------------------------- Packet Contents

	call	tx_begin_content
	incf	packet_seq, f

	movlw	0		; 2-bit protocol ID
	movwf	temp
	call	tx_content_2bit

	movlw	STATION_ID	; 6-bit station Id
	movwf	temp
	call	tx_content_6bit

	bsf	ADCON0, GO	; Start converting the battery voltage

	movf	packet_seq, w	; 5-bit packet sequence number
	movwf	temp
	call	tx_content_5bit

	bsf	STATUS, RP0	; 10-bit battery voltage
	movf	ADRESL, w
	bcf	STATUS, RP0
	movwf	temp
	call	tx_content_8bit
	movf	ADRESH, w
	movwf	temp
	call	tx_content_2bit

	call	tx_i2c_byte	; 8-bit temperature, read directly from I2C
	call	i2c_stop

	call	tx_end_content

;----------------------------------------------------- Shutdown

	clrf	GPIO		; Turn off external peripherals
	clrf	iolatch
	clrf	ADCON0		; Turn off the A/D converter

	;; Assign a 1:128 prescaler to the WDT. Each 'sleep' we do now
	;; will last as long as possible, about 2.3 seconds.
	clrwdt			; Clear the watchdog
	clrf	TMR0		; Clear the prescaler
	movlw	0xDC		; Reassign prescaler
	bsf	STATUS, RP0
	movwf	OPTION_REG
	bcf	STATUS, RP0

	movlw	SLEEP_TIME	; Sleep for a configurable amount of time
	movwf	temp
sleep_loop
	sleep
	decfsz	temp, f
	goto	sleep_loop

	goto	start		; Reinitialize and send another packet


;----------------------------------------------------- I2C Master

i2c_sda_high macro
	movlw	BATT_VOLT_MASK | UNUSED_MASK | SDA_MASK
	bsf	STATUS, RP0
	movwf	TRISIO
	bcf	STATUS, RP0
	endm

i2c_sda_low macro
	movlw	BATT_VOLT_MASK | UNUSED_MASK
	bsf	STATUS, RP0
	movwf	TRISIO
	bcf	STATUS, RP0
	endm

i2c_scl_high macro
	bsf	iolatch, SCL_PIN
	movf	iolatch, w
	movwf	GPIO
	endm

i2c_scl_low macro
	bcf	iolatch, SCL_PIN
	movf	iolatch, w
	movwf	GPIO
	endm

i2c_delay
	nop
	nop
	nop
	nop
	nop
	return

i2c_start
	i2c_sda_low
	call	i2c_delay
	i2c_scl_low
	call	i2c_delay
	return

i2c_stop
	i2c_sda_low
	call	i2c_delay
	i2c_scl_high
	call	i2c_delay
	i2c_sda_high
	call	i2c_delay
	return

i2c_send_zero
	i2c_sda_low
	call	i2c_delay
	i2c_scl_high
	call	i2c_delay
	i2c_scl_low
	call	i2c_delay
	return

i2c_send_one
	i2c_sda_high
	call	i2c_delay
	i2c_scl_high
	call	i2c_delay
	i2c_scl_low
	call	i2c_delay
	return

	;; Shift one bit out of 'temp'
i2c_send_bit
	rlf	temp, f
	btfss	STATUS, C
	goto	i2c_send_zero
	goto	i2c_send_one

	;; Shift one byte out of 'temp'
i2c_send_byte
	call	i2c_send_bit
	call	i2c_send_bit
	call	i2c_send_bit
	call	i2c_send_bit
	call	i2c_send_bit
	call	i2c_send_bit
	call	i2c_send_bit
	call	i2c_send_bit
	i2c_scl_high		; Clock in and ignore the ACK
	i2c_scl_low
	return

	;; Read a bit from I2C and transmit it as packet content
tx_i2c_bit
	i2c_scl_high
	call	i2c_delay
	call	tx_i2c_bit_test
	i2c_scl_low
	call	i2c_delay
	return
tx_i2c_bit_test
	btfsc	GPIO, SDA_PIN
	goto	tx_content_one
	goto	tx_content_zero

tx_i2c_byte
	call	tx_i2c_bit
	call	tx_i2c_bit
	call	tx_i2c_bit
	call	tx_i2c_bit
	call	tx_i2c_bit
	call	tx_i2c_bit
	call	tx_i2c_bit
	call	tx_i2c_bit
	i2c_scl_high		; Clock in and ignore the ACK
	i2c_scl_low
	return


;----------------------------------------------------- Protocol: Frame check sequence

	;; Reinitialize the CRC8 and send a few copies of the flag sequence
tx_begin_content
	clrf	crc_reg
	call	tx_send_start
	call	tx_send_flag
	call	tx_send_flag
	call	tx_send_flag
	call	tx_send_flag
	return

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
	call	tx_send_flag
	goto	tx_send_end

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

	;; Add to TMR0 and reset its overflow flag
tx_add_timer macro value
	movlw	value
	addwf	TMR0, f
	bcf	INTCON, T0IF
	endm

	;; Wait for TMR0 to overflow
tx_wait_timer macro
	local keep_waiting
keep_waiting
	clrwdt
	btfss	INTCON, T0IF
	goto	keep_waiting
	endm

	;; Bring the transmit pin high
tx_pin_high macro
	bsf	iolatch, TX_PIN
	movf	iolatch, w
	movwf	GPIO
	endm

	;; Bring the transmit pin low
tx_pin_low macro
	bcf	iolatch, TX_PIN
	movf	iolatch, w
	movwf	GPIO
	endm

	;; Send the preamble, a square wave that helps the
	;; receiver's automatic gain control settle.
	;; This can be done while we do other fun things, like
	;; let the temperature A/D conversion run.
tx_send_preamble
	movlw	PREAMBLE_LENGTH
	movwf	temp
preamble_loop
	tx_wait_timer
	tx_add_timer	TX_SHORT_DELAY
	tx_pin_high
	tx_wait_timer
	tx_add_timer	TX_SHORT_DELAY
	tx_pin_low
	decfsz	temp, f
	goto	preamble_loop
	return

	;; Start a new transmission
tx_send_start
	tx_pin_low
	tx_set_timer 0
	return

	;; End a transmission (wait for the last bit to finish)
tx_send_end
	tx_wait_timer
	return


	;; Send a '1' bit
tx_send_one
	tx_wait_timer		; Let the previous bit end
	tx_add_timer	TX_LONG_DELAY
	tx_pin_high
	tx_wait_timer
	tx_add_timer	TX_SHORT_DELAY
	tx_pin_low
	return

	;; Send a '0' bit
tx_send_zero
	tx_wait_timer		; Let the previous bit end
	tx_add_timer	TX_SHORT_DELAY
	tx_pin_high
	tx_wait_timer
	tx_add_timer	TX_LONG_DELAY
	tx_pin_low
	return

	end


