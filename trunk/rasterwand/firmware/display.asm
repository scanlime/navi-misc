;################################################################################
;
; display.asm - Display initialization, timing, and control algorithms
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

	errorlevel -302		; supress "register not in bank0, check page bits" message

	global	display_poll

bank0	udata

wand_position	res	1 	; Gives the wand's current predicted position, scaled from 0 to 255
edge_buffer		res	8	; Stores 16-bit duration values at the last 4 angle sensor edges

unbanked	udata_shr

display_flags	res	1
delta_t			res	2	; 16-bit time delta, in cycles, since the last poll

#define FLAG_DIR_RIGHT	display_flags, 0	; Flag indicating that the wand is moving right,
											; unset when the wand is moving left.

#define FLAG_ASENSOR_TEMP display_flags, 1	; The previous sample recorded from the angle sensor

	code

;********************************************* Polling handler

	; Every iteration through the main loop, we get called.
	; The full oscillation period of the wand is divided into 510
	; position units, 255 right followed by 255 left. The width of
	; one position unit is defined by the overflow period of TMR1.
	; The reset value of TMR1 is synchronized to the actual oscillation
	; period of the wand.
display_poll

	; Calculate delta_t while resetting the timer, careful not to lose any cycles
	banksel	T1CON
	bcf		T1CON, TMR1ON		; Turn the timer off, so we don't have a moving target
	movf	TMR1L, w			; Copy its value
	movwf	delta_t
	movf	TMR1H, w
	movwf	delta_t+1
	clrf	TMR1H				; Reset the timer
	clrf	TMR1L
	bsf		T1CON, TMR1ON		; Turn the timer back on
	movlw	7					; Add back in the 7 cycles we lost
	addwf	delta_t, f
	btfsc	STATUS, C			; ...with carry
	incf	delta_t+1, f
	
	; Add our current delta-t to the latest slot on the edge buffer
	banksel	edge_buffer
	movf	delta_t+1, w
	addwf	edge_buffer+7, f
	movf	delta_t, w
	addwf	edge_buffer+6, f
	btfsc	STATUS, C
	incf	edge_buffer+7, f

	; Handle edges from the angle sensor
	clrw					; Transfer the angle_sensor bit into W
	banksel	PORTC
	btfsc	ANGLE_SENSOR
	movlw	0xFF
	btfsc	FLAG_ASENSOR_TEMP	; XOR with FLAG_ASENSOR_TEMP
	xorlw	0xFF
	xorlw	0x00			; Test and set the Z flag
	pagesel	handle_angle_edge
	btfss	STATUS, Z
	call	handle_angle_edge

	return


	; This is invoked when our angle sensor's value doesn't match FLAG_ASENSOR_TEMP.
	; Copies the value back into FLAG_ASENSOR_TEMP so the next edge detection works,
	; then takes a sample for edge_buffer.
handle_angle_edge
	bcf		FLAG_ASENSOR_TEMP	; Copy ANGLE_SENSOR to FLAG_ASENSOR_TEMP
	banksel	PORTC
	btfsc	ANGLE_SENSOR
	bsf		FLAG_ASENSOR_TEMP

	banksel	edge_buffer
	comf	edge_buffer+7, w
	banksel	PORTB
	movwf	PORTB

	banksel	edge_buffer
	movf	edge_buffer+2, w	; Scroll the edge_buffer
	movwf	edge_buffer
	movf	edge_buffer+3, w
	movwf	edge_buffer+1
	movf	edge_buffer+4, w
	movwf	edge_buffer+2
	movf	edge_buffer+5, w
	movwf	edge_buffer+3
	movf	edge_buffer+6, w
	movwf	edge_buffer+4
	movf	edge_buffer+7, w
	movwf	edge_buffer+5

	return

	end

;### The End ###
