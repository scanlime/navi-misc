gpasm-0.10.6 alpha              main.asm    2-25-2003  14:46:13          PAGE  1


LOC  OBJECT CODE     LINE SOURCE TEXT
  VALUE
 
               00001 ;--------------------------------------------------------
               00002 ; File Created by SDCC : FreeWare ANSI-C Compiler
               00003 ; Version 0.0.0 Tue Feb 25 14:46:13 2003
               00004 
               00005 ;--------------------------------------------------------
               00006 ; PIC port for the 14-bit core
               00007 ;--------------------------------------------------------
               00008 ;       .module main
               00009         list    p=16f877
002007 3FFF    00010         __CONFIG 0x3fff
               00011         radix dec
               00012 include "p16f877.inc"
               00001         LIST
               00002 ; P16F877.INC  Standard Header File, Version 1.00    Microchip Technology, Inc.
               00373         LIST
               00013 ;--------------------------------------------------------
               00014 ; publics variables in this module
               00015 ;--------------------------------------------------------
               00016 ;       .globl _main
               00017 ;       .globl _SerialTris
               00018 ;       .globl _SerialPort
               00019 ;       .globl _Light3
               00020 ;       .globl _Light2
               00021 ;       .globl _Light1
               00022 ;       .globl _Light0
               00023 ;       .globl _RD
               00024 ;       .globl _WR
               00025 ;       .globl _WREN
               00026 ;       .globl _WRERR
               00027 ;       .globl _EEPGD
               00028 ;       .globl _PCFG0
               00029 ;       .globl _PCFG1
               00030 ;       .globl _PCFG2
               00031 ;       .globl _PCFG3
               00032 ;       .globl _ADFM
               00033 ;       .globl _TXD8
               00034 ;       .globl _TX9D
               00035 ;       .globl _TRMT
               00036 ;       .globl _BRGH
               00037 ;       .globl _SYNC
               00038 ;       .globl _TXEN
               00039 ;       .globl _TX8_9
               00040 ;       .globl _NOT_TX8
               00041 ;       .globl _TX9
               00042 ;       .globl _CSRC
               00043 ;       .globl _BF
               00044 ;       .globl _UA
               00045 ;       .globl _READ_WRITE
               00046 ;       .globl _R_W
               00047 ;       .globl _NOT_WRITE
               00048 ;       .globl _NOT_W
               00049 ;       .globl _I2C_READ
               00050 ;       .globl _R
               00051 ;       .globl _I2C_START
gpasm-0.10.6 alpha              main.asm    2-25-2003  14:46:13          PAGE  2


LOC  OBJECT CODE     LINE SOURCE TEXT
  VALUE
 
               00052 ;       .globl _S
               00053 ;       .globl _I2C_STOP
               00054 ;       .globl _P
               00055 ;       .globl _DATA_ADDRESS
               00056 ;       .globl _D_A
               00057 ;       .globl _NOT_ADDRESS
               00058 ;       .globl _NOT_A
               00059 ;       .globl _I2C_DATA
               00060 ;       .globl _D
               00061 ;       .globl _CKE
               00062 ;       .globl _SMP
               00063 ;       .globl _SEN
               00064 ;       .globl _RSEN
               00065 ;       .globl _PEN
               00066 ;       .globl _RCEN
               00067 ;       .globl _ACKEN
               00068 ;       .globl _ACKDT
               00069 ;       .globl _ACKSTAT
               00070 ;       .globl _GCEN
               00071 ;       .globl _NOT_BOR
               00072 ;       .globl _NOT_BO
               00073 ;       .globl _NOT_POR
               00074 ;       .globl _CCP2IE
               00075 ;       .globl _BCLIE
               00076 ;       .globl _EEIE
               00077 ;       .globl _TMR1IE
               00078 ;       .globl _TMR2IE
               00079 ;       .globl _CCP1IE
               00080 ;       .globl _SSPIE
               00081 ;       .globl _TXIE
               00082 ;       .globl _RCIE
               00083 ;       .globl _ADIE
               00084 ;       .globl _PSPIE
               00085 ;       .globl _TRISE0
               00086 ;       .globl _TRISE1
               00087 ;       .globl _TRISE2
               00088 ;       .globl _PSPMODE
               00089 ;       .globl _IBOV
               00090 ;       .globl _OBF
               00091 ;       .globl _IBF
               00092 ;       .globl _PS0
               00093 ;       .globl _PS1
               00094 ;       .globl _PS2
               00095 ;       .globl _PSA
               00096 ;       .globl _T0SE
               00097 ;       .globl _T0CS
               00098 ;       .globl _INTEDG
               00099 ;       .globl _NOT_RBPU
               00100 ;       .globl _ADON
               00101 ;       .globl _GO_DONE
               00102 ;       .globl _NOT_DONE
               00103 ;       .globl _GO
               00104 ;       .globl _CHS0
               00105 ;       .globl _CHS1
gpasm-0.10.6 alpha              main.asm    2-25-2003  14:46:13          PAGE  3


LOC  OBJECT CODE     LINE SOURCE TEXT
  VALUE
 
               00106 ;       .globl _CHS2
               00107 ;       .globl _ADCS0
               00108 ;       .globl _ADCS1
               00109 ;       .globl _CCP2M0
               00110 ;       .globl _CCP2M1
               00111 ;       .globl _CCP2M2
               00112 ;       .globl _CCP2M3
               00113 ;       .globl _CCP2Y
               00114 ;       .globl _CCP2X
               00115 ;       .globl _RCD8
               00116 ;       .globl _RX9D
               00117 ;       .globl _OERR
               00118 ;       .globl _FERR
               00119 ;       .globl _ADDEN
               00120 ;       .globl _CREN
               00121 ;       .globl _SREN
               00122 ;       .globl _RC8_9
               00123 ;       .globl _NOT_RC8
               00124 ;       .globl _RC9
               00125 ;       .globl _RX9
               00126 ;       .globl _SPEN
               00127 ;       .globl _CCP1M0
               00128 ;       .globl _CCP1M1
               00129 ;       .globl _CCP1M2
               00130 ;       .globl _CCP1M3
               00131 ;       .globl _CCP1Y
               00132 ;       .globl _CCP1X
               00133 ;       .globl _SSPM0
               00134 ;       .globl _SSPM1
               00135 ;       .globl _SSPM2
               00136 ;       .globl _SSPM3
               00137 ;       .globl _CKP
               00138 ;       .globl _SSPEN
               00139 ;       .globl _SSPOV
               00140 ;       .globl _WCOL
               00141 ;       .globl _T2CKPS0
               00142 ;       .globl _T2CKPS1
               00143 ;       .globl _TMR2ON
               00144 ;       .globl _TOUTPS0
               00145 ;       .globl _TOUTPS1
               00146 ;       .globl _TOUTPS2
               00147 ;       .globl _TOUTPS3
               00148 ;       .globl _TMR1ON
               00149 ;       .globl _TMR1CS
               00150 ;       .globl _T1SYNC
               00151 ;       .globl _T1INSYNC
               00152 ;       .globl _NOT_T1SYNC
               00153 ;       .globl _T1OSCEN
               00154 ;       .globl _T1CKPS0
               00155 ;       .globl _T1CKPS1
               00156 ;       .globl _CCP2IF
               00157 ;       .globl _BCLIF
               00158 ;       .globl _EEIF
               00159 ;       .globl _TMR1IF
gpasm-0.10.6 alpha              main.asm    2-25-2003  14:46:13          PAGE  4


LOC  OBJECT CODE     LINE SOURCE TEXT
  VALUE
 
               00160 ;       .globl _TMR2IF
               00161 ;       .globl _CCP1IF
               00162 ;       .globl _SSPIF
               00163 ;       .globl _TXIF
               00164 ;       .globl _RCIF
               00165 ;       .globl _ADIF
               00166 ;       .globl _PSPIF
               00167 ;       .globl _RBIF
               00168 ;       .globl _INTF
               00169 ;       .globl _T0IF
               00170 ;       .globl _RBIE
               00171 ;       .globl _INTE
               00172 ;       .globl _T0IE
               00173 ;       .globl _PEIE
               00174 ;       .globl _GIE
               00175 ;       .globl _C
               00176 ;       .globl _DC
               00177 ;       .globl _Z
               00178 ;       .globl _NOT_PD
               00179 ;       .globl _NOT_TO
               00180 ;       .globl _RP0
               00181 ;       .globl _RP1
               00182 ;       .globl _IRP
               00183 ;       .globl _reading7
               00184 ;       .globl _reading6
               00185 ;       .globl _reading5
               00186 ;       .globl _reading4
               00187 ;       .globl _reading3
               00188 ;       .globl _reading2
               00189 ;       .globl _reading1
               00190 ;       .globl _reading0
               00191 ;       .globl _PCL
               00192 ;       .globl _INDF
               00193 ;       .globl _initialize
               00194 ;       .globl _sampleAll
               00195 ;       .globl _integrateSample
               00196 ;       .globl _sensorSample
               00197 ;       .globl _serialBitDelay
               00198 ;       .globl _putc
               00199 ;       .globl _sendPacket
               00200 ;--------------------------------------------------------
               00201 ; special function registers
               00202 ;--------------------------------------------------------
  00000001     00203 _TMR0   EQU     0x0001
  00000003     00204 _STATUS EQU     0x0003
  00000004     00205 _FSR    EQU     0x0004
  00000005     00206 _PORTA  EQU     0x0005
  00000006     00207 _PORTB  EQU     0x0006
  00000007     00208 _PORTC  EQU     0x0007
  00000008     00209 _PORTD  EQU     0x0008
  00000009     00210 _PORTE  EQU     0x0009
  0000000A     00211 _PCLATH EQU     0x000a
  0000000B     00212 _INTCON EQU     0x000b
  0000000C     00213 _PIR1   EQU     0x000c
gpasm-0.10.6 alpha              main.asm    2-25-2003  14:46:13          PAGE  5


LOC  OBJECT CODE     LINE SOURCE TEXT
  VALUE
 
  0000000D     00214 _PIR2   EQU     0x000d
  0000000E     00215 _TMR1L  EQU     0x000e
  0000000F     00216 _TMR1H  EQU     0x000f
  00000010     00217 _T1CON  EQU     0x0010
  00000011     00218 _TMR2   EQU     0x0011
  00000012     00219 _T2CON  EQU     0x0012
  00000013     00220 _SSPBUF EQU     0x0013
  00000014     00221 _SSPCON EQU     0x0014
  00000015     00222 _CCPR1L EQU     0x0015
  00000016     00223 _CCPR1H EQU     0x0016
  00000017     00224 _CCP1CON        EQU     0x0017
  00000018     00225 _RCSTA  EQU     0x0018
  00000019     00226 _TXREG  EQU     0x0019
  0000001A     00227 _RCREG  EQU     0x001a
  0000001B     00228 _CCPR2L EQU     0x001b
  0000001C     00229 _CCPR2H EQU     0x001c
  0000001D     00230 _CCP2CON        EQU     0x001d
  0000001E     00231 _ADRESH EQU     0x001e
  0000001F     00232 _ADCON0 EQU     0x001f
  00000081     00233 _OPTION_REG     EQU     0x0081
  00000085     00234 _TRISA  EQU     0x0085
  00000086     00235 _TRISB  EQU     0x0086
  00000087     00236 _TRISC  EQU     0x0087
  00000088     00237 _TRISD  EQU     0x0088
  00000089     00238 _TRISE  EQU     0x0089
  0000008C     00239 _PIE1   EQU     0x008c
  0000008D     00240 _PIE2   EQU     0x008d
  0000008E     00241 _PCON   EQU     0x008e
  00000091     00242 _SSPCON2        EQU     0x0091
  00000092     00243 _PR2    EQU     0x0092
  00000093     00244 _SSPADD EQU     0x0093
  00000094     00245 _SSPSTAT        EQU     0x0094
  00000098     00246 _TXSTA  EQU     0x0098
  00000099     00247 _SPBRG  EQU     0x0099
  0000009E     00248 _ADRESL EQU     0x009e
  0000009F     00249 _ADCON1 EQU     0x009f
  0000010C     00250 _EEDATA EQU     0x010c
  0000010D     00251 _EEADR  EQU     0x010d
  0000010E     00252 _EEDATH EQU     0x010e
  0000010F     00253 _EEADRH EQU     0x010f
  0000018C     00254 _EECON1 EQU     0x018c
  0000018D     00255 _EECON2 EQU     0x018d
               00256   cblock  0X0005        ; Bank 0
               00257         _PORTA
               00258         fbitfield06
               00259         _PORTC
               00260         _PORTD
               00261         fbitfield09
               00262   endc
               00263   cblock  0X000B        ; Bank 0
               00264         fbitfield0b
               00265   endc
               00266   cblock  0X001E        ; Bank 0
               00267         _ADRESH
