;
; Utilities for sampling the TC74 temperature sensor
; Copyright 2004, Micah Dowty <micah@navi.cx>
;

TC74_ADDR_READ	equ (0x91 | (TC74_ADDR << 1))
TC74_ADDR_WRITE	equ (0x90 | (TC74_ADDR << 1))

#define	 tc74_define_vars	therm_total_low, therm_total_high, therm_count


	;; Take the TC74 out of standby mode if necessary
tc74_init macro
	call	i2c_start
	movlw	TC74_ADDR_WRITE	; Address it
	movwf	temp
	call	i2c_send_byte
	movlw	0x01		; Select the CONFIG register
	movwf	temp
	call	i2c_send_byte
	movlw	0x00		; Out of standby mode
	movwf	temp
	call	i2c_send_byte
	call	i2c_stop
	endm

	;; Put the TC74 into its own low-power shutdown mode
tc74_shutdown macro
	call	i2c_start
	movlw	TC74_ADDR_WRITE	; Address it
	movwf	temp
	call	i2c_send_byte
	movlw	0x01		; Select the CONFIG register
	movwf	temp
	call	i2c_send_byte
	movlw	0x80		; Into standby mode
	movwf	temp
	call	i2c_send_byte
	call	i2c_stop
	endm

	;; Read and store one sample from the TC74
tc74_read macro
	local done

	call	i2c_start
	movlw	TC74_ADDR_WRITE	; Address it
	movwf	temp
	call	i2c_send_byte
	movlw	0x00		; Select the TEMP register
	movwf	temp
	call	i2c_send_byte
	call	i2c_stop

	call	i2c_start
	movlw	TC74_ADDR_READ	; Address it for reading
	movwf	temp
	call	i2c_send_byte
	call	i2c_read_byte	; Read the result
	call	i2c_stop

	btfss	temp2, 0	; If the read was unsuccessful, return now
	goto	done

	incf	therm_count, f	; Accumulate this sample
	movf	temp, w
	addwf	therm_total_low, f
	btfsc	STATUS, C	; Carry the one...
	incf	therm_total_high, f
	btfsc	temp, 7		; Sign extend
	decf	therm_total_high, f

done
	endm


	;; Clear the temperature accumulator
tc74_flush macro
	clrf	therm_total_low
	clrf	therm_total_high
	clrf	therm_count
	endm

	;; The End
