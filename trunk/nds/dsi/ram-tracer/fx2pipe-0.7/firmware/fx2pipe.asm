;--------------------------------------------------------
; File Created by SDCC : free open source ANSI-C Compiler
; Version 2.8.0 #5117 (May 29 2008) (UNIX)
; This file was generated Wed Apr 29 00:40:33 2009
;--------------------------------------------------------
	.module fx2pipe
	.optsdcc -mmcs51 --model-small
	
;--------------------------------------------------------
; Public variables in this module
;--------------------------------------------------------
	.globl _main
	.globl _EIP
	.globl _B
	.globl _EIE
	.globl _ACC
	.globl _EICON
	.globl _PSW
	.globl _TH2
	.globl _TL2
	.globl _RCAP2H
	.globl _RCAP2L
	.globl _T2CON
	.globl _SBUF1
	.globl _SCON1
	.globl _GPIFSGLDATLNOX
	.globl _GPIFSGLDATLX
	.globl _GPIFSGLDATH
	.globl _GPIFTRIG
	.globl _EP01STAT
	.globl _IP
	.globl _OEE
	.globl _OED
	.globl _OEC
	.globl _OEB
	.globl _OEA
	.globl _IOE
	.globl _IOD
	.globl _AUTOPTRSETUP
	.globl _EP68FIFOFLGS
	.globl _EP24FIFOFLGS
	.globl _EP2468STAT
	.globl _IE
	.globl _INT4CLR
	.globl _INT2CLR
	.globl _IOC
	.globl _AUTODAT2
	.globl _AUTOPTRL2
	.globl _AUTOPTRH2
	.globl _AUTODAT1
	.globl _APTR1L
	.globl _APTR1H
	.globl _SBU
	.globl _SCO
	.globl _MPA
	.globl _EXIF
	.globl _IOB
	.globl _SPC_
	.globl _CKCO
	.globl _TH1
	.globl _TH0
	.globl _TL1
	.globl _TL0
	.globl _TMOD
	.globl _TCON
	.globl _PCON
	.globl _DPS
	.globl _DPH1
	.globl _DPL1
	.globl _DPH
	.globl _DPL
	.globl _SP
	.globl _IOA
	.globl _EP8FIFOBUF
	.globl _EP6FIFOBUF
	.globl _EP4FIFOBUF
	.globl _EP2FIFOBUF
	.globl _EP1INBUF
	.globl _EP1OUTBUF
	.globl _EP0BUF
	.globl _CT4
	.globl _CT3
	.globl _CT2
	.globl _CT1
	.globl _USBTEST
	.globl _TESTCFG
	.globl _DBUG
	.globl _UDMACRCQUAL
	.globl _UDMACRCL
	.globl _UDMACRCH
	.globl _GPIFHOLDAMOUNT
	.globl _FLOWSTBHPERIOD
	.globl _FLOWSTBEDGE
	.globl _FLOWSTB
	.globl _FLOWHOLDOFF
	.globl _FLOWEQ1CTL
	.globl _FLOWEQ0CTL
	.globl _FLOWLOGIC
	.globl _FLOWSTATE
	.globl _GPIFABORT
	.globl _GPIFREADYSTAT
	.globl _GPIFREADYCFG
	.globl _XGPIFSGLDATLNOX
	.globl _XGPIFSGLDATLX
	.globl _XGPIFSGLDATH
	.globl _EP8GPIFTRIG
	.globl _EP8GPIFPFSTOP
	.globl _EP8GPIFFLGSEL
	.globl _EP6GPIFTRIG
	.globl _EP6GPIFPFSTOP
	.globl _EP6GPIFFLGSEL
	.globl _EP4GPIFTRIG
	.globl _EP4GPIFPFSTOP
	.globl _EP4GPIFFLGSEL
	.globl _EP2GPIFTRIG
	.globl _EP2GPIFPFSTOP
	.globl _EP2GPIFFLGSEL
	.globl _GPIFTCB0
	.globl _GPIFTCB1
	.globl _GPIFTCB2
	.globl _GPIFTCB3
	.globl _GPIFADRL
	.globl _GPIFADRH
	.globl _GPIFCTLCFG
	.globl _GPIFIDLECTL
	.globl _GPIFIDLECS
	.globl _GPIFWFSELECT
	.globl _SETUPDAT
	.globl _SUDPTRCTL
	.globl _SUDPTRL
	.globl _SUDPTRH
	.globl _EP8FIFOBCL
	.globl _EP8FIFOBCH
	.globl _EP6FIFOBCL
	.globl _EP6FIFOBCH
	.globl _EP4FIFOBCL
	.globl _EP4FIFOBCH
	.globl _EP2FIFOBCL
	.globl _EP2FIFOBCH
	.globl _EP8FIFOFLGS
	.globl _EP6FIFOFLGS
	.globl _EP4FIFOFLGS
	.globl _EP2FIFOFLGS
	.globl _EP8CS
	.globl _EP6CS
	.globl _EP4CS
	.globl _EP2CS
	.globl _EP1INCS
	.globl _EP1OUTCS
	.globl _EP0CS
	.globl _EP8BCL
	.globl _EP8BCH
	.globl _EP6BCL
	.globl _EP6BCH
	.globl _EP4BCL
	.globl _EP4BCH
	.globl _EP2BCL
	.globl _EP2BCH
	.globl _EP1INBC
	.globl _EP1OUTBC
	.globl _EP0BCL
	.globl _EP0BCH
	.globl _FNADDR
	.globl _MICROFRAME
	.globl _USBFRAMEL
	.globl _USBFRAMEH
	.globl _TOGCTL
	.globl _WAKEUPCS
	.globl _SUSPEND
	.globl _USBCS
	.globl _XAUTODAT2
	.globl _XAUTODAT1
	.globl _I2CTL
	.globl _I2DAT
	.globl _I2CS
	.globl _PORTECFG
	.globl _PORTCCFG
	.globl _PORTACFG
	.globl _INTSETUP
	.globl _INT4IVEC
	.globl _INT2IVEC
	.globl _CLRERRCNT
	.globl _ERRCNTLIM
	.globl _USBERRIRQ
	.globl _USBERRIE
	.globl _GPIFIRQ
	.globl _GPIFIE
	.globl _EPIRQ
	.globl _EPIE
	.globl _USBIRQ
	.globl _USBIE
	.globl _NAKIRQ
	.globl _NAKIE
	.globl _IBNIRQ
	.globl _IBNIE
	.globl _EP8FIFOIRQ
	.globl _EP8FIFOIE
	.globl _EP6FIFOIRQ
	.globl _EP6FIFOIE
	.globl _EP4FIFOIRQ
	.globl _EP4FIFOIE
	.globl _EP2FIFOIRQ
	.globl _EP2FIFOIE
	.globl _OUTPKTEND
	.globl _INPKTEND
	.globl _EP8ISOINPKTS
	.globl _EP6ISOINPKTS
	.globl _EP4ISOINPKTS
	.globl _EP2ISOINPKTS
	.globl _EP8FIFOPFL
	.globl _EP8FIFOPFH
	.globl _EP6FIFOPFL
	.globl _EP6FIFOPFH
	.globl _EP4FIFOPFL
	.globl _EP4FIFOPFH
	.globl _EP2FIFOPFL
	.globl _EP2FIFOPFH
	.globl _EP8AUTOINLENL
	.globl _EP8AUTOINLENH
	.globl _EP6AUTOINLENL
	.globl _EP6AUTOINLENH
	.globl _EP4AUTOINLENL
	.globl _EP4AUTOINLENH
	.globl _EP2AUTOINLENL
	.globl _EP2AUTOINLENH
	.globl _EP8FIFOCFG
	.globl _EP6FIFOCFG
	.globl _EP4FIFOCFG
	.globl _EP2FIFOCFG
	.globl _EP8CFG
	.globl _EP6CFG
	.globl _EP4CFG
	.globl _EP2CFG
	.globl _EP1INCFG
	.globl _EP1OUTCFG
	.globl _REVCTL
	.globl _REVID
	.globl _FIFOPINPOLAR
	.globl _UART230
	.globl _BPADDRL
	.globl _BPADDRH
	.globl _BREAKPT
	.globl _FIFORESET
	.globl _PINFLAGSCD
	.globl _PINFLAGSAB
	.globl _IFCONFIG
	.globl _CPUCS
	.globl _RES_WAVEDATA_END
	.globl _GPIF_WAVE_DATA
