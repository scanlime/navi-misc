;################################################################################
;
; commands.asm - Implementation of our vendor-specific commands
;
; The RCPOD project
; Copyright (C) 2004 Micah Dowty <micah@navi.cx>
;
;  This library is free software; you can redistribute it and/or
;  modify it under the terms of the GNU Lesser General Public
;  License as published by the Free Software Foundation; either
;  version 2.1 of the License, or (at your option) any later version.
;
;  This library is distributed in the hope that it will be useful,
;  but WITHOUT ANY WARRANTY; without even the implied warranty of
;  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
;  Lesser General Public License for more details.
;
;  You should have received a copy of the GNU Lesser General Public
;  License along with this library; if not, write to the Free Software
;  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
;
;###############################################################################

	errorlevel -226		; suppress the crazy include file warnings on gpasm

#include <p16C765.inc>
#include "usb_defs.inc"
#include "../include/rcpod_protocol.h"

	errorlevel -302			; supress "register not in bank0, check page bits" message

	global	CheckVendor

	extern	BufferData
	extern	wrongstate
	extern	temp
	extern	Send_0Len_pkt

	extern	rx_fsr_initial		; Initial buffer pointer, rx_fsr is reset to this when rx_remaining runs out
	extern	rx_fsr			; Pointer into the buffer where the next received byte will be stored
	extern	rx_status		; STATUS byte containing an IRP bit pointing to our buffer
	extern	rx_remaining		; Number of bytes until the buffer must roll over
	extern	rx_size			; Initial rx_remaining, and the value it is reset to when it hits zero
	extern	rx_count		; Total number of received bytes, modulo 256
	extern	rx_Reset		; Resets the above values to the receiver's idle state

	extern	io_Assert
	extern	io_Deassert
	extern	io_Read
	extern	io_pin

	extern	i2c_speed
	extern	i2c_clock_pin
	extern	i2c_data_pin
	extern	i2c_address
	extern	i2c_ack_count

	extern	i2c_Reset
	extern	i2c_Write
	extern	i2c_Read

	global	txe_pin

bank1	udata

txe_pin		res	1		 ; Pin number for transmit enable
last_poke_addr	res	2		 ; Address of the last byte of the most recently completed 'poke'
byte_count	res	1
buffer_ptr	res	1
acq_iterator	res	1		 ; Iterator for analog aquisition

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


;; **********************************************************************************
;; *************************************************************** Utilities ********
;; **********************************************************************************

	;; Send a 1-byte packet holding 'w', similar to Send_0Len_pkt.
Send_Byte_pkt
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


;; **********************************************************************************
;; *************************************************************** Memory Access ****
;; **********************************************************************************

	;**** Request to write a byte to RAM
	; A 9-bit address in wIndex, 8-bit data in wValue
request_Poke
	banksel BufferData
	movf	BufferData+wIndex, w		; Save this poke address
	banksel	last_poke_addr
	movwf	last_poke_addr
	banksel	BufferData
	movf	BufferData+(wIndex+1), w
	banksel	last_poke_addr
	movwf	last_poke_addr+1

	banksel	BufferData
	bcf	STATUS, IRP			; Transfer bit 8 of wIndex into IRP
	btfsc	BufferData+wIndexHigh, 0
	bsf	STATUS, IRP

	movf	BufferData+wIndex, w		; And bits 0-7 into FSR
	movwf	FSR

	movf	BufferData+wValue, w		; Now write bits 0-7 of wValue to [IRP:FSR]
	movwf	INDF

	; Acknowledge the request
	psgoto	Send_0Len_pkt


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

	movf	INDF, w
	psgoto	Send_Byte_pkt


	;**** Request to read a 8 contiguous bytes from RAM
	; 9-bit address in wIndex, returns an 8-byte packet
request_Peek8
	banksel	BD0IAL
	movf	low BD0IAL,w	; get address of buffer
	banksel	buffer_ptr
	movwf	buffer_ptr		; Start a buffer pointer we'll increment...

	banksel	byte_count
	movlw	8				; We're peeking 8 bytes
	movwf	byte_count
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

	banksel	byte_count	; Loop over all 8 bytes...
	decfsz	byte_count, f
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
	psgoto	Send_Byte_pkt


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
	banksel	rx_count
	movf	rx_count, w
	psgoto	Send_Byte_pkt


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


	;; **** Set the current I2C bus, address, and speed
request_I2CSet
	banksel	BufferData
	movf	BufferData+(wIndex+1), w
	banksel	i2c_speed
	movwf	i2c_speed

	banksel	BufferData
	movf	BufferData+wIndex, w
	banksel	i2c_address
	movwf	i2c_address

	banksel	BufferData
	movf	BufferData+(wValue+1), w
	banksel	i2c_clock_pin
	movwf	i2c_clock_pin

	banksel	BufferData
	movf	BufferData+wValue, w
	banksel	i2c_data_pin
	movwf	i2c_data_pin

	pscall	i2c_Reset
	psgoto	Send_0Len_pkt


	;; **** These commands all write up to 4 bytes stored in the setup packet
