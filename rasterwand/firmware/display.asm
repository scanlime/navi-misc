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
#include "../include/rwand_protocol.h"

	errorlevel -302		; supress "register not in bank0, check page bits" message

	global	display_poll
	global	display_init

	global	edge_buffer
	global	wand_period	; NOTE: It's important that the order of wand_period and wand_phase
	global	wand_phase	;       be preserved, they're sent in one piece over USB.
	global	coil_window_min
	global	coil_window_max
	global	mode_flags

bank0	udata

edge_buffer		res	8	; Stores 16-bit duration values at the last 4 angle sensor edges.
						; Units are 16 instruction cycles, or about 2.666us

wand_period		res	2	; The wand oscillation period, 16-bit little endian in 16-cycle units
wand_phase		res	2	; The current phase angle of the want, ranging from 0 to wand_period-1

coil_window_min	res	2	; Minimum and maximum wand_phase to enable the coil for
coil_window_max	res	2

display_fwd_phase	res	2	; The wand_phase value to start forward display at
display_rev_phase	res 2	; The wand_phase value to start reverse display at
display_column_width res 2	; The wand_phase width of each column of pixels

unbanked	udata_shr

current_column		res	1	; The current display column, starting at zero
remaining_col_width	res 2	; Remaining width in this column

mode_flags		res	1	; Holds the mode flags, settable by the USB host. These are defined in rwand_protocol.h
display_flags	res	1
delta_t			res	2	; 16-bit time delta, in cycles, since the last poll

#define FLAG_ASENSOR_TEMP 	display_flags, 0	; The previous sample recorded from the angle sensor
#define FLAG_FLIP_REQUEST 	display_flags, 1	; At the beginning of the next display scan, copy the backbuffer
												; to the fontbuffer and clear this flag.
#define FLAG_DISPLAY_FWD	display_flags, 2	; Set if we're currently traversing video memory forward
#define FLAG_DISPLAY_REV	display_flags, 3	; Set if we're currently traversing video memory backwards
#define FLAG_FWD_TRIGGERED	display_flags, 4	; Keep track of whether we've done a forward scan yet this period
#define FLAG_REV_TRIGGERED	display_flags, 5	; Keep track of whether we've done a reverse scan yet this period
	
bank1	udata
front_buffer	res	NUM_COLUMNS
bank2	udata
back_buffer		res	NUM_COLUMNS

	code


;*****************************************************************************************
;************************************************************************ Initialization *
;*****************************************************************************************

display_init
	pagesel	edge_buffer
	clrf	edge_buffer
	clrf	edge_buffer+1
	clrf	edge_buffer+2
	clrf	edge_buffer+3
	clrf	edge_buffer+4
	clrf	edge_buffer+5
	clrf	edge_buffer+6
	clrf	edge_buffer+7
	clrf	wand_period
	clrf	wand_period+1
	clrf	wand_phase
	clrf	wand_phase+1
	clrf	coil_window_min
	clrf	coil_window_min+1
	clrf	coil_window_max
	clrf	coil_window_max+1
	clrf	mode_flags
	clrf	display_flags
	return


;*****************************************************************************************
;*********************************************************************** Polling handler *
;*****************************************************************************************

display_poll

	; Give up and try back later if we're polling too often, to keep our
	; cycle-chopping below from losing us too much timer precision.
	movf	TMR1H, w
	btfsc	STATUS, Z
	return

	pagesel	display_keep_time		; Must be done first, so we have valid angle predictions below
	call	display_keep_time

	pagesel	display_led_scan		; Must be done before drive_coil or sync, so they have
	call	display_led_scan		;   minimum impact on display timing.

	pagesel	display_drive_coil
	call	display_drive_coil

	pagesel	display_sync
	call	display_sync

	return


;*****************************************************************************************
;**************************************************************************** Timekeeper *
;*****************************************************************************************

	; Measure the delta-t since the last poll using TMR1, and update our wand predictor.
	; The period and phase of this wand predictor is corrected by the synchronization code.
