;################################################################################
;
; io.asm - Functions to manipulate the uvswitch's hardware
;
; This file is part of the uvswitch project. This file contains original code,
; released into the public domain.
;
; Micah Dowty <micah@picogui.org>
;
;###############################################################################

#include <p16C765.inc>

	errorlevel -302		; supress "register not in bank0, check page bits" message

	global	io_Init

#define	GREEN_LED	PORTC, 2
#define	YELLOW_LED	PORTC, 1
#define RED_LEDS	PORTD

#define	SCL_TRIS	TRISC, 6 ; The I2C lines are used as open-collector outputs
#define	SDA_TRIS	TRISC, 7
#define SCL_PORT	PORTC, 6
#define SDA_PORT	PORTC, 7

#define U2_ADDRESS	0x68	; I2C address of video switch U2
#define U3_ADDRESS	0x6A	; I2C address of video switch U3

#define CMD_SWITCHSET	0xC0	; Video switch command, sets switch states
#define CMD_MODESET	0x40	; Set switch modes (soft/hard)

#define DEF_PRECHARGE_SAMPLES .10	; Default number of analog samples to ignore to precharge the ADC hold capacitor
#define	DEF_INTEGRATION_SAMPLES .20	; Default number of samples to integrate for every reading

	;; ************************************************ Variables

bank0	udata
i2c_byte	res	1
i2c_iterator	res	1

i2c_address	res	1	; Packet bytes for a word-long write
i2c_command	res	1
i2c_high	res	1
i2c_low		res	1

channel_yellow	res	1	; Current input for video (yellow output)
channel_white	res	1	; Current input for white audio
channel_red	res	1	; Current input for red audio
channel_bypass	res	1	; If nonzero, indicates that the bypass input is active

analog_buffer	res	8
analog_channel	res 1
analog_sample	res 1

analog_precharge_samples res 1
analog_integration_samples res 1

	global	channel_yellow
	global	channel_white
	global	channel_red
	global	channel_bypass
	global	switch_Update
	global	led_Update
	global	analog_buffer
	global	SampleAnalog
	global	analog_precharge_samples
	global	analog_integration_samples

	code

	;; ************************************************ Initialization
io_Init

	banksel	ADCON1		; All analog inputs active, ADC turned on
	clrf	ADCON1
	banksel	ADCON0
	movlw	0x81
	movwf	ADCON0

	banksel	T1CON		; Make sure the timers, PWM units, and USART are disabled
	clrf	T1CON
	banksel	T2CON
	clrf	T2CON
	banksel	CCP1CON
	clrf	CCP1CON
	banksel	CCP2CON
	clrf	CCP2CON
	banksel	RCSTA
	clrf	RCSTA
	banksel	TXSTA
	movlw	0x02
	movwf	TXSTA

	banksel	TRISA		; Set pin directions:
	movlw	0x2F		; Analog 0-4, others unused
	movwf	TRISA
	movlw	0x00		; All unused
	movwf	TRISB
	movlw	0xC0		; LED outputs, I2C bus initially idle
	movwf	TRISC
	movlw	0x00		; LED outputs
	movwf	TRISD
	movlw	0x07		; Analog 5-7, PSP disabled
	movwf	TRISE

	banksel	PORTA		; All outputs initially disabled
	clrf	PORTA
	clrf	PORTB
	clrf	PORTC		; Especially output drivers for the I2C bus
	clrf	PORTD
	clrf	PORTE

	banksel	channel_yellow ; Turn off all inputs
	clrf	channel_yellow
	clrf	channel_red
	clrf	channel_white
	clrf	channel_bypass

	pagesel	switch_Update ; Initialize the video switch chips and LEDs
	call	switch_Update

	movlw	DEF_PRECHARGE_SAMPLES ; Set up default analog tweakables
	banksel	analog_precharge_samples
	movwf	analog_precharge_samples
	movlw	DEF_INTEGRATION_SAMPLES
	banksel	analog_integration_samples
	movwf	analog_integration_samples

	return


	;; ************************************************ I2C

	;; Delay to let the I2C bus stabilize
i2c_Delay
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	return

sdaHigh	macro
	pagesel	i2c_Delay
	call	i2c_Delay
	banksel	TRISA
	bsf		SDA_TRIS
	call	i2c_Delay
	endm

