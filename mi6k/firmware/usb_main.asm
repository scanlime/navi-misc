;################################################################################
;
; usb_main.asm - Main loop and interrupt service routine for the MI6K project.
;
; This file is part of the MI6K project. This file is based on the revision 2.00
; USB firmware distributed by Microchip for use with the PIC16C745 and PIC16C765
; microcontrollers. New code added for the MI6K project is placed in the public domain.
;
; MI6K modifications done by Micah Dowty <micah@picogui.org>
;
; Peripheral resources:
;    CCP1, CCP2, TMR2 - LED brightness controls
;    USART            - VFD serial
;    TMR1             - IR receive timing
;    TMR0             - IR transmit timing
;    INT              - IR receiver external interrupt
;
; Control flow:
;    USB activity and the resulting control requests are processed from the
;    program's main loop. The interrupt driven parts of the original USB code
;    have been preserved, and in addition a service routine for the IR receiver
;    has been added.
;    Received IR pulses cause an interrupt which uses TMR1 to generate mark and
;    space timings for the received pulses. These timings are buffered, then stored
;    in the endpoint 0 buffer for pickup by the host.
;    VFD serial, VFD power, status information, LED control, and IR transmission
;    are all triggered by USB requests in the main loop. They all use peripherals
;    for timing rather than cycle-counting, so interrupts will not disrupt serial
;    communications, make the LEDs flicker, or disrupt IR transmission.
;
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
#include "hardware.inc"

	__CONFIG  _H4_OSC & _WDT_OFF & _PWRTE_OFF & _CP_OFF

unbanked	udata_shr
W_save		res	1	; register for saving W during ISR
tmr1h_save	res 1
tmr1l_save	res 1

bank0	udata
Status_save	res	1	; registers for saving context 
PCLATH_save	res	1	;  during ISR
FSR_save	res	1
PIRmasked	res	1
USBMaskedInterrupts  res  1
epBuffer	res 8
epBufferSize res 1
epBufferSizeTemp res 1
epBufferPtrTemp res 1
epByteTemp	res 1

	; Ring buffers for IR receive and transmit. Buffer size must be a power of two.
	; Head indices point to the next available slot for new data, tail indices
	; point to the oldest stored data.
bank1	udata
#define BUFFER_SIZE .32
ir_rx_Buffer res BUFFER_SIZE
ir_rx_Head	res 1
ir_rx_Tail	res 1
ir_tx_Buffer res BUFFER_SIZE
ir_tx_Head	res 1
ir_tx_Tail	res 1
SendTemp	res 1

	; IR transmitter state- must be in a bank that has TMR0 (either 0 or 2)
bank2	udata
ir_tx_Cycles	res 1
ir_tx_Mask		res 1

	extern	InitUSB
	extern	PutEP1
	extern	GetEP1
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

	; Is it a TMR0 interrupt? (used for the IR transmitter)
	; This is the most time-critical interrupt.
TEST_TMR0
	btfss	INTCON, T0IF
	goto	TEST_RB0_INT
	bcf		INTCON, T0IF	; A TMR0 overflow occurred. Reset the timer ASAP
	movlw	.199			; 256 - (6 MIPS / 38 KHz / 2 - 18 cycles overhead)
	banksel	ir_tx_Mask		; TMR0, ir_tx_Cycles, and ir_tx_Mask all in the same bank
	movwf	TMR0
	movf	ir_tx_Mask, w	; Flip the transmitter bit if necessary
	xorwf	PORTB, f
	decfsz	ir_tx_Cycles, f	; If we finished this batch of transmit cycles, get some more from
	goto	EndISR			; the buffer. If not, return from the interrupt now.

	; Code to execute after each transmitted pulse or space	
	movf	ir_tx_Mask, w	; We just finished one pulse or space. Flip the tx mask so we do the opposite now
	xorlw	IR_TX_MASK
	movwf	ir_tx_Mask
	bcf		IR_TX			; Shut off the transmitter

	banksel	ir_tx_Buffer	; If our transmit buffer is now empty, disable the TMR0 interrupt
	movf	ir_tx_Head, w
	xorwf	ir_tx_Tail, w
	btfss	STATUS, Z
	goto	txNextByte
	bcf		INTCON, T0IE
	goto	EndISR			; ...and exit the ISR

