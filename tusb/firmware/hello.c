#include <tusb.h>
#include <stdio.h>

/* GPIO allocation on the Keyspan USA-19HS board */
#define LED          P3_0
#define RS232_POWER  P3_1

xdata at 0xFF00 unsigned char setupPacket[8];

void watchdog_reset() {
  WDCSR |= WDT;
}

void delay(unsigned int i) {
  unsigned int j;
  for (j=0;j<i;j++)
    watchdog_reset();
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

void putchar(char c) {
  while (!(LSR & LSR_TxE))
    watchdog_reset();
  TDR = c;

  /* Convert \n into \n\r */
  if (c == '\n')
    putchar('\r');
}

void usb_init() {
  FUNADR = 0;       /* We haven't been assigned an address yet */

  /* Set up endpoint 0 */
  IEPCNFG_0 = STALL | USBIE | UBME;
  OEPCNFG_0 = STALL | USBIE | UBME;

  USBCTL &= ~CONT;  /* Disconnect from USB */
  USBSTA = 0xFF;    /* Clear USB status bits */
  delay(60000);     /* Wait a bit and reconnect */
  USBCTL |= CONT;
}

void usb_loop() {
  unsigned char status;
  int i;

  while (1) {
    status = USBSTA;     /* Read all status bits and clear them all */
    USBSTA = 0xFF;

    if (status & RSTR) {
      puts("Function reset request");
    }

    if (status & SUSR) {
      puts("Function suspend request");
    }

    if (status & RESR) {
      puts("Function resume request");
    }

    if (status & SETUP) {
      puts("SETUP received");
      for (i=0; i<8; i++)
	printf(" %02X", setupPacket[i]);
      printf("\n");
    }

    if (status & STPOW) {
      puts("SETUP overwrite");
    }

    delay(20000);
  }
}

void main() {
  EA = 0;           /* Global interrupt disable */

  uart_init();
  puts("\n---- Startup ----");

  usb_init();
  puts("USB initialized");

  usb_loop();
}