gpasm-0.10.6 alpha              main.asm    2-25-2003  14:46:13          PAGE  6


LOC  OBJECT CODE     LINE SOURCE TEXT
  VALUE
 
               00268         fbitfield1f
               00269         r0x53
               00270         r0x54
               00271         r0x55
               00272         r0x56
               00273         r0x57
               00274         r0x58
               00275         r0x59
               00276         r0x5A
               00277         r0x5C
               00278         r0x5E
               00279         r0x60
               00280         r0x62
               00281         r0x63
               00282         r0x65
               00283         r0x66
               00284         r0x67
               00285         r0x68
               00286         r0x6A
               00287         r0x6C
               00288         r0x6D
               00289         r0x6E
               00290         r0x6F
               00291         r0x70
               00292         r0x71
               00293         r0x72
               00294         r0x73
               00295         r0x74
               00296         r0x75
               00297         r0x76
               00298         r0x77
               00299         r0x78
               00300         r0x79
               00301         r0x7A
               00302         r0x7B
               00303         r0x7C
               00304         r0x7D
               00305         r0x7E
               00306         r0x7F
               00307         r0x80
               00308         s0xFFF
               00309         s0x1000
               00310         _reading0
               00311         _reading0_1
               00312         _reading1
               00313         _reading1_1
               00314         _reading2
               00315         _reading2_1
               00316         _reading3
               00317         _reading3_1
               00318         _reading4
               00319         _reading4_1
               00320         _reading5
               00321         _reading5_1
gpasm-0.10.6 alpha              main.asm    2-25-2003  14:46:13          PAGE  7


LOC  OBJECT CODE     LINE SOURCE TEXT
  VALUE
 
               00322         _reading6
               00323         _reading6_1
               00324         _reading7
               00325         _reading7_1
               00326         bitfield0
               00327   endc
               00328   cblock  0X0085        ; Bank 1
               00329         _TRISA
               00330         _TRISB
               00331         _TRISC
               00332         _TRISD
               00333         fbitfield89
               00334   endc
               00335   cblock  0X009E        ; Bank 1
               00336         _ADRESL
               00337         _ADCON1
               00338   endc
  00000030     00339 _Light0 EQU     ( (fbitfield06<<3)+0)
  0000005F     00340 _GIE    EQU     ( (fbitfield0b<<3)+7)
  000002C8     00341 _sensorSample_phase0_1_1        EQU     ( (bitfield0<<3)+0)
  000002C9     00342 _sensorSample_phase1_1_1        EQU     ( (bitfield0<<3)+1)
  000002CA     00343 _sensorSample_phase2_1_1        EQU     ( (bitfield0<<3)+2)
  000002CB     00344 _sensorSample_phase3_1_1        EQU     ( (bitfield0<<3)+3)
  000000FA     00345 _GO_DONE        EQU     ( (fbitfield1f<<3)+2)
  0000004A     00346 _SerialPort     EQU     ( (fbitfield09<<3)+2)
  0000044A     00347 _SerialTris     EQU     ( (fbitfield89<<3)+2)
  00000020     00348 r0x53   EQU     0x020
  00000021     00349 r0x54   EQU     0x021
  00000022     00350 r0x55   EQU     0x022
  00000023     00351 r0x56   EQU     0x023
  00000024     00352 r0x57   EQU     0x024
  00000025     00353 r0x58   EQU     0x025
  00000026     00354 r0x59   EQU     0x026
  00000027     00355 r0x5A   EQU     0x027
  00000028     00356 r0x5C   EQU     0x028
  00000029     00357 r0x5E   EQU     0x029
  0000002A     00358 r0x60   EQU     0x02a
  0000002B     00359 r0x62   EQU     0x02b
  0000002C     00360 r0x63   EQU     0x02c
  0000002D     00361 r0x65   EQU     0x02d
  0000002E     00362 r0x66   EQU     0x02e
  0000002F     00363 r0x67   EQU     0x02f
  00000030     00364 r0x68   EQU     0x030
  00000031     00365 r0x6A   EQU     0x031
  00000032     00366 r0x6C   EQU     0x032
  00000033     00367 r0x6D   EQU     0x033
  00000034     00368 r0x6E   EQU     0x034
  00000035     00369 r0x6F   EQU     0x035
  00000036     00370 r0x70   EQU     0x036
  00000037     00371 r0x71   EQU     0x037
  00000038     00372 r0x72   EQU     0x038
  00000039     00373 r0x73   EQU     0x039
  0000003A     00374 r0x74   EQU     0x03a
  0000003B     00375 r0x75   EQU     0x03b
gpasm-0.10.6 alpha              main.asm    2-25-2003  14:46:13          PAGE  8


LOC  OBJECT CODE     LINE SOURCE TEXT
  VALUE
 
  0000003C     00376 r0x76   EQU     0x03c
  0000003D     00377 r0x77   EQU     0x03d
  0000003E     00378 r0x78   EQU     0x03e
  0000003F     00379 r0x79   EQU     0x03f
  00000040     00380 r0x7A   EQU     0x040
  00000041     00381 r0x7B   EQU     0x041
  00000042     00382 r0x7C   EQU     0x042
  00000043     00383 r0x7D   EQU     0x043
  00000044     00384 r0x7E   EQU     0x044
  00000045     00385 r0x7F   EQU     0x045
  00000046     00386 r0x80   EQU     0x046
  00000085     00387 _TRISA  EQU     0x085
  00000086     00388 _TRISB  EQU     0x086
  00000087     00389 _TRISC  EQU     0x087
  00000088     00390 _TRISD  EQU     0x088
  00000089     00391 _TRISE  EQU     0x089
  00000005     00392 _PORTA  EQU     0x005
  00000006     00393 _PORTB  EQU     0x006
  00000007     00394 _PORTC  EQU     0x007
  00000008     00395 _PORTD  EQU     0x008
  00000009     00396 _PORTE  EQU     0x009
  0000009F     00397 _ADCON1 EQU     0x09f
  0000001F     00398 _ADCON0 EQU     0x01f
  00000049     00399 _reading0       EQU     0x049
  0000004B     00400 _reading1       EQU     0x04b
  0000004D     00401 _reading2       EQU     0x04d
  0000004F     00402 _reading3       EQU     0x04f
  00000051     00403 _reading4       EQU     0x051
  00000053     00404 _reading5       EQU     0x053
  00000055     00405 _reading6       EQU     0x055
  00000057     00406 _reading7       EQU     0x057
  00000006     00407 _PORTB  EQU     0x006
  00000086     00408 _TRISB  EQU     0x086
  0000001F     00409 _ADCON0 EQU     0x01f
  0000001F     00410 _ADCON0 EQU     0x01f
  00000006     00411 _PORTB  EQU     0x006
  00000086     00412 _TRISB  EQU     0x086
  0000001E     00413 _ADRESH EQU     0x01e
  0000009E     00414 _ADRESL EQU     0x09e
  00000006     00415 fbitfield06     EQU     0x006
  0000000B     00416 fbitfield0b     EQU     0x00b
  00000059     00417 bitfield0       EQU     0x059
  0000001F     00418 fbitfield1f     EQU     0x01f
  00000009     00419 fbitfield09     EQU     0x009
  00000089     00420 fbitfield89     EQU     0x089
  00000047     00421 s0xFFF  EQU     0x047
  00000048     00422 s0x1000 EQU     0x048
  00000003     00423 _STATUS EQU     0x003
  00000002     00424 PCL     EQU     0x002
  0000000A     00425 _PCLATH EQU     0x00a
  00000004     00426 FSR     EQU     0x004
  00000000     00427 INDF    EQU     0x000
  0000000B     00428 INTCON  EQU     0x00b
               00429 ;--------------------------------------------------------
gpasm-0.10.6 alpha              main.asm    2-25-2003  14:46:13          PAGE  9


LOC  OBJECT CODE     LINE SOURCE TEXT
  VALUE
 
               00430 ; overlayable items in internal ram 
               00431 ;--------------------------------------------------------
               00432 ;       .area   OSEG    (OVR,DATA)
               00433 ;--------------------------------------------------------
               00434 ; Stack segment in internal ram 
               00435 ;--------------------------------------------------------
               00436 ;       .area   SSEG    (DATA)
               00437 ;__start__stack:
               00438 ;       .ds     1
               00439 
               00440 ;--------------------------------------------------------
               00441 ; indirectly addressable internal ram data
               00442 ;--------------------------------------------------------
               00443 ;       .area   ISEG    (DATA)
               00444 ;--------------------------------------------------------
               00445 ; external ram data
               00446 ;--------------------------------------------------------
               00447 ;       .area   XSEG    (XDATA)
               00448 ;--------------------------------------------------------
               00449 ; bit data
               00450 ;--------------------------------------------------------
               00451 ;       .area   BSEG    (BIT)
  0000001F     00452 _IRP    EQU     0x001f
  0000001E     00453 _RP1    EQU     0x001e
  0000001D     00454 _RP0    EQU     0x001d
  0000001C     00455 _NOT_TO EQU     0x001c
  0000001B     00456 _NOT_PD EQU     0x001b
  0000001A     00457 _Z      EQU     0x001a
  00000019     00458 _DC     EQU     0x0019
  00000018     00459 _C      EQU     0x0018
  0000005F     00460 _GIE    EQU     0x005f
  0000005E     00461 _PEIE   EQU     0x005e
  0000005D     00462 _T0IE   EQU     0x005d
  0000005C     00463 _INTE   EQU     0x005c
  0000005B     00464 _RBIE   EQU     0x005b
  0000005A     00465 _T0IF   EQU     0x005a
  00000059     00466 _INTF   EQU     0x0059
  00000058     00467 _RBIF   EQU     0x0058
  00000067     00468 _PSPIF  EQU     0x0067
  00000066     00469 _ADIF   EQU     0x0066
  00000065     00470 _RCIF   EQU     0x0065
  00000064     00471 _TXIF   EQU     0x0064
  00000063     00472 _SSPIF  EQU     0x0063
  00000062     00473 _CCP1IF EQU     0x0062
  00000061     00474 _TMR2IF EQU     0x0061
  00000060     00475 _TMR1IF EQU     0x0060
  0000006C     00476 _EEIF   EQU     0x006c
  0000006B     00477 _BCLIF  EQU     0x006b
  00000068     00478 _CCP2IF EQU     0x0068
  00000085     00479 _T1CKPS1        EQU     0x0085
  00000084     00480 _T1CKPS0        EQU     0x0084
  00000083     00481 _T1OSCEN        EQU     0x0083
  00000082     00482 _NOT_T1SYNC     EQU     0x0082
  00000082     00483 _T1INSYNC       EQU     0x0082
gpasm-0.10.6 alpha              main.asm    2-25-2003  14:46:13          PAGE 10


LOC  OBJECT CODE     LINE SOURCE TEXT
  VALUE
 
  00000082     00484 _T1SYNC EQU     0x0082
  00000081     00485 _TMR1CS EQU     0x0081
  00000080     00486 _TMR1ON EQU     0x0080
  00000096     00487 _TOUTPS3        EQU     0x0096
  00000095     00488 _TOUTPS2        EQU     0x0095
  00000094     00489 _TOUTPS1        EQU     0x0094
  00000093     00490 _TOUTPS0        EQU     0x0093
  00000092     00491 _TMR2ON EQU     0x0092
  00000091     00492 _T2CKPS1        EQU     0x0091
  00000090     00493 _T2CKPS0        EQU     0x0090
  000000A7     00494 _WCOL   EQU     0x00a7
  000000A6     00495 _SSPOV  EQU     0x00a6
  000000A5     00496 _SSPEN  EQU     0x00a5
  000000A4     00497 _CKP    EQU     0x00a4
  000000A3     00498 _SSPM3  EQU     0x00a3
  000000A2     00499 _SSPM2  EQU     0x00a2
  000000A1     00500 _SSPM1  EQU     0x00a1
  000000A0     00501 _SSPM0  EQU     0x00a0
  000000BD     00502 _CCP1X  EQU     0x00bd
  000000BC     00503 _CCP1Y  EQU     0x00bc
  000000BB     00504 _CCP1M3 EQU     0x00bb
  000000BA     00505 _CCP1M2 EQU     0x00ba
  000000B9     00506 _CCP1M1 EQU     0x00b9
  000000B8     00507 _CCP1M0 EQU     0x00b8
  000000C7     00508 _SPEN   EQU     0x00c7
  000000C6     00509 _RX9    EQU     0x00c6
  000000C6     00510 _RC9    EQU     0x00c6
  000000C6     00511 _NOT_RC8        EQU     0x00c6
  000000C6     00512 _RC8_9  EQU     0x00c6
  000000C5     00513 _SREN   EQU     0x00c5
  000000C4     00514 _CREN   EQU     0x00c4
  000000C3     00515 _ADDEN  EQU     0x00c3
  000000C2     00516 _FERR   EQU     0x00c2
  000000C1     00517 _OERR   EQU     0x00c1
  000000C0     00518 _RX9D   EQU     0x00c0
  000000C0     00519 _RCD8   EQU     0x00c0
  000000ED     00520 _CCP2X  EQU     0x00ed
  000000EC     00521 _CCP2Y  EQU     0x00ec
  000000EB     00522 _CCP2M3 EQU     0x00eb
  000000EA     00523 _CCP2M2 EQU     0x00ea
  000000E9     00524 _CCP2M1 EQU     0x00e9
  000000E8     00525 _CCP2M0 EQU     0x00e8
  000000FF     00526 _ADCS1  EQU     0x00ff
  000000FE     00527 _ADCS0  EQU     0x00fe
  000000FD     00528 _CHS2   EQU     0x00fd
  000000FC     00529 _CHS1   EQU     0x00fc
  000000FB     00530 _CHS0   EQU     0x00fb
  000000FA     00531 _GO     EQU     0x00fa
  000000FA     00532 _NOT_DONE       EQU     0x00fa
  000000FA     00533 _GO_DONE        EQU     0x00fa
  000000F8     00534 _ADON   EQU     0x00f8
  0000040F     00535 _NOT_RBPU       EQU     0x040f
  0000040E     00536 _INTEDG EQU     0x040e
  0000040D     00537 _T0CS   EQU     0x040d