;--------------------------------------------------------
; special function registers
;--------------------------------------------------------
	.area RSEG    (DATA)
_IOA	=	0x0080
_SP	=	0x0081
_DPL	=	0x0082
_DPH	=	0x0083
_DPL1	=	0x0084
_DPH1	=	0x0085
_DPS	=	0x0086
_PCON	=	0x0087
_TCON	=	0x0088
_TMOD	=	0x0089
_TL0	=	0x008a
_TL1	=	0x008b
_TH0	=	0x008c
_TH1	=	0x008d
_CKCO	=	0x008e
_SPC_	=	0x008f
_IOB	=	0x0090
_EXIF	=	0x0091
_MPA	=	0x0092
_SCO	=	0x0098
_SBU	=	0x0099
_APTR1H	=	0x009a
_APTR1L	=	0x009b
_AUTODAT1	=	0x009c
_AUTOPTRH2	=	0x009d
_AUTOPTRL2	=	0x009e
_AUTODAT2	=	0x009f
_IOC	=	0x00a0
_INT2CLR	=	0x00a1
_INT4CLR	=	0x00a2
_IE	=	0x00a8
_EP2468STAT	=	0x00aa
_EP24FIFOFLGS	=	0x00ab
_EP68FIFOFLGS	=	0x00ac
_AUTOPTRSETUP	=	0x00af
_IOD	=	0x00b0
_IOE	=	0x00b1
_OEA	=	0x00b2
_OEB	=	0x00b3
_OEC	=	0x00b4
_OED	=	0x00b5
_OEE	=	0x00b6
_IP	=	0x00b8
_EP01STAT	=	0x00ba
_GPIFTRIG	=	0x00bb
_GPIFSGLDATH	=	0x00bd
_GPIFSGLDATLX	=	0x00be
_GPIFSGLDATLNOX	=	0x00bf
_SCON1	=	0x00c0
_SBUF1	=	0x00c1
_T2CON	=	0x00c8
_RCAP2L	=	0x00ca
_RCAP2H	=	0x00cb
_TL2	=	0x00cc
_TH2	=	0x00cd
_PSW	=	0x00d0
_EICON	=	0x00d8
_ACC	=	0x00e0
_EIE	=	0x00e8
_B	=	0x00f0
_EIP	=	0x00f8
;--------------------------------------------------------
; special function bits
;--------------------------------------------------------
	.area RSEG    (DATA)
