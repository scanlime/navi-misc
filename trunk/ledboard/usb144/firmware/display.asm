;################################################################################
;
; display.asm - Implements the LED matrix driver itself
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
#include "macros.inc"
#include "../include/ledboard_protocol.h"

	errorlevel -302		; supress "register not in bank0, check page bits" message
	errorlevel -306		; ..and the page boundary warning

	global	display_poll
	global	display_init
	global	display_request_flip
	global	display_poll_flip
	global	display_seq_write_byte
	global	display_seek

	global	back_fsr
	global	back_status
	global	scan_rate
	global	pwm_cycles
	global	pwm_table

	extern	temp
	extern	temp2

	;; Hardware definitions
	#define ROW_DRIVE_0	TRISA, 1
	#define ROW_DRIVE_1	TRISA, 2
	#define ROW_DRIVE_2	TRISA, 3
	#define ROW_DRIVE_3	TRISA, 4
	#define ROW_DRIVE_4	TRISA, 5
	#define ROW_DRIVE_5	TRISE, 0
	#define ROW_DRIVE_6	TRISE, 1
	#define ROW_DRIVE_7	TRISE, 2
	#define ROW_DRIVE_8	TRISC, 0
	#define COLUMNS_LOW	PORTD
	#define COLUMNS_HIGH	PORTB


bank0	udata

write_pointer	res	1	; The current position in writing to the backbuffer
scan_state	res	2	; Function pointer for the scanner thread's state

scan_rate	res	2	; 16-bit big-endian TMR1 adjustment used to control scanning speed.
				; Higher values make it go faster, improving refresh rate
				; but possibly increasing flicker due to USB activity.

pwm_cycles	res	1	; Number of scan cycles used for PWM, sets our PWM resolution

pwm_table	res	0x10	; Table mapping LED brightness codes to PWM values

pwm_row		res	0x10	; PWM values for the current LED row

unbanked udata_shr

display_flags	res	1
pwm_iter	res	1	; Iterates from 0 up to pwm_cycles, inclusive

front_fsr	 res	1	; FSR pointing to the current front buffer
front_status	 res	1	; STATUS pointing to the current front buffer
back_fsr	 res	1	; FSR pointing to the current back buffer
back_status	 res	1	; STATUS pointing to the current back buffer

flip_counter	res	1	; Count number of page flips for display_poll_flip
	
#define FLAG_FLIP_REQUEST	display_flags, 0

bank1	udata
vram_buffer_1	res	LEDBOARD_VRAM_SIZE

bank2	udata
vram_buffer_2	res	LEDBOARD_VRAM_SIZE

prog3	code


;*****************************************************************************************
;************************************************************************** Entry Points *
;*****************************************************************************************

display_init
	fpset	scan_state, scanner_thread

	clrf	flip_counter
	
	;; Clear all video buffers
	bzero	vram_buffer_1, LEDBOARD_VRAM_SIZE
	bzero	vram_buffer_2, LEDBOARD_VRAM_SIZE
	bzero	pwm_table, 0x10

	bankisel vram_buffer_1	; Start the front buffer out at vram_buffer_1
	movf	STATUS, w
	movwf	front_status
	movlw	vram_buffer_1
	movwf	front_fsr

	bankisel vram_buffer_2	; ...and the back buffer at vram_buffer_2
	movf	STATUS, w
	movwf	back_status
	movlw	vram_buffer_2
	movwf	back_fsr

	banksel	scan_rate
	movlw	0xFD		; Default scan rate is 0xFD00
	movwf	scan_rate
	clrf	scan_rate+1
	clrf	display_flags	; Clear all display flags
	movlw	0x10		; 16 PWM cycles by default
	movwf	pwm_cycles

	banksel	T1CON		; Start TMR1 running off the instruction clock
	movlw	0x01
	movwf	T1CON
	return


	;; Called frequently from the main loop. It runs our scanning thread a bit
	;; every time TMR1 overflows, adjusting it according to our scan_rate parameter.
display_poll
	banksel	PIR1		; Go away if TMR1 hasn't done anything interesting
	btfss	PIR1, TMR1IF
	return
	bcf	PIR1, TMR1IF

	movf	scan_rate+1, w	; Add our scan_rate adjustment
	addwf	TMR1L, f
	btfsc	STATUS, C
	incf	TMR1H, f
	movf	scan_rate, w
	addwf	TMR1H, f

	fpgoto	scan_state	; Jump to our scanner thread


	;; Request an asynchronous page flip. Has the side effect of resetting
	;; the write pointer.
display_request_flip
	bsf	FLAG_FLIP_REQUEST
	banksel	write_pointer			; Reset the write_pointer
	clrf	write_pointer
	return

	
	;; Return in 'w' the number of page flips that have completed successfully
	;; since the last call to this function.
display_poll_flip
	movf	flip_counter, w
	clrf	flip_counter
	return


	;; Set our write pointer to the given column number, form 'w'.
display_seek
	banksel	write_pointer
	movwf	write_pointer
	return


	; Sequentially write the byte given in 'w' to the backbuffer.
display_seq_write_byte
	movwf	temp
	banksel	write_pointer
	movf	back_status, w
	movwf	STATUS

	movlw	LEDBOARD_VRAM_SIZE	; Test write_pointer - LEDBOARD_VRAM_SIZE
	subwf	write_pointer, w
	btfsc	STATUS, C
	return				; C=1, B=0, LEDBOARD_VRAM_SIZE <= write_pointer

	movf	write_pointer, w	; Write and increment
	addwf	back_fsr, w
	movwf	FSR
	movf	temp, w
	movwf	INDF
	incf	write_pointer, f
	return


	code

