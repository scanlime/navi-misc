/*-------------------------------------------------------------------------
   Register Declarations for the Texas Instruments TUSB3210/3410 Processors

   TUSB3410 definitions added by Micah Dowty / micah@navi.cx (May 2004)
   Written By -  Michael Petruzelka / michael@root.at (December 2001)
   based on 8051.h from Bela Torok / bela.torok@kssg.ch (July 2000)

   This program is free software; you can redistribute it and/or modify it
   under the terms of the GNU General Public License as published by the
   Free Software Foundation; either version 2, or (at your option) any
   later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.

   In other words, you are welcome to use, share and improve this program.
   You are forbidden to forbid anyone else to use, share and improve
   what you give them.   Help stamp out software-hoarding!
-------------------------------------------------------------------------*/

#ifndef REGTUSB_H
#define REGTUSB_H

/*  BYTE Register  */
sfr at 0x80 P0   ;
sfr at 0x81 SP   ;
sfr at 0x82 DPL  ;
sfr at 0x83 DPH  ;
sfr at 0x87 PCON ;
sfr at 0x88 TCON ;
sfr at 0x89 TMOD ;
sfr at 0x8A TL0  ;
sfr at 0x8B TL1  ;
sfr at 0x8C TH0  ;
sfr at 0x8D TH1  ;
sfr at 0x90 P1   ;
sfr at 0x98 SCON ;
sfr at 0x99 SBUF ;
sfr at 0xA0 P2   ;
sfr at 0xA8 IE   ;
sfr at 0xB0 P3   ;
sfr at 0xB8 IP   ;
sfr at 0xD0 PSW  ;
sfr at 0xE0 ACC  ;
sfr at 0xF0 B    ;


/*  BIT Register  */
/* P0 */
sbit at 0x80 P0_0 ;
sbit at 0x81 P0_1 ;
sbit at 0x82 P0_2 ;
sbit at 0x83 P0_3 ;
sbit at 0x84 P0_4 ;
sbit at 0x85 P0_5 ;
sbit at 0x86 P0_6 ;
sbit at 0x87 P0_7 ;

/*  TCON  */
sbit at 0x88 IT0  ;
sbit at 0x89 IE0  ;
sbit at 0x8A IT1  ;
sbit at 0x8B IE1  ;
sbit at 0x8C TR0  ;
sbit at 0x8D TF0  ;
sbit at 0x8E TR1  ;
sbit at 0x8F TF1  ;

/* P1 */
sbit at 0x90 P1_0 ;
sbit at 0x91 P1_1 ;
sbit at 0x92 P1_2 ;
sbit at 0x93 P1_3 ;
sbit at 0x94 P1_4 ;
sbit at 0x95 P1_5 ;
sbit at 0x96 P1_6 ;
sbit at 0x97 P1_7 ;

/*  SCON  */
sbit at 0x98 RI   ;
sbit at 0x99 TI   ;
sbit at 0x9A RB8  ;
sbit at 0x9B TB8  ;
sbit at 0x9C REN  ;
sbit at 0x9D SM2  ;
sbit at 0x9E SM1  ;
sbit at 0x9F SM0  ;

/* P2 */
sbit at 0xA0 P2_0 ;
sbit at 0xA1 P2_1 ;
sbit at 0xA2 P2_2 ;
sbit at 0xA3 P2_3 ;
sbit at 0xA4 P2_4 ;
sbit at 0xA5 P2_5 ;
sbit at 0xA6 P2_6 ;
sbit at 0xA7 P2_7 ;

/*  IE   */
sbit at 0xA8 EX0  ;
sbit at 0xA9 ET0  ;
sbit at 0xAA EX1  ;
sbit at 0xAB ET1  ;
sbit at 0xAC ES   ;
sbit at 0xAF EA   ;

/*  P3  */
sbit at 0xB0 P3_0 ;
sbit at 0xB1 P3_1 ;
sbit at 0xB2 P3_2 ;
sbit at 0xB3 P3_3 ;
sbit at 0xB4 P3_4 ;
sbit at 0xB5 P3_5 ;
sbit at 0xB6 P3_6 ;
sbit at 0xB7 P3_7 ;

