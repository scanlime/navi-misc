;################################################################################
;
; io.asm - Functions to manipulate general purpose IO pins using pin descriptors
;
; This file is part of the rcpod project. This file contains original code,
; released into the public domain.
;
; Micah Dowty <micah@picogui.org>
;
;###############################################################################

#include <p16C765.inc>

	errorlevel -302		; supress "register not in bank0, check page bits" message

	global	io_pin		; Current pin descriptor

	global	io_Assert	; Assert the pin descriptor passed in io_pin
	global	io_Deassert	; Invert the polarity of io_pin then assert it
	global	io_Read		; Test the value of the pin descriptor io_pin, return it in 'w'

bank0	udata
io_pin	res	1
io_tmp  res 1
io_iterator res 1

io_dummy_port res 1		; FSR is pointed at this by io_SetFSR if the pin descriptor is a no-op

	code

io_Deassert
	banksel	io_pin
	movlw	0x80		; Toggle the active low/high bit
	xorwf	io_pin, f	; and fall through to io_Assert...

io_Assert
	pagesel	io_SetFSR
	call	io_SetFSR

	movlw	0x87		; Mask off the low/high and bit number bits
	andwf	io_pin, w
	movwf	io_tmp

	; Test io_tmp and run the proper bcf/bsf instruction on INDF

	movlw	0x00		; Clear bit 0
	xorwf	io_tmp, w
	btfsc	STATUS, Z
	bcf		INDF, 0

	movlw	0x01		; Clear bit 1
	xorwf	io_tmp, w
	btfsc	STATUS, Z
	bcf		INDF, 1

	movlw	0x02		; Clear bit 2
	xorwf	io_tmp, w
	btfsc	STATUS, Z
	bcf		INDF, 2

	movlw	0x03		; Clear bit 3
	xorwf	io_tmp, w
	btfsc	STATUS, Z
	bcf		INDF, 3

	movlw	0x04		; Clear bit 4
	xorwf	io_tmp, w
	btfsc	STATUS, Z
	bcf		INDF, 4

	movlw	0x05		; Clear bit 5
	xorwf	io_tmp, w
	btfsc	STATUS, Z
	bcf		INDF, 5

	movlw	0x06		; Clear bit 6
	xorwf	io_tmp, w
	btfsc	STATUS, Z
	bcf		INDF, 6

	movlw	0x80		; Set bit 0
	xorwf	io_tmp, w
	btfsc	STATUS, Z
	bsf		INDF, 0

	movlw	0x81		; Set bit 1
	xorwf	io_tmp, w
	btfsc	STATUS, Z
	bsf		INDF, 1

	movlw	0x82		; Set bit 2
	xorwf	io_tmp, w
	btfsc	STATUS, Z
	bsf		INDF, 2

	movlw	0x83		; Set bit 3
	xorwf	io_tmp, w
	btfsc	STATUS, Z
	bsf		INDF, 3

	movlw	0x84		; Set bit 4
	xorwf	io_tmp, w
	btfsc	STATUS, Z
	bsf		INDF, 4

	movlw	0x85		; Set bit 5
	xorwf	io_tmp, w
	btfsc	STATUS, Z
	bsf		INDF, 5

	movlw	0x86		; Set bit 6
	xorwf	io_tmp, w
	btfsc	STATUS, Z
	bsf		INDF, 6
	return

	; Test the current pin descriptor, return its value in w
io_Read
	pagesel	io_SetFSR
	call	io_SetFSR

	movlw	0x07		; Mask off bit number, stick it plus 1 into io_iterator
	andwf	io_pin, w
	movwf	io_iterator

	movf	INDF, w		; Copy the current port value into io_tmp
	movwf	io_tmp

	btfss	io_pin, 7	; If the 'active high' bit is not set, complement our port reading
	comf	io_tmp, f

	pagesel	ioReadShiftLoop
ioReadShiftLoop
	rrf		io_tmp, f	; Shift the bit we're interested in into the Carry flag
	decfsz	io_iterator, f
	goto	ioReadShiftLoop

	btfss	STATUS, C	; Return the carry flag's value
	retlw	0
	retlw	1


	; Set FSR to point to the I/O port indicated by the current pin descriptor
io_SetFSR
	banksel	io_pin
	movlw	0x78		; Mask off the PORT/TRIS and port number bits
	andwf	io_pin, w
	movwf	io_tmp
	bankisel PORTA		; All PORT and TRIS registers are at an address < 0x100

	; If we don't have a valid address, point FSR at a dummy variable
	movlw	io_dummy_port
	movwf	FSR

	; Test io_tmp and set the proper PORT/TRIS address in FSR
	movlw	0x08		; PORTA
	xorwf	io_tmp, w
	movlw	PORTA
	btfsc	STATUS, Z
	movwf	FSR
	
	movlw	0x10		; PORTB
	xorwf	io_tmp, w
	movlw	PORTA
	btfsc	STATUS, Z
	movwf	FSR

	movlw	0x18		; PORTC
	xorwf	io_tmp, w
	movlw	PORTA
	btfsc	STATUS, Z
	movwf	FSR

	movlw	0x20		; PORTD
	xorwf	io_tmp, w
	movlw	PORTA
	btfsc	STATUS, Z
	movwf	FSR

	movlw	0x28		; PORTE
	xorwf	io_tmp, w
	movlw	PORTA
	btfsc	STATUS, Z
	movwf	FSR

	movlw	0x48		; TRISA
	xorwf	io_tmp, w
	movlw	TRISA
	btfsc	STATUS, Z
	movwf	FSR
	
	movlw	0x50		; TRISB
	xorwf	io_tmp, w
	movlw	TRISA
	btfsc	STATUS, Z
	movwf	FSR

	movlw	0x58		; TRISC
	xorwf	io_tmp, w
	movlw	TRISA
	btfsc	STATUS, Z
	movwf	FSR

	movlw	0x60		; TRISD
	xorwf	io_tmp, w
	movlw	TRISA
	btfsc	STATUS, Z
	movwf	FSR

	movlw	0x68		; TRISE
	xorwf	io_tmp, w
	movlw	TRISA
	btfsc	STATUS, Z
	movwf	FSR
	return

	end

;### The End ###
