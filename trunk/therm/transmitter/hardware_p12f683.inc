	;; The PIC12F675 has 8 pins, has an 8MHz internal oscillator, and
	;; doesn't need any explicit oscillator calibration procedure.
	;; We use it with its default divider settings, which runs it at 4MHz
	;; for compatibility with our existing code and lower power.

	errorlevel -302

list            p=12f683
#include        p12f683.inc
#define         CLOCK_MHZ    4
                __CONFIG _FCMEN_OFF & _IESO_OFF & _BOD_ON & _CPD_OFF & _CP_OFF & _MCLRE_OFF & _PWRTE_ON & _WDT_ON & _INTRC_OSC_NOCLKOUT

calibrate_oscillator	macro
	endm

	;; The End
