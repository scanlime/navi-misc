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
#include "macros.inc"
#include "../include/rwand_protocol.h"

	errorlevel -302		; supress "register not in bank0, check page bits" message

	global	display_poll
	global	display_init
	global	display_request_flip
	global	display_save_status
	global	display_seq_write_byte
	global	display_seek

	global	edge_buffer
	global	wand_period
	global	wand_phase
	global	edge_counter
	global	coil_window_min
	global	coil_window_max
	global	mode_flags
	global	display_fwd_phase
	global	display_rev_phase
	global	display_column_width
	global	display_gap_width
	global	back_buffer
	global	num_active_columns
	global	flip_counter

	extern	temp
	extern	temp2

bank0	udata

edge_buffer		res	2	; Stores 16-bit duration values at the last 4 angle sensor edges.
						; Units are 16 instruction cycles, or about 2.666us
edge_buffer_1	res	2
edge_buffer_2	res	2
edge_buffer_3	res	2

display_fwd_phase	res	2	; The wand_phase value to start forward display at
display_rev_phase	res 2	; The wand_phase value to start reverse display at
display_column_width res 2	; The wand_phase width of each column of pixels
display_gap_width	res 2	; The wand_phase width of the blank gap between columns

wand_period		res	2	; The wand oscillation period, 16-bit little endian in 16-cycle units
wand_phase		res	2	; The current phase angle of the want, ranging from 0 to wand_period-1

edge_counter	res	1	; A counter that increments every time our synchronization algorithm runs at an
						; edge detected on the angle sensor input. Normally this will increment four times per period.
						; This can let the host know whether the wand is actually moving, whereas the predictor
						; is always running.

flip_counter	res 1	; A counter that increments every time a page flip is completed

num_active_columns	res	1	; Effective width of our display, always <= NUM_COLUMNS

write_pointer	res	1	; The current position in writing to the backbuffer

unbanked	udata_shr

coil_window_min	res	2	; Minimum and maximum wand_phase to enable the coil for
coil_window_max	res	2

current_column		res	1	; The current display column, starting at zero
remaining_col_width	res 2	; Remaining width in this column

mode_flags		res	1	; Holds the mode flags, settable by the USB host. These are defined in rwand_protocol.h
display_flags	res	1
delta_t			res	2	; 16-bit time delta, in cycles, since the last poll
tmr1_prev		res	2	; Previous value of TMR1, for calculating delta_t

#define FLAG_ASENSOR_TEMP 	display_flags, 0	; The previous sample recorded from the angle sensor
#define FLAG_FLIP_REQUEST 	display_flags, 1	; At the beginning of the next display scan, copy the backbuffer
												; to the fontbuffer and clear this flag.
#define FLAG_DISPLAY_FWD	display_flags, 2	; Set if we're currently traversing video memory forward
#define FLAG_DISPLAY_REV	display_flags, 3	; Set if we're currently traversing video memory backwards
#define FLAG_FWD_TRIGGERED	display_flags, 4	; Keep track of whether we've done a forward scan yet this period
#define FLAG_REV_TRIGGERED	display_flags, 5	; Keep track of whether we've done a reverse scan yet this period
#define FLAG_IN_GAP			display_flags, 6	; Set if we're in a gap rather than an actual column

bank1	udata
front_buffer	res	NUM_COLUMNS
bank2	udata
back_buffer		res	NUM_COLUMNS

	code


;*****************************************************************************************
;************************************************************************** Entry Points *
;*****************************************************************************************

	; Initialize important things, don't bother setting parameters
	; that won't matter until the host sets them and turns on the proper flags.
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
	clrf	remaining_col_width
	clrf	remaining_col_width+1
	clrf	tmr1_prev
	clrf	tmr1_prev+1

	clrf	mode_flags
	clrf	display_flags
	clrf	edge_counter
	clrf	flip_counter
	clrf	current_column

	movlw	NUM_COLUMNS
	movwf	num_active_columns

	banksel	write_pointer
	movlw	back_buffer
	movwf	write_pointer

	return


	; Called frequently from the main loop