;--------------------------------------------------------
; overlayable register banks
;--------------------------------------------------------
	.area REG_BANK_0	(REL,OVR,DATA)
	.ds 8
;--------------------------------------------------------
; internal ram data
;--------------------------------------------------------
	.area DSEG    (DATA)
;--------------------------------------------------------
; overlayable items in internal ram 
;--------------------------------------------------------
	.area	OSEG    (OVR,DATA)
;--------------------------------------------------------
; Stack segment in internal ram 
;--------------------------------------------------------
	.area	SSEG	(DATA)
__start__stack:
	.ds	1

;--------------------------------------------------------
; indirectly addressable internal ram data
;--------------------------------------------------------
	.area ISEG    (DATA)
;--------------------------------------------------------
; absolute internal ram data
;--------------------------------------------------------
	.area IABS    (ABS,DATA)
	.area IABS    (ABS,DATA)
;--------------------------------------------------------
; bit data
;--------------------------------------------------------
	.area BSEG    (BIT)
;--------------------------------------------------------
; paged external ram data
;--------------------------------------------------------
	.area PSEG    (PAG,XDATA)
;--------------------------------------------------------
; external ram data
;--------------------------------------------------------
	.area XSEG    (XDATA)
_GPIF_WAVE_DATA	=	0xe400
_RES_WAVEDATA_END	=	0xe480
_CPUCS	=	0xe600
_IFCONFIG	=	0xe601
_PINFLAGSAB	=	0xe602
_PINFLAGSCD	=	0xe603
_FIFORESET	=	0xe604
_BREAKPT	=	0xe605
_BPADDRH	=	0xe606
_BPADDRL	=	0xe607
_UART230	=	0xe608
_FIFOPINPOLAR	=	0xe609
_REVID	=	0xe60a
_REVCTL	=	0xe60b
_EP1OUTCFG	=	0xe610
_EP1INCFG	=	0xe611
_EP2CFG	=	0xe612
_EP4CFG	=	0xe613
_EP6CFG	=	0xe614
_EP8CFG	=	0xe615
_EP2FIFOCFG	=	0xe618
_EP4FIFOCFG	=	0xe619
_EP6FIFOCFG	=	0xe61a
_EP8FIFOCFG	=	0xe61b
_EP2AUTOINLENH	=	0xe620
_EP2AUTOINLENL	=	0xe621
_EP4AUTOINLENH	=	0xe622
_EP4AUTOINLENL	=	0xe623
_EP6AUTOINLENH	=	0xe624
_EP6AUTOINLENL	=	0xe625
_EP8AUTOINLENH	=	0xe626
_EP8AUTOINLENL	=	0xe627
_EP2FIFOPFH	=	0xe630
_EP2FIFOPFL	=	0xe631
_EP4FIFOPFH	=	0xe632
_EP4FIFOPFL	=	0xe633
_EP6FIFOPFH	=	0xe634
_EP6FIFOPFL	=	0xe635
_EP8FIFOPFH	=	0xe636
_EP8FIFOPFL	=	0xe637
_EP2ISOINPKTS	=	0xe640
_EP4ISOINPKTS	=	0xe641
_EP6ISOINPKTS	=	0xe642
_EP8ISOINPKTS	=	0xe643
_INPKTEND	=	0xe648
_OUTPKTEND	=	0xe649
_EP2FIFOIE	=	0xe650
_EP2FIFOIRQ	=	0xe651
_EP4FIFOIE	=	0xe652
_EP4FIFOIRQ	=	0xe653
_EP6FIFOIE	=	0xe654
_EP6FIFOIRQ	=	0xe655
_EP8FIFOIE	=	0xe656
_EP8FIFOIRQ	=	0xe657
_IBNIE	=	0xe658
_IBNIRQ	=	0xe659
_NAKIE	=	0xe65a
_NAKIRQ	=	0xe65b
_USBIE	=	0xe65c
_USBIRQ	=	0xe65d
_EPIE	=	0xe65e
_EPIRQ	=	0xe65f
_GPIFIE	=	0xe660
_GPIFIRQ	=	0xe661
_USBERRIE	=	0xe662
_USBERRIRQ	=	0xe663
_ERRCNTLIM	=	0xe664
_CLRERRCNT	=	0xe665
_INT2IVEC	=	0xe666
_INT4IVEC	=	0xe667
_INTSETUP	=	0xe668
_PORTACFG	=	0xe670
_PORTCCFG	=	0xe671
_PORTECFG	=	0xe672
_I2CS	=	0xe678
_I2DAT	=	0xe679
_I2CTL	=	0xe67a
_XAUTODAT1	=	0xe67b
_XAUTODAT2	=	0xe67c
_USBCS	=	0xe680
_SUSPEND	=	0xe681
_WAKEUPCS	=	0xe682
_TOGCTL	=	0xe683
_USBFRAMEH	=	0xe684
_USBFRAMEL	=	0xe685
_MICROFRAME	=	0xe686
_FNADDR	=	0xe687
_EP0BCH	=	0xe68a
_EP0BCL	=	0xe68b
_EP1OUTBC	=	0xe68d
_EP1INBC	=	0xe68f
_EP2BCH	=	0xe690
_EP2BCL	=	0xe691
_EP4BCH	=	0xe694
_EP4BCL	=	0xe695
_EP6BCH	=	0xe698
_EP6BCL	=	0xe699
_EP8BCH	=	0xe69c
_EP8BCL	=	0xe69d
_EP0CS	=	0xe6a0
_EP1OUTCS	=	0xe6a1
_EP1INCS	=	0xe6a2
_EP2CS	=	0xe6a3
_EP4CS	=	0xe6a4
_EP6CS	=	0xe6a5
_EP8CS	=	0xe6a6
_EP2FIFOFLGS	=	0xe6a7
_EP4FIFOFLGS	=	0xe6a8
_EP6FIFOFLGS	=	0xe6a9
_EP8FIFOFLGS	=	0xe6aa
_EP2FIFOBCH	=	0xe6ab
_EP2FIFOBCL	=	0xe6ac
_EP4FIFOBCH	=	0xe6ad
_EP4FIFOBCL	=	0xe6ae
_EP6FIFOBCH	=	0xe6af
_EP6FIFOBCL	=	0xe6b0
_EP8FIFOBCH	=	0xe6b1
_EP8FIFOBCL	=	0xe6b2
_SUDPTRH	=	0xe6b3
_SUDPTRL	=	0xe6b4
_SUDPTRCTL	=	0xe6b5
_SETUPDAT	=	0xe6b8
_GPIFWFSELECT	=	0xe6c0
_GPIFIDLECS	=	0xe6c1
_GPIFIDLECTL	=	0xe6c2
_GPIFCTLCFG	=	0xe6c3
_GPIFADRH	=	0xe6c4
_GPIFADRL	=	0xe6c5
_GPIFTCB3	=	0xe6ce
_GPIFTCB2	=	0xe6cf
_GPIFTCB1	=	0xe6d0
_GPIFTCB0	=	0xe6d1
_EP2GPIFFLGSEL	=	0xe6d2
_EP2GPIFPFSTOP	=	0xe6d3
_EP2GPIFTRIG	=	0xe6d4
_EP4GPIFFLGSEL	=	0xe6da
_EP4GPIFPFSTOP	=	0xe6db
_EP4GPIFTRIG	=	0xe6dc
_EP6GPIFFLGSEL	=	0xe6e2
_EP6GPIFPFSTOP	=	0xe6e3
_EP6GPIFTRIG	=	0xe6e4
_EP8GPIFFLGSEL	=	0xe6ea
_EP8GPIFPFSTOP	=	0xe6eb
_EP8GPIFTRIG	=	0xe6ec
_XGPIFSGLDATH	=	0xe6f0
_XGPIFSGLDATLX	=	0xe6f1
_XGPIFSGLDATLNOX	=	0xe6f2
_GPIFREADYCFG	=	0xe6f3
_GPIFREADYSTAT	=	0xe6f4
_GPIFABORT	=	0xe6f5
_FLOWSTATE	=	0xe6c6
_FLOWLOGIC	=	0xe6c7
_FLOWEQ0CTL	=	0xe6c8
_FLOWEQ1CTL	=	0xe6c9
_FLOWHOLDOFF	=	0xe6ca
_FLOWSTB	=	0xe6cb
_FLOWSTBEDGE	=	0xe6cc
_FLOWSTBHPERIOD	=	0xe6cd
_GPIFHOLDAMOUNT	=	0xe60c
_UDMACRCH	=	0xe67d
_UDMACRCL	=	0xe67e
_UDMACRCQUAL	=	0xe67f
_DBUG	=	0xe6f8
_TESTCFG	=	0xe6f9
_USBTEST	=	0xe6fa
_CT1	=	0xe6fb
_CT2	=	0xe6fc
_CT3	=	0xe6fd
_CT4	=	0xe6fe
_EP0BUF	=	0xe740
_EP1OUTBUF	=	0xe780
_EP1INBUF	=	0xe7c0
_EP2FIFOBUF	=	0xf000
_EP4FIFOBUF	=	0xf400
_EP6FIFOBUF	=	0xf800
_EP8FIFOBUF	=	0xfc00
;--------------------------------------------------------
; absolute external ram data
;--------------------------------------------------------
	.area XABS    (ABS,XDATA)
