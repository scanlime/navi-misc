;################################################################################
;
; commands.asm - Implementation of our vendor-specific commands
;
; This file is part of the rcpod project. This file contains original code,
; released into the public domain.
;
; Micah Dowty <micah@picogui.org>
;
;###############################################################################

#include <p16C765.inc>
#include "usb_defs.inc"
#include "../include/rcpod_protocol.h"

	errorlevel -302		; supress "register not in bank0, check page bits" message

	global	CheckVendor

	extern	BufferData
	extern	wrongstate
	extern	temp
	extern	Send_0Len_pkt
	extern	rx_fsr
	extern	rx_status
	extern	rx_remaining
	extern	rx_count
	extern	io_pin
	extern	io_High
	extern	io_Low
	extern	io_Output
	extern	io_Input
	extern	io_Read

	global	txe_pin

bank0	udata
txe_pin	res	1		 ; Pin number for transmit enable
last_poke_addr res 2 ; Address of the last byte of the most recently completed 'poke'

	code

; *********************************************************************
; Vendor Specific calls
; control is transferred here when bmRequestType bits 5 & 6 = 10 indicating
; the request is a vendor specific request.  This function then would
; interpret the bRequest field to determine what action is required.
; The end of each vendor specific command should be terminated with a
; return.
; *********************************************************************
CheckVendor
	movf	BufferData+bRequest,w
	xorlw	RCPOD_CTRL_POKE
	pagesel	PokeRequest
	btfsc	STATUS,Z
	goto	PokeRequest

	movf	BufferData+bRequest,w
	xorlw	RCPOD_CTRL_PEEK
	pagesel	PeekRequest
	btfsc	STATUS,Z
	goto	PeekRequest

	movf	BufferData+bRequest,w
	xorlw	RCPOD_CTRL_POKE4
	pagesel	Poke4Request
	btfsc	STATUS,Z
	goto	Poke4Request

	movf	BufferData+bRequest,w
	xorlw	RCPOD_CTRL_PEEK8
	pagesel	Peek8Request
	btfsc	STATUS,Z
	goto	Peek8Request

	movf	BufferData+bRequest,w
	xorlw	RCPOD_CTRL_ANALOG_ALL
	pagesel	AnalogAllRequest
	btfsc	STATUS,Z
	goto	AnalogAllRequest

	movf	BufferData+bRequest,w
	xorlw	RCPOD_CTRL_USART_TXRX
	pagesel	TxRxRequest
	btfsc	STATUS,Z
	goto	TxRxRequest

	movf	BufferData+bRequest,w
	xorlw	RCPOD_CTRL_USART_RX_END
	pagesel	RxEndRequest
	btfsc	STATUS,Z
	goto	RxEndRequest

	movf	BufferData+bRequest,w
	xorlw	RCPOD_CTRL_USART_TXE
	pagesel	TxeRequest
	btfsc	STATUS,Z
	goto	TxeRequest

	movf	BufferData+bRequest,w
	xorlw	RCPOD_CTRL_GPIO_ASSERT
	pagesel	GpioAssertRequest
	btfsc	STATUS,Z
	goto	GpioAssertRequest

	movf	BufferData+bRequest,w
	xorlw	RCPOD_CTRL_GPIO_READ
	pagesel	GpioReadRequest
	btfsc	STATUS,Z
	goto	GpioReadRequest

	movf	BufferData+bRequest,w
	xorlw	RCPOD_CTRL_SYNC_TX
	pagesel	SyncTxRequest
	btfsc	STATUS,Z
	goto	SyncTxRequest

	movf	BufferData+bRequest,w
	xorlw	RCPOD_CTRL_SYNC_RX
	pagesel	SyncRxRequest
	btfsc	STATUS,Z
	goto	SyncRxRequest

	pagesel	wrongstate		; Not a recognized request
	goto	wrongstate

	;********************* Request to write a byte to RAM
	; A 9-bit address in wIndex, 8-bit data in wValue
PokeRequest
	banksel BufferData
	bcf	STATUS, IRP	; Transfer bit 8 of wIndex into IRP
	btfsc	BufferData+(wIndex+1), 0
	bsf	STATUS, IRP

	movf	BufferData+wIndex, w ; And bits 0-7 into FSR
	movwf	FSR

	movf	BufferData+wValue, w ; Now write bits 0-7 of wValue to [IRP:FSR]
	movwf	INDF

	; Acknowledge the request
	pagesel	Send_0Len_pkt
	call	Send_0Len_pkt
	return

	;********************* Request read a byte from RAM
	; 9-bit address in wIndex, an 8-bit value returned in a 1-byte packet
PeekRequest
	banksel BufferData
	bcf	STATUS, IRP	; Transfer bit 8 of wIndex into IRP
	btfsc	BufferData+(wIndex+1), 0
	bsf	STATUS, IRP

	movf	BufferData+wIndex, w ; And bits 0-7 into FSR
	movwf	FSR

	movf	INDF, w		; Save the referenced byte in temp
	movwf	temp

	banksel	BD0IAL
	movf	low BD0IAL,w	; get address of buffer
	movwf	FSR
	bsf 	STATUS,IRP	; indirectly to banks 2-3

	movf	temp, w		;  Write the saved byte to our buffer
	movwf	INDF
	banksel	BD0IBC
	bsf 	STATUS, RP0
	movlw	0x01
	movwf	BD0IBC		; set byte count to 1
	movlw	0xc8		; DATA1 packet, DTS enabled
	movwf	BD0IST		; give buffer back to SIE
	return

	;********************* Request read all A/D converters
	; No inputs, returns an 8-byte packet