display_poll

	pagesel	display_keep_time		; Must be done first, so we have valid angle predictions below
	call	display_keep_time

	pagesel	display_led_scan		; Must be done before drive_coil or sync, so they have
	call	display_led_scan		;   minimum impact on display timing.

	pagesel	display_drive_coil
	call	display_drive_coil

	pagesel	display_sync
	call	display_sync
	return


	; Request a page flip at our next opportunity
display_request_flip
	bsf		FLAG_FLIP_REQUEST
	banksel	write_pointer			; Reset the write_pointer
	movlw	back_buffer
	movwf	write_pointer
	return


	; Starting at the location in IRP:FSR, save an 8-byte status
	; packet as described in rwand_protocol.h for RWAND_CTRL_READ_STATUS.
display_save_status
	banksel	wand_period
	movf	wand_period, w		; 1
	movwf	INDF
	incf	FSR, f
	movf	wand_period+1, w	; 2
	movwf	INDF
	incf	FSR, f
	movf	wand_phase, w		; 3
	movwf	INDF
	incf	FSR, f
	movf	wand_phase+1, w		; 4
	movwf	INDF
	incf	FSR, f
	movf	edge_counter, w		; 5
	movwf	INDF
	incf	FSR, f
	movf	mode_flags, w		; 6
	movwf	INDF
	incf	FSR, f
	movf	flip_counter, w		; 7
	movwf	INDF
	incf	FSR, f
	banksel	BUTTON_PORT			; 8
	movf	BUTTON_PORT, w
	movwf	INDF
	return


	; Set our write pointer to the given column number, form 'w'.
display_seek
	addlw	back_buffer
	banksel	write_pointer
	movwf	write_pointer
	return


	; Sequentially write the byte given in 'w' to the backbuffer.
display_seq_write_byte
	movwf	temp
	banksel	write_pointer
	bankisel back_buffer

	movf	back_buffer+NUM_COLUMNS, w	; Test write_pointer - back_buffer[NUM_COLUMNS]
	subwf	write_pointer, w
	btfsc	STATUS, C
	return								; Out of range

	movf	write_pointer, w			; Write and increment
	movwf	FSR
	movf	temp, w
	movwf	INDF
	incf	write_pointer, f
	return


;*****************************************************************************************
;**************************************************************************** Timekeeper *
;*****************************************************************************************

	; Measure the delta-t since the last poll using TMR1, and update our wand predictor.
	; The period and phase of this wand predictor is corrected by the synchronization code.
display_keep_time

	; Capture the current value of TMR1, safe from overflows. This uses the method
	; described in example 12-2 of the PICmicro mid-range reference manual.
	; This procedure should be performed with interrupts off- we have interrupts on,
	; but they shouldn't be in use now and nothing catastrophic will happen if they are.
	banksel	TMR1H
	pagesel	try_tmr1_again
try_tmr1_again
	movf	TMR1H, w			; Read the high byte followed by the low byte
	movwf	temp2
	movf	TMR1L, w
	movwf	temp
	movf	TMR1H, w			; Read the high byte again, in case it rolled over.
	subwf	temp2, w
	btfss	STATUS, Z
	goto	try_tmr1_again		; It rolled over, try again
	
	movf	temp, w				; Copy to delta_t
	movwf	delta_t
	movf	temp2, w
	movwf	delta_t+1

	movf	tmr1_prev, w		; Subtract tmr1_prev from delta_t
	subwf	delta_t, f
	btfss	STATUS, C
	decf	delta_t+1, f
	movf	tmr1_prev+1, w
	subwf	delta_t+1, f

	movf	temp, w				; Copy to tmr1_prev
	movwf	tmr1_prev
	movf	temp2, w
	movwf	tmr1_prev+1

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

	; Detect wand stalls by comparing the latest edge buffer entry with the
	; current predictor period. If we think the wand's stalled, shut off the
	; LEDs and coil driver.
	pagesel	no_stall
	btfss	mode_flags, RWAND_MODE_STALL_DETECT_BIT
	goto	no_stall
	jl_16	edge_buffer_3, wand_period, no_stall
	bcf		mode_flags, RWAND_MODE_ENABLE_DISPLAY_BIT
	bcf		mode_flags, RWAND_MODE_ENABLE_COIL_BIT
