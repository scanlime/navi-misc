;
; NES lightgun IR transmitter
; Copyright 2003, Micah Dowty <micah@navi.picogui.org>
;
; This is firmware for a PIC12C509A used to interface a Nintendo
; Zapper(tm) light gun with a computer wirelessly, using infrared.
;
;
; Hardware description:
;      GP0: Output to light gun board power
;      GP2: Input from light gun's light sensor, tied to GP0 with a 2.2K resistor
;    /MCLR: Connected to ground via trigger switch
;           (There was a large capacitor across the switch on the
;            original board- this must be removed)
;      GP4: \ Both pins tied together and connected directly
;      GP5: / to the transmit LED.
;
;
; PIC configuration:
;     - 4MHz Internal oscillator
;     - No watchdog
;     - GP3 == /MCLR
;
;
; Protocol:
;   The protocol used by the transmitter should be compatible
;   with any receiver that can process signals from Consumer IR
;   remote controls.
;
;     - 38kHz carrier
;     - Header: 255 cycles (6710 us) on, 128 cycles (3368 us) off
;     - "1" bit: 20 cycles (526 us) on, 20 cycles (526 us) off
;     - "0" bit: 20 cycles (526 us) on, 60 cycles (1579 us) off
;     - Footer: 128 cycles (3368 us) on, 60 cycles (1579 us) off
;
;   A 16-bit code is sent using this low-level protocol, MSB first.
;   This high-level protocol is designed to not interfere with other
;   devices, to uniquely identify many units, to be resistant to single-bit
;   errors in encoding, and to allow future expansion.
;
;   The high byte is a unique code identifying the particular light
;   gun. It should be generated randomly, to reduce the chance that
;   this system interferes with others.
;
;   The low byte is one of the following function codes:
;
;        01111111 - Trigger was pressed
;        10111111 - Light sensor pulse
;
;   To provide failure rather than misidentification in the case of
;   single-bit errors, new device and function codes should be
;   different by more than one bit.
;

	list	p=12c509a
	#include p12c509a.inc


;----------------------------------------------------- Per-device configuration

;--- Values for first prototype gun
oscCalValue	equ	054
deviceID	equ	0C3

;--- Values for transmitter/receiver test breadboard
;oscCalValue	equ	04C
;deviceID	equ	095

;----------------------------------------------------- Constants

; I/O
tristateMask	equ	b'00001100'
ledMask		equ	b'00110000'
powerBit	equ	0
sensorBit	equ	2

; IR timing
IRheaderMark	equ	D'255'
IRheaderSpace	equ	D'128'
IRoneMark	equ	D'20'
IRoneSpace	equ	D'20'
IRzeroMark	equ	D'20'
IRzeroSpace	equ	D'60'
IRfooterMark	equ	D'128'
IRfooterSpace	equ	D'60'

; IR commands
cmdTrigger	equ	b'01111111'
cmdLightSensor	equ	b'10111111'


;----------------------------------------------------- Variables

	cblock 7
	  IRtemp1, IRtemp2, IRwordHigh, IRwordLow
	  pollTimer1, pollTimer2, pollTimer3, gpioLatch
	endc


;----------------------------------------------------- Main program
	org 0

	; Initialize oscillator (must come first)
	ifdef oscCalValue
	movlw	oscCalValue
	endif
	movwf	OSCCAL
	
	; Initialize I/O
	clrf	GPIO
	clrf	gpioLatch
	movlw	tristateMask
	tris	GPIO

	; Power up the sensor board.
	; We use gpioLatch so that we can do read-modify-write on the
	; GPIO pins correctly even if this pin is trying to charge a 
	; capacitor in the sensor board.
	bsf	gpioLatch, powerBit
	movf	gpioLatch, w
	movwf	GPIO

	; Send the command indicating the trigger was pressed.
	; This also serves as a delay of about 70ms to let the light sensor stabilize
	movlw	cmdTrigger
	call	IRsendCommand

	; The light gun input should normally be low, but it will
	; pulse high. We want to send a cmdLightSensor command if and
	; when it pulses, but after waiting a while we
	; will go to sleep.
	movlw	5
	movwf	pollTimer1
