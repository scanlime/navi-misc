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
	global	edge_buffer

	global	wand_period	; NOTE: It's important that the order of wand_period and wand_phase
	global	wand_phase	;       be preserved, they're sent in one piece over USB.

bank0	udata

edge_buffer		res	8	; Stores 16-bit duration values at the last 4 angle sensor edges.
						; Units are 16 instruction cycles, or about 2.666us

wand_period		res	2	; The wand oscillation period, 16-bit little endian in 16-cycle units
wand_phase		res	2	; The current phase angle of the want, ranging from 0 to wand_period-1

unbanked	udata_shr

display_flags	res	1
delta_t			res	2	; 16-bit time delta, in cycles, since the last poll

#define FLAG_DIR_RIGHT	display_flags, 0	; Flag indicating that the wand is moving right,
											; unset when the wand is moving left.

#define FLAG_ASENSOR_TEMP display_flags, 1	; The previous sample recorded from the angle sensor

	code


;********************************************* Polling handler

display_poll

	; Give up and try back later if we're polling too often, to keep our
	; cycle-chopping below from losing us too much timer precision.
	movf	TMR1H, w
	btfsc	STATUS, Z
	return

	; Calculate delta_t while resetting the timer. This loses about 8 cycles,
	; but that's still within the precision that we're cutting off anyway.
	banksel	T1CON
	bcf		T1CON, TMR1ON		; Turn the timer off, so we don't have a moving target
	movf	TMR1L, w			; Copy its value
	movwf	delta_t
	movf	TMR1H, w
	movwf	delta_t+1
	clrf	TMR1H				; Reset the timer
	clrf	TMR1L
	bsf		T1CON, TMR1ON		; Turn the timer back on
	rrf		delta_t+1, f		; Divide by 2, to get a final 16:1 prescale
	rrf		delta_t, f
	bcf		delta_t+1, 7
	
	; Add our current delta-t to the predicted wand phase
	banksel	edge_buffer
	movf	delta_t+1, w
	addwf	wand_phase+1, f
	movf	delta_t, w
	addwf	wand_phase, f
	btfsc	STATUS, C
	incf	wand_phase+1, f

	; If wand_phase >= wand_period, subtract wand_period
	movf	wand_period+1, w	; Test high byte of wand_phase - wand_period
	subwf	wand_phase+1, w
	pagesel	phase_rollover
	btfsc	STATUS, C
	goto	phase_rollover		; C=1, B=0, wand_period <= wand_phase

	movf	wand_period+1, w	; Test high byte of wand_phase == wand_period
	subwf	wand_phase+1, w
	pagesel	no_phase_rollover
	btfss	STATUS, Z
	goto	no_phase_rollover	; Not equal, don't need to look at low byte
	movf	wand_period, w		; Test low byte of wand_phase - wand_period
	subwf	wand_phase, w
	btfsc	STATUS, C
	goto	no_phase_rollover	; C=0, B=1, wand_period > wand_phase

	; Subtract wand_period from wand_phase
phase_rollover
	bcf		PORTB, 0

	movf	wand_period, w
	subwf	wand_phase, f
	btfss	STATUS, C
	decf	wand_phase+1, f
	movf	wand_period+1, w
	subwf	wand_phase+1, f

	bsf		PORTB, 0

no_phase_rollover
	; Add our current delta-t to the latest slot on the edge buffer
	movf	delta_t+1, w
	addwf	edge_buffer+7, f
	movf	delta_t, w
	addwf	edge_buffer+6, f
	btfsc	STATUS, C
	incf	edge_buffer+7, f

	; Detect edges from the angle sensor. Give up now if we haven't found one.
	clrw						; Transfer the angle_sensor bit into W
	banksel	PORTC
	btfsc	ANGLE_SENSOR
	movlw	0xFF
	btfsc	FLAG_ASENSOR_TEMP	; XOR with FLAG_ASENSOR_TEMP
	xorlw	0xFF
	xorlw	0x00				; Test and set the Z flag
	btfsc	STATUS, Z
	return

	; Save the new current value of our angle sensor's output
	bcf		FLAG_ASENSOR_TEMP	; Copy ANGLE_SENSOR to FLAG_ASENSOR_TEMP
	banksel	PORTC
	btfsc	ANGLE_SENSOR
	bsf		FLAG_ASENSOR_TEMP

	; Attempt to synchronize using our current edge_buffer data
	pagesel	sync
	call	sync

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
	clrf	edge_buffer+6		; Leaving zeroes to accumulate into
	clrf	edge_buffer+7
	return


;********************************************* Synchronization

	; This uses the current contents of edge_buffer to try to synchronize
	; our angle predictor with the wand's actual angle. edge_buffer records
	; the time (in 16 clock cycle units) between edge detections on the
	; angle sensor. We take advantage of the asymmetry of the angle sensor
	; pulses to uniquely identify the want's period and phase.
