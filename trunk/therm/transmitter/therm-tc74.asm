;
; Wireless temperature sensor, single TC74 version
; Copyright 2004, Micah Dowty <micah@navi.cx>
;
; This samples an I2C temperature sensor (Microchip TC74) and sends back
; results over a Laipac 315MHz ASK transmitter. The protocol is described
; in tx-protocol.inc. This version supports a single sensor, and uses
; protocol 0.
;
; Hardware description:
;      GP0: Battery voltage sensor (470Kohms to battery+, 470K to ground)
;      GP1: Transmitter data
;      GP2: Transmitter power
;      GP3: Unused input. For convenience, tied to GP4
;      GP4: I2C data, 4.7k pullup resistor to VCC
;      GP5: I2C clock
;
; Packet content for protocol 0, starting after the 2-bit protocol ID
; and ending before the CRC-16:
;
;  - 6-bit station ID
;  - 5-bit packet sequence number
;  - 10-bit battery voltage
;  - 16-bit temperature numerator (total)
;  - 8-bit temperature denominator (sample count)
;

;----------------------------------------------------- Device configuration

STATION_ID	equ	1	; A unique ID between 0 and 63
TC74_ADDR	equ	7	; The address marked on this station's TC74

SAMPLE_DELAY	equ	.4	; Delay between temperature readings, in 2.3-second units
N_THERM_SAMPLES	equ	.15	; Number of temperature readings per RF burst.

N_PACKETS	equ	.5	; Number of duplicate packets sent in a burst


;-----------------------------------------------------

include "hardware_p12f683.inc"

include "tx-protocol.inc"
include "power.inc"
include "i2c.inc"
include "tc74.inc"

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


	cblock 0x20
		iolatch
		temp, temp2
		main_iter, packet_seq

		tc74_define_vars
		tx_define_vars
	endc

	org 0

	;; One-time-only setup
	clrf	packet_seq
	tc74_flush
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

	tc74_flush			; Take several therm readings...
	movlw	N_THERM_SAMPLES
	movwf	main_iter
therm_sample_loop

	init_low_power			; Turn on the TC74
	tc74_init
	init_timer_sleep_288msec	; Take a short nap while it samples
	sleep
	tc74_read			; Read the sample and shut it down
	tc74_shutdown

	init_low_power			; Go into low power long-duration sleep between samples
	init_timer_sleep_2304msec
	sleep_n	SAMPLE_DELAY

	decfsz	main_iter, f		; Next therm sample...
	goto	therm_sample_loop

	goto	main_loop		; Next RF packet...


	i2c_define_all
	tx_define_all_layers

	end
