;################################################################################
;
; sensor.asm - Field sensor sampling. This consists of exciting an LC resonator
;              for a few periods then taking one A/D converter reading. The period
;              of excitation, number of excitation cycles, phase of the A/D reading,
;              and antenna pair are all inputs to the sampling operation.
;
;              This is done in 5 steps:
;                1. A/D and LC tank parameters are set up
;                2. A calculated jump into page_1 is performed to get a precise excitation period
;                3. In page_2, the LC tank outputs are toggled, jumping back to step 2 as necessary
;                4. A calculated jump into page_3 is made to get a precise sampling phase
;                5. In page_4, the A/D converter is sampled and a reading is returned
;
; The USB Electric Field Sensor project
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
#include "nop.inc"
#include "../include/efs_protocol.h"

	errorlevel -302			; Disable register page warnings

	global	sensor_sampler

;************************************************** Variables

	; These variables must be in the same bank as PORTB (bank0 or bank2)
bank0	udata
	
	; Cached parameter values
lc_port_xor		res	1
period			res	1
phase			res 1
period_counter	res 1
accumulator_num	res 1
lc_port_buffer  res 1

;************************************************** Step 1

	; This is our entry point, it can occur in any code page.
	; A parameter block as described in efs_protocol.h must be
	; pointed to by IRP:FSR.
	code
sensor_sampler

	movf	INDF, w			; 0. EFS_PARAM_ACCUMULATOR_NUM
	banksel	accumulator_num
	movwf	accumulator_num
	incf	FSR, f
	
	movf	INDF, w			; 1. EFS_PARAM_LC_PORT_XOR
	movwf	lc_port_xor
	incf	FSR, f

	movf	INDF, w			; 2. EFS_PARAM_ADCON_INIT
	banksel	ADCON0
	movwf	ADCON0
	incf	FSR, f
	bsf		ADCON0, ADON	; (Make sure the ADC is at least turned on)

	movf	INDF, w			; 3. EFS_PARAM_PERIOD
	banksel	period
	movwf	period
	incf	FSR, f

	movf	INDF, w			; 4. EFS_PARAM_PHASE
	movwf	phase
	incf	FSR, f

	movf	INDF, w			; 5. EFS_PARAM_NUM_HALF_PERIODS
	movwf	period_counter
	incf	FSR, f

	movf	INDF, w			; 6. EFS_PARAM_LC_TRIS_INIT
	banksel	TRISB
	movwf	TRISB
	incf	FSR, f

	movf	INDF, w			; 7. EFS_PARAM_LC_PORT_INIT
	banksel	lc_port_buffer			; bank 0 (PORTB and all our variables)
	movwf	PORTB
	movwf	lc_port_buffer
	incf	FSR, f

;************************************************** Step 2

	; Now that we've set everything up, take our first jump into
	; the period table. The timing isn't going to be quite the same
	; as on the future periods, but it will be close since PORT_INIT was
	; the last parameter we initialized. The first period shouldn't have
	; to be exactly timed, as the LC tank(s) are just getting warmed up.
	movlw	0x01
	movwf	PCLATH
	movf	period, w
	movwf	PCL
	
	; Page 0x01 is full of no-ops, as the period jump table.
	; Calculated jumps end up somewhere in this table, and the amount
	; of time it takes to roll over into page 0x02 can be precisely controlled.
page_1	code
	nop256

;************************************************** Step 3

page_2 code

	; Toggle the LC tank outputs via a buffer register.
	; This is important, since reading from an I/O port
	; gives you the current inputs rather than the output
	; driver state. If the output is changing state slowly
	; (as it will be when we first try to excite the LC tank)
	; we'll end up reading the wrong state.
	movf	lc_port_xor, w
	xorwf	lc_port_buffer, f
	movf	lc_port_buffer, w
	movwf	PORTB

	; Loop back to the period jumptable if we still have
	; more excitation periods to run.
	movlw	0x01
	movwf	PCLATH
	movf	period, w
	decfsz	period_counter, f
	movwf	PCL

;************************************************** Step 4

	; Calculated jump into the phase table
	movlw	0x03
	movwf	PCLATH
	movf	phase, w
	movwf	PCL

page_3	code
	nop256

;************************************************** Step 5

page_4 code

	; Start the ADC and wait for it to finish conversion
	banksel ADCON0
	bsf		ADCON0, GO
	pagesel	adFinishLoop
adFinishLoop
	btfsc	ADCON0, NOT_DONE
	goto	adFinishLoop

	; Get the LC tanks to discharge as fast as possible by
	; tristating PORTB- this disconnects them from everything
	; except the antennas.
	movlw	0xFF
	banksel	TRISB
	movwf	TRISB	

	; Return the ADC's result
	banksel ADRES
	movf	ADRES, w
	return

	end

;### The End ###
