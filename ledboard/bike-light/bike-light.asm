;
; PWM controller for a simple high power bicycle headlight
; Copyright (C) 2005 Micah Dowty <micah@navi.cx>
;
; This whole thing is really simple, quick, and dirty.. but
; it really shouldn't have to be complicated.
;
; Hardware description:
;      GP0: Output to LED drive MOSFET, active-high
;      GP1: Unused, set as input
;      GP2: Unused, set as output
;      GP3: Unused, set as output
;      GP4: Unused, set as input
;      GP5: Input from pushbutton, active-low

;-----------------------------------------------------

include "hardware_p12f675.inc"

LED_PIN		equ 0
UNUSED1_PIN	equ 1
UNUSED2_PIN	equ 2
UNUSED3_PIN	equ 3
UNUSED4_PIN	equ 4
BUTTON_PIN	equ 5

LED_MASK	equ 0x01
UNUSED1_MASK	equ 0x02
UNUSED2_MASK	equ 0x04
UNUSED3_MASK	equ 0x08
UNUSED4_MASK	equ 0x10
BUTTON_MASK	equ 0x20

DEFAULT_GPIO	equ 0
DEFAULT_TRIS	equ UNUSED1_MASK | UNUSED4_MASK | BUTTON_MASK

	cblock 0x20
		temp, pwm_iter, brightness
	endc

	org 0			; Reset vector
	goto	start

	org 4			; Interrupt vector
	retfie

start
	;; I/O Initialization
	movlw	DEFAULT_GPIO
	movwf	GPIO
	movlw	DEFAULT_TRIS
	bsf	STATUS, RP0
	movwf	TRISIO
	bcf	STATUS, RP0

	;; Variable initialization
	clrf	pwm_iter
	clrf	brightness

	;; For now, the button just cycles through brightness levels
	;; and sleep mode.
main_loop
	call	sleep_until_button_press ; Sleep mode, LEDs off

	movlw	0x10
	movwf	brightness
	call	wait_button_press

	movlw	0x20
	movwf	brightness
	call	wait_button_press

	movlw	0x40
	movwf	brightness
	call	wait_button_press

	movlw	0x80
	movwf	brightness
	call	wait_button_press

	movlw	0xFF
	movwf	brightness
	call	wait_button_press

	goto	main_loop

;-----------------------------------------------------

	;; Generate one PWM cycle for the LEDs. The
	;; LEDs are only ever turned on during this call,
	;; so we can't quite reach 100% brightness using
	;; this method. That's ok, since 100% is really
	;; awfully bright. Note that this also can't quite
	;; hit 0% brightness. That simplifies the code,
	;; and is perfectly fine since we go to sleep when
	;; the light is completely off.
led_pwm_cycle
	bsf	GPIO, LED_PIN
pwm_loop

	movf	pwm_iter, w	; Turn off the LED when pwm_iter
	xorwf	brightness, w	;   crosses our brightness setting.
	btfsc	STATUS, Z
	bcf	GPIO, LED_PIN

	call	pwm_delay	; Limit the rate this runs at,
				;   to limit RF interference and button bounce

	incf	pwm_iter, f	; Loop until pwm_iter crosses zero
	btfss	STATUS, Z
	goto	pwm_loop
	return


pwm_delay
	movlw	0x04
	movwf	temp
pwm_delay_loop
	decfsz	temp, f
	goto	pwm_delay_loop
	return


;-----------------------------------------------------

wait_button_down
	call	led_pwm_cycle
	btfsc	GPIO, BUTTON_PIN
	goto	wait_button_down
	return

wait_button_up
	call	led_pwm_cycle
	btfss	GPIO, BUTTON_PIN
	goto	wait_button_up
	return

wait_button_press
	call	wait_button_up
	call	wait_button_down
	return

sleep_until_button_press
	call	wait_button_up
	movlw	BUTTON_MASK	; Set up interrupt-on-change for the button
	bsf	STATUS, RP0
	movwf	IOC
	bcf	STATUS, RP0
	movlw	0x88		; Enable port change interrupt, clear flags
	movwf	INTCON
	sleep			; Sleep until the interrupt wakes us
	clrf	INTCON		; Disable interrupts
	return

	end
