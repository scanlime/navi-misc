;################################################################################
;
; i2c.asm - Implements an I2C bus master in software
;
; The RCPOD project
; Copyright (C) 2004 Micah Dowty <micah@navi.cx>
;
;  This library is free software; you can redistribute it and/or
;  modify it under the terms of the GNU Lesser General Public
;  License as published by the Free Software Foundation; either
;  version 2.1 of the License, or (at your option) any later version.
;
;  This library is distributed in the hope that it will be useful,
;  but WITHOUT ANY WARRANTY; without even the implied warranty of
;  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
;  Lesser General Public License for more details.
;
;  You should have received a copy of the GNU Lesser General Public
;  License along with this library; if not, write to the Free Software
;  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
;
;###############################################################################

	errorlevel -226		; suppress the crazy include file warnings from GPASM

#include <p16C765.inc>
#include "usb_defs.inc"

	errorlevel -302		; supress "register not in bank0, check page bits" message

	global	i2c_speed	; Current bus settings
	global	i2c_clock_pin
	global	i2c_data_pin
	global	i2c_address

	global	i2c_ack_count

	;; Write/read 'w' bytes using the buffer at IRP:FSR and the current bus
	;; pins, speed, and address. Accumulates successful acks in i2c_ack_count.
	global	i2c_Write
	global	i2c_Read

	extern	io_pin
	extern	io_Assert
	extern	io_Deassert
	extern	io_Read

bank1	udata

i2c_speed	res	1
i2c_clock_pin	res	1
i2c_data_pin	res	1
i2c_address	res	1
i2c_ack_count	res	1

i2c_byte	res	1
i2c_byte_count	res	1
i2c_bit_count	res	1

	code


;; **********************************************************************************
;; *************************************************************** I/O Utilities ****
;; **********************************************************************************

	;; Assert a pin descriptor, from the given file register
pin_assert macro reg
	banksel	reg
	movf	reg, w
	banksel	io_pin
	movwf	io_pin
	pscall	io_Assert
	endm

	;; Deassert a pin descriptor, from the given file register
pin_deassert macro reg
	banksel	reg
	movf	reg, w
	banksel	io_pin
	movwf	io_pin
	pscall	io_Deassert
	endm

	;; Make the given pin descriptor an output
pin_output macro reg
	banksel	reg
	movf	reg, w
	iorlw	0x40		; Set the TRIS bit
	andlw	0x7F		; Clear the HIGH bit
	banksel	io_pin
	movwf	io_pin
	pscall	io_Assert
	endm

	;; Make the given pin descriptor an input
pin_input macro reg
	banksel	reg
	movf	reg, w
	iorlw	0xC0		; Set the TRIS and HIGH bits
	banksel	io_pin
	movwf	io_pin
	pscall	io_Assert
	endm

	;; Manipulate the clock output
clock_high
	pin_assert i2c_clock_pin
	return

clock_low
	pin_deassert i2c_clock_pin
	return

	;; Manipulate the bidirectional open-drain data line
data_high
	pin_input i2c_data_pin
	return

data_low
	pin_output i2c_data_pin
	return

data_read
	banksel	i2c_data_pin
	movf	i2c_data_pin, w
	banksel	io_pin
	movwf	io_pin
	pscall	io_Read
	return

	;; Generate the appropriate delay between i2c bus transitions,
	;; approximately 1/2 the bit period.
i2c_delay
	return

	;; Put the bus into various states
i2c_stop
	pscall	i2c_delay
	pscall	data_low
	pscall	i2c_delay
	pscall	clock_high
	pscall	i2c_delay
	pscall	data_high
	return

i2c_start
	pscall	clock_high
	pin_output i2c_clock_pin
	pin_deassert i2c_data_pin
	pscall	data_low
	pscall	i2c_delay
	pscall	clock_low
	return

i2c_clockout
	pscall	i2c_delay
	pscall	clock_high
	pscall	i2c_delay
	pscall	clock_low
	return


	;; Macro for looping over the I2C buffer
i2c_buffer_loop macro	lbl
	banksel	i2c_byte_count
	incf	FSR, f
	pagesel	lbl
	decfsz	i2c_byte_count, f
	goto	lbl
	endm


	;; Write out i2c_byte. If and only if the byte was successfully acknowledged,
	;; the zero flag will be set.
i2c_write_byte
	banksel	i2c_bit_count
	movlw	8
	movwf	i2c_bit_count
write_loop
	banksel	i2c_byte		; Shift the next MSB into the carry flag
	rlf	i2c_byte, f
	pagesel	write_zero
	btfss	STATUS, C
	goto	write_zero		; Put the next bit on the bus
