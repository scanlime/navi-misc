;
; Utilities for sampling the DS18B20 1-wire temperature sensor
; Copyright 2004, Micah Dowty <micah@navi.cx>
;

;; DS18B20 function commands

CONVERT_T	equ	0x44
WRITE_SCRATCHPAD equ	0x4E
READ_SCRATCHPAD	equ	0xBE
COPY_SCRATCHPAD	equ	0x48
RECALL_EE	equ	0xB8
READ_POWER_SUPPLY equ	0xB4


#define	 ds18b20_define_vars	therm_total_low, therm_total_high, therm_count


	;; Address the device, assuming it's alone on the bus.
	;; No error checking yet.
ds18b20_address macro
	call	onewire_slot_reset
	onewire_write_const SKIP_ROM
	endm


	;; Initialize the device, and start it sampling
ds18b20_start macro
	ds18b20_address
	onewire_write_const CONVERT_T
	onewire_dq_strong_pullup	; Assume parasite power, give the device
					;  a strong pullup during conversion.
	endm


	;; Read and store one sample from the DS18B20 after conversion is done
ds18b20_read macro
	onewire_dq_high			; Go back to a regular weak pullup
	ds18b20_address
	onewire_write_const READ_SCRATCHPAD ; Start reading back temperatures (first 2 bytes of scratchpad)

	call	onewire_read_byte	; Read and accumulate the LSB
	movf	temp, w
	addwf	therm_total_low, f
	btfsc	STATUS, C
	incf	therm_total_high, f

	call	onewire_read_byte	; Read and accumulate the MSB
	movf	temp, w
	addwf	therm_total_high, f

	movlw	SAMPLE_DIVISOR		; Add to the denominator
	addwf	therm_count, f
	endm


	;; Clear the temperature accumulator
ds18b20_flush macro
	clrf	therm_total_low
	clrf	therm_total_high
	clrf	therm_count
	endm

	;; The End
