;--------------------------------------------------------
; File Created by SDCC : FreeWare ANSI-C Compiler
; Version 0.0.0 Tue Feb 25 14:46:13 2003

;--------------------------------------------------------
; PIC port for the 14-bit core
;--------------------------------------------------------
;	.module main
	list	p=16f877
	__CONFIG 0x3fff
	radix dec
include "p16f877.inc"
;--------------------------------------------------------
; publics variables in this module
;--------------------------------------------------------
;	.globl _main
;	.globl _SerialTris
;	.globl _SerialPort
;	.globl _Light3
;	.globl _Light2
;	.globl _Light1
;	.globl _Light0
;	.globl _RD
;	.globl _WR
;	.globl _WREN
;	.globl _WRERR
;	.globl _EEPGD
;	.globl _PCFG0
;	.globl _PCFG1
;	.globl _PCFG2
;	.globl _PCFG3
;	.globl _ADFM
;	.globl _TXD8
;	.globl _TX9D
;	.globl _TRMT
;	.globl _BRGH
;	.globl _SYNC
;	.globl _TXEN
;	.globl _TX8_9
;	.globl _NOT_TX8
;	.globl _TX9
;	.globl _CSRC
;	.globl _BF
;	.globl _UA
;	.globl _READ_WRITE
;	.globl _R_W
;	.globl _NOT_WRITE
;	.globl _NOT_W
;	.globl _I2C_READ
;	.globl _R
;	.globl _I2C_START
;	.globl _S
;	.globl _I2C_STOP
;	.globl _P
;	.globl _DATA_ADDRESS
;	.globl _D_A
;	.globl _NOT_ADDRESS
;	.globl _NOT_A
;	.globl _I2C_DATA
;	.globl _D
;	.globl _CKE
;	.globl _SMP
;	.globl _SEN
;	.globl _RSEN
;	.globl _PEN
;	.globl _RCEN
;	.globl _ACKEN
;	.globl _ACKDT
;	.globl _ACKSTAT
;	.globl _GCEN
;	.globl _NOT_BOR
;	.globl _NOT_BO
;	.globl _NOT_POR
;	.globl _CCP2IE
;	.globl _BCLIE
;	.globl _EEIE
;	.globl _TMR1IE
;	.globl _TMR2IE
;	.globl _CCP1IE
;	.globl _SSPIE
;	.globl _TXIE
;	.globl _RCIE
;	.globl _ADIE
;	.globl _PSPIE
;	.globl _TRISE0
;	.globl _TRISE1
;	.globl _TRISE2
;	.globl _PSPMODE
;	.globl _IBOV
;	.globl _OBF
;	.globl _IBF
;	.globl _PS0
;	.globl _PS1
;	.globl _PS2
;	.globl _PSA
;	.globl _T0SE
;	.globl _T0CS
;	.globl _INTEDG
;	.globl _NOT_RBPU
;	.globl _ADON
;	.globl _GO_DONE
;	.globl _NOT_DONE
;	.globl _GO
;	.globl _CHS0
;	.globl _CHS1
;	.globl _CHS2
;	.globl _ADCS0
;	.globl _ADCS1
;	.globl _CCP2M0
;	.globl _CCP2M1
;	.globl _CCP2M2
;	.globl _CCP2M3
;	.globl _CCP2Y
;	.globl _CCP2X
;	.globl _RCD8
;	.globl _RX9D
;	.globl _OERR
;	.globl _FERR
;	.globl _ADDEN
;	.globl _CREN
;	.globl _SREN
;	.globl _RC8_9
;	.globl _NOT_RC8
;	.globl _RC9
;	.globl _RX9
;	.globl _SPEN
;	.globl _CCP1M0
;	.globl _CCP1M1
;	.globl _CCP1M2
;	.globl _CCP1M3
;	.globl _CCP1Y
;	.globl _CCP1X
;	.globl _SSPM0
;	.globl _SSPM1
;	.globl _SSPM2
;	.globl _SSPM3
;	.globl _CKP
;	.globl _SSPEN
;	.globl _SSPOV
;	.globl _WCOL
;	.globl _T2CKPS0
;	.globl _T2CKPS1
;	.globl _TMR2ON
;	.globl _TOUTPS0
;	.globl _TOUTPS1
;	.globl _TOUTPS2
;	.globl _TOUTPS3
;	.globl _TMR1ON
;	.globl _TMR1CS
;	.globl _T1SYNC
;	.globl _T1INSYNC
;	.globl _NOT_T1SYNC
;	.globl _T1OSCEN
;	.globl _T1CKPS0
;	.globl _T1CKPS1
;	.globl _CCP2IF
;	.globl _BCLIF
;	.globl _EEIF
;	.globl _TMR1IF
;	.globl _TMR2IF
;	.globl _CCP1IF
;	.globl _SSPIF
;	.globl _TXIF
;	.globl _RCIF
;	.globl _ADIF
;	.globl _PSPIF
;	.globl _RBIF
;	.globl _INTF
;	.globl _T0IF
;	.globl _RBIE
;	.globl _INTE
;	.globl _T0IE
;	.globl _PEIE
;	.globl _GIE
;	.globl _C
;	.globl _DC
;	.globl _Z
;	.globl _NOT_PD
;	.globl _NOT_TO
;	.globl _RP0
;	.globl _RP1
;	.globl _IRP
;	.globl _reading7
;	.globl _reading6
;	.globl _reading5
;	.globl _reading4
;	.globl _reading3
;	.globl _reading2
;	.globl _reading1
;	.globl _reading0
;	.globl _PCL
;	.globl _INDF
;	.globl _initialize
;	.globl _sampleAll
;	.globl _integrateSample
;	.globl _sensorSample
;	.globl _serialBitDelay
;	.globl _putc
;	.globl _sendPacket
;--------------------------------------------------------
; special function registers
;--------------------------------------------------------
_TMR0	EQU	0x0001
_STATUS	EQU	0x0003
_FSR	EQU	0x0004
_PORTA	EQU	0x0005
_PORTB	EQU	0x0006
_PORTC	EQU	0x0007
_PORTD	EQU	0x0008
_PORTE	EQU	0x0009
_PCLATH	EQU	0x000a
_INTCON	EQU	0x000b
_PIR1	EQU	0x000c
_PIR2	EQU	0x000d
_TMR1L	EQU	0x000e
_TMR1H	EQU	0x000f
_T1CON	EQU	0x0010
_TMR2	EQU	0x0011
_T2CON	EQU	0x0012
_SSPBUF	EQU	0x0013
_SSPCON	EQU	0x0014
_CCPR1L	EQU	0x0015
_CCPR1H	EQU	0x0016
_CCP1CON	EQU	0x0017
_RCSTA	EQU	0x0018
_TXREG	EQU	0x0019
_RCREG	EQU	0x001a
_CCPR2L	EQU	0x001b
_CCPR2H	EQU	0x001c
_CCP2CON	EQU	0x001d
_ADRESH	EQU	0x001e
_ADCON0	EQU	0x001f
_OPTION_REG	EQU	0x0081
_TRISA	EQU	0x0085
_TRISB	EQU	0x0086
_TRISC	EQU	0x0087
_TRISD	EQU	0x0088
_TRISE	EQU	0x0089
_PIE1	EQU	0x008c
_PIE2	EQU	0x008d
_PCON	EQU	0x008e
_SSPCON2	EQU	0x0091
_PR2	EQU	0x0092
_SSPADD	EQU	0x0093
_SSPSTAT	EQU	0x0094
_TXSTA	EQU	0x0098
_SPBRG	EQU	0x0099
_ADRESL	EQU	0x009e
_ADCON1	EQU	0x009f
_EEDATA	EQU	0x010c
_EEADR	EQU	0x010d
_EEDATH	EQU	0x010e
_EEADRH	EQU	0x010f
_EECON1	EQU	0x018c
_EECON2	EQU	0x018d
  cblock  0X0005	; Bank 0
	_PORTA
	fbitfield06
	_PORTC
	_PORTD
	fbitfield09
  endc
  cblock  0X000B	; Bank 0
	fbitfield0b
  endc
  cblock  0X001E	; Bank 0
	_ADRESH
	fbitfield1f
	r0x53
	r0x54
	r0x55
	r0x56
	r0x57
	r0x58
	r0x59
	r0x5A
	r0x5C
	r0x5E
	r0x60
	r0x62
	r0x63
	r0x65
	r0x66
	r0x67
	r0x68
	r0x6A
	r0x6C
	r0x6D
	r0x6E
	r0x6F
	r0x70
	r0x71
	r0x72
	r0x73
	r0x74
	r0x75
	r0x76
	r0x77
	r0x78
	r0x79
	r0x7A
	r0x7B
	r0x7C
	r0x7D
	r0x7E
	r0x7F
	r0x80
	s0xFFF
	s0x1000
	_reading0
	_reading0_1
	_reading1
	_reading1_1
	_reading2
	_reading2_1
	_reading3
	_reading3_1
	_reading4
	_reading4_1
	_reading5
	_reading5_1
	_reading6
	_reading6_1
	_reading7
	_reading7_1
	bitfield0
  endc
  cblock  0X0085	; Bank 1
	_TRISA
	_TRISB
	_TRISC
	_TRISD
	fbitfield89
  endc
  cblock  0X009E	; Bank 1
	_ADRESL
	_ADCON1
  endc