no_stall

	; If wand_phase >= wand_period, subtract wand_period
	jl_16	wand_phase, wand_period, no_phase_rollover

	movf	wand_period, w
	subwf	wand_phase, f
	btfss	STATUS, C
	decf	wand_phase+1, f
	movf	wand_period+1, w
	subwf	wand_phase+1, f

	bcf		FLAG_FWD_TRIGGERED	; Clear status flags used last period
	bcf		FLAG_REV_TRIGGERED
	bcf		FLAG_DISPLAY_FWD
	bcf		FLAG_DISPLAY_REV
no_phase_rollover

	return




;*****************************************************************************************
;************************************************************************** LED Scanning *
;*****************************************************************************************

	; Update the state of our LED column scanning, and update the
	; LED_PORT from video memory.
display_led_scan

	; Check for the beginning of a forward scan
	pagesel	no_forward_scan
	btfsc	FLAG_FWD_TRIGGERED		; If we've already started, don't bother
	goto	no_forward_scan
	banksel	wand_phase
	jl_16	wand_phase, display_fwd_phase, no_forward_scan

	pagesel	start_scan_common			; Do the things common to both types of scans
	call	start_scan_common
	bsf		FLAG_DISPLAY_FWD			; Set flags
	bcf		FLAG_DISPLAY_REV
	bsf		FLAG_FWD_TRIGGERED
	banksel	current_column
	clrf	current_column				; Start at the beginning
no_forward_scan

	; Check for the beginning of a reverse scan
	pagesel	no_reverse_scan
	btfsc	FLAG_REV_TRIGGERED		; If we've already started, don't bother
	goto	no_reverse_scan
	banksel	wand_phase
	jl_16	wand_phase, display_rev_phase, no_reverse_scan

	pagesel	start_scan_common			; Do the things common to both types of scans
	call	start_scan_common
	bsf		FLAG_DISPLAY_REV			; Set flags
	bcf		FLAG_DISPLAY_FWD
	bsf		FLAG_REV_TRIGGERED
	decf	num_active_columns, w		; Start at the end (num_active_columns - 1)
	banksel	current_column
	movwf	current_column
no_reverse_scan

	; Bounds check the current column. If it's out of range, disable current scans
	movf	num_active_columns, w		; Test current_column - num_active_columns
	subwf	current_column, w
	pagesel	led_scan_disabled
	btfsc	STATUS, C
	goto	led_scan_disabled			; C=1, B=0, num_active_columns <= current_column

	; Is there any scan in progress? If not...
	pagesel	scan_in_progress
	btfsc	FLAG_DISPLAY_FWD
	goto	scan_in_progress
	btfsc	FLAG_DISPLAY_REV
	goto	scan_in_progress

	; Blank the LEDs, clear the scan bits, and return.
	; This is invoked at the end of a scan, (once current_column is out of bounds)
	; or if no scan has been started.
led_scan_disabled
	movlw	0xFF
	banksel	LED_PORT
	movwf	LED_PORT
	bcf		FLAG_DISPLAY_FWD
	bcf		FLAG_DISPLAY_REV
	return

	; We have a scan (forward or reverse) in progress
