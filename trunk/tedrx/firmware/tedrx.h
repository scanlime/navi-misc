#ifndef _TEDRX_H
#define _TEDRX_H

/**** Hardware definitions ****/

#define SERIAL_OUT_PIN   PINB4         /* RS-232 output */
#define ASK_PIN          PINB0         /* Filtered and amplified ASK modulated data. */
#define MAINS_PIN        PINB5         /* Mains signal, for phase detection */
//#define DEBUG_PIN      PINB4         /* Optional debug pin */

/**** Optional debug features: Only used if DEBUG_PIN is set ****/

//#define DEBUG_TMR0
//#define DEBUG_BASEBAND
#define DEBUG_RX_BITS

/**** serial.c ****/

void serial_init(void);

/**** filter.c ****/

void filter_init(void);
void filter_rx(uint8_t *buffer, uint8_t length);

#endif /* _TEDRX_H */