pollLoop1
	movlw	0FF
	movwf	pollTimer2
pollLoop2
	movlw	0FF
	movwf	pollTimer3
pollLoop3
	btfsc	GPIO, sensorBit
	goto	sensorActivated
	decfsz	pollTimer3, f
	goto	pollLoop3
	decfsz	pollTimer2, f
	goto	pollLoop2
	decfsz	pollTimer1, f
	goto	pollLoop1

	clrf	GPIO		; Power down until the trigger resets us again
	sleep

sensorActivated
	movlw	cmdLightSensor	; Send IR command for light sensor
	call	IRsendCommand
unPollLoop
	btfsc	GPIO, sensorBit	; Wait until the sensor line is low again
	goto	unPollLoop
	goto	pollLoop3


;----------------------------------------------------- IR transmitter

	; Send the command specified in w, from our device ID
IRsendCommand
	movwf	IRwordLow
	movlw	deviceID
	movwf	IRwordHigh
	; ... fall through to IRsendWord

	; Send a 16-bit code, specified in IRwordHigh and IRwordLow
IRsendWord
	movlw	IRheaderMark	; Send header
	call	IRsendPulses
	movlw	IRheaderSpace
	call	IRsendBlanks
	movlw	10		; Loop through 16 bits
	movwf	IRtemp2
IRbitLoop
	rlf	IRwordLow, f	; Shift carry <- IRwordHigh <- IRwordLow
	rlf	IRwordHigh, f
	btfsc	STATUS, C	; Test the LSB
	goto	IRone
IRzero
	movlw	IRzeroMark	; Send a zero
	call	IRsendPulses
	movlw	IRzeroSpace
	call	IRsendBlanks
	goto	IRbitDone
IRone
	movlw	IRoneMark	; Send a one
	call	IRsendPulses
	movlw	IRoneSpace
	call	IRsendBlanks
IRbitDone
	decfsz	IRtemp2, f	; Loop through all bits
	goto IRbitLoop
	movlw	IRfooterMark	; Send footer
	call	IRsendPulses
	movlw	IRfooterSpace
	call	IRsendBlanks
	retlw	0


	; Send a number of IR pulses specified in W
	; This loop has a period of 26 instructions,
	; and 50% duty cycle. At 1 MIPS, this will produce a 38.461kHz
	; square wave, close enough to our target of 38kHz.
IRsendPulses
	movwf	IRtemp1
IRpulseLoop1			;         Loop for a number of pulses
	movlw	ledMask		; off 1	  Turn transmitter on
	iorwf	gpioLatch, f    ; off 2
	movf	gpioLatch, w    ; off 3
	movwf	GPIO            ; off 4   After this instruction, transmitter is on
	nop			; on  1
	nop			; on  2
	nop			; on  3
	nop			; on  4
	nop			; on  5
	nop			; on  6
	nop			; on  7
	nop			; on  8
	nop			; on  9
	movlw	~ledMask	; on  10  Turn transmitter off
	andwf	gpioLatch, f	; on  11
	movf	gpioLatch, w	; on  12
	movwf	GPIO            ; on  13  After this instruction, transmitter is off
	nop			; off 5
	nop			; off 6
	nop			; off 7
	nop			; off 8
	nop			; off 9
	nop			; off 10
	decfsz	IRtemp1, f	; off 11
	goto	IRpulseLoop1	; off 12,13
	retlw	0


	; This loop has the same timing as IRsendPulses, but doesn't
	; enable the transmitter.
IRsendBlanks
	movwf	IRtemp1
IRblankLoop1
	nop			; 1
	nop			; 2
	nop			; 3
	nop			; 4
	nop			; 5
	nop			; 6
	nop			; 7
	nop			; 8
	nop			; 9
	nop			; 10
	nop			; 11
	nop			; 12
	nop			; 13
	nop			; 14
	nop			; 15
	nop			; 16
	nop			; 17
	nop			; 18
	nop			; 19
	nop			; 21
	nop			; 22
	nop			; 23
	decfsz	IRtemp1, f	; 24
	goto	IRblankLoop1	; 25,26
	retlw	0

	end


