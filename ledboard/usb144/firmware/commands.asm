;################################################################################
;
; commands.asm - Implementation of our vendor-specific commands
;
; The LED board project
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

	errorlevel -226		; suppress gpasm's silly include file warning

#include <p16c765.inc>
#include "usb_defs.inc"
#include "macros.inc"
#include "../include/ledboard_protocol.h"

	errorlevel -302		; suppress "register not in bank0, check page bits" message

	global	CheckVendor
	global	ep0_write_backbuffer

	extern	USB_dev_req
	extern	BufferData
	extern	wrongstate
	extern	temp
	extern	Send_0Len_pkt

	extern	display_request_flip
	extern	display_poll_flip
	extern	display_seek
	extern	display_seq_write_byte

	extern	pwm_cycles
	extern	pwm_table
	extern	scan_rate

bank0	udata

src_ptr		res     1
dest_ptr	res     1
byte_iterator	res     1

	code

;********************************************** Vendor-specific request table

defineRequest	macro	id,	handler
	movf	BufferData+bRequest,w
	xorlw	id
	pagesel	handler
	btfsc	STATUS,Z
	goto	handler
	endm

CheckVendor
	defineRequest	LEDBOARD_CTRL_RANDOM_WRITE3,	request_randomWrite3
	defineRequest	LEDBOARD_CTRL_REQUEST_FLIP,	request_requestFlip
	defineRequest	LEDBOARD_CTRL_SEQ_WRITE12,	request_seqWrite12
	defineRequest	LEDBOARD_CTRL_SEQ_WRITE4,	request_seqWrite4
	defineRequest	LEDBOARD_CTRL_RANDOM_WRITE8,	request_randomWrite8
	defineRequest	LEDBOARD_CTRL_SET_SCAN_RATE,	request_setScanRate
	defineRequest	LEDBOARD_CTRL_SET_PWM_CYCLES,	request_setPwmCycles
	defineRequest	LEDBOARD_CTRL_SET_PWM_ENTRY,	request_setPwmEntry
	defineRequest	LEDBOARD_CTRL_GET_LED_VOLTAGE,	request_getLedVoltage
	defineRequest	LEDBOARD_CTRL_POLL_FLIP,	request_pollFlip
	defineRequest	LEDBOARD_CTRL_STATUS_INTENSITY,	request_statusIntensity

	psgoto	wrongstate		; Not a recognized request


;********************************************** Macros


	; Acknowledge the command with a zero-length packet
returnEmpty		macro
	pagesel	Send_0Len_pkt
	goto	Send_0Len_pkt
	endm

    ; Return 'length' bytes of data copied from buffer
returnBuffer    macro   buffer, length
	local   loop
	movlw   buffer
	banksel src_ptr
	movwf   src_ptr

	banksel BD0IAL
	movf    low BD0IAL,w	; Get the address of the EP0 IN buffer
	banksel dest_ptr
	movwf   dest_ptr	; Start a buffer pointer we'll increment...

	movlw   length
	movwf   byte_iterator
loop

	bankisel buffer
	movf    src_ptr, w
	movwf   FSR

	movf    INDF, w		; Save the referenced byte in temp
	movwf   temp

	movf	dest_ptr, w	; get address of buffer
	movwf   FSR
	bsf	STATUS,IRP	; indirectly to banks 2-3

	movf	temp, w		;  Write the saved byte to our buffer
	movwf   INDF

	incf	src_ptr, f	; Next...
	incf	dest_ptr, f
	banksel byte_iterator
	decfsz  byte_iterator, f
	goto	loop

	banksel BD0IBC
	bsf	 STATUS, RP0
	movlw   length
	movwf   BD0IBC		; set byte count
	movlw   0xc8		; DATA1 packet, DTS enabled
	movwf   BD0IST		; give buffer back to SIE
	return
	endm


    ; Return 1 byte from 'w'
returnByte    macro
	movwf	temp		; Save the byte

	banksel BD0IAL
	movf    low BD0IAL,w	; Get the address of the EP0 IN buffer
	movwf   FSR
	bsf     STATUS,IRP	; indirectly to banks 2-3

	movf    temp, w         ;  Write the saved byte to our buffer
	movwf   INDF

	banksel BD0IBC
	bsf     STATUS, RP0
	movlw   1
	movwf   BD0IBC          ; set byte count
	movlw   0xc8            ; DATA1 packet, DTS enabled
	movwf   BD0IST          ; give buffer back to SIE
	return
	endm


;********************************************** Request handlers

	; Request a page flip at the display's next opportunity.
	; This acknowledges right away, even though the flip itself is asynchronous.
request_requestFlip
	pscall	display_request_flip
	returnEmpty

	
	;; Get the number of asynchronous page flips that have completed
	;; since the last time this request was issued. Returns a 1-byte packet.