write_one
	pscall	data_high
	psgoto	write_next
write_zero
	pscall	data_low
write_next
	pscall	i2c_clockout		; Clock out this bit, with the right delays
	banksel	i2c_bit_count		; Next bit...
	pagesel	write_loop
	decfsz	i2c_bit_count, f
	goto	write_loop

	pscall	i2c_delay		; Clock in the ACK bit
	pscall	clock_high
	pscall	data_high
	pscall	i2c_delay

	pscall	data_read		; Store the ACK bit in i2c_byte
	banksel	i2c_byte
	movwf	i2c_byte

	pscall	clock_low		; Pull the clock back low and return

	banksel	i2c_byte
	movf	i2c_byte, w		; Zero flag set if we got ACK'ed
	return


	;; Read into i2c_byte. This does not clock in an ACK byte, call either i2c_read_ack
	;; or i2c_read_nak after this, to indicate whether you want more data.
i2c_read_byte
	banksel	i2c_bit_count
	movlw	8
	movwf	i2c_bit_count
read_loop

	pscall	data_high		; Release the data line
	pscall	i2c_delay		; Clock high, wait for valid data
	pscall	clock_high
	pscall	i2c_delay

	pscall	data_read		; Shift in the next bit
	banksel	i2c_byte
	rlf	i2c_byte, f
	iorwf	i2c_byte, f

	pscall	clock_low		; Clock low again
	banksel	i2c_bit_count		; Next bit...
	pagesel	read_loop
	decfsz	i2c_bit_count, f
	goto	read_loop
	return


	;; Clock out a low bit indicating that we've acknowledged a read and want more data
i2c_read_ack
	pscall	data_low
	pscall	i2c_clockout
	return


	;; Clock out a high bit indicating that we don't want any more data
i2c_read_nak
	pscall	data_high
	pscall	i2c_clockout
	return


;; **********************************************************************************
;; *************************************************************** Public Methods ***
;; **********************************************************************************


	;; Write 'w' bytes from IRP:FSR to the current bus
i2c_Write
	banksel	i2c_byte_count
	movwf	i2c_byte_count
	pscall	i2c_start

	banksel	i2c_address		; Write the address, with the read bit clear
	rlf	i2c_address, w
	andlw	0xFE
	movwf	i2c_byte

	pscall	i2c_write_byte		; Write the byte, give up if we get no ACK
	pagesel	i2c_Write_done
	btfss	STATUS, Z
	goto	i2c_Write_done

	banksel	i2c_ack_count		; Note that we got an ACK
	incf	i2c_ack_count, f

	movf	i2c_byte_count, w	; If we have no bytes to write, skip the loop
	pagesel	i2c_Write_done
	btfsc	STATUS, Z
	goto	i2c_Write_done

i2c_Write_loop
	movf	INDF, w
	banksel	i2c_byte
	movwf	i2c_byte

	pscall	i2c_write_byte		; Write the byte, give up if we get no ACK
	pagesel	i2c_Write_done
	btfss	STATUS, Z
	goto	i2c_Write_done

	banksel	i2c_ack_count		; Note that we got an ACK
	incf	i2c_ack_count, f

	i2c_buffer_loop i2c_Write_loop	; Next...

i2c_Write_done
	pscall	i2c_stop
	return


	;; Read 'w' bytes into IRP:FSR from the current bus
i2c_Read
	banksel	i2c_byte_count
	movwf	i2c_byte_count
	pscall	i2c_start

	banksel	i2c_address		; Write the address, with the read bit set
	rlf	i2c_address, w
	andlw	0xFE
	iorlw	0x01
	movwf	i2c_byte

	pscall	i2c_write_byte		; Write the byte, give up if we get no ACK
	pagesel	i2c_Read_done
	btfss	STATUS, Z
	goto	i2c_Read_done

	banksel	i2c_ack_count		; Note that we got an ACK
	incf	i2c_ack_count, f

	movf	i2c_byte_count, w	; If we have no bytes to read, skip the loop
	pagesel	i2c_Read_done
	btfsc	STATUS, Z
	goto	i2c_Read_done

	psgoto	i2c_Read_first_loop	; Don't ack before reading the first byte

i2c_Read_loop
	pscall	i2c_read_ack		; ACK the last byte read

i2c_Read_first_loop
	pscall	i2c_read_byte		; Read the byte
	banksel	i2c_byte		; Store it to our current pointer
	movf	i2c_byte, w
	movwf	INDF

	i2c_buffer_loop i2c_Read_loop	; Next...

	pscall	i2c_read_nak		; NAK the last byte read

i2c_Read_done
	pscall	i2c_stop
	return

	end

;### The End ###