gpasm-0.10.6 alpha              main.asm    2-25-2003  14:46:13          PAGE 11


LOC  OBJECT CODE     LINE SOURCE TEXT
  VALUE
 
  0000040C     00538 _T0SE   EQU     0x040c
  0000040B     00539 _PSA    EQU     0x040b
  0000040A     00540 _PS2    EQU     0x040a
  00000409     00541 _PS1    EQU     0x0409
  00000408     00542 _PS0    EQU     0x0408
  0000044F     00543 _IBF    EQU     0x044f
  0000044E     00544 _OBF    EQU     0x044e
  0000044D     00545 _IBOV   EQU     0x044d
  0000044C     00546 _PSPMODE        EQU     0x044c
  0000044A     00547 _TRISE2 EQU     0x044a
  00000449     00548 _TRISE1 EQU     0x0449
  00000448     00549 _TRISE0 EQU     0x0448
  00000467     00550 _PSPIE  EQU     0x0467
  00000466     00551 _ADIE   EQU     0x0466
  00000465     00552 _RCIE   EQU     0x0465
  00000464     00553 _TXIE   EQU     0x0464
  00000463     00554 _SSPIE  EQU     0x0463
  00000462     00555 _CCP1IE EQU     0x0462
  00000461     00556 _TMR2IE EQU     0x0461
  00000460     00557 _TMR1IE EQU     0x0460
  0000046C     00558 _EEIE   EQU     0x046c
  0000046B     00559 _BCLIE  EQU     0x046b
  00000468     00560 _CCP2IE EQU     0x0468
  00000471     00561 _NOT_POR        EQU     0x0471
  00000470     00562 _NOT_BO EQU     0x0470
  00000470     00563 _NOT_BOR        EQU     0x0470
  0000048F     00564 _GCEN   EQU     0x048f
  0000048E     00565 _ACKSTAT        EQU     0x048e
  0000048D     00566 _ACKDT  EQU     0x048d
  0000048C     00567 _ACKEN  EQU     0x048c
  0000048B     00568 _RCEN   EQU     0x048b
  0000048A     00569 _PEN    EQU     0x048a
  00000489     00570 _RSEN   EQU     0x0489
  00000488     00571 _SEN    EQU     0x0488
  000004A7     00572 _SMP    EQU     0x04a7
  000004A6     00573 _CKE    EQU     0x04a6
  000004A5     00574 _D      EQU     0x04a5
  000004A5     00575 _I2C_DATA       EQU     0x04a5
  000004A5     00576 _NOT_A  EQU     0x04a5
  000004A5     00577 _NOT_ADDRESS    EQU     0x04a5
  000004A5     00578 _D_A    EQU     0x04a5
  000004A5     00579 _DATA_ADDRESS   EQU     0x04a5
  000004A4     00580 _P      EQU     0x04a4
  000004A4     00581 _I2C_STOP       EQU     0x04a4
  000004A3     00582 _S      EQU     0x04a3
  000004A3     00583 _I2C_START      EQU     0x04a3
  000004A2     00584 _R      EQU     0x04a2
  000004A2     00585 _I2C_READ       EQU     0x04a2
  000004A2     00586 _NOT_W  EQU     0x04a2
  000004A2     00587 _NOT_WRITE      EQU     0x04a2
  000004A2     00588 _R_W    EQU     0x04a2
  000004A2     00589 _READ_WRITE     EQU     0x04a2
  000004A1     00590 _UA     EQU     0x04a1
  000004A0     00591 _BF     EQU     0x04a0
gpasm-0.10.6 alpha              main.asm    2-25-2003  14:46:13          PAGE 12


LOC  OBJECT CODE     LINE SOURCE TEXT
  VALUE
 
  000004C7     00592 _CSRC   EQU     0x04c7
  000004C6     00593 _TX9    EQU     0x04c6
  000004C6     00594 _NOT_TX8        EQU     0x04c6
  000004C6     00595 _TX8_9  EQU     0x04c6
  000004C5     00596 _TXEN   EQU     0x04c5
  000004C4     00597 _SYNC   EQU     0x04c4
  000004C2     00598 _BRGH   EQU     0x04c2
  000004C1     00599 _TRMT   EQU     0x04c1
  000004C0     00600 _TX9D   EQU     0x04c0
  000004C0     00601 _TXD8   EQU     0x04c0
  000004FF     00602 _ADFM   EQU     0x04ff
  000004FB     00603 _PCFG3  EQU     0x04fb
  000004FA     00604 _PCFG2  EQU     0x04fa
  000004F9     00605 _PCFG1  EQU     0x04f9
  000004F8     00606 _PCFG0  EQU     0x04f8
  00000C67     00607 _EEPGD  EQU     0x0c67
  00000C63     00608 _WRERR  EQU     0x0c63
  00000C62     00609 _WREN   EQU     0x0c62
  00000C61     00610 _WR     EQU     0x0c61
  00000C60     00611 _RD     EQU     0x0c60
  00000030     00612 _Light0 EQU     0x0030
  00000031     00613 _Light1 EQU     0x0031
  00000032     00614 _Light2 EQU     0x0032
  00000033     00615 _Light3 EQU     0x0033
  0000004A     00616 _SerialPort     EQU     0x004a
  0000044A     00617 _SerialTris     EQU     0x044a
0000           00618         ORG 0
               00619 ;--------------------------------------------------------
               00620 ; interrupt vector 
               00621 ;--------------------------------------------------------
               00622 ;       .area   CSEG    (CODE)
               00623 ;__interrupt_vect:
               00624 ;       ljmp    __sdcc_gsinit_startup
               00625 ;       ljmp    _txInterrupt
               00626 ;       .ds     5
               00627 ;       reti
               00628 ;       .ds     7
               00629 ;       reti
               00630 ;       .ds     7
               00631 ;       reti
               00632 ;       .ds     7
               00633 ;       reti
               00634 ;       .ds     7
               00635 ;       reti
               00636 ;       .ds     7
               00637 ;--------------------------------------------------------
               00638 ; global & static initialisations
               00639 ;--------------------------------------------------------
               00640 ;       .area GSINIT  (CODE)
               00641 ;       .area GSFINAL (CODE)
               00642 ;       .area GSINIT  (CODE)
0000           00643 __sdcc_gsinit_startup:
               00644 ;       .area GSFINAL (CODE)
               00645 ;       ljmp    __sdcc_program_startup
gpasm-0.10.6 alpha              main.asm    2-25-2003  14:46:13          PAGE 13


LOC  OBJECT CODE     LINE SOURCE TEXT
  VALUE
 
               00646 ;--------------------------------------------------------
               00647 ; code
               00648 ;--------------------------------------------------------
               00649 ;       .area CSEG    (CODE)
               00650 ;***
               00651 ;  pBlock Stats: dbName = M
               00652 ;***
               00653 ;entry:  _main  ;Function start
               00654 ; 2 exit points
               00655 ;has an exit
               00656 ;functions called:
               00657 ;   _initialize
               00658 ;   _sampleAll
               00659 ;   _sendPacket
               00660 ;   _initialize
               00661 ;   _sampleAll
               00662 ;   _sendPacket
               00663 ;; Starting pCode block
0000           00664 _main   ;Function start
               00665 ; 2 exit points
               00666 ;#CSRC  main.c 95
               00667 ;  NOP NOP NOP NOP   /* Padding for the bootloader */
0000 0000      00668         NOP     
0001 0000      00669         NOP     
0002 0000      00670         NOP     
0003 0000      00671         NOP     
               00672 ;#CSRC  main.c 97
               00673 ;  initialize();
0004 21C0      00674         CALL    _initialize
               00675 ;#CSRC  main.c 99
               00676 ;  Light0 = 1;
0005 1406      00677         BSF     (_Light0 >> 3), (_Light0 & 7)
0006           00678 _00106_DS_
               00679 ;#CSRC  main.c 102
               00680 ;  sampleAll();
0006 2187      00681         CALL    _sampleAll
               00682 ;#CSRC  main.c 103
               00683 ;  sendPacket();
0007 200A      00684         CALL    _sendPacket
0008 2806      00685         GOTO    _00106_DS_
0009 0008      00686         RETURN  
               00687 ; exit point of _main
               00688 ;***
               00689 ;  pBlock Stats: dbName = C
               00690 ;***
               00691 ;entry:  _sendPacket    ;Function start
               00692 ; 2 exit points
               00693 ;has an exit
               00694 ;functions called:
               00695 ;   _putc
               00696 ;   _putc
               00697 ;   _putc
               00698 ;   _putc
               00699 ;   _putc
gpasm-0.10.6 alpha              main.asm    2-25-2003  14:46:13          PAGE 14


LOC  OBJECT CODE     LINE SOURCE TEXT
  VALUE
 
               00700 ;   _putc
               00701 ;   _putc
               00702 ;   _putc
               00703 ;   _putc
               00704 ;   _putc
               00705 ;   _putc
               00706 ;   _putc
               00707 ;   _putc
               00708 ;   _putc
               00709 ;   _putc
               00710 ;   _putc
               00711 ;   _putc
               00712 ;   _putc
               00713 ;   _putc
               00714 ;   _putc
               00715 ;   _putc
               00716 ;   _putc
               00717 ;   _putc
               00718 ;   _putc
               00719 ;   _putc
               00720 ;   _putc
               00721 ;   _putc
               00722 ;   _putc
               00723 ;   _putc
               00724 ;   _putc
               00725 ;   _putc
               00726 ;   _putc
               00727 ;   _putc
               00728 ;   _putc
               00729 ;   _putc
               00730 ;   _putc
               00731 ;18 compiler assigned registers:
               00732 ;   r0x6F
               00733 ;   r0x70
               00734 ;   r0x71
               00735 ;   r0x72
               00736 ;   r0x73
               00737 ;   r0x74
               00738 ;   r0x75
               00739 ;   r0x76
               00740 ;   r0x77
               00741 ;   r0x78
               00742 ;   r0x79
               00743 ;   r0x7A
               00744 ;   r0x7B
               00745 ;   r0x7C
               00746 ;   r0x7D
               00747 ;   r0x7E
               00748 ;   r0x7F
               00749 ;   r0x80
               00750 ;; Starting pCode block
000A           00751 _sendPacket     ;Function start
               00752 ; 2 exit points
               00753 ;#CSRC  main.c 272
gpasm-0.10.6 alpha              main.asm    2-25-2003  14:46:13          PAGE 15


LOC  OBJECT CODE     LINE SOURCE TEXT
  VALUE
 
               00754 ;  putc(0x80);
000A 3080      00755         MOVLW   0x80
000B 208A      00756         CALL    _putc
               00757 ;#CSRC  main.c 274
               00758 ;  h = reading0 >> 8;
000C 084A      00759         MOVF    (_reading0 + 1),W
000D 00B5      00760         MOVWF   r0x6F
000E 01B6      00761         CLRF    r0x70
000F 1BC9      00762         BTFSC   _reading0,7
0010 03B6      00763         DECF    r0x70,F
               00764 ;;     peep 11 - Removed redundant move
0011 0835      00765         MOVF    r0x6F,W
               00766 ;#CSRC  main.c 275
               00767 ;  l = reading0;