sbit at 0xB0 RXD  ;
sbit at 0xB1 TXD  ;
sbit at 0xB2 INT0 ;
sbit at 0xB3 INT1 ;
sbit at 0xB4 T0   ;
sbit at 0xB5 T1   ;
sbit at 0xB6 WR   ;
sbit at 0xB7 RD   ;

/*  IP   */
sbit at 0xB8 PX0  ;
sbit at 0xB9 PT0  ;
sbit at 0xBA PX1  ;
sbit at 0xBB PT1  ;
sbit at 0xBC PS   ;

/*  PSW   */
sbit at 0xD0 P    ;
sbit at 0xD1 F1   ;
sbit at 0xD2 OV   ;
sbit at 0xD3 RS0  ;
sbit at 0xD4 RS1  ;
sbit at 0xD5 F0   ;
sbit at 0xD6 AC   ;
sbit at 0xD7 CY   ;

/* BIT definitions for bits that are not directly accessible */
/* PCON bits */
#define IDL             0x01
#define PD              0x02
#define GF0             0x04
#define GF1             0x08
#define SMOD            0x80

/* TMOD bits */
#define T0_M0           0x01
#define T0_M1           0x02
#define T0_CT           0x04
#define T0_GATE         0x08
#define T1_M0           0x10
#define T1_M1           0x20
#define T1_CT           0x40
#define T1_GATE         0x80

#define T0_MASK         0x0F
#define T1_MASK         0xF0

/* Interrupt vectors */
#define VECTOR_NONE         0x00
#define VECTOR_OEP1         0x12
#define VECTOR_OEP2         0x14
#define VECTOR_OEP3         0x16
#define VECTOR_IEP1         0x22
#define VECTOR_IEP2         0x24
#define VECTOR_IEP3         0x26
#define VECTOR_STPOW        0x30
#define VECTOR_SETUP        0x32
#define VECTOR_RESR         0x38
#define VECTOR_SUSR         0x3A
#define VECTOR_RSTR         0x3C
#define VECTOR_I2C_TXE      0x40
#define VECTOR_I2C_RXF      0x42
#define VECTOR_IEP0         0x44
#define VECTOR_OEP0         0x46
#define VECTOR_UART_STATUS  0x50
#define VECTOR_UART_MODEM   0x52
#define VECTOR_UART_RXF     0x60
#define VECTOR_UART_TXE     0x62
#define VECTOR_DMA1         0x80
#define VECTOR_DMA3         0x84


/* Memory mapped USB control registers */
/* Output 1 Endpoint Descriptor Block */
volatile xdata at 0xFF08 unsigned char OEPCNF_1;         /* Output endpoint 1 configuration */
volatile xdata at 0xFF09 unsigned char OEPBBAX_1;        /* Output endpoint 1 X buffer base address */
volatile xdata at 0xFF0A unsigned char OEPBCTX_1;        /* Output endpoint 1 X byte count */
volatile xdata at 0xFF0D unsigned char OEPBBAY_1;        /* Output endpoint 1 Y buffer base address */
volatile xdata at 0xFF0E unsigned char OEPBCTY_1;        /* Output endpoint 1 Y byte count */
volatile xdata at 0xFF0F unsigned char OEPSIZXY_1;       /* Output endpoint 1 X/Y buffer size */

/* Output 2 Endpoint Descriptor Block */
volatile xdata at 0xFF10 unsigned char OEPCNF_2;         /* Output endpoint 2 configuration */
volatile xdata at 0xFF11 unsigned char OEPBBAX_2;        /* Output endpoint 2 X buffer base address */
volatile xdata at 0xFF12 unsigned char OEPBCTX_2;        /* Output endpoint 2 X byte count */
volatile xdata at 0xFF15 unsigned char OEPBBAY_2;        /* Output endpoint 2 Y buffer base address */
volatile xdata at 0xFF16 unsigned char OEPBCTY_2;        /* Output endpoint 2 Y byte count */
volatile xdata at 0xFF17 unsigned char OEPSIZXY_2;       /* Output endpoint 2 X/Y buffer size */

