;################################################################################
;
; usb_main.asm - Main loop and interrupt service routine for the USB Laser Projector project.
;
; This file is part of the USB Laser Projector project. This file is based
; on the revision 2.00 USB firmware distributed by Microchip for use with the
; PIC16C745 and PIC16C765 microcontrollers. New code added for this project
; is placed in the public domain.
;
; USB Laser Projector modifications done by Micah Dowty <micah@navi.cx>
;;
;###############################################################################
;
; The original license agreement and author information for the USB firmware follow:
;
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
;	Author:			Dan Butler and Reston Condit
;	Company:		Microchip Technology Inc
;
;################################################################################

#include <p16c745.inc>
#include "usb_defs.inc"
#include "macros.inc"
#include "hardware.inc"

	errorlevel -302		; supress "register not in bank0, check page bits" message

	__CONFIG  _H4_OSC & _WDT_ON & _PWRTE_OFF & _CP_OFF

unbanked	udata_shr
W_save		res	1	; register for saving W during ISR

; Stick event buffer metadata in unbanked RAM so it's easier
; to access our buffer that's spread across two banks.
num_queued_events	res	1
remaining_duration	res	1
buffer_tail			res	1
buffer_head			res 1

bank0	udata
Status_save	res	1	; registers for saving context
PCLATH_save	res	1	;  during ISR
FSR_save	res	1
PIRmasked	res	1
USBMaskedInterrupts  res  1

tmr1_adj	res	2	; Adjustment applied to tmr1 each time it resets

bank1	udata
x_buffer	res	EVENT_BUFFER_SIZE
y_buffer	res	EVENT_BUFFER_SIZE
bank2	udata
duration_buffer	res	EVENT_BUFFER_SIZE
misc_buffer	res	EVENT_BUFFER_SIZE

	global	num_queued_events
	global	buffer_head
	global	x_buffer
	global	y_buffer
	global	duration_buffer
	global	misc_buffer
	global	tmr1_adj

	global	ServiceTimer

	extern	InitUSB
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
	banksel	Status_save
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

PERIPHERALTEST
	pagesel	EndISR
	btfss	INTCON,PEIE	; is there a peripheral interrupt?
	goto	EndISR		; all done....

TEST_PIR1
	banksel PIE1
	movf	PIE1,w
	banksel	PIR1
	andwf	PIR1,w		; mask the enables with the flags
	banksel	PIRmasked
	movwf	PIRmasked

	btfss	PIRmasked,USBIF	; USB interrupt flag
	goto	EndISR
	banksel	PIR1
	bcf		PIR1,USBIF

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

	pagesel	EndISR
	btfss	USBMaskedInterrupts,TOK_DNE	; is it a Token Done?
	goto	EndISR				; no, skip the queueing process

CheckFinishSetAddr
	banksel UIR
	bcf	UIR, TOK_DNE	; clear Token Done
	banksel	USB_dev_req
	movf	USB_dev_req,w	; yes: Are we waiting for the In transaction ack-ing the end of the set address?
	xorlw	SET_ADDRESS
	btfss	STATUS,Z
	goto	EndISR		; no - skip the rest.. just queue the USTAT register

	pagesel finish_set_address
	call	finish_set_address

; ******************************************************************
; End ISR, restore context and return to the Main program
; ******************************************************************
EndISR
	banksel	FSR_save
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
; ServiceTimer
;    Services any outstanding timer interrupts.
;    This should be called from the main loop.
; ******************************************************************

ServiceTimer
	banksel	PIR1		; Check for a TMR1 overflow, the rest of the loop should only run
	btfss	PIR1, TMR1IF; at regular intervals determined by tmr1_adj.
	return
	bcf		PIR1, TMR1IF

	movf	tmr1_adj, w	; Apply our adjustment to TMR1's speed
	addwf	TMR1L, f
	btfsc	STATUS, C
	incf	TMR1H, f
	movf	tmr1_adj+1, w
	addwf	TMR1H, f

	decfsz	remaining_duration, f ; Decrement this event's duration, give up if it isn't expiring
	return

	movf	num_queued_events, w ; Are we out of queued events?
	pagesel	not_out_of_events
	btfss	STATUS, Z
	goto	not_out_of_events

	incf	remaining_duration, f ; Stay in this event a little longer... we don't want to roll over
	return
	
not_out_of_events
	
	; Extract the next event from our buffer
	bankisel x_buffer
	movlw	x_buffer
	addwf	buffer_tail, w
	movwf	FSR
	movf	INDF, w
	movwf	CCPR2L				; High byte of X axis -> CCPR2L

	movlw	y_buffer
	addwf	buffer_tail, w
	movwf	FSR
	movf	INDF, w
	movwf	CCPR1L				; High byte of Y axis -> CCPR1L

	bankisel duration_buffer
	movlw	duration_buffer
	addwf	buffer_tail, w
	movwf	FSR
	movf	INDF, w
	movwf	remaining_duration	; Event duration

	movlw	misc_buffer			; Misc buffer, holds enables and low bits
	addwf	buffer_tail, w
	movwf	FSR

	movf	INDF, w				; Mask off and set the coil and laser enables
	andlw	0xC0
	movwf	PORTB

	swapf	INDF, w				; Low bits of X axis -> CCP2CON
	andlw	0x30
	iorlw	CCP2CON_VALUE
	movwf	CCP2CON

	movf	INDF, w				; Low bits of Y axis -> CCP1CON
	andlw	0x30
	iorlw	CCP1CON_VALUE
	movwf	CCP1CON

	; Update buffer state
	decf	num_queued_events, f
	incf	buffer_tail, f
	movf	buffer_tail, w		; Wrap around if necessary
	xorlw	EVENT_BUFFER_SIZE
	btfsc	STATUS, Z
	clrf	buffer_tail

	return


;******************************************************************* Setup

Main
	movlw	.30			; delay 16 uS to wait for USB to reset
	movwf	W_save		; SIE before initializing registers
	decfsz	W_save,f	; W_save is merely a convienient register
	goto	$-1			; to use for the delay counter.

	pagesel	InitUSB
	call	InitUSB

	banksel	tmr1_adj	; Just run as slow as we can until the host gives us a better speed
	clrf	tmr1_adj
	clrf	tmr1_adj+1

	; Initialize hardware
	movlf	PORTA_VALUE, PORTA
	movlf	PORTB_VALUE, PORTB
	movlf	PORTC_VALUE, PORTC
	movlf	TRISA_VALUE, TRISA
	movlf	TRISB_VALUE, TRISB
	movlf	TRISC_VALUE, TRISC
	movlf	ADCON1_VALUE, ADCON1
	movlf	T1CON_VALUE, T1CON
	movlf	T2CON_VALUE, T2CON
	movlf	PR2_VALUE, PR2_VALUE
	movlf	CCP1CON_VALUE, CCP1CON
	movlf	CCP2CON_VALUE, CCP2CON
	movlf	CCPR1L_VALUE, CCPR1L
	movlf	CCPR2L_VALUE, CCPR1L

	; Initialize the event ringbuffer
	clrf	num_queued_events
	clrf	remaining_duration
	clrf	buffer_tail
	clrf	buffer_head


;******************************************************************* Main Loop

MainLoop
	clrwdt				; This should be the only place we clear the WDT!

	pagesel ServiceUSB
	call	ServiceUSB	; see if there are any USB tokens to process

	pagesel	ServiceTimer
	call	ServiceTimer

	pagesel MainLoop
	goto    MainLoop

	end

;### The End ###
