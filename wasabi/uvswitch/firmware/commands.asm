;################################################################################
;
; commands.asm - Implementation of our vendor-specific commands
;
; This file is part of the uvswitch project. This file contains original code,
; released into the public domain.
;
; Micah Dowty <micah@picogui.org>
;
;###############################################################################

#include <p16C765.inc>
#include "usb_defs.inc"
#include "../include/uvswitch_protocol.h"

	errorlevel -302		; supress "register not in bank0, check page bits" message

	global	CheckVendor

	extern	BufferData
	extern	wrongstate
	extern	temp
	extern	Send_0Len_pkt

	extern	channel_red
	extern	channel_white
	extern	channel_yellow
	extern	channel_bypass
	extern	switch_Update

	extern	analog_precharge_samples
	extern	analog_integration_samples

	code

; *********************************************************************
; Vendor Specific calls
; control is transferred here when bmRequestType bits 5 & 6 = 10 indicating
; the request is a vendor specific request.  This function then would
; interpret the bRequest field to determine what action is required.
; The end of each vendor specific command should be terminated with a
; return.
; *********************************************************************
CheckVendor

	movf	BufferData+bRequest,w
	xorlw	UVSWITCH_CTRL_SWITCH
	pagesel	SwitchRequest
	btfsc	STATUS,Z
	goto	SwitchRequest

	movf	BufferData+bRequest,w
	xorlw	UVSWITCH_CTRL_ADC_CYCLES
	pagesel	AdcCyclesRequest
	btfsc	STATUS,Z
	goto	AdcCyclesRequest

	pagesel	wrongstate		; Not a recognized request
	goto	wrongstate

	;********************* Request to set the switch status

	; wIndex :	red channel
	; wIndex+1:	white channel
	; wValue:	video channel
	; wValue+1:	bypass
SwitchRequest
	banksel BufferData
	movf	BufferData+wIndex, w
	banksel	channel_red
	movwf	channel_red

	banksel BufferData
	movf	BufferData+(wIndex+1), w
	banksel	channel_white
	movwf	channel_white

	banksel BufferData
	movf	BufferData+wValue, w
	banksel	channel_yellow
	movwf	channel_yellow

	banksel BufferData
	movf	BufferData+(wValue+1), w
	banksel	channel_bypass
	movwf	channel_bypass

	;;  Actually update the switches
	pagesel	switch_Update
	call	switch_Update

	; Acknowledge the request
	pagesel	Send_0Len_pkt
	call	Send_0Len_pkt
	return

	;********************* Request to set the number of ADC precharge/integration cycles

	; wIndex :	precharge cycles
	; wValue:	integration cycles
AdcCyclesRequest
	banksel BufferData
	movf	BufferData+wIndex, w
	banksel	analog_precharge_samples
	movwf	analog_precharge_samples

	banksel BufferData
	movf	BufferData+wValue, w
	banksel	analog_integration_samples
	movwf	analog_integration_samples

	; Acknowledge the request
	pagesel	Send_0Len_pkt
	call	Send_0Len_pkt
	return

	end

;### The End ###
