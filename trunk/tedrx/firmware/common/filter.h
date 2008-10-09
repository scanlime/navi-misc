#ifndef _TEDRX_FILTER_H
#define _TEDRX_FILTER_H

void filter_init(void);
uint8_t filter_rx(uint8_t *buffer, uint8_t length);

#endif /* _TEDRX_FILTER_H */
