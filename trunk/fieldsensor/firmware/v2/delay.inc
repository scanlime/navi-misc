 list;ignore
 ;---------------------------------------------------------------------
 ; CONTROL:
 ;	file		delay.inc
 ;	author		wouter van ooijen <wf@xs4all.nl>
 ; 	version		1.0
 ; 	date		05-09-1997
 ; 	language	MPASM assembler for Microchip PIC processors
 ;      summary         various delay macros and subroutines
 ;      info            http://www.xs4all.nl/~wf/wouter/pic/wloader
 ;---------------------------------------------------------------------
 nolist
 ; Copyright (C) 2000 Wouter van Ooijen
 ;
 ; This application is free software; you can redistribute it and/or
 ; modify it under the terms of the GNU Library General Public
 ; License as published by the Free Software Foundation; either
 ; version 2 of the License, or (at your option) any later version.
 ;
 ; This library is distributed in the hope that it will be useful,
 ; but WITHOUT ANY WARRANTY; without even the implied warranty of
 ; MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 ; Library General Public License for more details.
 ; 
 ; You should have received a copy of the GNU Library General Public
 ; License along with this library; if not, write to the
 ; Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 ; Boston, MA  02111-1307, USA.
 ;---------------------------------------------------------------------
 ;listing -1

;
; DESCRIPTION:
; delay_cycles n	; delay of n cycles
; delay_4w_cycles	; delay of 4 * w cycles
; delay_microseconds n 	; delay of n microseconds
; delay_8w_microseconds	; delay of 4 * w microseconds
; delay_milliseconds n	; delay of n milliseconds
; delay_w_milliseconds	; delay of w milliseconds
; delay_seconds n	; delay of n seconds
; delay_w_seconds	; delay of w seconds
; 
; REQUIRES:
; misc.inc	must have been included
; xtal		clock frequency in Hz 
;
; RESOURCES (once):
; code		29
; file bytes	2
;

;
; SUMMARY:
; 	Delay macros for up to 6 cycles.
;
; DESCRIPTION:
; 	Inserting delay_cycles_#v(n) will give a delay of exactly n cycles.
; 	These macros should not be used directly. The delay_cycles macro
; 	provides a convenient front-end. Flags and w are preserved.
;
; RESOURCES (each use):
;	code		0 (for n==0)
;			1 (for n==1,2,4) 
;			2 (for n==3, 5 <= n <= 1285)
;	cycles		n
;	stack		0
; RESOURCES (once):
;	none
;
	#define delay_cycles_0
	#define delay_cycles_1 nop
	#define delay_cycles_2 goto $+1
	#define delay_cycles_4 call delay_return
delay_cycles_3 macro
 ;listing +100
 delay_cycles_1
 delay_cycles_2
 nolist
 ;listing -100
 endm
delay_cycles_5 macro
 ;listing +100
 delay_cycles_1
 delay_cycles_4
 nolist
 ;listing -100
 endm
delay_cycles_6 macro
 ;listing +100
 delay_cycles_2
 delay_cycles_4
 nolist
 ;listing -100
 endm


;
; SUMMARY:
;	delay subroutine for delays up to 1285 cycles.
;
; DESCRIPTION:
;	A call to delay_#v(n)_5w_c gives a delay of exactly 5 * w + n 
;	cycles (including call and return). The maximum delay (w == 255, 
;	n == 10) is 1285 cycles. This routine should not be used directly. 
;	The delay_cycles macro provides a convenient front-end. Flags
;	and w are not preserved.
;
; RESOURCES (each use):
;	code		1
;	cycles		5 * w + n
;	stack		1
; RESOURCES (once):
;	code		8
;	file bytes	0 
;
 ;listing +100
delay_10_5w_c
 nop
delay_9_5w_c	
 nop
delay_8_5w_c	
 nop
delay_7_5w_c	
 nop
delay_6_5w_c	
 addlw -1
 skpc
delay_return		
 return
 goto delay_6_5w_c
 nolist
 ;listing -100


