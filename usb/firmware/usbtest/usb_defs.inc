;                            Software License Agreement
;
; The software supplied herewith by Microchip Technology Incorporated (the "Company")
; for its PICmicro® Microcontroller is intended and supplied to you, the Company’s
; customer, for use solely and exclusively on Microchip PICmicro Microcontroller
; products.
;
; The software is owned by the Company and/or its supplier, and is protected under
; applicable copyright laws. All rights are reserved. Any use in violation of the
; foregoing restrictions may subject the user to criminal sanctions under applicable
; laws, as well as to civil liability for the breach of the terms and conditions of
; this license.
;
; THIS SOFTWARE IS PROVIDED IN AN "AS IS" CONDITION. NO WARRANTIES, WHETHER EXPRESS,
; IMPLIED OR STATUTORY, INCLUDING, BUT NOT LIMITED TO, IMPLIED WARRANTIES OF
; MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE APPLY TO THIS SOFTWARE. THE
; COMPANY SHALL NOT, IN ANY CIRCUMSTANCES, BE LIABLE FOR SPECIAL, INCIDENTAL OR
; CONSEQUENTIAL DAMAGES, FOR ANY REASON WHATSOEVER.
;
; ###############################################################################
;	filename:		USB_DEFS.INC
; 
; Definitions used throughout the Chapter 9 code
;
; ###############################################################################
;
;	Author:			Dan Butler and Reston Condit
;	Company:		Microchip Technology Inc
;
;	Revision:		2.00
;	Date:			06 November 2002
;	Assembled using:	MPASM 2.80
;
;################################################################################
;
;	include files:
;		none
;
;################################################################################
; Edit these as appropriate for your descriptors
#define	NUM_CONFIGURATIONS	1
#define	NUM_INTERFACES		1

; Define the states that the USB interface can be in
#define	POWERED_STATE	0x00
#define	DEFAULT_STATE	0x01
#define	ADDRESS_STATE	0x02
#define	CONFIG_STATE	0x03

; Define the states for Control EndPoints
#define	EP_IDLE_STATE		0x00
#define	EP_SETUP_STATE		0x01
#define	EP_DISABLED_STATE	0xff

#define	ENDPT_DISABLED		0x00
#define ENDPT_IN_ONLY		0x02
#define ENDPT_OUT_ONLY		0x04
#define ENDPT_CONTROL		0x06	; enable for in, out and setup
#define ENDPT_NON_CONTROL	0x0E	; enable for in, and out


#define INT_STAT_MASK_RESET	0x01
#define INT_STAT_MASK_ERROR	0x02
#define INT_STAT_MASK_TOKEN_DONE	0x04
#define INT_STAT_MASK_SLEEP	0x08
#define INT_STAT_MASK_STALL	0x10

#define TOKEN_OUT	(0x01<<2)
#define TOKEN_ACK	(0x02<<2)
#define TOKEN_IN	(0x09<<2)
#define TOKEN_SETUP	(0x0D<<2)

#define USB_Buffer	0xB8		; on page 3 so actual address 0x1B8

; offsets from the beginning of the Buffer Descriptor
#define BYTECOUNT	0x01
#define	ADDRESS		0x02

; Descriptor types
#define	DEVICE		1
#define	CONFIGURATION	2
#define	STRING		3
#define	INTERFACE	4
#define	ENDPOINT	5

; offsets from the beginning of the setup data record
#define	bmRequestType	0x00
#define	bRequest	0x01
#define	wValue		0x02
#define	wValueHigh	0x03
#define	wIndex		0x04
#define	wIndexHigh	0x05
#define	wLength		0x06
#define	wLengthHigh	0x07

