;################################################################################
;
; hardware.inc - Hardware definitions for the Raster Wand
;
; This file is part of the Raster Wand project.
; Micah Dowty <micah@navi.cx>
;
;###############################################################################

; The available hardware models...
#define HW_RAVINIA	.1			; The cheap but hard to find Ravinia clocks
#define HW_XP3		.2			; Fascinations XP3 clock

; Define this to indicate the hardware this firmware is for
#define HWMODEL		HW_RAVINIA
;#define HWMODEL		HW_XP3

#if HWMODEL == HW_RAVINIA
#define TRISA_VALUE	0x3F	; Buttons and angle sensor are inputs
#define TRISB_VALUE	0x00
#define TRISC_VALUE	0x02
#define PORTA_VALUE 0x00 	; LEDs are active-low, everything off
#define PORTB_VALUE 0xFF
#define PORTC_VALUE 0x00
#define LEDS_ACTIVE_LOW
#define BUTTON_AND_MASK		0x3F	; We have all six buttons
#define BUTTON_XOR_MASK		0x00	; Everything's active-high, nothing to do
#endif

#if HWMODEL == HW_XP3
#define TRISA_VALUE	0x1F	; Buttons and angle sensor are inputs, but we have no power switch
#define TRISB_VALUE	0x00
#define TRISC_VALUE	0x02
#define PORTA_VALUE 0x00 	; LEDs are active-high, everything off
#define PORTB_VALUE 0x00
#define PORTC_VALUE 0x00
#define LEDS_ACTIVE_HIGH
#define BUTTON_AND_MASK		0x1F	; We have no power button, mask it to zero
#define BUTTON_XOR_MASK		0x3F	; Active-low buttons, and force the power button on
#endif

; All inputs are digital
#define ADCON1_VALUE 0x06

; Timer 1 on, 8:1 prescaler
#define T1CON_VALUE	0x31

#define BUTTON_PORT  PORTA
#define LED_PORT 	 PORTB		; All of PORTB are active-low LEDs
#define COIL_DRIVER  PORTC, 0	; Coil drive transistor
#define ANGLE_SENSOR PORTC, 1	; Interruption sensor used to synchronize wand motion

#define DEBUG_PIN    PORTC, 2	; Not connected, used for firmware debugging

; Our display is 80x8 pixels. The vertical resolution is limited by the number
; of LEDs on our wand, but the horizontal is only limited in practise by our memory.
#define	NUM_COLUMNS	0x50

;### The End ###