_Light0	EQU	( (fbitfield06<<3)+0)
_GIE	EQU	( (fbitfield0b<<3)+7)
_sensorSample_phase0_1_1	EQU	( (bitfield0<<3)+0)
_sensorSample_phase1_1_1	EQU	( (bitfield0<<3)+1)
_sensorSample_phase2_1_1	EQU	( (bitfield0<<3)+2)
_sensorSample_phase3_1_1	EQU	( (bitfield0<<3)+3)
_GO_DONE	EQU	( (fbitfield1f<<3)+2)
_SerialPort	EQU	( (fbitfield09<<3)+2)
_SerialTris	EQU	( (fbitfield89<<3)+2)
r0x53	EQU	0x020
r0x54	EQU	0x021
r0x55	EQU	0x022
r0x56	EQU	0x023
r0x57	EQU	0x024
r0x58	EQU	0x025
r0x59	EQU	0x026
r0x5A	EQU	0x027
r0x5C	EQU	0x028
r0x5E	EQU	0x029
r0x60	EQU	0x02a
r0x62	EQU	0x02b
r0x63	EQU	0x02c
r0x65	EQU	0x02d
r0x66	EQU	0x02e
r0x67	EQU	0x02f
r0x68	EQU	0x030
r0x6A	EQU	0x031
r0x6C	EQU	0x032
r0x6D	EQU	0x033
r0x6E	EQU	0x034
r0x6F	EQU	0x035
r0x70	EQU	0x036
r0x71	EQU	0x037
r0x72	EQU	0x038
r0x73	EQU	0x039
r0x74	EQU	0x03a
r0x75	EQU	0x03b
r0x76	EQU	0x03c
r0x77	EQU	0x03d
r0x78	EQU	0x03e
r0x79	EQU	0x03f
r0x7A	EQU	0x040
r0x7B	EQU	0x041
r0x7C	EQU	0x042
r0x7D	EQU	0x043
r0x7E	EQU	0x044
r0x7F	EQU	0x045
r0x80	EQU	0x046
_TRISA	EQU	0x085
_TRISB	EQU	0x086
_TRISC	EQU	0x087
_TRISD	EQU	0x088
_TRISE	EQU	0x089
_PORTA	EQU	0x005
_PORTB	EQU	0x006
_PORTC	EQU	0x007
_PORTD	EQU	0x008
_PORTE	EQU	0x009
_ADCON1	EQU	0x09f
_ADCON0	EQU	0x01f
_reading0	EQU	0x049
_reading1	EQU	0x04b
_reading2	EQU	0x04d
_reading3	EQU	0x04f
_reading4	EQU	0x051
_reading5	EQU	0x053
_reading6	EQU	0x055
_reading7	EQU	0x057
_PORTB	EQU	0x006
_TRISB	EQU	0x086
_ADCON0	EQU	0x01f
_ADCON0	EQU	0x01f
_PORTB	EQU	0x006
_TRISB	EQU	0x086
_ADRESH	EQU	0x01e
_ADRESL	EQU	0x09e
fbitfield06	EQU	0x006
fbitfield0b	EQU	0x00b
bitfield0	EQU	0x059
fbitfield1f	EQU	0x01f
fbitfield09	EQU	0x009
fbitfield89	EQU	0x089
s0xFFF	EQU	0x047
s0x1000	EQU	0x048
_STATUS	EQU	0x003
PCL	EQU	0x002
_PCLATH	EQU	0x00a
FSR	EQU	0x004
INDF	EQU	0x000
INTCON	EQU	0x00b
;--------------------------------------------------------
; overlayable items in internal ram 
;--------------------------------------------------------
;	.area	OSEG    (OVR,DATA)
;--------------------------------------------------------
; Stack segment in internal ram 
;--------------------------------------------------------
;	.area	SSEG	(DATA)
;__start__stack:
;	.ds	1