scan_in_progress
	
	bankisel front_buffer				; Point IRP:FSR at the front buffer's current column
	banksel	LED_PORT
	movlw	front_buffer
	addwf	current_column, w
	movwf	FSR
	comf	INDF, w						; Invert (the LEDs are active-low) and blast to LED_PORT.
	btfss	mode_flags, RWAND_MODE_ENABLE_DISPLAY_BIT 	; If the display has been disabled, blank the LEDs
	movlw	0xFF
	btfsc	FLAG_IN_GAP					; If we're in a gap, blank the LEDs
	movlw	0xFF
	movwf	LED_PORT					; It's just as fast to invert it while moving as to not invert it,
										; so we might as well do that here.

	; Subtract our delta-t from the current column width
	banksel	remaining_col_width
	movf	delta_t, w					; Subtract the low byte
	subwf	remaining_col_width, f
	btfss	STATUS, C
	decf	remaining_col_width+1, f
	movf	delta_t+1, w				; Subtract the high byte
	subwf	remaining_col_width+1, f

	; Skip columns until remaining_col_width is positive, up to a maximum of NUM_COLUMNS*2
	; (to prevent infinite looping of display_column_width is zero)
	movlw	NUM_COLUMNS*2
	movwf	temp
next_column_loop
	btfss	remaining_col_width+1, 7
	return
	
	; Are we skipping to the next column, or to the gap after this column?
	pagesel	not_in_gap
	btfss	FLAG_IN_GAP
	goto	not_in_gap

in_gap
	bcf		FLAG_IN_GAP					; Exit this gap

	movf	display_column_width, w		; Give the next column all its allocated width.
	addwf	remaining_col_width, f		; Note that this carries over the error from the last column
	btfsc	STATUS, C
	incf	remaining_col_width+1, f
	movf	display_column_width+1, w
	addwf	remaining_col_width+1, f

	pagesel	next_column_loop	
	decfsz	temp, f
	return								; We're giving up

not_in_gap
	bsf		FLAG_IN_GAP					; Enter a gap

	btfsc	FLAG_DISPLAY_FWD			; Increment/decrement the current column
	incf	current_column, f
	btfsc	FLAG_DISPLAY_REV
	decf	current_column, f

	movf	display_gap_width, w		; Give this next gap all its allocated width.
	addwf	remaining_col_width, f		; Note that this carries over the error from the last gap
	btfsc	STATUS, C
	incf	remaining_col_width+1, f
	movf	display_gap_width+1, w
	addwf	remaining_col_width+1, f

	pagesel	next_column_loop	
	decfsz	temp, f
	return								; We're giving up


	; Do the things common to starting forward and reverse scans. This
	; currently includes allocating width for the first column, and doing a back->front
	; blit if we need one.
	;
	; NOTE: This uses current_column for a blit if it needs to, so this must be called
	;       before that is initialized.
start_scan_common
	movf	display_column_width, w		; Give this column all its allocated width.
	movwf	remaining_col_width			; We add the current delta_t, since it will be subtracted later.
	movf	display_column_width+1, w
	movwf	remaining_col_width+1
	movf	delta_t, w
	addwf	remaining_col_width, f
	btfsc	STATUS, C
	incf	remaining_col_width+1, f
	movf	delta_t+1, w
	addwf	remaining_col_width+1, f

	bcf		FLAG_IN_GAP					; Start in a column, not a gap

	btfss	FLAG_FLIP_REQUEST			; We're done if we haven't had a page flip request
	return
	bcf		FLAG_FLIP_REQUEST
	incf	flip_counter, f

	clrf	current_column				; Start the blit at the first column
	pagesel	flip_blit_loop
flip_blit_loop
	bankisel back_buffer				; Point IRP:FSR at the back buffer's current column
	movlw	back_buffer
	addwf	current_column, w
	movwf	FSR
	movf	INDF, w						; Copy to temp
	movwf	temp
	bankisel front_buffer				; Point IRP:FSR at the front buffer's current column
	movlw	front_buffer
	addwf	current_column, w
	movwf	FSR
	movf	temp, w						; Copy from temp
	movwf	INDF
	incf	current_column, f			; Next column...
	movf	current_column, w			; Are we done yet?
	sublw	NUM_COLUMNS
	btfss	STATUS, Z
	goto	flip_blit_loop

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
	banksel	coil_window_min

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

	; Attempt to synchronize using our current edge_buffer data, if synchronization is enabled
	pagesel	sync_detect
	btfsc	mode_flags, RWAND_MODE_ENABLE_SYNC_BIT
	call	sync_detect

	banksel	edge_buffer
	incf	edge_counter, f		; Count this edge
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