sync

	; if edge[0] > edge[1] or edge[2] > edge[3], we're outside the sync
	; pulse and we'll wait until we aren't any more.
	banksel	edge_buffer
	movf	edge_buffer+1, w	; Test high byte of edge[1] - edge[0]
	subwf	edge_buffer+3, w
	btfss	STATUS, C
	return						; C=0, B=1, edge[0] > edge[1]

	movf	edge_buffer+5, w	; Test high byte of edge[3] - edge[2]
	subwf	edge_buffer+7, w
	btfss	STATUS, C
	return						; C=0, B=1, edge[2] > edge[3]
	
	movf	edge_buffer+1, w	; Test high byte of edge[1] == edge[0]
	subwf	edge_buffer+3, w
	pagesel	edge10_less
	btfss	STATUS, Z
	goto	edge10_less			; Not equal, don't need to look at low byte
	movf	edge_buffer, w		; Test low byte of edge[1] - edge[0]
	subwf	edge_buffer+2, w
	btfss	STATUS, C
	return						; C=0, B=1, edge[0] > edge[1]	
edge10_less

	movf	edge_buffer+5, w	; Test high byte of edge[3] == edge[2]
	subwf	edge_buffer+7, w
	pagesel	edge10_less
	btfss	STATUS, Z
	goto	edge32_less			; Not equal, don't need to look at low byte
	movf	edge_buffer+4, w		; Test low byte of edge[3] - edge[2]
	subwf	edge_buffer+6, w
	btfss	STATUS, C
	return						; C=0, B=1, edge[3] > edge[2]	
edge32_less

	; Now we can reconstruct the period just by adding up all edge buffer samples
	clrf	wand_period			; wand_period = 0
	clrf	wand_period+1

	movf	edge_buffer, w		; Add in edge[0]
	addwf	wand_period, f
	btfsc	STATUS, C
	incf	wand_period+1, f
	movf	edge_buffer+1, w
	addwf	wand_period+1, f

	movf	edge_buffer+2, w	; Add in edge[1]
	addwf	wand_period, f
	btfsc	STATUS, C
	incf	wand_period+1, f
	movf	edge_buffer+3, w
	addwf	wand_period+1, f

	movf	edge_buffer+4, w	; Add in edge[2]
	addwf	wand_period, f
	btfsc	STATUS, C
	incf	wand_period+1, f
	movf	edge_buffer+5, w
	addwf	wand_period+1, f

	movf	edge_buffer+6, w	; Add in edge[3]
	addwf	wand_period, f
	btfsc	STATUS, C
	incf	wand_period+1, f
	movf	edge_buffer+7, w
	addwf	wand_period+1, f

	; Now to adjust the oscillator phase, we need to know which side of the
	; asymmetric period we're on. If step[1] < step[3] the origin is in the
	; center of step[1], otherwise the origin is in the center of step[3].
	movf	edge_buffer+7, w	; Test high byte of edge[1] - edge[3]
	subwf	edge_buffer+3, w
	pagesel	edge1_smaller
	btfss	STATUS, C
	goto	edge1_smaller		; C=0, B=1, edge[3] > edge[1]

	movf	edge_buffer+7, w	; Test high byte of edge[3] == edge[1]
	subwf	edge_buffer+3, w
	pagesel	edge3_smaller
	btfss	STATUS, Z
	goto	edge3_smaller		; Not equal, don't need to look at low byte
	movf	edge_buffer+6, w	; Test low byte of edge[1] - edge[3]
	subwf	edge_buffer+2, w
	pagesel	edge1_smaller
	btfss	STATUS, C
	goto	edge1_smaller		; C=0, B=1, edge[3] > edge[1]

	; edge[3] is smaller, and contains the origin.
	; Set the phase to edge[3]/2
edge3_smaller
	rrf		edge_buffer+7, w
	movwf	wand_phase+1
	rrf		edge_buffer+6, w
	movwf	wand_phase
	bcf		wand_phase+1, 7
	return

	; edge[1] is smaller, and contains the origin
	; Set the phase to edge[1]/2 + edge[2] + edge[3]
edge1_smaller
	rrf		edge_buffer+3, w	; Set to edge[1]/2
	movwf	wand_phase+1
	rrf		edge_buffer+2, w
	movwf	wand_phase
	bcf		wand_phase+1, 7
	movf	edge_buffer+4, w	; Add in edge[2]
	addwf	wand_phase, f
	btfsc	STATUS, C
	incf	wand_phase+1, f
	movf	edge_buffer+5, w
	addwf	wand_phase+1, f
	movf	edge_buffer+6, w	; Add in edge[3]
	addwf	wand_phase, f
	btfsc	STATUS, C
	incf	wand_phase+1, f
	movf	edge_buffer+7, w
	addwf	wand_phase+1, f
	return

	end

;### The End ###
