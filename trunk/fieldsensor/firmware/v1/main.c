/*
 * SDCC source for the electric field sensor box
 * Copyright (C) 2003  Micah Dowty <micah@navi.picogui.org>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 *
 * --------------------------------------------------------------------------
 *
 * This box has essentially the same transmit and receive
 * hardware as the LazyFish. There are 6 electrodes arranged
 * under the lid of the project's enclosure. A PIC16F877
 * microcontroller was chosen, for easy in-circuit programming
 * using the WLoader bootloader firmware.
 *
 * Hardware connections:
 * 
 *   RB0-3:	Front panel LEDs, active high
 *   RB4-7:	Transmit electrodes
 *   RA0-1:	Receive electrodes
 *   RE2:	WLoader serial transmit and receive
 * 
 *   20MHz crystal oscillator-
 *     The serial and sensor modulation/demodulation code has been tuned
 *     for this frequency.
 *
 *   USB port is currently only used for power. It
 *   is planned to eventually add a separate USB chip,
 *   or use a PIC with built-in USB.
 * 
 * --------------------------------------------------------------------------
 */

#define __16F877
#include "p16f877.h"

#define NOP \
   _asm     \
   nop      \
   _endasm; \

/* Globals */
BIT_AT(PORTE_ADDR,2) SerialPort;
BIT_AT(TRISE_ADDR,2) SerialTris;
BIT_AT(PORTB_ADDR,2) SerialTris;
int reading0, reading1, reading2, reading3, reading4, reading5, reading6, reading7;
unsigned char portbBuffer;

/* Number of half-cycles used to excite the resonator */
unsigned char numResonationHalfcycles = 10;      

/* Number of samples to integrate-
 *   larger numbers give less jittery results, but at a lower rate.
 */
unsigned char numIntegrationSamples = 8;      

/* mask values for each transmit channel */
#define Transmit0   0x10
#define Transmit1   0x20
#define Transmit2   0x40
#define Transmit3   0x80

/* ADCON0 values for each receive channel */
#define Receive0   0x81
#define Receive1   0x89

/* Debugging pins */
BIT_AT(PORTD_ADDR,7) debugSync;
BIT_AT(PORTD_ADDR,6) debugSample;

/* Functions */
void txInterrupt(void) interrupt 0;
void initialize(void);
void readADC(unsigned char channel);
void serialBitDelay(void);
void putc(unsigned char byte);
unsigned char getc(void);
void sendReading(int r);
int integrateSample(unsigned char transmitter, unsigned char receiver);
int sensorSample(unsigned char transmitter, unsigned char receiver, unsigned char phase);
void sampleAll(void);
void sendPacket(void);
 

/****************************************************** Main program ******/

void main(void) {
  initialize();

  /* Just send out packets continuously */
  while (1) {
    sampleAll();
    sendPacket();
  } 
}

/****************************************************** Utilities ******/

void initialize(void) {
  /* Set port directions */
  TRISA = 0xFF;
  TRISB = 0x00;
  TRISC = 0x00;
  TRISD = 0x00;
  TRISE = 0x04;

  /* Clear port outputs */
  PORTA = 0x00;
  PORTB = 0x00;
  PORTC = 0x00;
  PORTD = 0x00;
  PORTE = 0x00;

  portbBuffer = 1;

  /* ADC init */
  ADCON1 = 0x82;
  ADCON0 = 0x81;
  GIE = 1;
}


/****************************************************** Sensor I/O ******/

/* Sample all transmitter-receiver pairs */
void sampleAll(void) {
  reading0 = integrateSample(Transmit0, Receive0);
  reading1 = integrateSample(Transmit1, Receive0);
  reading2 = integrateSample(Transmit2, Receive0);
  debugSync = 1;
  reading3 = integrateSample(Transmit3, Receive0);
  debugSync = 0;
  reading4 = integrateSample(Transmit0, Receive1);
  reading5 = integrateSample(Transmit1, Receive1);
  reading6 = integrateSample(Transmit2, Receive1);
  reading7 = integrateSample(Transmit3, Receive1);
}

/* Demodulate and integrate many samples between one transmitter and one receiver */
int integrateSample(unsigned char transmitter, unsigned char receiver) {
  signed int i=0, q=0;
  int numsamples = numIntegrationSamples;

  do {
    /* Take four synchronous samples, updating in-phase and quadrature accumulators */
    i += sensorSample(transmitter, receiver, 0);
    q += sensorSample(transmitter, receiver, 1);
    i -= sensorSample(transmitter, receiver, 2);
    q -= sensorSample(transmitter, receiver, 3);
  } while (--numsamples);

  /* Approximate the length of the vector (i,q) */
  if (i<0) i = -i;
  if (q<0) q = -q;
  return i+q;
}

