;//################################################################################
;//
;// rcpod_protocol.h - Definitions describing the host to device protocol used by the
;//                    rcpod This file is both valid C and assembly source, so it can be
;//                    shared by host and device code.
;//
;// This file is part of the rcpod project.
;// Micah Dowty <micah@picogui.org>
;//
;//###############################################################################

#ifndef __RCPOD_PROTOCOL_H
#define __RCPOD_PROTOCOL_H

;//************************************************** Idenfification

;// The protocol version, stored in binary coded decimal.
;// This is available from the device in the bcdDevice field
;// of its DEVICE descriptor.
#define RCPOD_PROTOCOL_VERSION  0x0102

;// Device vendor and product IDs.
;// The RCPOD's device class and protocol are both set to 'vendor-specific'.
#define RCPOD_VENDOR_ID   0xE461
#define RCPOD_PRODUCT_ID  0x0002

;//************************************************** Control requests

;//------------------ Peek/poke

;// Write an 8-bit value (wValue) to a 9-bit address in RAM (wIndex)
#define RCPOD_CTRL_POKE		0x01

;// Read an 8-bit value from the given 9-bit RAM address (wIndex), return
;// it in a 1-byte data packet.
#define RCPOD_CTRL_PEEK		0x02

;//------------------ Analog

;// Read all analog inputs at full precision, returns an 8-byte packet
#define RCPOD_CTRL_ANALOG_ALL   0x10

;//------------------ Serial

;// Using current USART settings, perform an optional transmit and optional receive
;// using a buffer at the address in wIndex. The number of bytes specified in the
;// low byte of wValue are transmitted, then from the beginning of the same buffer
;// an asynchronous receive is started. It runs in the background, until the number
;// of bytes in the high byte of wValue have been received or UART_RX_END is called.
;// Either byte of wValue can be zero to perform only a transmit or only a receive.
#define RCPOD_CTRL_USART_TXRX   0x20

;// Cancels an ongoing USART receive
#define RCPOD_CTRL_USART_RX_END 0x21

;// Sets the pin to use as a USART transmit enable. When transmitting, this pin is
;// pulled high, then after the transmission is complete it is pulled low.
;// This feature is disabled by setting the pin to zero.
#define RCPOD_CTRL_USART_TXE    0x23

;//------------------ Memory management

;// Returns information about the memory available for use by user programs, as buffers
;// for USART activity and such. wIndex and wValue are ignored, returns a packet
;// with two bytes (in little endian) specifying the start address of the heap and
;// two bytes specifying the size of the heap.
#define RCPOD_CTRL_GET_HEAP     0x30

;//------------------ General purpose I/O

;// Make the pin indicated by wIndex an output
#define RCPOD_CTRL_PIN_OUTPUT   0x40

;// Make the pin indicated by wIndex an input
#define RCPOD_CTRL_PIN_INPUT    0x41

;// Make the pin indicated by wIndex an output and pull it high
#define RCPOD_CTRL_PIN_HIGH     0x42

;// Make the pin indicated by wIndex an output and pull it low
#define RCPOD_CTRL_PIN_LOW      0x43

;// make the pin indicated by wIndex an input and return its status
#define RCPOD_CTRL_PIN_READ     0x44


#endif

;//### The End ###