#define	CLEAR_FEATURE		0x01
#define	GET_CONFIGURATION	0x08
#define	GET_DESCRIPTOR		0x06
#define	GET_STRING_DESCRIPTOR	0x66
#define	GET_INTERFACE		0x0A
#define	GET_STATUS		0x00
#define	SET_ADDRESS		0x05
#define	SET_CONFIGURATION	0x09
#define	SET_FEATURE		0x03
#define	SET_INTERFACE		0x0B
#define	HID_SET_REPORT		0x21
#define	VEND_SET_MEMORY		0x80

#define	SVCUSBINT		0x01 << 2
#define	SVCTOKENDONE		0x02 << 2
#define	SVCRESET		0x03 << 2
#define	SVCSLEEP		0x04 << 2
#define	SVCSTALL		0x05 << 2
#define	SVCERROR		0x06 << 2
#define	SVCACTIVITY		0x07 << 2
#define	TOKENOUT		0x08 << 2
#define	TOKENIN			0x09 << 2
#define	TOKENSETUP		0x0A << 2
#define	CLEARFEATURE		0x0B << 2
#define	GETCONFIG		0x0C << 2
#define	GETDESCRIPTOR		0x0D << 2
#define	GETINTERFACE		0x0E << 2
#define	GETSTATUS		0x0F << 2
#define	SETADDRESS		0x10 << 2
#define	SETCONFIG		0x11 << 2
#define	SETFEATURE		0x12 << 2
#define	SETINTERFACE		0x13 << 2
#define	FINISHSETADDRESS	0x14 << 2
#define	COPYDESC2EP0		0x15 << 2
#define	COPYSTRINGDESC2EP0	0x16 << 2
#define	ZEROLENPACKET		0x17 << 2

COPYBUFFERDESCRIPTOR	macro
	bankisel BD0OST
	banksel	USTAT
	movf	USTAT,w			; get the status register
	addlw	0xA0			; add the offset to the beginning of the BD's
	movwf	FSR			; save in the FSR.
	bcf 	STATUS, RP0		; back to bank 2
	movf	INDF,w
	movwf	BufferDescriptor	; in shared RAM
	incf	FSR,f
	movf	INDF,w
	movwf	BufferDescriptor+1
	incf	FSR,f
	movf	INDF,w
	movwf	BufferDescriptor+2
	endm	
	
; Increments a 16bit counter, stored Lowbyte:Highbyte
INCREMENT16	macro	index
	local	endinc16
	incfsz	index,f
	goto	endinc16
	incf	index+1,f
endinc16
	endm	

REQUESTERROR	macro
	bsf	STATUS,RP0	; page 3
	movf	USTAT,w		; get the status register
	addlw	0xA0		; add the offset to the beginning of the BD's
	movwf	FSR
	bsf	INDF,EP_STALL	; set endpoint stall bit
	bcf	STATUS,RP0	; back to page 2
	endm	

; *********************************************************************
; Checks to see if we are enumerated.  Application should not attempt 
; to communicate with host until enumerated.
;
; Returns result in the STATUS,Z
;   1 - Configured
;   0 - Not Configured
; *********************************************************************
ConfiguredUSB macro
	banksel	USWSTAT
	movf	USWSTAT,w	
	andlw	0x03		; save lower 2 bits of USWSTAT	
	xorlw	CONFIG_STATE	; compare with configured state
	endm