0012 0849      00768         MOVF    _reading0,W
0013 00B7      00769         MOVWF   r0x71
               00770 ;#CSRC  main.c 276
               00771 ;  putc(h);
0014 0835      00772         MOVF    r0x6F,W
0015 208A      00773         CALL    _putc
               00774 ;#CSRC  main.c 277
               00775 ;  putc(l);
0016 0837      00776         MOVF    r0x71,W
0017 208A      00777         CALL    _putc
               00778 ;#CSRC  main.c 278
               00779 ;  checksum += h + l;
0018 0837      00780         MOVF    r0x71,W
0019 0735      00781         ADDWF   r0x6F,W
001A 00B8      00782         MOVWF   r0x72
               00783 ;#CSRC  main.c 280
               00784 ;  h = reading1 >> 8;
001B 084C      00785         MOVF    (_reading1 + 1),W
001C 00B9      00786         MOVWF   r0x73
001D 01BA      00787         CLRF    r0x74
001E 1BCB      00788         BTFSC   _reading1,7
001F 03BA      00789         DECF    r0x74,F
0020 0839      00790         MOVF    r0x73,W
0021 00B5      00791         MOVWF   r0x6F
               00792 ;#CSRC  main.c 281
               00793 ;  l = reading1;
0022 084B      00794         MOVF    _reading1,W
0023 00B7      00795         MOVWF   r0x71
               00796 ;#CSRC  main.c 282
               00797 ;  putc(h);
0024 0835      00798         MOVF    r0x6F,W
0025 208A      00799         CALL    _putc
               00800 ;#CSRC  main.c 283
               00801 ;  putc(l);
0026 0837      00802         MOVF    r0x71,W
0027 208A      00803         CALL    _putc
               00804 ;#CSRC  main.c 284
               00805 ;  checksum += h + l;
0028 0837      00806         MOVF    r0x71,W
0029 0735      00807         ADDWF   r0x6F,W
gpasm-0.10.6 alpha              main.asm    2-25-2003  14:46:13          PAGE 16


LOC  OBJECT CODE     LINE SOURCE TEXT
  VALUE
 
002A 07B8      00808         ADDWF   r0x72,F
               00809 ;#CSRC  main.c 286
               00810 ;  h = reading2 >> 8;
002B 084E      00811         MOVF    (_reading2 + 1),W
002C 00BB      00812         MOVWF   r0x75
002D 01BC      00813         CLRF    r0x76
002E 1BCD      00814         BTFSC   _reading2,7
002F 03BC      00815         DECF    r0x76,F
0030 083B      00816         MOVF    r0x75,W
0031 00B5      00817         MOVWF   r0x6F
               00818 ;#CSRC  main.c 287
               00819 ;  l = reading2;
0032 084D      00820         MOVF    _reading2,W
0033 00B7      00821         MOVWF   r0x71
               00822 ;#CSRC  main.c 288
               00823 ;  putc(h);
0034 0835      00824         MOVF    r0x6F,W
0035 208A      00825         CALL    _putc
               00826 ;#CSRC  main.c 289
               00827 ;  putc(l);
0036 0837      00828         MOVF    r0x71,W
0037 208A      00829         CALL    _putc
               00830 ;#CSRC  main.c 290
               00831 ;  checksum += h + l;
0038 0837      00832         MOVF    r0x71,W
0039 0735      00833         ADDWF   r0x6F,W
003A 07B8      00834         ADDWF   r0x72,F
               00835 ;#CSRC  main.c 292
               00836 ;  h = reading3 >> 8;
003B 0850      00837         MOVF    (_reading3 + 1),W
003C 00BD      00838         MOVWF   r0x77
003D 01BE      00839         CLRF    r0x78
003E 1BCF      00840         BTFSC   _reading3,7
003F 03BE      00841         DECF    r0x78,F
0040 083D      00842         MOVF    r0x77,W
0041 00B5      00843         MOVWF   r0x6F
               00844 ;#CSRC  main.c 293
               00845 ;  l = reading3;
0042 084F      00846         MOVF    _reading3,W
0043 00B7      00847         MOVWF   r0x71
               00848 ;#CSRC  main.c 294
               00849 ;  putc(h);
0044 0835      00850         MOVF    r0x6F,W
0045 208A      00851         CALL    _putc
               00852 ;#CSRC  main.c 295
               00853 ;  putc(l);
0046 0837      00854         MOVF    r0x71,W
0047 208A      00855         CALL    _putc
               00856 ;#CSRC  main.c 296
               00857 ;  checksum += h + l;
0048 0837      00858         MOVF    r0x71,W
0049 0735      00859         ADDWF   r0x6F,W
004A 07B8      00860         ADDWF   r0x72,F
               00861 ;#CSRC  main.c 298
gpasm-0.10.6 alpha              main.asm    2-25-2003  14:46:13          PAGE 17


LOC  OBJECT CODE     LINE SOURCE TEXT
  VALUE
 
               00862 ;  h = reading4 >> 8;
004B 0852      00863         MOVF    (_reading4 + 1),W
004C 00BF      00864         MOVWF   r0x79
004D 01C0      00865         CLRF    r0x7A
004E 1BD1      00866         BTFSC   _reading4,7
004F 03C0      00867         DECF    r0x7A,F
               00868 ;#CSRC  main.c 299
               00869 ;  l = reading4;
0050 0851      00870         MOVF    _reading4,W
0051 00B7      00871         MOVWF   r0x71
               00872 ;#CSRC  main.c 300
               00873 ;  putc(h);
0052 083F      00874         MOVF    r0x79,W
0053 00B5      00875         MOVWF   r0x6F
0054 208A      00876         CALL    _putc
               00877 ;#CSRC  main.c 301
               00878 ;  putc(l);
0055 0837      00879         MOVF    r0x71,W
0056 208A      00880         CALL    _putc
               00881 ;#CSRC  main.c 302
               00882 ;  checksum += h + l;
0057 0837      00883         MOVF    r0x71,W
0058 0735      00884         ADDWF   r0x6F,W
0059 07B8      00885         ADDWF   r0x72,F
               00886 ;#CSRC  main.c 304
               00887 ;  h = reading5 >> 8;
005A 0854      00888         MOVF    (_reading5 + 1),W
005B 00C1      00889         MOVWF   r0x7B
005C 01C2      00890         CLRF    r0x7C
005D 1BD3      00891         BTFSC   _reading5,7
005E 03C2      00892         DECF    r0x7C,F
               00893 ;#CSRC  main.c 305
               00894 ;  l = reading5;
005F 0853      00895         MOVF    _reading5,W
0060 00B7      00896         MOVWF   r0x71
               00897 ;#CSRC  main.c 306
               00898 ;  putc(h);
0061 0841      00899         MOVF    r0x7B,W
0062 00B5      00900         MOVWF   r0x6F
0063 208A      00901         CALL    _putc
               00902 ;#CSRC  main.c 307
               00903 ;  putc(l);
0064 0837      00904         MOVF    r0x71,W
0065 208A      00905         CALL    _putc
               00906 ;#CSRC  main.c 308
               00907 ;  checksum += h + l;
0066 0837      00908         MOVF    r0x71,W
0067 0735      00909         ADDWF   r0x6F,W
0068 07B8      00910         ADDWF   r0x72,F
               00911 ;#CSRC  main.c 310
               00912 ;  h = reading6 >> 8;
0069 0856      00913         MOVF    (_reading6 + 1),W
006A 00C3      00914         MOVWF   r0x7D
006B 01C4      00915         CLRF    r0x7E
gpasm-0.10.6 alpha              main.asm    2-25-2003  14:46:13          PAGE 18


LOC  OBJECT CODE     LINE SOURCE TEXT
  VALUE
 
006C 1BD5      00916         BTFSC   _reading6,7
006D 03C4      00917         DECF    r0x7E,F
               00918 ;#CSRC  main.c 311
               00919 ;  l = reading6;
006E 0855      00920         MOVF    _reading6,W
006F 00B7      00921         MOVWF   r0x71
               00922 ;#CSRC  main.c 312
               00923 ;  putc(h);
0070 0843      00924         MOVF    r0x7D,W
0071 00B5      00925         MOVWF   r0x6F
0072 208A      00926         CALL    _putc
               00927 ;#CSRC  main.c 313
               00928 ;  putc(l);
0073 0837      00929         MOVF    r0x71,W
0074 208A      00930         CALL    _putc
               00931 ;#CSRC  main.c 314
               00932 ;  checksum += h + l;
0075 0837      00933         MOVF    r0x71,W
0076 0735      00934         ADDWF   r0x6F,W
0077 07B8      00935         ADDWF   r0x72,F
               00936 ;#CSRC  main.c 316
               00937 ;  h = reading7 >> 8;
0078 0858      00938         MOVF    (_reading7 + 1),W
0079 00C5      00939         MOVWF   r0x7F
007A 01C6      00940         CLRF    r0x80
007B 1BD7      00941         BTFSC   _reading7,7
007C 03C6      00942         DECF    r0x80,F
               00943 ;#CSRC  main.c 317
               00944 ;  l = reading7;
007D 0857      00945         MOVF    _reading7,W
007E 00B7      00946         MOVWF   r0x71
               00947 ;#CSRC  main.c 318
               00948 ;  putc(h);
007F 0845      00949         MOVF    r0x7F,W
0080 00B5      00950         MOVWF   r0x6F
0081 208A      00951         CALL    _putc
               00952 ;#CSRC  main.c 319
               00953 ;  putc(l);
0082 0837      00954         MOVF    r0x71,W
0083 208A      00955         CALL    _putc
               00956 ;#CSRC  main.c 320
               00957 ;  checksum += h + l;
0084 0837      00958         MOVF    r0x71,W
0085 0735      00959         ADDWF   r0x6F,W
0086 07B8      00960         ADDWF   r0x72,F
               00961 ;#CSRC  main.c 323
               00962 ;  putc(checksum);
0087 0838      00963         MOVF    r0x72,W
0088 208A      00964         CALL    _putc
0089 0008      00965         RETURN  
               00966 ; exit point of _sendPacket
               00967 ;***
               00968 ;  pBlock Stats: dbName = C
               00969 ;***
gpasm-0.10.6 alpha              main.asm    2-25-2003  14:46:13          PAGE 19


LOC  OBJECT CODE     LINE SOURCE TEXT
  VALUE
 
               00970 ;entry:  _putc  ;Function start
               00971 ; 2 exit points
               00972 ;has an exit
               00973 ;functions called:
               00974 ;   _serialBitDelay
               00975 ;   _serialBitDelay
               00976 ;   _serialBitDelay
               00977 ;   _serialBitDelay
               00978 ;   _serialBitDelay
               00979 ;   _serialBitDelay
               00980 ;2 compiler assigned registers:
               00981 ;   r0x6D
               00982 ;   r0x6E
               00983 ;; Starting pCode block
