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

#include <p16C765.inc>
#include "usb_defs.inc"
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
	extern	display_seek
	extern	display_seq_write_byte
	extern	back_buffer

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
	defineRequest	LEDBOARD_CTRL_FLIP,		request_flip
	defineRequest	LEDBOARD_CTRL_SEQ_WRITE12,	request_seqWrite12
	defineRequest	LEDBOARD_CTRL_SEQ_WRITE4,	request_seqWrite4
	defineRequest	LEDBOARD_CTRL_RANDOM_WRITE8,	request_randomWrite8
	defineRequest	LEDBOARD_CTRL_BLIT,		request_blit

	pagesel	wrongstate		; Not a recognized request
	goto	wrongstate


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

	; Request a page flip at the display's next opportunity
request_flip
	pagesel	display_request_flip
	call	display_request_flip
	returnEmpty


	; Perform a random 3-byte write to the display, using
	; the low byte of wValue as a column address.
request_randomWrite3
	banksel	BufferData
	movf	BufferData+wValue, w
	pagesel	display_seek
	call	display_seek

	banksel	BufferData
	movf	BufferData+(wValue+1), w
	pagesel	display_seq_write_byte
	call	display_seq_write_byte

	banksel	BufferData
	movf	BufferData+wIndex, w
	pagesel	display_seq_write_byte
	call	display_seq_write_byte

	banksel	BufferData
	movf	BufferData+(wIndex+1), w
	pagesel	display_seq_write_byte
	call	display_seq_write_byte
	returnEmpty


	; Position the write pointer at the high byte of wValue,
	; and write out wIndex bytes copied from the column address
	; at the low byte of wValue.
request_blit
	banksel	BufferData					; Put the pointer at wValue+1
	movf	BufferData+(wValue+1), w
	pagesel	display_seek
	call	display_seek

blit_loop
	banksel	BufferData					; Get the backbuffer data at the low byte of wValue
	movf	BufferData+wValue, w
	incf	BufferData+wValue, f		; (and increment it)
	bankisel back_buffer
	addlw	back_buffer
	movwf	FSR
	movf	INDF, w						; Copy to w and write it
	pagesel	display_seq_write_byte
	call	display_seq_write_byte

	banksel	BufferData					; Loop, counting bytes with wIndex
	pagesel	blit_loop
	decfsz	BufferData+wIndex, f
	goto	blit_loop
	returnEmpty


	; Sequentially write 4 bytes from wValue and wIndex
request_seqWrite4
	banksel	BufferData
	movf	BufferData+wValue, w
	pagesel	display_seq_write_byte
	call	display_seq_write_byte

	banksel	BufferData
	movf	BufferData+(wValue+1), w
	pagesel	display_seq_write_byte
	call	display_seq_write_byte

	banksel	BufferData
	movf	BufferData+wIndex, w
	pagesel	display_seq_write_byte
	call	display_seq_write_byte

	banksel	BufferData
	movf	BufferData+(wIndex+1), w
	pagesel	display_seq_write_byte
	call	display_seq_write_byte
	returnEmpty


	; Seek to the column given in the low byte of wValue, write data sent to EP0
request_randomWrite8
	banksel	BufferData
	movf	BufferData+wValue, w
	pagesel	display_seek
	call	display_seek

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
	pagesel	display_seq_write_byte
	call	display_seq_write_byte

	banksel	BufferData
	movf	BufferData+(wValue+1), w
	pagesel	display_seq_write_byte
	call	display_seq_write_byte

	banksel	BufferData
	movf	BufferData+wIndex, w
	pagesel	display_seq_write_byte
	call	display_seq_write_byte

	banksel	BufferData
	movf	BufferData+(wIndex+1), w
	pagesel	display_seq_write_byte
	call	display_seq_write_byte

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

	movf    BD0OBC, w
	movwf   byte_iterator
ep0loop
	movf    src_ptr,w		; get address of buffer
	movwf   FSR
	bsf     STATUS,IRP		; indirectly to banks 2-3
	movf	INDF, w
	pagesel	display_seq_write_byte
	call	display_seq_write_byte

	banksel	src_ptr
	incf    src_ptr, f              ; Next...
	pagesel	ep0loop
	decfsz  byte_iterator, f
	goto    ep0loop
	return

	end

;### The End ###