;
; SUMMARY:
;	macro for delays of exactly n cycles
;
; DESCRIPTION:
;	This macro inserts a delay of exactly n cycles. It is a 
;	front-end for the delay_cycles_#(n) macros and the 
;	delay_5_#(n)w_c routine. For n > 5 w and the flags are not
;	preserved.
;
; RESOURCES (each use):
;	code		0 (for n==0)
;			1 (for n==1,2,4) 
;			2 (for n==3, 5 <= n <= 1285)
;			2 * ((n + 1284)/1285) (for n > 1285)
;	cycles		n
;	stack		0 (for n==1,2,3)
;			1 (for n==4, 5 >= n)
; RESOURCES (once):
;	code		8 (for delay_5_#(n)w_c)
;	file 		0
;
delay_cycles_max  equ ((D'5' * D'255') + D'10')
delay_cycles macro n
 variable i
i = 0
 while n > ( i + 1 ) * delay_cycles_max
  movlw D'255'
  call delay_10_5w_c
i = i + 1
 endw
 local nn = n - i * delay_cycles_max

 if n <= 6
  delay_cycles_#v(nn)
  exitm
 endif

 local delay_w_value = ((nn - 6 ) / 5)
 ;listing +100
 movlw delay_w_value
 call delay_#v(nn - (5 * delay_w_value))_5w_c
 nolist
 ;listing -100
 endm


;
; SUMMARY:
;	subroutine for a delay of 4 * w cycles
;
; DESCRIPTION:
;	A call to this subroutine gives a delay of 4 * w + 3 cycles,
;	including call and return. w == 0 is interpreted as w == 256.
;	On exit w == 0, the zero flag is set and the acrry flag is clear.
;
; RESOURCES (each use):
;	code		1
;	cycles		4 * w + 3
;	stack		1
; RESOURCES (once):
;	code		4
;	file 		0
;
delay_4w_cycles
 ;listing +100
 addlw 0 - 1
 skpnz
  return
 goto delay_4w_cycles
 nolist
 ;listing -100


;	
; SUMMARY:
;	macro for a delay of n microseconds
;
; DESCRIPTION:
;	This macro provides a delay of exactly n microseconds. It is a 
;	simple front-end to delay_cycles.
;
; RESOURCES (each use):
;	code		depends on n and xtal (see delay_cycles)
;	cycles		n * (x
;	stack		0 or 1
; RESOURCES (once):
;	code		8 (for delay_5_#(n)w_c)
;	file 		0
;
delay_microseconds macro n
 local cycles = ( ( ( xtal / D'1000000' ) * n ) / 4 )
 delay_cycles cycles
 endm


;
; SUMMARY:
;	subroutine for a delay of w microseconds
;
; DESCRIPTION:
;	A call to this subroutine gives a delay of w microseconds.
;	The delay is accurate within 4 cycles for xtal == 1, 4 or 10 MHz.
;
; RESOURCES (each use):
;	code		1
;	cycles		w
;	stack		1
; RESOURCES (once):
;	code		4
;	file 		0
;
delay_8w_microseconds
 ;listing +100
 addlw 0 - (xtal / (D'2000000'))
 skpnc
  goto delay_8w_microseconds
 return
 nolist
 ;listing -100


;	
;SUMMARY:
;	subroutine for a delay of w milliseconds
;
; DESCRIPTION:
;	A call to this subroutine provides a delay of w milliseconds.
;
; RESOURCES (each use):
;	code		1
;	cycles		
;	stack		2
; RESOURCES (once):
;	code		6  (for xtal =< 4MHz)
;			8  (for xtal == 10MHz)
;			10 (for xtal == 16MHz)
;	file 		1
;
 ;listing +100
 cblock
  delay_milliseconds_counter
 endc
delay_1000_milliseconds
 call delay_500_milliseconds
delay_500_milliseconds
 call delay_250_milliseconds
delay_250_milliseconds
 movlw 0
delay_w_milliseconds
 movwf delay_milliseconds_counter
delay_milliseconds_loop
 nolist
 ;listing -100
 delay_microseconds D'1000'
 ;listing +100
 decfsz delay_milliseconds_counter, 1
  goto delay_milliseconds_loop
 return
 nolist
 ;listing -100


;	
; SUMMARY:
;	macro for a delay of n milliseconds
;
; DESCRIPTION:
;	This macro provides a delay of n milliseconds. It is a simple
;	front-end to the delay_w_milliseconds subroutine.
;
; RESOURCES (each use):
;	code		2
;	cycles		
;	stack		2
;	RESOURCES (once):
;	code		x (see )
;	file 		1 (see )
;
delay_milliseconds macro n
 local milliseconds = n
 while milliseconds >= D'1000'
  ;listing +100
  call delay_1000_milliseconds
  nolist
  ;listing -100
milliseconds	-= D'1000'
 endw
 if milliseconds >= D'500'
  ;listing +100
  call delay_500_milliseconds
  nolist
  ;listing -100
milliseconds	-= D'500'
 endif
 if milliseconds >= D'250'
  ;listing +100
  call delay_250_milliseconds
  nolist
  ;listing -100
milliseconds	-= D'250'
 endif
 if milliseconds > 0
  ;listing +100
  movlw milliseconds
  call delay_w_milliseconds
  nolist
  ;listing -100
 endif
 endm


;
; SUMMARY:
;	subroutine for a delay of w seconds
;
; DESCRIPTION:
;	A call to this subroutine gives a delay of w seconds.
;
; RESOURCES (each use):
;	code		1
;	cycles		
;	stack		9
; RESOURCES (once):
;	code		7
;	file 		1
;
 ;listing +100
 cblock
  delay_seconds_counter
 endc
delay_w_seconds
 movwf delay_seconds_counter
delay_seconds_loop
 nolist
 ;listing -100
 delay_milliseconds D'1000'
 ;listing +100
 decfsz delay_seconds_counter, 1
  goto delay_seconds_loop
 return
 nolist
 ;listing -100


;	
; SUMMARY:
;	macro for a delay of n seconds
;
; DESCRIPTION:
;	This macro provides a delay of n seconds. It is a simple
;	front-end to the delay_w_seconds subroutine.
;
; RESOURCES (each use):
;	code		2
;	cycles		
;	stack		2
; RESOURCES (once):
;	code		x (see )
;	file 		1 (see )
;
delay_seconds macro n
 ;listing +100
 movlw n
 call delay_w_seconds
 nolist
 ;listing -100
 endm

 ;listing +1