/* Take a single measurement between one transmitter and one receiver, at one phase */
int sensorSample(unsigned char transmitter, unsigned char receiver, unsigned char phase) {
  
  bit phase0, phase1, phase2, phase3;
  unsigned char halfcycles;
  unsigned char h,l;

  portbBuffer &= 0x0F;      /* Disable all transmitters */  
  TRISB = 0x00;             /* I -think- we want the other plates to be ground planes now,
			     * rather than leaving them floating at high impedence.
			     */
  ADCON0 = receiver;        /* Switch the ADC to this channel */
  
  /* Set phase bits here to save time later */
  if (phase == 0) phase0 = 1; else phase0 = 0;
  if (phase == 1) phase1 = 1; else phase1 = 0;
  if (phase == 2) phase2 = 1; else phase2 = 0;
  if (phase == 3) phase3 = 1; else phase3 = 0;

  /* First we need to ring up the transmitter LC tank-
   * At 20Mhz, our 100KHz RF wave will need a period of 50 CPU cycles.
   * There should be exactly 25 CPU cycles per loop.
   */
  halfcycles = numResonationHalfcycles;
  do {
    portbBuffer ^= transmitter;
    PORTB = portbBuffer;
    NOP;
    NOP NOP NOP NOP NOP;
    NOP NOP NOP NOP NOP;
    NOP NOP NOP NOP NOP;
    NOP NOP;
  } while (--halfcycles);

  /* Now the phase controls the position of our ADC sample within the wave. */
  debugSample = 1;
  if (phase0)
    GO_DONE = 1;
  debugSample = 0;
  NOP NOP NOP NOP NOP;
  NOP NOP NOP NOP NOP;
  debugSample = 1;
  if (phase1)
    GO_DONE = 1;
  debugSample = 0;
  NOP NOP NOP NOP NOP;
  NOP NOP NOP NOP NOP NOP;
  debugSample = 1;
  if (phase2)
    GO_DONE = 1;
  debugSample = 0;
  NOP NOP NOP NOP NOP;
  NOP NOP NOP NOP NOP;
  debugSample = 1;
  if (phase3)
    GO_DONE = 1;
  debugSample = 0;

  /* Wait for the ADC to do its magic */
  while (NOT_DONE);

  /* Get our resonator to stop resonating */
  TRISB = 0xF0;

  h = ADRESH;
  l = ADRESL;
  return (((int)h)<<8) | l;
}


/****************************************************** Serial Port ******/

/* NOTE: The PIC16F877 has a built-in USART which it would make a lot of
 *       sense to use here, but we're bit-banging instead so the hardware can
 *       be shared by this app and WLoader. This also supports migrating this
 *       code to other PICs without a USART.
 */

/* 19200bps at 20MHz */
void serialBitDelay(void) {
  unsigned char i = 64;
  do {
    NOP;
  } while (--i);
}

void halfSerialBitDelay(void) {
  unsigned char i = 32;
  do {
    NOP;
  } while (--i);
}

void putc(unsigned char byte) {
  unsigned char bitnum;

  /* Start bit */
  SerialPort = 0;
  SerialTris = 0;
  serialBitDelay();

  /* Shift out 8 bits, LSB first */
  bitnum = 8;
  do {
    if (byte & 1)
      SerialPort = 1;
    else
      SerialPort = 0;
    byte >>= 1;
    serialBitDelay();
  } while (--bitnum);
  
  /* Stop bit */
  SerialPort = 1;
  serialBitDelay();
  SerialTris = 1;
}

void sendPacket(void) {
  int i;
  unsigned char h,l,checksum = 0;

  /* Synchronization byte */
  putc(0x80);

  h = reading0 >> 8;
  l = reading0;
  putc(h);
  putc(l);
  checksum += h + l;

  h = reading1 >> 8;
  l = reading1;
  putc(h);
  putc(l);
  checksum += h + l;

  h = reading2 >> 8;
  l = reading2;
  putc(h);
  putc(l);
  checksum += h + l;

  h = reading3 >> 8;
  l = reading3;
  putc(h);
  putc(l);
  checksum += h + l;

  h = reading4 >> 8;
  l = reading4;
  putc(h);
  putc(l);
  checksum += h + l;

  h = reading5 >> 8;
  l = reading5;
  putc(h);
  putc(l);
  checksum += h + l;

  h = reading6 >> 8;
  l = reading6;
  putc(h);
  putc(l);
  checksum += h + l;

  h = reading7 >> 8;
  l = reading7;
  putc(h);
  putc(l);
  checksum += h + l;

  /* Checksum byte */
  putc(checksum);
}

/* The End */
