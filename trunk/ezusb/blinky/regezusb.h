/*-------------------------------------------------------------------------
  Register Declarations for Anchorchips AN21xx USB processors
  
       Written By -  Thomas Sailer . sailer@ife.ee.ethz.ch (1999)
         based on reg51.h by Sandeep Dutta . sandeep.dutta@usa.net (1998)

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

#ifndef REGAN21xx_H
#define REGAN21xx_H

/*  BYTE Register  */
sfr at 0x81 SP      ;
sfr at 0x82 DPL     ;
sfr at 0x83 DPH     ;
sfr at 0x82 DPL0    ;
sfr at 0x83 DPH0    ;
sfr at 0x84 DPL1    ;
sfr at 0x85 DPH1    ;
sfr at 0x86 DPS     ;
sfr at 0x87 PCON    ;
sfr at 0x88 TCON    ;
sfr at 0x89 TMOD    ;
sfr at 0x8A TL0     ;
sfr at 0x8B TL1     ;
sfr at 0x8C TH0     ;
sfr at 0x8D TH1     ;
sfr at 0x8E CKCON   ;
sfr at 0x8F SPC_FNC ;
sfr at 0x91 EXIF    ;
sfr at 0x92 MPAGE   ;
sfr at 0x98 SCON0   ;
sfr at 0x99 SBUF0   ;
sfr at 0xA8 IE      ;
sfr at 0xB8 IP      ;
sfr at 0xC0 SCON1   ;
sfr at 0xC1 SBUF1   ;
sfr at 0xC8 T2CON   ;
sfr at 0xCA RCAP2L  ;
sfr at 0xCB RCAP2H  ;
sfr at 0xCC TL2     ;
sfr at 0xCD TH2     ;
sfr at 0xD0 PSW     ;
sfr at 0xD8 EICON   ;
sfr at 0xE0 ACC     ;
sfr at 0xE8 EIE     ;
sfr at 0xF0 B       ;
sfr at 0xF8 EIP     ;


/*  BIT Register  */
/*  TCON  */
sbit at 0x8F TF1  ;
sbit at 0x8E TR1  ;
sbit at 0x8D TF0  ;
sbit at 0x8C TR0  ;
sbit at 0x8B IE1  ;
sbit at 0x8A IT1  ;
sbit at 0x89 IE0  ;
sbit at 0x88 IT0  ;

/* TMOD bits */
#define GATE1   (1<<7)
#define C_T1    (1<<6)
#define M1_1    (1<<5)
#define M0_1    (1<<4)
#define GATE0   (1<<3)
#define C_T0    (1<<2)
#define M1_0    (1<<1)
#define M0_0    (1<<0)

/*  SCON0  */
sbit at 0x9F SM0_0 ;
sbit at 0x9E SM1_0 ;
sbit at 0x9D SM2_0 ;
sbit at 0x9C REN_0 ;
sbit at 0x9B TB8_0 ;
sbit at 0x9A RB8_0 ;
sbit at 0x99 TI_0  ;
sbit at 0x98 RI_0  ;

/*  IE   */
sbit at 0xAF EA    ;
sbit at 0xAE ES1   ;
sbit at 0xAD ET2   ;
sbit at 0xAC ES0   ;
sbit at 0xAB ET1   ;
sbit at 0xAA EX1   ;
sbit at 0xA9 ET0   ;
sbit at 0xA8 EX0   ;

/*  IP   */ 
sbit at 0xBE PS1   ;
sbit at 0xBD PT2   ;
sbit at 0xBC PS0   ;
sbit at 0xBB PT1   ;
sbit at 0xBA PX1   ;
sbit at 0xB9 PT0   ;
sbit at 0xB8 PX0   ;

/*  SCON1  */
sbit at 0xC7 SM0_1 ;
sbit at 0xC6 SM1_1 ;
sbit at 0xC5 SM2_1 ;
sbit at 0xC4 REN_1 ;
sbit at 0xC3 TB8_1 ;
sbit at 0xC2 RB8_1 ;
sbit at 0xC1 TI_1  ;
sbit at 0xC0 RI_1  ;

/*  T2CON  */
sbit at 0xCF TF2   ;
sbit at 0xCE EXF2  ;
sbit at 0xCD RCLK  ;
sbit at 0xCC TCLK  ;
sbit at 0xCB EXEN2 ;
sbit at 0xCA TR2   ;
sbit at 0xC9 CT2   ;
sbit at 0xC8 CPRL2 ;