request_pollFlip
	pscall	display_poll_flip
	returnByte

	
	; Perform a random 3-byte write to the display, using
	; the low byte of wValue as a column address.
request_randomWrite3
	banksel	BufferData
	movf	BufferData+wValue, w
	pscall	display_seek

	banksel	BufferData
	movf	BufferData+(wValue+1), w
	pscall 	display_seq_write_byte

	banksel	BufferData
	movf	BufferData+wIndex, w
	pscall 	display_seq_write_byte

	banksel	BufferData
	movf	BufferData+(wIndex+1), w
	pscall 	display_seq_write_byte
	returnEmpty


	; Sequentially write 4 bytes from wValue and wIndex
request_seqWrite4
	banksel	BufferData
	movf	BufferData+wValue, w
	pscall 	display_seq_write_byte

	banksel	BufferData
	movf	BufferData+(wValue+1), w
	pscall 	display_seq_write_byte

	banksel	BufferData
	movf	BufferData+wIndex, w
	pscall 	display_seq_write_byte

	banksel	BufferData
	movf	BufferData+(wIndex+1), w
	pscall 	display_seq_write_byte
	returnEmpty


	; Seek to the column given in the low byte of wValue, write data sent to EP0
request_randomWrite8
	banksel	BufferData
	movf	BufferData+wValue, w
	pscall 	display_seek

	; Don't acknowledge yet, wait for some data on EP0 OUT
	movlw	EP0_WRITE_BACKBUFFER
	banksel	USB_dev_req
	movwf	USB_dev_req
	return


	; Perform a sequential 12-byte write. Start with the 4 bytes in this
	; packet, and prepare to receive an 8-byte data packet.
request_seqWrite12
	banksel	BufferData
	movf	BufferData+wValue, w
	pscall 	display_seq_write_byte

	banksel	BufferData
	movf	BufferData+(wValue+1), w
	pscall 	display_seq_write_byte

	banksel	BufferData
	movf	BufferData+wIndex, w
	pscall 	display_seq_write_byte

	banksel	BufferData
	movf	BufferData+(wIndex+1), w
	pscall 	display_seq_write_byte

	; Don't acknowledge yet, wait for some data on EP0 OUT
	movlw	EP0_WRITE_BACKBUFFER
	banksel	USB_dev_req
	movwf	USB_dev_req
	return


	; Handles the data packets that should follow request_seqWrite12
ep0_write_backbuffer
	banksel BD0OAL
	movf    low BD0OAL,w    	; Get the address of the EP0 OUT buffer
	banksel src_ptr
	movwf   src_ptr                 ; Start a buffer pointer we'll increment...

	banksel	BD0OBC
	movf    BD0OBC, w
	banksel	byte_iterator
	movwf   byte_iterator
ep0loop
	banksel	src_ptr
	movf    src_ptr,w		; get address of buffer
	movwf   FSR
	bsf     STATUS,IRP		; indirectly to banks 2-3
	movf	INDF, w
	pscall	display_seq_write_byte

	banksel	src_ptr
	incf    src_ptr, f              ; Next...
	pagesel	ep0loop
	decfsz  byte_iterator, f
	goto    ep0loop
	return


	; Set the current LED scan rate, from wValue
request_setScanRate
	banksel	BufferData
	movf	BufferData+wValue, w
	banksel	scan_rate
	movwf	scan_rate+1

	banksel	BufferData
	movf	BufferData+(wValue+1), w
	banksel	scan_rate
	movwf	scan_rate
	returnEmpty


	; Set the number of PWM cycles, from wValue low
request_setPwmCycles
	banksel	BufferData
	movf	BufferData+wValue, w
	banksel	pwm_cycles
	movwf	pwm_cycles
	returnEmpty


	; Set one entry in our PWM table
request_setPwmEntry
	banksel	BufferData
	movf	BufferData+wIndex, w
	andlw	0x0F
	addlw	pwm_table
	bankisel pwm_table
	movwf	FSR
	movf	BufferData+wValue, w
	movwf	INDF
	returnEmpty


	;; Read back the LED supply voltage, from AN0.
	;; This is nice and simple with no precharge delay, since
	;; we only have one analog input and it was init'ed in usb_main.
request_getLedVoltage
	banksel	ADCON0
	bsf	ADCON0, GO
	pagesel	adc_loop
adc_loop
	btfsc	ADCON0, NOT_DONE
	goto	adc_loop
	movf	ADRES, w
	returnByte


	;; Set the intensity of the status LED (on CCP1) from the
	;; high 10 bits of wValue.
request_statusIntensity
	banksel	BufferData
	movf	BufferData+(wValue+1), w
	banksel	CCPR1L
	movwf	CCPR1L
	banksel	BufferData
	rrf	BufferData+wValue, f
	rrf	BufferData+wValue, w
	andlw	0x30
	iorlw	0x0F
	banksel	CCP1CON
	movwf	CCP1CON
	returnEmpty

	end

;### The End ###