GETEP1  macro
; **********************************************************************
; GetEP1
; Enter with buffer pointer in IRP+FSR.  
; The function checks the UOWNs bit for the EP1 OUT endpoint, 
; and copies the buffer if available.  Upon completion of the function, 
; the bank bits are restored to the state they were in when this 
; function was entered.
;
; Returns the bytecount in the W register and return status in the carry
; bit as follows:
; 0 - no buffer available,
; 1 - Buffer copied and buffer made available for next transfer.
; 
; The number of bytes moved is returned in W reg.
; **********************************************************************
GetEP1
	global	GetEP1
	local	getEPloop
	local	exitgetloop
	local	nobuffer

	movf	STATUS,w	; save bank bits before we trash them
	banksel	RP_save 	; switch to bank 2
	movwf	RP_save

	movf	FSR,w
	movwf	dest_ptr	; save the buffer destination pointer

	banksel	BD1OST  	; bank 3
	pagesel	nobuffer
	btfsc	BD1OST,UOWN 	; Has the buffer been filled?
	goto	nobuffer	; nope, OWN = 1 ==> SIE owns the buffer
				; Yep: OWN = 0 ==> PIC owns the buffer

	movf	BD1OBC,w	; get byte count
	banksel	counter		; bank 2
	movwf	counter
	movwf	bytecounter	; # of bytes that will be moved
	pagesel	exitgetloop
	btfsc	STATUS,Z	; is it a zero length buffer?
	goto	exitgetloop	; yes, bail out now and avoid the rush
	banksel	BD1OAL		; bank 3
	movf	BD1OAL,w	; get address pointer
	banksel	source_ptr	; bank 2
	movwf	source_ptr

; This loop operates with the direct bank bits set to bank 2, while IRP
; gets switched as needed to for the buffer copy
getEPloop
	bsf 	STATUS,IRP	; select high banks on INDF
	movf	source_ptr,w	; get source pointer
	movwf	FSR
	movf	INDF,w
	movwf	GPtemp		; in common RAM to avoid paging issues

	movf	dest_ptr,w
	movwf	FSR
	btfss	RP_save,IRP	; should it be zero?
	bcf 	STATUS,IRP	; yes: make it so.
	movf	GPtemp,w	; no, get the byte we read.
	movwf	INDF		; store it
	incf	dest_ptr,f
	incf	source_ptr,f
	pagesel	getEPloop
	decfsz	counter,f
	goto	getEPloop

exitgetloop
	bsf 	STATUS,RP0	; bank 3
	movf	BD1OST,w
	andlw	0x40		; save only the data 0/1 bit
	xorlw	0x40		; toggle the data o/1 bit
	iorlw	0x88		; set owns bit and DTS bit
	movwf	BD1OST

	movlw	0x08		; reset byte counter
	movwf	BD1OBC
	bcf 	STATUS,RP0	; bank 2
	movf	bytecounter,w	; return # of bytes moved in W reg
	movwf	GPtemp		; move byte counter to temporary ram
	movf	RP_save,w	; restore bank bits
	movwf	STATUS
	movf	GPtemp,w	; load W with the byte count
	bsf 	STATUS,C	; signal success
	return

nobuffer
	banksel	RP_save		; restore the bank bits
	movf	RP_save,w
	movwf	STATUS
	bcf 	STATUS,C
	return
	endm

GETEP2	macro
; **********************************************************************
; GetEP2
; Enter with buffer pointer in IRP+FSR.  
; The function checks the UOWNs bit for the EP2 OUT endpoint, 
; and copies the buffer if available.  Upon completion of the function, 
; the bank bits are restored to the state they were in when this 
; function was entered.
;
; Returns the bytecount in the W register and return status in the carry
; bit as follows:
; 0 - no buffer available,
; 1 - Buffer copied and buffer made available for next transfer.
; 
; The number of bytes moved is returned in W reg.
; **********************************************************************
GetEP2
	global	GetEP2
	local	getEPloop2
	local	exitgetloop2
	local	nobuffer2

	movf	STATUS,w	; save bank bits before we trash them
	banksel	RP_save		; switch to bank 2
	movwf	RP_save

	movf	FSR,w
	movwf	dest_ptr	; save the buffer destination pointer

	banksel	BD2OST		; bank 3
	pagesel	nobuffer2
	btfsc	BD2OST,UOWN	; Has the buffer been filled?
	goto	nobuffer2	; nope, OWN = 1 ==> SIE owns the buffer
				; Yep: OWN = 0 ==> PIC owns the buffer

	movf	BD2OBC,w	; get byte count
	banksel	counter		; bank 2
	movwf	counter
	movwf	bytecounter	; # of bytes that will be moved
	pagesel	exitgetloop2
	btfsc	STATUS,Z	; is it a zero length buffer?
	goto	exitgetloop2	; yes, bail out now and avoid the rush
	banksel	BD2OAL		; bank 3
	movf	BD2OAL,w	; get address pointer
	banksel	source_ptr	; bank 2
	movwf	source_ptr