/*  PSW   */
sbit at 0xD7 CY    ;
sbit at 0xD6 AC    ;
sbit at 0xD5 F0    ;
sbit at 0xD4 RS1   ;
sbit at 0xD3 RS0   ;
sbit at 0xD2 OV    ;
sbit at 0xD0 P     ;

/*  EICON  */
sbit at 0xDF SMOD1 ;
sbit at 0xDD ERESI ;
sbit at 0xDC RESI  ;
sbit at 0xDB INT6  ;

/*  EIE  */
sbit at 0xEC EWDI  ;
sbit at 0xEB EX5   ;
sbit at 0xEA EX4   ;
sbit at 0xE9 EI2C  ;
sbit at 0xE8 EUSB  ;

/*  EIP  */
sbit at 0xFC PX6   ;
sbit at 0xFB PX5   ;
sbit at 0xFA PX4   ;
sbit at 0xF9 PI2C  ;
sbit at 0xF8 PUSB  ;


/* XDATA registers */

/* control/interrupt/bulk endpoints */
xdata at 0x7F00 char IN0BUF[64];
xdata at 0x7EC0 char OUT0BUF[64];
xdata at 0x7E80 char IN1BUF[64];
xdata at 0x7E40 char OUT1BUF[64];
xdata at 0x7E00 char IN2BUF[64];
xdata at 0x7DC0 char OUT2BUF[64];
xdata at 0x7D80 char IN3BUF[64];
xdata at 0x7D40 char OUT3BUF[64];
xdata at 0x7D00 char IN4BUF[64];
xdata at 0x7CC0 char OUT4BUF[64];
xdata at 0x7C80 char IN5BUF[64];
xdata at 0x7C40 char OUT5BUF[64];
xdata at 0x7C00 char IN6BUF[64];
xdata at 0x7BC0 char OUT6BUF[64];
xdata at 0x7B80 char IN7BUF[64];
xdata at 0x7B40 char OUT7BUF[64];
xdata at 0x7FE8 char SETUPBUF[8];
xdata at 0x7FE8 char SETUPDAT[8];

xdata at 0x7FB4 char EP0CS;
xdata at 0x7FB5 char IN0BC;
xdata at 0x7FB6 char IN1CS;
xdata at 0x7FB7 char IN1BC;
xdata at 0x7FB8 char IN2CS;
xdata at 0x7FB9 char IN2BC;
xdata at 0x7FBA char IN3CS;
xdata at 0x7FBB char IN3BC;
xdata at 0x7FBC char IN4CS;
xdata at 0x7FBD char IN4BC;
xdata at 0x7FBE char IN5CS;
xdata at 0x7FBF char IN5BC;
xdata at 0x7FC0 char IN6CS;
xdata at 0x7FC1 char IN6BC;
xdata at 0x7FC2 char IN7CS;
xdata at 0x7FC3 char IN7BC;
xdata at 0x7FC5 char OUT0BC;
xdata at 0x7FC6 char OUT1CS;
xdata at 0x7FC7 char OUT1BC;
xdata at 0x7FC8 char OUT2CS;
xdata at 0x7FC9 char OUT2BC;
xdata at 0x7FCA char OUT3CS;
xdata at 0x7FCB char OUT3BC;
xdata at 0x7FCC char OUT4CS;
xdata at 0x7FCD char OUT4BC;
xdata at 0x7FCE char OUT5CS;
xdata at 0x7FCF char OUT5BC;
xdata at 0x7FD0 char OUT6CS;
xdata at 0x7FD1 char OUT6BC;
xdata at 0x7FD2 char OUT7CS;
xdata at 0x7FD3 char OUT7BC;

xdata at 0x7FA8 char IVEC;
xdata at 0x7FA9 char IN07IRQ;
xdata at 0x7FAA char OUT07IRQ;
xdata at 0x7FAB char USBIRQ;
xdata at 0x7FAC char IN07IEN;
xdata at 0x7FAD char OUT07IEN;
xdata at 0x7FAE char USBIEN;
xdata at 0x7FAF char USBBAV;
xdata at 0x7FB2 char BPADDRH;
xdata at 0x7FB3 char BPADDRL;

