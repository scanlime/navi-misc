;
; Sega Genesis Net-controller Interface
;
; Wasabi Net-controllers Project
; Micah Dowty <micah@navi.cx>
;
; This device emulates two 3-button Sega Genesis
; controllers. It accepts low-speed USB requests
; with the desired state of each controller.
;
; The controller interface is described well at:
; http://www-2.cs.cmu.edu/~chuck/infopg/segasix.txt
;
; The prototype was mounted inside the Genesis console
; itself, with a USB type B plug added to the back.
; This gives us control over the reset button and
; frees us from the low-quality connectors used on
; the front. This same firmware should work fine
; with normal DB-9 cables connecting it to the controller
; ports.
;
; The PIC16C745 power and USB are connected as
; described in the data sheet. I/Os are allocated
; as follows. Note that all controller pins were
; connected via 47k resistors so that externally
; attached controllers can override the USB interface.
; This is unnecessary if you're connecting this to the
; genesis externally.
;
;   RB4: Controller 1, pin 7 (select)
;   RB5: Controller 2, pin 7 (select)
;   RB6: Reserved (interrupt-on-change)
;   RB7: Reserved (interrupt-on-change)
;
;   RA0: Controller 1, pin 6 (A/B)
;   RA1: Controller 1, pin 9 (Start/C)
;   RA2: Controller 2, pin 6 (A/B)
;   RA3: Controller 2, pin 9 (Start/C)
;
;   RB0: Controller 1, pin 1 (up)
;   RB1: Controller 1, pin 2 (down)
;   RB2: Controller 1, pin 3 (left)
;   RB3: Controller 1, pin 4 (right)
;
;   RC0: Controller 2, pin 1 (up)
;   RC1: Controller 2, pin 2 (down)
;   RC2: Controller 2, pin 3 (left)
;   RC6: Controller 2, pin 4 (right)
;
;   RA5: Reset button output
;   RC7: Power switch output

#define PIN_C1_SELECT	PORTB, 4
#define PIN_C2_SELECT	PORTB, 5

#define PIN_C1_A_B	PORTA, 0
#define PIN_C1_START_C	PORTA, 1
#define PIN_C2_A_B	PORTA, 2
#define PIN_C2_START_C	PORTA, 3

#define PIN_C1_UP	PORTB, 0
#define PIN_C1_DOWN	PORTB, 1
#define PIN_C1_LEFT	PORTB, 2
#define PIN_C1_RIGHT	PORTB, 3

#define PIN_C2_UP	PORTB, 0
#define PIN_C2_DOWN	PORTB, 1
#define PIN_C2_LEFT	PORTB, 2
#define PIN_C2_RIGHT	PORTB, 6

#define PIN_RESET	PORTA, 5   ; Pulls high when active, high-z when inactive
#define TRIS_RESET	TRISA, 5

#define PIN_POWER	PORTC, 7

; --- The End ---