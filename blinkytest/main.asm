	;;
	;; Electric field sensor box
	;; Copyright 2003, Micah Dowty <micah@navi.picogui.org>
	;;
	;; This box has essentially the same transmit and receive
	;; hardware as the LazyFish. There are 6 electrodes arranged
	;; under the lid of the project's enclosure. A PIC16F877
	;; microcontroller was chosen, for easy in-circuit programming
	;; using the WLoader bootloader firmware.
	;;
	;; Hardware connections:
	;; 
	;;   RB0-3:	Front panel LEDs, active high
	;;   RB4-7:	Transmit electrodes
	;;   RA0-1:	Receive electrodes
	;;   RE2:	WLoader serial transmit and receive
	;; 
	;;   20MHz crystal oscillator
	;;
	;;   USB port is currently only used for power. It
	;;   is planned to eventually add a separate USB chip,
	;;   or use a PIC with built-in USB.
	;; 
	
	list	p=16f877
	#include p16f877.inc


;----------------------------------------------------- Constants

; I/O
#define	Light0		PORTB, 0
#define	Light1		PORTB, 1
#define	Light2		PORTB, 2
#define	Light3		PORTB, 3
#define Transmit0	PORTB, 4
#define Transmit1	PORTB, 5
#define Transmit2	PORTB, 6
#define Transmit3	PORTB, 7
#define Receive0	PORTA, 0
#define	Receive1	PORTA, 1
#define DefaultTrisA	B'00000011'
#define DefaultTrisB	B'00000000'
#define DefaultTrisC	B'00000000'
#define DefaultTrisD	B'00000000'
#define DefaultTrisE	B'00000100'

	
;----------------------------------------------------- Variables

	cblock 0C
	  delayTemp1, delayTemp2, delayTemp3
	endc


;----------------------------------------------------- Main program
	org 0

	;; I/O Initialization
	bsf	STATUS, RP0	; TRIS registers in page 1
	movlw	DefaultTrisA
	movwf	TRISA
	movlw	DefaultTrisB
	movwf	TRISB
	movlw	DefaultTrisC
	movwf	TRISC
	movlw	DefaultTrisD
	movwf	TRISD
	movlw	DefaultTrisE
	movwf	TRISE
	bcf	STATUS, RP0
	clrf	PORTA		; Clear ports
	clrf	PORTB
	clrf	PORTC
	clrf	PORTD
	clrf	PORTE

blinkyloop
	bsf	Light0
	call	delay
	bcf	Light0
	
	bsf	Light1
	call	delay
	bcf	Light1

	bsf	Light2
	call	delay
	bcf	Light2

	bsf	Light3
	call	delay
	bcf	Light3

	bsf	Light2
	call	delay
	bcf	Light2

	bsf	Light1
	call	delay
	bcf	Light1

	goto	blinkyloop

delay
	movlw	0D5
	movwf	delayTemp1
delayLoop1
	movlw	0FF
	movwf	delayTemp2
delayLoop2
	movlw	0FF
	movwf	delayTemp3
delayLoop3
	decfsz	delayTemp3, f
	goto	delayLoop3
	decfsz	delayTemp2, f
	goto	delayLoop2
	decfsz	delayTemp1, f
	goto	delayLoop1
	return	
	
	end
	