;--------------------------------------------------------
; indirectly addressable internal ram data
;--------------------------------------------------------
;	.area	ISEG    (DATA)
;--------------------------------------------------------
; external ram data
;--------------------------------------------------------
;	.area	XSEG    (XDATA)
;--------------------------------------------------------
; bit data
;--------------------------------------------------------
;	.area	BSEG    (BIT)
_IRP	EQU	0x001f
_RP1	EQU	0x001e
_RP0	EQU	0x001d
_NOT_TO	EQU	0x001c
_NOT_PD	EQU	0x001b
_Z	EQU	0x001a
_DC	EQU	0x0019
_C	EQU	0x0018
_GIE	EQU	0x005f
_PEIE	EQU	0x005e
_T0IE	EQU	0x005d
_INTE	EQU	0x005c
_RBIE	EQU	0x005b
_T0IF	EQU	0x005a
_INTF	EQU	0x0059
_RBIF	EQU	0x0058
_PSPIF	EQU	0x0067
_ADIF	EQU	0x0066
_RCIF	EQU	0x0065
_TXIF	EQU	0x0064
_SSPIF	EQU	0x0063
_CCP1IF	EQU	0x0062
_TMR2IF	EQU	0x0061
_TMR1IF	EQU	0x0060
_EEIF	EQU	0x006c
_BCLIF	EQU	0x006b
_CCP2IF	EQU	0x0068
_T1CKPS1	EQU	0x0085
_T1CKPS0	EQU	0x0084
_T1OSCEN	EQU	0x0083
_NOT_T1SYNC	EQU	0x0082
_T1INSYNC	EQU	0x0082
_T1SYNC	EQU	0x0082
_TMR1CS	EQU	0x0081
_TMR1ON	EQU	0x0080
_TOUTPS3	EQU	0x0096
_TOUTPS2	EQU	0x0095
_TOUTPS1	EQU	0x0094
_TOUTPS0	EQU	0x0093
_TMR2ON	EQU	0x0092
_T2CKPS1	EQU	0x0091
_T2CKPS0	EQU	0x0090
_WCOL	EQU	0x00a7
_SSPOV	EQU	0x00a6
_SSPEN	EQU	0x00a5
_CKP	EQU	0x00a4
_SSPM3	EQU	0x00a3
_SSPM2	EQU	0x00a2
_SSPM1	EQU	0x00a1
_SSPM0	EQU	0x00a0
_CCP1X	EQU	0x00bd
_CCP1Y	EQU	0x00bc
_CCP1M3	EQU	0x00bb
_CCP1M2	EQU	0x00ba
_CCP1M1	EQU	0x00b9
_CCP1M0	EQU	0x00b8
_SPEN	EQU	0x00c7
_RX9	EQU	0x00c6
_RC9	EQU	0x00c6
_NOT_RC8	EQU	0x00c6
_RC8_9	EQU	0x00c6
_SREN	EQU	0x00c5
_CREN	EQU	0x00c4
_ADDEN	EQU	0x00c3
_FERR	EQU	0x00c2
_OERR	EQU	0x00c1
_RX9D	EQU	0x00c0
_RCD8	EQU	0x00c0
_CCP2X	EQU	0x00ed
_CCP2Y	EQU	0x00ec
_CCP2M3	EQU	0x00eb
_CCP2M2	EQU	0x00ea
_CCP2M1	EQU	0x00e9
_CCP2M0	EQU	0x00e8
_ADCS1	EQU	0x00ff
_ADCS0	EQU	0x00fe
_CHS2	EQU	0x00fd
_CHS1	EQU	0x00fc
_CHS0	EQU	0x00fb
_GO	EQU	0x00fa
_NOT_DONE	EQU	0x00fa
_GO_DONE	EQU	0x00fa
_ADON	EQU	0x00f8
_NOT_RBPU	EQU	0x040f
_INTEDG	EQU	0x040e
_T0CS	EQU	0x040d
_T0SE	EQU	0x040c
_PSA	EQU	0x040b
_PS2	EQU	0x040a
_PS1	EQU	0x0409
_PS0	EQU	0x0408
_IBF	EQU	0x044f
_OBF	EQU	0x044e
_IBOV	EQU	0x044d
_PSPMODE	EQU	0x044c
_TRISE2	EQU	0x044a
_TRISE1	EQU	0x0449
_TRISE0	EQU	0x0448
_PSPIE	EQU	0x0467
_ADIE	EQU	0x0466
_RCIE	EQU	0x0465
_TXIE	EQU	0x0464
_SSPIE	EQU	0x0463
_CCP1IE	EQU	0x0462
_TMR2IE	EQU	0x0461
_TMR1IE	EQU	0x0460
_EEIE	EQU	0x046c
_BCLIE	EQU	0x046b
_CCP2IE	EQU	0x0468
_NOT_POR	EQU	0x0471
_NOT_BO	EQU	0x0470
_NOT_BOR	EQU	0x0470
_GCEN	EQU	0x048f
_ACKSTAT	EQU	0x048e
_ACKDT	EQU	0x048d
_ACKEN	EQU	0x048c
_RCEN	EQU	0x048b
_PEN	EQU	0x048a
_RSEN	EQU	0x0489
_SEN	EQU	0x0488
_SMP	EQU	0x04a7
_CKE	EQU	0x04a6
_D	EQU	0x04a5
_I2C_DATA	EQU	0x04a5
_NOT_A	EQU	0x04a5
_NOT_ADDRESS	EQU	0x04a5
_D_A	EQU	0x04a5
_DATA_ADDRESS	EQU	0x04a5
_P	EQU	0x04a4
_I2C_STOP	EQU	0x04a4
_S	EQU	0x04a3
_I2C_START	EQU	0x04a3
_R	EQU	0x04a2
_I2C_READ	EQU	0x04a2
_NOT_W	EQU	0x04a2
_NOT_WRITE	EQU	0x04a2
_R_W	EQU	0x04a2
_READ_WRITE	EQU	0x04a2
_UA	EQU	0x04a1
_BF	EQU	0x04a0
_CSRC	EQU	0x04c7
_TX9	EQU	0x04c6
_NOT_TX8	EQU	0x04c6
_TX8_9	EQU	0x04c6
_TXEN	EQU	0x04c5
_SYNC	EQU	0x04c4
_BRGH	EQU	0x04c2
_TRMT	EQU	0x04c1
_TX9D	EQU	0x04c0
_TXD8	EQU	0x04c0
_ADFM	EQU	0x04ff
_PCFG3	EQU	0x04fb
_PCFG2	EQU	0x04fa
_PCFG1	EQU	0x04f9
_PCFG0	EQU	0x04f8
_EEPGD	EQU	0x0c67
_WRERR	EQU	0x0c63
_WREN	EQU	0x0c62
_WR	EQU	0x0c61
_RD	EQU	0x0c60
_Light0	EQU	0x0030
_Light1	EQU	0x0031
_Light2	EQU	0x0032
_Light3	EQU	0x0033
_SerialPort	EQU	0x004a
_SerialTris	EQU	0x044a
	ORG 0
