;################################################################################
;
; commands.asm - Implementation of our vendor-specific commands
;
; The Raster Wand project
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

#include <p16C745.inc>
#include "hardware.inc"
#include "usb_defs.inc"
#include "../include/rwand_protocol.h"

	errorlevel -302		; supress "register not in bank0, check page bits" message

	global	CheckVendor

	extern	BufferData
	extern	wrongstate
	extern	temp
	extern	Send_0Len_pkt

	extern	edge_buffer
	extern	wand_period
	extern	wand_phase
	extern	coil_window_min
	extern	coil_window_max
	extern	mode_flags

bank0	udata

src_ptr                 res     1
dest_ptr                res     1
byte_iterator   res     1       

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
	defineRequest	RWAND_CTRL_READ_EDGES,		request_readEdges
	defineRequest	RWAND_CTRL_READ_PREDICTION,	request_readPrediction
	defineRequest	RWAND_CTRL_SET_PREDICTION,	request_setPrediction
	defineRequest	RWAND_CTRL_SET_COIL_PHASE,	request_setCoilPhase
	defineRequest	RWAND_CTRL_SET_MODES,		request_setModes

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
    movf    low BD0IAL,w    ; Get the address of the EP0 IN buffer
    banksel dest_ptr
    movwf   dest_ptr                ; Start a buffer pointer we'll increment...

    movlw   length
    movwf   byte_iterator
loop

    bankisel buffer
    movf    src_ptr, w
    movwf   FSR

    movf    INDF, w                 ; Save the referenced byte in temp
    movwf   temp

    movf    dest_ptr,w              ; get address of buffer
    movwf   FSR
    bsf     STATUS,IRP              ; indirectly to banks 2-3

    movf    temp, w                 ;  Write the saved byte to our buffer
    movwf   INDF

    incf    src_ptr, f              ; Next...
    incf    dest_ptr, f
    banksel byte_iterator
    decfsz  byte_iterator, f
    goto    loop

    banksel BD0IBC
    bsf     STATUS, RP0
    movlw   length
    movwf   BD0IBC                  ; set byte count
    movlw   0xc8                    ; DATA1 packet, DTS enabled
    movwf   BD0IST                  ; give buffer back to SIE
    return
    endm


;********************************************** Request handlers

	; Return edge buffer contents
request_readEdges
	returnBuffer edge_buffer, 8

	; Return prediction values
request_readPrediction
	returnBuffer wand_period, 4

	; Set prediction balues: wValue -> phase, wIndex -> period
request_setPrediction
	banksel	BufferData
	movf	BufferData+wValue, w
	banksel	wand_phase
	movwf	wand_phase
	banksel	BufferData
	movf	BufferData+(wValue+1), w
	banksel	wand_phase
	movwf	wand_phase+1
	banksel	BufferData
	movf	BufferData+wIndex, w
	banksel	wand_phase
	movwf	wand_period
	banksel	BufferData
	movf	BufferData+(wIndex+1), w
	banksel	wand_phase
	movwf	wand_period+1
	returnEmpty

	; Set coil driver, enable between wValue and wIndex
request_setCoilPhase
	banksel	BufferData
	movf	BufferData+wValue, w
	banksel	coil_window_min
	movwf	coil_window_min
	banksel	BufferData
	movf	BufferData+(wValue+1), w
	banksel	coil_window_min
	movwf	coil_window_min+1
	banksel	BufferData
	movf	BufferData+wIndex, w
	banksel	coil_window_min
	movwf	coil_window_max
	banksel	BufferData
	movf	BufferData+(wIndex+1), w
	banksel	coil_window_min
	movwf	coil_window_max+1
	returnEmpty

	; Set mode flags
request_setModes
	banksel	BufferData
	movf	BufferData+wValue, w
	movwf	mode_flags
	returnEmpty

	end

;### The End ###