txNextByte					; If the buffer was not empty, pop the next ir_tx_Cycles off of it
	bankisel ir_tx_Buffer
	movf	ir_tx_Tail, w
	addlw	ir_tx_Buffer
	movwf	FSR				; ir_tx_Buffer[ir_tx_Tail]
	movf	INDF, w
	banksel	ir_tx_Cycles
	movwf	ir_tx_Cycles
	banksel	ir_tx_Buffer
	incf	ir_tx_Tail, f	; Increment the buffer tail, modulo BUFFER_SIZE
	movlw	BUFFER_SIZE-1
	andwf	ir_tx_Tail, f
	goto	EndISR			; ...and exit the ISR


	; Is it an external interrupt? (state of the IR receiver changed)
TEST_RB0_INT
	btfss	INTCON, INTF
	goto	PERIPHERALTEST
	banksel	TMR1H
	movf	TMR1H, w		; Save and reset the tmr1 value ASAP
	movwf	tmr1h_save
	movf	TMR1L, w
	movwf	tmr1l_save
	clrf	TMR1L
	clrf	TMR1H
	clrf	TMR1L
	bcf		INTCON, INTF	; Clear the external interrupt flag
	banksel	ir_rx_Head
	bankisel ir_rx_Head
	movf	ir_rx_Head, w	; Push the low byte onto our rx ringbuffer
	addlw	ir_rx_Buffer
	movwf	FSR
	movf	tmr1l_save, w
	movwf	INDF
	incf	ir_rx_Head, f	; Increment the buffer head, modulo BUFFER_SIZE
	movlw	BUFFER_SIZE-1
	andwf	ir_rx_Head, f
	movf	ir_rx_Head, w	; Push the high byte onto our rx ringbuffer
	addlw	ir_rx_Buffer
	movwf	FSR
	movf	tmr1h_save, w
	movwf	INDF
	incf	ir_rx_Head, f	; Increment the buffer head, modulo BUFFER_SIZE
	movlw	BUFFER_SIZE-1
	andwf	ir_rx_Head, f
	banksel	OPTION_REG		; Now trigger our interrupt on the opposite edge
	movlw	0x40			; (toggle the INTEDG bit)
	xorwf	OPTION_REG, f
	goto	EndISR			; ...and exit the ISR


PERIPHERALTEST
	btfss	INTCON,PEIE	; is there a peripheral interrupt?
	goto	EndISR		; all done....

TEST_PIR1
	bsf 	STATUS, RP0	; Bank1
	movf	PIE1,w
	bcf	STATUS, RP0	; Bank0
	andwf	PIR1,w		; mask the enables with the flags
	movwf	PIRmasked

	btfss	PIRmasked,USBIF	; USB interrupt flag
	goto	EndISR
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

	btfss	USBMaskedInterrupts,TOK_DNE	; is it a Token Done?
	goto	EndISR				; no, skip the queueing process

CheckFinishSetAddr
	banksel UIR
	bcf	UIR, TOK_DNE	; clear Token Done
	bcf	STATUS,RP0	; bank 2
	movf	USB_dev_req,w	; yes: Are we waiting for the In transaction ack-ing the end of the set address?
	xorlw	SET_ADDRESS
	btfss	STATUS,Z
	goto	EndISR		; no - skip the rest.. just queue the USTAT register

	pagesel finish_set_address
	call	finish_set_address
	clrf	STATUS		; bank 0

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


;******************************************************************* Setup