;--------------------------------------------------------
; interrupt vector 
;--------------------------------------------------------
;	.area	CSEG    (CODE)
;__interrupt_vect:
;	ljmp	__sdcc_gsinit_startup
;	ljmp	_txInterrupt
;	.ds	5
;	reti
;	.ds	7
;	reti
;	.ds	7
;	reti
;	.ds	7
;	reti
;	.ds	7
;	reti
;	.ds	7
;--------------------------------------------------------
; global & static initialisations
;--------------------------------------------------------
;	.area GSINIT  (CODE)
;	.area GSFINAL (CODE)
;	.area GSINIT  (CODE)
__sdcc_gsinit_startup:
;	.area GSFINAL (CODE)
;	ljmp	__sdcc_program_startup
;--------------------------------------------------------
; code
;--------------------------------------------------------
;	.area CSEG    (CODE)
;***
;  pBlock Stats: dbName = M
;***
;entry:  _main	;Function start
; 2 exit points
;has an exit
;functions called:
;   _initialize
;   _sampleAll
;   _sendPacket
;   _initialize
;   _sampleAll
;   _sendPacket
;; Starting pCode block
_main	;Function start
; 2 exit points
;#CSRC	main.c 95
;  NOP NOP NOP NOP   /* Padding for the bootloader */
	NOP	
	NOP	
	NOP	
	NOP	
;#CSRC	main.c 97
;  initialize();
	CALL	_initialize
;#CSRC	main.c 99
;  Light0 = 1;
	BSF	(_Light0 >> 3), (_Light0 & 7)
_00106_DS_
;#CSRC	main.c 102
;  sampleAll();
	CALL	_sampleAll
;#CSRC	main.c 103
;  sendPacket();
	CALL	_sendPacket
	GOTO	_00106_DS_
	RETURN	
; exit point of _main
;***
;  pBlock Stats: dbName = C
;***
;entry:  _sendPacket	;Function start
; 2 exit points
;has an exit
;functions called:
;   _putc
;   _putc
;   _putc
;   _putc
;   _putc
;   _putc
;   _putc
;   _putc
;   _putc
;   _putc
;   _putc
;   _putc
;   _putc
;   _putc
;   _putc
;   _putc
;   _putc
;   _putc
;   _putc
;   _putc
;   _putc
;   _putc
;   _putc
;   _putc
;   _putc
;   _putc
;   _putc
;   _putc
;   _putc
;   _putc
;   _putc
;   _putc
;   _putc
;   _putc
;   _putc
;   _putc
;18 compiler assigned registers:
;   r0x6F
;   r0x70
;   r0x71
;   r0x72
;   r0x73
;   r0x74
;   r0x75
;   r0x76
;   r0x77
;   r0x78
;   r0x79
;   r0x7A
;   r0x7B
;   r0x7C
;   r0x7D
;   r0x7E
;   r0x7F
;   r0x80
;; Starting pCode block
_sendPacket	;Function start
; 2 exit points
;#CSRC	main.c 272
;  putc(0x80);
	MOVLW	0x80
	CALL	_putc
;#CSRC	main.c 274
;  h = reading0 >> 8;
	MOVF	(_reading0 + 1),W
	MOVWF	r0x6F
	CLRF	r0x70
	BTFSC	_reading0,7
	DECF	r0x70,F
;;     peep 11 - Removed redundant move
	MOVF	r0x6F,W
;#CSRC	main.c 275
;  l = reading0;
	MOVF	_reading0,W
	MOVWF	r0x71
;#CSRC	main.c 276
;  putc(h);
	MOVF	r0x6F,W
	CALL	_putc
;#CSRC	main.c 277
;  putc(l);
	MOVF	r0x71,W
	CALL	_putc
;#CSRC	main.c 278
;  checksum += h + l;
	MOVF	r0x71,W
	ADDWF	r0x6F,W
	MOVWF	r0x72
;#CSRC	main.c 280
;  h = reading1 >> 8;
	MOVF	(_reading1 + 1),W
	MOVWF	r0x73
	CLRF	r0x74
	BTFSC	_reading1,7
	DECF	r0x74,F
	MOVF	r0x73,W
	MOVWF	r0x6F
;#CSRC	main.c 281
;  l = reading1;
	MOVF	_reading1,W
	MOVWF	r0x71
;#CSRC	main.c 282
;  putc(h);
	MOVF	r0x6F,W
	CALL	_putc
;#CSRC	main.c 283
;  putc(l);
	MOVF	r0x71,W
	CALL	_putc
;#CSRC	main.c 284
;  checksum += h + l;
	MOVF	r0x71,W
	ADDWF	r0x6F,W
	ADDWF	r0x72,F