;*****************************************************************************************
;*********************************************************************** Scanning Thread *
;*****************************************************************************************

	;; Prepare the pwm_row entry for one column
prepare_pwm_row macro address, column_number, high_nybble
	movf	front_status, w	; Get this LED's nybble
	movwf	STATUS
	movf	front_fsr, w
	addlw	address
	movwf	FSR

	if high_nybble
	swapf	INDF, w
	else
	movf	INDF, w
	endif

	andlw	0x0F		; Index into pwm_table
	addlw	pwm_table
	bankisel pwm_table
	movwf	FSR
	movf	INDF, w		; Store the result in this LED's pwm_row byte
	banksel	pwm_row
	movwf	pwm_row + column_number
	endm


	;; Compare one column against the PWM iterator and right-shift an
	;; inverted bit for its column driver into 'temp'.
	;; Everything in here stays in bank0.
shift_pwm_column macro column_number
	movf	pwm_row + column_number, w
	subwf	pwm_iter, w	; This borrows if our PWM value > pwm_iter.
	rrf	temp, f		; We want the LED to be high if it borrows, so when C=0.
				; We get the invert for free below, when we use comf.
	endm


	;; Output all PWM values for the current row
generate_row_pwm
	banksel	pwm_row
	shift_pwm_column 0x00
	shift_pwm_column 0x01
	shift_pwm_column 0x02
	shift_pwm_column 0x03
	shift_pwm_column 0x04
	shift_pwm_column 0x05
	shift_pwm_column 0x06
	shift_pwm_column 0x07
	comf	temp, w
	movwf	COLUMNS_LOW

	shift_pwm_column 0x08
	shift_pwm_column 0x09
	shift_pwm_column 0x0A
	shift_pwm_column 0x0B
	shift_pwm_column 0x0C
	shift_pwm_column 0x0D
	shift_pwm_column 0x0E
	shift_pwm_column 0x0F
	comf	temp, w
	movwf	COLUMNS_HIGH
	return


	;; Macro to scan one row of our display
scan_row macro prev_port, prev_bit, current_port, current_bit, address
	local	pwm_cycle_loop
	local	pwm_row_done

	;; Prepare the pwm_row buffer for the current row.
	;; We do this before changing row drivers, to reduce the time
	;; when all LEDs are off
	prepare_pwm_row address + 0x00, 0x00, 1
	prepare_pwm_row address + 0x00, 0x01, 0
	prepare_pwm_row address + 0x01, 0x02, 1
	prepare_pwm_row address + 0x01, 0x03, 0
	prepare_pwm_row address + 0x02, 0x04, 1
	prepare_pwm_row address + 0x02, 0x05, 0
	prepare_pwm_row address + 0x03, 0x06, 1
	prepare_pwm_row address + 0x03, 0x07, 0
	prepare_pwm_row address + 0x04, 0x08, 1
	prepare_pwm_row address + 0x04, 0x09, 0
	prepare_pwm_row address + 0x05, 0x0A, 1
	prepare_pwm_row address + 0x05, 0x0B, 0
	prepare_pwm_row address + 0x06, 0x0C, 1
	prepare_pwm_row address + 0x06, 0x0D, 0
	prepare_pwm_row address + 0x07, 0x0E, 1
	prepare_pwm_row address + 0x07, 0x0F, 0

	;; Turn off the last row driver, clear the column drivers, then turn on the
	;; current row. This sequence is necessary to keep the rows isolated.
	banksel	prev_port
	bsf	prev_port, prev_bit
	banksel	COLUMNS_LOW
	clrf	COLUMNS_LOW
	clrf	COLUMNS_HIGH
	banksel	current_port
	bcf	current_port, current_bit

	;; Loop over each PWM cycle...
	banksel	pwm_iter
	clrf	pwm_iter
pwm_cycle_loop

	pscall	generate_row_pwm
	fpsleep	scan_state

	banksel	pwm_iter	; Are we there yet?
	movf	pwm_cycles, w
	xorwf	pwm_iter, w
	pagesel	pwm_row_done
	btfsc	STATUS, Z
	goto	pwm_row_done

	incf	pwm_iter, f
	psgoto	pwm_cycle_loop

pwm_row_done
	endm


	;; Page flip handler. This swaps the front and back
	;; pointers, and increments the flip counter.
page_flip
	bcf	FLAG_FLIP_REQUEST
	swapff	front_fsr, back_fsr
	swapff	front_status, back_status
	incf	flip_counter, f
	return


	;; This is a cooperatively-multitasked thread that scans through the LED matrix.
	;; It's invoked by display_poll each time TMR1 rolls over, and runs until the
	;; next fpsleep.
scanner_thread

	scan_row ROW_DRIVE_8, ROW_DRIVE_0, 0x00
	scan_row ROW_DRIVE_0, ROW_DRIVE_1, 0x08
	scan_row ROW_DRIVE_1, ROW_DRIVE_2, 0x10
	scan_row ROW_DRIVE_2, ROW_DRIVE_3, 0x18

	;; This gets big, switch banks...
	psgoto	scanner_second_half
prog4 code
scanner_second_half

	scan_row ROW_DRIVE_3, ROW_DRIVE_4, 0x20
	scan_row ROW_DRIVE_4, ROW_DRIVE_5, 0x28
	scan_row ROW_DRIVE_5, ROW_DRIVE_6, 0x30
	scan_row ROW_DRIVE_6, ROW_DRIVE_7, 0x38
	scan_row ROW_DRIVE_7, ROW_DRIVE_8, 0x40

	clrwdt			; Pat the watchdog only after a successful scan

	pagesel	page_flip
	btfsc	FLAG_FLIP_REQUEST
	call	page_flip

	psgoto	scanner_thread


	end

;### The End ###
