;################################################################################
;
; hardware.inc - Hardware definitions for the Raster Wand
;
; This file is part of the Raster Wand project.
; Micah Dowty <micah@navi.cx>
;
;###############################################################################

; Default tristate values: the only input we have is the
; interruption sensor, all others are outputs.
#define TRISA_VALUE	0x00
#define TRISB_VALUE	0x00
#define TRISC_VALUE	0x02

; Default port values: LEDs are active-low
#define PORTA_VALUE 0x00
#define PORTB_VALUE 0xFF
#define PORTC_VALUE 0x00

; All inputs are digital
#define ADCON1_VALUE 0x06

; Timer 1 on, 8:1 prescaler
#define T1CON_VALUE	0x31

#define LED_PORT 	 PORTB		; All of PORTB are active-low LEDs
#define COIL_DRIVER  PORTC, 0	; Coil drive transistor
#define ANGLE_SENSOR PORTC, 1	; Interruption sensor used to synchronize wand motion

;### The End ###