;#CSRC	main.c 286
;  h = reading2 >> 8;
	MOVF	(_reading2 + 1),W
	MOVWF	r0x75
	CLRF	r0x76
	BTFSC	_reading2,7
	DECF	r0x76,F
	MOVF	r0x75,W
	MOVWF	r0x6F
;#CSRC	main.c 287
;  l = reading2;
	MOVF	_reading2,W
	MOVWF	r0x71
;#CSRC	main.c 288
;  putc(h);
	MOVF	r0x6F,W
	CALL	_putc
;#CSRC	main.c 289
;  putc(l);
	MOVF	r0x71,W
	CALL	_putc
;#CSRC	main.c 290
;  checksum += h + l;
	MOVF	r0x71,W
	ADDWF	r0x6F,W
	ADDWF	r0x72,F
;#CSRC	main.c 292
;  h = reading3 >> 8;
	MOVF	(_reading3 + 1),W
	MOVWF	r0x77
	CLRF	r0x78
	BTFSC	_reading3,7
	DECF	r0x78,F
	MOVF	r0x77,W
	MOVWF	r0x6F
;#CSRC	main.c 293
;  l = reading3;
	MOVF	_reading3,W
	MOVWF	r0x71
;#CSRC	main.c 294
;  putc(h);
	MOVF	r0x6F,W
	CALL	_putc
;#CSRC	main.c 295
;  putc(l);
	MOVF	r0x71,W
	CALL	_putc
;#CSRC	main.c 296
;  checksum += h + l;
	MOVF	r0x71,W
	ADDWF	r0x6F,W
	ADDWF	r0x72,F
;#CSRC	main.c 298
;  h = reading4 >> 8;
	MOVF	(_reading4 + 1),W
	MOVWF	r0x79
	CLRF	r0x7A
	BTFSC	_reading4,7
	DECF	r0x7A,F
;#CSRC	main.c 299
;  l = reading4;
	MOVF	_reading4,W
	MOVWF	r0x71
;#CSRC	main.c 300
;  putc(h);
	MOVF	r0x79,W
	MOVWF	r0x6F
	CALL	_putc
;#CSRC	main.c 301
;  putc(l);
	MOVF	r0x71,W
	CALL	_putc
;#CSRC	main.c 302
;  checksum += h + l;
	MOVF	r0x71,W
	ADDWF	r0x6F,W
	ADDWF	r0x72,F
;#CSRC	main.c 304
;  h = reading5 >> 8;
	MOVF	(_reading5 + 1),W
	MOVWF	r0x7B
	CLRF	r0x7C
	BTFSC	_reading5,7
	DECF	r0x7C,F
;#CSRC	main.c 305
;  l = reading5;
	MOVF	_reading5,W
	MOVWF	r0x71
;#CSRC	main.c 306
;  putc(h);
	MOVF	r0x7B,W
	MOVWF	r0x6F
	CALL	_putc
;#CSRC	main.c 307
;  putc(l);
	MOVF	r0x71,W
	CALL	_putc
;#CSRC	main.c 308
;  checksum += h + l;
	MOVF	r0x71,W
	ADDWF	r0x6F,W
	ADDWF	r0x72,F
;#CSRC	main.c 310
;  h = reading6 >> 8;
	MOVF	(_reading6 + 1),W
	MOVWF	r0x7D
	CLRF	r0x7E
	BTFSC	_reading6,7
	DECF	r0x7E,F
;#CSRC	main.c 311
;  l = reading6;
	MOVF	_reading6,W
	MOVWF	r0x71
;#CSRC	main.c 312
;  putc(h);
	MOVF	r0x7D,W
	MOVWF	r0x6F
	CALL	_putc
;#CSRC	main.c 313
;  putc(l);
	MOVF	r0x71,W
	CALL	_putc
;#CSRC	main.c 314
;  checksum += h + l;
	MOVF	r0x71,W
	ADDWF	r0x6F,W
	ADDWF	r0x72,F
;#CSRC	main.c 316
;  h = reading7 >> 8;
	MOVF	(_reading7 + 1),W
	MOVWF	r0x7F
	CLRF	r0x80
	BTFSC	_reading7,7
	DECF	r0x80,F
;#CSRC	main.c 317
;  l = reading7;
	MOVF	_reading7,W
	MOVWF	r0x71
;#CSRC	main.c 318
;  putc(h);
	MOVF	r0x7F,W
	MOVWF	r0x6F
	CALL	_putc
;#CSRC	main.c 319
;  putc(l);
	MOVF	r0x71,W
	CALL	_putc
;#CSRC	main.c 320
;  checksum += h + l;
	MOVF	r0x71,W
	ADDWF	r0x6F,W
	ADDWF	r0x72,F
;#CSRC	main.c 323
;  putc(checksum);
	MOVF	r0x72,W
	CALL	_putc
	RETURN	
