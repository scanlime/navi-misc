;################################################################################
;
; sensor.asm - Field sensor sampling. This consists of exciting an LC resonator
;              for a few periods then taking one A/D converter reading. The period
;              of excitation, number of excitation cycles, phase of the A/D reading,
;              and antenna pair are all inputs to the sampling operation.
;
;              This is done in 5 steps:
;                1. A/D and LC tank parameters are set up
;                2. A calculated jump into page_18 is performed to get a precise excitation period
;                3. In page_19, the LC tank outputs are toggled, jumping back to step 2 as necessary
;                4. A calculated jump into page_1A is made to get a precise sampling phase
;                5. In page_1B, the A/D converter is sampled and a reading is returned
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
	banksel	ADCON1
	movwf	ADCON1
	incf	FSR, f

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
	banksel	period			; bank 0 (PORTB and all our variables)
	movwf	PORTB
	incf	FSR, f

;************************************************** Step 2

	; Now that we've set everything up, take our first jump into
	; the period table. The timing isn't going to be quite the same
	; as on the future periods, but it will be close since PORT_INIT was
	; the last parameter we initialized. The first period shouldn't have
	; to be exactly timed, as the LC tank(s) are just getting warmed up.
	movlw	0x18
	movwf	PCLATH
	movf	period, w
	movwf	PCL
	
	; Page 0x18 is full of no-ops, as the period jump table.
	; Calculated jumps end up somewhere in this table, and the amount
	; of time it takes to roll over into page 0x19 can be precisely controlled.
page_18	code
	nop256

;************************************************** Step 3

page_19 code

	; Toggle the LC tank outputs
	movf	lc_port_xor, w
	xorwf	PORTB, f

	; Loop back to the period jumptable if we still have
	; more excitation periods to run.
	movlw	0x18
	movwf	PCLATH
	movf	period, w
	decfsz	period_counter, f
	movwf	PCL

;************************************************** Step 4

	; Calculated jump into the phase table
	movlw	0x1A
	movwf	PCLATH
	movf	phase, w
	movwf	PCL

page_1A	code
	nop256

;************************************************** Step 5

page_1B code
	
	; Start the ADC and wait for it to finish conversion
	bsf		ADCON0, GO
adFinishLoop
	btfsc	ADCON0, NOT_DONE
	goto	adFinishLoop

	movf	ADRES, w
	return

	end

;### The End ###