xdata at 0x7FD4 char SUDPTRH;
xdata at 0x7FD5 char SUDPTRL;
xdata at 0x7FD6 char USBCS;
xdata at 0x7FD7 char TOGCTL;
xdata at 0x7FD8 char USBFRAMEL;
xdata at 0x7FD9 char USBFRAMEH;
xdata at 0x7FDB char FNADDR;
xdata at 0x7FDD char USBPAIR;
xdata at 0x7FDE char IN07VAL;
xdata at 0x7FDF char OUT07VAL;
xdata at 0x7FE3 char AUTOPTRH;
xdata at 0x7FE4 char AUTOPTRL;
xdata at 0x7FE5 char AUTODATA;

/* isochronous endpoints. only available if ISODISAB=0 */

xdata at 0x7F60 char OUT8DATA;
xdata at 0x7F61 char OUT9DATA;
xdata at 0x7F62 char OUT10DATA;
xdata at 0x7F63 char OUT11DATA;
xdata at 0x7F64 char OUT12DATA;
xdata at 0x7F65 char OUT13DATA;
xdata at 0x7F66 char OUT14DATA;
xdata at 0x7F67 char OUT15DATA;

xdata at 0x7F68 char IN8DATA;
xdata at 0x7F69 char IN9DATA;
xdata at 0x7F6A char IN10DATA;
xdata at 0x7F6B char IN11DATA;
xdata at 0x7F6C char IN12DATA;
xdata at 0x7F6D char IN13DATA;
xdata at 0x7F6E char IN14DATA;
xdata at 0x7F6F char IN15DATA;

xdata at 0x7F70 char OUT8BCH;
xdata at 0x7F71 char OUT8BCL;
xdata at 0x7F72 char OUT9BCH;
xdata at 0x7F73 char OUT9BCL;
xdata at 0x7F74 char OUT10BCH;
xdata at 0x7F75 char OUT10BCL;
xdata at 0x7F76 char OUT11BCH;
xdata at 0x7F77 char OUT11BCL;
xdata at 0x7F78 char OUT12BCH;
xdata at 0x7F79 char OUT12BCL;
xdata at 0x7F7A char OUT13BCH;
xdata at 0x7F7B char OUT13BCL;
xdata at 0x7F7C char OUT14BCH;
xdata at 0x7F7D char OUT14BCL;
xdata at 0x7F7E char OUT15BCH;
xdata at 0x7F7F char OUT15BCL;

xdata at 0x7FF0 char OUT8ADDR;
xdata at 0x7FF1 char OUT9ADDR;
xdata at 0x7FF2 char OUT10ADDR;
xdata at 0x7FF3 char OUT11ADDR;
xdata at 0x7FF4 char OUT12ADDR;
xdata at 0x7FF5 char OUT13ADDR;
xdata at 0x7FF6 char OUT14ADDR;
xdata at 0x7FF7 char OUT15ADDR;
xdata at 0x7FF8 char IN8ADDR;
xdata at 0x7FF9 char IN9ADDR;
xdata at 0x7FFA char IN10ADDR;
xdata at 0x7FFB char IN11ADDR;
xdata at 0x7FFC char IN12ADDR;
xdata at 0x7FFD char IN13ADDR;
xdata at 0x7FFE char IN14ADDR;
xdata at 0x7FFF char IN15ADDR;

xdata at 0x7FA0 char ISOERR;
xdata at 0x7FA1 char ISOCTL;
xdata at 0x7FA2 char ZBCOUNT;
xdata at 0x7FE0 char INISOVAL;
xdata at 0x7FE1 char OUTISOVAL;
xdata at 0x7FE2 char FASTXFR;

/* CPU control registers */

xdata at 0x7F92 char CPUCS;

/* IO port control registers */

xdata at 0x7F93 char PORTACFG;
xdata at 0x7F94 char PORTBCFG;
xdata at 0x7F95 char PORTCCFG;
xdata at 0x7F96 char OUTA;
xdata at 0x7F97 char OUTB;
xdata at 0x7F98 char OUTC;
xdata at 0x7F99 char PINSA;
xdata at 0x7F9A char PINSB;
xdata at 0x7F9B char PINSC;
xdata at 0x7F9C char OEA;
xdata at 0x7F9D char OEB;
xdata at 0x7F9E char OEC;

/* I2C controller registers */

xdata at 0x7FA5 char I2CS;
xdata at 0x7FA6 char I2DAT;

/* interrupts */

#endif