; exit point of _sendPacket
;***
;  pBlock Stats: dbName = C
;***
;entry:  _putc	;Function start
; 2 exit points
;has an exit
;functions called:
;   _serialBitDelay
;   _serialBitDelay
;   _serialBitDelay
;   _serialBitDelay
;   _serialBitDelay
;   _serialBitDelay
;2 compiler assigned registers:
;   r0x6D
;   r0x6E
;; Starting pCode block
_putc	;Function start
; 2 exit points
;#CSRC	main.c 241
;  void putc(unsigned char byte) {
	MOVWF	r0x6D
;#CSRC	main.c 245
;  SerialPort = 0;
	BCF	(_SerialPort >> 3), (_SerialPort & 7)
	BSF	_STATUS,5
;#CSRC	main.c 246
;  SerialTris = 0;
	BCF	(_SerialTris >> 3), (_SerialTris & 7)
	BCF	_STATUS,5
;#CSRC	main.c 247
;  serialBitDelay();
	CALL	_serialBitDelay
;#CSRC	main.c 251
;  do {
	MOVLW	0x08
	MOVWF	r0x6E
_00175_DS_
;#CSRC	main.c 252
;  if (byte & 1)
	BTFSS	r0x6D,0
	GOTO	_00173_DS_
;#CSRC	main.c 253
;  SerialPort = 1;
	BSF	(_SerialPort >> 3), (_SerialPort & 7)
	GOTO	_00174_DS_
_00173_DS_
;#CSRC	main.c 255
;  SerialPort = 0;
	BCF	(_SerialPort >> 3), (_SerialPort & 7)
_00174_DS_
;#CSRC	main.c 256
;  byte >>= 1;
	BCF	_STATUS,0
	RRF	r0x6D,F
;#CSRC	main.c 257
;  serialBitDelay();
	CALL	_serialBitDelay
;#CSRC	main.c 258
;  } while (--bitnum);
	DECFSZ	r0x6E,F
	GOTO	_00175_DS_
;#CSRC	main.c 261
;  SerialPort = 1;
	BSF	(_SerialPort >> 3), (_SerialPort & 7)
;#CSRC	main.c 262
;  serialBitDelay();
	CALL	_serialBitDelay
	BSF	_STATUS,5
;#CSRC	main.c 263
;  SerialTris = 1;
	BSF	(_SerialTris >> 3), (_SerialTris & 7)
	BCF	_STATUS,5
	RETURN	
; exit point of _putc
;***
;  pBlock Stats: dbName = C
;***
;entry:  _serialBitDelay	;Function start
; 2 exit points
;has an exit
;1 compiler assigned register :
;   r0x6C
;; Starting pCode block
_serialBitDelay	;Function start
; 2 exit points
;#CSRC	main.c 234
;  do {
	MOVLW	0x40
	MOVWF	r0x6C
_00167_DS_
;#CSRC	main.c 237
;  _endasm;
	NOP	
;#CSRC	main.c 238
;  } while (--i);
	DECFSZ	r0x6C,F
	GOTO	_00167_DS_
	RETURN	
; exit point of _serialBitDelay
;***
;  pBlock Stats: dbName = C
;***
;entry:  _sensorSample	;Function start
; 2 exit points
;has an exit
;11 compiler assigned registers:
;   r0x63
;   s0xFFF
;   r0x64
;   s0x1000
;   r0x65
;   r0x66
;   r0x67
;   r0x68
;   r0x69
;   r0x6A
;   r0x6B
;; Starting pCode block
_sensorSample	;Function start
; 2 exit points
;#CSRC	main.c 165
;  int sensorSample(unsigned char transmitter, unsigned char receiver, unsigned char phase) {
	MOVWF	r0x63
	MOVF	s0xFFF,W
	MOVWF	_ADCON0
;#CSRC	main.c 171
;  PORTB &= 0x0F;               /* Disable all transmitters */  
	MOVLW	0x0f
	ANDWF	_PORTB,F
;#CSRC	main.c 172
;  TRISB = (~transmitter) & 0xF0;
	MOVWF	r0x66
	MOVLW	0xf0
	ANDWF	r0x66,W
	BSF	_STATUS,5
	MOVWF	_TRISB
	BCF	_STATUS,5
;#CSRC	main.c 176
;  if (phase == 0) phase0 = 1; else phase0 = 0;
	MOVF	s0x1000,W
	MOVWF	r0x65
	BTFSS	_STATUS,2
	GOTO	_00131_DS_
	BSF	(_sensorSample_phase0_1_1 >> 3), (_sensorSample_phase0_1_1 & 7)
	GOTO	_00132_DS_
_00131_DS_
	BCF	(_sensorSample_phase0_1_1 >> 3), (_sensorSample_phase0_1_1 & 7)
_00132_DS_
;#CSRC	main.c 177
;  if (phase == 1) phase1 = 1; else phase1 = 0;
	DECFSZ	r0x65,W
	GOTO	_00134_DS_
	BSF	(_sensorSample_phase1_1_1 >> 3), (_sensorSample_phase1_1_1 & 7)
	GOTO	_00135_DS_
_00134_DS_
	BCF	(_sensorSample_phase1_1_1 >> 3), (_sensorSample_phase1_1_1 & 7)
_00135_DS_
;#CSRC	main.c 178
;  if (phase == 2) phase2 = 1; else phase2 = 0;
	MOVF	r0x65,W
	XORLW	0x02
	BTFSS	_STATUS,2
	GOTO	_00137_DS_
	BSF	(_sensorSample_phase2_1_1 >> 3), (_sensorSample_phase2_1_1 & 7)
	GOTO	_00138_DS_
_00137_DS_
	BCF	(_sensorSample_phase2_1_1 >> 3), (_sensorSample_phase2_1_1 & 7)
_00138_DS_
;#CSRC	main.c 179
;  if (phase == 3) phase3 = 1; else phase3 = 0;
	MOVF	r0x65,W
	XORLW	0x03
	BTFSS	_STATUS,2
	GOTO	_00140_DS_
	BSF	(_sensorSample_phase3_1_1 >> 3), (_sensorSample_phase3_1_1 & 7)
	GOTO	_00162_DS_
_00140_DS_
	BCF	(_sensorSample_phase3_1_1 >> 3), (_sensorSample_phase3_1_1 & 7)
_00162_DS_
;#CSRC	main.c 186
;  do {
	MOVLW	0x03
	MOVWF	r0x67
_00142_DS_
;#CSRC	main.c 187
;  PORTB ^= transmitter;
	MOVF	r0x63,W
	XORWF	_PORTB,F
;#CSRC	main.c 188
;  NOP NOP NOP;
	NOP	
	NOP	
	NOP	
;#CSRC	main.c 189
;  NOP NOP NOP NOP NOP;
	NOP	
	NOP	
	NOP	
	NOP	
	NOP	
;#CSRC	main.c 190
;  NOP NOP NOP NOP NOP;
	NOP	
	NOP	
	NOP	
	NOP	
	NOP	
;#CSRC	main.c 191
;  NOP NOP NOP NOP NOP;
	NOP	
	NOP	
	NOP	
	NOP	
	NOP	
;#CSRC	main.c 192
;  NOP NOP;
	NOP	
	NOP	
;#CSRC	main.c 193
;  } while (--halfcycles);
	DECFSZ	r0x67,F
;;     peep 1 - test/jump to test/skip
	GOTO	_00142_DS_
	BTFSC	(_sensorSample_phase0_1_1 >> 3), (_sensorSample_phase0_1_1 & 7)
	BSF	(_GO_DONE >> 3), (_GO_DONE & 7)
	NOP	
	NOP	
	NOP	
	NOP	
	NOP	
;#CSRC	main.c 199
;  NOP NOP NOP NOP NOP;
	NOP	
	NOP	
	NOP	
	NOP	
;;     peep 1 - test/jump to test/skip
	NOP	
	BTFSC	(_sensorSample_phase1_1_1 >> 3), (_sensorSample_phase1_1_1 & 7)
	BSF	(_GO_DONE >> 3), (_GO_DONE & 7)
	NOP	
	NOP	
	NOP	
	NOP	
	NOP	
;#CSRC	main.c 203
;  NOP NOP NOP NOP NOP NOP;
	NOP	
	NOP	
	NOP	
	NOP	
	NOP	
;;     peep 1 - test/jump to test/skip
	NOP	
	BTFSC	(_sensorSample_phase2_1_1 >> 3), (_sensorSample_phase2_1_1 & 7)
	BSF	(_GO_DONE >> 3), (_GO_DONE & 7)
	NOP	
	NOP	
	NOP	
	NOP	
	NOP	
;#CSRC	main.c 207
;  NOP NOP NOP NOP NOP;
	NOP	
	NOP	
	NOP	
	NOP	
;;     peep 1 - test/jump to test/skip
	NOP	
	BTFSC	(_sensorSample_phase3_1_1 >> 3), (_sensorSample_phase3_1_1 & 7)
	BSF	(_GO_DONE >> 3), (_GO_DONE & 7)
_00153_DS_
	BTFSC	(_NOT_DONE >> 3), (_NOT_DONE & 7)
	GOTO	_00153_DS_
;#CSRC	main.c 215
;  TRISB = 0xF0;
	MOVLW	0xf0
	BSF	_STATUS,5
	MOVWF	_TRISB
;;     peep 11 - Removed redundant move
	BCF	_STATUS,5
;#CSRC	main.c 218
;  l = ADRESL;
	CLRF	r0x6A
	MOVF	_ADRESH,W
	MOVWF	r0x68
	MOVWF	r0x6A
	CLRF	r0x68
;;     peep 11 - Removed redundant move
;;     peep 4 - Removed redundant move
	BSF	_STATUS,5
	MOVF	_ADRESL,W
	BCF	_STATUS,5
	IORWF	r0x68,F
	MOVLW	0x00
	IORWF	r0x6A,F
	MOVF	r0x68,W
	MOVWF	s0xFFF
	MOVF	r0x6A,W
	RETURN	
; exit point of _sensorSample
;***
;  pBlock Stats: dbName = C
;***
;entry:  _integrateSample	;Function start
; 2 exit points
;has an exit
;functions called:
;   _sensorSample
;   _sensorSample
;   _sensorSample
;   _sensorSample
;   _sensorSample
;   _sensorSample
;   _sensorSample
;   _sensorSample
;18 compiler assigned registers:
;   r0x53
;   s0xFFF
;   r0x54
;   r0x55
;   r0x56
;   r0x57
;   r0x58
;   r0x59
;   r0x5A
;   s0x1000
;   r0x5C
;   r0x5B
;   r0x5E
;   r0x5D
;   r0x60
;   r0x5F
;   r0x62
;   r0x61
;; Starting pCode block
_integrateSample	;Function start
; 2 exit points
;#CSRC	main.c 146
;  int integrateSample(unsigned char transmitter, unsigned char receiver) {
	MOVWF	r0x53
	MOVF	s0xFFF,W
	MOVWF	r0x54
;#CSRC	main.c 147
;  signed int i=0, q=0;
	CLRF	r0x55
	CLRF	r0x56
	CLRF	r0x57
	CLRF	r0x58
;#CSRC	main.c 150
;  do {
	MOVLW	0x08
	MOVWF	r0x59
	CLRF	r0x5A
_00119_DS_
;#CSRC	main.c 152
;  i += sensorSample(transmitter, receiver, 0);
	MOVLW	0x00
	MOVWF	s0x1000
	MOVF	r0x54,W
	MOVWF	s0xFFF
	MOVF	r0x53,W
	CALL	_sensorSample
	MOVWF	r0x5C
;;     peep 9c - Removed redundant move
	MOVF	s0xFFF,W
	ADDWF	r0x55,F
	MOVF	r0x5C,W
	BTFSC	_STATUS,0
	INCFSZ	r0x5C,W
	ADDWF	r0x56,F
;#CSRC	main.c 153
;  q += sensorSample(transmitter, receiver, 1);
	MOVLW	0x01
	MOVWF	s0x1000
	MOVF	r0x54,W
	MOVWF	s0xFFF
	MOVF	r0x53,W
	CALL	_sensorSample
	MOVWF	r0x5E
;;     peep 9c - Removed redundant move
	MOVF	s0xFFF,W
	ADDWF	r0x57,F
	MOVF	r0x5E,W
	BTFSC	_STATUS,0
	INCFSZ	r0x5E,W
	ADDWF	r0x58,F
;#CSRC	main.c 154
;  i -= sensorSample(transmitter, receiver, 2);
	MOVLW	0x02
	MOVWF	s0x1000
	MOVF	r0x54,W
	MOVWF	s0xFFF
	MOVF	r0x53,W
	CALL	_sensorSample
	MOVWF	r0x60
;;     peep 9c - Removed redundant move
	MOVF	s0xFFF,W
	SUBWF	r0x55,F
	MOVF	r0x60,W
	BTFSS	_STATUS,0
	INCFSZ	r0x60,W
	SUBWF	r0x56,F
;#CSRC	main.c 155
;  q -= sensorSample(transmitter, receiver, 3);
	MOVLW	0x03
	MOVWF	s0x1000
	MOVF	r0x54,W
	MOVWF	s0xFFF
	MOVF	r0x53,W
	CALL	_sensorSample
	MOVWF	r0x62
;;     peep 9c - Removed redundant move
	MOVF	s0xFFF,W
	SUBWF	r0x57,F
	MOVF	r0x62,W
	BTFSS	_STATUS,0
	INCFSZ	r0x62,W
	SUBWF	r0x58,F
;#CSRC	main.c 156
;  } while (--numsamples);
	MOVLW	0xff
	ADDWF	r0x59,F
	BTFSS	_STATUS,0
	DECF	r0x5A,F
	MOVF	r0x59,W
	IORWF	r0x5A,W
	BTFSS	_STATUS,2
	GOTO	_00119_DS_
;#CSRC	main.c 159
;  if (i<0) i = -i;
	MOVF	r0x56,W
	ADDLW	0x80
	ADDLW	0x80
	BTFSC	_STATUS,0
	GOTO	_00123_DS_
	COMF	r0x55,F
	COMF	r0x56,F
	INCF	r0x55,F
	BTFSC	_STATUS,2
	INCF	r0x56,F
_00123_DS_
;#CSRC	main.c 160
;  if (q<0) q = -q;
	MOVF	r0x58,W
	ADDLW	0x80
	ADDLW	0x80
	BTFSC	_STATUS,0
	GOTO	_00125_DS_
	COMF	r0x57,F
	COMF	r0x58,F
	INCF	r0x57,F
	BTFSC	_STATUS,2
	INCF	r0x58,F
_00125_DS_
;#CSRC	main.c 161
;  return i+q;
	MOVF	r0x57,W
	ADDWF	r0x55,F
	MOVF	r0x58,W
	BTFSC	_STATUS,0
	INCFSZ	r0x58,W
	ADDWF	r0x56,F
	MOVF	r0x55,W
	MOVWF	s0xFFF
	MOVF	r0x56,W
	RETURN	
; exit point of _integrateSample
;***
;  pBlock Stats: dbName = C
;***
;entry:  _sampleAll	;Function start
; 2 exit points
;has an exit
;functions called:
;   _integrateSample
;   _integrateSample
;   _integrateSample
;   _integrateSample
;   _integrateSample
;   _integrateSample
;   _integrateSample
;   _integrateSample
;   _integrateSample
;   _integrateSample
;   _integrateSample
;   _integrateSample
;   _integrateSample
;   _integrateSample
;   _integrateSample
;   _integrateSample
;1 compiler assigned register :
;   s0xFFF
;; Starting pCode block
_sampleAll	;Function start
; 2 exit points
;#CSRC	main.c 135
;  reading0 = integrateSample(Transmit0, Receive0);
	MOVLW	0x81
	MOVWF	s0xFFF
	MOVLW	0x10
	CALL	_integrateSample
	MOVWF	(_reading0 + 1)
	MOVF	s0xFFF,W
	MOVWF	_reading0
;#CSRC	main.c 136
;  reading1 = integrateSample(Transmit1, Receive0);
	MOVLW	0x81
	MOVWF	s0xFFF
	MOVLW	0x20
	CALL	_integrateSample
	MOVWF	(_reading1 + 1)
	MOVF	s0xFFF,W
	MOVWF	_reading1
;#CSRC	main.c 137
;  reading2 = integrateSample(Transmit2, Receive0);
	MOVLW	0x81
	MOVWF	s0xFFF
	MOVLW	0x40
	CALL	_integrateSample
	MOVWF	(_reading2 + 1)
	MOVF	s0xFFF,W
	MOVWF	_reading2
;#CSRC	main.c 138
;  reading3 = integrateSample(Transmit3, Receive0);
	MOVLW	0x81
	MOVWF	s0xFFF
	MOVLW	0x80
	CALL	_integrateSample
	MOVWF	(_reading3 + 1)
	MOVF	s0xFFF,W
	MOVWF	_reading3
;#CSRC	main.c 139
;  reading4 = integrateSample(Transmit0, Receive1);
	MOVLW	0x89
	MOVWF	s0xFFF
	MOVLW	0x10
	CALL	_integrateSample
	MOVWF	(_reading4 + 1)
	MOVF	s0xFFF,W
	MOVWF	_reading4
;#CSRC	main.c 140
;  reading5 = integrateSample(Transmit1, Receive1);
	MOVLW	0x89
	MOVWF	s0xFFF
	MOVLW	0x20
	CALL	_integrateSample
	MOVWF	(_reading5 + 1)
	MOVF	s0xFFF,W
	MOVWF	_reading5
;#CSRC	main.c 141
;  reading6 = integrateSample(Transmit2, Receive1);
	MOVLW	0x89
	MOVWF	s0xFFF
	MOVLW	0x40
	CALL	_integrateSample
	MOVWF	(_reading6 + 1)
	MOVF	s0xFFF,W
	MOVWF	_reading6
;#CSRC	main.c 142
;  reading7 = integrateSample(Transmit3, Receive1);
	MOVLW	0x89
	MOVWF	s0xFFF
	MOVLW	0x80
	CALL	_integrateSample
	MOVWF	(_reading7 + 1)
	MOVF	s0xFFF,W
	MOVWF	_reading7
	RETURN	
; exit point of _sampleAll
;***
;  pBlock Stats: dbName = C
;***
;entry:  _initialize	;Function start
; 2 exit points
;has an exit
;; Starting pCode block
_initialize	;Function start
; 2 exit points
;#CSRC	main.c 111
;  TRISA = 0xFF;
	MOVLW	0xff
	BSF	_STATUS,5
	MOVWF	_TRISA
;#CSRC	main.c 112
;  TRISB = 0x00;
	CLRF	_TRISB
;#CSRC	main.c 113
;  TRISC = 0x00;
	CLRF	_TRISC
;#CSRC	main.c 114
;  TRISD = 0x00;
	CLRF	_TRISD
;#CSRC	main.c 115
;  TRISE = 0x04;
	MOVLW	0x04
	MOVWF	_TRISE
	BCF	_STATUS,5
;#CSRC	main.c 118
;  PORTA = 0x00;
	CLRF	_PORTA
;#CSRC	main.c 119
;  PORTB = 0x00;
	CLRF	_PORTB
;#CSRC	main.c 120
;  PORTC = 0x00;
	CLRF	_PORTC
;#CSRC	main.c 121
;  PORTD = 0x00;
	CLRF	_PORTD
;#CSRC	main.c 122
;  PORTE = 0x00;
	CLRF	_PORTE
;#CSRC	main.c 125
;  ADCON1 = 0x82;
	MOVLW	0x82
	BSF	_STATUS,5
	MOVWF	_ADCON1
;#CSRC	main.c 126
;  ADCON0 = 0x81;
	MOVLW	0x81
	BCF	_STATUS,5
	MOVWF	_ADCON0
;#CSRC	main.c 127
;  GIE = 1;
	BSF	(_GIE >> 3), (_GIE & 7)
	RETURN	
; exit point of _initialize
	end
