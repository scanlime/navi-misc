;                            Software License Agreement
;
; The software supplied herewith by Microchip Technology Incorporated (the "Company")
; for its PICmicro(r) Microcontroller is intended and supplied to you, the Company's
; customer, for use solely and exclusively on Microchip PICmicro Microcontroller
; products.
;
; The software is owned by the Company and/or its supplier, and is protected under
; applicable copyright laws. All rights are reserved. Any use in violation of the
; foregoing restrictions may subject the user to criminal sanctions under applicable
; laws, as well as to civil liability for the breach of the terms and conditions of
; this license.
;
; THIS SOFTWARE IS PROVIDED IN AN "AS IS" CONDITION. NO WARRANTIES, WHETHER EXPRESS,
; IMPLIED OR STATUTORY, INCLUDING, BUT NOT LIMITED TO, IMPLIED WARRANTIES OF
; MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE APPLY TO THIS SOFTWARE. THE
; COMPANY SHALL NOT, IN ANY CIRCUMSTANCES, BE LIABLE FOR SPECIAL, INCIDENTAL OR
; CONSEQUENTIAL DAMAGES, FOR ANY REASON WHATSOEVER.
;
;###############################################################################
; filename:	USB_MAIN.ASM
;		Sample mainline program
;
; This file implements a basic interrupt service routine and shows how the
; USB interrupt would be serviced, and also how InitUSB and PutUSB
; should be called.  It may be used as a reference, or as a starting point 
; from which to build an application.  
;
;###############################################################################
;
;	Author:			Dan Butler and Reston Condit
;	Company:		Microchip Technology Inc
;
;	Revision:		2.00 (Polling Version)
;	Date:			06 November 2002
;	Assembled using:	MPASM 2.80
;	Configuration Bits:	H4 Oscillator, WDT Off, Power up timer off
; 
;	Notes about the polling version:
;
;	V2.00 marks a fairly substantial change in architecture from the V1.xx 
;	versions.  While V1.xx handled the USB completely from the ISR, V2.00 shifts 
;	most of the processing to a function called from the main program loop.  There 
;	are several advantages for this.  Firstly, it eliminates the may levels of the 
;	stack that currently must be reserved for the ISR.  Secondly, it minimizes the 
;	length of the ISR which is desirable from a "good software engineering 
;	practices" standpoint.  And finally it gives control over when the USB is 
;	serviced to the software engineer writing the application.  This is 
;	particularly important for timing critical systems where a few hundred 
;	instruction cycles to process the interrupt at the wrong time could potentially 
;	damage the system.   
;
;	This version is intended for use with the PIC16C745/765, revision A2 only (date 
;	code 0231 and later.)  The PCM16XQ1 processor module must be used to emulate
;	this revision of the silicon.  See the PIC16C745/765 rev. A1 and PIC16C745/765 
;	rev. A2 errata for differences in the revisions.  
;
;	Revision History:
; 		none
;
;################################################################################
;
;    include files:
;        P16C765.inc    Rev 1.00
;        usb_defs.inc   Rev 2.00
;
;################################################################################
#include <p16c745.inc>
#include "usb_defs.inc"

	__CONFIG  _H4_OSC & _WDT_OFF & _PWRTE_OFF & _CP_OFF

unbanked	udata_shr
W_save		res	1	; register for saving W during ISR

bank0	udata
Status_save	res	1	; registers for saving context 
PCLATH_save	res	1	;  during ISR
FSR_save	res	1
PIRmasked	res	1
USBMaskedInterrupts  res  1
BUFFER		res	8	; Location for data to be sent to host
COUNTER		res	1   
ByteCount	res	1
SerialByte	res 1
DelayTemp	res 1
BitCount	res 1

	extern	InitUSB
	extern	PutEP1
	extern	GetEP1
	extern	RemoteWakeup	; Remote Wakeup works with the use of the RA4 pin (active low)
	extern	USBReset
	extern	USBActivity
	extern	USBStall
	extern	USBError
	extern	USBSleep
	extern	TokenDone
	extern	USB_dev_req
	extern	finish_set_address

STARTUP	code
	pagesel	Main
	goto	Main
	nop
