;
; NES lightgun IR transmitter
; Copyright 2003-2004, Micah Dowty <micah@navi.picogui.org>
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
;     - external /MCLR
;
; The transmission protocol was designed to be compatible with the Pinnacle PCTV
; remote control receiver. The gun's trigger sends the "1" key, the light sensor
; sends the "2" key.
;

	list	p=12c509a
	#include p12c509a.inc


;----------------------------------------------------- Per-device configuration

#define device1

#ifdef device1
oscCalValue	equ	04C
#endif
#ifdef device2
oscCalValue	equ	054
#endif
#ifdef device3
oscCalValue	equ	054  ; Not a typo- chips 2 and 3 have the same calibration
#endif

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

	; Delay for a bit to debounce the trigger button
	movlw	0FF
	call	IRsendBlanks

	; Send the command indicating the trigger was pressed.
	; This also serves as a delay of about 70ms to let the light sensor stabilize
	call	sendKey1

	; Wait for the receiver to settle- the transmitter will generate a lot
	; of electrical and optical noise that the receiver might pick up.
	movlw	0FF
	call	IRsendBlanks
	movlw	0FF
	call	IRsendBlanks
	movlw	0FF
	call	IRsendBlanks
	movlw	0FF
	call	IRsendBlanks

	; The light gun input should normally be high, but it will
	; pulse low. We want to send a cmdLightSensor command if and
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
	btfss	GPIO, sensorBit
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
	call	sendKey2
unPollLoop
	btfss	GPIO, sensorBit	; Wait until the sensor line is low again
	goto	unPollLoop
	goto	pollLoop3


;----------------------------------------------------- IR transmitter

; the pinnacle systems remote control sends data as a device byte,
; followed by the button code (LSB), followed by the complement of
; the button code

; 1100000000 1000001100 1011100110
; 

; button '1': 110000000 110001100 101110011
; button '2': 110000000 101001100 110110011

sendKey1
	; byte 1
	movlw	D'65'
	call	IRsendPulses
	movlw	D'255'
	call	IRsendBlanks
	movlw	D'23'
	call	IRsendBlanks

	; byte 2
	movlw	D'41'
	call	IRsendPulses
	movlw	D'193'
	call	IRsendBlanks
	movlw	D'66'
	call	IRsendPulses
	movlw	D'68'
	call	IRsendBlanks

	; byte 3
	movlw	D'33'
	call	IRsendPulses
	movlw	D'35'
	call	IRsendBlanks
	movlw	D'98'
	call	IRsendPulses
	movlw	D'68'
	call	IRsendBlanks
	movlw	D'66'
	call	IRsendPulses
	retlw	0	

sendKey2
	; byte 1
	movlw	D'64'
	call	IRsendPulses
	movlw	D'255'
	call	IRsendBlanks
	movlw	D'22'
	call	IRsendBlanks

	; byte 2
	movlw	D'32'
	call	IRsendPulses
	movlw	D'36'
	call	IRsendBlanks
	movlw	D'32'
	call	IRsendPulses
	movlw	D'159'
	call	IRsendBlanks
	movlw	D'64'
	call	IRsendPulses
	movlw	D'27'
	call	IRsendBlanks

	; byte 3
	movlw	D'64'
	call	IRsendPulses
	movlw	D'32'
	call	IRsendBlanks
	movlw	D'64'
	call	IRsendPulses
	movlw	D'71'
	call	IRsendBlanks
	movlw	D'64'
	call	IRsendPulses

	; stop bits
	movlw	D'64'
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


