#include <tusb.h>
#include "uart_driver.h"
#include "util.h"

/* GPIO allocation on the Keyspan USA-19HS board */
#define LED          P3_0
#define RS232_POWER  P3_1


void uart_init() {
  RS232_POWER = 0;            /* Turn on the RS-232 hardware */
  MODECNFG = TXCNTL | SOFTSW; /* Transmitter on */
  MCR  = MCR_USRT;            /* UART reset */
  MCR  = 0;                   /* UART out of reset */
  LCR  = 0;                   /* FIFO reset */
  LCR  = LCR_WL8 | LCR_FEN;   /* 8-N-1, enable FIFOs */
  FCRL = 0;                   /* No flow control */
  MSR  = 0xFF;                /* Reset modem status */
  DLH  = 0;                   /* 115200 baud */
  DLL  = 8;
}

void putchar(char c) {
  while (!(LSR & LSR_TxE))
    watchdog_reset();
  TDR = c;

  /* Convert \n into \n\r */
  if (c == '\n')
    putchar('\r');
}
