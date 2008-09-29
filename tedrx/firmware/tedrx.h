#ifndef _TEDRX_H
#define _TEDRX_H

/* Pins */
#define SERIAL_OUT_PIN   PINB4         /* RS-232 output */
#define ASK_PIN          PINB0         /* Filtered and amplified ASK modulated data. */
#define DEBUG_PIN        PINB4         /* Optional debug pin */

/* Optional debug features: Only used if DEBUG_PIN is set */
//#define DEBUG_TMR0
#define DEBUG_BASEBAND
//#define DEBUG_RX_BITS

/*
 * Serial port.
 */
#define SERIAL_BAUD        9600
#define SERIAL_INVERT      0

void serial_init(void);

void filter_init(void);
void filter_rx(uint8_t *buffer, uint8_t length);

#endif /* _TEDRX_H */