;--------------------------------------------------------
; external initialized ram data
;--------------------------------------------------------
	.area XISEG   (XDATA)
	.area HOME    (CODE)
	.area GSINIT0 (CODE)
	.area GSINIT1 (CODE)
	.area GSINIT2 (CODE)
	.area GSINIT3 (CODE)
	.area GSINIT4 (CODE)
	.area GSINIT5 (CODE)
	.area GSINIT  (CODE)
	.area GSFINAL (CODE)
	.area CSEG    (CODE)
;--------------------------------------------------------
; interrupt vector 
;--------------------------------------------------------
	.area HOME    (CODE)
__interrupt_vect:
	ljmp	__sdcc_gsinit_startup
;--------------------------------------------------------
; global & static initialisations
;--------------------------------------------------------
	.area HOME    (CODE)
	.area GSINIT  (CODE)
	.area GSFINAL (CODE)
	.area GSINIT  (CODE)
	.globl __sdcc_gsinit_startup
	.globl __sdcc_program_startup
	.globl __start__stack
	.globl __mcs51_genXINIT
	.globl __mcs51_genXRAMCLEAR
	.globl __mcs51_genRAMCLEAR
	.area GSFINAL (CODE)
	ljmp	__sdcc_program_startup
;--------------------------------------------------------
; Home
;--------------------------------------------------------
	.area HOME    (CODE)
	.area HOME    (CODE)
