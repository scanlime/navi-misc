;################################################################################
;
; hardware.inc - Definitions related to the hardware present in the MI6K
;
; This file is part of the MI6K project.
; Micah Dowty <micah@picogui.org>
;
;###############################################################################

;//****** Default port directions
#define TRISA_VALUE		0x00
#define	TRISB_VALUE		0x03
#define	TRISC_VALUE		0x00

;//****** I/O bits
#define LED_WHITE		PORTC,1
#define LED_BLUE		PORTC,2
#define VFD_POWER		PORTB,7
#define IR_TX			PORTB,6
#define IR_TX_MASK		0x40
#define EXT_POWER_SENSE	PORTB,1
#define IR_RX			PORTB,0
#define VFD_TX			PORTC,6

;### The End ###