; This loop operates with the direct bank bits set to bank 2, while IRP
; gets switched as needed to for the buffer copy
getEPloop2
	bsf 	STATUS,IRP	; select high banks on INDF
	movf	source_ptr,w	; get source pointer
	movwf	FSR
	movf	INDF,w
	movwf	GPtemp		; in common RAM to avoid paging issues

	movf	dest_ptr,w
	movwf	FSR
	btfss	RP_save,IRP	; should it be zero?
	bcf 	STATUS,IRP	; yes: make it so.
	movf	GPtemp,w	; no, get the byte we read.
	movwf	INDF		; store it
	incf	dest_ptr,f
	incf	source_ptr,f
	pagesel	getEPloop2
	decfsz	counter,f
	goto	getEPloop2

exitgetloop2
	bsf 	STATUS,RP0	; bank 3
	movf	BD2OST,w
	andlw	0x40		; save only the data 0/1 bit
	xorlw	0x40		; toggle the data o/1 bit
	iorlw	0x88		; set owns bit and DTS bit
	movwf	BD2OST

	movlw	0x08		; reset byte counter
	movwf	BD2OBC
	bcf 	STATUS,RP0	; bank 2
	movf	bytecounter,w	; return # of bytes moved in W reg
	movwf	GPtemp		; move byte counter to temporary ram
	movf	RP_save,w	; restore bank bits
	movwf	STATUS
	movf	GPtemp,w	; load W with the byte count
	bsf 	STATUS,C	; signal success
	return

nobuffer2
	banksel	RP_save		; restore the bank bits
	movf	RP_save,w
	movwf	STATUS
	bcf 	STATUS,C
	return
	endm

PUTEP1	macro
; ******************************************************************
; PutEP1
; Enter with bytecount in W and buffer pointer in IRP+FSR.
;
; Tests the UOWNs bit for the IN side of the specified Endpoint.
; If we own the buffer, the buffer pointed to by the FSR is copied
; to the EPn IN buffer, then the UOWNs bit is set so the data will be
; TX'd next time polled.  
;
; Returns the status in the carry bit as follows:
; 1 - buffer available and copied.
; 0 - buffer not available (try again later)
; ******************************************************************
PutEP1
	global	PutEP1
	local	putEPloop
	local	exitputloop
	local	nobufferputep

	movwf	GPtemp		; save Bytecount temporarily in common RAM

	movf	STATUS,w	; save bank bits before we trash them
	banksel	RP_save		; switch to bank 2
	movwf	RP_save

	movf	GPtemp,w
	andlw	0x0F		; extract byte count.
	movwf	counter

	movf	FSR,w
	movwf	source_ptr

	movf	counter,w	; prepare to copy the byte count
	banksel	BD1IST		; bank 3
	pagesel	nobufferputep1
	btfsc	BD1IST,UOWN	; is the buffer already full?
	goto	nobufferputep1	; yes - don't write over it

	movwf	BD1IBC		; set byte count in BD
	pagesel	exitputloop
	btfsc	STATUS,Z	; is it a zero length buffer?
	goto	exitputloop	; yes, bail out now and avoid the rush
	movf	BD1IAL,w	; get address pointer
	bcf 	STATUS,RP0	; back to bank 2
	movwf	dest_ptr