__sdcc_program_startup:
	lcall	_main
;	return from main will lock up
	sjmp .
;--------------------------------------------------------
; code
;--------------------------------------------------------
	.area CSEG    (CODE)
;------------------------------------------------------------
;Allocation info for local variables in function 'Initialize'
;------------------------------------------------------------
;cfg_data                  Allocated to registers 
;cfg_data_ok               Allocated to registers r5 
;------------------------------------------------------------
;	fx2pipe.c:56: static void Initialize(void)
;	-----------------------------------------
;	 function Initialize
;	-----------------------------------------
_Initialize:
	ar2 = 0x02
	ar3 = 0x03
	ar4 = 0x04
	ar5 = 0x05
	ar6 = 0x06
	ar7 = 0x07
	ar0 = 0x00
	ar1 = 0x01
;	fx2pipe.c:65: char cfg_data_ok = (cfg_data[0]==0x12U || cfg_data[0]==0x21U);
	mov	dptr,#0x1003
	movx	a,@dptr
	mov	r2,a
	rlc	a
	subb	a,acc
	mov	r3,a
	clr	a
	cjne	r2,#0x12,00126$
	cjne	r3,#0x00,00126$
	inc	a
00126$:
	mov	r4,a
	jnz	00112$
	cjne	r2,#0x21,00129$
	cjne	r3,#0x00,00129$
	sjmp	00112$
00129$:
	mov	r5,#0x00
	sjmp	00113$
00112$:
	mov	r5,#0x01
00113$:
;	fx2pipe.c:67: SYNCDELAY;
	 nop 
	 nop 
	 nop 
	 nop 
	 nop 
	 nop 
	 nop 
	 nop 
	 nop 
	 nop 
	 nop 
	 nop 
	 nop 
	 nop 
	 nop 
	 nop 
	 nop 
;	fx2pipe.c:77: CPUCS = cfg_data_ok ? cfg_data[4] : 0x12;  // 0x12
	mov	a,r5
	jz	00114$
	mov	dptr,#0x1007
	movx	a,@dptr
	mov	r6,a
	sjmp	00115$
00114$:
	mov	r6,#0x12
00115$:
	mov	dptr,#_CPUCS
	mov	a,r6
	movx	@dptr,a
;	fx2pipe.c:78: SYNCDELAY;
	 nop 
	 nop 
	 nop 
	 nop 
	 nop 
	 nop 
	 nop 
	 nop 
	 nop 
	 nop 
	 nop 
	 nop 
	 nop 
	 nop 
	 nop 
	 nop 
	 nop 
;	fx2pipe.c:93: IFCONFIG = cfg_data_ok ? cfg_data[1] : 0xc3;
	mov	a,r5
	jz	00116$
	mov	dptr,#0x1004
	movx	a,@dptr
	mov	r5,a
	sjmp	00117$
00116$:
	mov	r5,#0xC3
00117$:
	mov	dptr,#_IFCONFIG
	mov	a,r5
	movx	@dptr,a
;	fx2pipe.c:94: SYNCDELAY;
	 nop 
	 nop 
	 nop 
	 nop 
	 nop 
	 nop 
	 nop 
	 nop 
	 nop 
	 nop 
	 nop 
	 nop 
	 nop 
	 nop 
	 nop 
	 nop 
	 nop 
;	fx2pipe.c:97: REVCTL = 0x03;  // See TRM...
	mov	dptr,#_REVCTL
	mov	a,#0x03
	movx	@dptr,a
;	fx2pipe.c:98: SYNCDELAY;
	 nop 
	 nop 
	 nop 
	 nop 
	 nop 
	 nop 
	 nop 
	 nop 
	 nop 
	 nop 
	 nop 
	 nop 
	 nop 
	 nop 
	 nop 
	 nop 
	 nop 