display_keep_time

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
	movf	wand_period, w
	subwf	wand_phase, f
	btfss	STATUS, C
	decf	wand_phase+1, f
	movf	wand_period+1, w
	subwf	wand_phase+1, f

	bcf		FLAG_FWD_TRIGGERED	; Clear status flags used last period
	bcf		FLAG_REV_TRIGGERED

no_phase_rollover
	return


;*****************************************************************************************
;************************************************************************** LED Scanning *
;*****************************************************************************************

	; Update the state of our LED column scanning, and update the
	; LED_PORT from video memory.
display_led_scan

	; Is the display disabled?
	pagesel	led_scan_disabled
	btfss	mode_flags, RWAND_MODE_ENABLE_DISPLAY_BIT
	goto	led_scan_disabled

	; Check for the beginning of a forward scan
	pagesel	no_forward_scan
	btfsc	FLAG_FWD_TRIGGERED		; If we've already started, don't bother
	goto	no_forward_scan
	movf	display_fwd_phase+1, w	; Test high byte of wand_phase - display_fwd_phase
	subwf	wand_phase+1, w
	pagesel	no_forward_scan
	btfss	STATUS, C
	goto	no_forward_scan			; C=0, B=1, wand_phase < display_fwd_phase
	pagesel	start_forward_scan		; If the high bytes aren't equal, no further testing needed...
	btfss	STATUS, Z
	goto	start_forward_scan
	movf	display_fwd_phase, w	; Test low byte of wand_phase - display_fwd_phase
	subwf	wand_phase, w
	pagesel	no_forward_scan
	btfss	STATUS, C
	goto	no_forward_scan			; C=0, B=1, wand_phase < display_fwd_phase

	; Yay, start a forward display scan
start_forward_scan
	bsf		FLAG_DISPLAY_FWD			; Set flags
	bsf		FLAG_FWD_TRIGGERED
	clrf	current_column				; Start at the beginning
	movf	display_column_width, w		; Give this column all its allocated width
	movwf	remaining_col_width	
	movf	display_column_width+1, w
	movwf	remaining_col_width+1
no_forward_scan

	; Check for the beginning of a reverse scan
	pagesel	no_reverse_scan
	btfsc	FLAG_REV_TRIGGERED		; If we've already started, don't bother
	goto	no_reverse_scan
	movf	display_rev_phase+1, w	; Test high byte of wand_phase - display_rev_phase
	subwf	wand_phase+1, w
	pagesel	no_reverse_scan
	btfss	STATUS, C
	goto	no_reverse_scan			; C=0, B=1, wand_phase < display_rev_phase
	pagesel	start_reverse_scan		; If the high bytes aren't equal, no further testing needed...
	btfss	STATUS, Z
	goto	start_reverse_scan
	movf	display_rev_phase, w	; Test low byte of wand_phase - display_rev_phase
	subwf	wand_phase, w
	pagesel	no_reverse_scan
	btfss	STATUS, C
	goto	no_reverse_scan			; C=0, B=1, wand_phase < display_rev_phase

	; Yay, start a reverse display scan
start_reverse_scan
	bsf		FLAG_DISPLAY_REV			; Set flags
	bsf		FLAG_REV_TRIGGERED
	movlw	NUM_COLUMNS-1				; Start at the end
	movwf	current_column
	movf	display_column_width, w		; Give this column all its allocated width
	movwf	remaining_col_width	
	movf	display_column_width+1, w
	movwf	remaining_col_width+1
no_reverse_scan

	; Bounds check the current column. If it's out of range, disable current scans
	movlw	NUM_COLUMNS-1			; Test current_column - (NUM_COLUMNS-1)
	subwf	current_column, w
	pagesel	led_scan_disabled
	btfss	STATUS, C
	goto	led_scan_disabled		; C=0, B=1, (NUM_COLUMNS-1) > display_fwd_phase

	; Is there any scan in progress? If not...
	pagesel	scan_in_progress
	btfsc	FLAG_DISPLAY_FWD
	goto	scan_in_progress
	btfsc	FLAG_DISPLAY_REV
	goto	scan_in_progress

	; Blank the LEDs, clear the scan bits, and return.
	; This is invoked at the end of a scan, (once current_column is out of bounds)
	; if no scan has been started, or if the LED scanner has been disabled entirely.
