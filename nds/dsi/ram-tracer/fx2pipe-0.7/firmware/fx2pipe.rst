                              1 ;--------------------------------------------------------
                              2 ; File Created by SDCC : free open source ANSI-C Compiler
                              3 ; Version 2.8.0 #5117 (May 29 2008) (UNIX)
                              4 ; This file was generated Wed Apr 29 00:40:33 2009
                              5 ;--------------------------------------------------------
                              6 	.module fx2pipe
                              7 	.optsdcc -mmcs51 --model-small
                              8 	
                              9 ;--------------------------------------------------------
                             10 ; Public variables in this module
                             11 ;--------------------------------------------------------
                             12 	.globl _main
                             13 	.globl _EIP
                             14 	.globl _B
                             15 	.globl _EIE
                             16 	.globl _ACC
                             17 	.globl _EICON
                             18 	.globl _PSW
                             19 	.globl _TH2
                             20 	.globl _TL2
                             21 	.globl _RCAP2H
                             22 	.globl _RCAP2L
                             23 	.globl _T2CON
                             24 	.globl _SBUF1
                             25 	.globl _SCON1
                             26 	.globl _GPIFSGLDATLNOX
                             27 	.globl _GPIFSGLDATLX
                             28 	.globl _GPIFSGLDATH
                             29 	.globl _GPIFTRIG
                             30 	.globl _EP01STAT
                             31 	.globl _IP
                             32 	.globl _OEE
                             33 	.globl _OED
                             34 	.globl _OEC
                             35 	.globl _OEB
                             36 	.globl _OEA
                             37 	.globl _IOE
                             38 	.globl _IOD
                             39 	.globl _AUTOPTRSETUP
                             40 	.globl _EP68FIFOFLGS
                             41 	.globl _EP24FIFOFLGS
                             42 	.globl _EP2468STAT
                             43 	.globl _IE
                             44 	.globl _INT4CLR
                             45 	.globl _INT2CLR
                             46 	.globl _IOC
                             47 	.globl _AUTODAT2
                             48 	.globl _AUTOPTRL2
                             49 	.globl _AUTOPTRH2
                             50 	.globl _AUTODAT1
                             51 	.globl _APTR1L
                             52 	.globl _APTR1H
                             53 	.globl _SBU
                             54 	.globl _SCO
                             55 	.globl _MPA
                             56 	.globl _EXIF
                             57 	.globl _IOB
                             58 	.globl _SPC_
                             59 	.globl _CKCO
                             60 	.globl _TH1
                             61 	.globl _TH0
                             62 	.globl _TL1
                             63 	.globl _TL0
                             64 	.globl _TMOD
                             65 	.globl _TCON
                             66 	.globl _PCON
                             67 	.globl _DPS
                             68 	.globl _DPH1
                             69 	.globl _DPL1
                             70 	.globl _DPH
                             71 	.globl _DPL
                             72 	.globl _SP
                             73 	.globl _IOA
                             74 	.globl _EP8FIFOBUF
                             75 	.globl _EP6FIFOBUF
                             76 	.globl _EP4FIFOBUF
                             77 	.globl _EP2FIFOBUF
                             78 	.globl _EP1INBUF
                             79 	.globl _EP1OUTBUF
                             80 	.globl _EP0BUF
                             81 	.globl _CT4
                             82 	.globl _CT3
                             83 	.globl _CT2
                             84 	.globl _CT1
                             85 	.globl _USBTEST
                             86 	.globl _TESTCFG
                             87 	.globl _DBUG
                             88 	.globl _UDMACRCQUAL
                             89 	.globl _UDMACRCL
                             90 	.globl _UDMACRCH
                             91 	.globl _GPIFHOLDAMOUNT
                             92 	.globl _FLOWSTBHPERIOD
                             93 	.globl _FLOWSTBEDGE
                             94 	.globl _FLOWSTB
                             95 	.globl _FLOWHOLDOFF
                             96 	.globl _FLOWEQ1CTL
                             97 	.globl _FLOWEQ0CTL
                             98 	.globl _FLOWLOGIC
                             99 	.globl _FLOWSTATE
                            100 	.globl _GPIFABORT
                            101 	.globl _GPIFREADYSTAT
                            102 	.globl _GPIFREADYCFG
                            103 	.globl _XGPIFSGLDATLNOX
                            104 	.globl _XGPIFSGLDATLX
                            105 	.globl _XGPIFSGLDATH
                            106 	.globl _EP8GPIFTRIG
                            107 	.globl _EP8GPIFPFSTOP
                            108 	.globl _EP8GPIFFLGSEL
                            109 	.globl _EP6GPIFTRIG
                            110 	.globl _EP6GPIFPFSTOP
                            111 	.globl _EP6GPIFFLGSEL
                            112 	.globl _EP4GPIFTRIG
                            113 	.globl _EP4GPIFPFSTOP
                            114 	.globl _EP4GPIFFLGSEL
                            115 	.globl _EP2GPIFTRIG
                            116 	.globl _EP2GPIFPFSTOP
                            117 	.globl _EP2GPIFFLGSEL
                            118 	.globl _GPIFTCB0
                            119 	.globl _GPIFTCB1
                            120 	.globl _GPIFTCB2
                            121 	.globl _GPIFTCB3
                            122 	.globl _GPIFADRL
                            123 	.globl _GPIFADRH
                            124 	.globl _GPIFCTLCFG
                            125 	.globl _GPIFIDLECTL
                            126 	.globl _GPIFIDLECS
                            127 	.globl _GPIFWFSELECT
                            128 	.globl _SETUPDAT
                            129 	.globl _SUDPTRCTL
                            130 	.globl _SUDPTRL
                            131 	.globl _SUDPTRH
                            132 	.globl _EP8FIFOBCL
                            133 	.globl _EP8FIFOBCH
                            134 	.globl _EP6FIFOBCL
                            135 	.globl _EP6FIFOBCH
                            136 	.globl _EP4FIFOBCL
                            137 	.globl _EP4FIFOBCH
                            138 	.globl _EP2FIFOBCL
                            139 	.globl _EP2FIFOBCH
                            140 	.globl _EP8FIFOFLGS
                            141 	.globl _EP6FIFOFLGS
                            142 	.globl _EP4FIFOFLGS
                            143 	.globl _EP2FIFOFLGS
                            144 	.globl _EP8CS
                            145 	.globl _EP6CS
                            146 	.globl _EP4CS
                            147 	.globl _EP2CS
                            148 	.globl _EP1INCS
                            149 	.globl _EP1OUTCS
                            150 	.globl _EP0CS
                            151 	.globl _EP8BCL
                            152 	.globl _EP8BCH
                            153 	.globl _EP6BCL
                            154 	.globl _EP6BCH
                            155 	.globl _EP4BCL
                            156 	.globl _EP4BCH
                            157 	.globl _EP2BCL
                            158 	.globl _EP2BCH
                            159 	.globl _EP1INBC
                            160 	.globl _EP1OUTBC
                            161 	.globl _EP0BCL
                            162 	.globl _EP0BCH
                            163 	.globl _FNADDR
                            164 	.globl _MICROFRAME
                            165 	.globl _USBFRAMEL
                            166 	.globl _USBFRAMEH
                            167 	.globl _TOGCTL
                            168 	.globl _WAKEUPCS
                            169 	.globl _SUSPEND
                            170 	.globl _USBCS
                            171 	.globl _XAUTODAT2
                            172 	.globl _XAUTODAT1
                            173 	.globl _I2CTL
                            174 	.globl _I2DAT
                            175 	.globl _I2CS
                            176 	.globl _PORTECFG
                            177 	.globl _PORTCCFG
                            178 	.globl _PORTACFG
                            179 	.globl _INTSETUP
                            180 	.globl _INT4IVEC
                            181 	.globl _INT2IVEC
                            182 	.globl _CLRERRCNT
                            183 	.globl _ERRCNTLIM
                            184 	.globl _USBERRIRQ
                            185 	.globl _USBERRIE
                            186 	.globl _GPIFIRQ
                            187 	.globl _GPIFIE
                            188 	.globl _EPIRQ
                            189 	.globl _EPIE
                            190 	.globl _USBIRQ
                            191 	.globl _USBIE
                            192 	.globl _NAKIRQ
                            193 	.globl _NAKIE
                            194 	.globl _IBNIRQ
                            195 	.globl _IBNIE
                            196 	.globl _EP8FIFOIRQ
                            197 	.globl _EP8FIFOIE
                            198 	.globl _EP6FIFOIRQ
                            199 	.globl _EP6FIFOIE
                            200 	.globl _EP4FIFOIRQ
                            201 	.globl _EP4FIFOIE
                            202 	.globl _EP2FIFOIRQ
                            203 	.globl _EP2FIFOIE
                            204 	.globl _OUTPKTEND
                            205 	.globl _INPKTEND
                            206 	.globl _EP8ISOINPKTS
                            207 	.globl _EP6ISOINPKTS
                            208 	.globl _EP4ISOINPKTS
                            209 	.globl _EP2ISOINPKTS
                            210 	.globl _EP8FIFOPFL
                            211 	.globl _EP8FIFOPFH
                            212 	.globl _EP6FIFOPFL
                            213 	.globl _EP6FIFOPFH
                            214 	.globl _EP4FIFOPFL
                            215 	.globl _EP4FIFOPFH
                            216 	.globl _EP2FIFOPFL
                            217 	.globl _EP2FIFOPFH
                            218 	.globl _EP8AUTOINLENL
                            219 	.globl _EP8AUTOINLENH
                            220 	.globl _EP6AUTOINLENL
                            221 	.globl _EP6AUTOINLENH
                            222 	.globl _EP4AUTOINLENL
                            223 	.globl _EP4AUTOINLENH
                            224 	.globl _EP2AUTOINLENL
                            225 	.globl _EP2AUTOINLENH
                            226 	.globl _EP8FIFOCFG
                            227 	.globl _EP6FIFOCFG
                            228 	.globl _EP4FIFOCFG
                            229 	.globl _EP2FIFOCFG
                            230 	.globl _EP8CFG
                            231 	.globl _EP6CFG
                            232 	.globl _EP4CFG
                            233 	.globl _EP2CFG
                            234 	.globl _EP1INCFG
                            235 	.globl _EP1OUTCFG
                            236 	.globl _REVCTL
                            237 	.globl _REVID
                            238 	.globl _FIFOPINPOLAR
                            239 	.globl _UART230
                            240 	.globl _BPADDRL
                            241 	.globl _BPADDRH
                            242 	.globl _BREAKPT
                            243 	.globl _FIFORESET
                            244 	.globl _PINFLAGSCD
                            245 	.globl _PINFLAGSAB
                            246 	.globl _IFCONFIG
                            247 	.globl _CPUCS
                            248 	.globl _RES_WAVEDATA_END
                            249 	.globl _GPIF_WAVE_DATA
                            250 ;--------------------------------------------------------
                            251 ; special function registers
                            252 ;--------------------------------------------------------
                            253 	.area RSEG    (DATA)
                    0080    254 _IOA	=	0x0080
                    0081    255 _SP	=	0x0081
                    0082    256 _DPL	=	0x0082
                    0083    257 _DPH	=	0x0083
                    0084    258 _DPL1	=	0x0084
                    0085    259 _DPH1	=	0x0085
                    0086    260 _DPS	=	0x0086
                    0087    261 _PCON	=	0x0087
                    0088    262 _TCON	=	0x0088
                    0089    263 _TMOD	=	0x0089
                    008A    264 _TL0	=	0x008a
                    008B    265 _TL1	=	0x008b
                    008C    266 _TH0	=	0x008c
                    008D    267 _TH1	=	0x008d
                    008E    268 _CKCO	=	0x008e
                    008F    269 _SPC_	=	0x008f
                    0090    270 _IOB	=	0x0090
                    0091    271 _EXIF	=	0x0091
                    0092    272 _MPA	=	0x0092
                    0098    273 _SCO	=	0x0098
                    0099    274 _SBU	=	0x0099
                    009A    275 _APTR1H	=	0x009a
                    009B    276 _APTR1L	=	0x009b
                    009C    277 _AUTODAT1	=	0x009c
                    009D    278 _AUTOPTRH2	=	0x009d
                    009E    279 _AUTOPTRL2	=	0x009e
                    009F    280 _AUTODAT2	=	0x009f
                    00A0    281 _IOC	=	0x00a0
                    00A1    282 _INT2CLR	=	0x00a1
                    00A2    283 _INT4CLR	=	0x00a2
                    00A8    284 _IE	=	0x00a8
                    00AA    285 _EP2468STAT	=	0x00aa
                    00AB    286 _EP24FIFOFLGS	=	0x00ab
                    00AC    287 _EP68FIFOFLGS	=	0x00ac
                    00AF    288 _AUTOPTRSETUP	=	0x00af
                    00B0    289 _IOD	=	0x00b0
                    00B1    290 _IOE	=	0x00b1
                    00B2    291 _OEA	=	0x00b2
                    00B3    292 _OEB	=	0x00b3
                    00B4    293 _OEC	=	0x00b4
                    00B5    294 _OED	=	0x00b5
                    00B6    295 _OEE	=	0x00b6
                    00B8    296 _IP	=	0x00b8
                    00BA    297 _EP01STAT	=	0x00ba
                    00BB    298 _GPIFTRIG	=	0x00bb
                    00BD    299 _GPIFSGLDATH	=	0x00bd
                    00BE    300 _GPIFSGLDATLX	=	0x00be
                    00BF    301 _GPIFSGLDATLNOX	=	0x00bf
                    00C0    302 _SCON1	=	0x00c0
                    00C1    303 _SBUF1	=	0x00c1
                    00C8    304 _T2CON	=	0x00c8
                    00CA    305 _RCAP2L	=	0x00ca
                    00CB    306 _RCAP2H	=	0x00cb
                    00CC    307 _TL2	=	0x00cc
                    00CD    308 _TH2	=	0x00cd
                    00D0    309 _PSW	=	0x00d0
                    00D8    310 _EICON	=	0x00d8
                    00E0    311 _ACC	=	0x00e0
                    00E8    312 _EIE	=	0x00e8
                    00F0    313 _B	=	0x00f0
                    00F8    314 _EIP	=	0x00f8
                            315 ;--------------------------------------------------------
                            316 ; special function bits
                            317 ;--------------------------------------------------------
                            318 	.area RSEG    (DATA)
                            319 ;--------------------------------------------------------
                            320 ; overlayable register banks
                            321 ;--------------------------------------------------------
                            322 	.area REG_BANK_0	(REL,OVR,DATA)
   0000                     323 	.ds 8
                            324 ;--------------------------------------------------------
                            325 ; internal ram data
                            326 ;--------------------------------------------------------
                            327 	.area DSEG    (DATA)
                            328 ;--------------------------------------------------------
                            329 ; overlayable items in internal ram 
                            330 ;--------------------------------------------------------
                            331 	.area	OSEG    (OVR,DATA)
                            332 ;--------------------------------------------------------
                            333 ; Stack segment in internal ram 
                            334 ;--------------------------------------------------------
                            335 	.area	SSEG	(DATA)
   0008                     336 __start__stack:
   0008                     337 	.ds	1
                            338 
                            339 ;--------------------------------------------------------
                            340 ; indirectly addressable internal ram data
                            341 ;--------------------------------------------------------
                            342 	.area ISEG    (DATA)
                            343 ;--------------------------------------------------------
                            344 ; absolute internal ram data
                            345 ;--------------------------------------------------------
                            346 	.area IABS    (ABS,DATA)
                            347 	.area IABS    (ABS,DATA)
                            348 ;--------------------------------------------------------
                            349 ; bit data
                            350 ;--------------------------------------------------------
                            351 	.area BSEG    (BIT)
                            352 ;--------------------------------------------------------
                            353 ; paged external ram data
                            354 ;--------------------------------------------------------
                            355 	.area PSEG    (PAG,XDATA)
                            356 ;--------------------------------------------------------
                            357 ; external ram data
                            358 ;--------------------------------------------------------
                            359 	.area XSEG    (XDATA)
                    E400    360 _GPIF_WAVE_DATA	=	0xe400
                    E480    361 _RES_WAVEDATA_END	=	0xe480
                    E600    362 _CPUCS	=	0xe600
                    E601    363 _IFCONFIG	=	0xe601
                    E602    364 _PINFLAGSAB	=	0xe602
                    E603    365 _PINFLAGSCD	=	0xe603
                    E604    366 _FIFORESET	=	0xe604
                    E605    367 _BREAKPT	=	0xe605
                    E606    368 _BPADDRH	=	0xe606
                    E607    369 _BPADDRL	=	0xe607
                    E608    370 _UART230	=	0xe608
                    E609    371 _FIFOPINPOLAR	=	0xe609
                    E60A    372 _REVID	=	0xe60a
                    E60B    373 _REVCTL	=	0xe60b
                    E610    374 _EP1OUTCFG	=	0xe610
                    E611    375 _EP1INCFG	=	0xe611
                    E612    376 _EP2CFG	=	0xe612
                    E613    377 _EP4CFG	=	0xe613
                    E614    378 _EP6CFG	=	0xe614
                    E615    379 _EP8CFG	=	0xe615
                    E618    380 _EP2FIFOCFG	=	0xe618
                    E619    381 _EP4FIFOCFG	=	0xe619
                    E61A    382 _EP6FIFOCFG	=	0xe61a
                    E61B    383 _EP8FIFOCFG	=	0xe61b
                    E620    384 _EP2AUTOINLENH	=	0xe620
                    E621    385 _EP2AUTOINLENL	=	0xe621
                    E622    386 _EP4AUTOINLENH	=	0xe622
                    E623    387 _EP4AUTOINLENL	=	0xe623
                    E624    388 _EP6AUTOINLENH	=	0xe624
                    E625    389 _EP6AUTOINLENL	=	0xe625
                    E626    390 _EP8AUTOINLENH	=	0xe626
                    E627    391 _EP8AUTOINLENL	=	0xe627
                    E630    392 _EP2FIFOPFH	=	0xe630
                    E631    393 _EP2FIFOPFL	=	0xe631
                    E632    394 _EP4FIFOPFH	=	0xe632
                    E633    395 _EP4FIFOPFL	=	0xe633
                    E634    396 _EP6FIFOPFH	=	0xe634
                    E635    397 _EP6FIFOPFL	=	0xe635
                    E636    398 _EP8FIFOPFH	=	0xe636
                    E637    399 _EP8FIFOPFL	=	0xe637
                    E640    400 _EP2ISOINPKTS	=	0xe640
                    E641    401 _EP4ISOINPKTS	=	0xe641
                    E642    402 _EP6ISOINPKTS	=	0xe642
                    E643    403 _EP8ISOINPKTS	=	0xe643
                    E648    404 _INPKTEND	=	0xe648
                    E649    405 _OUTPKTEND	=	0xe649
                    E650    406 _EP2FIFOIE	=	0xe650
                    E651    407 _EP2FIFOIRQ	=	0xe651
                    E652    408 _EP4FIFOIE	=	0xe652
                    E653    409 _EP4FIFOIRQ	=	0xe653
                    E654    410 _EP6FIFOIE	=	0xe654
                    E655    411 _EP6FIFOIRQ	=	0xe655
                    E656    412 _EP8FIFOIE	=	0xe656
                    E657    413 _EP8FIFOIRQ	=	0xe657
                    E658    414 _IBNIE	=	0xe658
                    E659    415 _IBNIRQ	=	0xe659
                    E65A    416 _NAKIE	=	0xe65a
                    E65B    417 _NAKIRQ	=	0xe65b
                    E65C    418 _USBIE	=	0xe65c
                    E65D    419 _USBIRQ	=	0xe65d
                    E65E    420 _EPIE	=	0xe65e
                    E65F    421 _EPIRQ	=	0xe65f
                    E660    422 _GPIFIE	=	0xe660
                    E661    423 _GPIFIRQ	=	0xe661
                    E662    424 _USBERRIE	=	0xe662
                    E663    425 _USBERRIRQ	=	0xe663
                    E664    426 _ERRCNTLIM	=	0xe664
                    E665    427 _CLRERRCNT	=	0xe665
                    E666    428 _INT2IVEC	=	0xe666
                    E667    429 _INT4IVEC	=	0xe667
                    E668    430 _INTSETUP	=	0xe668
                    E670    431 _PORTACFG	=	0xe670
                    E671    432 _PORTCCFG	=	0xe671
                    E672    433 _PORTECFG	=	0xe672
                    E678    434 _I2CS	=	0xe678
                    E679    435 _I2DAT	=	0xe679
                    E67A    436 _I2CTL	=	0xe67a
                    E67B    437 _XAUTODAT1	=	0xe67b
                    E67C    438 _XAUTODAT2	=	0xe67c
                    E680    439 _USBCS	=	0xe680
                    E681    440 _SUSPEND	=	0xe681
                    E682    441 _WAKEUPCS	=	0xe682
                    E683    442 _TOGCTL	=	0xe683
                    E684    443 _USBFRAMEH	=	0xe684
                    E685    444 _USBFRAMEL	=	0xe685
                    E686    445 _MICROFRAME	=	0xe686
                    E687    446 _FNADDR	=	0xe687
                    E68A    447 _EP0BCH	=	0xe68a
                    E68B    448 _EP0BCL	=	0xe68b
                    E68D    449 _EP1OUTBC	=	0xe68d
                    E68F    450 _EP1INBC	=	0xe68f
                    E690    451 _EP2BCH	=	0xe690
                    E691    452 _EP2BCL	=	0xe691
                    E694    453 _EP4BCH	=	0xe694
                    E695    454 _EP4BCL	=	0xe695
                    E698    455 _EP6BCH	=	0xe698
                    E699    456 _EP6BCL	=	0xe699
                    E69C    457 _EP8BCH	=	0xe69c
                    E69D    458 _EP8BCL	=	0xe69d
                    E6A0    459 _EP0CS	=	0xe6a0
                    E6A1    460 _EP1OUTCS	=	0xe6a1
                    E6A2    461 _EP1INCS	=	0xe6a2
                    E6A3    462 _EP2CS	=	0xe6a3
                    E6A4    463 _EP4CS	=	0xe6a4
                    E6A5    464 _EP6CS	=	0xe6a5
                    E6A6    465 _EP8CS	=	0xe6a6
                    E6A7    466 _EP2FIFOFLGS	=	0xe6a7
                    E6A8    467 _EP4FIFOFLGS	=	0xe6a8
                    E6A9    468 _EP6FIFOFLGS	=	0xe6a9
                    E6AA    469 _EP8FIFOFLGS	=	0xe6aa
                    E6AB    470 _EP2FIFOBCH	=	0xe6ab
                    E6AC    471 _EP2FIFOBCL	=	0xe6ac
                    E6AD    472 _EP4FIFOBCH	=	0xe6ad
                    E6AE    473 _EP4FIFOBCL	=	0xe6ae
                    E6AF    474 _EP6FIFOBCH	=	0xe6af
                    E6B0    475 _EP6FIFOBCL	=	0xe6b0
                    E6B1    476 _EP8FIFOBCH	=	0xe6b1
                    E6B2    477 _EP8FIFOBCL	=	0xe6b2
                    E6B3    478 _SUDPTRH	=	0xe6b3
                    E6B4    479 _SUDPTRL	=	0xe6b4
                    E6B5    480 _SUDPTRCTL	=	0xe6b5
                    E6B8    481 _SETUPDAT	=	0xe6b8
                    E6C0    482 _GPIFWFSELECT	=	0xe6c0
                    E6C1    483 _GPIFIDLECS	=	0xe6c1
                    E6C2    484 _GPIFIDLECTL	=	0xe6c2
                    E6C3    485 _GPIFCTLCFG	=	0xe6c3
                    E6C4    486 _GPIFADRH	=	0xe6c4
                    E6C5    487 _GPIFADRL	=	0xe6c5
                    E6CE    488 _GPIFTCB3	=	0xe6ce
                    E6CF    489 _GPIFTCB2	=	0xe6cf
                    E6D0    490 _GPIFTCB1	=	0xe6d0
                    E6D1    491 _GPIFTCB0	=	0xe6d1
                    E6D2    492 _EP2GPIFFLGSEL	=	0xe6d2
                    E6D3    493 _EP2GPIFPFSTOP	=	0xe6d3
                    E6D4    494 _EP2GPIFTRIG	=	0xe6d4
                    E6DA    495 _EP4GPIFFLGSEL	=	0xe6da
                    E6DB    496 _EP4GPIFPFSTOP	=	0xe6db
                    E6DC    497 _EP4GPIFTRIG	=	0xe6dc
                    E6E2    498 _EP6GPIFFLGSEL	=	0xe6e2
                    E6E3    499 _EP6GPIFPFSTOP	=	0xe6e3
                    E6E4    500 _EP6GPIFTRIG	=	0xe6e4
                    E6EA    501 _EP8GPIFFLGSEL	=	0xe6ea
                    E6EB    502 _EP8GPIFPFSTOP	=	0xe6eb
                    E6EC    503 _EP8GPIFTRIG	=	0xe6ec
                    E6F0    504 _XGPIFSGLDATH	=	0xe6f0
                    E6F1    505 _XGPIFSGLDATLX	=	0xe6f1
                    E6F2    506 _XGPIFSGLDATLNOX	=	0xe6f2
                    E6F3    507 _GPIFREADYCFG	=	0xe6f3
                    E6F4    508 _GPIFREADYSTAT	=	0xe6f4
                    E6F5    509 _GPIFABORT	=	0xe6f5
                    E6C6    510 _FLOWSTATE	=	0xe6c6
                    E6C7    511 _FLOWLOGIC	=	0xe6c7
                    E6C8    512 _FLOWEQ0CTL	=	0xe6c8
                    E6C9    513 _FLOWEQ1CTL	=	0xe6c9
                    E6CA    514 _FLOWHOLDOFF	=	0xe6ca
                    E6CB    515 _FLOWSTB	=	0xe6cb
                    E6CC    516 _FLOWSTBEDGE	=	0xe6cc
                    E6CD    517 _FLOWSTBHPERIOD	=	0xe6cd
                    E60C    518 _GPIFHOLDAMOUNT	=	0xe60c
                    E67D    519 _UDMACRCH	=	0xe67d
                    E67E    520 _UDMACRCL	=	0xe67e
                    E67F    521 _UDMACRCQUAL	=	0xe67f
                    E6F8    522 _DBUG	=	0xe6f8
                    E6F9    523 _TESTCFG	=	0xe6f9
                    E6FA    524 _USBTEST	=	0xe6fa
                    E6FB    525 _CT1	=	0xe6fb
                    E6FC    526 _CT2	=	0xe6fc
                    E6FD    527 _CT3	=	0xe6fd
                    E6FE    528 _CT4	=	0xe6fe
                    E740    529 _EP0BUF	=	0xe740
                    E780    530 _EP1OUTBUF	=	0xe780
                    E7C0    531 _EP1INBUF	=	0xe7c0
                    F000    532 _EP2FIFOBUF	=	0xf000
                    F400    533 _EP4FIFOBUF	=	0xf400
                    F800    534 _EP6FIFOBUF	=	0xf800
                    FC00    535 _EP8FIFOBUF	=	0xfc00
                            536 ;--------------------------------------------------------
                            537 ; absolute external ram data
                            538 ;--------------------------------------------------------
                            539 	.area XABS    (ABS,XDATA)
                            540 ;--------------------------------------------------------
                            541 ; external initialized ram data
                            542 ;--------------------------------------------------------
                            543 	.area XISEG   (XDATA)
                            544 	.area HOME    (CODE)
                            545 	.area GSINIT0 (CODE)
                            546 	.area GSINIT1 (CODE)
                            547 	.area GSINIT2 (CODE)
                            548 	.area GSINIT3 (CODE)
                            549 	.area GSINIT4 (CODE)
                            550 	.area GSINIT5 (CODE)
                            551 	.area GSINIT  (CODE)
                            552 	.area GSFINAL (CODE)
                            553 	.area CSEG    (CODE)
                            554 ;--------------------------------------------------------
                            555 ; interrupt vector 
                            556 ;--------------------------------------------------------
                            557 	.area HOME    (CODE)
   0000                     558 __interrupt_vect:
   0000 02 00 08            559 	ljmp	__sdcc_gsinit_startup
                            560 ;--------------------------------------------------------
                            561 ; global & static initialisations
                            562 ;--------------------------------------------------------
                            563 	.area HOME    (CODE)
                            564 	.area GSINIT  (CODE)
                            565 	.area GSFINAL (CODE)
                            566 	.area GSINIT  (CODE)
                            567 	.globl __sdcc_gsinit_startup
                            568 	.globl __sdcc_program_startup
                            569 	.globl __start__stack
                            570 	.globl __mcs51_genXINIT
                            571 	.globl __mcs51_genXRAMCLEAR
                            572 	.globl __mcs51_genRAMCLEAR
                            573 	.area GSFINAL (CODE)
   0061 02 00 03            574 	ljmp	__sdcc_program_startup
                            575 ;--------------------------------------------------------
                            576 ; Home
                            577 ;--------------------------------------------------------
                            578 	.area HOME    (CODE)
                            579 	.area HOME    (CODE)
   0003                     580 __sdcc_program_startup:
   0003 12 03 A6            581 	lcall	_main
                            582 ;	return from main will lock up
   0006 80 FE               583 	sjmp .
                            584 ;--------------------------------------------------------
                            585 ; code
                            586 ;--------------------------------------------------------
                            587 	.area CSEG    (CODE)
                            588 ;------------------------------------------------------------
                            589 ;Allocation info for local variables in function 'Initialize'
                            590 ;------------------------------------------------------------
                            591 ;cfg_data                  Allocated to registers 
                            592 ;cfg_data_ok               Allocated to registers r5 
                            593 ;------------------------------------------------------------
                            594 ;	fx2pipe.c:56: static void Initialize(void)
                            595 ;	-----------------------------------------
                            596 ;	 function Initialize
                            597 ;	-----------------------------------------
   0064                     598 _Initialize:
                    0002    599 	ar2 = 0x02
                    0003    600 	ar3 = 0x03
                    0004    601 	ar4 = 0x04
                    0005    602 	ar5 = 0x05
                    0006    603 	ar6 = 0x06
                    0007    604 	ar7 = 0x07
                    0000    605 	ar0 = 0x00
                    0001    606 	ar1 = 0x01
                            607 ;	fx2pipe.c:65: char cfg_data_ok = (cfg_data[0]==0x12U || cfg_data[0]==0x21U);
   0064 90 10 03            608 	mov	dptr,#0x1003
   0067 E0                  609 	movx	a,@dptr
   0068 FA                  610 	mov	r2,a
   0069 33                  611 	rlc	a
   006A 95 E0               612 	subb	a,acc
   006C FB                  613 	mov	r3,a
   006D E4                  614 	clr	a
   006E BA 12 04            615 	cjne	r2,#0x12,00126$
   0071 BB 00 01            616 	cjne	r3,#0x00,00126$
   0074 04                  617 	inc	a
   0075                     618 00126$:
   0075 FC                  619 	mov	r4,a
   0076 70 0C               620 	jnz	00112$
   0078 BA 21 05            621 	cjne	r2,#0x21,00129$
   007B BB 00 02            622 	cjne	r3,#0x00,00129$
   007E 80 04               623 	sjmp	00112$
   0080                     624 00129$:
   0080 7D 00               625 	mov	r5,#0x00
   0082 80 02               626 	sjmp	00113$
   0084                     627 00112$:
   0084 7D 01               628 	mov	r5,#0x01
   0086                     629 00113$:
                            630 ;	fx2pipe.c:67: SYNCDELAY;
   0086 00                  631 	 nop 
   0087 00                  632 	 nop 
   0088 00                  633 	 nop 
   0089 00                  634 	 nop 
   008A 00                  635 	 nop 
   008B 00                  636 	 nop 
   008C 00                  637 	 nop 
   008D 00                  638 	 nop 
   008E 00                  639 	 nop 
   008F 00                  640 	 nop 
   0090 00                  641 	 nop 
   0091 00                  642 	 nop 
   0092 00                  643 	 nop 
   0093 00                  644 	 nop 
   0094 00                  645 	 nop 
   0095 00                  646 	 nop 
   0096 00                  647 	 nop 
                            648 ;	fx2pipe.c:77: CPUCS = cfg_data_ok ? cfg_data[4] : 0x12;  // 0x12
   0097 ED                  649 	mov	a,r5
   0098 60 07               650 	jz	00114$
   009A 90 10 07            651 	mov	dptr,#0x1007
   009D E0                  652 	movx	a,@dptr
   009E FE                  653 	mov	r6,a
   009F 80 02               654 	sjmp	00115$
   00A1                     655 00114$:
   00A1 7E 12               656 	mov	r6,#0x12
   00A3                     657 00115$:
   00A3 90 E6 00            658 	mov	dptr,#_CPUCS
   00A6 EE                  659 	mov	a,r6
   00A7 F0                  660 	movx	@dptr,a
                            661 ;	fx2pipe.c:78: SYNCDELAY;
   00A8 00                  662 	 nop 
   00A9 00                  663 	 nop 
   00AA 00                  664 	 nop 
   00AB 00                  665 	 nop 
   00AC 00                  666 	 nop 
   00AD 00                  667 	 nop 
   00AE 00                  668 	 nop 
   00AF 00                  669 	 nop 
   00B0 00                  670 	 nop 
   00B1 00                  671 	 nop 
   00B2 00                  672 	 nop 
   00B3 00                  673 	 nop 
   00B4 00                  674 	 nop 
   00B5 00                  675 	 nop 
   00B6 00                  676 	 nop 
   00B7 00                  677 	 nop 
   00B8 00                  678 	 nop 
                            679 ;	fx2pipe.c:93: IFCONFIG = cfg_data_ok ? cfg_data[1] : 0xc3;
   00B9 ED                  680 	mov	a,r5
   00BA 60 07               681 	jz	00116$
   00BC 90 10 04            682 	mov	dptr,#0x1004
   00BF E0                  683 	movx	a,@dptr
   00C0 FD                  684 	mov	r5,a
   00C1 80 02               685 	sjmp	00117$
   00C3                     686 00116$:
   00C3 7D C3               687 	mov	r5,#0xC3
   00C5                     688 00117$:
   00C5 90 E6 01            689 	mov	dptr,#_IFCONFIG
   00C8 ED                  690 	mov	a,r5
   00C9 F0                  691 	movx	@dptr,a
                            692 ;	fx2pipe.c:94: SYNCDELAY;
   00CA 00                  693 	 nop 
   00CB 00                  694 	 nop 
   00CC 00                  695 	 nop 
   00CD 00                  696 	 nop 
   00CE 00                  697 	 nop 
   00CF 00                  698 	 nop 
   00D0 00                  699 	 nop 
   00D1 00                  700 	 nop 
   00D2 00                  701 	 nop 
   00D3 00                  702 	 nop 
   00D4 00                  703 	 nop 
   00D5 00                  704 	 nop 
   00D6 00                  705 	 nop 
   00D7 00                  706 	 nop 
   00D8 00                  707 	 nop 
   00D9 00                  708 	 nop 
   00DA 00                  709 	 nop 
                            710 ;	fx2pipe.c:97: REVCTL = 0x03;  // See TRM...
   00DB 90 E6 0B            711 	mov	dptr,#_REVCTL
   00DE 74 03               712 	mov	a,#0x03
   00E0 F0                  713 	movx	@dptr,a
                            714 ;	fx2pipe.c:98: SYNCDELAY;
   00E1 00                  715 	 nop 
   00E2 00                  716 	 nop 
   00E3 00                  717 	 nop 
   00E4 00                  718 	 nop 
   00E5 00                  719 	 nop 
   00E6 00                  720 	 nop 
   00E7 00                  721 	 nop 
   00E8 00                  722 	 nop 
   00E9 00                  723 	 nop 
   00EA 00                  724 	 nop 
   00EB 00                  725 	 nop 
   00EC 00                  726 	 nop 
   00ED 00                  727 	 nop 
   00EE 00                  728 	 nop 
   00EF 00                  729 	 nop 
   00F0 00                  730 	 nop 
   00F1 00                  731 	 nop 
                            732 ;	fx2pipe.c:101: PORTACFG = 0x00;
   00F2 90 E6 70            733 	mov	dptr,#_PORTACFG
   00F5 E4                  734 	clr	a
   00F6 F0                  735 	movx	@dptr,a
                            736 ;	fx2pipe.c:102: SYNCDELAY; // maybe not needed
   00F7 00                  737 	 nop 
   00F8 00                  738 	 nop 
   00F9 00                  739 	 nop 
   00FA 00                  740 	 nop 
   00FB 00                  741 	 nop 
   00FC 00                  742 	 nop 
   00FD 00                  743 	 nop 
   00FE 00                  744 	 nop 
   00FF 00                  745 	 nop 
   0100 00                  746 	 nop 
   0101 00                  747 	 nop 
   0102 00                  748 	 nop 
   0103 00                  749 	 nop 
   0104 00                  750 	 nop 
   0105 00                  751 	 nop 
   0106 00                  752 	 nop 
   0107 00                  753 	 nop 
                            754 ;	fx2pipe.c:105: FIFOPINPOLAR=0x00;
   0108 90 E6 09            755 	mov	dptr,#_FIFOPINPOLAR
   010B E4                  756 	clr	a
   010C F0                  757 	movx	@dptr,a
                            758 ;	fx2pipe.c:106: SYNCDELAY;
   010D 00                  759 	 nop 
   010E 00                  760 	 nop 
   010F 00                  761 	 nop 
   0110 00                  762 	 nop 
   0111 00                  763 	 nop 
   0112 00                  764 	 nop 
   0113 00                  765 	 nop 
   0114 00                  766 	 nop 
   0115 00                  767 	 nop 
   0116 00                  768 	 nop 
   0117 00                  769 	 nop 
   0118 00                  770 	 nop 
   0119 00                  771 	 nop 
   011A 00                  772 	 nop 
   011B 00                  773 	 nop 
   011C 00                  774 	 nop 
   011D 00                  775 	 nop 
                            776 ;	fx2pipe.c:109: EP6CFG=0x00U;  SYNCDELAY;
   011E 90 E6 14            777 	mov	dptr,#_EP6CFG
   0121 E4                  778 	clr	a
   0122 F0                  779 	movx	@dptr,a
   0123 00                  780 	 nop 
   0124 00                  781 	 nop 
   0125 00                  782 	 nop 
   0126 00                  783 	 nop 
   0127 00                  784 	 nop 
   0128 00                  785 	 nop 
   0129 00                  786 	 nop 
   012A 00                  787 	 nop 
   012B 00                  788 	 nop 
   012C 00                  789 	 nop 
   012D 00                  790 	 nop 
   012E 00                  791 	 nop 
   012F 00                  792 	 nop 
   0130 00                  793 	 nop 
   0131 00                  794 	 nop 
   0132 00                  795 	 nop 
   0133 00                  796 	 nop 
                            797 ;	fx2pipe.c:110: EP2CFG=0x00U;  SYNCDELAY;
   0134 90 E6 12            798 	mov	dptr,#_EP2CFG
   0137 E4                  799 	clr	a
   0138 F0                  800 	movx	@dptr,a
   0139 00                  801 	 nop 
   013A 00                  802 	 nop 
   013B 00                  803 	 nop 
   013C 00                  804 	 nop 
   013D 00                  805 	 nop 
   013E 00                  806 	 nop 
   013F 00                  807 	 nop 
   0140 00                  808 	 nop 
   0141 00                  809 	 nop 
   0142 00                  810 	 nop 
   0143 00                  811 	 nop 
   0144 00                  812 	 nop 
   0145 00                  813 	 nop 
   0146 00                  814 	 nop 
   0147 00                  815 	 nop 
   0148 00                  816 	 nop 
   0149 00                  817 	 nop 
                            818 ;	fx2pipe.c:111: EP6FIFOCFG=0x00U;  SYNCDELAY;
   014A 90 E6 1A            819 	mov	dptr,#_EP6FIFOCFG
   014D E4                  820 	clr	a
   014E F0                  821 	movx	@dptr,a
   014F 00                  822 	 nop 
   0150 00                  823 	 nop 
   0151 00                  824 	 nop 
   0152 00                  825 	 nop 
   0153 00                  826 	 nop 
   0154 00                  827 	 nop 
   0155 00                  828 	 nop 
   0156 00                  829 	 nop 
   0157 00                  830 	 nop 
   0158 00                  831 	 nop 
   0159 00                  832 	 nop 
   015A 00                  833 	 nop 
   015B 00                  834 	 nop 
   015C 00                  835 	 nop 
   015D 00                  836 	 nop 
   015E 00                  837 	 nop 
   015F 00                  838 	 nop 
                            839 ;	fx2pipe.c:112: EP2FIFOCFG=0x00U;  SYNCDELAY;
   0160 90 E6 18            840 	mov	dptr,#_EP2FIFOCFG
   0163 E4                  841 	clr	a
   0164 F0                  842 	movx	@dptr,a
   0165 00                  843 	 nop 
   0166 00                  844 	 nop 
   0167 00                  845 	 nop 
   0168 00                  846 	 nop 
   0169 00                  847 	 nop 
   016A 00                  848 	 nop 
   016B 00                  849 	 nop 
   016C 00                  850 	 nop 
   016D 00                  851 	 nop 
   016E 00                  852 	 nop 
   016F 00                  853 	 nop 
   0170 00                  854 	 nop 
   0171 00                  855 	 nop 
   0172 00                  856 	 nop 
   0173 00                  857 	 nop 
   0174 00                  858 	 nop 
   0175 00                  859 	 nop 
                            860 ;	fx2pipe.c:113: OEA=0x00U;
   0176 75 B2 00            861 	mov	_OEA,#0x00
                            862 ;	fx2pipe.c:115: if(cfg_data[0]==0x12U) /* INPUT: USB->HOST */
   0179 EC                  863 	mov	a,r4
   017A 70 03               864 	jnz	00132$
   017C 02 02 6A            865 	ljmp	00107$
   017F                     866 00132$:
                            867 ;	fx2pipe.c:126: EP6CFG = cfg_data[2];  // bulk: 0xe2 double-buffered; 0xe3 triple-; 0xe0 quad
   017F 90 10 05            868 	mov	dptr,#0x1005
   0182 E0                  869 	movx	a,@dptr
   0183 FC                  870 	mov	r4,a
   0184 90 E6 14            871 	mov	dptr,#_EP6CFG
   0187 F0                  872 	movx	@dptr,a
                            873 ;	fx2pipe.c:127: SYNCDELAY;
   0188 00                  874 	 nop 
   0189 00                  875 	 nop 
   018A 00                  876 	 nop 
   018B 00                  877 	 nop 
   018C 00                  878 	 nop 
   018D 00                  879 	 nop 
   018E 00                  880 	 nop 
   018F 00                  881 	 nop 
   0190 00                  882 	 nop 
   0191 00                  883 	 nop 
   0192 00                  884 	 nop 
   0193 00                  885 	 nop 
   0194 00                  886 	 nop 
   0195 00                  887 	 nop 
   0196 00                  888 	 nop 
   0197 00                  889 	 nop 
   0198 00                  890 	 nop 
                            891 ;	fx2pipe.c:131: FIFORESET = 0x80;  SYNCDELAY;  // NAK all requests from host. 
   0199 90 E6 04            892 	mov	dptr,#_FIFORESET
   019C 74 80               893 	mov	a,#0x80
   019E F0                  894 	movx	@dptr,a
   019F 00                  895 	 nop 
   01A0 00                  896 	 nop 
   01A1 00                  897 	 nop 
   01A2 00                  898 	 nop 
   01A3 00                  899 	 nop 
   01A4 00                  900 	 nop 
   01A5 00                  901 	 nop 
   01A6 00                  902 	 nop 
   01A7 00                  903 	 nop 
   01A8 00                  904 	 nop 
   01A9 00                  905 	 nop 
   01AA 00                  906 	 nop 
   01AB 00                  907 	 nop 
   01AC 00                  908 	 nop 
   01AD 00                  909 	 nop 
   01AE 00                  910 	 nop 
   01AF 00                  911 	 nop 
                            912 ;	fx2pipe.c:132: FIFORESET = 0x82;  SYNCDELAY;  // Reset individual EP (2,4,6,8)
   01B0 90 E6 04            913 	mov	dptr,#_FIFORESET
   01B3 74 82               914 	mov	a,#0x82
   01B5 F0                  915 	movx	@dptr,a
   01B6 00                  916 	 nop 
   01B7 00                  917 	 nop 
   01B8 00                  918 	 nop 
   01B9 00                  919 	 nop 
   01BA 00                  920 	 nop 
   01BB 00                  921 	 nop 
   01BC 00                  922 	 nop 
   01BD 00                  923 	 nop 
   01BE 00                  924 	 nop 
   01BF 00                  925 	 nop 
   01C0 00                  926 	 nop 
   01C1 00                  927 	 nop 
   01C2 00                  928 	 nop 
   01C3 00                  929 	 nop 
   01C4 00                  930 	 nop 
   01C5 00                  931 	 nop 
   01C6 00                  932 	 nop 
                            933 ;	fx2pipe.c:133: FIFORESET = 0x84;  SYNCDELAY;
   01C7 90 E6 04            934 	mov	dptr,#_FIFORESET
   01CA 74 84               935 	mov	a,#0x84
   01CC F0                  936 	movx	@dptr,a
   01CD 00                  937 	 nop 
   01CE 00                  938 	 nop 
   01CF 00                  939 	 nop 
   01D0 00                  940 	 nop 
   01D1 00                  941 	 nop 
   01D2 00                  942 	 nop 
   01D3 00                  943 	 nop 
   01D4 00                  944 	 nop 
   01D5 00                  945 	 nop 
   01D6 00                  946 	 nop 
   01D7 00                  947 	 nop 
   01D8 00                  948 	 nop 
   01D9 00                  949 	 nop 
   01DA 00                  950 	 nop 
   01DB 00                  951 	 nop 
   01DC 00                  952 	 nop 
   01DD 00                  953 	 nop 
                            954 ;	fx2pipe.c:134: FIFORESET = 0x86;  SYNCDELAY;
   01DE 90 E6 04            955 	mov	dptr,#_FIFORESET
   01E1 74 86               956 	mov	a,#0x86
   01E3 F0                  957 	movx	@dptr,a
   01E4 00                  958 	 nop 
   01E5 00                  959 	 nop 
   01E6 00                  960 	 nop 
   01E7 00                  961 	 nop 
   01E8 00                  962 	 nop 
   01E9 00                  963 	 nop 
   01EA 00                  964 	 nop 
   01EB 00                  965 	 nop 
   01EC 00                  966 	 nop 
   01ED 00                  967 	 nop 
   01EE 00                  968 	 nop 
   01EF 00                  969 	 nop 
   01F0 00                  970 	 nop 
   01F1 00                  971 	 nop 
   01F2 00                  972 	 nop 
   01F3 00                  973 	 nop 
   01F4 00                  974 	 nop 
                            975 ;	fx2pipe.c:135: FIFORESET = 0x88;  SYNCDELAY;
   01F5 90 E6 04            976 	mov	dptr,#_FIFORESET
   01F8 74 88               977 	mov	a,#0x88
   01FA F0                  978 	movx	@dptr,a
   01FB 00                  979 	 nop 
   01FC 00                  980 	 nop 
   01FD 00                  981 	 nop 
   01FE 00                  982 	 nop 
   01FF 00                  983 	 nop 
   0200 00                  984 	 nop 
   0201 00                  985 	 nop 
   0202 00                  986 	 nop 
   0203 00                  987 	 nop 
   0204 00                  988 	 nop 
   0205 00                  989 	 nop 
   0206 00                  990 	 nop 
   0207 00                  991 	 nop 
   0208 00                  992 	 nop 
   0209 00                  993 	 nop 
   020A 00                  994 	 nop 
   020B 00                  995 	 nop 
                            996 ;	fx2pipe.c:136: FIFORESET = 0x00;  SYNCDELAY;  // Resume normal operation. 
   020C 90 E6 04            997 	mov	dptr,#_FIFORESET
   020F E4                  998 	clr	a
   0210 F0                  999 	movx	@dptr,a
   0211 00                 1000 	 nop 
   0212 00                 1001 	 nop 
   0213 00                 1002 	 nop 
   0214 00                 1003 	 nop 
   0215 00                 1004 	 nop 
   0216 00                 1005 	 nop 
   0217 00                 1006 	 nop 
   0218 00                 1007 	 nop 
   0219 00                 1008 	 nop 
   021A 00                 1009 	 nop 
   021B 00                 1010 	 nop 
   021C 00                 1011 	 nop 
   021D 00                 1012 	 nop 
   021E 00                 1013 	 nop 
   021F 00                 1014 	 nop 
   0220 00                 1015 	 nop 
   0221 00                 1016 	 nop 
                           1017 ;	fx2pipe.c:148: EP6FIFOCFG = cfg_data[3]; /*0x0d //&0xfe*/;
   0222 90 10 06           1018 	mov	dptr,#0x1006
   0225 E0                 1019 	movx	a,@dptr
   0226 FC                 1020 	mov	r4,a
   0227 90 E6 1A           1021 	mov	dptr,#_EP6FIFOCFG
   022A F0                 1022 	movx	@dptr,a
                           1023 ;	fx2pipe.c:149: SYNCDELAY;
   022B 00                 1024 	 nop 
   022C 00                 1025 	 nop 
   022D 00                 1026 	 nop 
   022E 00                 1027 	 nop 
   022F 00                 1028 	 nop 
   0230 00                 1029 	 nop 
   0231 00                 1030 	 nop 
   0232 00                 1031 	 nop 
   0233 00                 1032 	 nop 
   0234 00                 1033 	 nop 
   0235 00                 1034 	 nop 
   0236 00                 1035 	 nop 
   0237 00                 1036 	 nop 
   0238 00                 1037 	 nop 
   0239 00                 1038 	 nop 
   023A 00                 1039 	 nop 
   023B 00                 1040 	 nop 
                           1041 ;	fx2pipe.c:154: EP6AUTOINLENH = 0x02; // MSB
   023C 90 E6 24           1042 	mov	dptr,#_EP6AUTOINLENH
   023F 74 02              1043 	mov	a,#0x02
   0241 F0                 1044 	movx	@dptr,a
                           1045 ;	fx2pipe.c:155: SYNCDELAY;
   0242 00                 1046 	 nop 
   0243 00                 1047 	 nop 
   0244 00                 1048 	 nop 
   0245 00                 1049 	 nop 
   0246 00                 1050 	 nop 
   0247 00                 1051 	 nop 
   0248 00                 1052 	 nop 
   0249 00                 1053 	 nop 
   024A 00                 1054 	 nop 
   024B 00                 1055 	 nop 
   024C 00                 1056 	 nop 
   024D 00                 1057 	 nop 
   024E 00                 1058 	 nop 
   024F 00                 1059 	 nop 
   0250 00                 1060 	 nop 
   0251 00                 1061 	 nop 
   0252 00                 1062 	 nop 
                           1063 ;	fx2pipe.c:156: EP6AUTOINLENL = 0x00; // LSB
   0253 90 E6 25           1064 	mov	dptr,#_EP6AUTOINLENL
   0256 E4                 1065 	clr	a
   0257 F0                 1066 	movx	@dptr,a
                           1067 ;	fx2pipe.c:157: SYNCDELAY;
   0258 00                 1068 	 nop 
   0259 00                 1069 	 nop 
   025A 00                 1070 	 nop 
   025B 00                 1071 	 nop 
   025C 00                 1072 	 nop 
   025D 00                 1073 	 nop 
   025E 00                 1074 	 nop 
   025F 00                 1075 	 nop 
   0260 00                 1076 	 nop 
   0261 00                 1077 	 nop 
   0262 00                 1078 	 nop 
   0263 00                 1079 	 nop 
   0264 00                 1080 	 nop 
   0265 00                 1081 	 nop 
   0266 00                 1082 	 nop 
   0267 00                 1083 	 nop 
   0268 00                 1084 	 nop 
   0269 22                 1085 	ret
   026A                    1086 00107$:
                           1087 ;	fx2pipe.c:165: else if(cfg_data[0]==0x21U) /* OUTPUT: HOST->USB */	
   026A BA 21 05           1088 	cjne	r2,#0x21,00133$
   026D BB 00 02           1089 	cjne	r3,#0x00,00133$
   0270 80 01              1090 	sjmp	00134$
   0272                    1091 00133$:
   0272 22                 1092 	ret
   0273                    1093 00134$:
                           1094 ;	fx2pipe.c:176: EP2CFG = cfg_data[2];  // bulk: 0xa2 double-buffered; 0xa3 triple-; 0xa0 quad
   0273 90 10 05           1095 	mov	dptr,#0x1005
   0276 E0                 1096 	movx	a,@dptr
   0277 FA                 1097 	mov	r2,a
   0278 90 E6 12           1098 	mov	dptr,#_EP2CFG
   027B F0                 1099 	movx	@dptr,a
                           1100 ;	fx2pipe.c:177: SYNCDELAY;
   027C 00                 1101 	 nop 
   027D 00                 1102 	 nop 
   027E 00                 1103 	 nop 
   027F 00                 1104 	 nop 
   0280 00                 1105 	 nop 
   0281 00                 1106 	 nop 
   0282 00                 1107 	 nop 
   0283 00                 1108 	 nop 
   0284 00                 1109 	 nop 
   0285 00                 1110 	 nop 
   0286 00                 1111 	 nop 
   0287 00                 1112 	 nop 
   0288 00                 1113 	 nop 
   0289 00                 1114 	 nop 
   028A 00                 1115 	 nop 
   028B 00                 1116 	 nop 
   028C 00                 1117 	 nop 
                           1118 ;	fx2pipe.c:181: FIFORESET = 0x80;  SYNCDELAY;  // NAK all requests from host. 
   028D 90 E6 04           1119 	mov	dptr,#_FIFORESET
   0290 74 80              1120 	mov	a,#0x80
   0292 F0                 1121 	movx	@dptr,a
   0293 00                 1122 	 nop 
   0294 00                 1123 	 nop 
   0295 00                 1124 	 nop 
   0296 00                 1125 	 nop 
   0297 00                 1126 	 nop 
   0298 00                 1127 	 nop 
   0299 00                 1128 	 nop 
   029A 00                 1129 	 nop 
   029B 00                 1130 	 nop 
   029C 00                 1131 	 nop 
   029D 00                 1132 	 nop 
   029E 00                 1133 	 nop 
   029F 00                 1134 	 nop 
   02A0 00                 1135 	 nop 
   02A1 00                 1136 	 nop 
   02A2 00                 1137 	 nop 
   02A3 00                 1138 	 nop 
                           1139 ;	fx2pipe.c:182: FIFORESET = 0x82;  SYNCDELAY;  // Reset individual EP (2,4,6,8)
   02A4 90 E6 04           1140 	mov	dptr,#_FIFORESET
   02A7 74 82              1141 	mov	a,#0x82
   02A9 F0                 1142 	movx	@dptr,a
   02AA 00                 1143 	 nop 
   02AB 00                 1144 	 nop 
   02AC 00                 1145 	 nop 
   02AD 00                 1146 	 nop 
   02AE 00                 1147 	 nop 
   02AF 00                 1148 	 nop 
   02B0 00                 1149 	 nop 
   02B1 00                 1150 	 nop 
   02B2 00                 1151 	 nop 
   02B3 00                 1152 	 nop 
   02B4 00                 1153 	 nop 
   02B5 00                 1154 	 nop 
   02B6 00                 1155 	 nop 
   02B7 00                 1156 	 nop 
   02B8 00                 1157 	 nop 
   02B9 00                 1158 	 nop 
   02BA 00                 1159 	 nop 
                           1160 ;	fx2pipe.c:183: FIFORESET = 0x84;  SYNCDELAY;
   02BB 90 E6 04           1161 	mov	dptr,#_FIFORESET
   02BE 74 84              1162 	mov	a,#0x84
   02C0 F0                 1163 	movx	@dptr,a
   02C1 00                 1164 	 nop 
   02C2 00                 1165 	 nop 
   02C3 00                 1166 	 nop 
   02C4 00                 1167 	 nop 
   02C5 00                 1168 	 nop 
   02C6 00                 1169 	 nop 
   02C7 00                 1170 	 nop 
   02C8 00                 1171 	 nop 
   02C9 00                 1172 	 nop 
   02CA 00                 1173 	 nop 
   02CB 00                 1174 	 nop 
   02CC 00                 1175 	 nop 
   02CD 00                 1176 	 nop 
   02CE 00                 1177 	 nop 
   02CF 00                 1178 	 nop 
   02D0 00                 1179 	 nop 
   02D1 00                 1180 	 nop 
                           1181 ;	fx2pipe.c:184: FIFORESET = 0x86;  SYNCDELAY;
   02D2 90 E6 04           1182 	mov	dptr,#_FIFORESET
   02D5 74 86              1183 	mov	a,#0x86
   02D7 F0                 1184 	movx	@dptr,a
   02D8 00                 1185 	 nop 
   02D9 00                 1186 	 nop 
   02DA 00                 1187 	 nop 
   02DB 00                 1188 	 nop 
   02DC 00                 1189 	 nop 
   02DD 00                 1190 	 nop 
   02DE 00                 1191 	 nop 
   02DF 00                 1192 	 nop 
   02E0 00                 1193 	 nop 
   02E1 00                 1194 	 nop 
   02E2 00                 1195 	 nop 
   02E3 00                 1196 	 nop 
   02E4 00                 1197 	 nop 
   02E5 00                 1198 	 nop 
   02E6 00                 1199 	 nop 
   02E7 00                 1200 	 nop 
   02E8 00                 1201 	 nop 
                           1202 ;	fx2pipe.c:185: FIFORESET = 0x88;  SYNCDELAY;
   02E9 90 E6 04           1203 	mov	dptr,#_FIFORESET
   02EC 74 88              1204 	mov	a,#0x88
   02EE F0                 1205 	movx	@dptr,a
   02EF 00                 1206 	 nop 
   02F0 00                 1207 	 nop 
   02F1 00                 1208 	 nop 
   02F2 00                 1209 	 nop 
   02F3 00                 1210 	 nop 
   02F4 00                 1211 	 nop 
   02F5 00                 1212 	 nop 
   02F6 00                 1213 	 nop 
   02F7 00                 1214 	 nop 
   02F8 00                 1215 	 nop 
   02F9 00                 1216 	 nop 
   02FA 00                 1217 	 nop 
   02FB 00                 1218 	 nop 
   02FC 00                 1219 	 nop 
   02FD 00                 1220 	 nop 
   02FE 00                 1221 	 nop 
   02FF 00                 1222 	 nop 
                           1223 ;	fx2pipe.c:186: FIFORESET = 0x00;  SYNCDELAY;  // Resume normal operation. 
   0300 90 E6 04           1224 	mov	dptr,#_FIFORESET
   0303 E4                 1225 	clr	a
   0304 F0                 1226 	movx	@dptr,a
   0305 00                 1227 	 nop 
   0306 00                 1228 	 nop 
   0307 00                 1229 	 nop 
   0308 00                 1230 	 nop 
   0309 00                 1231 	 nop 
   030A 00                 1232 	 nop 
   030B 00                 1233 	 nop 
   030C 00                 1234 	 nop 
   030D 00                 1235 	 nop 
   030E 00                 1236 	 nop 
   030F 00                 1237 	 nop 
   0310 00                 1238 	 nop 
   0311 00                 1239 	 nop 
   0312 00                 1240 	 nop 
   0313 00                 1241 	 nop 
   0314 00                 1242 	 nop 
   0315 00                 1243 	 nop 
                           1244 ;	fx2pipe.c:189: OUTPKTEND = 0x82;  SYNCDELAY;
   0316 90 E6 49           1245 	mov	dptr,#_OUTPKTEND
   0319 74 82              1246 	mov	a,#0x82
   031B F0                 1247 	movx	@dptr,a
   031C 00                 1248 	 nop 
   031D 00                 1249 	 nop 
   031E 00                 1250 	 nop 
   031F 00                 1251 	 nop 
   0320 00                 1252 	 nop 
   0321 00                 1253 	 nop 
   0322 00                 1254 	 nop 
   0323 00                 1255 	 nop 
   0324 00                 1256 	 nop 
   0325 00                 1257 	 nop 
   0326 00                 1258 	 nop 
   0327 00                 1259 	 nop 
   0328 00                 1260 	 nop 
   0329 00                 1261 	 nop 
   032A 00                 1262 	 nop 
   032B 00                 1263 	 nop 
   032C 00                 1264 	 nop 
                           1265 ;	fx2pipe.c:190: OUTPKTEND = 0x82;  SYNCDELAY;
   032D 90 E6 49           1266 	mov	dptr,#_OUTPKTEND
   0330 74 82              1267 	mov	a,#0x82
   0332 F0                 1268 	movx	@dptr,a
   0333 00                 1269 	 nop 
   0334 00                 1270 	 nop 
   0335 00                 1271 	 nop 
   0336 00                 1272 	 nop 
   0337 00                 1273 	 nop 
   0338 00                 1274 	 nop 
   0339 00                 1275 	 nop 
   033A 00                 1276 	 nop 
   033B 00                 1277 	 nop 
   033C 00                 1278 	 nop 
   033D 00                 1279 	 nop 
   033E 00                 1280 	 nop 
   033F 00                 1281 	 nop 
   0340 00                 1282 	 nop 
   0341 00                 1283 	 nop 
   0342 00                 1284 	 nop 
   0343 00                 1285 	 nop 
                           1286 ;	fx2pipe.c:191: switch(cfg_data[2]&0x03U)
   0344 EA                 1287 	mov	a,r2
   0345 33                 1288 	rlc	a
   0346 95 E0              1289 	subb	a,acc
   0348 53 02 03           1290 	anl	ar2,#0x03
   034B 7B 00              1291 	mov	r3,#0x00
   034D BA 00 05           1292 	cjne	r2,#0x00,00135$
   0350 BB 00 02           1293 	cjne	r3,#0x00,00135$
   0353 80 08              1294 	sjmp	00101$
   0355                    1295 00135$:
                           1296 ;	fx2pipe.c:193: case 0x00U:  OUTPKTEND = 0x82;  SYNCDELAY;  // Quad-buffered.
   0355 BA 03 33           1297 	cjne	r2,#0x03,00103$
   0358 BB 00 30           1298 	cjne	r3,#0x00,00103$
   035B 80 17              1299 	sjmp	00102$
   035D                    1300 00101$:
   035D 90 E6 49           1301 	mov	dptr,#_OUTPKTEND
   0360 74 82              1302 	mov	a,#0x82
   0362 F0                 1303 	movx	@dptr,a
   0363 00                 1304 	 nop 
   0364 00                 1305 	 nop 
   0365 00                 1306 	 nop 
   0366 00                 1307 	 nop 
   0367 00                 1308 	 nop 
   0368 00                 1309 	 nop 
   0369 00                 1310 	 nop 
   036A 00                 1311 	 nop 
   036B 00                 1312 	 nop 
   036C 00                 1313 	 nop 
   036D 00                 1314 	 nop 
   036E 00                 1315 	 nop 
   036F 00                 1316 	 nop 
   0370 00                 1317 	 nop 
   0371 00                 1318 	 nop 
   0372 00                 1319 	 nop 
   0373 00                 1320 	 nop 
                           1321 ;	fx2pipe.c:194: case 0x03U:  OUTPKTEND = 0x82;  SYNCDELAY;  // Triple-buffered.
   0374                    1322 00102$:
   0374 90 E6 49           1323 	mov	dptr,#_OUTPKTEND
   0377 74 82              1324 	mov	a,#0x82
   0379 F0                 1325 	movx	@dptr,a
   037A 00                 1326 	 nop 
   037B 00                 1327 	 nop 
   037C 00                 1328 	 nop 
   037D 00                 1329 	 nop 
   037E 00                 1330 	 nop 
   037F 00                 1331 	 nop 
   0380 00                 1332 	 nop 
   0381 00                 1333 	 nop 
   0382 00                 1334 	 nop 
   0383 00                 1335 	 nop 
   0384 00                 1336 	 nop 
   0385 00                 1337 	 nop 
   0386 00                 1338 	 nop 
   0387 00                 1339 	 nop 
   0388 00                 1340 	 nop 
   0389 00                 1341 	 nop 
   038A 00                 1342 	 nop 
                           1343 ;	fx2pipe.c:195: }
   038B                    1344 00103$:
                           1345 ;	fx2pipe.c:207: EP2FIFOCFG = cfg_data[3]; /*0x11;*/
   038B 90 10 06           1346 	mov	dptr,#0x1006
   038E E0                 1347 	movx	a,@dptr
   038F FA                 1348 	mov	r2,a
   0390 90 E6 18           1349 	mov	dptr,#_EP2FIFOCFG
   0393 F0                 1350 	movx	@dptr,a
                           1351 ;	fx2pipe.c:208: SYNCDELAY;
   0394 00                 1352 	 nop 
   0395 00                 1353 	 nop 
   0396 00                 1354 	 nop 
   0397 00                 1355 	 nop 
   0398 00                 1356 	 nop 
   0399 00                 1357 	 nop 
   039A 00                 1358 	 nop 
   039B 00                 1359 	 nop 
   039C 00                 1360 	 nop 
   039D 00                 1361 	 nop 
   039E 00                 1362 	 nop 
   039F 00                 1363 	 nop 
   03A0 00                 1364 	 nop 
   03A1 00                 1365 	 nop 
   03A2 00                 1366 	 nop 
   03A3 00                 1367 	 nop 
   03A4 00                 1368 	 nop 
   03A5 22                 1369 	ret
                           1370 ;------------------------------------------------------------
                           1371 ;Allocation info for local variables in function 'main'
                           1372 ;------------------------------------------------------------
                           1373 ;------------------------------------------------------------
                           1374 ;	fx2pipe.c:219: void main()
                           1375 ;	-----------------------------------------
                           1376 ;	 function main
                           1377 ;	-----------------------------------------
   03A6                    1378 _main:
                           1379 ;	fx2pipe.c:221: Initialize();
   03A6 12 00 64           1380 	lcall	_Initialize
   03A9                    1381 00102$:
   03A9 80 FE              1382 	sjmp	00102$
                           1383 	.area CSEG    (CODE)
                           1384 	.area CONST   (CODE)
                           1385 	.area XINIT   (CODE)
                           1386 	.area CABS    (ABS,CODE)