Main
	movlw	.30			; delay 16 uS to wait for USB to reset
	movwf	W_save		; SIE before initializing registers
	decfsz	W_save,f	; W_save is merely a convienient register
	goto	$-1			; to use for the delay counter.

	clrf	STATUS		; Clear outputs and initialize port directions
	clrf	PORTA
	clrf	PORTB
	clrf	PORTC
	banksel	TRISA
	movlw	TRISA_VALUE
	movwf	TRISA
	movlw	TRISB_VALUE
	movwf	TRISB
	movlw	TRISC_VALUE
	movwf	TRISC

	banksel	ir_tx_Buffer
	clrf	ir_tx_Head	; Clear all the transmit/receive buffers
	clrf	ir_tx_Tail
	clrf	ir_rx_Head
	clrf	ir_rx_Tail
	banksel	ir_tx_Mask
	movlw	IR_TX_MASK	; First transmission is a pulse
	movwf	ir_tx_Mask

	pagesel	InitUSB
	call	InitUSB

	movlw	.77			; Set UART to 19200 baud 
	banksel	SPBRG
	movwf	SPBRG
	banksel	RCSTA
	bsf		RCSTA, SPEN	; Enable serial port
	banksel	TXSTA
	bsf		TXSTA, BRGH ; High speed mode
	bcf		TXSTA, SYNC	; Asynchronous mode
	bsf		TXSTA, TXEN	; Enable transmitter

	movlw	0xFF		; Maximum PWM period for both CCP modules
	banksel	PR2
	movlw	PR2
	movlw	0x0C		; PWM mode
	banksel	CCP1CON
	movwf	CCP1CON
	movwf	CCP2CON
	banksel	CCPR1L
	clrf	CCPR1L		; LEDs off by default
	clrf	CCPR2L
	movlw	0x06		; Timer2 on, prescaler of 16
	banksel	T2CON
	movwf	T2CON

	; Initialize TMR0 according to example 6-1 in the PIC16C745 data sheet
	banksel	OPTION_REG
	movlw	0x87		; Pullups disabled, interrupt on falling edge of IR signal
	movwf	OPTION_REG
	banksel	TMR0
	clrf	TMR0		; Clear TMR0 and prescaler
	banksel	OPTION_REG
	movlw	0x8F		; Assign the prescaler to the WDT, TMR0 now runs at the instruction frequency
	movwf	OPTION_REG
	clrwdt				; Necessary to prevent the WDT from unexpectedly resetting

	movlw	0x31		; TMR1 enabled, 1:8 prescaler. This causes it to count in 4/3us units
	banksel	T1CON
	movwf	T1CON
	clrf	TMR1L		; Clear the timer such that it's safe from rollovers
	clrf	TMR1H
	clrf	TMR1L

	banksel	INTCON
	bsf		INTCON, INTE ; External (IR receiver) interrupt enable
	

;******************************************************************* Main Loop

MainLoop
	pagesel ServiceUSB
	call	ServiceUSB	; see if there are any USB tokens to process

	ConfiguredUSB		; macro to check configuration status
	pagesel MainLoop
	btfss	STATUS,Z	; Z = 1 when configured
	goto	MainLoop    ; Wait until we're configured

	; The rest of this main loop is concerned with transferring data from the
	; ir_rx ringbuffer to the EP1 IN endpoint. This can only happen when the
	; EP1 buffer is available, and there is at least one byte in the ir_rx
	; ringbuffer. If a transfer can be made, up to 8 bytes will be sent to the
	; EP1 buffer to await pickup by the host.

	banksel	BD1IST		; If we don't own the EP1 buffer, keep waiting
	btfsc	BD1IST, UOWN
	goto	MainLoop

	banksel	ir_rx_Buffer ; Calculate the number of bytes in the ir_rx buffer
	movf	ir_rx_Tail, w
	subwf	ir_rx_Head, w
	andlw	BUFFER_SIZE-1
	btfsc	STATUS, Z	; No data? keep waiting
	goto	MainLoop
	banksel	epBufferSize ; Store the number of available bytes
	movwf	epBufferSize

	movlw	0xF8		; If we have more than 8 bytes available, clamp it to 8
	andwf	epBufferSize, w ; The Z flag will be clear if epBufferSize >= 8
	movlw	0x08		; Prepare our value to clamp to
	btfss	STATUS, Z	; Clamp if the Z flag is clear
	movwf	epBufferSize
	
	movlw	epBuffer	; Copy epBufferSize bytes from the ring buffer to epBuffer
	movwf	epBufferPtrTemp ; Initialize loop iterators
	movf	epBufferSize, w
	movwf	epBufferSizeTemp