008A           00984 _putc   ;Function start
               00985 ; 2 exit points
               00986 ;#CSRC  main.c 241
               00987 ;  void putc(unsigned char byte) {
008A 00B3      00988         MOVWF   r0x6D
               00989 ;#CSRC  main.c 245
               00990 ;  SerialPort = 0;
008B 1109      00991         BCF     (_SerialPort >> 3), (_SerialPort & 7)
008C 1683      00992         BSF     _STATUS,5
               00993 ;#CSRC  main.c 246
               00994 ;  SerialTris = 0;
Message [302] : Register in operand not in bank 0. Ensure bank bits are correct.
008D 1109      00995         BCF     (_SerialTris >> 3), (_SerialTris & 7)
008E 1283      00996         BCF     _STATUS,5
               00997 ;#CSRC  main.c 247
               00998 ;  serialBitDelay();
008F 20A2      00999         CALL    _serialBitDelay
               01000 ;#CSRC  main.c 251
               01001 ;  do {
0090 3008      01002         MOVLW   0x08
0091 00B4      01003         MOVWF   r0x6E
0092           01004 _00175_DS_
               01005 ;#CSRC  main.c 252
               01006 ;  if (byte & 1)
0092 1C33      01007         BTFSS   r0x6D,0
0093 2896      01008         GOTO    _00173_DS_
               01009 ;#CSRC  main.c 253
               01010 ;  SerialPort = 1;
0094 1509      01011         BSF     (_SerialPort >> 3), (_SerialPort & 7)
0095 2897      01012         GOTO    _00174_DS_
0096           01013 _00173_DS_
               01014 ;#CSRC  main.c 255
               01015 ;  SerialPort = 0;
0096 1109      01016         BCF     (_SerialPort >> 3), (_SerialPort & 7)
0097           01017 _00174_DS_
               01018 ;#CSRC  main.c 256
               01019 ;  byte >>= 1;
0097 1003      01020         BCF     _STATUS,0
0098 0CB3      01021         RRF     r0x6D,F
               01022 ;#CSRC  main.c 257
gpasm-0.10.6 alpha              main.asm    2-25-2003  14:46:13          PAGE 20


LOC  OBJECT CODE     LINE SOURCE TEXT
  VALUE
 
               01023 ;  serialBitDelay();
0099 20A2      01024         CALL    _serialBitDelay
               01025 ;#CSRC  main.c 258
               01026 ;  } while (--bitnum);
009A 0BB4      01027         DECFSZ  r0x6E,F
009B 2892      01028         GOTO    _00175_DS_
               01029 ;#CSRC  main.c 261
               01030 ;  SerialPort = 1;
009C 1509      01031         BSF     (_SerialPort >> 3), (_SerialPort & 7)
               01032 ;#CSRC  main.c 262
               01033 ;  serialBitDelay();
009D 20A2      01034         CALL    _serialBitDelay
009E 1683      01035         BSF     _STATUS,5
               01036 ;#CSRC  main.c 263
               01037 ;  SerialTris = 1;
Message [302] : Register in operand not in bank 0. Ensure bank bits are correct.
009F 1509      01038         BSF     (_SerialTris >> 3), (_SerialTris & 7)
00A0 1283      01039         BCF     _STATUS,5
00A1 0008      01040         RETURN  
               01041 ; exit point of _putc
               01042 ;***
               01043 ;  pBlock Stats: dbName = C
               01044 ;***
               01045 ;entry:  _serialBitDelay        ;Function start
               01046 ; 2 exit points
               01047 ;has an exit
               01048 ;1 compiler assigned register :
               01049 ;   r0x6C
               01050 ;; Starting pCode block
00A2           01051 _serialBitDelay ;Function start
               01052 ; 2 exit points
               01053 ;#CSRC  main.c 234
               01054 ;  do {
00A2 3040      01055         MOVLW   0x40
00A3 00B2      01056         MOVWF   r0x6C
00A4           01057 _00167_DS_
               01058 ;#CSRC  main.c 237
               01059 ;  _endasm;
00A4 0000      01060         NOP     
               01061 ;#CSRC  main.c 238
               01062 ;  } while (--i);
00A5 0BB2      01063         DECFSZ  r0x6C,F
00A6 28A4      01064         GOTO    _00167_DS_
00A7 0008      01065         RETURN  
               01066 ; exit point of _serialBitDelay
               01067 ;***
               01068 ;  pBlock Stats: dbName = C
               01069 ;***
               01070 ;entry:  _sensorSample  ;Function start
               01071 ; 2 exit points
               01072 ;has an exit
               01073 ;11 compiler assigned registers:
               01074 ;   r0x63
               01075 ;   s0xFFF
gpasm-0.10.6 alpha              main.asm    2-25-2003  14:46:13          PAGE 21


LOC  OBJECT CODE     LINE SOURCE TEXT
  VALUE
 
               01076 ;   r0x64
               01077 ;   s0x1000
               01078 ;   r0x65
               01079 ;   r0x66
               01080 ;   r0x67
               01081 ;   r0x68
               01082 ;   r0x69
               01083 ;   r0x6A
               01084 ;   r0x6B
               01085 ;; Starting pCode block
00A8           01086 _sensorSample   ;Function start
               01087 ; 2 exit points
               01088 ;#CSRC  main.c 165
               01089 ;  int sensorSample(unsigned char transmitter, unsigned char receiver, unsigned char phase) {
00A8 00AC      01090         MOVWF   r0x63
00A9 0847      01091         MOVF    s0xFFF,W
00AA 009F      01092         MOVWF   _ADCON0
               01093 ;#CSRC  main.c 171
               01094 ;  PORTB &= 0x0F;               /* Disable all transmitters */  
00AB 300F      01095         MOVLW   0x0f
00AC 0586      01096         ANDWF   _PORTB,F
               01097 ;#CSRC  main.c 172
               01098 ;  TRISB = (~transmitter) & 0xF0;
00AD 00AE      01099         MOVWF   r0x66
00AE 30F0      01100         MOVLW   0xf0
00AF 052E      01101         ANDWF   r0x66,W
00B0 1683      01102         BSF     _STATUS,5
Message [302] : Register in operand not in bank 0. Ensure bank bits are correct.
00B1 0086      01103         MOVWF   _TRISB
00B2 1283      01104         BCF     _STATUS,5
               01105 ;#CSRC  main.c 176
               01106 ;  if (phase == 0) phase0 = 1; else phase0 = 0;
00B3 0848      01107         MOVF    s0x1000,W
00B4 00AD      01108         MOVWF   r0x65
00B5 1D03      01109         BTFSS   _STATUS,2
00B6 28B9      01110         GOTO    _00131_DS_
00B7 1459      01111         BSF     (_sensorSample_phase0_1_1 >> 3), (_sensorSample_phase0_1_1 & 7)
00B8 28BA      01112         GOTO    _00132_DS_
00B9           01113 _00131_DS_
00B9 1059      01114         BCF     (_sensorSample_phase0_1_1 >> 3), (_sensorSample_phase0_1_1 & 7)
00BA           01115 _00132_DS_
               01116 ;#CSRC  main.c 177
               01117 ;  if (phase == 1) phase1 = 1; else phase1 = 0;
00BA 0B2D      01118         DECFSZ  r0x65,W
00BB 28BE      01119         GOTO    _00134_DS_
00BC 14D9      01120         BSF     (_sensorSample_phase1_1_1 >> 3), (_sensorSample_phase1_1_1 & 7)
00BD 28BF      01121         GOTO    _00135_DS_
00BE           01122 _00134_DS_
00BE 10D9      01123         BCF     (_sensorSample_phase1_1_1 >> 3), (_sensorSample_phase1_1_1 & 7)
00BF           01124 _00135_DS_
               01125 ;#CSRC  main.c 178
               01126 ;  if (phase == 2) phase2 = 1; else phase2 = 0;
00BF 082D      01127         MOVF    r0x65,W
00C0 3A02      01128         XORLW   0x02
gpasm-0.10.6 alpha              main.asm    2-25-2003  14:46:13          PAGE 22


LOC  OBJECT CODE     LINE SOURCE TEXT
  VALUE
 
00C1 1D03      01129         BTFSS   _STATUS,2
00C2 28C5      01130         GOTO    _00137_DS_
00C3 1559      01131         BSF     (_sensorSample_phase2_1_1 >> 3), (_sensorSample_phase2_1_1 & 7)
00C4 28C6      01132         GOTO    _00138_DS_
00C5           01133 _00137_DS_
00C5 1159      01134         BCF     (_sensorSample_phase2_1_1 >> 3), (_sensorSample_phase2_1_1 & 7)
00C6           01135 _00138_DS_
               01136 ;#CSRC  main.c 179
               01137 ;  if (phase == 3) phase3 = 1; else phase3 = 0;
00C6 082D      01138         MOVF    r0x65,W
00C7 3A03      01139         XORLW   0x03
00C8 1D03      01140         BTFSS   _STATUS,2
00C9 28CC      01141         GOTO    _00140_DS_
00CA 15D9      01142         BSF     (_sensorSample_phase3_1_1 >> 3), (_sensorSample_phase3_1_1 & 7)
00CB 28CD      01143         GOTO    _00162_DS_
00CC           01144 _00140_DS_
00CC 11D9      01145         BCF     (_sensorSample_phase3_1_1 >> 3), (_sensorSample_phase3_1_1 & 7)
00CD           01146 _00162_DS_
               01147 ;#CSRC  main.c 186
               01148 ;  do {
00CD 3003      01149         MOVLW   0x03
00CE 00AF      01150         MOVWF   r0x67
00CF           01151 _00142_DS_
               01152 ;#CSRC  main.c 187
               01153 ;  PORTB ^= transmitter;
00CF 082C      01154         MOVF    r0x63,W
00D0 0686      01155         XORWF   _PORTB,F
               01156 ;#CSRC  main.c 188
               01157 ;  NOP NOP NOP;
00D1 0000      01158         NOP     
00D2 0000      01159         NOP     
00D3 0000      01160         NOP     
               01161 ;#CSRC  main.c 189
               01162 ;  NOP NOP NOP NOP NOP;
00D4 0000      01163         NOP     
00D5 0000      01164         NOP     
00D6 0000      01165         NOP     
00D7 0000      01166         NOP     
00D8 0000      01167         NOP     
               01168 ;#CSRC  main.c 190
               01169 ;  NOP NOP NOP NOP NOP;
00D9 0000      01170         NOP     
00DA 0000      01171         NOP     
00DB 0000      01172         NOP     
00DC 0000      01173         NOP     
00DD 0000      01174         NOP     
               01175 ;#CSRC  main.c 191
               01176 ;  NOP NOP NOP NOP NOP;
00DE 0000      01177         NOP     
00DF 0000      01178         NOP     
00E0 0000      01179         NOP     
00E1 0000      01180         NOP     
00E2 0000      01181         NOP     
               01182 ;#CSRC  main.c 192
gpasm-0.10.6 alpha              main.asm    2-25-2003  14:46:13          PAGE 23


LOC  OBJECT CODE     LINE SOURCE TEXT
  VALUE
 
               01183 ;  NOP NOP;
00E3 0000      01184         NOP     
00E4 0000      01185         NOP     
               01186 ;#CSRC  main.c 193
               01187 ;  } while (--halfcycles);
00E5 0BAF      01188         DECFSZ  r0x67,F
               01189 ;;     peep 1 - test/jump to test/skip
00E6 28CF      01190         GOTO    _00142_DS_
00E7 1859      01191         BTFSC   (_sensorSample_phase0_1_1 >> 3), (_sensorSample_phase0_1_1 & 7)
00E8 151F      01192         BSF     (_GO_DONE >> 3), (_GO_DONE & 7)
00E9 0000      01193         NOP     
00EA 0000      01194         NOP     
00EB 0000      01195         NOP     
00EC 0000      01196         NOP     
00ED 0000      01197         NOP     
               01198 ;#CSRC  main.c 199
               01199 ;  NOP NOP NOP NOP NOP;
00EE 0000      01200         NOP     
00EF 0000      01201         NOP     
00F0 0000      01202         NOP     
00F1 0000      01203         NOP     
               01204 ;;     peep 1 - test/jump to test/skip
00F2 0000      01205         NOP     
00F3 18D9      01206         BTFSC   (_sensorSample_phase1_1_1 >> 3), (_sensorSample_phase1_1_1 & 7)
00F4 151F      01207         BSF     (_GO_DONE >> 3), (_GO_DONE & 7)
00F5 0000      01208         NOP     
00F6 0000      01209         NOP     
00F7 0000      01210         NOP     
00F8 0000      01211         NOP     
00F9 0000      01212         NOP     
               01213 ;#CSRC  main.c 203
               01214 ;  NOP NOP NOP NOP NOP NOP;
00FA 0000      01215         NOP     
00FB 0000      01216         NOP     
00FC 0000      01217         NOP     
00FD 0000      01218         NOP     
00FE 0000      01219         NOP     
               01220 ;;     peep 1 - test/jump to test/skip
00FF 0000      01221         NOP     
0100 1959      01222         BTFSC   (_sensorSample_phase2_1_1 >> 3), (_sensorSample_phase2_1_1 & 7)
0101 151F      01223         BSF     (_GO_DONE >> 3), (_GO_DONE & 7)
0102 0000      01224         NOP     
0103 0000      01225         NOP     
0104 0000      01226         NOP     
0105 0000      01227         NOP     
0106 0000      01228         NOP     
               01229 ;#CSRC  main.c 207
               01230 ;  NOP NOP NOP NOP NOP;
0107 0000      01231         NOP     
0108 0000      01232         NOP     
0109 0000      01233         NOP     
010A 0000      01234         NOP     
               01235 ;;     peep 1 - test/jump to test/skip
010B 0000      01236         NOP     
gpasm-0.10.6 alpha              main.asm    2-25-2003  14:46:13          PAGE 24


LOC  OBJECT CODE     LINE SOURCE TEXT
  VALUE
 
010C 19D9      01237         BTFSC   (_sensorSample_phase3_1_1 >> 3), (_sensorSample_phase3_1_1 & 7)
010D 151F      01238         BSF     (_GO_DONE >> 3), (_GO_DONE & 7)
010E           01239 _00153_DS_
010E 191F      01240         BTFSC   (_NOT_DONE >> 3), (_NOT_DONE & 7)
010F 290E      01241         GOTO    _00153_DS_
               01242 ;#CSRC  main.c 215
               01243 ;  TRISB = 0xF0;
0110 30F0      01244         MOVLW   0xf0
0111 1683      01245         BSF     _STATUS,5
Message [302] : Register in operand not in bank 0. Ensure bank bits are correct.
0112 0086      01246         MOVWF   _TRISB
               01247 ;;     peep 11 - Removed redundant move
0113 1283      01248         BCF     _STATUS,5
               01249 ;#CSRC  main.c 218
               01250 ;  l = ADRESL;
0114 01B1      01251         CLRF    r0x6A
0115 081E      01252         MOVF    _ADRESH,W
0116 00B0      01253         MOVWF   r0x68
0117 00B1      01254         MOVWF   r0x6A
0118 01B0      01255         CLRF    r0x68
               01256 ;;     peep 11 - Removed redundant move
               01257 ;;     peep 4 - Removed redundant move
0119 1683      01258         BSF     _STATUS,5
Message [302] : Register in operand not in bank 0. Ensure bank bits are correct.
011A 081E      01259         MOVF    _ADRESL,W
011B 1283      01260         BCF     _STATUS,5
011C 04B0      01261         IORWF   r0x68,F
011D 3000      01262         MOVLW   0x00
011E 04B1      01263         IORWF   r0x6A,F
011F 0830      01264         MOVF    r0x68,W
0120 00C7      01265         MOVWF   s0xFFF
0121 0831      01266         MOVF    r0x6A,W
0122 0008      01267         RETURN  
               01268 ; exit point of _sensorSample
               01269 ;***
               01270 ;  pBlock Stats: dbName = C
               01271 ;***
               01272 ;entry:  _integrateSample       ;Function start
               01273 ; 2 exit points
               01274 ;has an exit
               01275 ;functions called:
               01276 ;   _sensorSample
               01277 ;   _sensorSample
               01278 ;   _sensorSample
               01279 ;   _sensorSample
               01280 ;   _sensorSample
               01281 ;   _sensorSample
               01282 ;   _sensorSample
               01283 ;   _sensorSample
               01284 ;18 compiler assigned registers:
               01285 ;   r0x53
               01286 ;   s0xFFF
               01287 ;   r0x54
               01288 ;   r0x55
gpasm-0.10.6 alpha              main.asm    2-25-2003  14:46:13          PAGE 25


LOC  OBJECT CODE     LINE SOURCE TEXT
  VALUE
 
               01289 ;   r0x56
               01290 ;   r0x57
               01291 ;   r0x58
               01292 ;   r0x59
               01293 ;   r0x5A
               01294 ;   s0x1000
               01295 ;   r0x5C
               01296 ;   r0x5B
               01297 ;   r0x5E
               01298 ;   r0x5D
               01299 ;   r0x60
               01300 ;   r0x5F
               01301 ;   r0x62
               01302 ;   r0x61
               01303 ;; Starting pCode block
0123           01304 _integrateSample        ;Function start
               01305 ; 2 exit points
               01306 ;#CSRC  main.c 146
               01307 ;  int integrateSample(unsigned char transmitter, unsigned char receiver) {
0123 00A0      01308         MOVWF   r0x53
0124 0847      01309         MOVF    s0xFFF,W
0125 00A1      01310         MOVWF   r0x54
               01311 ;#CSRC  main.c 147
               01312 ;  signed int i=0, q=0;
0126 01A2      01313         CLRF    r0x55
0127 01A3      01314         CLRF    r0x56
0128 01A4      01315         CLRF    r0x57
0129 01A5      01316         CLRF    r0x58
               01317 ;#CSRC  main.c 150
               01318 ;  do {
012A 3008      01319         MOVLW   0x08
012B 00A6      01320         MOVWF   r0x59
012C 01A7      01321         CLRF    r0x5A
012D           01322 _00119_DS_
               01323 ;#CSRC  main.c 152
               01324 ;  i += sensorSample(transmitter, receiver, 0);
012D 3000      01325         MOVLW   0x00
012E 00C8      01326         MOVWF   s0x1000
012F 0821      01327         MOVF    r0x54,W
0130 00C7      01328         MOVWF   s0xFFF
0131 0820      01329         MOVF    r0x53,W
0132 20A8      01330         CALL    _sensorSample
0133 00A8      01331         MOVWF   r0x5C
               01332 ;;     peep 9c - Removed redundant move
0134 0847      01333         MOVF    s0xFFF,W
0135 07A2      01334         ADDWF   r0x55,F
0136 0828      01335         MOVF    r0x5C,W
0137 1803      01336         BTFSC   _STATUS,0
0138 0F28      01337         INCFSZ  r0x5C,W
0139 07A3      01338         ADDWF   r0x56,F
               01339 ;#CSRC  main.c 153
               01340 ;  q += sensorSample(transmitter, receiver, 1);
013A 3001      01341         MOVLW   0x01
013B 00C8      01342         MOVWF   s0x1000
gpasm-0.10.6 alpha              main.asm    2-25-2003  14:46:13          PAGE 26


LOC  OBJECT CODE     LINE SOURCE TEXT
  VALUE
 
013C 0821      01343         MOVF    r0x54,W
013D 00C7      01344         MOVWF   s0xFFF
013E 0820      01345         MOVF    r0x53,W
013F 20A8      01346         CALL    _sensorSample
0140 00A9      01347         MOVWF   r0x5E
               01348 ;;     peep 9c - Removed redundant move
0141 0847      01349         MOVF    s0xFFF,W
0142 07A4      01350         ADDWF   r0x57,F
0143 0829      01351         MOVF    r0x5E,W
0144 1803      01352         BTFSC   _STATUS,0
0145 0F29      01353         INCFSZ  r0x5E,W
0146 07A5      01354         ADDWF   r0x58,F
               01355 ;#CSRC  main.c 154
               01356 ;  i -= sensorSample(transmitter, receiver, 2);
0147 3002      01357         MOVLW   0x02
0148 00C8      01358         MOVWF   s0x1000
0149 0821      01359         MOVF    r0x54,W
014A 00C7      01360         MOVWF   s0xFFF
014B 0820      01361         MOVF    r0x53,W
014C 20A8      01362         CALL    _sensorSample
014D 00AA      01363         MOVWF   r0x60
               01364 ;;     peep 9c - Removed redundant move
014E 0847      01365         MOVF    s0xFFF,W
014F 02A2      01366         SUBWF   r0x55,F
0150 082A      01367         MOVF    r0x60,W
0151 1C03      01368         BTFSS   _STATUS,0
0152 0F2A      01369         INCFSZ  r0x60,W
0153 02A3      01370         SUBWF   r0x56,F
               01371 ;#CSRC  main.c 155
               01372 ;  q -= sensorSample(transmitter, receiver, 3);
0154 3003      01373         MOVLW   0x03
0155 00C8      01374         MOVWF   s0x1000
0156 0821      01375         MOVF    r0x54,W
0157 00C7      01376         MOVWF   s0xFFF
0158 0820      01377         MOVF    r0x53,W
0159 20A8      01378         CALL    _sensorSample
015A 00AB      01379         MOVWF   r0x62
               01380 ;;     peep 9c - Removed redundant move
015B 0847      01381         MOVF    s0xFFF,W
015C 02A4      01382         SUBWF   r0x57,F
015D 082B      01383         MOVF    r0x62,W
015E 1C03      01384         BTFSS   _STATUS,0
015F 0F2B      01385         INCFSZ  r0x62,W
0160 02A5      01386         SUBWF   r0x58,F
               01387 ;#CSRC  main.c 156
               01388 ;  } while (--numsamples);
0161 30FF      01389         MOVLW   0xff
0162 07A6      01390         ADDWF   r0x59,F
0163 1C03      01391         BTFSS   _STATUS,0
0164 03A7      01392         DECF    r0x5A,F
0165 0826      01393         MOVF    r0x59,W
0166 0427      01394         IORWF   r0x5A,W
0167 1D03      01395         BTFSS   _STATUS,2
0168 292D      01396         GOTO    _00119_DS_
gpasm-0.10.6 alpha              main.asm    2-25-2003  14:46:13          PAGE 27


LOC  OBJECT CODE     LINE SOURCE TEXT
  VALUE
 
               01397 ;#CSRC  main.c 159
               01398 ;  if (i<0) i = -i;
0169 0823      01399         MOVF    r0x56,W
016A 3E80      01400         ADDLW   0x80
016B 3E80      01401         ADDLW   0x80
016C 1803      01402         BTFSC   _STATUS,0
016D 2973      01403         GOTO    _00123_DS_
016E 09A2      01404         COMF    r0x55,F
016F 09A3      01405         COMF    r0x56,F
0170 0AA2      01406         INCF    r0x55,F
0171 1903      01407         BTFSC   _STATUS,2
0172 0AA3      01408         INCF    r0x56,F
0173           01409 _00123_DS_
               01410 ;#CSRC  main.c 160
               01411 ;  if (q<0) q = -q;
0173 0825      01412         MOVF    r0x58,W
0174 3E80      01413         ADDLW   0x80
0175 3E80      01414         ADDLW   0x80
0176 1803      01415         BTFSC   _STATUS,0
0177 297D      01416         GOTO    _00125_DS_
0178 09A4      01417         COMF    r0x57,F
0179 09A5      01418         COMF    r0x58,F
017A 0AA4      01419         INCF    r0x57,F
017B 1903      01420         BTFSC   _STATUS,2
017C 0AA5      01421         INCF    r0x58,F
017D           01422 _00125_DS_
               01423 ;#CSRC  main.c 161
               01424 ;  return i+q;
017D 0824      01425         MOVF    r0x57,W
017E 07A2      01426         ADDWF   r0x55,F
017F 0825      01427         MOVF    r0x58,W
0180 1803      01428         BTFSC   _STATUS,0
0181 0F25      01429         INCFSZ  r0x58,W
0182 07A3      01430         ADDWF   r0x56,F
0183 0822      01431         MOVF    r0x55,W
0184 00C7      01432         MOVWF   s0xFFF
0185 0823      01433         MOVF    r0x56,W
0186 0008      01434         RETURN  
               01435 ; exit point of _integrateSample
               01436 ;***
               01437 ;  pBlock Stats: dbName = C
               01438 ;***
               01439 ;entry:  _sampleAll     ;Function start
               01440 ; 2 exit points
               01441 ;has an exit
               01442 ;functions called:
               01443 ;   _integrateSample
               01444 ;   _integrateSample
               01445 ;   _integrateSample
               01446 ;   _integrateSample
               01447 ;   _integrateSample
               01448 ;   _integrateSample
               01449 ;   _integrateSample
               01450 ;   _integrateSample
gpasm-0.10.6 alpha              main.asm    2-25-2003  14:46:13          PAGE 28


LOC  OBJECT CODE     LINE SOURCE TEXT
  VALUE
 
               01451 ;   _integrateSample
               01452 ;   _integrateSample
               01453 ;   _integrateSample
               01454 ;   _integrateSample
               01455 ;   _integrateSample
               01456 ;   _integrateSample
               01457 ;   _integrateSample
               01458 ;   _integrateSample
               01459 ;1 compiler assigned register :
               01460 ;   s0xFFF
               01461 ;; Starting pCode block
0187           01462 _sampleAll      ;Function start
               01463 ; 2 exit points
               01464 ;#CSRC  main.c 135
               01465 ;  reading0 = integrateSample(Transmit0, Receive0);
0187 3081      01466         MOVLW   0x81
0188 00C7      01467         MOVWF   s0xFFF
0189 3010      01468         MOVLW   0x10
018A 2123      01469         CALL    _integrateSample
018B 00CA      01470         MOVWF   (_reading0 + 1)
018C 0847      01471         MOVF    s0xFFF,W
018D 00C9      01472         MOVWF   _reading0
               01473 ;#CSRC  main.c 136
               01474 ;  reading1 = integrateSample(Transmit1, Receive0);
018E 3081      01475         MOVLW   0x81
018F 00C7      01476         MOVWF   s0xFFF
0190 3020      01477         MOVLW   0x20
0191 2123      01478         CALL    _integrateSample
0192 00CC      01479         MOVWF   (_reading1 + 1)
0193 0847      01480         MOVF    s0xFFF,W
0194 00CB      01481         MOVWF   _reading1
               01482 ;#CSRC  main.c 137
               01483 ;  reading2 = integrateSample(Transmit2, Receive0);
0195 3081      01484         MOVLW   0x81
0196 00C7      01485         MOVWF   s0xFFF
0197 3040      01486         MOVLW   0x40
0198 2123      01487         CALL    _integrateSample
0199 00CE      01488         MOVWF   (_reading2 + 1)
019A 0847      01489         MOVF    s0xFFF,W
019B 00CD      01490         MOVWF   _reading2
               01491 ;#CSRC  main.c 138
               01492 ;  reading3 = integrateSample(Transmit3, Receive0);
019C 3081      01493         MOVLW   0x81
019D 00C7      01494         MOVWF   s0xFFF
019E 3080      01495         MOVLW   0x80
019F 2123      01496         CALL    _integrateSample
01A0 00D0      01497         MOVWF   (_reading3 + 1)
01A1 0847      01498         MOVF    s0xFFF,W
01A2 00CF      01499         MOVWF   _reading3
               01500 ;#CSRC  main.c 139
               01501 ;  reading4 = integrateSample(Transmit0, Receive1);
01A3 3089      01502         MOVLW   0x89
01A4 00C7      01503         MOVWF   s0xFFF
01A5 3010      01504         MOVLW   0x10
gpasm-0.10.6 alpha              main.asm    2-25-2003  14:46:13          PAGE 29


LOC  OBJECT CODE     LINE SOURCE TEXT
  VALUE
 
01A6 2123      01505         CALL    _integrateSample
01A7 00D2      01506         MOVWF   (_reading4 + 1)
01A8 0847      01507         MOVF    s0xFFF,W
01A9 00D1      01508         MOVWF   _reading4
               01509 ;#CSRC  main.c 140
               01510 ;  reading5 = integrateSample(Transmit1, Receive1);
01AA 3089      01511         MOVLW   0x89
01AB 00C7      01512         MOVWF   s0xFFF
01AC 3020      01513         MOVLW   0x20
01AD 2123      01514         CALL    _integrateSample
01AE 00D4      01515         MOVWF   (_reading5 + 1)
01AF 0847      01516         MOVF    s0xFFF,W
01B0 00D3      01517         MOVWF   _reading5
               01518 ;#CSRC  main.c 141
               01519 ;  reading6 = integrateSample(Transmit2, Receive1);
01B1 3089      01520         MOVLW   0x89
01B2 00C7      01521         MOVWF   s0xFFF
01B3 3040      01522         MOVLW   0x40
01B4 2123      01523         CALL    _integrateSample
01B5 00D6      01524         MOVWF   (_reading6 + 1)
01B6 0847      01525         MOVF    s0xFFF,W
01B7 00D5      01526         MOVWF   _reading6
               01527 ;#CSRC  main.c 142
               01528 ;  reading7 = integrateSample(Transmit3, Receive1);
01B8 3089      01529         MOVLW   0x89
01B9 00C7      01530         MOVWF   s0xFFF
01BA 3080      01531         MOVLW   0x80
01BB 2123      01532         CALL    _integrateSample
01BC 00D8      01533         MOVWF   (_reading7 + 1)
01BD 0847      01534         MOVF    s0xFFF,W
01BE 00D7      01535         MOVWF   _reading7
01BF 0008      01536         RETURN  
               01537 ; exit point of _sampleAll
               01538 ;***
               01539 ;  pBlock Stats: dbName = C
               01540 ;***
               01541 ;entry:  _initialize    ;Function start
               01542 ; 2 exit points
               01543 ;has an exit
               01544 ;; Starting pCode block
01C0           01545 _initialize     ;Function start
               01546 ; 2 exit points
               01547 ;#CSRC  main.c 111
               01548 ;  TRISA = 0xFF;
01C0 30FF      01549         MOVLW   0xff
01C1 1683      01550         BSF     _STATUS,5
Message [302] : Register in operand not in bank 0. Ensure bank bits are correct.
01C2 0085      01551         MOVWF   _TRISA
               01552 ;#CSRC  main.c 112
               01553 ;  TRISB = 0x00;
Message [302] : Register in operand not in bank 0. Ensure bank bits are correct.
01C3 0186      01554         CLRF    _TRISB
               01555 ;#CSRC  main.c 113
               01556 ;  TRISC = 0x00;
gpasm-0.10.6 alpha              main.asm    2-25-2003  14:46:13          PAGE 30


LOC  OBJECT CODE     LINE SOURCE TEXT
  VALUE
 
Message [302] : Register in operand not in bank 0. Ensure bank bits are correct.
01C4 0187      01557         CLRF    _TRISC
               01558 ;#CSRC  main.c 114
               01559 ;  TRISD = 0x00;
Message [302] : Register in operand not in bank 0. Ensure bank bits are correct.
01C5 0188      01560         CLRF    _TRISD
               01561 ;#CSRC  main.c 115
               01562 ;  TRISE = 0x04;
01C6 3004      01563         MOVLW   0x04
Message [302] : Register in operand not in bank 0. Ensure bank bits are correct.
01C7 0089      01564         MOVWF   _TRISE
01C8 1283      01565         BCF     _STATUS,5
               01566 ;#CSRC  main.c 118
               01567 ;  PORTA = 0x00;
01C9 0185      01568         CLRF    _PORTA
               01569 ;#CSRC  main.c 119
               01570 ;  PORTB = 0x00;
01CA 0186      01571         CLRF    _PORTB
               01572 ;#CSRC  main.c 120
               01573 ;  PORTC = 0x00;
01CB 0187      01574         CLRF    _PORTC
               01575 ;#CSRC  main.c 121
               01576 ;  PORTD = 0x00;
01CC 0188      01577         CLRF    _PORTD
               01578 ;#CSRC  main.c 122
               01579 ;  PORTE = 0x00;
01CD 0189      01580         CLRF    _PORTE
               01581 ;#CSRC  main.c 125
               01582 ;  ADCON1 = 0x82;
01CE 3082      01583         MOVLW   0x82
01CF 1683      01584         BSF     _STATUS,5
Message [302] : Register in operand not in bank 0. Ensure bank bits are correct.
01D0 009F      01585         MOVWF   _ADCON1
               01586 ;#CSRC  main.c 126
               01587 ;  ADCON0 = 0x81;
01D1 3081      01588         MOVLW   0x81
01D2 1283      01589         BCF     _STATUS,5
01D3 009F      01590         MOVWF   _ADCON0
               01591 ;#CSRC  main.c 127
               01592 ;  GIE = 1;
01D4 178B      01593         BSF     (_GIE >> 3), (_GIE & 7)
01D5 0008      01594         RETURN  
               01595 ; exit point of _initialize
gpasm-0.10.6 alpha              main.asm    2-25-2003  14:46:13          PAGE 31


SYMBOL TABLE
  LABEL                             VALUE 

ACKDT                             00000005
ACKEN                             00000004
ACKSTAT                           00000006
ADCON0                            0000001F
ADCON1                            0000009F
ADCS0                             00000006
ADCS1                             00000007
ADDEN                             00000003
ADFM                              00000007
ADIE                              00000006
ADIF                              00000006
ADON                              00000000
ADRESH                            0000001E
ADRESL                            0000009E
BCLIE                             00000003
BCLIF                             00000003
BF                                00000000
BRGH                              00000002
C                                 00000000
CCP1CON                           00000017
CCP1IE                            00000002
CCP1IF                            00000002
CCP1M0                            00000000
CCP1M1                            00000001
CCP1M2                            00000002
CCP1M3                            00000003
CCP1X                             00000005
CCP1Y                             00000004
CCP2CON                           0000001D
CCP2IE                            00000000
CCP2IF                            00000000
CCP2M0                            00000000
CCP2M1                            00000001
CCP2M2                            00000002
CCP2M3                            00000003
CCP2X                             00000005
CCP2Y                             00000004
CCPR1H                            00000016
CCPR1L                            00000015
CCPR2H                            0000001C
CCPR2L                            0000001B
CHS0                              00000003
CHS1                              00000004
CHS2                              00000005
CKE                               00000006
CKP                               00000004
CREN                              00000004
CSRC                              00000007
D                                 00000005
DATA_ADDRESS                      00000005
DC                                00000001
D_A                               00000005
EEADR                             0000010D
gpasm-0.10.6 alpha              main.asm    2-25-2003  14:46:13          PAGE 32


LOC  OBJECT CODE     LINE SOURCE TEXT
  VALUE
 
EEADRH                            0000010F
EECON1                            0000018C
EECON2                            0000018D
EEDATA                            0000010C
EEDATH                            0000010E
EEIE                              00000004
EEIF                              00000004
EEPGD                             00000007
F                                 00000001
FERR                              00000002
FSR                               00000004
GCEN                              00000007
GIE                               00000007
GO                                00000002
GO_DONE                           00000002
I2C_DATA                          00000005
I2C_READ                          00000002
I2C_START                         00000003
I2C_STOP                          00000004
IBF                               00000007
IBOV                              00000005
INDF                              00000000
INTCON                            0000000B
INTE                              00000004
INTEDG                            00000006
INTF                              00000001
IRP                               00000007
NOT_A                             00000005
NOT_ADDRESS                       00000005
NOT_BO                            00000000
NOT_BOR                           00000000
NOT_DONE                          00000002
NOT_PD                            00000003
NOT_POR                           00000001
NOT_RBPU                          00000007
NOT_RC8                           00000006
NOT_T1SYNC                        00000002
NOT_TO                            00000004
NOT_TX8                           00000006
NOT_W                             00000002
NOT_WRITE                         00000002
OBF                               00000006
OERR                              00000001
OPTION_REG                        00000081
P                                 00000004
PCFG0                             00000000
PCFG1                             00000001
PCFG2                             00000002
PCFG3                             00000003
PCL                               00000002
PCLATH                            0000000A
PCON                              0000008E
PEIE                              00000006
PEN                               00000002
gpasm-0.10.6 alpha              main.asm    2-25-2003  14:46:13          PAGE 33


LOC  OBJECT CODE     LINE SOURCE TEXT
  VALUE
 
PIE1                              0000008C
PIE2                              0000008D
PIR1                              0000000C
PIR2                              0000000D
PORTA                             00000005
PORTB                             00000006
PORTC                             00000007
PORTD                             00000008
PORTE                             00000009
PR2                               00000092
PS0                               00000000
PS1                               00000001
PS2                               00000002
PSA                               00000003
PSPIE                             00000007
PSPIF                             00000007
PSPMODE                           00000004
R                                 00000002
RBIE                              00000003
RBIF                              00000000
RC8_9                             00000006
RC9                               00000006
RCD8                              00000000
RCEN                              00000003
RCIE                              00000005
RCIF                              00000005
RCREG                             0000001A
RCSTA                             00000018
RD                                00000000
READ_WRITE                        00000002
RP0                               00000005
RP1                               00000006
RSEN                              00000001
RX9                               00000006
RX9D                              00000000
R_W                               00000002
S                                 00000003
SEN                               00000000
SMP                               00000007
SPBRG                             00000099
SPEN                              00000007
SREN                              00000005
SSPADD                            00000093
SSPBUF                            00000013
SSPCON                            00000014
SSPCON2                           00000091
SSPEN                             00000005
SSPIE                             00000003
SSPIF                             00000003
SSPM0                             00000000
SSPM1                             00000001
SSPM2                             00000002
SSPM3                             00000003
SSPOV                             00000006
gpasm-0.10.6 alpha              main.asm    2-25-2003  14:46:13          PAGE 34


LOC  OBJECT CODE     LINE SOURCE TEXT
  VALUE
 
SSPSTAT                           00000094
STATUS                            00000003
SYNC                              00000004
T0CS                              00000005
T0IE                              00000005
T0IF                              00000002
T0SE                              00000004
T1CKPS0                           00000004
T1CKPS1                           00000005
T1CON                             00000010
T1INSYNC                          00000002
T1OSCEN                           00000003
T1SYNC                            00000002
T2CKPS0                           00000000
T2CKPS1                           00000001
T2CON                             00000012
TMR0                              00000001
TMR1CS                            00000001
TMR1H                             0000000F
TMR1IE                            00000000
TMR1IF                            00000000
TMR1L                             0000000E
TMR1ON                            00000000
TMR2                              00000011
TMR2IE                            00000001
TMR2IF                            00000001
TMR2ON                            00000002
TOUTPS0                           00000003
TOUTPS1                           00000004
TOUTPS2                           00000005
TOUTPS3                           00000006
TRISA                             00000085
TRISB                             00000086
TRISC                             00000087
TRISD                             00000088
TRISE                             00000089
TRISE0                            00000000
TRISE1                            00000001
TRISE2                            00000002
TRMT                              00000001
TX8_9                             00000006
TX9                               00000006
TX9D                              00000000
TXD8                              00000000
TXEN                              00000005
TXIE                              00000004
TXIF                              00000004
TXREG                             00000019
TXSTA                             00000098
UA                                00000001
W                                 00000000
WCOL                              00000007
WR                                00000001
WREN                              00000002
gpasm-0.10.6 alpha              main.asm    2-25-2003  14:46:13          PAGE 35


LOC  OBJECT CODE     LINE SOURCE TEXT
  VALUE
 
WRERR                             00000003
Z                                 00000002
_00106_DS_                        00000006
_00119_DS_                        0000012D
_00123_DS_                        00000173
_00125_DS_                        0000017D
_00131_DS_                        000000B9
_00132_DS_                        000000BA
_00134_DS_                        000000BE
_00135_DS_                        000000BF
_00137_DS_                        000000C5
_00138_DS_                        000000C6
_00140_DS_                        000000CC
_00142_DS_                        000000CF
_00153_DS_                        0000010E
_00162_DS_                        000000CD
_00167_DS_                        000000A4
_00173_DS_                        00000096
_00174_DS_                        00000097
_00175_DS_                        00000092
_ACKDT                            0000048D
_ACKEN                            0000048C
_ACKSTAT                          0000048E
_ADCON0                           0000001F
_ADCON1                           0000009F
_ADCS0                            000000FE
_ADCS1                            000000FF
_ADDEN                            000000C3
_ADFM                             000004FF
_ADIE                             00000466
_ADIF                             00000066
_ADON                             000000F8
_ADRESH                           0000001E
_ADRESL                           0000009E
_BCLIE                            0000046B
_BCLIF                            0000006B
_BF                               000004A0
_BODEN_OFF                        00003FBF
_BODEN_ON                         00003FFF
_BRGH                             000004C2
_C                                00000018
_CCP1CON                          00000017
_CCP1IE                           00000462
_CCP1IF                           00000062
_CCP1M0                           000000B8
_CCP1M1                           000000B9
_CCP1M2                           000000BA
_CCP1M3                           000000BB
_CCP1X                            000000BD
_CCP1Y                            000000BC
_CCP2CON                          0000001D
_CCP2IE                           00000468
_CCP2IF                           00000068
_CCP2M0                           000000E8
gpasm-0.10.6 alpha              main.asm    2-25-2003  14:46:13          PAGE 36


LOC  OBJECT CODE     LINE SOURCE TEXT
  VALUE
 
_CCP2M1                           000000E9
_CCP2M2                           000000EA
_CCP2M3                           000000EB
_CCP2X                            000000ED
_CCP2Y                            000000EC
_CCPR1H                           00000016
_CCPR1L                           00000015
_CCPR2H                           0000001C
_CCPR2L                           0000001B
_CHS0                             000000FB
_CHS1                             000000FC
_CHS2                             000000FD
_CKE                              000004A6
_CKP                              000000A4
_CPD_OFF                          00003FFF
_CPD_ON                           00003EFF
_CP_ALL                           00000FCF
_CP_HALF                          00001FDF
_CP_OFF                           00003FFF
_CP_UPPER_256                     00002FEF
_CREN                             000000C4
_CSRC                             000004C7
_D                                000004A5
_DATA_ADDRESS                     000004A5
_DC                               00000019
_DEBUG_OFF                        00003FFF
_DEBUG_ON                         000037FF
_D_A                              000004A5
_EEADR                            0000010D
_EEADRH                           0000010F
_EECON1                           0000018C
_EECON2                           0000018D
_EEDATA                           0000010C
_EEDATH                           0000010E
_EEIE                             0000046C
_EEIF                             0000006C
_EEPGD                            00000C67
_FERR                             000000C2
_FSR                              00000004
_GCEN                             0000048F
_GIE                              0000005F
_GO                               000000FA
_GO_DONE                          000000FA
_HS_OSC                           00003FFE
_I2C_DATA                         000004A5
_I2C_READ                         000004A2
_I2C_START                        000004A3
_I2C_STOP                         000004A4
_IBF                              0000044F
_IBOV                             0000044D
_INTCON                           0000000B
_INTE                             0000005C
_INTEDG                           0000040E
_INTF                             00000059
gpasm-0.10.6 alpha              main.asm    2-25-2003  14:46:13          PAGE 37


LOC  OBJECT CODE     LINE SOURCE TEXT
  VALUE
 
_IRP                              0000001F
_LP_OSC                           00003FFC
_LVP_OFF                          00003F7F
_LVP_ON                           00003FFF
_Light0                           00000030
_Light1                           00000031
_Light2                           00000032
_Light3                           00000033
_NOT_A                            000004A5
_NOT_ADDRESS                      000004A5
_NOT_BO                           00000470
_NOT_BOR                          00000470
_NOT_DONE                         000000FA
_NOT_PD                           0000001B
_NOT_POR                          00000471
_NOT_RBPU                         0000040F
_NOT_RC8                          000000C6
_NOT_T1SYNC                       00000082
_NOT_TO                           0000001C
_NOT_TX8                          000004C6
_NOT_W                            000004A2
_NOT_WRITE                        000004A2
_OBF                              0000044E
_OERR                             000000C1
_OPTION_REG                       00000081
_P                                000004A4
_PCFG0                            000004F8
_PCFG1                            000004F9
_PCFG2                            000004FA
_PCFG3                            000004FB
_PCLATH                           0000000A
_PCON                             0000008E
_PEIE                             0000005E
_PEN                              0000048A
_PIE1                             0000008C
_PIE2                             0000008D
_PIR1                             0000000C
_PIR2                             0000000D
_PORTA                            00000005
_PORTB                            00000006
_PORTC                            00000007
_PORTD                            00000008
_PORTE                            00000009
_PR2                              00000092
_PS0                              00000408
_PS1                              00000409
_PS2                              0000040A
_PSA                              0000040B
_PSPIE                            00000467
_PSPIF                            00000067
_PSPMODE                          0000044C
_PWRTE_OFF                        00003FFF
_PWRTE_ON                         00003FF7
_R                                000004A2
gpasm-0.10.6 alpha              main.asm    2-25-2003  14:46:13          PAGE 38


LOC  OBJECT CODE     LINE SOURCE TEXT
  VALUE
 
_RBIE                             0000005B
_RBIF                             00000058
_RC8_9                            000000C6
_RC9                              000000C6
_RCD8                             000000C0
_RCEN                             0000048B
_RCIE                             00000465
_RCIF                             00000065
_RCREG                            0000001A
_RCSTA                            00000018
_RC_OSC                           00003FFF
_RD                               00000C60
_READ_WRITE                       000004A2
_RP0                              0000001D
_RP1                              0000001E
_RSEN                             00000489
_RX9                              000000C6
_RX9D                             000000C0
_R_W                              000004A2
_S                                000004A3
_SEN                              00000488
_SMP                              000004A7
_SPBRG                            00000099
_SPEN                             000000C7
_SREN                             000000C5
_SSPADD                           00000093
_SSPBUF                           00000013
_SSPCON                           00000014
_SSPCON2                          00000091
_SSPEN                            000000A5
_SSPIE                            00000463
_SSPIF                            00000063
_SSPM0                            000000A0
_SSPM1                            000000A1
_SSPM2                            000000A2
_SSPM3                            000000A3
_SSPOV                            000000A6
_SSPSTAT                          00000094
_STATUS                           00000003
_SYNC                             000004C4
_SerialPort                       0000004A
_SerialTris                       0000044A
_T0CS                             0000040D
_T0IE                             0000005D
_T0IF                             0000005A
_T0SE                             0000040C
_T1CKPS0                          00000084
_T1CKPS1                          00000085
_T1CON                            00000010
_T1INSYNC                         00000082
_T1OSCEN                          00000083
_T1SYNC                           00000082
_T2CKPS0                          00000090
_T2CKPS1                          00000091
gpasm-0.10.6 alpha              main.asm    2-25-2003  14:46:13          PAGE 39


LOC  OBJECT CODE     LINE SOURCE TEXT
  VALUE
 
_T2CON                            00000012
_TMR0                             00000001
_TMR1CS                           00000081
_TMR1H                            0000000F
_TMR1IE                           00000460
_TMR1IF                           00000060
_TMR1L                            0000000E
_TMR1ON                           00000080
_TMR2                             00000011
_TMR2IE                           00000461
_TMR2IF                           00000061
_TMR2ON                           00000092
_TOUTPS0                          00000093
_TOUTPS1                          00000094
_TOUTPS2                          00000095
_TOUTPS3                          00000096
_TRISA                            00000085
_TRISB                            00000086
_TRISC                            00000087
_TRISD                            00000088
_TRISE                            00000089
_TRISE0                           00000448
_TRISE1                           00000449
_TRISE2                           0000044A
_TRMT                             000004C1
_TX8_9                            000004C6
_TX9                              000004C6
_TX9D                             000004C0
_TXD8                             000004C0
_TXEN                             000004C5
_TXIE                             00000464
_TXIF                             00000064
_TXREG                            00000019
_TXSTA                            00000098
_UA                               000004A1
_WCOL                             000000A7
_WDT_OFF                          00003FFB
_WDT_ON                           00003FFF
_WR                               00000C61
_WREN                             00000C62
_WRERR                            00000C63
_WRT_ENABLE_OFF                   00003DFF
_WRT_ENABLE_ON                    00003FFF
_XT_OSC                           00003FFD
_Z                                0000001A
__16F877                          00000001
__sdcc_gsinit_startup             00000000
_initialize                       000001C0
_integrateSample                  00000123
_main                             00000000
_putc                             0000008A
_reading0                         00000049
_reading0_1                       0000004A
_reading1                         0000004B
gpasm-0.10.6 alpha              main.asm    2-25-2003  14:46:13          PAGE 40


LOC  OBJECT CODE     LINE SOURCE TEXT
  VALUE
 
_reading1_1                       0000004C
_reading2                         0000004D
_reading2_1                       0000004E
_reading3                         0000004F
_reading3_1                       00000050
_reading4                         00000051
_reading4_1                       00000052
_reading5                         00000053
_reading5_1                       00000054
_reading6                         00000055
_reading6_1                       00000056
_reading7                         00000057
_reading7_1                       00000058
_sampleAll                        00000187
_sendPacket                       0000000A
_sensorSample                     000000A8
_sensorSample_phase0_1_1          000002C8
_sensorSample_phase1_1_1          000002C9
_sensorSample_phase2_1_1          000002CA
_sensorSample_phase3_1_1          000002CB
_serialBitDelay                   000000A2
bitfield0                         00000059
fbitfield06                       00000006
fbitfield09                       00000009
fbitfield0b                       0000000B
fbitfield1f                       0000001F
fbitfield89                       00000089
r0x53                             00000020
r0x54                             00000021
r0x55                             00000022
r0x56                             00000023
r0x57                             00000024
r0x58                             00000025
r0x59                             00000026
r0x5A                             00000027
r0x5C                             00000028
r0x5E                             00000029
r0x60                             0000002A
r0x62                             0000002B
r0x63                             0000002C
r0x65                             0000002D
r0x66                             0000002E
r0x67                             0000002F
r0x68                             00000030
r0x6A                             00000031
r0x6C                             00000032
r0x6D                             00000033
r0x6E                             00000034
r0x6F                             00000035
r0x70                             00000036
r0x71                             00000037
r0x72                             00000038
r0x73                             00000039
r0x74                             0000003A
gpasm-0.10.6 alpha              main.asm    2-25-2003  14:46:13          PAGE 41


LOC  OBJECT CODE     LINE SOURCE TEXT
  VALUE
 
r0x75                             0000003B
r0x76                             0000003C
r0x77                             0000003D
r0x78                             0000003E
r0x79                             0000003F
r0x7A                             00000040
r0x7B                             00000041
r0x7C                             00000042
r0x7D                             00000043
r0x7E                             00000044
r0x7F                             00000045
r0x80                             00000046
s0x1000                           00000048
s0xFFF                            00000047


MEMORY USAGE MAP ('X' = Used,  '-' = Unused)

0000 : XXXXXXXXXXXXXXXX XXXXXXXXXXXXXXXX XXXXXXXXXXXXXXXX XXXXXXXXXXXXXXXX
0040 : XXXXXXXXXXXXXXXX XXXXXXXXXXXXXXXX XXXXXXXXXXXXXXXX XXXXXXXXXXXXXXXX
0080 : XXXXXXXXXXXXXXXX XXXXXXXXXXXXXXXX XXXXXXXXXXXXXXXX XXXXXXXXXXXXXXXX
00c0 : XXXXXXXXXXXXXXXX XXXXXXXXXXXXXXXX XXXXXXXXXXXXXXXX XXXXXXXXXXXXXXXX
0100 : XXXXXXXXXXXXXXXX XXXXXXXXXXXXXXXX XXXXXXXXXXXXXXXX XXXXXXXXXXXXXXXX
0140 : XXXXXXXXXXXXXXXX XXXXXXXXXXXXXXXX XXXXXXXXXXXXXXXX XXXXXXXXXXXXXXXX
0180 : XXXXXXXXXXXXXXXX XXXXXXXXXXXXXXXX XXXXXXXXXXXXXXXX XXXXXXXXXXXXXXXX
01c0 : XXXXXXXXXXXXXXXX XXXXXX---------- ---------------- ----------------
2000 : -------X-------- ---------------- ---------------- ----------------

All other memory blocks unused.

Program Memory Words Used: 471


Errors   :       0
Warnings :       0 reported,       0 suppressed
Messages :      11 reported,       0 suppressed