request_I2CWrite0
	movlw	0
	psgoto	write4_common
request_I2CWrite1
	movlw	1
	psgoto	write4_common
request_I2CWrite2
	movlw	2
	psgoto	write4_common
request_I2CWrite3
	movlw	3
	psgoto	write4_common
request_I2CWrite4
	movlw	4
write4_common
	banksel	byte_count
	movwf	byte_count		; Our write size is in byte_count now

	banksel	i2c_ack_count		; Clear the ACK counter
	clrf	i2c_ack_count

	bankisel BufferData		; Point IRP:FSR at our setup packet
	movlw	BufferData+wValue
	movwf	FSR

	banksel	byte_count		; Perform the write
	movf	byte_count, w
	pscall	i2c_Write

	banksel	i2c_ack_count
	movf	i2c_ack_count, w	; Send back the ACK count
	psgoto	Send_Byte_pkt


	;; *** Just read wValue bytes
request_I2CRead8
	banksel	i2c_ack_count
	clrf	i2c_ack_count

	banksel	BD0IAL			; Point IRP:FSR at the EP0 IN buffer
	movf	low BD0IAL,w
	movwf	FSR
	bsf 	STATUS,IRP

	banksel BufferData		; Get the number of bytes to receive
	movf	BufferData+wValue, w
	banksel	BD0IBC
	movwf	BD0IBC			; Save this as the EP0 IN byte count too

	pscall	i2c_Read		; Receive...

	banksel	i2c_ack_count
	movf	i2c_ack_count, w	; We should have 1 ACK by now
	xorlw	0x01
	pagesel	Send_0Len_pkt
	btfss	STATUS, Z
	goto	Send_0Len_pkt

	movlw	0xc8			; DATA1 packet, DTS enabled
	movwf	BD0IST			; give buffer back to SIE
	return


	;; **** Write one byte from wValue, read and return wIndex bytes of data
request_I2CW1Read8
	banksel	i2c_ack_count
	clrf	i2c_ack_count

	bankisel BufferData		; Point IRP:FSR at our one byte to transmit
	movlw	BufferData+wValue
	movwf	FSR

	movlw	1			; Write one byte
	pscall	i2c_Write

	banksel	i2c_ack_count
	movf	i2c_ack_count, w	; If it didn't ACK twice, quit now
	xorlw	0x02
	pagesel	Send_0Len_pkt
	btfss	STATUS, Z
	goto	Send_0Len_pkt

	banksel	BD0IAL			; Point IRP:FSR at the EP0 IN buffer
	movf	low BD0IAL,w
	movwf	FSR
	bsf 	STATUS,IRP

	banksel BufferData		; Get the number of bytes to receive
	movf	BufferData+wIndex, w
	banksel	BD0IBC
	movwf	BD0IBC			; Save this as the EP0 IN byte count too

	pscall	i2c_Read		; Receive...

	banksel	i2c_ack_count
	movf	i2c_ack_count, w	; We should have 3 ACKs total by now
	xorlw	0x03
	pagesel	Send_0Len_pkt
	btfss	STATUS, Z
	goto	Send_0Len_pkt

	banksel	BD0IST
	movlw	0xc8			; DATA1 packet, DTS enabled
	movwf	BD0IST			; give buffer back to SIE
	return


	;; **** Write and/or read any number of bytes to the buffer at wIndex.
	;; Bytes to transmit are in wValue low, bytes to receive are in wValue high.
	;; Returns the ACK count.
request_I2CTxRx
	banksel	i2c_ack_count
	clrf	i2c_ack_count

	banksel	BufferData
	bcf	STATUS, IRP			; Transfer bit 8 of wIndex into IRP
	btfsc	BufferData+wIndexHigh, 0
	bsf	STATUS, IRP

	movf	BufferData+wIndex, w		; Load write address
	movwf	FSR
	movf	BufferData+wValue, w		; Load write count
	pagesel	i2c_Write
	btfss	STATUS, Z			; Skip if the write count is zero
	call	i2c_Write			; Perform the write

	banksel	i2c_ack_count			; If the write was successful, we got write_count+1 ACKs
	decf	i2c_ack_count, w
	banksel	BufferData
	xorwf	BufferData+wValue, w
	pagesel	i2c_txrx_write_failed
	btfss	STATUS, Z
	goto	i2c_txrx_write_failed

	banksel	BufferData
	movf	BufferData+wIndex, w		; Load read address
	movwf	FSR
	movf	BufferData+(wValue+1), w	; Load read count
	pagesel	i2c_Read
	btfss	STATUS, Z			; Skip if the read count is zero
	call	i2c_Read			; Perform the read

i2c_txrx_write_failed
	banksel	i2c_ack_count
	movf	i2c_ack_count, w		; Send back the ACK count
	psgoto	Send_Byte_pkt

	end

;### The End ###