;	fx2pipe.c:101: PORTACFG = 0x00;
	mov	dptr,#_PORTACFG
	clr	a
	movx	@dptr,a
;	fx2pipe.c:102: SYNCDELAY; // maybe not needed
	 nop 
	 nop 
	 nop 
	 nop 
	 nop 
	 nop 
	 nop 
	 nop 
	 nop 
	 nop 
	 nop 
	 nop 
	 nop 
	 nop 
	 nop 
	 nop 
	 nop 
;	fx2pipe.c:105: FIFOPINPOLAR=0x00;
	mov	dptr,#_FIFOPINPOLAR
	clr	a
	movx	@dptr,a
;	fx2pipe.c:106: SYNCDELAY;
	 nop 
	 nop 
	 nop 
	 nop 
	 nop 
	 nop 
	 nop 
	 nop 
	 nop 
	 nop 
	 nop 
	 nop 
	 nop 
	 nop 
	 nop 
	 nop 
	 nop 
;	fx2pipe.c:109: EP6CFG=0x00U;  SYNCDELAY;
	mov	dptr,#_EP6CFG
	clr	a
	movx	@dptr,a
	 nop 
	 nop 
	 nop 
	 nop 
	 nop 
	 nop 
	 nop 
	 nop 
	 nop 
	 nop 
	 nop 
	 nop 
	 nop 
	 nop 
	 nop 
	 nop 
	 nop 
;	fx2pipe.c:110: EP2CFG=0x00U;  SYNCDELAY;
	mov	dptr,#_EP2CFG
	clr	a
	movx	@dptr,a
	 nop 
	 nop 
	 nop 
	 nop 
	 nop 
	 nop 
	 nop 
	 nop 
	 nop 
	 nop 
	 nop 
	 nop 
	 nop 
	 nop 
	 nop 
	 nop 
	 nop 
;	fx2pipe.c:111: EP6FIFOCFG=0x00U;  SYNCDELAY;
	mov	dptr,#_EP6FIFOCFG
	clr	a
	movx	@dptr,a
	 nop 
	 nop 
	 nop 
	 nop 
	 nop 
	 nop 
	 nop 
	 nop 
	 nop 
	 nop 
	 nop 
	 nop 
	 nop 
	 nop 
	 nop 
	 nop 
	 nop 
;	fx2pipe.c:112: EP2FIFOCFG=0x00U;  SYNCDELAY;
	mov	dptr,#_EP2FIFOCFG
	clr	a
	movx	@dptr,a
	 nop 
	 nop 
	 nop 
	 nop 
	 nop 
	 nop 
	 nop 
	 nop 
	 nop 
	 nop 
	 nop 
	 nop 
	 nop 
	 nop 
	 nop 
	 nop 
	 nop 
;	fx2pipe.c:113: OEA=0x00U;
	mov	_OEA,#0x00
;	fx2pipe.c:115: if(cfg_data[0]==0x12U) /* INPUT: USB->HOST */
	mov	a,r4
	jnz	00132$
	ljmp	00107$
00132$:
;	fx2pipe.c:126: EP6CFG = cfg_data[2];  // bulk: 0xe2 double-buffered; 0xe3 triple-; 0xe0 quad
	mov	dptr,#0x1005
	movx	a,@dptr
	mov	r4,a
	mov	dptr,#_EP6CFG
	movx	@dptr,a
;	fx2pipe.c:127: SYNCDELAY;
	 nop 
	 nop 
	 nop 
	 nop 
	 nop 
	 nop 
	 nop 
	 nop 
	 nop 
	 nop 
	 nop 
	 nop 
	 nop 
	 nop 
	 nop 
	 nop 
	 nop 
;	fx2pipe.c:131: FIFORESET = 0x80;  SYNCDELAY;  // NAK all requests from host. 
	mov	dptr,#_FIFORESET
	mov	a,#0x80
	movx	@dptr,a
	 nop 
	 nop 
	 nop 
	 nop 
	 nop 
	 nop 
	 nop 
	 nop 
	 nop 
	 nop 
	 nop 
	 nop 
	 nop 
	 nop 
	 nop 
	 nop 
	 nop 
;	fx2pipe.c:132: FIFORESET = 0x82;  SYNCDELAY;  // Reset individual EP (2,4,6,8)
	mov	dptr,#_FIFORESET
	mov	a,#0x82
	movx	@dptr,a
	 nop 
	 nop 
	 nop 
	 nop 
	 nop 
	 nop 
	 nop 
	 nop 
	 nop 
	 nop 
	 nop 
	 nop 
	 nop 
	 nop 
	 nop 
	 nop 
	 nop 
;	fx2pipe.c:133: FIFORESET = 0x84;  SYNCDELAY;
	mov	dptr,#_FIFORESET
	mov	a,#0x84
	movx	@dptr,a
	 nop 
	 nop 
	 nop 
	 nop 
	 nop 
	 nop 
	 nop 
	 nop 
	 nop 
	 nop 
	 nop 
	 nop 
	 nop 
	 nop 
	 nop 
	 nop 
	 nop 
