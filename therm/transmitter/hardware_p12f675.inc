	;; The PIC12F675 has 8 pins, runs at 4MHz on its internal oscillator,
	;; and does need oscillator calibration.

	errorlevel -302

list            p=12f675
#include        p12f675.inc
#define         CLOCK_MHZ    4
                __CONFIG _CPD_OFF & _CP_OFF & _BODEN_ON & _MCLRE_OFF & _PWRTE_ON & _WDT_ON & _INTRC_OSC_NOCLKOUT

calibrate_oscillator macro
	bsf	STATUS, RP0
	call	0x3FF
	movwf	OSCCAL
	bcf	STATUS, RP0
	endm

	;; The End