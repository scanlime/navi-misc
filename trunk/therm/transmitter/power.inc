;
; Power management macro library
; Copyright 2004, Micah Dowty <micah@navi.cx>
;

	;; Initialize I/O with the transmitter off
init_low_power macro
	movlw	SCL_MASK
	movwf	iolatch
	movwf	GPIO
	movlw	BATT_VOLT_MASK | UNUSED_MASK | SDA_MASK
	bsf	STATUS, RP0
	movwf	TRISIO
	bcf	STATUS, RP0

	clrf	ADCON0		; Turn off the A/D converter
	endm

	; Initialize I/O with the transmitter on
init_high_power macro
	movlw	POWER_MASK | SCL_MASK
	movwf	iolatch
	movwf	GPIO
	movlw	BATT_VOLT_MASK | UNUSED_MASK | SDA_MASK
	bsf	STATUS, RP0
	movwf	TRISIO
	bcf	STATUS, RP0
	endm

	;; Prepare the timers for normal use
init_timer_normal macro
	calibrate_oscillator

	;; Initialize TMR0 and the watchdog timer. Initially, we assign
	;; a prescaler to TMR0 so we can time delays long enough for our
	;; slow bits. Before sleeping we swap that over to the WDT.
	clrwdt
	if CLOCK_MHZ == 4
	  movlw	0xD1		; 1:4
	endif
	if CLOCK_MHZ == 8
	  movlw 0xD2		; 1:8
	endif
	bsf	STATUS, RP0
	movwf	OPTION_REG
	bcf	STATUS, RP0
	clrf	TMR0
	clrf	INTCON
	clrwdt
	endm

	;; Prepare the timers for sleep, with a user-specified prescaler
init_timer_sleep_arbitrary macro prescaler
	clrwdt			; Clear the watchdog
	clrf	TMR0		; Clear the prescaler
	movlw	0xD8 | prescaler
	bsf	STATUS, RP0
	movwf	OPTION_REG
	bcf	STATUS, RP0
	endm

	;; Prepare for sleep with various fixed delays
init_timer_sleep_2304msec macro
	init_timer_sleep_arbitrary 7	; 1:128
	endm
init_timer_sleep_1152msec macro
	init_timer_sleep_arbitrary 6	; 1:64
	endm
init_timer_sleep_576msec macro
	init_timer_sleep_arbitrary 5	; 1:32
	endm
init_timer_sleep_288msec macro
	init_timer_sleep_arbitrary 4	; 1:16
	endm
init_timer_sleep_144msec macro
	init_timer_sleep_arbitrary 3	; 1:8
	endm
init_timer_sleep_72msec macro
	init_timer_sleep_arbitrary 2	; 1:4
	endm
init_timer_sleep_36msec macro
	init_timer_sleep_arbitrary 1	; 1:2
	endm
init_timer_sleep_18msec macro
	init_timer_sleep_arbitrary 0	; 1:1
	endm

	;; Start acquiring the battery voltage
battvolts_acquire macro
	bsf	STATUS, RP0
	movlw	0x31		; RC osc, only AN0 is analog
	movwf	ANSEL
	bcf	STATUS, RP0
	movlw	0x81		; Right justified, VDD reference, channel 0, converter on
	movwf	ADCON0
	endm

	;; Start sampling the battery voltage
battvolts_sample macro
	bsf	ADCON0, GO
	endm

	;; Sleep for a constant number of iterations
sleep_n	macro n
	local	sleep_loop
	if n > 0
	 movlw	n
	 movwf	temp
sleep_loop
	 sleep
	 decfsz	temp, f
	 goto	sleep_loop
	endif
	endm

	;; The End