sdaLow	macro
	pagesel	i2c_Delay
	call	i2c_Delay
	banksel	PORTA
	bcf		SDA_PORT
	banksel	TRISA
	bcf		SDA_TRIS
	call	i2c_Delay
	endm

sclHigh	macro
	pagesel	i2c_Delay
	call	i2c_Delay
	banksel	TRISA
	bsf		SCL_TRIS
	call	i2c_Delay
	endm

sclLow	macro
	pagesel	i2c_Delay
	call	i2c_Delay
	banksel	PORTA
	bcf		SCL_PORT
	banksel	TRISA
	bcf		SCL_TRIS
	call	i2c_Delay
	endm


	;; Clock out a byte, in w, and the corresponding acknowledge bit
i2c_Byte
	banksel	i2c_byte
	movwf	i2c_byte

	movlw	8
	banksel i2c_iterator
	movwf	i2c_iterator	; Loop over all bits
bitLoop
	banksel	i2c_byte
	rlf	i2c_byte, f	; Send MSB-first
	pagesel	bitSet
	btfsc	STATUS, C	; Transfer the bit from C to SDA
	goto	bitSet
	sdaLow
	pagesel	bitClear
	goto	bitClear
bitSet
	sdaHigh
bitClear

	sclHigh					; Clock out this bit
	sclLow

	banksel	i2c_iterator
	decfsz	i2c_iterator, f	; Next bit...
	goto	bitLoop

	sdaHigh					; Let the device send an acknowledge bit, but ignore it
	sclHigh
	sclLow
	return


	;; Send a word-long I2C packet
i2c_Send
	sclHigh		; Start condition
	sdaLow
	sclLow

	banksel	i2c_address
	pagesel	i2c_Byte
	movf	i2c_address, w
	call	i2c_Byte

	banksel	i2c_command
	pagesel	i2c_Byte
	movf	i2c_command, w
	call	i2c_Byte

	banksel	i2c_high
	pagesel	i2c_Byte
	movf	i2c_high, w
	call	i2c_Byte

	banksel	i2c_low
	pagesel	i2c_Byte
	movf	i2c_low, w
	call	i2c_Byte

	sclHigh		; Stop condition
	sdaHigh
	return


	;; ************************************************ Video switches

	;; Configure both video switches, using the current channel_* values