;	fx2pipe.c:134: FIFORESET = 0x86;  SYNCDELAY;
	mov	dptr,#_FIFORESET
	mov	a,#0x86
	movx	@dptr,a
	 nop 
	 nop 
	 nop 
	 nop 
	 nop 
	 nop 
	 nop 
	 nop 
	 nop 
	 nop 
	 nop 
	 nop 
	 nop 
	 nop 
	 nop 
	 nop 
	 nop 
;	fx2pipe.c:135: FIFORESET = 0x88;  SYNCDELAY;
	mov	dptr,#_FIFORESET
	mov	a,#0x88
	movx	@dptr,a
	 nop 
	 nop 
	 nop 
	 nop 
	 nop 
	 nop 
	 nop 
	 nop 
	 nop 
	 nop 
	 nop 
	 nop 
	 nop 
	 nop 
	 nop 
	 nop 
	 nop 
;	fx2pipe.c:136: FIFORESET = 0x00;  SYNCDELAY;  // Resume normal operation. 
	mov	dptr,#_FIFORESET
	clr	a
	movx	@dptr,a
	 nop 
	 nop 
	 nop 
	 nop 
	 nop 
	 nop 
	 nop 
	 nop 
	 nop 
	 nop 
	 nop 
	 nop 
	 nop 
	 nop 
	 nop 
	 nop 
	 nop 
;	fx2pipe.c:148: EP6FIFOCFG = cfg_data[3]; /*0x0d //&0xfe*/;
	mov	dptr,#0x1006
	movx	a,@dptr
	mov	r4,a
	mov	dptr,#_EP6FIFOCFG
	movx	@dptr,a
;	fx2pipe.c:149: SYNCDELAY;
	 nop 
	 nop 
	 nop 
	 nop 
	 nop 
	 nop 
	 nop 
	 nop 
	 nop 
	 nop 
	 nop 
	 nop 
	 nop 
	 nop 
	 nop 
	 nop 
	 nop 
;	fx2pipe.c:154: EP6AUTOINLENH = 0x02; // MSB
	mov	dptr,#_EP6AUTOINLENH
	mov	a,#0x02
	movx	@dptr,a
;	fx2pipe.c:155: SYNCDELAY;
	 nop 
	 nop 
	 nop 
	 nop 
	 nop 
	 nop 
	 nop 
	 nop 
	 nop 
	 nop 
	 nop 
	 nop 
	 nop 
	 nop 
	 nop 
	 nop 
	 nop 
;	fx2pipe.c:156: EP6AUTOINLENL = 0x00; // LSB
	mov	dptr,#_EP6AUTOINLENL
	clr	a
	movx	@dptr,a
;	fx2pipe.c:157: SYNCDELAY;
	 nop 
	 nop 
	 nop 
	 nop 
	 nop 
	 nop 
	 nop 
	 nop 
	 nop 
	 nop 
	 nop 
	 nop 
	 nop 
	 nop 
	 nop 
	 nop 
	 nop 
	ret
00107$:
;	fx2pipe.c:165: else if(cfg_data[0]==0x21U) /* OUTPUT: HOST->USB */	
	cjne	r2,#0x21,00133$
	cjne	r3,#0x00,00133$
	sjmp	00134$
00133$:
	ret
00134$:
;	fx2pipe.c:176: EP2CFG = cfg_data[2];  // bulk: 0xa2 double-buffered; 0xa3 triple-; 0xa0 quad
	mov	dptr,#0x1005
	movx	a,@dptr
	mov	r2,a
	mov	dptr,#_EP2CFG
	movx	@dptr,a
;	fx2pipe.c:177: SYNCDELAY;
	 nop 
	 nop 
	 nop 
	 nop 
	 nop 
	 nop 
	 nop 
	 nop 
	 nop 
	 nop 
	 nop 
	 nop 
	 nop 
	 nop 
	 nop 
	 nop 
	 nop 
;	fx2pipe.c:181: FIFORESET = 0x80;  SYNCDELAY;  // NAK all requests from host. 
	mov	dptr,#_FIFORESET
	mov	a,#0x80
	movx	@dptr,a
	 nop 
	 nop 
	 nop 
	 nop 
	 nop 
	 nop 
	 nop 
	 nop 
	 nop 
	 nop 
	 nop 
	 nop 
	 nop 
	 nop 
	 nop 
	 nop 
	 nop 
;	fx2pipe.c:182: FIFORESET = 0x82;  SYNCDELAY;  // Reset individual EP (2,4,6,8)
	mov	dptr,#_FIFORESET
	mov	a,#0x82
	movx	@dptr,a
	 nop 
	 nop 
	 nop 
	 nop 
	 nop 
	 nop 
	 nop 
	 nop 
	 nop 
	 nop 
	 nop 
	 nop 
	 nop 
	 nop 
	 nop 
	 nop 
	 nop 
;	fx2pipe.c:183: FIFORESET = 0x84;  SYNCDELAY;
	mov	dptr,#_FIFORESET
	mov	a,#0x84
	movx	@dptr,a
	 nop 
	 nop 
	 nop 
	 nop 
	 nop 
	 nop 
	 nop 
	 nop 
	 nop 
	 nop 
	 nop 
	 nop 
	 nop 
	 nop 
	 nop 
	 nop 
	 nop 
