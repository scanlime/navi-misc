;################################################################################
;
; hardware.inc - Hardware definitions for the USB Laser Projector
;
; This file is part of the USB Laser Projector project.
; Micah Dowty <micah@navi.cx>
;
;###############################################################################

; Default tristate values: all outputs
#define TRISA_VALUE	0x00
#define TRISB_VALUE	0x00
#define TRISC_VALUE	0x00

; Default port values: everything off
#define PORTA_VALUE 0x00
#define PORTB_VALUE 0x00
#define PORTC_VALUE 0x00

; All inputs are digital
#define ADCON1_VALUE 0x06

; Timer 1 on, 8:1 prescaler
#define T1CON_VALUE	0x31

;### The End ###
