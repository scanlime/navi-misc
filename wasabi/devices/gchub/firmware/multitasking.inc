;################################################################################
;
; multitasking.inc - A set of macros that can be used to
;                    construct a simple cooperative multitasking
;                    system.
;
; The Gamecube Hub project
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


#ifndef __MULTITASKING_INC
#define __MULTITASKING_INC


	;; Store a new value to a 2-byte function pointer
fpset	macro	ptr, lbl
	banksel	ptr
	movlw	high lbl
	movwf	ptr
	movlw	low lbl
	movwf	ptr+1
	endm


	;; Goto a label from a function pointer
fpgoto	macro	ptr
	banksel	ptr
	movf	ptr, w
	movwf	PCLATH
	movf	ptr+1, w
	movwf	PCL
	endm


	;; Call a function stored in a function pointer
fpcall	macro	ptr
	local	sub
	local	finished
	pagesel	sub
	call	sub
	pagesel	finished
	goto	finished
sub
	fpgoto	ptr
finished
	endm


	;; Effectively wait until a function pointer is called,
	;; then continue execution. This works by saving the instruction
	;; after this one in the function pointer then returning,
	;; but it can be used to create state machines that look
	;; an awful lot like threads.
fpsleep	macro	ptr
	local	next
	fpset	ptr, next
	return
next
	endm


	;; Sleep on a function pointer until a bit is in a particular state
fpsleep_until macro ptr, reg, bit, state
	local	loop
loop
	fpsleep	ptr
	banksel	reg
	pagesel	loop
	if state
	btfss	reg, bit
	else
	btfsc	reg, bit
	endif
	goto	loop
	endm


#endif ; __MULTITASKING_INC

	;; --- The End ---
