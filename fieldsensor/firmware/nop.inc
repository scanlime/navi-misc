;
; Macros for creating large numbers of no-ops, for precice timing using calculated jumps
;

nop2	macro
	nop
	nop
	endm

nop4	macro
	nop2
	nop2
	endm

nop8	macro
	nop4
	nop4
	endm

nop16	macro
	nop8
	nop8
	endm

nop32	macro
	nop16
	nop16
	endm

nop64	macro
	nop32
	nop32
	endm

nop128	macro
	nop64
	nop64
	endm

nop256 macro
	nop128
	nop128
	endm
