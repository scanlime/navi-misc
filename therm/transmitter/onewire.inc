;
; A Dallas 1-wire master implementation
; Copyright 2004, Micah Dowty <micah@navi.cx>
;
; NOTE: Currently this only works at 4 MHz (1us instruction period)
;

;; ROM commands

SEARCH_ROM	equ	0xF0
READ_ROM	equ	0x33
MATCH_ROM	equ	0x55
SKIP_ROM	equ	0xCC
ALARM_SEARCH	equ	0xEC

;----------------------------------------------------- I/O Primitives


	;; Pull DQ high weakly, using the external 4.7k resistor only.
	;; Executes in 4 clock cycles
onewire_dq_high	macro
	movlw	DEFAULT_TRIS | DQ_MASK
	bsf	STATUS, RP0
	movwf	TRISIO
	bcf	STATUS, RP0
	endm

	;; Pull DQ low actively. Executes in 7 clock cycles.
onewire_dq_low macro
	bcf	iolatch, DQ_PIN
	movf	iolatch, w
	movwf	GPIO
	movlw	DEFAULT_TRIS & (~DQ_MASK)
	bsf	STATUS, RP0
	movwf	TRISIO
	bcf	STATUS, RP0
	endm

	;; Pull DQ high actively. Executes in 7 clock cycles.
onewire_dq_strong_pullup macro
	bsf	iolatch, DQ_PIN
	movf	iolatch, w
	movwf	GPIO
	movlw	DEFAULT_TRIS & (~DQ_MASK)
	bsf	STATUS, RP0
	movwf	TRISIO
	bcf	STATUS, RP0
	endm

	;; Shift DQ into the MSB of 'temp'. Samples 2us in.
onewire_dq_read	macro
	rrf	temp, f
	bcf	temp, 7
	btfsc	GPIO, DQ_PIN
	bsf	temp, 7
	endm

delay_10us macro
	goto	$+1
	goto	$+1
	goto	$+1
	goto	$+1
	goto	$+1
	endm

	;; Define non-macro I2C routines
onewire_define_all	macro


;----------------------------------------------------- Time slots

delay_60us
	delay_10us
	delay_10us
	delay_10us
	delay_10us
	delay_10us
	delay_10us
	return


	;; Issue a reset pulse, shift the response into 'temp'
onewire_slot_reset
	onewire_dq_low
	call	delay_60us
	call	delay_60us
	call	delay_60us
	call	delay_60us
	call	delay_60us
	call	delay_60us
	call	delay_60us
	call	delay_60us
	onewire_dq_high
	call	delay_60us
	onewire_dq_read
	call	delay_60us
	call	delay_60us
	call	delay_60us
	call	delay_60us
	call	delay_60us
	call	delay_60us
	call	delay_60us
	return


	;; Write a 0. This pulls DQ low for a little over 60us.
onewire_slot_write_0
	onewire_dq_low
	call	delay_60us
	onewire_dq_high
	return

	;; Write a 1. This pulls DQ low for 5us then leaves it high for the rest of the slot
onewire_slot_write_1
	onewire_dq_low
	onewire_dq_high
	call	delay_60us
	return


	;; Shift out the LSB from 'temp'
onewire_slot_write
	rrf	temp, f
	btfsc	STATUS, C
	goto	onewire_slot_write_1
	goto	onewire_slot_write_0


	;; Issue a read timeslot. This samples 15us after DQ initially goes low.
	;; The read bit is shifted into the MSB side of 'temp'.
onewire_slot_read
	onewire_dq_low
	onewire_dq_high
	goto	$+1
	goto	$+1
	goto	$+1
	goto	$+1
	onewire_dq_read
	call	delay_60us
	return


;----------------------------------------------------- Reading/writing bytes

onewire_write_byte
	call	onewire_slot_write
	call	onewire_slot_write
	call	onewire_slot_write
	call	onewire_slot_write
	call	onewire_slot_write
	call	onewire_slot_write
	call	onewire_slot_write
	call	onewire_slot_write
	return

onewire_read_byte
	call	onewire_slot_read
	call	onewire_slot_read
	call	onewire_slot_read
	call	onewire_slot_read
	call	onewire_slot_read
	call	onewire_slot_read
	call	onewire_slot_read
	call	onewire_slot_read
	return

	endm

onewire_write_const macro x
	movlw	x
	movwf	temp
	call	onewire_write_byte
	endm

	;; The End