InterruptServiceVector
	movwf	W_save		; save W
	movf	STATUS,W
	clrf	STATUS		; force to page 0
	movwf	Status_save	; save STATUS
	movf	PCLATH,w
	movwf	PCLATH_save	; save PCLATH
	movf	FSR,w
	movwf	FSR_save	; save FSR

; *************************************************************
; Interrupt Service Routine
; First we step through several stages, attempting to identify the source 
; of the interrupt.
; ******************************************************************

;Process_ISR
PERIPHERALTEST
	pagesel EndISR
	btfss	INTCON,PEIE	; is there a peripheral interrupt?
	goto	EndISR		; all done....

TEST_PIR1
	bsf 	STATUS, RP0	; Bank1
	movf	PIE1,w
	bcf	STATUS, RP0	; Bank0
	andwf	PIR1,w		; mask the enables with the flags
	movwf	PIRmasked

	pagesel TryADIF
	btfss	PIRmasked,USBIF	; USB interrupt flag
	goto	TryADIF
	bcf	PIR1,USBIF
	
	banksel UIR
	movf	UIR,w
	andwf	UIE,w
	banksel USBMaskedInterrupts
	movwf	USBMaskedInterrupts

	pagesel	USBActivity
	btfsc	USBMaskedInterrupts,ACTIVITY	; Is there activity on the bus?
	call	USBActivity

	pagesel	USBReset
	btfsc	USBMaskedInterrupts,USB_RST	; is it a reset?
	call	USBReset			; yes, reset the SIE

	pagesel TryADIF
	btfss	USBMaskedInterrupts,TOK_DNE	; is it a Token Done?
	goto	TryADIF				; no, skip the queueing process

CheckFinishSetAddr
	banksel UIR
	bcf	UIR, TOK_DNE	; clear Token Done
	bcf	STATUS,RP0	; bank 2
	movf	USB_dev_req,w	; yes: Are we waiting for the In transaction ack-ing the end of the set address?
	xorlw	SET_ADDRESS
	btfss	STATUS,Z
	goto	TryADIF		; no - skip the rest.. just queue the USTAT register

	pagesel finish_set_address
	call	finish_set_address
	clrf	STATUS		; bank 0

TryADIF
;	btfsc	PIRmasked,ADIF	; AD Done?
;	nop
;	btfsc	PIRmasked,RCIF
;	nop
;	btfsc	PIRmasked,TXIF
;	nop
;	btfsc	PIRmasked,CCP1IF
;	nop
;	btfsc	PIRmasked,TMR2IF
;	nop
;	btfsc	PIRmasked,TMR1IF
;	nop
TEST_PIR2
;	banksel	PIR2
;	movf	PIR2,w
;	banksel	PIE2
;	andwf	PIE2,w
;	banksel	PIRmasked
;	movwf	PIRmasked
;	btfsc	PIRmasked,CCP2IF
;	nop

; ******************************************************************
; End ISR, restore context and return to the Main program
; ******************************************************************
EndISR
	clrf	STATUS		; select bank 0
	movf	FSR_save,w	; restore the FSR
	movwf	FSR
	movf	PCLATH_save,w	; restore PCLATH
	movwf	PCLATH
	movf	Status_save,w	; restore Status
	movwf	STATUS
	swapf	W_save,f	; restore W without corrupting STATUS
	swapf	W_save,w
	retfie

	code

; ******************************************************************
; ServiceUSB
;    Services any outstanding USB interrupts.
;    Checks for 
;    This should be called from the main loop.
; ******************************************************************
ServiceUSB
	banksel	UIR
	movf	UIR,w
	banksel	USBMaskedInterrupts
	movwf	USBMaskedInterrupts

	pagesel	USBError
	btfsc	USBMaskedInterrupts,UERR
	call	USBError

	pagesel	USBSleep
	btfsc	USBMaskedInterrupts,UIDLE
	call	USBSleep

	pagesel	USBStall
	btfsc	USBMaskedInterrupts,STALL
	call	USBStall

	pagesel	TokenDone
	btfsc	USBMaskedInterrupts,TOK_DNE	; is there a TOKEN Done interrupt?
	call	TokenDone

	return

