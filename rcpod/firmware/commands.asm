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

	extern	rx_fsr_initial	; Initial buffer pointer, rx_fsr is reset to this when rx_remaining runs out
	extern	rx_fsr			; Pointer into the buffer where the next received byte will be stored
	extern	rx_status		; STATUS byte containing an IRP bit pointing to our buffer
	extern	rx_remaining	; Number of bytes until the buffer must roll over
	extern	rx_size			; Initial rx_remaining, and the value it is reset to when it hits zero
	extern	rx_count		; Total number of received bytes, modulo 256
	extern	rx_Reset		; Resets the above values to the receiver's idle state

	extern	io_Assert
	extern	io_Deassert
	extern	io_Read
	extern	io_pin

	global	txe_pin

bank1	udata
txe_pin	res	1		 ; Pin number for transmit enable
last_poke_addr res 2 ; Address of the last byte of the most recently completed 'poke'
byte_iterator res 1
buffer_ptr	res 1
acq_iterator res 1	 ; Iterator for analog aquisition

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
	xorlw	RCPOD_CTRL_USART_RX_PROGRESS
	pagesel	RxProgressRequest
	btfsc	STATUS,Z
	goto	RxProgressRequest

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

	pagesel	wrongstate		; Not a recognized request
	goto	wrongstate

	;********************* Request to write a byte to RAM
	; A 9-bit address in wIndex, 8-bit data in wValue
PokeRequest
	banksel BufferData
	movf	BufferData+wIndex, w	; Save this poke address
	banksel	last_poke_addr
	movwf	last_poke_addr
	banksel	BufferData
	movf	BufferData+(wIndex+1), w
	banksel	last_poke_addr
	movwf	last_poke_addr+1

	banksel	BufferData
	bcf	STATUS, IRP	; Transfer bit 8 of wIndex into IRP
	btfsc	BufferData+wIndexHigh, 0
	bsf	STATUS, IRP

	movf	BufferData+wIndex, w ; And bits 0-7 into FSR
	movwf	FSR

	movf	BufferData+wValue, w ; Now write bits 0-7 of wValue to [IRP:FSR]
	movwf	INDF

	; Acknowledge the request
	pagesel	Send_0Len_pkt
	call	Send_0Len_pkt
	return

	;********************* Request to write 4 bytes after the last one poked
Poke4Request
	
prepare_poke macro
	banksel	last_poke_addr			; Increment the last_poke_addr
	incf	last_poke_addr, f
	btfsc	STATUS, C
	incf	last_poke_addr+1, f

	bcf		STATUS, IRP				; Load the high bit of last_poke_addr into IRP
	btfsc	last_poke_addr+1, 0
	bsf		STATUS, IRP

	movf	last_poke_addr, w 		; ...and bits 0-7 into FSR
	movwf	FSR
	banksel	BufferData
	endm

	; Write the proper 4 bytes from wValue and wIndex...
	prepare_poke
	movf	BufferData+wValue, w
	movwf	INDF

	prepare_poke
	movf	BufferData+wValueHigh, w
	movwf	INDF

	prepare_poke
	movf	BufferData+wIndex, w
	movwf	INDF

	prepare_poke
	movf	BufferData+wIndexHigh, w
	movwf	INDF

	; Acknowledge the request
	pagesel	Send_0Len_pkt
	call	Send_0Len_pkt
	return

	;********************* Request to assert 4 pin descriptors
GpioAssertRequest
	banksel BufferData
	movf	BufferData+wValue, w
	banksel	io_pin
	movwf	io_pin
	pagesel	io_Assert
	call	io_Assert

	banksel BufferData
	movf	BufferData+wValueHigh, w
	banksel	io_pin
	movwf	io_pin
	pagesel	io_Assert
	call	io_Assert

	banksel BufferData
	movf	BufferData+wIndex, w
	banksel	io_pin
	movwf	io_pin
	pagesel	io_Assert
	call	io_Assert

	banksel BufferData
	movf	BufferData+wIndexHigh, w
	banksel	io_pin
	movwf	io_pin
	pagesel	io_Assert
	call	io_Assert

	; Acknowledge the request
	pagesel	Send_0Len_pkt
	call	Send_0Len_pkt
	return

	;********************* Request to read a byte from RAM
	; 9-bit address in wIndex, an 8-bit value returned in a 1-byte packet
PeekRequest
	banksel BufferData
	bcf	STATUS, IRP	; Transfer bit 8 of wIndex into IRP
	btfsc	BufferData+(wIndex+1), 0
	bsf	STATUS, IRP

	movf	BufferData+wIndex, w ; And bits 0-7 into FSR
	movwf	FSR

	movlw	8

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

	;********************* Request to read a 8 contiguous bytes from RAM
	; 9-bit address in wIndex, returns an 8-byte packet
