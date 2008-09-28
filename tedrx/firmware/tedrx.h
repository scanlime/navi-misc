#ifndef _TEDRX_H
#define _TEDRX_H

/* Pins */
#define SERIAL_OUT_PIN   PINB4         /* RS-232 output */
#define ASK_PIN          PINB0         /* Filtered and amplified ASK modulated data. */
#define MAINS_PIN        PINB3         /* Mains signal, for phase detection */
#define DEBUG_PIN        PINB4         /* Optional debug pin */

/* Optional debug features: Only used if DEBUG_PIN is set */
//#define DEBUG_TMR0
//#define DEBUG_BASEBAND
//#define DEBUG_RX_BITS
#define DEBUG_PLL

/*
 * Serial port.
 * Warning: The serial baud timer is also used as the mains PLL sampling rate.
 *          Change it only if you understand how to test and tune the PLL.
 */
#define OUTPUT_BAUD      9600
#define INVERT_OUTPUT    0

void serial_init(void);

void filter_init(void);

void pll_update(void);

#endif /* _TEDRX_H */
