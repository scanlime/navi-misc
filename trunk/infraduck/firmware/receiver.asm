;
; Lightgun IR receiver board
; Copyright 2003-2004, Micah Dowty <micah@navi.picogui.org>
;
; This is firmware for an IR receiver board using a PIC16F84
; and a Sharp GP1U or equivalent IR receiver module.
;
; Hardware description:
;     RB0: Input from IR receiver
;     RA1: Yellow LED
;     RA0: Green LED
;
; PIC configuration:
;     - 6 MHz crystal oscillator
;     - No watchdog
;
; Protocol:
;   See protocol description in lightgun.asm for the transmitter's timing.
;   Actual values measured at the receiver:
;        Header: 6.5ms on, 3ms off
;       "1" bit: 0.6ms on, 0.4ms off
;       "0" bit: 0.6ms on, 1.3ms off
;        Footer: 3.4ms on
;
;   The timing differences caused by the GP1U receiver are asymmetric
;   between on and off times.
;

	list	p=16f84a
	#include p16f84a.inc


;----------------------------------------------------- Constants

; I/O
tristateMaskA	equ	b'00000000'
tristateMaskB	equ	b'00000001'
#define		greenLED	PORTA, 1
#define		yellowLED	PORTA, 0
#define		IRsensor	PORTB, 0

; IR timing, in 42.6us units
IRheaderMarkMin	equ	D'60'	; Should be 153, but this allows for a truncated header
IRheaderSpace	equ	D'70'
IRbitMark	equ	D'14'
IRoneSpace	equ	D'9'
IRzeroSpace	equ	D'31'
IRtolerance	equ	D'2'

; IR commands
cmdTrigger	equ	b'01111111'
cmdLightSensor	equ	b'10111111'


;----------------------------------------------------- Variables

	cblock 0C
	  receivedDevice, receivedCommand, delayTemp1, delayTemp2
	  markCycles, spaceCycles, IRword1, IRword2, IRbitCount
	  savedStatus, savedW
	endc


;----------------------------------------------------- Interrupt handler
	org	0
	goto	main
	org	4

	; Initialize interrupt handler
	movwf	savedW			; Save W
	swapf	STATUS, w		; Save STATUS
	movwf	savedStatus
	bcf	STATUS, RP0		; Select page 0
	bcf	INTCON, INTF		; Clear interrupt flag

	; Pulse width detection
	movf	TMR0, w			; Save timer value
	clrf	TMR0			; Clear timer
	movwf	spaceCycles		; If the sensor was off, this goes in spaceCycles
	btfsc	IRsensor		; If it's off now, it was on before the interrupt
	movwf	markCycles		; If it was on, this goes in markCycles

	; Set up an interrupt on the next pulse edge
	btfss	IRsensor		; INTEDG = !IRsensor
	goto	setIntedg
clearIntedg
	bsf	STATUS, RP0		; OPTION_REG is in page 1
	bcf	OPTION_REG, INTEDG
	bcf	STATUS, RP0
	goto	doneIntedg
setIntedg
	bsf	STATUS, RP0		; OPTION_REG is in page 1
	bsf	OPTION_REG, INTEDG
	bcf	STATUS, RP0
doneIntedg
	
	; Only do code detection after a space
	btfsc	IRsensor
	goto	interruptDone	

	; Was this a header?
	movlw	IRheaderMarkMin
	subwf	markCycles, w
	btfss	STATUS, C
	goto	notHeader			; Mark too short
	movlw	IRheaderSpace - IRtolerance
	subwf	spaceCycles, w
	btfss	STATUS, C
	goto	notHeader			; Space too short
	movlw	IRheaderSpace + IRtolerance
	subwf	spaceCycles, w
	btfsc	STATUS, C
	goto	notHeader			; Space too long
	clrf	IRbitCount			; Clear counter
	clrf	IRword1				; Clear word
	clrf	IRword2	
	goto	interruptDone