/* Output 3 Endpoint Descriptor Block */
volatile xdata at 0xFF18 unsigned char OEPCNF_3;         /* Output endpoint 2 configuration */
volatile xdata at 0xFF19 unsigned char OEPBBAX_3;        /* Output endpoint 2 X buffer base address */
volatile xdata at 0xFF1A unsigned char OEPBCTX_3;        /* Output endpoint 2 X byte count */
volatile xdata at 0xFF1D unsigned char OEPBBAY_3;        /* Output endpoint 2 Y buffer base address */
volatile xdata at 0xFF1E unsigned char OEPBCTY_3;        /* Output endpoint 2 Y byte count */
volatile xdata at 0xFF1F unsigned char OEPSIZXY_3;       /* Output endpoint 2 X/Y buffer size */

/* Input 1 Endpoint Descriptor Block */
volatile xdata at 0xFF48 unsigned char IEPCNF_1;         /* Output endpoint 1 configuration */
volatile xdata at 0xFF49 unsigned char IEPBBAX_1;        /* Output endpoint 1 X buffer base address */
volatile xdata at 0xFF4A unsigned char IEPBCTX_1;        /* Output endpoint 1 X byte count */
volatile xdata at 0xFF4D unsigned char IEPBBAY_1;        /* Output endpoint 1 Y buffer base address */
volatile xdata at 0xFF4E unsigned char IEPBCTY_1;        /* Output endpoint 1 Y byte count */
volatile xdata at 0xFF4F unsigned char IEPSIZXY_1;       /* Output endpoint 1 X/Y buffer size */

/* Input 2 Endpoint Descriptor Block */
volatile xdata at 0xFF50 unsigned char IEPCNF_2;         /* Output endpoint 2 configuration */
volatile xdata at 0xFF51 unsigned char IEPBBAX_2;        /* Output endpoint 2 X buffer base address */
volatile xdata at 0xFF52 unsigned char IEPBCTX_2;        /* Output endpoint 2 X byte count */
volatile xdata at 0xFF55 unsigned char IEPBBAY_2;        /* Output endpoint 2 Y buffer base address */
volatile xdata at 0xFF56 unsigned char IEPBCTY_2;        /* Output endpoint 2 Y byte count */
volatile xdata at 0xFF57 unsigned char IEPSIZXY_2;       /* Output endpoint 2 X/Y buffer size */

/* Input 3 Endpoint Descriptor Block */
volatile xdata at 0xFF58 unsigned char IEPCNF_3;         /* Output endpoint 3 configuration */
volatile xdata at 0xFF59 unsigned char IEPBBAX_3;        /* Output endpoint 3 X buffer base address */
volatile xdata at 0xFF5A unsigned char IEPBCTX_3;        /* Output endpoint 3 X byte count */
volatile xdata at 0xFF5D unsigned char IEPBBAY_3;        /* Output endpoint 3 Y buffer base address */
volatile xdata at 0xFF5E unsigned char IEPBCTY_3;        /* Output endpoint 3 Y byte count */
volatile xdata at 0xFF5F unsigned char IEPSIZXY_3;       /* Output endpoint 3 X/Y buffer size */

/* Endpoint Configuration Bits */
        #define USBIE    (1<<2)                 /* USB interrupt enable */
        #define STALL    (1<<3)                 /* Stall indicator bit */
        #define DBUF     (1<<4)                 /* Double buffer enable */
        #define TOGLE    (1<<5)                 /* Data0,Data1 toggle status */
        #define ISO      (1<<6)                 /* =0 Only non Isochronous transfer supported */
        #define UBME     (1<<7)                 /* UBM enable */

/* Memory Mapped Registers */
volatile xdata at 0xFF80 unsigned char IEPCNFG_0;        /* Input endpoint-0 configuration register */
volatile xdata at 0xFF81 unsigned char IEPBCNT_0;        /* Input endpoint-0 byte count register */
volatile xdata at 0xFF82 unsigned char OEPCNFG_0;        /* Output endpoint-0 configuration register */
volatile xdata at 0xFF83 unsigned char OEPBCNT_0;        /* Output endpoint-0 byte count register */
volatile xdata at 0xFF84 unsigned char INTCNF;           /* Interrupt delay configuration register */
volatile xdata at 0xFF90 unsigned char MCNFG;            /* MCU configuration register */
        #define SDW      (1<<0)                 /* Boot Rom/RAM selector */
        #define XINT     (1<<6)                 /* Int 1 source */
        #define OSC1248  (1<<7)                 /* Pll selector */