switch_Update

	banksel	i2c_address	; Address U2
	movlw	U2_ADDRESS
	movwf	i2c_address
	movlw	CMD_MODESET	; Set switch modes
	movwf	i2c_command
	movlw	0x30		; Bypass video switches hard, audio switches soft
	movwf	i2c_high
	movlw	0x00
	movwf	i2c_low
	pagesel	i2c_Send
	call	i2c_Send

	banksel	i2c_address	; Address U3
	movlw	U3_ADDRESS
	movwf	i2c_address
	movlw	CMD_MODESET	; Set switch modes
	movwf	i2c_command
	movlw	0x00		; Video switches hard, audio switches soft
	movwf	i2c_high
	movlw	0xFF
	movwf	i2c_low
	pagesel	i2c_Send
	call	i2c_Send

	banksel	i2c_address	; Address U2
	movlw	U2_ADDRESS
	movwf	i2c_address
	movlw	CMD_SWITCHSET	; Set switch states
	movwf	i2c_command
	clrf	i2c_high	; Clear switch bits...
	clrf	i2c_low

	movlw	2		; U2 switch 1A:	red channel 2
	xorwf	channel_red, w
	btfsc	STATUS, Z
	bsf	i2c_low, 0

	movlw	1		; U2 switch 1B:	red channel 1
	xorwf	channel_red, w
	btfsc	STATUS, Z
	bsf	i2c_low, 1

	movlw	3		; U2 switch 2A:	red channel 3
	xorwf	channel_red, w
	btfsc	STATUS, Z
	bsf	i2c_low, 2

	movlw	4		; U2 switch 2B:	red channel 4
	xorwf	channel_red, w
	btfsc	STATUS, Z
	bsf	i2c_low, 3

	movlw	5		; U2 switch 3A:	red channel 5
	xorwf	channel_red, w
	btfsc	STATUS, Z
	bsf	i2c_low, 4

	movlw	6		; U2 switch 3B:	red channel 6
	xorwf	channel_red, w
	btfsc	STATUS, Z
	bsf	i2c_low, 5

	movlw	7		; U2 switch 4A:	red channel 7
	xorwf	channel_red, w
	btfsc	STATUS, Z
	bsf	i2c_low, 6

	movlw	8		; U2 switch 4B:	red channel 8
	xorwf	channel_red, w
	btfsc	STATUS, Z
	bsf	i2c_low, 7

	movlw	1		; U2 switch 6A:	white channel 1
	xorwf	channel_white, w
	btfsc	STATUS, Z
	bsf	i2c_high, 0

	movlw	2		; U2 switch 6B:	white channel 2
	xorwf	channel_white, w
	btfsc	STATUS, Z
	bsf	i2c_high, 1

	movlw	3		; U2 switch 7A:	white channel 3
	xorwf	channel_white, w
	btfsc	STATUS, Z
	bsf	i2c_high, 2

	movlw	4		; U2 switch 7B:	white channel 4
	xorwf	channel_white, w
	btfsc	STATUS, Z
	bsf	i2c_high, 3

	movf	channel_bypass, w ; U2 switch 5: !bypass (white out -> bypass out)
	btfsc	STATUS, Z
	bsf	i2c_high, 4

	movf	channel_bypass, w ; U2 switch 8: bypass (bypass in -> bypass out)
	btfss	STATUS, Z
	bsf	i2c_high, 5

	pagesel	i2c_Send	; Send U2 switch states
	call	i2c_Send

	banksel	i2c_address	; Address U3
	movlw	U3_ADDRESS
	movwf	i2c_address
	movlw	CMD_SWITCHSET	; Set switch states
	movwf	i2c_command
	clrf	i2c_high	; Clear switch bits...
	clrf	i2c_low

	movlw	2		; U3 switch 1A:	yellow channel 2
	xorwf	channel_yellow, w
	btfsc	STATUS, Z
	bsf	i2c_low, 0

	movlw	1		; U3 switch 1B:	yellow channel 1
	xorwf	channel_yellow, w
	btfsc	STATUS, Z
	bsf	i2c_low, 1

	movlw	3		; U3 switch 2A:	yellow channel 3
	xorwf	channel_yellow, w
	btfsc	STATUS, Z
	bsf	i2c_low, 2

	movlw	4		; U3 switch 2B:	yellow channel 4
	xorwf	channel_yellow, w
	btfsc	STATUS, Z
	bsf	i2c_low, 3

	movlw	5		; U3 switch 3A:	yellow channel 5
	xorwf	channel_yellow, w
	btfsc	STATUS, Z
	bsf	i2c_low, 4

	movlw	6		; U3 switch 3B:	yellow channel 6
	xorwf	channel_yellow, w
	btfsc	STATUS, Z
	bsf	i2c_low, 5

	movlw	7		; U3 switch 4A:	yellow channel 7
	xorwf	channel_yellow, w
	btfsc	STATUS, Z
	bsf	i2c_low, 6

	movlw	8		; U3 switch 4B:	yellow channel 8
	xorwf	channel_yellow, w
	btfsc	STATUS, Z
	bsf	i2c_low, 7

	movlw	5		; U3 switch 6A:	white channel 5
	xorwf	channel_white, w
	btfsc	STATUS, Z
	bsf	i2c_high, 0

	movlw	6		; U3 switch 6B:	white channel 6
	xorwf	channel_white, w
	btfsc	STATUS, Z
	bsf	i2c_high, 1

	movlw	7		; U3 switch 7A:	white channel 7
	xorwf	channel_white, w
	btfsc	STATUS, Z
	bsf	i2c_high, 2

	movlw	8		; U3 switch 7B:	white channel 8
	xorwf	channel_white, w
	btfsc	STATUS, Z
	bsf	i2c_high, 3

	pagesel	i2c_Send	; Send U3 switch states
	call	i2c_Send

	pagesel	led_Update
	goto	led_Update	; Update the LEDs now to reflect the new switch status


	;; ************************************************ LEDs

	;; Update all LEDs to reflect the current video switch settings