epCopyLoop

	bankisel ir_rx_Buffer 	; Pop one byte out of the ir_rx ringbuffer
	banksel	ir_rx_Buffer
	movf	ir_rx_Tail, w
	addlw	ir_rx_Buffer
	movwf	FSR				; ir_rx_Buffer[ir_rx_Tail]
	movf	INDF, w
	banksel	epByteTemp		; Save it in epByteTemp
	movwf	epByteTemp
	banksel	ir_rx_Buffer
	incf	ir_rx_Tail, f	; Increment the buffer tail, modulo BUFFER_SIZE
	movlw	BUFFER_SIZE-1
	andwf	ir_rx_Tail, f

	bankisel epBuffer		; Copy epByteTemp to the current epBuffer byte
	banksel	epBufferPtrTemp	
	movf	epBufferPtrTemp, w
	movwf	FSR
	movf	epByteTemp, w
	movwf	INDF

	incf	epBufferPtrTemp, f ; Update iterators, loop back for another byte
	decfsz	epBufferSizeTemp, f
	goto	epCopyLoop
	
	movlw	epBuffer	; Submit the completed buffer for the host to pick up
	movwf	FSR
	movf	epBufferSize, w
	pagesel	PutEP1
	call	PutEP1

	pagesel	MainLoop
	goto	MainLoop


;******************************************************************* Peripherals

	; Send a byte to the VFD from 'w', return once it's done
VFD_SendByte:
	global	VFD_SendByte
	banksel	TXREG
	movwf	TXREG
	pagesel	sendLoop
	banksel	PIR1
sendLoop:
	btfss	PIR1, TXIF
	goto	sendLoop
	return

	; Push a byte onto the transmit buffer from 'w', return without waiting.
	; If the TMR0 interrupt is already running, we have to push the value on
	; to the transmit ring buffer. If not, we set up the interrupt and get it
	; started sending right away.
IR_SendByte:
	global	IR_SendByte
	banksel	INTCON			; If the TMR0 interrupt is already running, we have to enqueue our value
	btfsc	INTCON, T0IE
	goto	enqueueTxByte

	; Send the value immediately, starting the interrupt handler
	banksel	ir_tx_Cycles	; Store this value in ir_tx_Cycles for immediate transmission
	movwf	ir_tx_Cycles
	clrf	TMR0			; Reset TMR0 so we don't actually start for 258 cycles
	banksel	INTCON			; Enable the TMR0 interrupt, making sure the interrupt flag starts out clear
	bcf		INTCON, T0IF
	bsf		INTCON, T0IE
	return

	; Push the value into our transmit ring buffer
enqueueTxByte
	banksel	ir_tx_Buffer	
	bankisel ir_tx_Buffer
	movwf	SendTemp		; Save w while we calculate the buffer address
	movf	ir_tx_Head, w
	addlw	ir_tx_Buffer
	movwf	FSR				; ir_tx_Buffer[ir_tx_Head] = SendTemp
	movf	SendTemp, w
	movwf	INDF
	incf	ir_tx_Head, f	; Increment the buffer head, modulo BUFFER_SIZE
	movlw	BUFFER_SIZE-1
	andwf	ir_tx_Head, f
	return

	end

;### The End ###