; This loop operates with the direct bits set to bank 2, while IRP
; gets switched as needed to for the buffer copy
putEPloop
	bsf	STATUS,IRP	; assume IRP is set
	btfss	RP_save,IRP	; should it be zero?
	bcf	STATUS,IRP	; yes: make it so.
	movf	source_ptr,w
	movwf	FSR
	movf	INDF,w
	movwf	GPtemp

	bsf	STATUS,IRP	; select high banks on INDF
	movf	dest_ptr,w
	movwf	FSR
	movf	GPtemp,w	; no, get the byte we read.
	movwf	INDF		; store it

	incf	dest_ptr,f
	incf	source_ptr,f
	pagesel	putEPloop
	decfsz	counter,f
	goto	putEPloop

exitputloop
	bsf 	STATUS,RP0	; back to bank 3
	movf	BD1IST,w
	andlw	0x40		; save only the data 0/1 bit
	xorlw	0x40		; toggle the data o/1 bit
	iorlw	0x88		; set owns bit and DTS bit
	movwf	BD1IST
	banksel	RP_save
	movf	RP_save,w	; restore bank bits the way we found them
	movwf	STATUS
	bsf 	STATUS,C	; set carry to show success
	return

nobufferputep1
	bcf 	STATUS,C
	return
	endm

PUTEP2	macro
; ******************************************************************
; PutEP2
; Enter with bytecount in W and buffer pointer in IRP+FSR.
;
; Tests the UOWNs bit for the IN side of the specified Endpoint.
; If we own the buffer, the buffer pointed to by the FSR is copied
; to the EPn IN buffer, then the UOWNs bit is set so the data will be
; TX'd next time polled.  
;
; Returns the status in the carry bit as follows:
; 1 - buffer available and copied.
; 0 - buffer not available (try again later)
; ******************************************************************
PutEP2
	global	PutEP2
	local	putEPloop2
	local	exitputloop2
	local	nobufferputep2
	
	movwf	GPtemp		; save Bytecount temporarily in common RAM

	movf	STATUS,w	; save bank bits before we trash them
	banksel	RP_save		; switch to bank 2
	movwf	RP_save

	movf	GPtemp,w
	andlw	0x0F		; extract byte count.
	movwf	counter

	movf	FSR,w
	movwf	source_ptr

	movf	counter,w	; prepare to copy the byte count
	banksel	BD2IST		; bank 3
	pagesel	nobufferputep2
	btfsc	BD2IST,UOWN	; is the buffer already full?
	goto	nobufferputep2	; yes - don't write over it

	movwf	BD2IBC		; set byte count in BD
	pagesel	exitputloop2
	btfsc	STATUS,Z	; is it a zero length buffer?
	goto	exitputloop2	; yes, bail out now and avoid the rush
	movf	BD2IAL,w	; get address pointer
	bcf 	STATUS,RP0	; back to bank 2
	movwf	dest_ptr

; This loop operates with the direct bits set to bank 2, while IRP
; gets switched as needed to for the buffer copy
putEPloop2
	bsf 	STATUS,IRP	; assume IRP is set
	btfss	RP_save,IRP	; should it be zero?
	bcf 	STATUS,IRP	; yes: make it so.
	movf	source_ptr,w
	movwf	FSR
	movf	INDF,w
	movwf	GPtemp

	bsf 	STATUS,IRP	; select high banks on INDF
	movf	dest_ptr,w
	movwf	FSR
	movf	GPtemp,w	; no, get the byte we read.
	movwf	INDF		; store it

	incf	dest_ptr,f
	incf	source_ptr,f
	pagesel	putEPloop2
	decfsz	counter,f
	goto	putEPloop2

exitputloop2
	bsf 	STATUS,RP0	; back to bank 3
	movf	BD2IST,w
	andlw	0x40		; save only the data 0/1 bit
	xorlw	0x40		; toggle the data o/1 bit
	iorlw	0x88		; set owns bit and DTS bit
	movwf	BD2IST
	banksel	RP_save
	movf	RP_save,w	; restore bank bits the way we found them
	movwf	STATUS
	bsf 	STATUS,C	; set carry to show success
	return

nobufferputep2
	bcf 	STATUS,C
	return
	endm