led_Update

	banksel	RED_LEDS
	clrf	RED_LEDS

	movlw	1		; Video channel 1
	banksel	channel_yellow
	xorwf	channel_yellow, w
	banksel	RED_LEDS
	btfsc	STATUS, Z
	bsf	RED_LEDS, 0

	movlw	2		; Video channel 2
	banksel	channel_yellow
	xorwf	channel_yellow, w
	banksel	RED_LEDS
	btfsc	STATUS, Z
	bsf	RED_LEDS, 1

	movlw	3		; Video channel 3
	banksel	channel_yellow
	xorwf	channel_yellow, w
	banksel	RED_LEDS
	btfsc	STATUS, Z
	bsf	RED_LEDS, 2

	movlw	4		; Video channel 4
	banksel	channel_yellow
	xorwf	channel_yellow, w
	banksel	RED_LEDS
	btfsc	STATUS, Z
	bsf	RED_LEDS, 3

	movlw	5		; Video channel 5
	banksel	channel_yellow
	xorwf	channel_yellow, w
	banksel	RED_LEDS
	btfsc	STATUS, Z
	bsf	RED_LEDS, 4

	movlw	6		; Video channel 6
	banksel	channel_yellow
	xorwf	channel_yellow, w
	banksel	RED_LEDS
	btfsc	STATUS, Z
	bsf	RED_LEDS, 5

	movlw	7		; Video channel 7
	banksel	channel_yellow
	xorwf	channel_yellow, w
	banksel	RED_LEDS
	btfsc	STATUS, Z
	bsf	RED_LEDS, 6

	movlw	8		; Video channel 8
	banksel	channel_yellow
	xorwf	channel_yellow, w
	banksel	RED_LEDS
	btfsc	STATUS, Z
	bsf	RED_LEDS, 7

	bcf	YELLOW_LED	; Bypass channel
	banksel	channel_bypass
	movf	channel_bypass, w
	banksel	PORTA
	btfss	STATUS, Z
	bsf	YELLOW_LED

	bsf	GREEN_LED	; Power LED on

	return

	;; ************************************************ Video detection ADCs

	; Sample all ADCs, put the results in analog_buffer
SampleAnalog
	
	bankisel analog_buffer		; Point IRP:FSR to our buffer
	movlw	analog_buffer
	movwf	FSR
	
	movlw	0x81				; Make sure the ADC is on and pointed at the first channel
	banksel	ADCON0
	movwf	ADCON0

	movlw	8					; Loop over 8 channels
	banksel	analog_channel
	movwf	analog_channel
channelLoop

	banksel	analog_precharge_samples
	movf	analog_precharge_samples,w ; Take several samples and ignore them, to precharge the hold capacitor.
	banksel	analog_sample		; This increases isolation between input channels.
	movwf	analog_sample
prechargeLoop
	
	banksel	ADCON0		; Take an analog sample
	bsf		ADCON0, GO
	pagesel	prechargeConversionLoop
prechargeConversionLoop
	btfsc	ADCON0, NOT_DONE
	goto	prechargeConversionLoop

	banksel	analog_sample		; Next precharge sample
	pagesel	prechargeLoop
	decfsz	analog_sample
	goto	prechargeLoop

	clrf	INDF				; INDF points to the current sample in our buffer, clear it to prepare to integrate A/D samples

	banksel	analog_integration_samples
	movf	analog_integration_samples,w ; Now take several more samples, adding them to INDF to integrate the video signal over time
	banksel	analog_sample
	movwf	analog_sample
integrationLoop

	banksel	ADCON0		; Take an analog sample
	bsf		ADCON0, GO
	pagesel	integrationConversionLoop
integrationConversionLoop
	btfsc	ADCON0, NOT_DONE
	goto	integrationConversionLoop
	
	banksel	ADRES		; Add the fresh sample to INDF
	movf	ADRES, w
	addwf	INDF, f
	btfss	STATUS, C	; If this caused an overflow, clamp to 0xFF
	goto	noOverflow
	movlw	0xFF
	movwf	INDF
noOverflow

	banksel	analog_sample		; Next integration sample
	pagesel	integrationLoop
	decfsz	analog_sample
	goto	integrationLoop

	incf	FSR, f		; Next buffer location
	movlw	0x08		; Next ADC channel
	banksel	ADCON0
	addwf	ADCON0, f
	
	banksel	analog_channel 	; Loop over all channels...
	pagesel	channelLoop
	decfsz	analog_channel
	goto	channelLoop

	return

	end

;### The End ###
