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
;      GP2: Transmitter power
;      GP3: Unused input. For convenience, tied to GP4
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
;         - 16-bit temperature numerator (total)
;         - 8-bit temperature denominator (sample count)
;	  - 16-bit CRC-16 of the above
;      - 6-bit Flag sequence
;

	list	p=12f675
	#include p12f675.inc

	errorlevel -302
	__CONFIG _CPD_OFF & _CP_OFF & _BODEN_ON & _MCLRE_OFF & _PWRTE_ON & _WDT_ON & _INTRC_OSC_NOCLKOUT


;----------------------------------------------------- Device configuration

STATION_ID	equ	2	; A unique ID between 0 and 63
TC74_ADDR	equ	5	; The address marked on this station's TC74

SAMPLE_DELAY	equ	.4	; Delay between temperature readings, in 2.3-second units
N_THERM_SAMPLES	equ	.15	; Number of temperature readings per RF burst.

N_PACKETS	equ	.3	; Number of duplicate packets sent in a burst


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
		iolatch
		temp, temp2
		main_iter
		consecutive_ones, packet_seq
		crc_reg_high, crc_reg_low
		therm_total_low, therm_total_high, therm_count
	endc


;----------------------------------------------------- Main Loop
	org 0

	;; One-time-only setup
	clrf	packet_seq
	call	tc74_flush
	call	init_low_power

main_loop
	incf	packet_seq, f

	call	battvolts_acquire	; Take a short nap while acquiring battery voltage
	call	init_timer_nap		;   (use a lot of precharge time due to the very high impedance)
	sleep
	call	init_high_power		; Set up power and timers for transmitting
	call	init_timer_normal
	call	battvolts_sample	; Start the battery voltage A/D conversion
	call	tx_send_preamble	; Send preamble bits while sampling

	movlw	N_PACKETS		; Send a burst of packets. Note that the first packet we send after
	movwf	main_iter		;   powering up will have no therm data (zero samples).
packet_burst_loop			;   This lets the receiver detect resets or battery changes if necessary.
	call	tx_packet
	decfsz	main_iter, f
	goto	packet_burst_loop

	call	tc74_flush		; Take several therm readings...
	movlw	N_THERM_SAMPLES
	movwf	main_iter
therm_sample_loop

	call	init_low_power		; Turn on the TC74
	call	tc74_init
	call	init_timer_nap		; Take a short nap while it samples
	sleep
	call	tc74_read		; Read the sample and shut it down
	call	tc74_shutdown

	call	init_low_power		; Go into low power long-duration sleep between samples
	call	init_timer_sleep
	movlw	SAMPLE_DELAY
	movwf	temp
long_sleep_loop
	sleep
	decfsz	temp, f
	goto	long_sleep_loop

	decfsz	main_iter, f		; Next therm sample...
	goto	therm_sample_loop

	goto	main_loop		; Next RF packet...


;----------------------------------------------------- Power Management

	;; Initialize I/O with the transmitter off
init_low_power
	movlw	SCL_MASK
	movwf	iolatch
	movwf	GPIO
	movlw	BATT_VOLT_MASK | UNUSED_MASK | SDA_MASK
	bsf	STATUS, RP0
	movwf	TRISIO
	bcf	STATUS, RP0

	clrf	ADCON0		; Turn off the A/D converter
	return

	; Initialize I/O with the transmitter on
init_high_power
	movlw	POWER_MASK | SCL_MASK
	movwf	iolatch
	movwf	GPIO
	movlw	BATT_VOLT_MASK | UNUSED_MASK | SDA_MASK
	bsf	STATUS, RP0
	movwf	TRISIO
	bcf	STATUS, RP0
	return

	;; Prepare the timers for normal use
init_timer_normal
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
	return

	;; Prepare the timers for the longest period sleep possible, 2.3 seconds
init_timer_sleep
	clrwdt			; Clear the watchdog
	clrf	TMR0		; Clear the prescaler
	movlw	0xDF		; Reassign prescaler (1:128)
	bsf	STATUS, RP0
	movwf	OPTION_REG
	bcf	STATUS, RP0
	return

	;; Prepare the timers for a short 0.28 second nap, long enough for a temperature sample
init_timer_nap
	clrwdt			; Clear the watchdog
	clrf	TMR0		; Clear the prescaler
	movlw	0xDC		; Reassign prescaler (1:16)
	bsf	STATUS, RP0
	movwf	OPTION_REG
	bcf	STATUS, RP0
	return

	;; Start acquiring the battery voltage
battvolts_acquire
	bsf	STATUS, RP0
	movlw	0x31		; RC osc, only AN0 is analog
	movwf	ANSEL
	bcf	STATUS, RP0
	movlw	0x81		; Right justified, VDD reference, channel 0, converter on
	movwf	ADCON0
	return

	;; Start sampling the battery voltage