Peek8Request
	banksel	BD0IAL
	movf	low BD0IAL,w	; get address of buffer
	banksel	buffer_ptr
	movwf	buffer_ptr		; Start a buffer pointer we'll increment...

	banksel	byte_iterator
	movlw	8				; We're peeking 8 bytes
	movwf	byte_iterator
peek8Loop

	banksel BufferData
	bcf	STATUS, IRP			; Transfer bit 8 of wIndex into IRP
	btfsc	BufferData+wIndexHigh, 0
	bsf	STATUS, IRP

	movf	BufferData+wIndex, w ; And bits 0-7 into FSR
	movwf	FSR

	movf	INDF, w			; Save the referenced byte in temp
	movwf	temp

	banksel	buffer_ptr
	movf	buffer_ptr,w	; get address of buffer
	movwf	FSR
	bsf 	STATUS,IRP		; indirectly to banks 2-3

	movf	temp, w			;  Write the saved byte to our buffer
	movwf	INDF

	banksel	buffer_ptr
	incf	buffer_ptr, f	; Next destination byte...

	banksel	BufferData
	incf	BufferData+wIndex, f ; Next source byte...
	btfsc	STATUS, C		; If we get a carry, increment the high byte
	incf	BufferData+wIndexHigh, f

	banksel	byte_iterator	; Loop over all 8 bytes...
	decfsz	byte_iterator, f
	goto	peek8Loop

	banksel	BD0IBC
	bsf 	STATUS, RP0
	movlw	0x08
	movwf	BD0IBC			; set byte count to 8
	movlw	0xc8			; DATA1 packet, DTS enabled
	movwf	BD0IST			; give buffer back to SIE
	return

	;********************* Request to read the value of a pin descriptor
	; pin descriptor in wIndex
GpioReadRequest
	banksel BufferData
	movf	BufferData+wIndex, w
	banksel	io_pin
	movwf	io_pin
	pagesel	io_Read
	call	io_Read

	movwf	temp		; Save the return value in temp

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

	;********************* Request to read all A/D converters
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

	pagesel	aquisitionLoop
	banksel	acq_iterator
	movlw	0xFF		; Wait a while to for the ADC holding capacitor to aquire the signal.
	movwf	acq_iterator ; This should be plenty long enough for an input with an impedence up to 1k ohm
aquisitionLoop
	decfsz	acq_iterator, f
	goto	aquisitionLoop	

	banksel	ADCON0
	bsf	ADCON0, GO		; Start the ADC
	pagesel	adFinishLoop ; Wait for it to finish
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
	goto	adChannelLoop

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
	pagesel	rx_Reset		; Cancel any receive we may be doing
	call	rx_Reset

	pagesel	skipTx
	banksel	BufferData
	movf	BufferData+wValue, w ; See if we have anything to transmit
	btfsc	STATUS, Z
	goto	skipTx

	banksel	txe_pin		; turn on the transmit enable pin (0 will have no effect)
	movf	txe_pin, w
	banksel	io_pin
	movwf	io_pin
	pagesel	io_Assert
	call	io_Assert

	banksel BufferData
	bcf	STATUS, IRP	; Transfer bit 8 of wIndex into IRP
	btfsc	BufferData+(wIndex+1), 0
	bsf	STATUS, IRP
	movf	STATUS, w	; Save STATUS, containing the buffer's IRP bit, for receiving later
	banksel	rx_status
	movwf	rx_status

	banksel	BufferData
	movf	BufferData+wIndex, w ; Load address bits 0-7 into FSR
	movwf	FSR
	banksel	rx_fsr		; And save it for receiving later
	movwf	rx_fsr
	movwf	rx_fsr_initial

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
	pagesel	io_Deassert
	call	io_Deassert

skipTx

	; Reset the USART receiver and empty its buffer, so we don't receive a
	; stale byte from before now (including bytes that may have been echoed
	; back during transmit on a half-duplex line)
	banksel	RCSTA
	bcf		RCSTA, CREN
	bsf		RCSTA, CREN
	banksel	RCREG
	movf	RCREG, w

	; Store the number of bytes to receive before rolling over
	banksel	BufferData
	movf	BufferData+(wValue+1), w
	banksel	rx_remaining
	movwf	rx_remaining
	movwf	rx_size

	; Acknowledge the request
	pagesel	Send_0Len_pkt
	call	Send_0Len_pkt
	return

	;********************* Request to get USART reception progress
	; Returns the number of bytes received without cancelling the reception
RxProgressRequest
	banksel	BD0IAL
	movf	low BD0IAL,w	; get address of buffer
	movwf	FSR
	bsf 	STATUS,IRP		; indirectly to banks 2-3

	banksel	rx_count
	movf	rx_count, w		;  Write the received byte count to our buffer
	movwf	INDF

	banksel	BD0IBC
	bsf 	STATUS, RP0
	movlw	0x01
	movwf	BD0IBC			; set byte count to 1
	movlw	0xc8			; DATA1 packet, DTS enabled
	movwf	BD0IST			; give buffer back to SIE
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
