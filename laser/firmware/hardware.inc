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

; Timer 1 on, no prescaler. This is our event timebase
#define T1CON_VALUE	0x01

; Timer 2 is running our coil driver PWM. Run it as slow
; as we can without causing the coils to audibly hum.
#define T2CON_VALUE 0x7C

; Turn both PWM outputs on, full 10-bit resolution
#define PR2_VALUE   0xFF
#define CCP1CON_VALUE 0x0C
#define CCP2CON_VALUE 0x0C

; 0x80 is our center position
#define CCPR1L_VALUE 0x80
#define CCPR2L_VALUE 0x80

#define EVENT_BUFFER_SIZE	.40

;### The End ###
