;
; An I2C master implementation
; Copyright 2004, Micah Dowty <micah@navi.cx>
;

i2c_sda_high macro
	movlw	BATT_VOLT_MASK | UNUSED_MASK | SDA_MASK
	bsf	STATUS, RP0
	movwf	TRISIO
	bcf	STATUS, RP0
	endm

i2c_sda_low macro
	movlw	BATT_VOLT_MASK | UNUSED_MASK
	bsf	STATUS, RP0
	movwf	TRISIO
	bcf	STATUS, RP0
	endm

i2c_scl_high macro
	bsf	iolatch, SCL_PIN
	movf	iolatch, w
	movwf	GPIO
	endm

i2c_scl_low macro
	bcf	iolatch, SCL_PIN
	movf	iolatch, w
	movwf	GPIO
	endm

	;; Define non-macro I2C routines
i2c_define_all	macro

i2c_delay
	nop
	nop
	nop
	nop
	nop
	return

i2c_start
	i2c_sda_low
	call	i2c_delay
	i2c_scl_low
	call	i2c_delay
	return

i2c_stop
	i2c_sda_low
	call	i2c_delay
	i2c_scl_high
	call	i2c_delay
	i2c_sda_high
	call	i2c_delay
	return

i2c_send_zero
	i2c_sda_low
	call	i2c_delay
	i2c_scl_high
	call	i2c_delay
	i2c_scl_low
	call	i2c_delay
	return

i2c_send_one
	i2c_sda_high
	call	i2c_delay
	i2c_scl_high
	call	i2c_delay
	i2c_scl_low
	call	i2c_delay
	return

	;; Shift one bit out of 'temp'
i2c_send_bit
	rlf	temp, f
	btfss	STATUS, C
	goto	i2c_send_zero
	goto	i2c_send_one

	;; Shift one byte out of 'temp'
i2c_send_byte
	call	i2c_send_bit
	call	i2c_send_bit
	call	i2c_send_bit
	call	i2c_send_bit
	call	i2c_send_bit
	call	i2c_send_bit
	call	i2c_send_bit
	call	i2c_send_bit
	i2c_scl_high		; Clock in and ignore the ACK
	call	i2c_delay
	i2c_scl_low
	return

	;; Shift one bit into 'temp'
i2c_read_bit
	rlf	temp, f
	bcf	temp, 0
	i2c_scl_high
	call	i2c_delay
	btfsc	GPIO, SDA_PIN
	bsf	temp, 0
	i2c_scl_low
	call	i2c_delay
	return

	;; Shift one byte into 'temp'. Returns with temp2=1 on success,
	;; temp2=0 on failure.
i2c_read_byte
	call	i2c_read_bit
	call	i2c_read_bit
	call	i2c_read_bit
	call	i2c_read_bit
	call	i2c_read_bit
	call	i2c_read_bit
	call	i2c_read_bit
	call	i2c_read_bit

	i2c_sda_high		; Clock in the ACK
	call	i2c_delay
	i2c_scl_high
	call	i2c_delay
	clrf	temp2		; Read the ACK, store our return code
	btfsc	GPIO, SDA_PIN
	bsf	temp2, 0
	i2c_scl_low
	return

	endm

	;; The End