battvolts_sample
	bsf	ADCON0, GO
	return


;----------------------------------------------------- Temperature Sensor

	;; Take the TC74 out of standby mode if necessary
tc74_init
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
	return

	;; Put the TC74 into its own low-power shutdown mode
tc74_shutdown
	call	i2c_start
	movlw	TC74_ADDR_WRITE	; Address it
	movwf	temp
	call	i2c_send_byte
	movlw	0x01		; Select the CONFIG register
	movwf	temp
	call	i2c_send_byte
	movlw	0x80		; Into standby mode
	movwf	temp
	call	i2c_send_byte
	call	i2c_stop
	return

	;; Read and store one sample from the TC74
tc74_read
	call	i2c_start
	movlw	TC74_ADDR_WRITE	; Address it
	movwf	temp
	call	i2c_send_byte
	movlw	0x00		; Select the TEMP register
	movwf	temp
	call	i2c_send_byte
	call	i2c_stop

	call	i2c_start
	movlw	TC74_ADDR_READ	; Address it for reading
	movwf	temp
	call	i2c_send_byte
	call	i2c_read_byte	; Read the result
	call	i2c_stop

	btfss	temp2, 0	; If the read was unsuccessful, return now
	return

	incf	therm_count, f	; Accumulate this sample
	movf	temp, w
	addwf	therm_total_low, f
	btfsc	STATUS, C	; Carry the one...
	incf	therm_total_high, f
	btfsc	temp, 7		; Sign extend
	decf	therm_total_high, f
	return


	;; Clear the temperature accumulator
tc74_flush
	clrf	therm_total_low
	clrf	therm_total_high
	clrf	therm_count
	return


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
	call	i2c_delay
	i2c_scl_low
	return

	;; Shift one bit into 'temp'
i2c_read_bit
	rlf	temp, f
	bcf	temp, 0
	i2c_scl_high
	call	i2c_delay
	btfsc	GPIO, SDA_PIN
	bsf	temp, 0
	i2c_scl_low
	call	i2c_delay
	return

	;; Shift one byte into 'temp'. Returns with temp2=1 on success,
	;; temp2=0 on failure.
i2c_read_byte
	call	i2c_read_bit
	call	i2c_read_bit
	call	i2c_read_bit
	call	i2c_read_bit
	call	i2c_read_bit
	call	i2c_read_bit
	call	i2c_read_bit
	call	i2c_read_bit

	i2c_sda_high		; Clock in the ACK
	call	i2c_delay
	i2c_scl_high
	call	i2c_delay
	clrf	temp2		; Read the ACK, store our return code
	btfsc	GPIO, SDA_PIN
	bsf	temp2, 0
	i2c_scl_low
	return


;----------------------------------------------------- Protocol: Complete packets

tx_packet
	call	tx_begin_content

	movlw	0		; 2-bit protocol ID
	movwf	temp
	call	tx_content_2bit

	movlw	STATION_ID	; 6-bit station Id
	movwf	temp
	call	tx_content_6bit

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

	movf	therm_total_low, w ; 16-bit temperature total
	movwf	temp
	call	tx_content_8bit
	movf	therm_total_high, w
	movwf	temp
	call	tx_content_8bit

	movf	therm_count, w	; 8-bit sample count
	movwf	temp
	call	tx_content_8bit

	call	tx_end_content
	return


;----------------------------------------------------- Protocol: Frame check sequence

	;; Reinitialize the CRC8 and send a few copies of the flag sequence
tx_begin_content
	clrf	crc_reg_low
	clrf	crc_reg_high
	call	tx_send_start
	call	tx_send_flag
	call	tx_send_flag
	call	tx_send_flag
	call	tx_send_flag
	return

	;; A macro to shift a new bit into the CRC
crc_shift macro value
	local	skip_poly
	rlf	crc_reg_low, f
	rlf	crc_reg_high, f
	if value
	 bsf	crc_reg_low, 0
	else
	 bcf	crc_reg_low, 0
	endif
	btfss	STATUS, C	; XOR with the polynomial if 1 came out
	goto	skip_poly
	movlw	0x05
	xorwf	crc_reg_low, f
	movlw	0x80
	xorwf	crc_reg_high, f
skip_poly
	endm

	;; Send the CRC16 followed by a flag
tx_end_content
	movlw	.16		; Augment the message with 16 zero bits
	movwf	temp
message_augment
	crc_shift 0
	decfsz	temp, f
	goto	message_augment

	movf	crc_reg_low, w	; Send the CRC
	movwf	temp
	movf	crc_reg_high, w	; We have to save the high byte, since transmitting
	movwf	temp2		;   the low byte will alter it
	call	tx_content_8bit
	movf	temp2, w
	movwf	temp
	call	tx_content_8bit

	call	tx_send_flag	; Send the flag
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