notHeader

	; Have we received a valid header?
	movlw	0FF
	subwf	IRbitCount, w
	btfsc	STATUS, Z
	goto	interruptDone

	; Was the mark the correct length for a one or a zero?
	movlw	IRbitMark - IRtolerance
	subwf	markCycles, w
	btfss	STATUS, C
	goto	invalidCode			; Mark too short
	movlw	IRbitMark + IRtolerance
	subwf	markCycles, w
	btfsc	STATUS, C
	goto	invalidCode			; Mark too long
	
	; Was it a one?
	movlw	IRoneSpace - IRtolerance
	subwf	spaceCycles, w
	btfss	STATUS, C
	goto	notOne				; Space too short
	movlw	IRoneSpace + IRtolerance
	subwf	spaceCycles, w
	btfsc	STATUS, C
	goto	notOne				; Space too long	
	bsf	STATUS, C			; Prepare to shift in a one
	goto	gotBit
notOne

	; Was it a zero?
	movlw	IRzeroSpace - IRtolerance
	subwf	spaceCycles, w
	btfss	STATUS, C
	goto	invalidCode			; Space too short
	movlw	IRzeroSpace + IRtolerance
	subwf	spaceCycles, w
	btfsc	STATUS, C
	goto	invalidCode			; Space too long	
	bcf	STATUS, C			; Prepare to shift in a zero

	; Shift in a newly received bit
gotBit
	rlf	IRword2, f
	rlf	IRword1, f
	incf	IRbitCount, f
	movlw	10
	subwf	IRbitCount, w
	btfss	STATUS, Z
	goto	interruptDone			; Need more bits

	; Latch in a new word
	movf	IRword1, w
	movwf	receivedDevice
	movf	IRword2, w
	movwf	receivedCommand
	goto	invalidCode			; Require a header before any more bits

	; Restore registers and return
interruptDone
	swapf	savedStatus, w
	movwf	STATUS
	swapf	savedW, f
	swapf	savedW, w
	retfie
	
	; Don't accept any more bits until a header
invalidCode
	movlw	0FF
	movwf	IRbitCount
	goto	interruptDone


;----------------------------------------------------- Main program
main

	; Initialize I/O
	clrf	PORTA
	clrf	PORTB
	bsf	STATUS, RP0	; TRIS registers are in the second page
	movlw	tristateMaskA
	movwf	TRISA
	movlw	tristateMaskB
	movwf	TRISB
	bcf	STATUS, RP0

	; Set up the interrupt handler- we want an interrupt on Timer 0
	; which we use to measure pulse length, plus an interrupt on change
	; for the IR receiver. When a command is detected, it will be
	; placed in the receivedDevice and receivedCommand vars.
	bsf	STATUS, RP0	; OPTION is in the second page
	movlw	B'11000101'	; TMR0 == Fosc / 256
	movwf	OPTION_REG
	bcf	STATUS, RP0
	clrf	TMR0		; Reset timer
	movlw	B'10010000'	; Enable RB0 interrupt only
	movwf	INTCON

	; Blink both LEDs on power on, just for fun
	bsf	yellowLED
	call	ledDelay
	bsf	greenLED
	call	ledDelay
	bcf	yellowLED
	call	ledDelay
	bcf	greenLED

	; Most of the work is done in the interrupt handler-
	; For now, just dispatch handlers for each command.
mainLoop

	movlw	cmdTrigger
	subwf	receivedCommand, w
	btfsc	STATUS, Z
	call	gotCmdTrigger
	
	movlw	cmdLightSensor
	subwf	receivedCommand, w
	btfsc	STATUS, Z
	call	gotCmdLightSensor

	clrf	receivedCommand
	clrf	receivedDevice
	goto	mainLoop


;----------------------------------------------------- Command handlers

gotCmdTrigger
	bsf	yellowLED
	call	ledDelay
	bcf	yellowLED
	retlw	0

gotCmdLightSensor
	bsf	greenLED
	call	ledDelay
	bcf	greenLED
	retlw	0


;----------------------------------------------------- Utilities

	; Delay long enough for an LED flash to be easily visible.
ledDelay
	movlw	0FF
	movwf	delayTemp1
ledDelayLoop1
	movlw	0FF
	movwf	delayTemp2
ledDelayLoop2
	nop
	decfsz	delayTemp2, f
	goto	ledDelayLoop2
	decfsz	delayTemp1, f
	goto	ledDelayLoop1
	retlw	0

	end


