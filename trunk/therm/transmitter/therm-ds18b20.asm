;
; Wireless temperature sensor, single DS18B20 version
; Copyright 2004, Micah Dowty <micah@navi.cx>
;
; This samples a Dallas 1-wire temperature sensor (DS18B20) and sends back
; results over a Laipac 315MHz ASK transmitter. The protocol is described
; in tx-protocol.inc. This version supports a single sensor, and uses
; protocol 0.
;
; Hardware description:
;      GP0: Battery voltage sensor (470Kohms to battery+, 470K to ground)
;      GP1: Transmitter data
;      GP2: Transmitter power
;      GP3: Unused input. For convenience, tied to GP4
;      GP4: 1-wire bus, 4.7k pullup resistor to VCC
;      GP5: Unused, configured as an output and not connected
;
; The 1-wire sensor should be configured for parasite power (GND and VCC
; grounded, DQ connected to the PIC's GP4)
;
; Packet content for protocol 0, starting after the 2-bit protocol ID
; and ending before the CRC-16:
;
;  - 6-bit station ID
;  - 5-bit packet sequence number
;  - 10-bit battery voltage
;  - 16-bit temperature numerator (total)
;  - 8-bit temperature denominator (sample count << 4)
;

;----------------------------------------------------- Device configuration

STATION_ID	equ	4	; A unique ID between 0 and 63

SAMPLE_DELAY	equ	.5	; Delay between temperature readings, in 2.3-second units
N_THERM_SAMPLES	equ	.10	; Number of temperature readings per RF burst.

N_PACKETS	equ	.5	; Number of duplicate packets sent in a burst


;-----------------------------------------------------

include "hardware_p12f683.inc"

include "tx-protocol.inc"
include "power.inc"
include "ds18b20.inc"
include "onewire.inc"

BATT_VOLT_PIN	equ 0
TX_PIN		equ 1
TX_POWER_PIN	equ 2
UNUSED1_PIN	equ 3
DQ_PIN		equ 4
UNUSED2_PIN	equ 5

BATT_VOLT_MASK	equ 0x01
TX_MASK		equ 0x02
TX_POWER_MASK	equ 0x04
UNUSED1_MASK	equ 0x08
DQ_MASK		equ 0x10
UNUSED2_MASK	equ 0x20

DEFAULT_GPIO	equ 0
DEFAULT_TRIS	equ BATT_VOLT_MASK | UNUSED1_MASK | DQ_MASK


	cblock 0x20
		iolatch
		temp, temp2
		main_iter, packet_seq

		ds18b20_define_vars
		tx_define_vars
	endc

	org 0

	;; One-time-only setup
	clrf	packet_seq
	ds18b20_flush
	init_low_power

main_loop
	incf	packet_seq, f

	battvolts_acquire		; Take a short nap while acquiring battery voltage
	init_timer_sleep_288msec	;   (use a lot of precharge time due to the very high impedance)
	sleep
	init_high_power			; Set up power and timers for transmitting
	init_timer_normal
	battvolts_sample		; Start the battery voltage A/D conversion
	call	tx_send_preamble	; Send preamble bits while sampling

	movlw	N_PACKETS		; Send a burst of packets. Note that the first packet we send after
	movwf	main_iter		;   powering up will have no therm data (zero samples).
packet_burst_loop			;   This lets the receiver detect resets or battery changes if necessary.
	tx_packet_proto0
	decfsz	main_iter, f
	goto	packet_burst_loop

	ds18b20_flush			; Take several therm readings...
	movlw	N_THERM_SAMPLES
	movwf	main_iter
therm_sample_loop

	init_low_power			; Ask the sensor to start sampling
	ds18b20_start
	init_timer_sleep_1152msec	; Take a short nap while it samples
	sleep
	ds18b20_read			; Read the sample and shut it down

	init_low_power			; Go into low power long-duration sleep between samples
	init_timer_sleep_2304msec
	sleep_n	SAMPLE_DELAY

	decfsz	main_iter, f		; Next therm sample...
	goto	therm_sample_loop

	goto	main_loop		; Next RF packet...


	onewire_define_all
	tx_define_all_layers

	end
