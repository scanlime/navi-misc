;
; Transmit-side implementation of the wireless therm protocol.
; Copyright 2004, Micah Dowty <micah@navi.cx>
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
;         - 2-bit protocol ID
;         - Arbitrary protocol-defined data
;	  - 16-bit CRC-16 of the above
;      - 6-bit Flag sequence
;

;----------------------------------------------------- Constants

PREAMBLE_LENGTH	equ .128

TX_SHORT	equ .312	; Transmit timings, in microseconds
TX_LONG		equ .730

TX_SHORT_DELAY	equ (.258 - (TX_SHORT / 4))
TX_LONG_DELAY	equ (.258 - (TX_LONG / 4))


#define	 tx_define_vars	consecutive_ones, packet_seq, crc_reg_high, crc_reg_low


;----------------------------------------------------- Protocol: Frame check sequence

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

tx_define_framing_layer macro

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

	endm

;----------------------------------------------------- Protocol: Bit stuffing and flags

tx_define_bitstuffing_layer macro

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

	endm

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

tx_define_timing_layer macro

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

	endm

;----------------------------------------------------- Other Macros

tx_define_all_layers	macro
	tx_define_framing_layer
	tx_define_bitstuffing_layer
	tx_define_timing_layer
	endm

;; The End