volatile xdata at 0xFF92 unsigned char VECINT;           /* Vector interrupt register */
volatile xdata at 0xFF93 unsigned char WDCSR;            /* Watchdog timer, control & status register */
        #define WDT      (1<<0)                 /* Watchdog retrigger bit */
        #define WDR      (1<<6)                 /* Watchdog reset indicator */
        #define WDE      (1<<7)                 /* Watchdog enable bit */
volatile xdata at 0xFF94 unsigned char PUR0;             /* Port 0 pullup resistor register */
volatile xdata at 0xFF95 unsigned char PUR1;             /* Port 1 pullup resistor register */
volatile xdata at 0xFF96 unsigned char PUR2;             /* Port 2 pullup resistor register */
volatile xdata at 0xFF97 unsigned char PUR3;             /* Port 3 pullup resistor register */
volatile xdata at 0xFFF0 unsigned char I2CSTA;           /* I2C status and control register */
        #define SWR      (1<<0)                 /* Stop write condition */
        #define SRD      (1<<1)                 /* Stop read condition */
        #define TIE      (1<<2)                 /* Transmitter empty interrupt */
        #define TXE      (1<<3)                 /* Transmitter not full indicator */
        #define S1_4     (1<<4)                 /* Bus speed selection */
        #define ERR      (1<<5)                 /* Bus error condition */
        #define RIE      (1<<6)                 /* Receiver ready interrupt enable */
        #define RXF      (1<<7)                 /* Receiver not empty indicator */
volatile xdata at 0xFFF1 unsigned char I2CDAO;           /* I2C data-output register */
volatile xdata at 0xFFF2 unsigned char I2CDAI;           /* I2C data-input register */
volatile xdata at 0xFFF3 unsigned char I2CADR;           /* I2C adress register */
volatile xdata at 0xFFF6 unsigned char VIDSTA;           /* VID/PID status register */
volatile xdata at 0xFFFB unsigned char MODECNFG;         /* Mode configuration register */
        #define TXCNTL   (1<<0)                 /* Firmware tx switching enabled */
        #define SOFTSW   (1<<1)                 /* TX buffer enable */
        #define CLKOUTEN (1<<2)                 /* Enable clock output */
        #define CLKSLCT  (1<<3)                 /* 0=UART baud clock is output, 1=3.556MHz clock is output */
volatile xdata at 0xFFFC unsigned char USBCTL;           /* USB control register */
        #define DIR      (1<<0)                 /* Data direction */
        #define SIR      (1<<1)                 /* Setup interrupt status */
        #define BUS_SELF (1<<2)                 /* Bus / Self powered */
        #define RWE      (1<<3)                 /* Remote wake up enable */
        #define FRSTE    (1<<4)                 /* Function reset connection bit */
        #define RWUP     (1<<5)                 /* Device remote wakeup request */
        #define CONT     (1<<7)                 /* Upstream port connect bit */
volatile xdata at 0xFFFD unsigned char USBMSK;           /* USB interrupt mask register */
volatile xdata at 0xFFFE unsigned char USBSTA;           /* USB status register */
        #define STPOW    (1<<0)                 /* Setup overwrite */
        #define WAKEUP   (1<<1)                 /* Remote wakeup request */
        #define SETUP    (1<<2)                 /* Setup transaction received */
        #define PWON     (1<<3)                 /* Power on request */
        #define PWOFF    (1<<4)                 /* Power off request */
        #define RESR     (1<<5)                 /* Function resume */
        #define SUSR     (1<<6)                 /* Function suspend request */
        #define RSTR     (1<<7)                 /* Function reset request */
volatile xdata at 0xFFFF unsigned char FUNADR;           /* Function adress register */