; ******************************************************************
; test program that sets up the buffers and calls the ISR for processing.
;     
; ******************************************************************
Main
	movlw	.30		; delay 16 uS to wait for USB to reset
	movwf	W_save		; SIE before initializing registers
	decfsz	W_save,f	; W_save is merely a convienient register
	goto	$-1		; to use for the delay counter.

	clrf	STATUS		; Bank0
	clrf	PORTB
	clrf	PORTA
	banksel	TRISA		; Bank 1
	clrf	TRISB		; Set PORTB as all outputs
	movlw	0x10
	movwf	TRISA		; Set RA4 as input

	movlw	0x07
	movwf	OPTION_REG	; Prescaler -> Timer0, (7 gives 1:256 scaling), 10.9mS

	pagesel	InitUSB		; These six lines of code show the appropriate
	call	InitUSB		;  way to initialize the USB. First, initialize
				;  the USB (wait for host enumeration) then wait
				;  until the enumeration process to complete.

;******************************************************************* Main Loop

	banksel COUNTER
	clrf	COUNTER

MainLoop
	pagesel MainLoop
	banksel	INTCON
	btfss	INTCON,T0IF
	goto	MainLoop    ; Wait for a timer interrupt

	bcf	INTCON,T0IF
	pagesel ServiceUSB
	call	ServiceUSB	; see if there are any USB tokens to process

	ConfiguredUSB		; macro to check configuration status
	pagesel MainLoop
	btfss	STATUS,Z	; Z = 1 when configured
	goto	MainLoop    ; Wait until we're configured

	pagesel GetEP1		; Read endpoint 1, calling VFD_SendBuffer with any received bytes
	bankisel BUFFER
	movlw	BUFFER
	movwf	FSR
	call	GetEP1
	pagesel	VFD_SendBuffer
	btfsc	STATUS, C
	call	VFD_SendBuffer

	pagesel	PutEP1	    ; Send a 4-byte packet consisting of a counter followed by 01 02 03
	banksel COUNTER
	movf	COUNTER, w
	banksel	BUFFER
	movwf	BUFFER
	movlw	1
	movwf	BUFFER+1
	movlw	2
	movwf	BUFFER+2
	movlw	3
	movwf	BUFFER+3
	bankisel BUFFER
	movlw	BUFFER
	movwf	FSR
	movlw	4
	call	PutEP1
	btfsc	STATUS, C
	incf	COUNTER, f

	goto	MainLoop

;******************************************************************* VFD functions
; Note that the PIC16C745 has a built-in USART, but we don't use it
; since it has no invert bit and we'd rather not have to use a real
; serial line driver to communicate with the VFD.

	; Send 'w' bytes to the VFD, starting at BUFFER
VFD_SendBuffer:
	banksel	ByteCount
	movwf	ByteCount
	bankisel BUFFER
	movlw	BUFFER
	movwf	FSR
byteLoop:
	pagesel	VFD_SendByte
	movf	INDF, w
	call	VFD_SendByte
	incf	FSR, f
	pagesel	byteLoop
	decfsz	ByteCount, f
	goto	byteLoop
	return

	; Macro to put a space on the VFD serial line
VFD_SerialSpace	macro
	bcf		PORTB, 7
	endm

	; Macro to put a mark on the VFD serial line
VFD_SerialMark	macro
	bsf		PORTB, 7
	endm

	; Delay for one bit length at 19200 baud and 6 MIPS
	; This should waste 312 clock cycles.
VFD_SerialDelay:
	movlw	0x67
	movwf	DelayTemp
delayLoop:
	decfsz	DelayTemp
	goto	delayLoop
	return

	; Send a byte to the VFD from 'w'
VFD_SendByte:
	movwf	SerialByte		; Save input byte
	movlw	8
	movwf	BitCount
	VFD_SerialMark			; Start bit
	call	VFD_SerialDelay
bitLoop:
	rrf		SerialByte		; Shift out the LSB
	VFD_SerialSpace			; Assume space
	btfss	STATUS, C
	VFD_SerialMark			; Set a mark if necessary
	call	VFD_SerialDelay
	decfsz	BitCount
	goto	bitLoop
	VFD_SerialSpace			; Stop bit
	call	VFD_SerialDelay
	return

	end