led_scan_disabled
	movlw	0xFF
	banksel	LED_PORT
	movwf	LED_PORT
	bcf		FLAG_DISPLAY_FWD
	bcf		FLAG_DISPLAY_REV
	return

	; We have a scan (forward or reverse) in progress
scan_in_progress
	
	


	return



;*****************************************************************************************
;*************************************************************************** Coil Driver *
;*****************************************************************************************

	; Using the current wand phase and coil window, determine whether the coil should
	; be driven or not and update its state.
display_drive_coil

	; Is the coil disabled?
	pagesel	coil_disabled
	btfss	mode_flags, RWAND_MODE_ENABLE_COIL_BIT
	goto	coil_disabled

	; Are we within the window in which the coil driver should be on?
	movf	coil_window_min+1, w	; Test high byte of wand_phase - coil_window_min
	subwf	wand_phase+1, w
	pagesel	coil_off
	btfss	STATUS, C
	goto	coil_off				; C=0, B=1, coil_window_min > wand_phase

	movf	wand_phase+1, w			; Test high byte of coil_window_max - wand_phase
	subwf	coil_window_max+1, w
	pagesel	coil_off
	btfss	STATUS, C
	goto	coil_off				; C=0, B=1, wand_phase > coil_window_max

	movf	coil_window_min+1, w	; Test high byte of wand_phase == coil_window_min
	subwf	wand_phase+1, w
	pagesel	coil_min_neq
	btfss	STATUS, Z
	goto	coil_min_neq			; Not equal, don't need to look at low byte
	movf	coil_window_min, w		; Test low byte of wand_phase - coil_window_min
	subwf	wand_phase, w
	pagesel	coil_off
	btfss	STATUS, C
	goto	coil_off				; C=0, B=1, coil_window_min > wand_phase
coil_min_neq

	movf	wand_phase+1, w			; Test high byte of wand_phase == coil_window_max
	subwf	coil_window_max+1, w
	pagesel	coil_max_neq
	btfss	STATUS, Z
	goto	coil_max_neq			; Not equal, don't need to look at low byte
	movf	wand_phase, w			; Test low byte of coil_window_max - wand_phase
	subwf	coil_window_max, w
	pagesel	coil_off
	btfss	STATUS, C
	goto	coil_off				; C=0, B=1, wand_phase > coil_window_max
coil_max_neq

	banksel	PORTC					; Turn the coil on
	bsf		COIL_DRIVER
	btfsc	mode_flags, RWAND_MODE_COIL_DEBUG_BIT	; In debug mode, turn on the LEDs
	clrf	LED_PORT
	return

coil_off
	movlw	0xFF
	btfsc	mode_flags, RWAND_MODE_COIL_DEBUG_BIT	; In debug mode, turn off the LEDs
	movwf	LED_PORT
	; ...fall through to coil_disabled

coil_disabled
	banksel	PORTC					; Turn the coil off
	bcf		COIL_DRIVER
	return


;*****************************************************************************************
;*********************************************************************** Synchronization *
;*****************************************************************************************

	; Using this poll cycle's delta-t, update our edge_buffer and synchronize our period
	; and phase to what we're detecting on the wand's angle sensor.
display_sync

	; Is synchronization disabled?
	btfss	mode_flags, RWAND_MODE_ENABLE_SYNC_BIT
	return

	; Add our current delta-t to the latest slot on the edge buffer
	banksel	edge_buffer
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
	pagesel	sync_detect
	call	sync_detect

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


	; This uses the current contents of edge_buffer to try to synchronize
	; our angle predictor with the wand's actual angle. edge_buffer records
	; the time (in 16 clock cycle units) between edge detections on the
	; angle sensor. We take advantage of the asymmetry of the angle sensor
	; pulses to uniquely identify the want's period and phase.
sync_detect

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