;	fx2pipe.c:184: FIFORESET = 0x86;  SYNCDELAY;
	mov	dptr,#_FIFORESET
	mov	a,#0x86
	movx	@dptr,a
	 nop 
	 nop 
	 nop 
	 nop 
	 nop 
	 nop 
	 nop 
	 nop 
	 nop 
	 nop 
	 nop 
	 nop 
	 nop 
	 nop 
	 nop 
	 nop 
	 nop 
;	fx2pipe.c:185: FIFORESET = 0x88;  SYNCDELAY;
	mov	dptr,#_FIFORESET
	mov	a,#0x88
	movx	@dptr,a
	 nop 
	 nop 
	 nop 
	 nop 
	 nop 
	 nop 
	 nop 
	 nop 
	 nop 
	 nop 
	 nop 
	 nop 
	 nop 
	 nop 
	 nop 
	 nop 
	 nop 
;	fx2pipe.c:186: FIFORESET = 0x00;  SYNCDELAY;  // Resume normal operation. 
	mov	dptr,#_FIFORESET
	clr	a
	movx	@dptr,a
	 nop 
	 nop 
	 nop 
	 nop 
	 nop 
	 nop 
	 nop 
	 nop 
	 nop 
	 nop 
	 nop 
	 nop 
	 nop 
	 nop 
	 nop 
	 nop 
	 nop 
;	fx2pipe.c:189: OUTPKTEND = 0x82;  SYNCDELAY;
	mov	dptr,#_OUTPKTEND
	mov	a,#0x82
	movx	@dptr,a
	 nop 
	 nop 
	 nop 
	 nop 
	 nop 
	 nop 
	 nop 
	 nop 
	 nop 
	 nop 
	 nop 
	 nop 
	 nop 
	 nop 
	 nop 
	 nop 
	 nop 
;	fx2pipe.c:190: OUTPKTEND = 0x82;  SYNCDELAY;
	mov	dptr,#_OUTPKTEND
	mov	a,#0x82
	movx	@dptr,a
	 nop 
	 nop 
	 nop 
	 nop 
	 nop 
	 nop 
	 nop 
	 nop 
	 nop 
	 nop 
	 nop 
	 nop 
	 nop 
	 nop 
	 nop 
	 nop 
	 nop 
;	fx2pipe.c:191: switch(cfg_data[2]&0x03U)
	mov	a,r2
	rlc	a
	subb	a,acc
	anl	ar2,#0x03
	mov	r3,#0x00
	cjne	r2,#0x00,00135$
	cjne	r3,#0x00,00135$
	sjmp	00101$
00135$:
;	fx2pipe.c:193: case 0x00U:  OUTPKTEND = 0x82;  SYNCDELAY;  // Quad-buffered.
	cjne	r2,#0x03,00103$
	cjne	r3,#0x00,00103$
	sjmp	00102$
00101$:
	mov	dptr,#_OUTPKTEND
	mov	a,#0x82
	movx	@dptr,a
	 nop 
	 nop 
	 nop 
	 nop 
	 nop 
	 nop 
	 nop 
	 nop 
	 nop 
	 nop 
	 nop 
	 nop 
	 nop 
	 nop 
	 nop 
	 nop 
	 nop 
;	fx2pipe.c:194: case 0x03U:  OUTPKTEND = 0x82;  SYNCDELAY;  // Triple-buffered.
00102$:
	mov	dptr,#_OUTPKTEND
	mov	a,#0x82
	movx	@dptr,a
	 nop 
	 nop 
	 nop 
	 nop 
	 nop 
	 nop 
	 nop 
	 nop 
	 nop 
	 nop 
	 nop 
	 nop 
	 nop 
	 nop 
	 nop 
	 nop 
	 nop 
;	fx2pipe.c:195: }
00103$:
;	fx2pipe.c:207: EP2FIFOCFG = cfg_data[3]; /*0x11;*/
	mov	dptr,#0x1006
	movx	a,@dptr
	mov	r2,a
	mov	dptr,#_EP2FIFOCFG
	movx	@dptr,a
;	fx2pipe.c:208: SYNCDELAY;
	 nop 
	 nop 
	 nop 
	 nop 
	 nop 
	 nop 
	 nop 
	 nop 
	 nop 
	 nop 
	 nop 
	 nop 
	 nop 
	 nop 
	 nop 
	 nop 
	 nop 
	ret
;------------------------------------------------------------
;Allocation info for local variables in function 'main'
;------------------------------------------------------------
;------------------------------------------------------------
;	fx2pipe.c:219: void main()
;	-----------------------------------------
;	 function main
;	-----------------------------------------
_main:
;	fx2pipe.c:221: Initialize();
	lcall	_Initialize
00102$:
	sjmp	00102$
	.area CSEG    (CODE)
	.area CONST   (CODE)
	.area XINIT   (CODE)
	.area CABS    (ABS,CODE)
