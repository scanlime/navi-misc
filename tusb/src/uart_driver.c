#include <tusb.h>
#include "uart_driver.h"
#include "util.h"


void uart_init()
{
  RS232_POWER = 0;            /* Turn on the RS-232 hardware */
  UART_MASK = 0;              /* No interrupts */
  MODECNFG = TXCNTL | SOFTSW; /* Transmitter on */
  MCR  = MCR_USRT;            /* UART reset */
  MCR  = 0;                   /* UART out of reset */
  LCR  = 0;                   /* FIFO reset */
  LCR  = LCR_WL8 | LCR_FEN;   /* 8-N-1, enable FIFOs */
  FCRL = 0;                   /* No flow control */
  MSR  = 0xFF;                /* Reset modem status */
  DLH  = 0;                   /* 115200 baud default speed */
  DLL  = 8;
}

void uart_set_speed(int divisor)
{
  DLH = divisor >> 8;
  DLL = divisor & 0xFF;
}

void uart_write(char c)
{
  while (!(LSR & LSR_TxE))
    watchdog_reset();
  TDR = c;
}

char uart_read()
{
  while (!(LSR & LSR_RxF))
    watchdog_reset();
  return RDR;
}

int uart_nb_write(char c)
{
  if (!(LSR & LSR_TxE))
    return -1;
  TDR = c;
  return 0;
}

int uart_nb_read()
{
  if (!(LSR & LSR_RxF))
    return -1;
  return RDR;
}

void putchar(char c) {
  uart_write(c);

  /* Convert \n into \n\r */
  if (c == '\n')
    uart_write('\r');
}