/*  UART registers, only present on the TUSB3410  */
volatile xdata at 0xFFA0 unsigned char RDR;              /* Receiver data register  */
volatile xdata at 0xFFA1 unsigned char TDR;              /* Transmitter data register  */
volatile xdata at 0xFFA2 unsigned char LCR;              /* Line control register  */
        #define LCR_WL5      0                  /*  5 bit words  */
        #define LCR_WL6      1                  /*  6 bit words  */
        #define LCR_WL7      2                  /*  7 bit words  */
        #define LCR_WL8      3                  /*  8 bit words  */
        #define LCR_STP      (1<<2)             /* 1.5 or 2 stop bits  */
        #define LCR_PRTY     (1<<3)             /* Generate parity  */
        #define LCR_EPRTY    (1<<4)             /* Even parity  */
        #define LCR_FPTY     (1<<5)             /* Forced parity  */
        #define LCR_BRK      (1<<6)             /* Break condition  */
        #define LCR_FEN      (1<<7)             /* FIFO enabled  */
volatile xdata at 0xFFA3 unsigned char FCRL;             /* Flow control register  */
        #define FCR_TXOF     (1<<0)             /* Enable transmitter Xon/Xoff flow control */
        #define FCR_TXOA     (1<<1)             /* Enable transmitter Xon-on-any/Xoff flow control*/
        #define FCR_CTS      (1<<2)             /* Enable transmitter /CTS flow control */
        #define FCR_DSR      (1<<3)             /* Enable transmitter /DSR flow control */
        #define FCR_RXOF     (1<<4)             /* Enable receiver Xon/Xoff flow control */
        #define FCR_RTS      (1<<5)             /* Enable receiver /RTS flow control */
        #define FCR_DTR      (1<<6)             /* Enable receiver /DTR flow control */
        #define FCR_485E     (1<<7)             /* RS-485 enable bit */
volatile xdata at 0xFFA4 unsigned char MCR;              /* Modem control register  */
        #define MCR_USRT     (1<<0)             /* UART soft reset */
        #define MCR_RCVE     (1<<1)             /* Receiver enable mode, used only for RS-485 */
        #define MCR_LOOP     (1<<2)             /* Loopback mode enable */
        #define MCR_DTR      (1<<4)             /* Manual DTR output */
        #define MCR_RTS      (1<<5)             /* Manual RTS output */
        #define MCR_LRI      (1<<6)             /* Ring output, for loopback mode */
        #define MCR_LCD      (1<<7)             /* Carrier Detect output, for loopback mode */
volatile xdata at 0xFFA5 unsigned char LSR;              /* Line status register */
        #define LSR_OVR      (1<<0)             /* A receiver overrun has occurred */
        #define LSR_PTE      (1<<1)             /* A parity error was detected */
        #define LSR_FRE      (1<<2)             /* A framing error was detected */
        #define LSR_BRK      (1<<4)             /* A break condition was received */
        #define LSR_RxF      (1<<4)             /* The receive FIFO contains data */
        #define LSR_TxE      (1<<5)             /* The transmit data register is empty */
        #define LSR_TEMT     (1<<6)             /* Set if and only if the TDR and TSR are empty */
volatile xdata at 0xFFA6 unsigned char MSR;              /* Modem status register */
        #define MSR_DCTS     (1<<0)             /* CTS has changed state. Cleared by writing a one */
        #define MSR_DDSR     (1<<1)             /* DSR has changed state. Cleared by writing a one */
        #define MSR_TRI      (1<<2)             /* High->low edge on the ring indicator, cleared by writing a one */
        #define MSR_DCD      (1<<3)             /* CD has changed state. Cleared by writing a one */
        #define MSR_LCTS     (1<<4)             /* CTS input, during loopback */
        #define MSR_LDSR     (1<<5)             /* DSR input, during loopback */
        #define MSR_LRI      (1<<6)             /* Ring input, during loopback */
        #define MSR_LCD      (1<<7)             /* Carrier Detect input, during loopback */
volatile xdata at 0xFFA7 unsigned char DLL;              /* Baud rate divisor register, low byte */
volatile xdata at 0xFFA8 unsigned char DLH;              /* Baud rate divisor register, high byte */
volatile xdata at 0xFFA9 unsigned char XON;              /* Xon character register */
volatile xdata at 0xFFAA unsigned char XOFF;             /* Xoff character register */
volatile xdata at 0xFFAB unsigned char UART_MASK;        /* UART interrupt mask register */
        #define UART_MASK_MIE  (1<<0)           /* Modem interrupt */
        #define UART_MASK_SIE  (1<<1)           /* Status interrupt */
        #define UART_MASK_TRI  (1<<2)           /* TxE/RxF interrupts */

#endif
