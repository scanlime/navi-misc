;################################################################################
;
; commands.asm - Implementation of our vendor-specific commands
;
; This file is part of the rcpod project. This file contains original code,
; released into the public domain.
;
; Micah Dowty <micah@navi.cx>
;
;###############################################################################

	errorlevel -226		; suppress the crazy include file warnings on gpasm

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

;; **********************************************************************************
;; *************************************************************** Request Table ****
;; **********************************************************************************

defineRequest	macro	id,	handler
	movf	BufferData+bRequest,w
	xorlw	id
	pagesel	handler
	btfsc	STATUS,Z
	goto	handler
	endm

CheckVendor
	defineRequest	RCPOD_CTRL_POKE,		request_Poke
	defineRequest	RCPOD_CTRL_PEEK,		request_Peek
	defineRequest	RCPOD_CTRL_POKE4,		request_Poke4
	defineRequest	RCPOD_CTRL_PEEK8,		request_Peek8
	defineRequest	RCPOD_CTRL_ANALOG_ALL,		request_AnalogAll
	defineRequest	RCPOD_CTRL_USART_TXRX,		request_UsartTxRx
	defineRequest	RCPOD_CTRL_USART_RX_PROGRESS,	request_UsartRxProgress
	defineRequest	RCPOD_CTRL_USART_TXE,		request_UsartTxe
	defineRequest	RCPOD_CTRL_GPIO_ASSERT,		request_GpioAssert
	defineRequest	RCPOD_CTRL_GPIO_READ,		request_GpioRead
	defineRequest	RCPOD_CTRL_I2C_SET,		request_I2CSet
	defineRequest	RCPOD_CTRL_I2C_WRITE0,		request_I2CWrite0
	defineRequest	RCPOD_CTRL_I2C_WRITE1,		request_I2CWrite1
	defineRequest	RCPOD_CTRL_I2C_WRITE2,		request_I2CWrite2
	defineRequest	RCPOD_CTRL_I2C_WRITE3,		request_I2CWrite3
	defineRequest	RCPOD_CTRL_I2C_WRITE4,		request_I2CWrite4
	defineRequest	RCPOD_CTRL_I2C_READ8,		request_I2CRead8
	defineRequest	RCPOD_CTRL_I2C_W1READ8,		request_I2CW1Read8
	defineRequest	RCPOD_CTRL_I2C_TXRX,		request_I2CTxRx

	pagesel	wrongstate		; Not a recognized request
	goto	wrongstate

	;********************* Request to write a byte to RAM
	; A 9-bit address in wIndex, 8-bit data in wValue
request_Poke
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
	psgoto	Send_0Len_pkt


;; **********************************************************************************
;; *************************************************************** Memory Access ****
;; **********************************************************************************

	; **** Request to write 4 bytes after the last one poked
request_Poke4

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
	psgoto	Send_0Len_pkt


	;**** Request to read a byte from RAM
	; 9-bit address in wIndex, an 8-bit value returned in a 1-byte packet
request_Peek
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


	;**** Request to read a 8 contiguous bytes from RAM
	; 9-bit address in wIndex, returns an 8-byte packet
request_Peek8
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


;; **********************************************************************************
;; *********************************************************** General Purpose I/O **
;; **********************************************************************************

	;**** Request to assert 4 pin descriptors
request_GpioAssert
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
	psgoto	Send_0Len_pkt


	;**** Request to read the value of a pin descriptor
	; pin descriptor in wIndex
request_GpioRead
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


	;**** Request to read all A/D converters
	; No inputs, returns an 8-byte packet
request_AnalogAll
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
	clrwdt
	decfsz	acq_iterator, f
	goto	aquisitionLoop

	banksel	ADCON0
	bsf	ADCON0, GO		; Start the ADC
	pagesel	adFinishLoop ; Wait for it to finish
adFinishLoop
	clrwdt
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


;; **********************************************************************************
;; *********************************************************** Serial Port **********
;; **********************************************************************************

	;**** Request to send then receive via the USART
	; A 9-bit buffer address in wIndex, number of bytes to transmit in wValue,
	; number of bytes to receive in wValue+1
request_UsartTxRx
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

	banksel	BufferData
	movf	BufferData+wIndex, w ; Load address bits 0-7 into FSR
	movwf	FSR

	pagesel	txLoop
txLoop
	clrwdt
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
	clrwdt
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

	; Store the STATUS register containing IRP from bit 8 of wIndex,
	; so we point to the right half of our address space when receiving
	banksel BufferData
	bcf	STATUS, IRP
	btfsc	BufferData+(wIndex+1), 0
	bsf	STATUS, IRP
	movf	STATUS, w
	banksel	rx_status
	movwf	rx_status

	; Save the lower 8 bits of wIndex for receiving
	banksel	BufferData
	movf	BufferData+wIndex, w
	banksel	rx_fsr
	movwf	rx_fsr
	movwf	rx_fsr_initial

	; Store the number of bytes to receive before rolling over
	banksel	BufferData
	movf	BufferData+(wValue+1), w
	banksel	rx_remaining
	movwf	rx_remaining
	movwf	rx_size

	; Acknowledge the request
	psgoto	Send_0Len_pkt


	;**** Request to get USART reception progress
	; Returns the number of bytes received without cancelling the reception
request_UsartRxProgress
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


	;**** Set the USART transmit enable pin
request_UsartTxe
	banksel BufferData
	movf	BufferData+wValue, w
	banksel	txe_pin
	movwf	txe_pin

	; Acknowledge the request
	psgoto	Send_0Len_pkt


;; **********************************************************************************
;; *********************************************************** I2C Master ***********
;; **********************************************************************************


request_I2CSet
	psgoto	Send_0Len_pkt

request_I2CWrite0
	psgoto	Send_0Len_pkt

request_I2CWrite1
	psgoto	Send_0Len_pkt

request_I2CWrite2
	psgoto	Send_0Len_pkt

request_I2CWrite3
	psgoto	Send_0Len_pkt

request_I2CWrite4
	psgoto	Send_0Len_pkt

request_I2CRead8
	psgoto	Send_0Len_pkt

request_I2CW1Read8
	psgoto	Send_0Len_pkt

request_I2CTxRx
	psgoto	Send_0Len_pkt

	end

;### The End ###
