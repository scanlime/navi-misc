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
	extern	back_buffer
	extern	display_fwd_phase
	extern	display_rev_phase
	extern	display_column_width
	extern	display_gap_width
	extern 	num_active_columns

	extern	display_request_flip
	extern	display_save_status

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
	defineRequest	RWAND_CTRL_READ_STATUS,		request_readStatus
	defineRequest	RWAND_CTRL_SET_PREDICTION,	request_setPrediction
	defineRequest	RWAND_CTRL_SET_COIL_PHASE,	request_setCoilPhase
	defineRequest	RWAND_CTRL_SET_MODES,		request_setModes
	defineRequest	RWAND_CTRL_SET_DISPLAY_PHASE, request_setDisplayPhase
	defineRequest	RWAND_CTRL_SET_COLUMN_WIDTH, request_setColumnWidth
	defineRequest	RWAND_CTRL_RANDOM_WRITE3,	request_randomWrite3
	defineRequest	RWAND_CTRL_FLIP,			request_flip
	defineRequest	RWAND_CTRL_SET_PERIOD,		request_setPeriod
	defineRequest	RWAND_CTRL_SET_NUM_COLUMNS,	request_setNumColumns

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


    ; Return 1 byte from 'w'
returnByte    macro
	movwf	temp					; Save the byte

    banksel BD0IAL
    movf    low BD0IAL,w			; Get the address of the EP0 IN buffer
    movwf   FSR
    bsf     STATUS,IRP				; indirectly to banks 2-3

    movf    temp, w                 ;  Write the saved byte to our buffer
    movwf   INDF

    banksel BD0IBC
    bsf     STATUS, RP0
    movlw   1
    movwf   BD0IBC                  ; set byte count
    movlw   0xc8                    ; DATA1 packet, DTS enabled
    movwf   BD0IST                  ; give buffer back to SIE
    return
    endm


;********************************************** Request handlers

	; Return edge buffer contents
request_readEdges
	returnBuffer edge_buffer, 8


	; Return a status packet
request_readStatus
    banksel BD0IAL
    movf    low BD0IAL,w			; Get the address of the EP0 IN buffer
    movwf   FSR
    bsf     STATUS,IRP				; indirectly to banks 2-3

	pagesel	display_save_status
	call	display_save_status

    banksel BD0IBC
    bsf     STATUS, RP0
    movlw   8
    movwf   BD0IBC                  ; set byte count
    movlw   0xc8                    ; DATA1 packet, DTS enabled
    movwf   BD0IST                  ; give buffer back to SIE
    return


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

	; Set prediction balues: wValue -> period
request_setPeriod
	banksel	BufferData
	movf	BufferData+wValue, w
	banksel	wand_period
	movwf	wand_period
	banksel	BufferData
	movf	BufferData+(wValue+1), w
	banksel	wand_period
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


	; Set display phase values, wValue -> forward and wIndex -> reverse
request_setDisplayPhase
	banksel	BufferData
	movf	BufferData+wValue, w
	banksel	display_fwd_phase
	movwf	display_fwd_phase
	banksel	BufferData
	movf	BufferData+(wValue+1), w
	banksel	display_fwd_phase
	movwf	display_fwd_phase+1
	banksel	BufferData
	movf	BufferData+wIndex, w
	banksel	display_fwd_phase
	movwf	display_rev_phase
	banksel	BufferData
	movf	BufferData+(wIndex+1), w
	banksel	display_fwd_phase
	movwf	display_rev_phase+1
	returnEmpty


	; Set the number of active columns from wValue, clamping at NUM_COLUMNS
request_setNumColumns
	banksel	BufferData
	movf	BufferData+wValue, w
	banksel	num_active_columns
	movwf	num_active_columns
	returnEmpty


	; Set column width from wValue
request_setColumnWidth
	banksel	BufferData
	movf	BufferData+wValue, w
	banksel	display_column_width
	movwf	display_column_width
	banksel	BufferData
	movf	BufferData+(wValue+1), w
	banksel	display_column_width
	movwf	display_column_width+1
	banksel	BufferData
	movf	BufferData+wIndex, w
	banksel	display_gap_width
	movwf	display_gap_width
	banksel	BufferData
	movf	BufferData+(wIndex+1), w
	banksel	display_gap_width
	movwf	display_gap_width+1
	returnEmpty


	; Request a page flip at the display's next opportunity
request_flip
	pagesel	display_request_flip
	call	display_request_flip
	returnEmpty


	; Perform a random 3-byte write to the display, using
	; the low byte of wValue as a column address.
request_randomWrite3
	bankisel back_buffer		; Point IRP:FSR at the column of interest.
	banksel	BufferData			; FIXME: no bounds checking here yet
	movf	BufferData+wValue, w
	addlw	back_buffer
	movwf	FSR
	
	movf	BufferData+(wValue+1), w	; Write 1...
	movwf	INDF
	incf	FSR, f
	movf	BufferData+wIndex, w		; Write 2...
	movwf	INDF
	incf	FSR, f
	movf	BufferData+(wIndex+1), w	; Write 3...
	movwf	INDF
	returnEmpty

	end

;### The End ###
