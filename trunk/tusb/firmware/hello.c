#include <tusb.h>

/* GPIO allocation on the Keyspan USA-19HS board */
#define LED          P3_0
#define RS232_POWER  P3_1

void watchdog_reset() {
  WDCSR |= WDT;
}

void delay(unsigned int i) {
  unsigned int j;
  for (j=0;j<i;j++)
    watchdog_reset();
}

void usb_init() {
  FUNADR = 0;       /* We haven't been assigned an address yet */

  USBCTL &= ~CONT;  /* Disconnect from USB then reconnect */
  delay(60000);
  USBCTL |= CONT;

  /* Set up endpoint 0 */
  IEPCNFG_0 = STALL | USBIE | UBME;
  OEPCNFG_0 = STALL | USBIE | UBME;
}

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

void uart_write(char *s) {
  while (*s) {
    LED = 0;
    while (!(LSR & LSR_TxE))
      watchdog_reset();
    LED = 1;
    TDR = *s;
    s++;
  }
}

void main() {
  EA = 0;           /* Global interrupt disable */

  uart_init();

  while (1) {
    uart_write("\n\rHello World!\n\r");
    delay(20000);
  }

#if 0
  usb_init();

  while (1) {

    switch (VECINT) {

    case 0:
      LED = 0;
      break;
    }

    watchdog_reset();
  }
#endif
}