AnalogAllRequest
	banksel	BD0IAL
	movf	low BD0IAL,w	; get address of buffer
	movwf	FSR
	bsf 	STATUS,IRP	; indirectly to banks 2-3

	banksel	ADCON0		; Turn on the ADC and initialize it to full precision and channel 0
	movlw	0x81
	movwf	ADCON0

	movlw	8		; Loop over all 8 A/D inputs
	movwf	temp
adChannelLoop
	banksel	ADCON0		; Start the ADC
	bsf	ADCON0, GO

	pagesel	adFinishLoop	; Wait for it to finish
adFinishLoop
	btfsc	ADCON0, NOT_DONE
	goto	adFinishLoop

	banksel	ADRES		; Stow the finished value in our packet buffer
	movf	ADRES, w
	movwf	INDF

	movlw	0x08		; Next channel
	banksel	ADCON0
	addwf	ADCON0, f

	incf	FSR, f		; Next buffer byte

	pagesel	adChannelLoop	; Loop over all channels
	decfsz	temp, f
	goto	adFinishLoop

	banksel	ADCON0		; Turn off the ADC
	clrf	ADCON0

	banksel	BD0IBC
	bsf 	STATUS, RP0
	movlw	0x08
	movwf	BD0IBC		; set byte count to 8
	movlw	0xc8		; DATA1 packet, DTS enabled
	movwf	BD0IST		; give buffer back to SIE
	return

	;********************* Request to send then receive via the USART
	; A 9-bit buffer address in wIndex, number of bytes to transmit in wValue,
	; number of bytes to receive in wValue+1
TxRxRequest
	banksel BufferData
	bcf	STATUS, IRP	; Transfer bit 8 of wIndex into IRP
	btfsc	BufferData+(wIndex+1), 0
	bsf	STATUS, IRP

	movf	BufferData+wIndex, w ; Load address bits 0-7 into FSR
	movwf	FSR

	movf	BufferData+wValue, w ; See if we have anything to transmit
	btfsc	STATUS, Z
	goto	skipTx

	banksel	txe_pin		; turn on the transmit enable pin (0 will have no effect)
	movf	txe_pin, w
	banksel	io_pin
	movwf	io_pin
	pagesel	io_High
	call	io_High

txLoop
	pagesel	txLoop
	banksel	PIR1		; Poll for transmitter availability
	btfss	PIR1, TXIF
	goto	txLoop

	movf	INDF, w		; Stick the next buffered byte into the transmitter
	banksel	TXREG
	movwf	TXREG
	incf	FSR, f		; Increment the buffer pointer

	banksel	BufferData
	decfsz	BufferData+wValue, f ; Count down the number of bytes to send...
	goto	txLoop

	banksel	TXSTA		; Wait for the transmit shift register to empty
	pagesel	txFinish
txFinish
	btfss	TXSTA, TRMT
	goto	txFinish

	banksel	txe_pin		; turn off the transmit enable pin (0 will have no effect)
	movf	txe_pin, w
	banksel	io_pin
	movwf	io_pin
	pagesel	io_Low
	call	io_Low

skipTx

	banksel	BufferData
	movf	BufferData+wIndex, w ; Store the beginning of the receive buffer
	banksel	rx_fsr
	movwf	rx_fsr

	banksel STATUS		; Save STATUS, containing the buffer's IRP bit
	movf	STATUS, w
	banksel	rx_status
	movwf	rx_status

	banksel	RCREG		; Make sure the receive register is empty, so we
	movf	RCREG, w	;   don't start receiving on a byte that came in either
				;   before the command was issued or while we were transmitting

	banksel	BufferData
	movf	BufferData+(wValue+1), w ; Store the number of bytes to receive
	banksel	rx_remaining
	movwf	rx_remaining

	banksel	rx_count	; Clear the count of received bytes
	clrf	rx_count

	; Acknowledge the request
	pagesel	Send_0Len_pkt
	call	Send_0Len_pkt
	return

	;********************* Request to cancel USART reception
	; Cancels an ongoing USART receive, and returns the number of bytes received.
RxEndRequest
	banksel	BD0IAL
	movf	low BD0IAL,w	; get address of buffer
	movwf	FSR
	bsf 	STATUS,IRP	; indirectly to banks 2-3

	banksel	rx_remaining	; Disable the receive
	clrf	rx_remaining

	banksel	rx_count
	movf	rx_count, w	;  Write the received byte count to our buffer
	movwf	INDF
	banksel	BD0IBC
	bsf 	STATUS, RP0
	movlw	0x01
	movwf	BD0IBC		; set byte count to 1
	movlw	0xc8		; DATA1 packet, DTS enabled
	movwf	BD0IST		; give buffer back to SIE
	return

	;********************* Set the USART transmit enable pin
TxeRequest
	banksel BufferData
	movf	BufferData+wValue, w
	banksel	txe_pin
	movwf	txe_pin

	; Acknowledge the request
	pagesel	Send_0Len_pkt
	call	Send_0Len_pkt
	return

	end

;### The End ###
