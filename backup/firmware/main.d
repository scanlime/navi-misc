<><><><><><><><><><><><><><><><><>
starting	ralloc.c:pic14_assignRegisters
ebbs before optimizing:

----------------------------------------------------------------
Basic Block _entry : loop Depth = 0 noPath = 0 , lastinLoop = 0
depth 1st num 2 : bbnum = 0 1st iCode = 1 , last iCode = 8
visited 0 : hasFcall = 1

defines bitVector :bitvector Size = 19 bSize = 3
Bits on { (8) }

local defines bitVector :bitvector Size = 19 bSize = 3
Bits on { (6) (8) }

pointers Set bitvector :
in pointers Set bitvector :
inDefs Set bitvector :
outDefs Set bitvector :bitvector Size = 19 bSize = 3
Bits on { (6) (8) }

usesDefs Set bitvector :bitvector Size = 19 bSize = 3
Bits on { (6) }

----------------------------------------------------------------
main.c(94:1:0:0:0)	 _entry($5) :
main.c(94:2:1:0:0)		proc _main [k1 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{void function    }
main.c(95:3:2:0:0)	 nop main.c(95:4:3:0:0)	 nop main.c(95:5:4:0:0)	 nop main.c(95:6:5:0:0)	 nop main.c(97:7:6:0:0)		iTemp0 [k3 lr7:7 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{void} = call _initialize [k2 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{void function    }
main.c(99:8:8:0:0)		_Light0 [k4 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{bit bit {0,1}} := 0x1 {literal bit {0,1}}

----------------------------------------------------------------
Basic Block preHeaderLbl0 : loop Depth = 0 noPath = 0 , lastinLoop = 0
depth 1st num 3 : bbnum = 1 1st iCode = 9 , last iCode = 9
visited 0 : hasFcall = 0

defines bitVector :bitvector Size = 19 bSize = 3
Bits on { }

local defines bitVector :
pointers Set bitvector :
in pointers Set bitvector :
inDefs Set bitvector :bitvector Size = 19 bSize = 3
Bits on { (6) (8) }

outDefs Set bitvector :bitvector Size = 19 bSize = 3
Bits on { (6) (8) }

usesDefs Set bitvector :bitvector Size = 19 bSize = 3
Bits on { }

----------------------------------------------------------------
main.c(101:9:18:0:0)	 preHeaderLbl0($6) :

----------------------------------------------------------------
Basic Block _whilebody_0 : loop Depth = 1 noPath = 0 , lastinLoop = 0
depth 1st num 4 : bbnum = 2 1st iCode = 10 , last iCode = 13
visited 0 : hasFcall = 1

defines bitVector :bitvector Size = 19 bSize = 3
Bits on { }

local defines bitVector :bitvector Size = 19 bSize = 3
Bits on { (12) (13) }

pointers Set bitvector :
in pointers Set bitvector :
inDefs Set bitvector :bitvector Size = 19 bSize = 3
Bits on { (6) (7) (8) (12) (13) }

outDefs Set bitvector :bitvector Size = 19 bSize = 3
Bits on { (6) (7) (8) (12) (13) }

usesDefs Set bitvector :bitvector Size = 19 bSize = 3
Bits on { (12) (13) }

----------------------------------------------------------------
main.c(101:10:11:1:0)	 _whilebody_0($2) :
main.c(102:11:12:1:0)		iTemp2 [k7 lr11:11 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{void} = call _sampleAll [k6 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{void function    }
main.c(103:12:13:1:0)		iTemp3 [k9 lr12:12 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{void} = call _sendPacket [k8 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{void function    }
main.c(103:13:14:1:0)		 goto _whilebody_0($2)

----------------------------------------------------------------
Basic Block _return : loop Depth = 0 noPath = 1 , lastinLoop = 0
depth 1st num 0 : bbnum = 3 1st iCode = 14 , last iCode = 15
visited 0 : hasFcall = 0

defines bitVector :
local defines bitVector :
pointers Set bitvector :
in pointers Set bitvector :
inDefs Set bitvector :
outDefs Set bitvector :
usesDefs Set bitvector :
----------------------------------------------------------------
main.c(103:14:16:0:0)	 _return($4) :
main.c(103:15:17:0:0)		eproc _main [k1 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{void function    }
packRegisters
packRegsForAssign
  result:_Light0 [k4 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{bit bit {0,1}}
  left:
  right:0x1 {literal bit {0,1}}
allocDirReg symbol name _Light0
  637  storage class 13 
 642  integral
 646  specifier
_Light0 [k4 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{bit bit {0,1}}
  -- added _Light0 to hash, size = 1
  -- and it is at a fixed address 0x30
  2695 - result is not temp
  2705 - not packing - right is not temp
allocDirReg BAD, op is NULL
  result:_Light0 [k4 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{bit bit {0,1}}
  left:
  right:0x1 {literal bit {0,1}}
  left:_main [k1 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{void function    }
        Symbol type: void function    
  left:_initialize [k2 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{void function    }
        Symbol type: void function    
  result:iTemp0 [k3 lr7:7 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{void}
          Symbol type: void
  3594 - pointer reg req = 0
  result:_Light0 [k4 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{bit bit {0,1}}
          Symbol type: bit bit {0,1}
  3594 - pointer reg req = 0
packRegisters
packRegisters
  left:_sampleAll [k6 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{void function    }
        Symbol type: void function    
  result:iTemp2 [k7 lr11:11 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{void}
          Symbol type: void
  3594 - pointer reg req = 0
  left:_sendPacket [k8 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{void function    }
        Symbol type: void function    
  result:iTemp3 [k9 lr12:12 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{void}
          Symbol type: void
  3594 - pointer reg req = 0
packRegisters
  left:_main [k1 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{void function    }
        Symbol type: void function    
dir registers allocated so far:
  -- #0 reg = s0xFFF  key 0, rIdx = 4095, size 1
  -- #1 reg = s0x1000  key 0, rIdx = 4096, size 1
  -- #2 reg = s0x1001  key 0, rIdx = 4097, size 1
  -- #3 reg = s0x1002  key 0, rIdx = 4098, size 1
  -- #4 reg = s0x1003  key 0, rIdx = 4099, size 1
  -- #5 reg = s0x1004  key 0, rIdx = 4100, size 1
  -- #6 reg = s0x1005  key 0, rIdx = 4101, size 1
  -- #7 reg = s0x1006  key 0, rIdx = 4102, size 1
  -- #8 reg = r0x20  key 0, rIdx = 32, size 1
  -- #9 reg = r0x21  key 0, rIdx = 33, size 1
  -- #10 reg = r0x22  key 0, rIdx = 34, size 1
  -- #11 reg = r0x23  key 0, rIdx = 35, size 1
  -- #12 reg = r0x24  key 0, rIdx = 36, size 1
  -- #13 reg = r0x25  key 0, rIdx = 37, size 1
  -- #14 reg = r0x26  key 0, rIdx = 38, size 1
  -- #15 reg = r0x27  key 0, rIdx = 39, size 1
  -- #16 reg = r0x28  key 0, rIdx = 40, size 1
  -- #17 reg = r0x29  key 0, rIdx = 41, size 1
  -- #18 reg = r0x2A  key 0, rIdx = 42, size 1
  -- #19 reg = r0x2B  key 0, rIdx = 43, size 1
  -- #20 reg = r0x2C  key 0, rIdx = 44, size 1
  -- #21 reg = r0x2D  key 0, rIdx = 45, size 1
  -- #22 reg = r0x2E  key 0, rIdx = 46, size 1
  -- #23 reg = r0x2F  key 0, rIdx = 47, size 1
  -- #24 reg = r0x30  key 0, rIdx = 48, size 1
  -- #25 reg = r0x31  key 0, rIdx = 49, size 1
  -- #26 reg = r0x32  key 0, rIdx = 50, size 1
  -- #27 reg = r0x33  key 0, rIdx = 51, size 1
  -- #28 reg = r0x34  key 0, rIdx = 52, size 1
  -- #29 reg = r0x35  key 0, rIdx = 53, size 1
  -- #30 reg = r0x36  key 0, rIdx = 54, size 1
  -- #31 reg = r0x37  key 0, rIdx = 55, size 1
  -- #32 reg = r0x38  key 0, rIdx = 56, size 1
  -- #33 reg = r0x39  key 0, rIdx = 57, size 1
  -- #34 reg = r0x3A  key 0, rIdx = 58, size 1
  -- #35 reg = r0x3B  key 0, rIdx = 59, size 1
  -- #36 reg = r0x3C  key 0, rIdx = 60, size 1
  -- #37 reg = r0x3D  key 0, rIdx = 61, size 1
  -- #38 reg = r0x3E  key 0, rIdx = 62, size 1
  -- #39 reg = r0x3F  key 0, rIdx = 63, size 1
  -- #40 reg = r0x40  key 0, rIdx = 64, size 1
  -- #41 reg = r0x41  key 0, rIdx = 65, size 1
  -- #42 reg = r0x42  key 0, rIdx = 66, size 1
  -- #43 reg = r0x43  key 0, rIdx = 67, size 1
  -- #44 reg = r0x44  key 0, rIdx = 68, size 1
  -- #45 reg = r0x45  key 0, rIdx = 69, size 1
  -- #46 reg = r0x46  key 0, rIdx = 70, size 1
  -- #47 reg = r0x47  key 0, rIdx = 71, size 1
  -- #48 reg = r0x48  key 0, rIdx = 72, size 1
  -- #49 reg = r0x49  key 0, rIdx = 73, size 1
  -- #50 reg = r0x4A  key 0, rIdx = 74, size 1
  -- #51 reg = r0x4B  key 0, rIdx = 75, size 1
  -- #52 reg = r0x4C  key 0, rIdx = 76, size 1
  -- #53 reg = r0x4D  key 0, rIdx = 77, size 1
  -- #54 reg = r0x4E  key 0, rIdx = 78, size 1
  -- #55 reg = r0x4F  key 0, rIdx = 79, size 1
  -- #56 reg = r0x50  key 0, rIdx = 80, size 1
  -- #57 reg = r0x51  key 0, rIdx = 81, size 1
  -- #58 reg = r0x52  key 0, rIdx = 82, size 1
  -- #59 reg = _PCLATH  key 6, rIdx = 10, size 1
  -- #60 reg = SSAVE  key 32, rIdx = 32765, size 1
  -- #61 reg = WSAVE  key 36, rIdx = 32766, size 1
  -- #62 reg = INTCON  key 47, rIdx = 11, size 1
  -- #63 reg = _STATUS  key 48, rIdx = 3, size 1
  -- #64 reg = PCL  key 48, rIdx = 2, size 1
  -- #65 reg = KZ  key 49, rIdx = 32767, size 1
  -- #66 reg = INDF  key 56, rIdx = 0, size 1
  -- #67 reg = _Light0  key 56, rIdx = 0, size 1
  -- #68 reg = FSR  key 60, rIdx = 4, size 1
regTypeNum
  2421 - iTemp0
  2421 - iTemp2
  2421 - iTemp3
serialRegAssign
  op: LABEL
deassignLRs
  op: unkown op 9 	
deassignLRs
  op: INLINEASM
deassignLRs
  op: INLINEASM
deassignLRs
  op: INLINEASM
deassignLRs
  op: INLINEASM
deassignLRs
  op: CALL
deassignLRs
  op: =
deassignLRs
  op: LABEL
deassignLRs
  op: LABEL
deassignLRs
  op: CALL
deassignLRs
  op: CALL
deassignLRs
  op: GOTO
deassignLRs
  op: LABEL
deassignLRs
  op: ENDFUNCTION
deassignLRs
createRegMask
regsUsedIniCode
rUmaskForOp
rUmaskForOp
regsUsedIniCode
rUmaskForOp
rUmaskForOp
regsUsedIniCode
rUmaskForOp
rUmaskForOp
regsUsedIniCode
rUmaskForOp
rUmaskForOp
ebbs after optimizing:

----------------------------------------------------------------
Basic Block _entry : loop Depth = 0 noPath = 0 , lastinLoop = 0
depth 1st num 2 : bbnum = 0 1st iCode = 1 , last iCode = 8
visited 0 : hasFcall = 1

defines bitVector :bitvector Size = 19 bSize = 3
Bits on { (8) }

local defines bitVector :bitvector Size = 19 bSize = 3
Bits on { (6) (8) }

pointers Set bitvector :
in pointers Set bitvector :
inDefs Set bitvector :
outDefs Set bitvector :bitvector Size = 19 bSize = 3
Bits on { (6) (8) }

usesDefs Set bitvector :bitvector Size = 19 bSize = 3
Bits on { (6) }

----------------------------------------------------------------
main.c(94:1:0:0:0)	 _entry($5) :
main.c(94:2:1:0:0)		proc _main [k1 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{void function    }
main.c(95:3:2:0:0)	 nop main.c(95:4:3:0:0)	 nop main.c(95:5:4:0:0)	 nop main.c(95:6:5:0:0)	 nop main.c(97:7:6:0:0)		iTemp0 [k3 lr7:7 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{void} = call _initialize [k2 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{void function    }
main.c(99:8:8:0:0)		_Light0 [k4 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{bit bit {0,1}} := 0x1 {literal bit {0,1}}
main.c(101:10:11:1:0)	 _whilebody_0($2) :
main.c(102:11:12:1:0)		iTemp2 [k7 lr11:11 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{void} = call _sampleAll [k6 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{void function    }
main.c(103:12:13:1:0)		iTemp3 [k9 lr12:12 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{void} = call _sendPacket [k8 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{void function    }
main.c(103:13:14:1:0)		 goto _whilebody_0($2)
main.c(103:14:16:0:0)	 _return($4) :
main.c(103:15:17:0:0)		eproc _main [k1 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{void function    }

----------------------------------------------------------------
Basic Block preHeaderLbl0 : loop Depth = 0 noPath = 0 , lastinLoop = 0
depth 1st num 3 : bbnum = 1 1st iCode = 9 , last iCode = 9
visited 0 : hasFcall = 0

defines bitVector :bitvector Size = 19 bSize = 3
Bits on { }

local defines bitVector :
pointers Set bitvector :
in pointers Set bitvector :
inDefs Set bitvector :bitvector Size = 19 bSize = 3
Bits on { (6) (8) }

outDefs Set bitvector :bitvector Size = 19 bSize = 3
Bits on { (6) (8) }

usesDefs Set bitvector :bitvector Size = 19 bSize = 3
Bits on { }

----------------------------------------------------------------
main.c(101:9:18:0:0)	 preHeaderLbl0($6) :
main.c(101:10:11:1:0)	 _whilebody_0($2) :
main.c(102:11:12:1:0)		iTemp2 [k7 lr11:11 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{void} = call _sampleAll [k6 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{void function    }
main.c(103:12:13:1:0)		iTemp3 [k9 lr12:12 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{void} = call _sendPacket [k8 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{void function    }
main.c(103:13:14:1:0)		 goto _whilebody_0($2)
main.c(103:14:16:0:0)	 _return($4) :
main.c(103:15:17:0:0)		eproc _main [k1 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{void function    }

----------------------------------------------------------------
Basic Block _whilebody_0 : loop Depth = 1 noPath = 0 , lastinLoop = 0
depth 1st num 4 : bbnum = 2 1st iCode = 10 , last iCode = 13
visited 0 : hasFcall = 1

defines bitVector :bitvector Size = 19 bSize = 3
Bits on { }

local defines bitVector :bitvector Size = 19 bSize = 3
Bits on { (12) (13) }

pointers Set bitvector :
in pointers Set bitvector :
inDefs Set bitvector :bitvector Size = 19 bSize = 3
Bits on { (6) (7) (8) (12) (13) }

outDefs Set bitvector :bitvector Size = 19 bSize = 3
Bits on { (6) (7) (8) (12) (13) }

usesDefs Set bitvector :bitvector Size = 19 bSize = 3
Bits on { (12) (13) }

----------------------------------------------------------------
main.c(101:10:11:1:0)	 _whilebody_0($2) :
main.c(102:11:12:1:0)		iTemp2 [k7 lr11:11 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{void} = call _sampleAll [k6 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{void function    }
main.c(103:12:13:1:0)		iTemp3 [k9 lr12:12 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{void} = call _sendPacket [k8 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{void function    }
main.c(103:13:14:1:0)		 goto _whilebody_0($2)
main.c(103:14:16:0:0)	 _return($4) :
main.c(103:15:17:0:0)		eproc _main [k1 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{void function    }

----------------------------------------------------------------
Basic Block _return : loop Depth = 0 noPath = 1 , lastinLoop = 0
depth 1st num 0 : bbnum = 3 1st iCode = 14 , last iCode = 15
visited 0 : hasFcall = 0

defines bitVector :
local defines bitVector :
pointers Set bitvector :
in pointers Set bitvector :
inDefs Set bitvector :
outDefs Set bitvector :
usesDefs Set bitvector :
----------------------------------------------------------------
main.c(103:14:16:0:0)	 _return($4) :
main.c(103:15:17:0:0)		eproc _main [k1 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{void function    }
leaving
<><><><><><><><><><><><><><><><><>
<><><><><><><><><><><><><><><><><>
starting	ralloc.c:pic14_assignRegisters
ebbs before optimizing:

----------------------------------------------------------------
Basic Block _entry : loop Depth = 0 noPath = 0 , lastinLoop = 0
depth 1st num 1 : bbnum = 0 1st iCode = 1 , last iCode = 15
visited 0 : hasFcall = 0

defines bitVector :bitvector Size = 18 bSize = 3
Bits on { (2) (3) (4) (5) (6) (7) (8) (9) (10) (11) (12) (13) (15) }

local defines bitVector :bitvector Size = 18 bSize = 3
Bits on { (2) (3) (4) (5) (6) (7) (8) (9) (10) (11) (12) (13) (15) }

pointers Set bitvector :
in pointers Set bitvector :
inDefs Set bitvector :
outDefs Set bitvector :bitvector Size = 18 bSize = 3
Bits on { (2) (3) (4) (5) (6) (7) (8) (9) (10) (11) (12) (13) (15) }

usesDefs Set bitvector :bitvector Size = 18 bSize = 3
Bits on { }

----------------------------------------------------------------
main.c(109:1:0:0:0)	 _entry($2) :
main.c(109:2:1:0:0)		proc _initialize [k1 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{void function    }
main.c(111:3:2:0:0)		_TRISA [k2 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{sfr volatile unsigned char} := 0xff {literal unsigned char}
main.c(112:4:3:0:0)		_TRISB [k3 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{sfr volatile unsigned char} := 0x0 {literal unsigned char}
main.c(113:5:4:0:0)		_TRISC [k4 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{sfr volatile unsigned char} := 0x0 {literal unsigned char}
main.c(114:6:5:0:0)		_TRISD [k5 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{sfr volatile unsigned char} := 0x0 {literal unsigned char}
main.c(115:7:6:0:0)		_TRISE [k6 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{sfr volatile unsigned char} := 0x4 {literal unsigned char}
main.c(118:8:7:0:0)		_PORTA [k7 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{sfr volatile unsigned char} := 0x0 {literal unsigned char}
main.c(119:9:8:0:0)		_PORTB [k8 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{sfr volatile unsigned char} := 0x0 {literal unsigned char}
main.c(120:10:9:0:0)		_PORTC [k9 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{sfr volatile unsigned char} := 0x0 {literal unsigned char}
main.c(121:11:10:0:0)		_PORTD [k10 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{sfr volatile unsigned char} := 0x0 {literal unsigned char}
main.c(122:12:11:0:0)		_PORTE [k11 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{sfr volatile unsigned char} := 0x0 {literal unsigned char}
main.c(125:13:12:0:0)		_ADCON1 [k12 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{sfr volatile unsigned char} := 0x82 {literal unsigned char}
main.c(126:14:13:0:0)		_ADCON0 [k13 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{sfr volatile unsigned char} := 0x81 {literal unsigned char}
main.c(127:15:15:0:0)		_GIE [k14 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{bit bit {0,1}} := 0x1 {literal bit {0,1}}

----------------------------------------------------------------
Basic Block _return : loop Depth = 0 noPath = 0 , lastinLoop = 0
depth 1st num 2 : bbnum = 1 1st iCode = 16 , last iCode = 17
visited 0 : hasFcall = 0

defines bitVector :bitvector Size = 18 bSize = 3
Bits on { }

local defines bitVector :
pointers Set bitvector :
in pointers Set bitvector :
inDefs Set bitvector :bitvector Size = 18 bSize = 3
Bits on { (2) (3) (4) (5) (6) (7) (8) (9) (10) (11) (12) (13) (15) }

outDefs Set bitvector :bitvector Size = 18 bSize = 3
Bits on { (2) (3) (4) (5) (6) (7) (8) (9) (10) (11) (12) (13) (15) }

usesDefs Set bitvector :bitvector Size = 18 bSize = 3
Bits on { }

----------------------------------------------------------------
main.c(127:16:16:0:0)	 _return($1) :
main.c(127:17:17:0:0)		eproc _initialize [k1 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{void function    }
packRegisters
packRegsForAssign
  result:_TRISA [k2 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{sfr volatile unsigned char}
  left:
  right:0xff {literal unsigned char}
allocDirReg symbol name _TRISA
  637  storage class 3 
 642  integral
 646  specifier
_TRISA [k2 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{sfr volatile unsigned char}
  -- added _TRISA to hash, size = 1
  -- and it is at a fixed address 0x85
  2695 - result is not temp
  2705 - not packing - right is not temp
allocDirReg BAD, op is NULL
  result:_TRISA [k2 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{sfr volatile unsigned char}
  left:
  right:0xff {literal unsigned char}
packRegsForAssign
  result:_TRISB [k3 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{sfr volatile unsigned char}
  left:
  right:0x0 {literal unsigned char}
allocDirReg symbol name _TRISB
  637  storage class 3 
 642  integral
 646  specifier
_TRISB [k3 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{sfr volatile unsigned char}
  -- added _TRISB to hash, size = 1
  -- and it is at a fixed address 0x86
  2695 - result is not temp
  2705 - not packing - right is not temp
allocDirReg BAD, op is NULL
  result:_TRISB [k3 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{sfr volatile unsigned char}
  left:
  right:0x0 {literal unsigned char}
packRegsForAssign
  result:_TRISC [k4 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{sfr volatile unsigned char}
  left:
  right:0x0 {literal unsigned char}
allocDirReg symbol name _TRISC
  637  storage class 3 
 642  integral
 646  specifier
_TRISC [k4 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{sfr volatile unsigned char}
  -- added _TRISC to hash, size = 1
  -- and it is at a fixed address 0x87
  2695 - result is not temp
  2705 - not packing - right is not temp
allocDirReg BAD, op is NULL
  result:_TRISC [k4 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{sfr volatile unsigned char}
  left:
  right:0x0 {literal unsigned char}
packRegsForAssign
  result:_TRISD [k5 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{sfr volatile unsigned char}
  left:
  right:0x0 {literal unsigned char}
allocDirReg symbol name _TRISD
  637  storage class 3 
 642  integral
 646  specifier
_TRISD [k5 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{sfr volatile unsigned char}
  -- added _TRISD to hash, size = 1
  -- and it is at a fixed address 0x88
  2695 - result is not temp
  2705 - not packing - right is not temp
allocDirReg BAD, op is NULL
  result:_TRISD [k5 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{sfr volatile unsigned char}
  left:
  right:0x0 {literal unsigned char}
packRegsForAssign
  result:_TRISE [k6 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{sfr volatile unsigned char}
  left:
  right:0x4 {literal unsigned char}
allocDirReg symbol name _TRISE
  637  storage class 3 
 642  integral
 646  specifier
_TRISE [k6 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{sfr volatile unsigned char}
  -- added _TRISE to hash, size = 1
  -- and it is at a fixed address 0x89
  2695 - result is not temp
  2705 - not packing - right is not temp
allocDirReg BAD, op is NULL
  result:_TRISE [k6 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{sfr volatile unsigned char}
  left:
  right:0x4 {literal unsigned char}
packRegsForAssign
  result:_PORTA [k7 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{sfr volatile unsigned char}
  left:
  right:0x0 {literal unsigned char}
allocDirReg symbol name _PORTA
  637  storage class 3 
 642  integral
 646  specifier
_PORTA [k7 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{sfr volatile unsigned char}
  -- added _PORTA to hash, size = 1
  -- and it is at a fixed address 0x05
  2695 - result is not temp
  2705 - not packing - right is not temp
allocDirReg BAD, op is NULL
  result:_PORTA [k7 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{sfr volatile unsigned char}
  left:
  right:0x0 {literal unsigned char}
packRegsForAssign
  result:_PORTB [k8 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{sfr volatile unsigned char}
  left:
  right:0x0 {literal unsigned char}
allocDirReg symbol name _PORTB
  637  storage class 3 
 642  integral
 646  specifier
_PORTB [k8 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{sfr volatile unsigned char}
  -- added _PORTB to hash, size = 1
  -- and it is at a fixed address 0x06
  2695 - result is not temp
  2705 - not packing - right is not temp
allocDirReg BAD, op is NULL
  result:_PORTB [k8 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{sfr volatile unsigned char}
  left:
  right:0x0 {literal unsigned char}
packRegsForAssign
  result:_PORTC [k9 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{sfr volatile unsigned char}
  left:
  right:0x0 {literal unsigned char}
allocDirReg symbol name _PORTC
  637  storage class 3 
 642  integral
 646  specifier
_PORTC [k9 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{sfr volatile unsigned char}
  -- added _PORTC to hash, size = 1
  -- and it is at a fixed address 0x07
  2695 - result is not temp
  2705 - not packing - right is not temp
allocDirReg BAD, op is NULL
  result:_PORTC [k9 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{sfr volatile unsigned char}
  left:
  right:0x0 {literal unsigned char}
packRegsForAssign
  result:_PORTD [k10 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{sfr volatile unsigned char}
  left:
  right:0x0 {literal unsigned char}
allocDirReg symbol name _PORTD
  637  storage class 3 
 642  integral
 646  specifier
_PORTD [k10 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{sfr volatile unsigned char}
  -- added _PORTD to hash, size = 1
  -- and it is at a fixed address 0x08
  2695 - result is not temp
  2705 - not packing - right is not temp
allocDirReg BAD, op is NULL
  result:_PORTD [k10 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{sfr volatile unsigned char}
  left:
  right:0x0 {literal unsigned char}
packRegsForAssign
  result:_PORTE [k11 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{sfr volatile unsigned char}
  left:
  right:0x0 {literal unsigned char}
allocDirReg symbol name _PORTE
  637  storage class 3 
 642  integral
 646  specifier
_PORTE [k11 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{sfr volatile unsigned char}
  -- added _PORTE to hash, size = 1
  -- and it is at a fixed address 0x09
  2695 - result is not temp
  2705 - not packing - right is not temp
allocDirReg BAD, op is NULL
  result:_PORTE [k11 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{sfr volatile unsigned char}
  left:
  right:0x0 {literal unsigned char}
packRegsForAssign
  result:_ADCON1 [k12 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{sfr volatile unsigned char}
  left:
  right:0x82 {literal unsigned char}
allocDirReg symbol name _ADCON1
  637  storage class 3 
 642  integral
 646  specifier
_ADCON1 [k12 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{sfr volatile unsigned char}
  -- added _ADCON1 to hash, size = 1
  -- and it is at a fixed address 0x9f
  2695 - result is not temp
  2705 - not packing - right is not temp
allocDirReg BAD, op is NULL
  result:_ADCON1 [k12 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{sfr volatile unsigned char}
  left:
  right:0x82 {literal unsigned char}
packRegsForAssign
  result:_ADCON0 [k13 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{sfr volatile unsigned char}
  left:
  right:0x81 {literal unsigned char}
allocDirReg symbol name _ADCON0
  637  storage class 3 
 642  integral
 646  specifier
_ADCON0 [k13 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{sfr volatile unsigned char}
  -- added _ADCON0 to hash, size = 1
  -- and it is at a fixed address 0x1f
  2695 - result is not temp
  2705 - not packing - right is not temp
allocDirReg BAD, op is NULL
  result:_ADCON0 [k13 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{sfr volatile unsigned char}
  left:
  right:0x81 {literal unsigned char}
packRegsForAssign
  result:_GIE [k14 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{bit bit {0,1}}
  left:
  right:0x1 {literal bit {0,1}}
allocDirReg symbol name _GIE
  637  storage class 13 
 642  integral
 646  specifier
_GIE [k14 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{bit bit {0,1}}
  -- added _GIE to hash, size = 1
  -- and it is at a fixed address 0x5f
  2695 - result is not temp
  2705 - not packing - right is not temp
allocDirReg BAD, op is NULL
  result:_GIE [k14 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{bit bit {0,1}}
  left:
  right:0x1 {literal bit {0,1}}
  left:_initialize [k1 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{void function    }
        Symbol type: void function    
  result:_TRISA [k2 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{sfr volatile unsigned char}
          Symbol type: sfr volatile unsigned char
  3594 - pointer reg req = 0
  result:_TRISB [k3 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{sfr volatile unsigned char}
          Symbol type: sfr volatile unsigned char
  3594 - pointer reg req = 0
  result:_TRISC [k4 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{sfr volatile unsigned char}
          Symbol type: sfr volatile unsigned char
  3594 - pointer reg req = 0
  result:_TRISD [k5 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{sfr volatile unsigned char}
          Symbol type: sfr volatile unsigned char
  3594 - pointer reg req = 0
  result:_TRISE [k6 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{sfr volatile unsigned char}
          Symbol type: sfr volatile unsigned char
  3594 - pointer reg req = 0
  result:_PORTA [k7 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{sfr volatile unsigned char}
          Symbol type: sfr volatile unsigned char
  3594 - pointer reg req = 0
  result:_PORTB [k8 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{sfr volatile unsigned char}
          Symbol type: sfr volatile unsigned char
  3594 - pointer reg req = 0
  result:_PORTC [k9 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{sfr volatile unsigned char}
          Symbol type: sfr volatile unsigned char
  3594 - pointer reg req = 0
  result:_PORTD [k10 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{sfr volatile unsigned char}
          Symbol type: sfr volatile unsigned char
  3594 - pointer reg req = 0
  result:_PORTE [k11 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{sfr volatile unsigned char}
          Symbol type: sfr volatile unsigned char
  3594 - pointer reg req = 0
  result:_ADCON1 [k12 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{sfr volatile unsigned char}
          Symbol type: sfr volatile unsigned char
  3594 - pointer reg req = 0
  result:_ADCON0 [k13 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{sfr volatile unsigned char}
          Symbol type: sfr volatile unsigned char
  3594 - pointer reg req = 0
  result:_GIE [k14 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{bit bit {0,1}}
          Symbol type: bit bit {0,1}
  3594 - pointer reg req = 0
packRegisters
  left:_initialize [k1 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{void function    }
        Symbol type: void function    
dir registers allocated so far:
  -- #0 reg = s0xFFF  key 0, rIdx = 4095, size 1
  -- #1 reg = s0x1000  key 0, rIdx = 4096, size 1
  -- #2 reg = s0x1001  key 0, rIdx = 4097, size 1
  -- #3 reg = s0x1002  key 0, rIdx = 4098, size 1
  -- #4 reg = s0x1003  key 0, rIdx = 4099, size 1
  -- #5 reg = s0x1004  key 0, rIdx = 4100, size 1
  -- #6 reg = s0x1005  key 0, rIdx = 4101, size 1
  -- #7 reg = s0x1006  key 0, rIdx = 4102, size 1
  -- #8 reg = r0x20  key 0, rIdx = 32, size 1
  -- #9 reg = r0x21  key 0, rIdx = 33, size 1
  -- #10 reg = r0x22  key 0, rIdx = 34, size 1
  -- #11 reg = r0x23  key 0, rIdx = 35, size 1
  -- #12 reg = r0x24  key 0, rIdx = 36, size 1
  -- #13 reg = r0x25  key 0, rIdx = 37, size 1
  -- #14 reg = r0x26  key 0, rIdx = 38, size 1
  -- #15 reg = r0x27  key 0, rIdx = 39, size 1
  -- #16 reg = r0x28  key 0, rIdx = 40, size 1
  -- #17 reg = r0x29  key 0, rIdx = 41, size 1
  -- #18 reg = r0x2A  key 0, rIdx = 42, size 1
  -- #19 reg = r0x2B  key 0, rIdx = 43, size 1
  -- #20 reg = r0x2C  key 0, rIdx = 44, size 1
  -- #21 reg = r0x2D  key 0, rIdx = 45, size 1
  -- #22 reg = r0x2E  key 0, rIdx = 46, size 1
  -- #23 reg = r0x2F  key 0, rIdx = 47, size 1
  -- #24 reg = r0x30  key 0, rIdx = 48, size 1
  -- #25 reg = r0x31  key 0, rIdx = 49, size 1
  -- #26 reg = r0x32  key 0, rIdx = 50, size 1
  -- #27 reg = r0x33  key 0, rIdx = 51, size 1
  -- #28 reg = r0x34  key 0, rIdx = 52, size 1
  -- #29 reg = r0x35  key 0, rIdx = 53, size 1
  -- #30 reg = r0x36  key 0, rIdx = 54, size 1
  -- #31 reg = r0x37  key 0, rIdx = 55, size 1
  -- #32 reg = r0x38  key 0, rIdx = 56, size 1
  -- #33 reg = r0x39  key 0, rIdx = 57, size 1
  -- #34 reg = r0x3A  key 0, rIdx = 58, size 1
  -- #35 reg = r0x3B  key 0, rIdx = 59, size 1
  -- #36 reg = r0x3C  key 0, rIdx = 60, size 1
  -- #37 reg = r0x3D  key 0, rIdx = 61, size 1
  -- #38 reg = r0x3E  key 0, rIdx = 62, size 1
  -- #39 reg = r0x3F  key 0, rIdx = 63, size 1
  -- #40 reg = r0x40  key 0, rIdx = 64, size 1
  -- #41 reg = r0x41  key 0, rIdx = 65, size 1
  -- #42 reg = r0x42  key 0, rIdx = 66, size 1
  -- #43 reg = r0x43  key 0, rIdx = 67, size 1
  -- #44 reg = r0x44  key 0, rIdx = 68, size 1
  -- #45 reg = r0x45  key 0, rIdx = 69, size 1
  -- #46 reg = r0x46  key 0, rIdx = 70, size 1
  -- #47 reg = r0x47  key 0, rIdx = 71, size 1
  -- #48 reg = r0x48  key 0, rIdx = 72, size 1
  -- #49 reg = r0x49  key 0, rIdx = 73, size 1
  -- #50 reg = r0x4A  key 0, rIdx = 74, size 1
  -- #51 reg = r0x4B  key 0, rIdx = 75, size 1
  -- #52 reg = r0x4C  key 0, rIdx = 76, size 1
  -- #53 reg = r0x4D  key 0, rIdx = 77, size 1
  -- #54 reg = r0x4E  key 0, rIdx = 78, size 1
  -- #55 reg = r0x4F  key 0, rIdx = 79, size 1
  -- #56 reg = r0x50  key 0, rIdx = 80, size 1
  -- #57 reg = r0x51  key 0, rIdx = 81, size 1
  -- #58 reg = r0x52  key 0, rIdx = 82, size 1
  -- #59 reg = _PCLATH  key 6, rIdx = 10, size 1
  -- #60 reg = _TRISA  key 7, rIdx = 1, size 1
  -- #61 reg = _TRISB  key 8, rIdx = 2, size 1
  -- #62 reg = _TRISC  key 9, rIdx = 3, size 1
  -- #63 reg = _TRISD  key 10, rIdx = 4, size 1
  -- #64 reg = _PORTA  key 10, rIdx = 6, size 1
  -- #65 reg = _TRISE  key 11, rIdx = 5, size 1
  -- #66 reg = _PORTB  key 11, rIdx = 7, size 1
  -- #67 reg = _PORTC  key 12, rIdx = 8, size 1
  -- #68 reg = _GIE  key 12, rIdx = 13, size 1
  -- #69 reg = _PORTD  key 13, rIdx = 9, size 1
  -- #70 reg = _PORTE  key 14, rIdx = 10, size 1
  -- #71 reg = _ADCON0  key 27, rIdx = 12, size 1
  -- #72 reg = _ADCON1  key 28, rIdx = 11, size 1
  -- #73 reg = SSAVE  key 32, rIdx = 32765, size 1
  -- #74 reg = WSAVE  key 36, rIdx = 32766, size 1
  -- #75 reg = INTCON  key 47, rIdx = 11, size 1
  -- #76 reg = _STATUS  key 48, rIdx = 3, size 1
  -- #77 reg = PCL  key 48, rIdx = 2, size 1
  -- #78 reg = KZ  key 49, rIdx = 32767, size 1
  -- #79 reg = INDF  key 56, rIdx = 0, size 1
  -- #80 reg = _Light0  key 56, rIdx = 0, size 1
  -- #81 reg = FSR  key 60, rIdx = 4, size 1
regTypeNum
serialRegAssign
  op: LABEL
deassignLRs
  op: unkown op 9 	
deassignLRs
  op: =
deassignLRs
  op: =
deassignLRs
  op: =
deassignLRs
  op: =
deassignLRs
  op: =
deassignLRs
  op: =
deassignLRs
  op: =
deassignLRs
  op: =
deassignLRs
  op: =
deassignLRs
  op: =
deassignLRs
  op: =
deassignLRs
  op: =
deassignLRs
  op: =
deassignLRs
  op: LABEL
deassignLRs
  op: ENDFUNCTION
deassignLRs
createRegMask
regsUsedIniCode
rUmaskForOp
rUmaskForOp
regsUsedIniCode
rUmaskForOp
rUmaskForOp
regsUsedIniCode
rUmaskForOp
rUmaskForOp
regsUsedIniCode
rUmaskForOp
rUmaskForOp
regsUsedIniCode
rUmaskForOp
rUmaskForOp
regsUsedIniCode
rUmaskForOp
rUmaskForOp
regsUsedIniCode
rUmaskForOp
rUmaskForOp
regsUsedIniCode
rUmaskForOp
rUmaskForOp
regsUsedIniCode
rUmaskForOp
rUmaskForOp
regsUsedIniCode
rUmaskForOp
rUmaskForOp
regsUsedIniCode
rUmaskForOp
rUmaskForOp
regsUsedIniCode
rUmaskForOp
rUmaskForOp
regsUsedIniCode
rUmaskForOp
rUmaskForOp
ebbs after optimizing:

----------------------------------------------------------------
Basic Block _entry : loop Depth = 0 noPath = 0 , lastinLoop = 0
depth 1st num 1 : bbnum = 0 1st iCode = 1 , last iCode = 15
visited 0 : hasFcall = 0

defines bitVector :bitvector Size = 18 bSize = 3
Bits on { (2) (3) (4) (5) (6) (7) (8) (9) (10) (11) (12) (13) (15) }

local defines bitVector :bitvector Size = 18 bSize = 3
Bits on { (2) (3) (4) (5) (6) (7) (8) (9) (10) (11) (12) (13) (15) }

pointers Set bitvector :
in pointers Set bitvector :
inDefs Set bitvector :
outDefs Set bitvector :bitvector Size = 18 bSize = 3
Bits on { (2) (3) (4) (5) (6) (7) (8) (9) (10) (11) (12) (13) (15) }

usesDefs Set bitvector :bitvector Size = 18 bSize = 3
Bits on { }

----------------------------------------------------------------
main.c(109:1:0:0:0)	 _entry($2) :
main.c(109:2:1:0:0)		proc _initialize [k1 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{void function    }
main.c(111:3:2:0:0)		_TRISA [k2 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{sfr volatile unsigned char} := 0xff {literal unsigned char}
main.c(112:4:3:0:0)		_TRISB [k3 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{sfr volatile unsigned char} := 0x0 {literal unsigned char}
main.c(113:5:4:0:0)		_TRISC [k4 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{sfr volatile unsigned char} := 0x0 {literal unsigned char}
main.c(114:6:5:0:0)		_TRISD [k5 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{sfr volatile unsigned char} := 0x0 {literal unsigned char}
main.c(115:7:6:0:0)		_TRISE [k6 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{sfr volatile unsigned char} := 0x4 {literal unsigned char}
main.c(118:8:7:0:0)		_PORTA [k7 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{sfr volatile unsigned char} := 0x0 {literal unsigned char}
main.c(119:9:8:0:0)		_PORTB [k8 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{sfr volatile unsigned char} := 0x0 {literal unsigned char}
main.c(120:10:9:0:0)		_PORTC [k9 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{sfr volatile unsigned char} := 0x0 {literal unsigned char}
main.c(121:11:10:0:0)		_PORTD [k10 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{sfr volatile unsigned char} := 0x0 {literal unsigned char}
main.c(122:12:11:0:0)		_PORTE [k11 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{sfr volatile unsigned char} := 0x0 {literal unsigned char}
main.c(125:13:12:0:0)		_ADCON1 [k12 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{sfr volatile unsigned char} := 0x82 {literal unsigned char}
main.c(126:14:13:0:0)		_ADCON0 [k13 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{sfr volatile unsigned char} := 0x81 {literal unsigned char}
main.c(127:15:15:0:0)		_GIE [k14 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{bit bit {0,1}} := 0x1 {literal bit {0,1}}
main.c(127:16:16:0:0)	 _return($1) :
main.c(127:17:17:0:0)		eproc _initialize [k1 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{void function    }

----------------------------------------------------------------
Basic Block _return : loop Depth = 0 noPath = 0 , lastinLoop = 0
depth 1st num 2 : bbnum = 1 1st iCode = 16 , last iCode = 17
visited 0 : hasFcall = 0

defines bitVector :bitvector Size = 18 bSize = 3
Bits on { }

local defines bitVector :
pointers Set bitvector :
in pointers Set bitvector :
inDefs Set bitvector :bitvector Size = 18 bSize = 3
Bits on { (2) (3) (4) (5) (6) (7) (8) (9) (10) (11) (12) (13) (15) }

outDefs Set bitvector :bitvector Size = 18 bSize = 3
Bits on { (2) (3) (4) (5) (6) (7) (8) (9) (10) (11) (12) (13) (15) }

usesDefs Set bitvector :bitvector Size = 18 bSize = 3
Bits on { }

----------------------------------------------------------------
main.c(127:16:16:0:0)	 _return($1) :
main.c(127:17:17:0:0)		eproc _initialize [k1 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{void function    }
leaving
<><><><><><><><><><><><><><><><><>
<><><><><><><><><><><><><><><><><>
starting	ralloc.c:pic14_assignRegisters
ebbs before optimizing:

----------------------------------------------------------------
Basic Block _entry : loop Depth = 0 noPath = 0 , lastinLoop = 0
depth 1st num 1 : bbnum = 0 1st iCode = 1 , last iCode = 42
visited 0 : hasFcall = 1

defines bitVector :bitvector Size = 44 bSize = 6
Bits on { (4) (5) (6) (9) (10) (11) (14) (15) (16) (19) (20) (21) (24) (25) (26) (29) (30) (31) (34) (35) (36) (39) (40) (41) }

local defines bitVector :bitvector Size = 44 bSize = 6
Bits on { (4) (5) (6) (9) (10) (11) (14) (15) (16) (19) (20) (21) (24) (25) (26) (29) (30) (31) (34) (35) (36) (39) (40) (41) }

pointers Set bitvector :
in pointers Set bitvector :
inDefs Set bitvector :
outDefs Set bitvector :bitvector Size = 44 bSize = 6
Bits on { (4) (5) (6) (9) (10) (11) (14) (15) (16) (19) (20) (21) (24) (25) (26) (29) (30) (31) (34) (35) (36) (39) (40) (41) }

usesDefs Set bitvector :bitvector Size = 44 bSize = 6
Bits on { (4) (5) (9) (10) (14) (15) (19) (20) (24) (25) (29) (30) (34) (35) (39) (40) }

----------------------------------------------------------------
main.c(134:1:0:0:0)	 _entry($2) :
main.c(134:2:1:0:0)		proc _sampleAll [k1 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{void function    }
main.c(135:3:2:0:0)		send 0x10 {literal unsigned char}{argreg = 1}
main.c(135:4:3:0:0)		send 0x81 {literal unsigned char}{argreg = 1}
main.c(135:5:4:0:0)		iTemp0 [k4 lr5:6 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int} = call _integrateSample [k3 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int function    }
main.c(135:6:5:0:0)		iTemp1 [k5 lr6:7 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int}{ sir@ _reading0} := iTemp0 [k4 lr5:6 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int}
main.c(135:7:6:0:0)		_reading0 [k2 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{int} := iTemp1 [k5 lr6:7 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int}{ sir@ _reading0}
main.c(136:8:7:0:0)		send 0x20 {literal unsigned char}{argreg = 1}
main.c(136:9:8:0:0)		send 0x81 {literal unsigned char}{argreg = 1}
main.c(136:10:9:0:0)		iTemp2 [k7 lr10:11 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int} = call _integrateSample [k3 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int function    }
main.c(136:11:10:0:0)		iTemp3 [k8 lr11:12 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int}{ sir@ _reading1} := iTemp2 [k7 lr10:11 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int}
main.c(136:12:11:0:0)		_reading1 [k6 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{int} := iTemp3 [k8 lr11:12 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int}{ sir@ _reading1}
main.c(137:13:12:0:0)		send 0x40 {literal unsigned char}{argreg = 1}
main.c(137:14:13:0:0)		send 0x81 {literal unsigned char}{argreg = 1}
main.c(137:15:14:0:0)		iTemp4 [k10 lr15:16 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int} = call _integrateSample [k3 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int function    }
main.c(137:16:15:0:0)		iTemp5 [k11 lr16:17 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int}{ sir@ _reading2} := iTemp4 [k10 lr15:16 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int}
main.c(137:17:16:0:0)		_reading2 [k9 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{int} := iTemp5 [k11 lr16:17 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int}{ sir@ _reading2}
main.c(138:18:17:0:0)		send 0x80 {literal unsigned char}{argreg = 1}
main.c(138:19:18:0:0)		send 0x81 {literal unsigned char}{argreg = 1}
main.c(138:20:19:0:0)		iTemp6 [k13 lr20:21 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int} = call _integrateSample [k3 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int function    }
main.c(138:21:20:0:0)		iTemp7 [k14 lr21:22 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int}{ sir@ _reading3} := iTemp6 [k13 lr20:21 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int}
main.c(138:22:21:0:0)		_reading3 [k12 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{int} := iTemp7 [k14 lr21:22 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int}{ sir@ _reading3}
main.c(139:23:22:0:0)		send 0x10 {literal unsigned char}{argreg = 1}
main.c(139:24:23:0:0)		send 0x89 {literal unsigned char}{argreg = 1}
main.c(139:25:24:0:0)		iTemp8 [k16 lr25:26 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int} = call _integrateSample [k3 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int function    }
main.c(139:26:25:0:0)		iTemp9 [k17 lr26:27 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int}{ sir@ _reading4} := iTemp8 [k16 lr25:26 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int}
main.c(139:27:26:0:0)		_reading4 [k15 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{int} := iTemp9 [k17 lr26:27 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int}{ sir@ _reading4}
main.c(140:28:27:0:0)		send 0x20 {literal unsigned char}{argreg = 1}
main.c(140:29:28:0:0)		send 0x89 {literal unsigned char}{argreg = 1}
main.c(140:30:29:0:0)		iTemp10 [k19 lr30:31 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int} = call _integrateSample [k3 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int function    }
main.c(140:31:30:0:0)		iTemp11 [k20 lr31:32 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int}{ sir@ _reading5} := iTemp10 [k19 lr30:31 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int}
main.c(140:32:31:0:0)		_reading5 [k18 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{int} := iTemp11 [k20 lr31:32 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int}{ sir@ _reading5}
main.c(141:33:32:0:0)		send 0x40 {literal unsigned char}{argreg = 1}
main.c(141:34:33:0:0)		send 0x89 {literal unsigned char}{argreg = 1}
main.c(141:35:34:0:0)		iTemp12 [k22 lr35:36 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int} = call _integrateSample [k3 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int function    }
main.c(141:36:35:0:0)		iTemp13 [k23 lr36:37 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int}{ sir@ _reading6} := iTemp12 [k22 lr35:36 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int}
main.c(141:37:36:0:0)		_reading6 [k21 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{int} := iTemp13 [k23 lr36:37 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int}{ sir@ _reading6}
main.c(142:38:37:0:0)		send 0x80 {literal unsigned char}{argreg = 1}
main.c(142:39:38:0:0)		send 0x89 {literal unsigned char}{argreg = 1}
main.c(142:40:39:0:0)		iTemp14 [k25 lr40:41 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int} = call _integrateSample [k3 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int function    }
main.c(142:41:40:0:0)		iTemp15 [k26 lr41:42 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int}{ sir@ _reading7} := iTemp14 [k25 lr40:41 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int}
main.c(142:42:41:0:0)		_reading7 [k24 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{int} := iTemp15 [k26 lr41:42 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int}{ sir@ _reading7}

----------------------------------------------------------------
Basic Block _return : loop Depth = 0 noPath = 0 , lastinLoop = 0
depth 1st num 2 : bbnum = 1 1st iCode = 43 , last iCode = 44
visited 0 : hasFcall = 0

defines bitVector :bitvector Size = 44 bSize = 6
Bits on { }

local defines bitVector :
pointers Set bitvector :
in pointers Set bitvector :
inDefs Set bitvector :bitvector Size = 44 bSize = 6
Bits on { (4) (5) (6) (9) (10) (11) (14) (15) (16) (19) (20) (21) (24) (25) (26) (29) (30) (31) (34) (35) (36) (39) (40) (41) }

outDefs Set bitvector :bitvector Size = 44 bSize = 6
Bits on { (4) (5) (6) (9) (10) (11) (14) (15) (16) (19) (20) (21) (24) (25) (26) (29) (30) (31) (34) (35) (36) (39) (40) (41) }

usesDefs Set bitvector :bitvector Size = 44 bSize = 6
Bits on { }

----------------------------------------------------------------
main.c(142:43:42:0:0)	 _return($1) :
main.c(142:44:43:0:0)		eproc _sampleAll [k1 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{void function    }
packRegisters
packRegsForAssign
  result:iTemp1 [k5 lr6:7 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int}{ sir@ _reading0}
  left:
  right:iTemp0 [k4 lr5:6 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int}
  2760 - dic result key == ic right key -- pointer set=N
  packing. removing iTemp0
  replacing with iTemp0
  result:iTemp1 [k5 lr5:7 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int}{ sir@ _reading0}
  left:
  right:iTemp0 [k4 lr5:6 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int}
packRegsForAssign
  result:_reading0 [k2 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{int}
  left:
  right:iTemp1 [k5 lr5:7 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int}{ sir@ _reading0}
allocDirReg symbol name _reading0
  637  storage class 0 
 642  integral
 646  specifier
_reading0 [k2 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{int}
  -- added _reading0 to hash, size = 2
  2695 - result is not temp
  2760 - dic result key == ic right key -- pointer set=N
  packing. removing iTemp1
  replacing with iTemp1
  result:_reading0 [k2 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{int}
  left:
  right:iTemp1 [k5 lr5:7 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int}{ sir@ _reading0}
packRegsForAssign
  result:iTemp3 [k8 lr11:12 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int}{ sir@ _reading1}
  left:
  right:iTemp2 [k7 lr10:11 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int}
  2760 - dic result key == ic right key -- pointer set=N
  packing. removing iTemp2
  replacing with iTemp2
  result:iTemp3 [k8 lr10:12 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int}{ sir@ _reading1}
  left:
  right:iTemp2 [k7 lr10:11 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int}
packRegsForAssign
  result:_reading1 [k6 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{int}
  left:
  right:iTemp3 [k8 lr10:12 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int}{ sir@ _reading1}
allocDirReg symbol name _reading1
  637  storage class 0 
 642  integral
 646  specifier
_reading1 [k6 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{int}
  -- added _reading1 to hash, size = 2
  2695 - result is not temp
  2760 - dic result key == ic right key -- pointer set=N
  packing. removing iTemp3
  replacing with iTemp3
  result:_reading1 [k6 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{int}
  left:
  right:iTemp3 [k8 lr10:12 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int}{ sir@ _reading1}
packRegsForAssign
  result:iTemp5 [k11 lr16:17 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int}{ sir@ _reading2}
  left:
  right:iTemp4 [k10 lr15:16 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int}
  2760 - dic result key == ic right key -- pointer set=N
  packing. removing iTemp4
  replacing with iTemp4
  result:iTemp5 [k11 lr15:17 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int}{ sir@ _reading2}
  left:
  right:iTemp4 [k10 lr15:16 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int}
packRegsForAssign
  result:_reading2 [k9 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{int}
  left:
  right:iTemp5 [k11 lr15:17 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int}{ sir@ _reading2}
allocDirReg symbol name _reading2
  637  storage class 0 
 642  integral
 646  specifier
_reading2 [k9 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{int}
  -- added _reading2 to hash, size = 2
  2695 - result is not temp
  2760 - dic result key == ic right key -- pointer set=N
  packing. removing iTemp5
  replacing with iTemp5
  result:_reading2 [k9 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{int}
  left:
  right:iTemp5 [k11 lr15:17 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int}{ sir@ _reading2}
packRegsForAssign
  result:iTemp7 [k14 lr21:22 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int}{ sir@ _reading3}
  left:
  right:iTemp6 [k13 lr20:21 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int}
  2760 - dic result key == ic right key -- pointer set=N
  packing. removing iTemp6
  replacing with iTemp6
  result:iTemp7 [k14 lr20:22 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int}{ sir@ _reading3}
  left:
  right:iTemp6 [k13 lr20:21 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int}
packRegsForAssign
  result:_reading3 [k12 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{int}
  left:
  right:iTemp7 [k14 lr20:22 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int}{ sir@ _reading3}
allocDirReg symbol name _reading3
  637  storage class 0 
 642  integral
 646  specifier
_reading3 [k12 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{int}
  -- added _reading3 to hash, size = 2
  2695 - result is not temp
  2760 - dic result key == ic right key -- pointer set=N
  packing. removing iTemp7
  replacing with iTemp7
  result:_reading3 [k12 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{int}
  left:
  right:iTemp7 [k14 lr20:22 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int}{ sir@ _reading3}
packRegsForAssign
  result:iTemp9 [k17 lr26:27 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int}{ sir@ _reading4}
  left:
  right:iTemp8 [k16 lr25:26 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int}
  2760 - dic result key == ic right key -- pointer set=N
  packing. removing iTemp8
  replacing with iTemp8
  result:iTemp9 [k17 lr25:27 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int}{ sir@ _reading4}
  left:
  right:iTemp8 [k16 lr25:26 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int}
packRegsForAssign
  result:_reading4 [k15 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{int}
  left:
  right:iTemp9 [k17 lr25:27 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int}{ sir@ _reading4}
allocDirReg symbol name _reading4
  637  storage class 0 
 642  integral
 646  specifier
_reading4 [k15 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{int}
  -- added _reading4 to hash, size = 2
  2695 - result is not temp
  2760 - dic result key == ic right key -- pointer set=N
  packing. removing iTemp9
  replacing with iTemp9
  result:_reading4 [k15 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{int}
  left:
  right:iTemp9 [k17 lr25:27 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int}{ sir@ _reading4}
packRegsForAssign
  result:iTemp11 [k20 lr31:32 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int}{ sir@ _reading5}
  left:
  right:iTemp10 [k19 lr30:31 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int}
  2760 - dic result key == ic right key -- pointer set=N
  packing. removing iTemp10
  replacing with iTemp10
  result:iTemp11 [k20 lr30:32 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int}{ sir@ _reading5}
  left:
  right:iTemp10 [k19 lr30:31 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int}
packRegsForAssign
  result:_reading5 [k18 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{int}
  left:
  right:iTemp11 [k20 lr30:32 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int}{ sir@ _reading5}
allocDirReg symbol name _reading5
  637  storage class 0 
 642  integral
 646  specifier
_reading5 [k18 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{int}
  -- added _reading5 to hash, size = 2
  2695 - result is not temp
  2760 - dic result key == ic right key -- pointer set=N
  packing. removing iTemp11
  replacing with iTemp11
  result:_reading5 [k18 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{int}
  left:
  right:iTemp11 [k20 lr30:32 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int}{ sir@ _reading5}
packRegsForAssign
  result:iTemp13 [k23 lr36:37 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int}{ sir@ _reading6}
  left:
  right:iTemp12 [k22 lr35:36 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int}
  2760 - dic result key == ic right key -- pointer set=N
  packing. removing iTemp12
  replacing with iTemp12
  result:iTemp13 [k23 lr35:37 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int}{ sir@ _reading6}
  left:
  right:iTemp12 [k22 lr35:36 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int}
packRegsForAssign
  result:_reading6 [k21 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{int}
  left:
  right:iTemp13 [k23 lr35:37 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int}{ sir@ _reading6}
allocDirReg symbol name _reading6
  637  storage class 0 
 642  integral
 646  specifier
_reading6 [k21 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{int}
  -- added _reading6 to hash, size = 2
  2695 - result is not temp
  2760 - dic result key == ic right key -- pointer set=N
  packing. removing iTemp13
  replacing with iTemp13
  result:_reading6 [k21 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{int}
  left:
  right:iTemp13 [k23 lr35:37 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int}{ sir@ _reading6}
packRegsForAssign
  result:iTemp15 [k26 lr41:42 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int}{ sir@ _reading7}
  left:
  right:iTemp14 [k25 lr40:41 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int}
  2760 - dic result key == ic right key -- pointer set=N
  packing. removing iTemp14
  replacing with iTemp14
  result:iTemp15 [k26 lr40:42 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int}{ sir@ _reading7}
  left:
  right:iTemp14 [k25 lr40:41 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int}
packRegsForAssign
  result:_reading7 [k24 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{int}
  left:
  right:iTemp15 [k26 lr40:42 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int}{ sir@ _reading7}
allocDirReg symbol name _reading7
  637  storage class 0 
 642  integral
 646  specifier
_reading7 [k24 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{int}
  -- added _reading7 to hash, size = 2
  2695 - result is not temp
  2760 - dic result key == ic right key -- pointer set=N
  packing. removing iTemp15
  replacing with iTemp15
  result:_reading7 [k24 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{int}
  left:
  right:iTemp15 [k26 lr40:42 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int}{ sir@ _reading7}
  left:_sampleAll [k1 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{void function    }
        Symbol type: void function    
  3594 - pointer reg req = 0
  3594 - pointer reg req = 0
  left:_integrateSample [k3 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int function    }
        Symbol type: int function    
  result:_reading0 [k2 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{int}
          Symbol type: int
  3594 - pointer reg req = 0
  3594 - pointer reg req = 0
  3594 - pointer reg req = 0
  left:_integrateSample [k3 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int function    }
        Symbol type: int function    
  result:_reading1 [k6 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{int}
          Symbol type: int
  3594 - pointer reg req = 0
  3594 - pointer reg req = 0
  3594 - pointer reg req = 0
  left:_integrateSample [k3 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int function    }
        Symbol type: int function    
  result:_reading2 [k9 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{int}
          Symbol type: int
  3594 - pointer reg req = 0
  3594 - pointer reg req = 0
  3594 - pointer reg req = 0
  left:_integrateSample [k3 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int function    }
        Symbol type: int function    
  result:_reading3 [k12 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{int}
          Symbol type: int
  3594 - pointer reg req = 0
  3594 - pointer reg req = 0
  3594 - pointer reg req = 0
  left:_integrateSample [k3 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int function    }
        Symbol type: int function    
  result:_reading4 [k15 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{int}
          Symbol type: int
  3594 - pointer reg req = 0
  3594 - pointer reg req = 0
  3594 - pointer reg req = 0
  left:_integrateSample [k3 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int function    }
        Symbol type: int function    
  result:_reading5 [k18 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{int}
          Symbol type: int
  3594 - pointer reg req = 0
  3594 - pointer reg req = 0
  3594 - pointer reg req = 0
  left:_integrateSample [k3 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int function    }
        Symbol type: int function    
  result:_reading6 [k21 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{int}
          Symbol type: int
  3594 - pointer reg req = 0
  3594 - pointer reg req = 0
  3594 - pointer reg req = 0
  left:_integrateSample [k3 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int function    }
        Symbol type: int function    
  result:_reading7 [k24 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{int}
          Symbol type: int
  3594 - pointer reg req = 0
packRegisters
  left:_sampleAll [k1 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{void function    }
        Symbol type: void function    
dir registers allocated so far:
  -- #0 reg = s0xFFF  key 0, rIdx = 4095, size 1
  -- #1 reg = s0x1000  key 0, rIdx = 4096, size 1
  -- #2 reg = s0x1001  key 0, rIdx = 4097, size 1
  -- #3 reg = s0x1002  key 0, rIdx = 4098, size 1
  -- #4 reg = s0x1003  key 0, rIdx = 4099, size 1
  -- #5 reg = s0x1004  key 0, rIdx = 4100, size 1
  -- #6 reg = s0x1005  key 0, rIdx = 4101, size 1
  -- #7 reg = s0x1006  key 0, rIdx = 4102, size 1
  -- #8 reg = r0x20  key 0, rIdx = 32, size 1
  -- #9 reg = r0x21  key 0, rIdx = 33, size 1
  -- #10 reg = r0x22  key 0, rIdx = 34, size 1
  -- #11 reg = r0x23  key 0, rIdx = 35, size 1
  -- #12 reg = r0x24  key 0, rIdx = 36, size 1
  -- #13 reg = r0x25  key 0, rIdx = 37, size 1
  -- #14 reg = r0x26  key 0, rIdx = 38, size 1
  -- #15 reg = r0x27  key 0, rIdx = 39, size 1
  -- #16 reg = r0x28  key 0, rIdx = 40, size 1
  -- #17 reg = r0x29  key 0, rIdx = 41, size 1
  -- #18 reg = r0x2A  key 0, rIdx = 42, size 1
  -- #19 reg = r0x2B  key 0, rIdx = 43, size 1
  -- #20 reg = r0x2C  key 0, rIdx = 44, size 1
  -- #21 reg = r0x2D  key 0, rIdx = 45, size 1
  -- #22 reg = r0x2E  key 0, rIdx = 46, size 1
  -- #23 reg = r0x2F  key 0, rIdx = 47, size 1
  -- #24 reg = r0x30  key 0, rIdx = 48, size 1
  -- #25 reg = r0x31  key 0, rIdx = 49, size 1
  -- #26 reg = r0x32  key 0, rIdx = 50, size 1
  -- #27 reg = r0x33  key 0, rIdx = 51, size 1
  -- #28 reg = r0x34  key 0, rIdx = 52, size 1
  -- #29 reg = r0x35  key 0, rIdx = 53, size 1
  -- #30 reg = r0x36  key 0, rIdx = 54, size 1
  -- #31 reg = r0x37  key 0, rIdx = 55, size 1
  -- #32 reg = r0x38  key 0, rIdx = 56, size 1
  -- #33 reg = r0x39  key 0, rIdx = 57, size 1
  -- #34 reg = r0x3A  key 0, rIdx = 58, size 1
  -- #35 reg = r0x3B  key 0, rIdx = 59, size 1
  -- #36 reg = r0x3C  key 0, rIdx = 60, size 1
  -- #37 reg = r0x3D  key 0, rIdx = 61, size 1
  -- #38 reg = r0x3E  key 0, rIdx = 62, size 1
  -- #39 reg = r0x3F  key 0, rIdx = 63, size 1
  -- #40 reg = r0x40  key 0, rIdx = 64, size 1
  -- #41 reg = r0x41  key 0, rIdx = 65, size 1
  -- #42 reg = r0x42  key 0, rIdx = 66, size 1
  -- #43 reg = r0x43  key 0, rIdx = 67, size 1
  -- #44 reg = r0x44  key 0, rIdx = 68, size 1
  -- #45 reg = r0x45  key 0, rIdx = 69, size 1
  -- #46 reg = r0x46  key 0, rIdx = 70, size 1
  -- #47 reg = r0x47  key 0, rIdx = 71, size 1
  -- #48 reg = r0x48  key 0, rIdx = 72, size 1
  -- #49 reg = r0x49  key 0, rIdx = 73, size 1
  -- #50 reg = r0x4A  key 0, rIdx = 74, size 1
  -- #51 reg = r0x4B  key 0, rIdx = 75, size 1
  -- #52 reg = r0x4C  key 0, rIdx = 76, size 1
  -- #53 reg = r0x4D  key 0, rIdx = 77, size 1
  -- #54 reg = r0x4E  key 0, rIdx = 78, size 1
  -- #55 reg = r0x4F  key 0, rIdx = 79, size 1
  -- #56 reg = r0x50  key 0, rIdx = 80, size 1
  -- #57 reg = r0x51  key 0, rIdx = 81, size 1
  -- #58 reg = r0x52  key 0, rIdx = 82, size 1
  -- #59 reg = _PCLATH  key 6, rIdx = 10, size 1
  -- #60 reg = _TRISA  key 7, rIdx = 1, size 1
  -- #61 reg = _TRISB  key 8, rIdx = 2, size 1
  -- #62 reg = _TRISC  key 9, rIdx = 3, size 1
  -- #63 reg = _TRISD  key 10, rIdx = 4, size 1
  -- #64 reg = _PORTA  key 10, rIdx = 6, size 1
  -- #65 reg = _TRISE  key 11, rIdx = 5, size 1
  -- #66 reg = _PORTB  key 11, rIdx = 7, size 1
  -- #67 reg = _PORTC  key 12, rIdx = 8, size 1
  -- #68 reg = _GIE  key 12, rIdx = 13, size 1
  -- #69 reg = _PORTD  key 13, rIdx = 9, size 1
  -- #70 reg = _PORTE  key 14, rIdx = 10, size 1
  -- #71 reg = _ADCON0  key 27, rIdx = 12, size 1
  -- #72 reg = _ADCON1  key 28, rIdx = 11, size 1
  -- #73 reg = SSAVE  key 32, rIdx = 32765, size 1
  -- #74 reg = WSAVE  key 36, rIdx = 32766, size 1
  -- #75 reg = _reading0  key 44, rIdx = 14, size 2
  -- #76 reg = _reading1  key 45, rIdx = 15, size 2
  -- #77 reg = _reading2  key 46, rIdx = 16, size 2
  -- #78 reg = INTCON  key 47, rIdx = 11, size 1
  -- #79 reg = _reading3  key 47, rIdx = 17, size 2
  -- #80 reg = _STATUS  key 48, rIdx = 3, size 1
  -- #81 reg = PCL  key 48, rIdx = 2, size 1
  -- #82 reg = _reading4  key 48, rIdx = 18, size 2
  -- #83 reg = KZ  key 49, rIdx = 32767, size 1
  -- #84 reg = _reading5  key 49, rIdx = 19, size 2
  -- #85 reg = _reading6  key 50, rIdx = 20, size 2
  -- #86 reg = _reading7  key 51, rIdx = 21, size 2
  -- #87 reg = INDF  key 56, rIdx = 0, size 1
  -- #88 reg = _Light0  key 56, rIdx = 0, size 1
  -- #89 reg = FSR  key 60, rIdx = 4, size 1
regTypeNum
  2421 - iTemp0
  2431 - itemp register
  reg name iTemp0,  reg type REG_GPR
  2421 - iTemp1
  2431 - itemp register
  reg name iTemp1,  reg type REG_GPR
  2421 - iTemp2
  2431 - itemp register
  reg name iTemp2,  reg type REG_GPR
  2421 - iTemp3
  2431 - itemp register
  reg name iTemp3,  reg type REG_GPR
  2421 - iTemp4
  2431 - itemp register
  reg name iTemp4,  reg type REG_GPR
  2421 - iTemp5
  2431 - itemp register
  reg name iTemp5,  reg type REG_GPR
  2421 - iTemp6
  2431 - itemp register
  reg name iTemp6,  reg type REG_GPR
  2421 - iTemp7
  2431 - itemp register
  reg name iTemp7,  reg type REG_GPR
  2421 - iTemp8
  2431 - itemp register
  reg name iTemp8,  reg type REG_GPR
  2421 - iTemp9
  2431 - itemp register
  reg name iTemp9,  reg type REG_GPR
  2421 - iTemp10
  2431 - itemp register
  reg name iTemp10,  reg type REG_GPR
  2421 - iTemp11
  2431 - itemp register
  reg name iTemp11,  reg type REG_GPR
  2421 - iTemp12
  2431 - itemp register
  reg name iTemp12,  reg type REG_GPR
  2421 - iTemp13
  2431 - itemp register
  reg name iTemp13,  reg type REG_GPR
  2421 - iTemp14
  2431 - itemp register
  reg name iTemp14,  reg type REG_GPR
  2421 - iTemp15
  2431 - itemp register
  reg name iTemp15,  reg type REG_GPR
serialRegAssign
  op: LABEL
deassignLRs
  op: unkown op 9 	
deassignLRs
  op: SEND
deassignLRs
  op: SEND
deassignLRs
  op: CALL
deassignLRs
  op: SEND
deassignLRs
  op: SEND
deassignLRs
  op: CALL
deassignLRs
  op: SEND
deassignLRs
  op: SEND
deassignLRs
  op: CALL
deassignLRs
  op: SEND
deassignLRs
  op: SEND
deassignLRs
  op: CALL
deassignLRs
  op: SEND
deassignLRs
  op: SEND
deassignLRs
  op: CALL
deassignLRs
  op: SEND
deassignLRs
  op: SEND
deassignLRs
  op: CALL
deassignLRs
  op: SEND
deassignLRs
  op: SEND
deassignLRs
  op: CALL
deassignLRs
  op: SEND
deassignLRs
  op: SEND
deassignLRs
  op: CALL
deassignLRs
  op: LABEL
deassignLRs
  op: ENDFUNCTION
deassignLRs
createRegMask
regsUsedIniCode
rUmaskForOp
regsUsedIniCode
rUmaskForOp
regsUsedIniCode
rUmaskForOp
rUmaskForOp
regsUsedIniCode
rUmaskForOp
regsUsedIniCode
rUmaskForOp
regsUsedIniCode
rUmaskForOp
rUmaskForOp
regsUsedIniCode
rUmaskForOp
regsUsedIniCode
rUmaskForOp
regsUsedIniCode
rUmaskForOp
rUmaskForOp
regsUsedIniCode
rUmaskForOp
regsUsedIniCode
rUmaskForOp
regsUsedIniCode
rUmaskForOp
rUmaskForOp
regsUsedIniCode
rUmaskForOp
regsUsedIniCode
rUmaskForOp
regsUsedIniCode
rUmaskForOp
rUmaskForOp
regsUsedIniCode
rUmaskForOp
regsUsedIniCode
rUmaskForOp
regsUsedIniCode
rUmaskForOp
rUmaskForOp
regsUsedIniCode
rUmaskForOp
regsUsedIniCode
rUmaskForOp
regsUsedIniCode
rUmaskForOp
rUmaskForOp
regsUsedIniCode
rUmaskForOp
regsUsedIniCode
rUmaskForOp
regsUsedIniCode
rUmaskForOp
rUmaskForOp
ebbs after optimizing:

----------------------------------------------------------------
Basic Block _entry : loop Depth = 0 noPath = 0 , lastinLoop = 0
depth 1st num 1 : bbnum = 0 1st iCode = 1 , last iCode = 42
visited 0 : hasFcall = 1

defines bitVector :bitvector Size = 44 bSize = 6
Bits on { (4) (5) (6) (9) (10) (11) (14) (15) (16) (19) (20) (21) (24) (25) (26) (29) (30) (31) (34) (35) (36) (39) (40) (41) }

local defines bitVector :bitvector Size = 44 bSize = 6
Bits on { (4) (5) (6) (9) (10) (11) (14) (15) (16) (19) (20) (21) (24) (25) (26) (29) (30) (31) (34) (35) (36) (39) (40) (41) }

pointers Set bitvector :
in pointers Set bitvector :
inDefs Set bitvector :
outDefs Set bitvector :bitvector Size = 44 bSize = 6
Bits on { (4) (5) (6) (9) (10) (11) (14) (15) (16) (19) (20) (21) (24) (25) (26) (29) (30) (31) (34) (35) (36) (39) (40) (41) }

usesDefs Set bitvector :bitvector Size = 44 bSize = 6
Bits on { (4) (5) (9) (10) (14) (15) (19) (20) (24) (25) (29) (30) (34) (35) (39) (40) }

----------------------------------------------------------------
main.c(134:1:0:0:0)	 _entry($2) :
main.c(134:2:1:0:0)		proc _sampleAll [k1 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{void function    }
main.c(135:3:2:0:0)		send 0x10 {literal unsigned char}{argreg = 1}
main.c(135:4:3:0:0)		send 0x81 {literal unsigned char}{argreg = 1}
main.c(135:5:4:0:0)		_reading0 [k2 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{int} = call _integrateSample [k3 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int function    }
main.c(136:8:7:0:0)		send 0x20 {literal unsigned char}{argreg = 1}
main.c(136:9:8:0:0)		send 0x81 {literal unsigned char}{argreg = 1}
main.c(136:10:9:0:0)		_reading1 [k6 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{int} = call _integrateSample [k3 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int function    }
main.c(137:13:12:0:0)		send 0x40 {literal unsigned char}{argreg = 1}
main.c(137:14:13:0:0)		send 0x81 {literal unsigned char}{argreg = 1}
main.c(137:15:14:0:0)		_reading2 [k9 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{int} = call _integrateSample [k3 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int function    }
main.c(138:18:17:0:0)		send 0x80 {literal unsigned char}{argreg = 1}
main.c(138:19:18:0:0)		send 0x81 {literal unsigned char}{argreg = 1}
main.c(138:20:19:0:0)		_reading3 [k12 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{int} = call _integrateSample [k3 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int function    }
main.c(139:23:22:0:0)		send 0x10 {literal unsigned char}{argreg = 1}
main.c(139:24:23:0:0)		send 0x89 {literal unsigned char}{argreg = 1}
main.c(139:25:24:0:0)		_reading4 [k15 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{int} = call _integrateSample [k3 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int function    }
main.c(140:28:27:0:0)		send 0x20 {literal unsigned char}{argreg = 1}
main.c(140:29:28:0:0)		send 0x89 {literal unsigned char}{argreg = 1}
main.c(140:30:29:0:0)		_reading5 [k18 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{int} = call _integrateSample [k3 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int function    }
main.c(141:33:32:0:0)		send 0x40 {literal unsigned char}{argreg = 1}
main.c(141:34:33:0:0)		send 0x89 {literal unsigned char}{argreg = 1}
main.c(141:35:34:0:0)		_reading6 [k21 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{int} = call _integrateSample [k3 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int function    }
main.c(142:38:37:0:0)		send 0x80 {literal unsigned char}{argreg = 1}
main.c(142:39:38:0:0)		send 0x89 {literal unsigned char}{argreg = 1}
main.c(142:40:39:0:0)		_reading7 [k24 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{int} = call _integrateSample [k3 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int function    }
main.c(142:43:42:0:0)	 _return($1) :
main.c(142:44:43:0:0)		eproc _sampleAll [k1 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{void function    }

----------------------------------------------------------------
Basic Block _return : loop Depth = 0 noPath = 0 , lastinLoop = 0
depth 1st num 2 : bbnum = 1 1st iCode = 43 , last iCode = 44
visited 0 : hasFcall = 0

defines bitVector :bitvector Size = 44 bSize = 6
Bits on { }

local defines bitVector :
pointers Set bitvector :
in pointers Set bitvector :
inDefs Set bitvector :bitvector Size = 44 bSize = 6
Bits on { (4) (5) (6) (9) (10) (11) (14) (15) (16) (19) (20) (21) (24) (25) (26) (29) (30) (31) (34) (35) (36) (39) (40) (41) }

outDefs Set bitvector :bitvector Size = 44 bSize = 6
Bits on { (4) (5) (6) (9) (10) (11) (14) (15) (16) (19) (20) (21) (24) (25) (26) (29) (30) (31) (34) (35) (36) (39) (40) (41) }

usesDefs Set bitvector :bitvector Size = 44 bSize = 6
Bits on { }

----------------------------------------------------------------
main.c(142:43:42:0:0)	 _return($1) :
main.c(142:44:43:0:0)		eproc _sampleAll [k1 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{void function    }
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
Found a Stack Register!
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
Found a Stack Register!
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
Found a Stack Register!
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
Found a Stack Register!
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
Found a Stack Register!
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
Found a Stack Register!
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
Found a Stack Register!
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
Found a Stack Register!
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
Found a Stack Register!
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
Found a Stack Register!
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
Found a Stack Register!
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
Found a Stack Register!
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
Found a Stack Register!
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
Found a Stack Register!
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
Found a Stack Register!
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
Found a Stack Register!
leaving
<><><><><><><><><><><><><><><><><>
<><><><><><><><><><><><><><><><><>
starting	ralloc.c:pic14_assignRegisters
ebbs before optimizing:

----------------------------------------------------------------
Basic Block _entry : loop Depth = 0 noPath = 0 , lastinLoop = 0
depth 1st num 1 : bbnum = 0 1st iCode = 1 , last iCode = 6
visited 0 : hasFcall = 0

defines bitVector :bitvector Size = 58 bSize = 8
Bits on { (2) (3) (4) (5) }

local defines bitVector :bitvector Size = 58 bSize = 8
Bits on { (2) (3) (4) (5) (6) }

pointers Set bitvector :
in pointers Set bitvector :
inDefs Set bitvector :
outDefs Set bitvector :bitvector Size = 58 bSize = 8
Bits on { (2) (3) (4) (5) }

usesDefs Set bitvector :bitvector Size = 58 bSize = 8
Bits on { (2) (3) }

----------------------------------------------------------------
main.c(146:1:0:0:0)	 _entry($9) :
main.c(146:2:1:0:0)		proc _integrateSample [k1 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int function    }
main.c(146:3:2:0:0)	iTemp0 [k2 lr3:35 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _integrateSample_transmitter_1_1} = recv 
main.c(146:4:3:0:0)	iTemp1 [k4 lr4:35 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _integrateSample_receiver_1_1} = recv 
main.c(147:5:4:0:0)		iTemp2 [k6 lr5:46 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{int}{ sir@ _integrateSample_i_1_1} := 0x0 {literal int}
main.c(147:6:5:0:0)		iTemp3 [k8 lr6:46 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{int}{ sir@ _integrateSample_q_1_1} := 0x0 {literal int}

----------------------------------------------------------------
Basic Block preHeaderLbl0 : loop Depth = 0 noPath = 0 , lastinLoop = 0
depth 1st num 2 : bbnum = 1 1st iCode = 7 , last iCode = 8
visited 0 : hasFcall = 0

defines bitVector :bitvector Size = 58 bSize = 8
Bits on { (57) }

local defines bitVector :bitvector Size = 58 bSize = 8
Bits on { (57) }

pointers Set bitvector :
in pointers Set bitvector :
inDefs Set bitvector :bitvector Size = 58 bSize = 8
Bits on { (2) (3) (4) (5) (6) }

outDefs Set bitvector :bitvector Size = 58 bSize = 8
Bits on { (2) (3) (4) (5) (6) (57) }

usesDefs Set bitvector :bitvector Size = 58 bSize = 8
Bits on { }

----------------------------------------------------------------
main.c(150:7:56:0:0)	 preHeaderLbl0($10) :
main.c(150:8:57:0:0)		iTemp13 [k21 lr8:35 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int}{ sir@ _integrateSample_numsamples_1_1} := 0x8 {literal int}

----------------------------------------------------------------
Basic Block _dobody_0 : loop Depth = 1 noPath = 0 , lastinLoop = 1
depth 1st num 3 : bbnum = 2 1st iCode = 9 , last iCode = 35
visited 0 : hasFcall = 1

defines bitVector :bitvector Size = 58 bSize = 8
Bits on { (11) (12) (17) (18) (23) (24) (25) (29) (30) (31) (33) }

local defines bitVector :bitvector Size = 58 bSize = 8
Bits on { (11) (12) (13) (17) (18) (19) (23) (24) (25) (29) (30) (31) (33) (34) }

pointers Set bitvector :
in pointers Set bitvector :
inDefs Set bitvector :bitvector Size = 58 bSize = 8
Bits on { (2) (3) (4) (5) (6) (11) (12) (17) (18) (23) (24) (25) (29) (30) (31) (33) (34) (57) }

outDefs Set bitvector :bitvector Size = 58 bSize = 8
Bits on { (2) (3) (11) (12) (17) (18) (23) (24) (25) (29) (30) (31) (33) }

usesDefs Set bitvector :bitvector Size = 58 bSize = 8
Bits on { (2) (3) (4) (5) (11) (12) (13) (17) (18) (19) (23) (24) (25) (29) (30) (31) (33) (57) }

----------------------------------------------------------------
main.c(150:9:7:1:0)	 _dobody_0($1) :
main.c(152:10:8:1:0)		send iTemp0 [k2 lr3:35 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _integrateSample_transmitter_1_1}{argreg = 1}
main.c(152:11:9:1:0)		send iTemp1 [k4 lr4:35 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _integrateSample_receiver_1_1}{argreg = 1}
main.c(152:12:10:1:0)		send 0x0 {literal unsigned char}{argreg = 1}
main.c(152:13:11:1:0)		iTemp5 [k13 lr13:14 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int} = call _sensorSample [k12 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int function    }
main.c(152:14:12:1:0)		iTemp6 [k14 lr14:15 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int} = iTemp2 [k6 lr5:46 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{int}{ sir@ _integrateSample_i_1_1} + iTemp5 [k13 lr13:14 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int}
main.c(152:15:13:1:0)		iTemp2 [k6 lr5:46 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{int}{ sir@ _integrateSample_i_1_1} := iTemp6 [k14 lr14:15 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int}
main.c(153:16:14:1:0)		send iTemp0 [k2 lr3:35 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _integrateSample_transmitter_1_1}{argreg = 1}
main.c(153:17:15:1:0)		send iTemp1 [k4 lr4:35 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _integrateSample_receiver_1_1}{argreg = 1}
main.c(153:18:16:1:0)		send 0x1 {literal unsigned char}{argreg = 1}
main.c(153:19:17:1:0)		iTemp7 [k15 lr19:20 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int} = call _sensorSample [k12 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int function    }
main.c(153:20:18:1:0)		iTemp8 [k16 lr20:21 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int} = iTemp3 [k8 lr6:46 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{int}{ sir@ _integrateSample_q_1_1} + iTemp7 [k15 lr19:20 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int}
main.c(153:21:19:1:0)		iTemp3 [k8 lr6:46 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{int}{ sir@ _integrateSample_q_1_1} := iTemp8 [k16 lr20:21 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int}
main.c(154:22:20:1:0)		send iTemp0 [k2 lr3:35 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _integrateSample_transmitter_1_1}{argreg = 1}
main.c(154:23:21:1:0)		send iTemp1 [k4 lr4:35 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _integrateSample_receiver_1_1}{argreg = 1}
main.c(154:24:22:1:0)		send 0x2 {literal unsigned char}{argreg = 1}
main.c(154:25:23:1:0)		iTemp9 [k17 lr25:26 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int} = call _sensorSample [k12 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int function    }
main.c(154:26:24:1:0)		iTemp10 [k18 lr26:27 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int} = iTemp2 [k6 lr5:46 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{int}{ sir@ _integrateSample_i_1_1} - iTemp9 [k17 lr25:26 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int}
main.c(154:27:25:1:0)		iTemp2 [k6 lr5:46 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{int}{ sir@ _integrateSample_i_1_1} := iTemp10 [k18 lr26:27 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int}
main.c(155:28:26:1:0)		send iTemp0 [k2 lr3:35 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _integrateSample_transmitter_1_1}{argreg = 1}
main.c(155:29:27:1:0)		send iTemp1 [k4 lr4:35 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _integrateSample_receiver_1_1}{argreg = 1}
main.c(155:30:28:1:0)		send 0x3 {literal unsigned char}{argreg = 1}
main.c(155:31:29:1:0)		iTemp11 [k19 lr31:32 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int} = call _sensorSample [k12 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int function    }
main.c(155:32:30:1:0)		iTemp12 [k20 lr32:33 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int} = iTemp3 [k8 lr6:46 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{int}{ sir@ _integrateSample_q_1_1} - iTemp11 [k19 lr31:32 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int}
main.c(155:33:31:1:0)		iTemp3 [k8 lr6:46 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{int}{ sir@ _integrateSample_q_1_1} := iTemp12 [k20 lr32:33 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int}
main.c(156:34:33:1:0)		iTemp13 [k21 lr8:35 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int}{ sir@ _integrateSample_numsamples_1_1} = iTemp13 [k21 lr8:35 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int}{ sir@ _integrateSample_numsamples_1_1} - 0x1 {literal unsigned char}
main.c(156:35:35:1:0)		if iTemp13 [k21 lr8:35 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int}{ sir@ _integrateSample_numsamples_1_1} != 0 goto _dobody_0($1)

----------------------------------------------------------------
Basic Block _eBBlock0 : loop Depth = 0 noPath = 0 , lastinLoop = 0
depth 1st num 4 : bbnum = 3 1st iCode = 36 , last iCode = 37
visited 0 : hasFcall = 0

defines bitVector :bitvector Size = 58 bSize = 8
Bits on { (38) }

local defines bitVector :bitvector Size = 58 bSize = 8
Bits on { (38) }

pointers Set bitvector :
in pointers Set bitvector :
inDefs Set bitvector :bitvector Size = 58 bSize = 8
Bits on { (2) (3) (11) (12) (17) (18) (23) (24) (25) (29) (30) (31) (33) (34) }

outDefs Set bitvector :bitvector Size = 58 bSize = 8
Bits on { (2) (3) (11) (12) (17) (18) (23) (24) (25) (29) (30) (31) (33) (34) (38) }

usesDefs Set bitvector :bitvector Size = 58 bSize = 8
Bits on { (25) (38) }

----------------------------------------------------------------
main.c(159:36:38:0:0)		iTemp14 [k22 lr36:37 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{char} = iTemp2 [k6 lr5:46 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{int}{ sir@ _integrateSample_i_1_1} < 0x0 {literal int}
main.c(159:37:39:0:0)		if iTemp14 [k22 lr36:37 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{char} == 0 goto _iffalse_0($5)

----------------------------------------------------------------
Basic Block _eBBlock1 : loop Depth = 0 noPath = 0 , lastinLoop = 0
depth 1st num 5 : bbnum = 4 1st iCode = 38 , last iCode = 39
visited 0 : hasFcall = 0

defines bitVector :bitvector Size = 58 bSize = 8
Bits on { (42) (43) }

local defines bitVector :bitvector Size = 58 bSize = 8
Bits on { (42) (43) }

pointers Set bitvector :
in pointers Set bitvector :
inDefs Set bitvector :bitvector Size = 58 bSize = 8
Bits on { (2) (3) (11) (12) (17) (18) (23) (24) (25) (29) (30) (31) (33) (34) (38) }

outDefs Set bitvector :bitvector Size = 58 bSize = 8
Bits on { (2) (3) (11) (12) (17) (18) (23) (24) (29) (30) (31) (33) (34) (38) (42) (43) }

usesDefs Set bitvector :bitvector Size = 58 bSize = 8
Bits on { (25) (42) }

----------------------------------------------------------------
main.c(159:38:42:0:0)		iTemp15 [k23 lr38:39 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int} = - iTemp2 [k6 lr5:46 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{int}{ sir@ _integrateSample_i_1_1}
main.c(159:39:43:0:0)		iTemp2 [k6 lr5:46 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{int}{ sir@ _integrateSample_i_1_1} := iTemp15 [k23 lr38:39 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int}

----------------------------------------------------------------
Basic Block _iffalse_0 : loop Depth = 0 noPath = 0 , lastinLoop = 0
depth 1st num 6 : bbnum = 5 1st iCode = 40 , last iCode = 42
visited 0 : hasFcall = 0

defines bitVector :bitvector Size = 58 bSize = 8
Bits on { (45) }

local defines bitVector :bitvector Size = 58 bSize = 8
Bits on { (45) }

pointers Set bitvector :
in pointers Set bitvector :
inDefs Set bitvector :bitvector Size = 58 bSize = 8
Bits on { (2) (3) (11) (12) (17) (18) (23) (24) (25) (29) (30) (31) (33) (34) (38) (42) (43) }

outDefs Set bitvector :bitvector Size = 58 bSize = 8
Bits on { (2) (3) (11) (12) (17) (18) (23) (24) (25) (29) (30) (31) (33) (34) (38) (42) (43) (45) }

usesDefs Set bitvector :bitvector Size = 58 bSize = 8
Bits on { (31) (45) }

----------------------------------------------------------------
main.c(159:40:44:0:0)	 _iffalse_0($5) :
main.c(160:41:45:0:0)		iTemp16 [k24 lr41:42 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{char} = iTemp3 [k8 lr6:46 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{int}{ sir@ _integrateSample_q_1_1} < 0x0 {literal int}
main.c(160:42:46:0:0)		if iTemp16 [k24 lr41:42 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{char} == 0 goto _iffalse_1($7)

----------------------------------------------------------------
Basic Block _eBBlock2 : loop Depth = 0 noPath = 0 , lastinLoop = 0
depth 1st num 7 : bbnum = 6 1st iCode = 43 , last iCode = 44
visited 0 : hasFcall = 0

defines bitVector :bitvector Size = 58 bSize = 8
Bits on { (49) (50) }

local defines bitVector :bitvector Size = 58 bSize = 8
Bits on { (49) (50) }

pointers Set bitvector :
in pointers Set bitvector :
inDefs Set bitvector :bitvector Size = 58 bSize = 8
Bits on { (2) (3) (11) (12) (17) (18) (23) (24) (25) (29) (30) (31) (33) (34) (38) (42) (43) (45) }

outDefs Set bitvector :bitvector Size = 58 bSize = 8
Bits on { (2) (3) (11) (12) (17) (18) (23) (24) (25) (29) (30) (33) (34) (38) (42) (43) (45) (49) (50) }

usesDefs Set bitvector :bitvector Size = 58 bSize = 8
Bits on { (31) (49) }

----------------------------------------------------------------
main.c(160:43:49:0:0)		iTemp17 [k25 lr43:44 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int} = - iTemp3 [k8 lr6:46 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{int}{ sir@ _integrateSample_q_1_1}
main.c(160:44:50:0:0)		iTemp3 [k8 lr6:46 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{int}{ sir@ _integrateSample_q_1_1} := iTemp17 [k25 lr43:44 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int}

----------------------------------------------------------------
Basic Block _iffalse_1 : loop Depth = 0 noPath = 0 , lastinLoop = 0
depth 1st num 8 : bbnum = 7 1st iCode = 45 , last iCode = 47
visited 0 : hasFcall = 0

defines bitVector :bitvector Size = 58 bSize = 8
Bits on { (52) }

local defines bitVector :bitvector Size = 58 bSize = 8
Bits on { (52) }

pointers Set bitvector :
in pointers Set bitvector :
inDefs Set bitvector :bitvector Size = 58 bSize = 8
Bits on { (2) (3) (11) (12) (17) (18) (23) (24) (25) (29) (30) (31) (33) (34) (38) (42) (43) (45) (49) (50) }

outDefs Set bitvector :bitvector Size = 58 bSize = 8
Bits on { (2) (3) (11) (12) (17) (18) (23) (24) (25) (29) (30) (31) (33) (34) (38) (42) (43) (45) (49) (50) (52) }

usesDefs Set bitvector :bitvector Size = 58 bSize = 8
Bits on { (25) (31) (43) (50) (52) }

----------------------------------------------------------------
main.c(160:45:51:0:0)	 _iffalse_1($7) :
main.c(161:46:52:0:0)		iTemp18 [k26 lr46:47 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int} = iTemp2 [k6 lr5:46 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{int}{ sir@ _integrateSample_i_1_1} + iTemp3 [k8 lr6:46 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{int}{ sir@ _integrateSample_q_1_1}
main.c(161:47:53:0:0)		ret iTemp18 [k26 lr46:47 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int}

----------------------------------------------------------------
Basic Block _return : loop Depth = 0 noPath = 0 , lastinLoop = 0
depth 1st num 9 : bbnum = 8 1st iCode = 48 , last iCode = 49
visited 0 : hasFcall = 0

defines bitVector :bitvector Size = 58 bSize = 8
Bits on { }

local defines bitVector :
pointers Set bitvector :
in pointers Set bitvector :
inDefs Set bitvector :bitvector Size = 58 bSize = 8
Bits on { (2) (3) (11) (12) (17) (18) (23) (24) (25) (29) (30) (31) (33) (34) (38) (42) (43) (45) (49) (50) (52) }

outDefs Set bitvector :bitvector Size = 58 bSize = 8
Bits on { (2) (3) (11) (12) (17) (18) (23) (24) (25) (29) (30) (31) (33) (34) (38) (42) (43) (45) (49) (50) (52) }

usesDefs Set bitvector :bitvector Size = 58 bSize = 8
Bits on { }

----------------------------------------------------------------
main.c(161:48:54:0:0)	 _return($8) :
main.c(161:49:55:0:0)		eproc _integrateSample [k1 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int function    }
packRegisters
packRegsForAssign
  result:iTemp2 [k6 lr5:46 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{int}{ sir@ _integrateSample_i_1_1}
  left:
  right:0x0 {literal int}
  2705 - not packing - right is not temp
allocDirReg BAD, op is NULL
  result:iTemp2 [k6 lr5:46 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{int}{ sir@ _integrateSample_i_1_1}
  left:
  right:0x0 {literal int}
packRegsForAssign
  result:iTemp3 [k8 lr6:46 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{int}{ sir@ _integrateSample_q_1_1}
  left:
  right:0x0 {literal int}
  2705 - not packing - right is not temp
allocDirReg BAD, op is NULL
  result:iTemp3 [k8 lr6:46 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{int}{ sir@ _integrateSample_q_1_1}
  left:
  right:0x0 {literal int}
  left:_integrateSample [k1 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int function    }
        Symbol type: int function    
  result:iTemp0 [k2 lr3:35 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _integrateSample_transmitter_1_1}
          Symbol type: unsigned char
  3594 - pointer reg req = 0
packForReceive
  result:iTemp0 [k2 lr3:35 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _integrateSample_transmitter_1_1}
  left:
  right:
  hey we can remove this unnecessary assign
  result:iTemp1 [k4 lr4:35 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _integrateSample_receiver_1_1}
          Symbol type: unsigned char
  3594 - pointer reg req = 0
packForReceive
  result:iTemp1 [k4 lr4:35 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _integrateSample_receiver_1_1}
  left:
  right:
  hey we can remove this unnecessary assign
  result:iTemp2 [k6 lr5:46 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{int}{ sir@ _integrateSample_i_1_1}
          Symbol type: int
  3594 - pointer reg req = 0
  result:iTemp3 [k8 lr6:46 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{int}{ sir@ _integrateSample_q_1_1}
          Symbol type: int
  3594 - pointer reg req = 0
packRegisters
packRegsForAssign
  result:iTemp13 [k21 lr8:35 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int}{ sir@ _integrateSample_numsamples_1_1}
  left:
  right:0x8 {literal int}
  2705 - not packing - right is not temp
allocDirReg BAD, op is NULL
  result:iTemp13 [k21 lr8:35 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int}{ sir@ _integrateSample_numsamples_1_1}
  left:
  right:0x8 {literal int}
  result:iTemp13 [k21 lr8:35 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int}{ sir@ _integrateSample_numsamples_1_1}
          Symbol type: int
  3594 - pointer reg req = 0
packRegisters
packRegsForAssign
  result:iTemp2 [k6 lr5:46 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{int}{ sir@ _integrateSample_i_1_1}
  left:
  right:iTemp6 [k14 lr14:15 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int}
  2760 - dic result key == ic right key -- pointer set=N
  packing. removing iTemp6
  replacing with iTemp6
  result:iTemp2 [k6 lr5:46 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{int}{ sir@ _integrateSample_i_1_1}
  left:
  right:iTemp6 [k14 lr14:15 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int}
packRegsForAssign
  result:iTemp3 [k8 lr6:46 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{int}{ sir@ _integrateSample_q_1_1}
  left:
  right:iTemp8 [k16 lr20:21 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int}
  2760 - dic result key == ic right key -- pointer set=N
  packing. removing iTemp8
  replacing with iTemp8
  result:iTemp3 [k8 lr6:46 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{int}{ sir@ _integrateSample_q_1_1}
  left:
  right:iTemp8 [k16 lr20:21 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int}
packRegsForAssign
  result:iTemp2 [k6 lr5:46 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{int}{ sir@ _integrateSample_i_1_1}
  left:
  right:iTemp10 [k18 lr26:27 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int}
  2760 - dic result key == ic right key -- pointer set=N
  packing. removing iTemp10
  replacing with iTemp10
  result:iTemp2 [k6 lr5:46 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{int}{ sir@ _integrateSample_i_1_1}
  left:
  right:iTemp10 [k18 lr26:27 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int}
packRegsForAssign
  result:iTemp3 [k8 lr6:46 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{int}{ sir@ _integrateSample_q_1_1}
  left:
  right:iTemp12 [k20 lr32:33 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int}
  2760 - dic result key == ic right key -- pointer set=N
  packing. removing iTemp12
  replacing with iTemp12
  result:iTemp3 [k8 lr6:46 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{int}{ sir@ _integrateSample_q_1_1}
  left:
  right:iTemp12 [k20 lr32:33 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int}
  left:iTemp0 [k2 lr3:35 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _integrateSample_transmitter_1_1}
        Symbol type: unsigned char
  3594 - pointer reg req = 0
  left:iTemp1 [k4 lr4:35 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _integrateSample_receiver_1_1}
        Symbol type: unsigned char
  3594 - pointer reg req = 0
  3594 - pointer reg req = 0
  left:_sensorSample [k12 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int function    }
        Symbol type: int function    
  result:iTemp5 [k13 lr13:14 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int}
          Symbol type: int
  3594 - pointer reg req = 0
  left:iTemp2 [k6 lr5:46 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{int}{ sir@ _integrateSample_i_1_1}
        Symbol type: int
  right:iTemp5 [k13 lr13:14 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int}
         Symbol type: int
  result:iTemp2 [k6 lr5:46 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{int}{ sir@ _integrateSample_i_1_1}
          Symbol type: int
  3594 - pointer reg req = 0
packRegsForSupport
findAssignToSym
packRegsForAccUse
  packRegsForAccUse:3181
  left:iTemp0 [k2 lr3:35 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _integrateSample_transmitter_1_1}
        Symbol type: unsigned char
  3594 - pointer reg req = 0
  left:iTemp1 [k4 lr4:35 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _integrateSample_receiver_1_1}
        Symbol type: unsigned char
  3594 - pointer reg req = 0
  3594 - pointer reg req = 0
  left:_sensorSample [k12 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int function    }
        Symbol type: int function    
  result:iTemp7 [k15 lr19:20 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int}
          Symbol type: int
  3594 - pointer reg req = 0
  left:iTemp3 [k8 lr6:46 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{int}{ sir@ _integrateSample_q_1_1}
        Symbol type: int
  right:iTemp7 [k15 lr19:20 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int}
         Symbol type: int
  result:iTemp3 [k8 lr6:46 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{int}{ sir@ _integrateSample_q_1_1}
          Symbol type: int
  3594 - pointer reg req = 0
packRegsForSupport
findAssignToSym
packRegsForAccUse
  packRegsForAccUse:3181
  left:iTemp0 [k2 lr3:35 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _integrateSample_transmitter_1_1}
        Symbol type: unsigned char
  3594 - pointer reg req = 0
  left:iTemp1 [k4 lr4:35 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _integrateSample_receiver_1_1}
        Symbol type: unsigned char
  3594 - pointer reg req = 0
  3594 - pointer reg req = 0
  left:_sensorSample [k12 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int function    }
        Symbol type: int function    
  result:iTemp9 [k17 lr25:26 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int}
          Symbol type: int
  3594 - pointer reg req = 0
  left:iTemp2 [k6 lr5:46 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{int}{ sir@ _integrateSample_i_1_1}
        Symbol type: int
  right:iTemp9 [k17 lr25:26 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int}
         Symbol type: int
  result:iTemp2 [k6 lr5:46 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{int}{ sir@ _integrateSample_i_1_1}
          Symbol type: int
  3594 - pointer reg req = 0
packRegsForSupport
findAssignToSym
packRegsForAccUse
  packRegsForAccUse:3181
  left:iTemp0 [k2 lr3:35 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _integrateSample_transmitter_1_1}
        Symbol type: unsigned char
  3594 - pointer reg req = 0
  left:iTemp1 [k4 lr4:35 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _integrateSample_receiver_1_1}
        Symbol type: unsigned char
  3594 - pointer reg req = 0
  3594 - pointer reg req = 0
  left:_sensorSample [k12 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int function    }
        Symbol type: int function    
  result:iTemp11 [k19 lr31:32 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int}
          Symbol type: int
  3594 - pointer reg req = 0
  left:iTemp3 [k8 lr6:46 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{int}{ sir@ _integrateSample_q_1_1}
        Symbol type: int
  right:iTemp11 [k19 lr31:32 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int}
         Symbol type: int
  result:iTemp3 [k8 lr6:46 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{int}{ sir@ _integrateSample_q_1_1}
          Symbol type: int
  3594 - pointer reg req = 0
packRegsForSupport
findAssignToSym
packRegsForAccUse
  packRegsForAccUse:3181
  left:iTemp13 [k21 lr8:35 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int}{ sir@ _integrateSample_numsamples_1_1}
        Symbol type: int
  result:iTemp13 [k21 lr8:35 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int}{ sir@ _integrateSample_numsamples_1_1}
          Symbol type: int
  3594 - pointer reg req = 0
packRegsForSupport
packRegsForAccUse
  packRegsForAccUse:3181
  left:iTemp13 [k21 lr8:35 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int}{ sir@ _integrateSample_numsamples_1_1}
        Symbol type: int
  3594 - pointer reg req = 0
packRegisters
  left:iTemp2 [k6 lr5:46 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{int}{ sir@ _integrateSample_i_1_1}
        Symbol type: int
  result:iTemp14 [k22 lr36:37 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{char}
          Symbol type: char
  3594 - pointer reg req = 0
  3611
  left:iTemp14 [k22 lr36:37 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{char}
        Symbol type: char
  3594 - pointer reg req = 0
packRegisters
packRegsForAssign
  result:iTemp2 [k6 lr5:46 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{int}{ sir@ _integrateSample_i_1_1}
  left:
  right:iTemp15 [k23 lr38:39 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int}
  2760 - dic result key == ic right key -- pointer set=N
  packing. removing iTemp15
  replacing with iTemp15
  result:iTemp2 [k6 lr5:46 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{int}{ sir@ _integrateSample_i_1_1}
  left:
  right:iTemp15 [k23 lr38:39 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int}
  left:iTemp2 [k6 lr5:46 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{int}{ sir@ _integrateSample_i_1_1}
        Symbol type: int
  result:iTemp2 [k6 lr5:46 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{int}{ sir@ _integrateSample_i_1_1}
          Symbol type: int
  3594 - pointer reg req = 0
packRegisters
  left:iTemp3 [k8 lr6:46 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{int}{ sir@ _integrateSample_q_1_1}
        Symbol type: int
  result:iTemp16 [k24 lr41:42 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{char}
          Symbol type: char
  3594 - pointer reg req = 0
  3611
  left:iTemp16 [k24 lr41:42 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{char}
        Symbol type: char
  3594 - pointer reg req = 0
packRegisters
packRegsForAssign
  result:iTemp3 [k8 lr6:46 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{int}{ sir@ _integrateSample_q_1_1}
  left:
  right:iTemp17 [k25 lr43:44 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int}
  2760 - dic result key == ic right key -- pointer set=N
  packing. removing iTemp17
  replacing with iTemp17
  result:iTemp3 [k8 lr6:46 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{int}{ sir@ _integrateSample_q_1_1}
  left:
  right:iTemp17 [k25 lr43:44 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int}
  left:iTemp3 [k8 lr6:46 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{int}{ sir@ _integrateSample_q_1_1}
        Symbol type: int
  result:iTemp3 [k8 lr6:46 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{int}{ sir@ _integrateSample_q_1_1}
          Symbol type: int
  3594 - pointer reg req = 0
packRegisters
  left:iTemp2 [k6 lr5:46 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{int}{ sir@ _integrateSample_i_1_1}
        Symbol type: int
  right:iTemp3 [k8 lr6:46 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{int}{ sir@ _integrateSample_q_1_1}
         Symbol type: int
  result:iTemp18 [k26 lr46:47 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int}
          Symbol type: int
  3594 - pointer reg req = 0
packRegsForSupport
findAssignToSym
findAssignToSym
packRegsForAccUse
  packRegsForAccUse:3181
  left:iTemp18 [k26 lr46:47 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int}
        Symbol type: int
  3594 - pointer reg req = 0
packRegisters
  left:_integrateSample [k1 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int function    }
        Symbol type: int function    
dir registers allocated so far:
  -- #0 reg = s0xFFF  key 0, rIdx = 4095, size 1
  -- #1 reg = s0x1000  key 0, rIdx = 4096, size 1
  -- #2 reg = s0x1001  key 0, rIdx = 4097, size 1
  -- #3 reg = s0x1002  key 0, rIdx = 4098, size 1
  -- #4 reg = s0x1003  key 0, rIdx = 4099, size 1
  -- #5 reg = s0x1004  key 0, rIdx = 4100, size 1
  -- #6 reg = s0x1005  key 0, rIdx = 4101, size 1
  -- #7 reg = s0x1006  key 0, rIdx = 4102, size 1
  -- #8 reg = r0x20  key 0, rIdx = 32, size 1
  -- #9 reg = r0x21  key 0, rIdx = 33, size 1
  -- #10 reg = r0x22  key 0, rIdx = 34, size 1
  -- #11 reg = r0x23  key 0, rIdx = 35, size 1
  -- #12 reg = r0x24  key 0, rIdx = 36, size 1
  -- #13 reg = r0x25  key 0, rIdx = 37, size 1
  -- #14 reg = r0x26  key 0, rIdx = 38, size 1
  -- #15 reg = r0x27  key 0, rIdx = 39, size 1
  -- #16 reg = r0x28  key 0, rIdx = 40, size 1
  -- #17 reg = r0x29  key 0, rIdx = 41, size 1
  -- #18 reg = r0x2A  key 0, rIdx = 42, size 1
  -- #19 reg = r0x2B  key 0, rIdx = 43, size 1
  -- #20 reg = r0x2C  key 0, rIdx = 44, size 1
  -- #21 reg = r0x2D  key 0, rIdx = 45, size 1
  -- #22 reg = r0x2E  key 0, rIdx = 46, size 1
  -- #23 reg = r0x2F  key 0, rIdx = 47, size 1
  -- #24 reg = r0x30  key 0, rIdx = 48, size 1
  -- #25 reg = r0x31  key 0, rIdx = 49, size 1
  -- #26 reg = r0x32  key 0, rIdx = 50, size 1
  -- #27 reg = r0x33  key 0, rIdx = 51, size 1
  -- #28 reg = r0x34  key 0, rIdx = 52, size 1
  -- #29 reg = r0x35  key 0, rIdx = 53, size 1
  -- #30 reg = r0x36  key 0, rIdx = 54, size 1
  -- #31 reg = r0x37  key 0, rIdx = 55, size 1
  -- #32 reg = r0x38  key 0, rIdx = 56, size 1
  -- #33 reg = r0x39  key 0, rIdx = 57, size 1
  -- #34 reg = r0x3A  key 0, rIdx = 58, size 1
  -- #35 reg = r0x3B  key 0, rIdx = 59, size 1
  -- #36 reg = r0x3C  key 0, rIdx = 60, size 1
  -- #37 reg = r0x3D  key 0, rIdx = 61, size 1
  -- #38 reg = r0x3E  key 0, rIdx = 62, size 1
  -- #39 reg = r0x3F  key 0, rIdx = 63, size 1
  -- #40 reg = r0x40  key 0, rIdx = 64, size 1
  -- #41 reg = r0x41  key 0, rIdx = 65, size 1
  -- #42 reg = r0x42  key 0, rIdx = 66, size 1
  -- #43 reg = r0x43  key 0, rIdx = 67, size 1
  -- #44 reg = r0x44  key 0, rIdx = 68, size 1
  -- #45 reg = r0x45  key 0, rIdx = 69, size 1
  -- #46 reg = r0x46  key 0, rIdx = 70, size 1
  -- #47 reg = r0x47  key 0, rIdx = 71, size 1
  -- #48 reg = r0x48  key 0, rIdx = 72, size 1
  -- #49 reg = r0x49  key 0, rIdx = 73, size 1
  -- #50 reg = r0x4A  key 0, rIdx = 74, size 1
  -- #51 reg = r0x4B  key 0, rIdx = 75, size 1
  -- #52 reg = r0x4C  key 0, rIdx = 76, size 1
  -- #53 reg = r0x4D  key 0, rIdx = 77, size 1
  -- #54 reg = r0x4E  key 0, rIdx = 78, size 1
  -- #55 reg = r0x4F  key 0, rIdx = 79, size 1
  -- #56 reg = r0x50  key 0, rIdx = 80, size 1
  -- #57 reg = r0x51  key 0, rIdx = 81, size 1
  -- #58 reg = r0x52  key 0, rIdx = 82, size 1
  -- #59 reg = _PCLATH  key 6, rIdx = 10, size 1
  -- #60 reg = _TRISA  key 7, rIdx = 1, size 1
  -- #61 reg = _TRISB  key 8, rIdx = 2, size 1
  -- #62 reg = _TRISC  key 9, rIdx = 3, size 1
  -- #63 reg = _TRISD  key 10, rIdx = 4, size 1
  -- #64 reg = _PORTA  key 10, rIdx = 6, size 1
  -- #65 reg = _TRISE  key 11, rIdx = 5, size 1
  -- #66 reg = _PORTB  key 11, rIdx = 7, size 1
  -- #67 reg = _PORTC  key 12, rIdx = 8, size 1
  -- #68 reg = _GIE  key 12, rIdx = 13, size 1
  -- #69 reg = _PORTD  key 13, rIdx = 9, size 1
  -- #70 reg = _PORTE  key 14, rIdx = 10, size 1
  -- #71 reg = _ADCON0  key 27, rIdx = 12, size 1
  -- #72 reg = _ADCON1  key 28, rIdx = 11, size 1
  -- #73 reg = SSAVE  key 32, rIdx = 32765, size 1
  -- #74 reg = WSAVE  key 36, rIdx = 32766, size 1
  -- #75 reg = _reading0  key 44, rIdx = 14, size 2
  -- #76 reg = _reading1  key 45, rIdx = 15, size 2
  -- #77 reg = _reading2  key 46, rIdx = 16, size 2
  -- #78 reg = INTCON  key 47, rIdx = 11, size 1
  -- #79 reg = _reading3  key 47, rIdx = 17, size 2
  -- #80 reg = _STATUS  key 48, rIdx = 3, size 1
  -- #81 reg = PCL  key 48, rIdx = 2, size 1
  -- #82 reg = _reading4  key 48, rIdx = 18, size 2
  -- #83 reg = KZ  key 49, rIdx = 32767, size 1
  -- #84 reg = _reading5  key 49, rIdx = 19, size 2
  -- #85 reg = _reading6  key 50, rIdx = 20, size 2
  -- #86 reg = _reading7  key 51, rIdx = 21, size 2
  -- #87 reg = INDF  key 56, rIdx = 0, size 1
  -- #88 reg = _Light0  key 56, rIdx = 0, size 1
  -- #89 reg = FSR  key 60, rIdx = 4, size 1
regTypeNum
  2421 - iTemp0
  2431 - itemp register
  reg name iTemp0,  reg type REG_GPR
  2421 - iTemp1
  2431 - itemp register
  reg name iTemp1,  reg type REG_GPR
  2421 - iTemp2
  2431 - itemp register
  reg name iTemp2,  reg type REG_GPR
  2421 - iTemp3
  2431 - itemp register
  reg name iTemp3,  reg type REG_GPR
  2421 - iTemp5
  2431 - itemp register
  reg name iTemp5,  reg type REG_GPR
  2421 - iTemp6
  2431 - itemp register
  reg name iTemp6,  reg type REG_GPR
  2421 - iTemp7
  2431 - itemp register
  reg name iTemp7,  reg type REG_GPR
  2421 - iTemp8
  2431 - itemp register
  reg name iTemp8,  reg type REG_GPR
  2421 - iTemp9
  2431 - itemp register
  reg name iTemp9,  reg type REG_GPR
  2421 - iTemp10
  2431 - itemp register
  reg name iTemp10,  reg type REG_GPR
  2421 - iTemp11
  2431 - itemp register
  reg name iTemp11,  reg type REG_GPR
  2421 - iTemp12
  2431 - itemp register
  reg name iTemp12,  reg type REG_GPR
  2421 - iTemp13
  2431 - itemp register
  reg name iTemp13,  reg type REG_GPR
  2421 - iTemp14
  2431 - itemp register
  2421 - iTemp15
  2431 - itemp register
  reg name iTemp15,  reg type REG_GPR
  2421 - iTemp16
  2431 - itemp register
  2421 - iTemp17
  2431 - itemp register
  reg name iTemp17,  reg type REG_GPR
  2421 - iTemp18
  2431 - itemp register
  reg name iTemp18,  reg type REG_GPR
serialRegAssign
  op: LABEL
deassignLRs
  op: unkown op 9 	
deassignLRs
  op: RECEIVE
deassignLRs
willCauseSpill
computeSpillable
When I get clever, I'll optimize the receive logic
  2125 - 
bitvector Size = 58 bSize = 8
Bits on { (2) }
getRegGpr
allocReg of type REG_GPR
  2141 - 
  2155 - 
  op: RECEIVE
deassignLRs
willCauseSpill
computeSpillable
When I get clever, I'll optimize the receive logic
  2125 - 
bitvector Size = 58 bSize = 8
Bits on { (2) (4) }
getRegGpr
allocReg of type REG_GPR
  2141 - 
  2155 - 
  op: =
deassignLRs
willCauseSpill
computeSpillable
  2125 - 
bitvector Size = 58 bSize = 8
Bits on { (2) (4) (6) }
getRegGpr
allocReg of type REG_GPR
getRegGpr
allocReg of type REG_GPR
  2141 - 
  2155 - 
  op: =
deassignLRs
willCauseSpill
computeSpillable
  2125 - 
bitvector Size = 58 bSize = 8
Bits on { (2) (4) (6) (8) }
getRegGpr
allocReg of type REG_GPR
getRegGpr
allocReg of type REG_GPR
  2141 - 
  2155 - 
  op: LABEL
deassignLRs
  op: =
deassignLRs
willCauseSpill
computeSpillable
  2125 - 
bitvector Size = 58 bSize = 8
Bits on { (2) (4) (6) (8) (21) }
getRegGpr
allocReg of type REG_GPR
getRegGpr
allocReg of type REG_GPR
  2141 - 
  2155 - 
  op: LABEL
deassignLRs
  op: SEND
deassignLRs
  op: SEND
deassignLRs
  op: SEND
deassignLRs
  op: CALL
deassignLRs
willCauseSpill
computeSpillable
  2125 - 
bitvector Size = 58 bSize = 8
Bits on { (2) (4) (6) (8) (13) (21) }
getRegGpr
allocReg of type REG_GPR
getRegGpr
allocReg of type REG_GPR
  2141 - 
  2155 - 
  op: +
deassignLRs
freeReg
freeReg
  op: SEND
deassignLRs
  op: SEND
deassignLRs
  op: SEND
deassignLRs
  op: CALL
deassignLRs
willCauseSpill
computeSpillable
  2125 - 
bitvector Size = 58 bSize = 8
Bits on { (2) (4) (6) (8) (15) (21) }
getRegGpr
allocReg of type REG_GPR
getRegGpr
allocReg of type REG_GPR
  2141 - 
  2155 - 
  op: +
deassignLRs
freeReg
freeReg
  op: SEND
deassignLRs
  op: SEND
deassignLRs
  op: SEND
deassignLRs
  op: CALL
deassignLRs
willCauseSpill
computeSpillable
  2125 - 
bitvector Size = 58 bSize = 8
Bits on { (2) (4) (6) (8) (17) (21) }
getRegGpr
allocReg of type REG_GPR
getRegGpr
allocReg of type REG_GPR
  2141 - 
  2155 - 
  op: -
deassignLRs
freeReg
freeReg
  op: SEND
deassignLRs
  op: SEND
deassignLRs
  op: SEND
deassignLRs
  op: CALL
deassignLRs
willCauseSpill
computeSpillable
  2125 - 
bitvector Size = 58 bSize = 8
Bits on { (2) (4) (6) (8) (19) (21) }
getRegGpr
allocReg of type REG_GPR
getRegGpr
allocReg of type REG_GPR
  2141 - 
  2155 - 
  op: -
deassignLRs
freeReg
freeReg
  op: -
deassignLRs
  op: IFX
deassignLRs
freeReg
freeReg
freeReg
freeReg
  op: <
deassignLRs
  op: IFX
deassignLRs
  op: UNARYMINUS
deassignLRs
  op: LABEL
deassignLRs
  op: <
deassignLRs
  op: IFX
deassignLRs
  op: UNARYMINUS
deassignLRs
  op: LABEL
deassignLRs
  op: +
deassignLRs
nfreeRegsType
freeReg
freeReg
  op: RETURN
deassignLRs
freeReg
freeReg
  op: LABEL
deassignLRs
  op: ENDFUNCTION
deassignLRs
createRegMask
regsUsedIniCode
rUmaskForOp
regsUsedIniCode
rUmaskForOp
regsUsedIniCode
rUmaskForOp
rUmaskForOp
regsUsedIniCode
rUmaskForOp
rUmaskForOp
regsUsedIniCode
rUmaskForOp
rUmaskForOp
regsUsedIniCode
rUmaskForOp
regsUsedIniCode
rUmaskForOp
regsUsedIniCode
rUmaskForOp
regsUsedIniCode
rUmaskForOp
rUmaskForOp
regsUsedIniCode
rUmaskForOp
rUmaskForOp
rUmaskForOp
regsUsedIniCode
rUmaskForOp
regsUsedIniCode
rUmaskForOp
regsUsedIniCode
rUmaskForOp
regsUsedIniCode
rUmaskForOp
rUmaskForOp
regsUsedIniCode
rUmaskForOp
rUmaskForOp
rUmaskForOp
regsUsedIniCode
rUmaskForOp
regsUsedIniCode
rUmaskForOp
regsUsedIniCode
rUmaskForOp
regsUsedIniCode
rUmaskForOp
rUmaskForOp
regsUsedIniCode
rUmaskForOp
rUmaskForOp
rUmaskForOp
regsUsedIniCode
rUmaskForOp
regsUsedIniCode
rUmaskForOp
regsUsedIniCode
rUmaskForOp
regsUsedIniCode
rUmaskForOp
rUmaskForOp
regsUsedIniCode
rUmaskForOp
rUmaskForOp
rUmaskForOp
regsUsedIniCode
rUmaskForOp
rUmaskForOp
rUmaskForOp
regsUsedIniCode
rUmaskForOp
regsUsedIniCode
rUmaskForOp
rUmaskForOp
rUmaskForOp
regsUsedIniCode
rUmaskForOp
regsUsedIniCode
rUmaskForOp
rUmaskForOp
regsUsedIniCode
rUmaskForOp
rUmaskForOp
rUmaskForOp
regsUsedIniCode
rUmaskForOp
regsUsedIniCode
rUmaskForOp
rUmaskForOp
regsUsedIniCode
rUmaskForOp
rUmaskForOp
rUmaskForOp
regsUsedIniCode
rUmaskForOp
ebbs after optimizing:

----------------------------------------------------------------
Basic Block _entry : loop Depth = 0 noPath = 0 , lastinLoop = 0
depth 1st num 1 : bbnum = 0 1st iCode = 1 , last iCode = 6
visited 0 : hasFcall = 0

defines bitVector :bitvector Size = 58 bSize = 8
Bits on { (2) (3) (4) (5) }

local defines bitVector :bitvector Size = 58 bSize = 8
Bits on { (2) (3) (4) (5) (6) }

pointers Set bitvector :
in pointers Set bitvector :
inDefs Set bitvector :
outDefs Set bitvector :bitvector Size = 58 bSize = 8
Bits on { (2) (3) (4) (5) }

usesDefs Set bitvector :bitvector Size = 58 bSize = 8
Bits on { (2) (3) }

----------------------------------------------------------------
main.c(146:1:0:0:0)	 _entry($9) :
main.c(146:2:1:0:0)		proc _integrateSample [k1 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int function    }
main.c(146:3:2:0:0)	iTemp0 [k2 lr3:35 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _integrateSample_transmitter_1_1}[r0x53 ] = recv 
main.c(146:4:3:0:0)	iTemp1 [k4 lr4:35 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _integrateSample_receiver_1_1}[r0x54 ] = recv 
main.c(147:5:4:0:0)		iTemp2 [k6 lr5:46 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{int}{ sir@ _integrateSample_i_1_1}[r0x55 r0x56 ] := 0x0 {literal int}
main.c(147:6:5:0:0)		iTemp3 [k8 lr6:46 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{int}{ sir@ _integrateSample_q_1_1}[r0x57 r0x58 ] := 0x0 {literal int}
main.c(150:8:57:0:0)		iTemp13 [k21 lr8:35 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int}{ sir@ _integrateSample_numsamples_1_1}[r0x59 r0x5A ] := 0x8 {literal int}
main.c(150:9:7:1:0)	 _dobody_0($1) :
main.c(152:10:8:1:0)		send iTemp0 [k2 lr3:35 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _integrateSample_transmitter_1_1}[r0x53 ]{argreg = 1}
main.c(152:11:9:1:0)		send iTemp1 [k4 lr4:35 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _integrateSample_receiver_1_1}[r0x54 ]{argreg = 1}
main.c(152:12:10:1:0)		send 0x0 {literal unsigned char}{argreg = 1}
main.c(152:13:11:1:0)		iTemp5 [k13 lr13:14 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int}[r0x5B r0x5C ] = call _sensorSample [k12 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int function    }
main.c(152:14:12:1:0)		iTemp2 [k6 lr5:46 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{int}{ sir@ _integrateSample_i_1_1}[r0x55 r0x56 ] = iTemp2 [k6 lr5:46 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{int}{ sir@ _integrateSample_i_1_1}[r0x55 r0x56 ] + iTemp5 [k13 lr13:14 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int}[r0x5B r0x5C ]
main.c(153:16:14:1:0)		send iTemp0 [k2 lr3:35 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _integrateSample_transmitter_1_1}[r0x53 ]{argreg = 1}
main.c(153:17:15:1:0)		send iTemp1 [k4 lr4:35 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _integrateSample_receiver_1_1}[r0x54 ]{argreg = 1}
main.c(153:18:16:1:0)		send 0x1 {literal unsigned char}{argreg = 1}
main.c(153:19:17:1:0)		iTemp7 [k15 lr19:20 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int}[r0x5D r0x5E ] = call _sensorSample [k12 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int function    }
main.c(153:20:18:1:0)		iTemp3 [k8 lr6:46 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{int}{ sir@ _integrateSample_q_1_1}[r0x57 r0x58 ] = iTemp3 [k8 lr6:46 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{int}{ sir@ _integrateSample_q_1_1}[r0x57 r0x58 ] + iTemp7 [k15 lr19:20 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int}[r0x5D r0x5E ]
main.c(154:22:20:1:0)		send iTemp0 [k2 lr3:35 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _integrateSample_transmitter_1_1}[r0x53 ]{argreg = 1}
main.c(154:23:21:1:0)		send iTemp1 [k4 lr4:35 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _integrateSample_receiver_1_1}[r0x54 ]{argreg = 1}
main.c(154:24:22:1:0)		send 0x2 {literal unsigned char}{argreg = 1}
main.c(154:25:23:1:0)		iTemp9 [k17 lr25:26 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int}[r0x5F r0x60 ] = call _sensorSample [k12 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int function    }
main.c(154:26:24:1:0)		iTemp2 [k6 lr5:46 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{int}{ sir@ _integrateSample_i_1_1}[r0x55 r0x56 ] = iTemp2 [k6 lr5:46 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{int}{ sir@ _integrateSample_i_1_1}[r0x55 r0x56 ] - iTemp9 [k17 lr25:26 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int}[r0x5F r0x60 ]
main.c(155:28:26:1:0)		send iTemp0 [k2 lr3:35 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _integrateSample_transmitter_1_1}[r0x53 ]{argreg = 1}
main.c(155:29:27:1:0)		send iTemp1 [k4 lr4:35 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _integrateSample_receiver_1_1}[r0x54 ]{argreg = 1}
main.c(155:30:28:1:0)		send 0x3 {literal unsigned char}{argreg = 1}
main.c(155:31:29:1:0)		iTemp11 [k19 lr31:32 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int}[r0x61 r0x62 ] = call _sensorSample [k12 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int function    }
main.c(155:32:30:1:0)		iTemp3 [k8 lr6:46 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{int}{ sir@ _integrateSample_q_1_1}[r0x57 r0x58 ] = iTemp3 [k8 lr6:46 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{int}{ sir@ _integrateSample_q_1_1}[r0x57 r0x58 ] - iTemp11 [k19 lr31:32 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int}[r0x61 r0x62 ]
main.c(156:34:33:1:0)		iTemp13 [k21 lr8:35 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int}{ sir@ _integrateSample_numsamples_1_1}[r0x59 r0x5A ] = iTemp13 [k21 lr8:35 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int}{ sir@ _integrateSample_numsamples_1_1}[r0x59 r0x5A ] - 0x1 {literal unsigned char}
main.c(156:35:35:1:0)		if iTemp13 [k21 lr8:35 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int}{ sir@ _integrateSample_numsamples_1_1}[r0x59 r0x5A ] != 0 goto _dobody_0($1)
main.c(159:36:38:0:0)		iTemp14 [k22 lr36:37 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{char} = iTemp2 [k6 lr5:46 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{int}{ sir@ _integrateSample_i_1_1}[r0x55 r0x56 ] < 0x0 {literal int}
main.c(159:37:39:0:0)		if iTemp14 [k22 lr36:37 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{char} == 0 goto _iffalse_0($5)
main.c(159:38:42:0:0)		iTemp2 [k6 lr5:46 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{int}{ sir@ _integrateSample_i_1_1}[r0x55 r0x56 ] = - iTemp2 [k6 lr5:46 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{int}{ sir@ _integrateSample_i_1_1}[r0x55 r0x56 ]
main.c(159:40:44:0:0)	 _iffalse_0($5) :
main.c(160:41:45:0:0)		iTemp16 [k24 lr41:42 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{char} = iTemp3 [k8 lr6:46 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{int}{ sir@ _integrateSample_q_1_1}[r0x57 r0x58 ] < 0x0 {literal int}
main.c(160:42:46:0:0)		if iTemp16 [k24 lr41:42 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{char} == 0 goto _iffalse_1($7)
main.c(160:43:49:0:0)		iTemp3 [k8 lr6:46 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{int}{ sir@ _integrateSample_q_1_1}[r0x57 r0x58 ] = - iTemp3 [k8 lr6:46 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{int}{ sir@ _integrateSample_q_1_1}[r0x57 r0x58 ]
main.c(160:45:51:0:0)	 _iffalse_1($7) :
main.c(161:46:52:0:0)		iTemp18 [k26 lr46:47 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int}[r0x55 r0x56 ] = iTemp2 [k6 lr5:46 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{int}{ sir@ _integrateSample_i_1_1}[r0x55 r0x56 ] + iTemp3 [k8 lr6:46 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{int}{ sir@ _integrateSample_q_1_1}[r0x57 r0x58 ]
main.c(161:47:53:0:0)		ret iTemp18 [k26 lr46:47 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int}[r0x55 r0x56 ]
main.c(161:48:54:0:0)	 _return($8) :
main.c(161:49:55:0:0)		eproc _integrateSample [k1 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int function    }

----------------------------------------------------------------
Basic Block preHeaderLbl0 : loop Depth = 0 noPath = 0 , lastinLoop = 0
depth 1st num 2 : bbnum = 1 1st iCode = 7 , last iCode = 8
visited 0 : hasFcall = 0

defines bitVector :bitvector Size = 58 bSize = 8
Bits on { (57) }

local defines bitVector :bitvector Size = 58 bSize = 8
Bits on { (57) }

pointers Set bitvector :
in pointers Set bitvector :
inDefs Set bitvector :bitvector Size = 58 bSize = 8
Bits on { (2) (3) (4) (5) (6) }

outDefs Set bitvector :bitvector Size = 58 bSize = 8
Bits on { (2) (3) (4) (5) (6) (57) }

usesDefs Set bitvector :bitvector Size = 58 bSize = 8
Bits on { }

----------------------------------------------------------------
main.c(150:7:56:0:0)	 preHeaderLbl0($10) :
main.c(150:8:57:0:0)		iTemp13 [k21 lr8:35 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int}{ sir@ _integrateSample_numsamples_1_1}[r0x59 r0x5A ] := 0x8 {literal int}
main.c(150:9:7:1:0)	 _dobody_0($1) :
main.c(152:10:8:1:0)		send iTemp0 [k2 lr3:35 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _integrateSample_transmitter_1_1}[r0x53 ]{argreg = 1}
main.c(152:11:9:1:0)		send iTemp1 [k4 lr4:35 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _integrateSample_receiver_1_1}[r0x54 ]{argreg = 1}
main.c(152:12:10:1:0)		send 0x0 {literal unsigned char}{argreg = 1}
main.c(152:13:11:1:0)		iTemp5 [k13 lr13:14 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int}[r0x5B r0x5C ] = call _sensorSample [k12 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int function    }
main.c(152:14:12:1:0)		iTemp2 [k6 lr5:46 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{int}{ sir@ _integrateSample_i_1_1}[r0x55 r0x56 ] = iTemp2 [k6 lr5:46 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{int}{ sir@ _integrateSample_i_1_1}[r0x55 r0x56 ] + iTemp5 [k13 lr13:14 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int}[r0x5B r0x5C ]
main.c(153:16:14:1:0)		send iTemp0 [k2 lr3:35 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _integrateSample_transmitter_1_1}[r0x53 ]{argreg = 1}
main.c(153:17:15:1:0)		send iTemp1 [k4 lr4:35 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _integrateSample_receiver_1_1}[r0x54 ]{argreg = 1}
main.c(153:18:16:1:0)		send 0x1 {literal unsigned char}{argreg = 1}
main.c(153:19:17:1:0)		iTemp7 [k15 lr19:20 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int}[r0x5D r0x5E ] = call _sensorSample [k12 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int function    }
main.c(153:20:18:1:0)		iTemp3 [k8 lr6:46 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{int}{ sir@ _integrateSample_q_1_1}[r0x57 r0x58 ] = iTemp3 [k8 lr6:46 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{int}{ sir@ _integrateSample_q_1_1}[r0x57 r0x58 ] + iTemp7 [k15 lr19:20 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int}[r0x5D r0x5E ]
main.c(154:22:20:1:0)		send iTemp0 [k2 lr3:35 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _integrateSample_transmitter_1_1}[r0x53 ]{argreg = 1}
main.c(154:23:21:1:0)		send iTemp1 [k4 lr4:35 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _integrateSample_receiver_1_1}[r0x54 ]{argreg = 1}
main.c(154:24:22:1:0)		send 0x2 {literal unsigned char}{argreg = 1}
main.c(154:25:23:1:0)		iTemp9 [k17 lr25:26 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int}[r0x5F r0x60 ] = call _sensorSample [k12 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int function    }
main.c(154:26:24:1:0)		iTemp2 [k6 lr5:46 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{int}{ sir@ _integrateSample_i_1_1}[r0x55 r0x56 ] = iTemp2 [k6 lr5:46 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{int}{ sir@ _integrateSample_i_1_1}[r0x55 r0x56 ] - iTemp9 [k17 lr25:26 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int}[r0x5F r0x60 ]
main.c(155:28:26:1:0)		send iTemp0 [k2 lr3:35 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _integrateSample_transmitter_1_1}[r0x53 ]{argreg = 1}
main.c(155:29:27:1:0)		send iTemp1 [k4 lr4:35 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _integrateSample_receiver_1_1}[r0x54 ]{argreg = 1}
main.c(155:30:28:1:0)		send 0x3 {literal unsigned char}{argreg = 1}
main.c(155:31:29:1:0)		iTemp11 [k19 lr31:32 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int}[r0x61 r0x62 ] = call _sensorSample [k12 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int function    }
main.c(155:32:30:1:0)		iTemp3 [k8 lr6:46 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{int}{ sir@ _integrateSample_q_1_1}[r0x57 r0x58 ] = iTemp3 [k8 lr6:46 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{int}{ sir@ _integrateSample_q_1_1}[r0x57 r0x58 ] - iTemp11 [k19 lr31:32 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int}[r0x61 r0x62 ]
main.c(156:34:33:1:0)		iTemp13 [k21 lr8:35 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int}{ sir@ _integrateSample_numsamples_1_1}[r0x59 r0x5A ] = iTemp13 [k21 lr8:35 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int}{ sir@ _integrateSample_numsamples_1_1}[r0x59 r0x5A ] - 0x1 {literal unsigned char}
main.c(156:35:35:1:0)		if iTemp13 [k21 lr8:35 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int}{ sir@ _integrateSample_numsamples_1_1}[r0x59 r0x5A ] != 0 goto _dobody_0($1)
main.c(159:36:38:0:0)		iTemp14 [k22 lr36:37 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{char} = iTemp2 [k6 lr5:46 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{int}{ sir@ _integrateSample_i_1_1}[r0x55 r0x56 ] < 0x0 {literal int}
main.c(159:37:39:0:0)		if iTemp14 [k22 lr36:37 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{char} == 0 goto _iffalse_0($5)
main.c(159:38:42:0:0)		iTemp2 [k6 lr5:46 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{int}{ sir@ _integrateSample_i_1_1}[r0x55 r0x56 ] = - iTemp2 [k6 lr5:46 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{int}{ sir@ _integrateSample_i_1_1}[r0x55 r0x56 ]
main.c(159:40:44:0:0)	 _iffalse_0($5) :
main.c(160:41:45:0:0)		iTemp16 [k24 lr41:42 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{char} = iTemp3 [k8 lr6:46 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{int}{ sir@ _integrateSample_q_1_1}[r0x57 r0x58 ] < 0x0 {literal int}
main.c(160:42:46:0:0)		if iTemp16 [k24 lr41:42 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{char} == 0 goto _iffalse_1($7)
main.c(160:43:49:0:0)		iTemp3 [k8 lr6:46 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{int}{ sir@ _integrateSample_q_1_1}[r0x57 r0x58 ] = - iTemp3 [k8 lr6:46 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{int}{ sir@ _integrateSample_q_1_1}[r0x57 r0x58 ]
main.c(160:45:51:0:0)	 _iffalse_1($7) :
main.c(161:46:52:0:0)		iTemp18 [k26 lr46:47 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int}[r0x55 r0x56 ] = iTemp2 [k6 lr5:46 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{int}{ sir@ _integrateSample_i_1_1}[r0x55 r0x56 ] + iTemp3 [k8 lr6:46 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{int}{ sir@ _integrateSample_q_1_1}[r0x57 r0x58 ]
main.c(161:47:53:0:0)		ret iTemp18 [k26 lr46:47 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int}[r0x55 r0x56 ]
main.c(161:48:54:0:0)	 _return($8) :
main.c(161:49:55:0:0)		eproc _integrateSample [k1 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int function    }

----------------------------------------------------------------
Basic Block _dobody_0 : loop Depth = 1 noPath = 0 , lastinLoop = 1
depth 1st num 3 : bbnum = 2 1st iCode = 9 , last iCode = 35
visited 0 : hasFcall = 1

defines bitVector :bitvector Size = 58 bSize = 8
Bits on { (11) (12) (17) (18) (23) (24) (25) (29) (30) (31) (33) }

local defines bitVector :bitvector Size = 58 bSize = 8
Bits on { (11) (12) (13) (17) (18) (19) (23) (24) (25) (29) (30) (31) (33) (34) }

pointers Set bitvector :
in pointers Set bitvector :
inDefs Set bitvector :bitvector Size = 58 bSize = 8
Bits on { (2) (3) (4) (5) (6) (11) (12) (17) (18) (23) (24) (25) (29) (30) (31) (33) (34) (57) }

outDefs Set bitvector :bitvector Size = 58 bSize = 8
Bits on { (2) (3) (11) (12) (17) (18) (23) (24) (25) (29) (30) (31) (33) }

usesDefs Set bitvector :bitvector Size = 58 bSize = 8
Bits on { (2) (3) (4) (5) (11) (12) (13) (17) (18) (19) (23) (24) (25) (29) (30) (31) (33) (57) }

----------------------------------------------------------------
main.c(150:9:7:1:0)	 _dobody_0($1) :
main.c(152:10:8:1:0)		send iTemp0 [k2 lr3:35 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _integrateSample_transmitter_1_1}[r0x53 ]{argreg = 1}
main.c(152:11:9:1:0)		send iTemp1 [k4 lr4:35 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _integrateSample_receiver_1_1}[r0x54 ]{argreg = 1}
main.c(152:12:10:1:0)		send 0x0 {literal unsigned char}{argreg = 1}
main.c(152:13:11:1:0)		iTemp5 [k13 lr13:14 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int}[r0x5B r0x5C ] = call _sensorSample [k12 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int function    }
main.c(152:14:12:1:0)		iTemp2 [k6 lr5:46 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{int}{ sir@ _integrateSample_i_1_1}[r0x55 r0x56 ] = iTemp2 [k6 lr5:46 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{int}{ sir@ _integrateSample_i_1_1}[r0x55 r0x56 ] + iTemp5 [k13 lr13:14 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int}[r0x5B r0x5C ]
main.c(153:16:14:1:0)		send iTemp0 [k2 lr3:35 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _integrateSample_transmitter_1_1}[r0x53 ]{argreg = 1}
main.c(153:17:15:1:0)		send iTemp1 [k4 lr4:35 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _integrateSample_receiver_1_1}[r0x54 ]{argreg = 1}
main.c(153:18:16:1:0)		send 0x1 {literal unsigned char}{argreg = 1}
main.c(153:19:17:1:0)		iTemp7 [k15 lr19:20 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int}[r0x5D r0x5E ] = call _sensorSample [k12 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int function    }
main.c(153:20:18:1:0)		iTemp3 [k8 lr6:46 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{int}{ sir@ _integrateSample_q_1_1}[r0x57 r0x58 ] = iTemp3 [k8 lr6:46 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{int}{ sir@ _integrateSample_q_1_1}[r0x57 r0x58 ] + iTemp7 [k15 lr19:20 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int}[r0x5D r0x5E ]
main.c(154:22:20:1:0)		send iTemp0 [k2 lr3:35 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _integrateSample_transmitter_1_1}[r0x53 ]{argreg = 1}
main.c(154:23:21:1:0)		send iTemp1 [k4 lr4:35 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _integrateSample_receiver_1_1}[r0x54 ]{argreg = 1}
main.c(154:24:22:1:0)		send 0x2 {literal unsigned char}{argreg = 1}
main.c(154:25:23:1:0)		iTemp9 [k17 lr25:26 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int}[r0x5F r0x60 ] = call _sensorSample [k12 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int function    }
main.c(154:26:24:1:0)		iTemp2 [k6 lr5:46 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{int}{ sir@ _integrateSample_i_1_1}[r0x55 r0x56 ] = iTemp2 [k6 lr5:46 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{int}{ sir@ _integrateSample_i_1_1}[r0x55 r0x56 ] - iTemp9 [k17 lr25:26 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int}[r0x5F r0x60 ]
main.c(155:28:26:1:0)		send iTemp0 [k2 lr3:35 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _integrateSample_transmitter_1_1}[r0x53 ]{argreg = 1}
main.c(155:29:27:1:0)		send iTemp1 [k4 lr4:35 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _integrateSample_receiver_1_1}[r0x54 ]{argreg = 1}
main.c(155:30:28:1:0)		send 0x3 {literal unsigned char}{argreg = 1}
main.c(155:31:29:1:0)		iTemp11 [k19 lr31:32 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int}[r0x61 r0x62 ] = call _sensorSample [k12 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int function    }
main.c(155:32:30:1:0)		iTemp3 [k8 lr6:46 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{int}{ sir@ _integrateSample_q_1_1}[r0x57 r0x58 ] = iTemp3 [k8 lr6:46 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{int}{ sir@ _integrateSample_q_1_1}[r0x57 r0x58 ] - iTemp11 [k19 lr31:32 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int}[r0x61 r0x62 ]
main.c(156:34:33:1:0)		iTemp13 [k21 lr8:35 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int}{ sir@ _integrateSample_numsamples_1_1}[r0x59 r0x5A ] = iTemp13 [k21 lr8:35 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int}{ sir@ _integrateSample_numsamples_1_1}[r0x59 r0x5A ] - 0x1 {literal unsigned char}
main.c(156:35:35:1:0)		if iTemp13 [k21 lr8:35 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int}{ sir@ _integrateSample_numsamples_1_1}[r0x59 r0x5A ] != 0 goto _dobody_0($1)
main.c(159:36:38:0:0)		iTemp14 [k22 lr36:37 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{char} = iTemp2 [k6 lr5:46 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{int}{ sir@ _integrateSample_i_1_1}[r0x55 r0x56 ] < 0x0 {literal int}
main.c(159:37:39:0:0)		if iTemp14 [k22 lr36:37 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{char} == 0 goto _iffalse_0($5)
main.c(159:38:42:0:0)		iTemp2 [k6 lr5:46 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{int}{ sir@ _integrateSample_i_1_1}[r0x55 r0x56 ] = - iTemp2 [k6 lr5:46 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{int}{ sir@ _integrateSample_i_1_1}[r0x55 r0x56 ]
main.c(159:40:44:0:0)	 _iffalse_0($5) :
main.c(160:41:45:0:0)		iTemp16 [k24 lr41:42 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{char} = iTemp3 [k8 lr6:46 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{int}{ sir@ _integrateSample_q_1_1}[r0x57 r0x58 ] < 0x0 {literal int}
main.c(160:42:46:0:0)		if iTemp16 [k24 lr41:42 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{char} == 0 goto _iffalse_1($7)
main.c(160:43:49:0:0)		iTemp3 [k8 lr6:46 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{int}{ sir@ _integrateSample_q_1_1}[r0x57 r0x58 ] = - iTemp3 [k8 lr6:46 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{int}{ sir@ _integrateSample_q_1_1}[r0x57 r0x58 ]
main.c(160:45:51:0:0)	 _iffalse_1($7) :
main.c(161:46:52:0:0)		iTemp18 [k26 lr46:47 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int}[r0x55 r0x56 ] = iTemp2 [k6 lr5:46 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{int}{ sir@ _integrateSample_i_1_1}[r0x55 r0x56 ] + iTemp3 [k8 lr6:46 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{int}{ sir@ _integrateSample_q_1_1}[r0x57 r0x58 ]
main.c(161:47:53:0:0)		ret iTemp18 [k26 lr46:47 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int}[r0x55 r0x56 ]
main.c(161:48:54:0:0)	 _return($8) :
main.c(161:49:55:0:0)		eproc _integrateSample [k1 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int function    }

----------------------------------------------------------------
Basic Block _eBBlock0 : loop Depth = 0 noPath = 0 , lastinLoop = 0
depth 1st num 4 : bbnum = 3 1st iCode = 36 , last iCode = 37
visited 0 : hasFcall = 0

defines bitVector :bitvector Size = 58 bSize = 8
Bits on { (38) }

local defines bitVector :bitvector Size = 58 bSize = 8
Bits on { (38) }

pointers Set bitvector :
in pointers Set bitvector :
inDefs Set bitvector :bitvector Size = 58 bSize = 8
Bits on { (2) (3) (11) (12) (17) (18) (23) (24) (25) (29) (30) (31) (33) (34) }

outDefs Set bitvector :bitvector Size = 58 bSize = 8
Bits on { (2) (3) (11) (12) (17) (18) (23) (24) (25) (29) (30) (31) (33) (34) (38) }

usesDefs Set bitvector :bitvector Size = 58 bSize = 8
Bits on { (25) (38) }

----------------------------------------------------------------
main.c(159:36:38:0:0)		iTemp14 [k22 lr36:37 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{char} = iTemp2 [k6 lr5:46 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{int}{ sir@ _integrateSample_i_1_1}[r0x55 r0x56 ] < 0x0 {literal int}
main.c(159:37:39:0:0)		if iTemp14 [k22 lr36:37 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{char} == 0 goto _iffalse_0($5)
main.c(159:38:42:0:0)		iTemp2 [k6 lr5:46 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{int}{ sir@ _integrateSample_i_1_1}[r0x55 r0x56 ] = - iTemp2 [k6 lr5:46 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{int}{ sir@ _integrateSample_i_1_1}[r0x55 r0x56 ]
main.c(159:40:44:0:0)	 _iffalse_0($5) :
main.c(160:41:45:0:0)		iTemp16 [k24 lr41:42 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{char} = iTemp3 [k8 lr6:46 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{int}{ sir@ _integrateSample_q_1_1}[r0x57 r0x58 ] < 0x0 {literal int}
main.c(160:42:46:0:0)		if iTemp16 [k24 lr41:42 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{char} == 0 goto _iffalse_1($7)
main.c(160:43:49:0:0)		iTemp3 [k8 lr6:46 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{int}{ sir@ _integrateSample_q_1_1}[r0x57 r0x58 ] = - iTemp3 [k8 lr6:46 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{int}{ sir@ _integrateSample_q_1_1}[r0x57 r0x58 ]
main.c(160:45:51:0:0)	 _iffalse_1($7) :
main.c(161:46:52:0:0)		iTemp18 [k26 lr46:47 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int}[r0x55 r0x56 ] = iTemp2 [k6 lr5:46 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{int}{ sir@ _integrateSample_i_1_1}[r0x55 r0x56 ] + iTemp3 [k8 lr6:46 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{int}{ sir@ _integrateSample_q_1_1}[r0x57 r0x58 ]
main.c(161:47:53:0:0)		ret iTemp18 [k26 lr46:47 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int}[r0x55 r0x56 ]
main.c(161:48:54:0:0)	 _return($8) :
main.c(161:49:55:0:0)		eproc _integrateSample [k1 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int function    }

----------------------------------------------------------------
Basic Block _eBBlock1 : loop Depth = 0 noPath = 0 , lastinLoop = 0
depth 1st num 5 : bbnum = 4 1st iCode = 38 , last iCode = 39
visited 0 : hasFcall = 0

defines bitVector :bitvector Size = 58 bSize = 8
Bits on { (42) (43) }

local defines bitVector :bitvector Size = 58 bSize = 8
Bits on { (42) (43) }

pointers Set bitvector :
in pointers Set bitvector :
inDefs Set bitvector :bitvector Size = 58 bSize = 8
Bits on { (2) (3) (11) (12) (17) (18) (23) (24) (25) (29) (30) (31) (33) (34) (38) }

outDefs Set bitvector :bitvector Size = 58 bSize = 8
Bits on { (2) (3) (11) (12) (17) (18) (23) (24) (29) (30) (31) (33) (34) (38) (42) (43) }

usesDefs Set bitvector :bitvector Size = 58 bSize = 8
Bits on { (25) (42) }

----------------------------------------------------------------
main.c(159:38:42:0:0)		iTemp2 [k6 lr5:46 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{int}{ sir@ _integrateSample_i_1_1}[r0x55 r0x56 ] = - iTemp2 [k6 lr5:46 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{int}{ sir@ _integrateSample_i_1_1}[r0x55 r0x56 ]
main.c(159:40:44:0:0)	 _iffalse_0($5) :
main.c(160:41:45:0:0)		iTemp16 [k24 lr41:42 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{char} = iTemp3 [k8 lr6:46 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{int}{ sir@ _integrateSample_q_1_1}[r0x57 r0x58 ] < 0x0 {literal int}
main.c(160:42:46:0:0)		if iTemp16 [k24 lr41:42 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{char} == 0 goto _iffalse_1($7)
main.c(160:43:49:0:0)		iTemp3 [k8 lr6:46 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{int}{ sir@ _integrateSample_q_1_1}[r0x57 r0x58 ] = - iTemp3 [k8 lr6:46 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{int}{ sir@ _integrateSample_q_1_1}[r0x57 r0x58 ]
main.c(160:45:51:0:0)	 _iffalse_1($7) :
main.c(161:46:52:0:0)		iTemp18 [k26 lr46:47 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int}[r0x55 r0x56 ] = iTemp2 [k6 lr5:46 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{int}{ sir@ _integrateSample_i_1_1}[r0x55 r0x56 ] + iTemp3 [k8 lr6:46 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{int}{ sir@ _integrateSample_q_1_1}[r0x57 r0x58 ]
main.c(161:47:53:0:0)		ret iTemp18 [k26 lr46:47 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int}[r0x55 r0x56 ]
main.c(161:48:54:0:0)	 _return($8) :
main.c(161:49:55:0:0)		eproc _integrateSample [k1 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int function    }

----------------------------------------------------------------
Basic Block _iffalse_0 : loop Depth = 0 noPath = 0 , lastinLoop = 0
depth 1st num 6 : bbnum = 5 1st iCode = 40 , last iCode = 42
visited 0 : hasFcall = 0

defines bitVector :bitvector Size = 58 bSize = 8
Bits on { (45) }

local defines bitVector :bitvector Size = 58 bSize = 8
Bits on { (45) }

pointers Set bitvector :
in pointers Set bitvector :
inDefs Set bitvector :bitvector Size = 58 bSize = 8
Bits on { (2) (3) (11) (12) (17) (18) (23) (24) (25) (29) (30) (31) (33) (34) (38) (42) (43) }

outDefs Set bitvector :bitvector Size = 58 bSize = 8
Bits on { (2) (3) (11) (12) (17) (18) (23) (24) (25) (29) (30) (31) (33) (34) (38) (42) (43) (45) }

usesDefs Set bitvector :bitvector Size = 58 bSize = 8
Bits on { (31) (45) }

----------------------------------------------------------------
main.c(159:40:44:0:0)	 _iffalse_0($5) :
main.c(160:41:45:0:0)		iTemp16 [k24 lr41:42 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{char} = iTemp3 [k8 lr6:46 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{int}{ sir@ _integrateSample_q_1_1}[r0x57 r0x58 ] < 0x0 {literal int}
main.c(160:42:46:0:0)		if iTemp16 [k24 lr41:42 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{char} == 0 goto _iffalse_1($7)
main.c(160:43:49:0:0)		iTemp3 [k8 lr6:46 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{int}{ sir@ _integrateSample_q_1_1}[r0x57 r0x58 ] = - iTemp3 [k8 lr6:46 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{int}{ sir@ _integrateSample_q_1_1}[r0x57 r0x58 ]
main.c(160:45:51:0:0)	 _iffalse_1($7) :
main.c(161:46:52:0:0)		iTemp18 [k26 lr46:47 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int}[r0x55 r0x56 ] = iTemp2 [k6 lr5:46 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{int}{ sir@ _integrateSample_i_1_1}[r0x55 r0x56 ] + iTemp3 [k8 lr6:46 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{int}{ sir@ _integrateSample_q_1_1}[r0x57 r0x58 ]
main.c(161:47:53:0:0)		ret iTemp18 [k26 lr46:47 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int}[r0x55 r0x56 ]
main.c(161:48:54:0:0)	 _return($8) :
main.c(161:49:55:0:0)		eproc _integrateSample [k1 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int function    }

----------------------------------------------------------------
Basic Block _eBBlock2 : loop Depth = 0 noPath = 0 , lastinLoop = 0
depth 1st num 7 : bbnum = 6 1st iCode = 43 , last iCode = 44
visited 0 : hasFcall = 0

defines bitVector :bitvector Size = 58 bSize = 8
Bits on { (49) (50) }

local defines bitVector :bitvector Size = 58 bSize = 8
Bits on { (49) (50) }

pointers Set bitvector :
in pointers Set bitvector :
inDefs Set bitvector :bitvector Size = 58 bSize = 8
Bits on { (2) (3) (11) (12) (17) (18) (23) (24) (25) (29) (30) (31) (33) (34) (38) (42) (43) (45) }

outDefs Set bitvector :bitvector Size = 58 bSize = 8
Bits on { (2) (3) (11) (12) (17) (18) (23) (24) (25) (29) (30) (33) (34) (38) (42) (43) (45) (49) (50) }

usesDefs Set bitvector :bitvector Size = 58 bSize = 8
Bits on { (31) (49) }

----------------------------------------------------------------
main.c(160:43:49:0:0)		iTemp3 [k8 lr6:46 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{int}{ sir@ _integrateSample_q_1_1}[r0x57 r0x58 ] = - iTemp3 [k8 lr6:46 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{int}{ sir@ _integrateSample_q_1_1}[r0x57 r0x58 ]
main.c(160:45:51:0:0)	 _iffalse_1($7) :
main.c(161:46:52:0:0)		iTemp18 [k26 lr46:47 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int}[r0x55 r0x56 ] = iTemp2 [k6 lr5:46 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{int}{ sir@ _integrateSample_i_1_1}[r0x55 r0x56 ] + iTemp3 [k8 lr6:46 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{int}{ sir@ _integrateSample_q_1_1}[r0x57 r0x58 ]
main.c(161:47:53:0:0)		ret iTemp18 [k26 lr46:47 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int}[r0x55 r0x56 ]
main.c(161:48:54:0:0)	 _return($8) :
main.c(161:49:55:0:0)		eproc _integrateSample [k1 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int function    }

----------------------------------------------------------------
Basic Block _iffalse_1 : loop Depth = 0 noPath = 0 , lastinLoop = 0
depth 1st num 8 : bbnum = 7 1st iCode = 45 , last iCode = 47
visited 0 : hasFcall = 0

defines bitVector :bitvector Size = 58 bSize = 8
Bits on { (52) }

local defines bitVector :bitvector Size = 58 bSize = 8
Bits on { (52) }

pointers Set bitvector :
in pointers Set bitvector :
inDefs Set bitvector :bitvector Size = 58 bSize = 8
Bits on { (2) (3) (11) (12) (17) (18) (23) (24) (25) (29) (30) (31) (33) (34) (38) (42) (43) (45) (49) (50) }

outDefs Set bitvector :bitvector Size = 58 bSize = 8
Bits on { (2) (3) (11) (12) (17) (18) (23) (24) (25) (29) (30) (31) (33) (34) (38) (42) (43) (45) (49) (50) (52) }

usesDefs Set bitvector :bitvector Size = 58 bSize = 8
Bits on { (25) (31) (43) (50) (52) }

----------------------------------------------------------------
main.c(160:45:51:0:0)	 _iffalse_1($7) :
main.c(161:46:52:0:0)		iTemp18 [k26 lr46:47 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int}[r0x55 r0x56 ] = iTemp2 [k6 lr5:46 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{int}{ sir@ _integrateSample_i_1_1}[r0x55 r0x56 ] + iTemp3 [k8 lr6:46 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{int}{ sir@ _integrateSample_q_1_1}[r0x57 r0x58 ]
main.c(161:47:53:0:0)		ret iTemp18 [k26 lr46:47 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int}[r0x55 r0x56 ]
main.c(161:48:54:0:0)	 _return($8) :
main.c(161:49:55:0:0)		eproc _integrateSample [k1 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int function    }

----------------------------------------------------------------
Basic Block _return : loop Depth = 0 noPath = 0 , lastinLoop = 0
depth 1st num 9 : bbnum = 8 1st iCode = 48 , last iCode = 49
visited 0 : hasFcall = 0

defines bitVector :bitvector Size = 58 bSize = 8
Bits on { }

local defines bitVector :
pointers Set bitvector :
in pointers Set bitvector :
inDefs Set bitvector :bitvector Size = 58 bSize = 8
Bits on { (2) (3) (11) (12) (17) (18) (23) (24) (25) (29) (30) (31) (33) (34) (38) (42) (43) (45) (49) (50) (52) }

outDefs Set bitvector :bitvector Size = 58 bSize = 8
Bits on { (2) (3) (11) (12) (17) (18) (23) (24) (25) (29) (30) (31) (33) (34) (38) (42) (43) (45) (49) (50) (52) }

usesDefs Set bitvector :bitvector Size = 58 bSize = 8
Bits on { }

----------------------------------------------------------------
main.c(161:48:54:0:0)	 _return($8) :
main.c(161:49:55:0:0)		eproc _integrateSample [k1 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int function    }
typeRegWithIdx - requesting index = 0x53
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
Found a Stack Register!
typeRegWithIdx - requesting index = 0x54
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x55
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x56
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x57
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x58
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x59
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x5a
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x1000
typeRegWithIdx - requesting index = 0x1000
typeRegWithIdx - requesting index = 0x1000
Found a Stack Register!
typeRegWithIdx - requesting index = 0x54
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
Found a Stack Register!
typeRegWithIdx - requesting index = 0x53
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x5c
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
Found a Stack Register!
typeRegWithIdx - requesting index = 0x5b
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x5b
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x55
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x5c
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x5c
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x56
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x1000
typeRegWithIdx - requesting index = 0x1000
typeRegWithIdx - requesting index = 0x1000
Found a Stack Register!
typeRegWithIdx - requesting index = 0x54
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
Found a Stack Register!
typeRegWithIdx - requesting index = 0x53
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x5e
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
Found a Stack Register!
typeRegWithIdx - requesting index = 0x5d
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x5d
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x57
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x5e
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x5e
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x58
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x1000
typeRegWithIdx - requesting index = 0x1000
typeRegWithIdx - requesting index = 0x1000
Found a Stack Register!
typeRegWithIdx - requesting index = 0x54
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
Found a Stack Register!
typeRegWithIdx - requesting index = 0x53
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x60
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
Found a Stack Register!
typeRegWithIdx - requesting index = 0x5f
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x5f
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x55
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x60
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x60
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x56
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x1000
typeRegWithIdx - requesting index = 0x1000
typeRegWithIdx - requesting index = 0x1000
Found a Stack Register!
typeRegWithIdx - requesting index = 0x54
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
Found a Stack Register!
typeRegWithIdx - requesting index = 0x53
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x62
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
Found a Stack Register!
typeRegWithIdx - requesting index = 0x61
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x61
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x57
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x62
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x62
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x58
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x59
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x5a
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x59
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x5a
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x56
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x55
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x56
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x55
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x56
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x58
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x57
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x58
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x57
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x58
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x57
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x55
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x58
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x58
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x56
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x55
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
Found a Stack Register!
typeRegWithIdx - requesting index = 0x56
Found a Dynamic Register!
leaving
<><><><><><><><><><><><><><><><><>
<><><><><><><><><><><><><><><><><>
starting	ralloc.c:pic14_assignRegisters
ebbs before optimizing:

----------------------------------------------------------------
Basic Block _entry : loop Depth = 0 noPath = 0 , lastinLoop = 0
depth 1st num 1 : bbnum = 0 1st iCode = 1 , last iCode = 13
visited 0 : hasFcall = 0

defines bitVector :bitvector Size = 153 bSize = 20
Bits on { (2) (3) (4) (5) (6) (7) (8) (9) (10) (11) }

local defines bitVector :bitvector Size = 153 bSize = 20
Bits on { (2) (3) (4) (5) (6) (7) (8) (9) (10) (11) }

pointers Set bitvector :
in pointers Set bitvector :
inDefs Set bitvector :
outDefs Set bitvector :bitvector Size = 153 bSize = 20
Bits on { (2) (3) (4) (5) (6) (7) (8) (9) (10) (11) }

usesDefs Set bitvector :bitvector Size = 153 bSize = 20
Bits on { (2) (3) (4) (5) (7) (8) (11) }

----------------------------------------------------------------
main.c(165:1:0:0:0)	 _entry($28) :
main.c(165:2:1:0:0)		proc _sensorSample [k1 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int function    }
main.c(165:3:2:0:0)	iTemp0 [k2 lr3:66 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sensorSample_transmitter_1_1} = recv 
main.c(165:4:3:0:0)	iTemp1 [k4 lr4:11 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sensorSample_receiver_1_1} = recv 
main.c(165:5:4:0:0)	iTemp2 [k6 lr5:33 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sensorSample_phase_1_1} = recv 
main.c(171:6:5:0:0)		iTemp3 [k9 lr6:7 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{unsigned char} = _PORTB [k8 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{sfr volatile unsigned char} & 0xf {literal unsigned char}
main.c(171:7:6:0:0)		_PORTB [k8 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{sfr volatile unsigned char} := iTemp3 [k9 lr6:7 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{unsigned char}
main.c(172:8:7:0:0)		iTemp4 [k11 lr8:9 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{unsigned char} = ~ iTemp0 [k2 lr3:66 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sensorSample_transmitter_1_1}
main.c(172:9:8:0:0)		iTemp5 [k12 lr9:10 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{unsigned char} = iTemp4 [k11 lr8:9 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{unsigned char} & 0xf0 {literal unsigned char}
main.c(172:10:9:0:0)		_TRISB [k10 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{sfr volatile unsigned char} := iTemp5 [k12 lr9:10 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{unsigned char}
main.c(173:11:10:0:0)		_ADCON0 [k13 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{sfr volatile unsigned char} := iTemp1 [k4 lr4:11 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sensorSample_receiver_1_1}
main.c(176:12:11:0:0)		iTemp6 [k14 lr12:13 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{char} = iTemp2 [k6 lr5:33 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sensorSample_phase_1_1} == 0x0 {literal unsigned char}
main.c(176:13:12:0:0)		if iTemp6 [k14 lr12:13 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{char} == 0 goto _iffalse_2($2)

----------------------------------------------------------------
Basic Block _eBBlock0 : loop Depth = 0 noPath = 0 , lastinLoop = 0
depth 1st num 2 : bbnum = 1 1st iCode = 14 , last iCode = 15
visited 0 : hasFcall = 0

defines bitVector :bitvector Size = 153 bSize = 20
Bits on { (15) }

local defines bitVector :bitvector Size = 153 bSize = 20
Bits on { (15) }

pointers Set bitvector :
in pointers Set bitvector :
inDefs Set bitvector :bitvector Size = 153 bSize = 20
Bits on { (2) (3) (4) (5) (6) (7) (8) (9) (10) (11) }

outDefs Set bitvector :bitvector Size = 153 bSize = 20
Bits on { (2) (3) (4) (5) (6) (7) (8) (9) (10) (11) (15) }

usesDefs Set bitvector :bitvector Size = 153 bSize = 20
Bits on { }

----------------------------------------------------------------
main.c(176:14:15:0:0)		_sensorSample_phase0_1_1 [k15 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{bit bit {0,1}} := 0x1 {literal bit {0,1}}
main.c(176:15:16:0:0)		 goto _ifend_2($3)

----------------------------------------------------------------
Basic Block _iffalse_2 : loop Depth = 0 noPath = 0 , lastinLoop = 0
depth 1st num 3 : bbnum = 2 1st iCode = 16 , last iCode = 17
visited 0 : hasFcall = 0

defines bitVector :bitvector Size = 153 bSize = 20
Bits on { (18) }

local defines bitVector :bitvector Size = 153 bSize = 20
Bits on { (18) }

pointers Set bitvector :
in pointers Set bitvector :
inDefs Set bitvector :bitvector Size = 153 bSize = 20
Bits on { (2) (3) (4) (5) (6) (7) (8) (9) (10) (11) }

outDefs Set bitvector :bitvector Size = 153 bSize = 20
Bits on { (2) (3) (4) (5) (6) (7) (8) (9) (10) (11) (18) }

usesDefs Set bitvector :bitvector Size = 153 bSize = 20
Bits on { }

----------------------------------------------------------------
main.c(176:16:17:0:0)	 _iffalse_2($2) :
main.c(176:17:18:0:0)		_sensorSample_phase0_1_1 [k15 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{bit bit {0,1}} := 0x0 {literal bit {0,1}}

----------------------------------------------------------------
Basic Block _ifend_2 : loop Depth = 0 noPath = 0 , lastinLoop = 0
depth 1st num 4 : bbnum = 3 1st iCode = 18 , last iCode = 20
visited 0 : hasFcall = 0

defines bitVector :bitvector Size = 153 bSize = 20
Bits on { (20) }

local defines bitVector :bitvector Size = 153 bSize = 20
Bits on { (20) }

pointers Set bitvector :
in pointers Set bitvector :
inDefs Set bitvector :bitvector Size = 153 bSize = 20
Bits on { (2) (3) (4) (5) (6) (7) (8) (9) (10) (11) (15) (18) }

outDefs Set bitvector :bitvector Size = 153 bSize = 20
Bits on { (2) (3) (4) (5) (6) (7) (8) (9) (10) (11) (15) (18) (20) }

usesDefs Set bitvector :bitvector Size = 153 bSize = 20
Bits on { (4) (20) }

----------------------------------------------------------------
main.c(176:18:19:0:0)	 _ifend_2($3) :
main.c(177:19:20:0:0)		iTemp7 [k16 lr19:20 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{char} = iTemp2 [k6 lr5:33 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sensorSample_phase_1_1} == 0x1 {literal unsigned char}
main.c(177:20:21:0:0)		if iTemp7 [k16 lr19:20 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{char} == 0 goto _iffalse_3($5)

----------------------------------------------------------------
Basic Block _eBBlock1 : loop Depth = 0 noPath = 0 , lastinLoop = 0
depth 1st num 5 : bbnum = 4 1st iCode = 21 , last iCode = 22
visited 0 : hasFcall = 0

defines bitVector :bitvector Size = 153 bSize = 20
Bits on { (24) }

local defines bitVector :bitvector Size = 153 bSize = 20
Bits on { (24) }

pointers Set bitvector :
in pointers Set bitvector :
inDefs Set bitvector :bitvector Size = 153 bSize = 20
Bits on { (2) (3) (4) (5) (6) (7) (8) (9) (10) (11) (15) (18) (20) }

outDefs Set bitvector :bitvector Size = 153 bSize = 20
Bits on { (2) (3) (4) (5) (6) (7) (8) (9) (10) (11) (15) (18) (20) (24) }

usesDefs Set bitvector :bitvector Size = 153 bSize = 20
Bits on { }

----------------------------------------------------------------
main.c(177:21:24:0:0)		_sensorSample_phase1_1_1 [k17 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{bit bit {0,1}} := 0x1 {literal bit {0,1}}
main.c(177:22:25:0:0)		 goto _ifend_3($6)

----------------------------------------------------------------
Basic Block _iffalse_3 : loop Depth = 0 noPath = 0 , lastinLoop = 0
depth 1st num 6 : bbnum = 5 1st iCode = 23 , last iCode = 24
visited 0 : hasFcall = 0

defines bitVector :bitvector Size = 153 bSize = 20
Bits on { (27) }

local defines bitVector :bitvector Size = 153 bSize = 20
Bits on { (27) }

pointers Set bitvector :
in pointers Set bitvector :
inDefs Set bitvector :bitvector Size = 153 bSize = 20
Bits on { (2) (3) (4) (5) (6) (7) (8) (9) (10) (11) (15) (18) (20) }

outDefs Set bitvector :bitvector Size = 153 bSize = 20
Bits on { (2) (3) (4) (5) (6) (7) (8) (9) (10) (11) (15) (18) (20) (27) }

usesDefs Set bitvector :bitvector Size = 153 bSize = 20
Bits on { }

----------------------------------------------------------------
main.c(177:23:26:0:0)	 _iffalse_3($5) :
main.c(177:24:27:0:0)		_sensorSample_phase1_1_1 [k17 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{bit bit {0,1}} := 0x0 {literal bit {0,1}}

----------------------------------------------------------------
Basic Block _ifend_3 : loop Depth = 0 noPath = 0 , lastinLoop = 0
depth 1st num 7 : bbnum = 6 1st iCode = 25 , last iCode = 27
visited 0 : hasFcall = 0

defines bitVector :bitvector Size = 153 bSize = 20
Bits on { (29) }

local defines bitVector :bitvector Size = 153 bSize = 20
Bits on { (29) }

pointers Set bitvector :
in pointers Set bitvector :
inDefs Set bitvector :bitvector Size = 153 bSize = 20
Bits on { (2) (3) (4) (5) (6) (7) (8) (9) (10) (11) (15) (18) (20) (24) (27) }

outDefs Set bitvector :bitvector Size = 153 bSize = 20
Bits on { (2) (3) (4) (5) (6) (7) (8) (9) (10) (11) (15) (18) (20) (24) (27) (29) }

usesDefs Set bitvector :bitvector Size = 153 bSize = 20
Bits on { (4) (29) }

----------------------------------------------------------------
main.c(177:25:28:0:0)	 _ifend_3($6) :
main.c(178:26:29:0:0)		iTemp8 [k18 lr26:27 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{char} = iTemp2 [k6 lr5:33 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sensorSample_phase_1_1} == 0x2 {literal unsigned char}
main.c(178:27:30:0:0)		if iTemp8 [k18 lr26:27 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{char} == 0 goto _iffalse_4($8)

----------------------------------------------------------------
Basic Block _eBBlock2 : loop Depth = 0 noPath = 0 , lastinLoop = 0
depth 1st num 8 : bbnum = 7 1st iCode = 28 , last iCode = 29
visited 0 : hasFcall = 0

defines bitVector :bitvector Size = 153 bSize = 20
Bits on { (33) }

local defines bitVector :bitvector Size = 153 bSize = 20
Bits on { (33) }

pointers Set bitvector :
in pointers Set bitvector :
inDefs Set bitvector :bitvector Size = 153 bSize = 20
Bits on { (2) (3) (4) (5) (6) (7) (8) (9) (10) (11) (15) (18) (20) (24) (27) (29) }

outDefs Set bitvector :bitvector Size = 153 bSize = 20
Bits on { (2) (3) (4) (5) (6) (7) (8) (9) (10) (11) (15) (18) (20) (24) (27) (29) (33) }

usesDefs Set bitvector :bitvector Size = 153 bSize = 20
Bits on { }

----------------------------------------------------------------
main.c(178:28:33:0:0)		_sensorSample_phase2_1_1 [k19 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{bit bit {0,1}} := 0x1 {literal bit {0,1}}
main.c(178:29:34:0:0)		 goto _ifend_4($9)

----------------------------------------------------------------
Basic Block _iffalse_4 : loop Depth = 0 noPath = 0 , lastinLoop = 0
depth 1st num 9 : bbnum = 8 1st iCode = 30 , last iCode = 31
visited 0 : hasFcall = 0

defines bitVector :bitvector Size = 153 bSize = 20
Bits on { (36) }

local defines bitVector :bitvector Size = 153 bSize = 20
Bits on { (36) }

pointers Set bitvector :
in pointers Set bitvector :
inDefs Set bitvector :bitvector Size = 153 bSize = 20
Bits on { (2) (3) (4) (5) (6) (7) (8) (9) (10) (11) (15) (18) (20) (24) (27) (29) }

outDefs Set bitvector :bitvector Size = 153 bSize = 20
Bits on { (2) (3) (4) (5) (6) (7) (8) (9) (10) (11) (15) (18) (20) (24) (27) (29) (36) }

usesDefs Set bitvector :bitvector Size = 153 bSize = 20
Bits on { }

----------------------------------------------------------------
main.c(178:30:35:0:0)	 _iffalse_4($8) :
main.c(178:31:36:0:0)		_sensorSample_phase2_1_1 [k19 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{bit bit {0,1}} := 0x0 {literal bit {0,1}}

----------------------------------------------------------------
Basic Block _ifend_4 : loop Depth = 0 noPath = 0 , lastinLoop = 0
depth 1st num 10 : bbnum = 9 1st iCode = 32 , last iCode = 34
visited 0 : hasFcall = 0

defines bitVector :bitvector Size = 153 bSize = 20
Bits on { (38) }

local defines bitVector :bitvector Size = 153 bSize = 20
Bits on { (38) }

pointers Set bitvector :
in pointers Set bitvector :
inDefs Set bitvector :bitvector Size = 153 bSize = 20
Bits on { (2) (3) (4) (5) (6) (7) (8) (9) (10) (11) (15) (18) (20) (24) (27) (29) (33) (36) }

outDefs Set bitvector :bitvector Size = 153 bSize = 20
Bits on { (2) (3) (4) (5) (6) (7) (8) (9) (10) (11) (15) (18) (20) (24) (27) (29) (33) (36) (38) }

usesDefs Set bitvector :bitvector Size = 153 bSize = 20
Bits on { (4) (38) }

----------------------------------------------------------------
main.c(178:32:37:0:0)	 _ifend_4($9) :
main.c(179:33:38:0:0)		iTemp9 [k20 lr33:34 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{char} = iTemp2 [k6 lr5:33 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sensorSample_phase_1_1} == 0x3 {literal unsigned char}
main.c(179:34:39:0:0)		if iTemp9 [k20 lr33:34 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{char} == 0 goto _iffalse_5($11)

----------------------------------------------------------------
Basic Block _eBBlock3 : loop Depth = 0 noPath = 0 , lastinLoop = 0
depth 1st num 11 : bbnum = 10 1st iCode = 35 , last iCode = 36
visited 0 : hasFcall = 0

defines bitVector :bitvector Size = 153 bSize = 20
Bits on { (42) }

local defines bitVector :bitvector Size = 153 bSize = 20
Bits on { (42) }

pointers Set bitvector :
in pointers Set bitvector :
inDefs Set bitvector :bitvector Size = 153 bSize = 20
Bits on { (2) (3) (4) (5) (6) (7) (8) (9) (10) (11) (15) (18) (20) (24) (27) (29) (33) (36) (38) }

outDefs Set bitvector :bitvector Size = 153 bSize = 20
Bits on { (2) (3) (4) (5) (6) (7) (8) (9) (10) (11) (15) (18) (20) (24) (27) (29) (33) (36) (38) (42) }

usesDefs Set bitvector :bitvector Size = 153 bSize = 20
Bits on { }

----------------------------------------------------------------
main.c(179:35:42:0:0)		_sensorSample_phase3_1_1 [k21 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{bit bit {0,1}} := 0x1 {literal bit {0,1}}
main.c(179:36:43:0:0)		 goto _ifend_5($12)

----------------------------------------------------------------
Basic Block _iffalse_5 : loop Depth = 0 noPath = 0 , lastinLoop = 0
depth 1st num 12 : bbnum = 11 1st iCode = 37 , last iCode = 38
visited 0 : hasFcall = 0

defines bitVector :bitvector Size = 153 bSize = 20
Bits on { (45) }

local defines bitVector :bitvector Size = 153 bSize = 20
Bits on { (45) }

pointers Set bitvector :
in pointers Set bitvector :
inDefs Set bitvector :bitvector Size = 153 bSize = 20
Bits on { (2) (3) (4) (5) (6) (7) (8) (9) (10) (11) (15) (18) (20) (24) (27) (29) (33) (36) (38) }

outDefs Set bitvector :bitvector Size = 153 bSize = 20
Bits on { (2) (3) (4) (5) (6) (7) (8) (9) (10) (11) (15) (18) (20) (24) (27) (29) (33) (36) (38) (45) }

usesDefs Set bitvector :bitvector Size = 153 bSize = 20
Bits on { }

----------------------------------------------------------------
main.c(179:37:44:0:0)	 _iffalse_5($11) :
main.c(179:38:45:0:0)		_sensorSample_phase3_1_1 [k21 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{bit bit {0,1}} := 0x0 {literal bit {0,1}}

----------------------------------------------------------------
Basic Block _ifend_5 : loop Depth = 0 noPath = 0 , lastinLoop = 0
depth 1st num 13 : bbnum = 12 1st iCode = 39 , last iCode = 39
visited 0 : hasFcall = 0

defines bitVector :bitvector Size = 153 bSize = 20
Bits on { }

local defines bitVector :bitvector Size = 153 bSize = 20
Bits on { (47) }

pointers Set bitvector :
in pointers Set bitvector :
inDefs Set bitvector :bitvector Size = 153 bSize = 20
Bits on { (2) (3) (4) (5) (6) (7) (8) (9) (10) (11) (15) (18) (20) (24) (27) (29) (33) (36) (38) (42) (45) }

outDefs Set bitvector :bitvector Size = 153 bSize = 20
Bits on { (2) (3) (4) (5) (6) (7) (8) (9) (10) (11) (15) (18) (20) (24) (27) (29) (33) (36) (38) (42) (45) }

usesDefs Set bitvector :bitvector Size = 153 bSize = 20
Bits on { }

----------------------------------------------------------------
main.c(179:39:46:0:0)	 _ifend_5($12) :

----------------------------------------------------------------
Basic Block preHeaderLbl0 : loop Depth = 0 noPath = 0 , lastinLoop = 0
depth 1st num 14 : bbnum = 13 1st iCode = 40 , last iCode = 41
visited 0 : hasFcall = 0

defines bitVector :bitvector Size = 153 bSize = 20
Bits on { (152) }

local defines bitVector :bitvector Size = 153 bSize = 20
Bits on { (152) }

pointers Set bitvector :
in pointers Set bitvector :
inDefs Set bitvector :bitvector Size = 153 bSize = 20
Bits on { (2) (3) (4) (5) (6) (7) (8) (9) (10) (11) (15) (18) (20) (24) (27) (29) (33) (36) (38) (42) (45) (47) }

outDefs Set bitvector :bitvector Size = 153 bSize = 20
Bits on { (2) (3) (4) (5) (6) (7) (8) (9) (10) (11) (15) (18) (20) (24) (27) (29) (33) (36) (38) (42) (45) (47) (152) }

usesDefs Set bitvector :bitvector Size = 153 bSize = 20
Bits on { }

----------------------------------------------------------------
main.c(186:40:150:0:0)	 preHeaderLbl0($33) :
main.c(186:41:152:0:0)		iTemp12 [k25 lr41:66 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sensorSample_halfcycles_1_1} := 0x3 {literal unsigned char}

----------------------------------------------------------------
Basic Block _dobody_1 : loop Depth = 1 noPath = 0 , lastinLoop = 1
depth 1st num 15 : bbnum = 14 1st iCode = 42 , last iCode = 66
visited 0 : hasFcall = 0

defines bitVector :bitvector Size = 153 bSize = 20
Bits on { (49) (50) (72) }

local defines bitVector :bitvector Size = 153 bSize = 20
Bits on { (49) (50) (72) (73) }

pointers Set bitvector :
in pointers Set bitvector :
inDefs Set bitvector :bitvector Size = 153 bSize = 20
Bits on { (2) (3) (4) (5) (6) (7) (8) (9) (10) (11) (15) (18) (20) (24) (27) (29) (33) (36) (38) (42) (45) (47) (49) (50) (72) (73) (152) }

outDefs Set bitvector :bitvector Size = 153 bSize = 20
Bits on { (2) (3) (4) (5) (7) (8) (9) (10) (11) (15) (18) (20) (24) (27) (29) (33) (36) (38) (42) (45) (49) (50) (72) }

usesDefs Set bitvector :bitvector Size = 153 bSize = 20
Bits on { (2) (6) (49) (50) (72) (152) }

----------------------------------------------------------------
main.c(186:42:48:1:0)	 _dobody_1($13) :
main.c(187:43:49:1:0)		iTemp11 [k24 lr43:44 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{unsigned char} = _PORTB [k8 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{sfr volatile unsigned char} ^ iTemp0 [k2 lr3:66 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sensorSample_transmitter_1_1}
main.c(187:44:50:1:0)		_PORTB [k8 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{sfr volatile unsigned char} := iTemp11 [k24 lr43:44 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{unsigned char}
main.c(188:45:51:1:0)	 nop main.c(188:46:52:1:0)	 nop main.c(188:47:53:1:0)	 nop main.c(189:48:54:1:0)	 nop main.c(189:49:55:1:0)	 nop main.c(189:50:56:1:0)	 nop main.c(189:51:57:1:0)	 nop main.c(189:52:58:1:0)	 nop main.c(190:53:59:1:0)	 nop main.c(190:54:60:1:0)	 nop main.c(190:55:61:1:0)	 nop main.c(190:56:62:1:0)	 nop main.c(190:57:63:1:0)	 nop main.c(191:58:64:1:0)	 nop main.c(191:59:65:1:0)	 nop main.c(191:60:66:1:0)	 nop main.c(191:61:67:1:0)	 nop main.c(191:62:68:1:0)	 nop main.c(192:63:69:1:0)	 nop main.c(192:64:70:1:0)	 nop main.c(193:65:72:1:0)		iTemp12 [k25 lr41:66 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sensorSample_halfcycles_1_1} = iTemp12 [k25 lr41:66 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sensorSample_halfcycles_1_1} - 0x1 {literal unsigned char}
main.c(193:66:74:1:0)		if iTemp12 [k25 lr41:66 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sensorSample_halfcycles_1_1} != 0 goto _dobody_1($13)

----------------------------------------------------------------
Basic Block _eBBlock4 : loop Depth = 0 noPath = 0 , lastinLoop = 0
depth 1st num 16 : bbnum = 15 1st iCode = 67 , last iCode = 67
visited 0 : hasFcall = 0

defines bitVector :bitvector Size = 153 bSize = 20
Bits on { }

local defines bitVector :
pointers Set bitvector :
in pointers Set bitvector :
inDefs Set bitvector :bitvector Size = 153 bSize = 20
Bits on { (2) (3) (4) (5) (7) (8) (9) (10) (11) (15) (18) (20) (24) (27) (29) (33) (36) (38) (42) (45) (49) (50) (72) (73) }

outDefs Set bitvector :bitvector Size = 153 bSize = 20
Bits on { (2) (3) (4) (5) (7) (8) (9) (10) (11) (15) (18) (20) (24) (27) (29) (33) (36) (38) (42) (45) (49) (50) (72) (73) }

usesDefs Set bitvector :bitvector Size = 153 bSize = 20
Bits on { (15) (18) }

----------------------------------------------------------------
main.c(196:67:77:0:0)		if _sensorSample_phase0_1_1 [k15 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{bit bit {0,1}} == 0 goto _iffalse_6($17)

----------------------------------------------------------------
Basic Block _eBBlock5 : loop Depth = 0 noPath = 0 , lastinLoop = 0
depth 1st num 17 : bbnum = 16 1st iCode = 68 , last iCode = 68
visited 0 : hasFcall = 0

defines bitVector :bitvector Size = 153 bSize = 20
Bits on { (81) }

local defines bitVector :bitvector Size = 153 bSize = 20
Bits on { (81) }

pointers Set bitvector :
in pointers Set bitvector :
inDefs Set bitvector :bitvector Size = 153 bSize = 20
Bits on { (2) (3) (4) (5) (7) (8) (9) (10) (11) (15) (18) (20) (24) (27) (29) (33) (36) (38) (42) (45) (49) (50) (72) (73) }

outDefs Set bitvector :bitvector Size = 153 bSize = 20
Bits on { (2) (3) (4) (5) (7) (8) (9) (10) (11) (15) (18) (20) (24) (27) (29) (33) (36) (38) (42) (45) (49) (50) (72) (73) (81) }

usesDefs Set bitvector :bitvector Size = 153 bSize = 20
Bits on { }

----------------------------------------------------------------
main.c(197:68:81:0:0)		_GO_DONE [k26 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{bit bit {0,1}} := 0x1 {literal bit {0,1}}

----------------------------------------------------------------
Basic Block _iffalse_6 : loop Depth = 0 noPath = 0 , lastinLoop = 0
depth 1st num 18 : bbnum = 17 1st iCode = 69 , last iCode = 80
visited 0 : hasFcall = 0

defines bitVector :bitvector Size = 153 bSize = 20
Bits on { }

local defines bitVector :
pointers Set bitvector :
in pointers Set bitvector :
inDefs Set bitvector :bitvector Size = 153 bSize = 20
Bits on { (2) (3) (4) (5) (7) (8) (9) (10) (11) (15) (18) (20) (24) (27) (29) (33) (36) (38) (42) (45) (49) (50) (72) (73) (81) }

outDefs Set bitvector :bitvector Size = 153 bSize = 20
Bits on { (2) (3) (4) (5) (7) (8) (9) (10) (11) (15) (18) (20) (24) (27) (29) (33) (36) (38) (42) (45) (49) (50) (72) (73) (81) }

usesDefs Set bitvector :bitvector Size = 153 bSize = 20
Bits on { (24) (27) }

----------------------------------------------------------------
main.c(197:69:82:0:0)	 _iffalse_6($17) :
main.c(198:70:83:0:0)	 nop main.c(198:71:84:0:0)	 nop main.c(198:72:85:0:0)	 nop main.c(198:73:86:0:0)	 nop main.c(198:74:87:0:0)	 nop main.c(199:75:88:0:0)	 nop main.c(199:76:89:0:0)	 nop main.c(199:77:90:0:0)	 nop main.c(199:78:91:0:0)	 nop main.c(199:79:92:0:0)	 nop main.c(200:80:93:0:0)		if _sensorSample_phase1_1_1 [k17 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{bit bit {0,1}} == 0 goto _iffalse_7($19)

----------------------------------------------------------------
Basic Block _eBBlock6 : loop Depth = 0 noPath = 0 , lastinLoop = 0
depth 1st num 19 : bbnum = 18 1st iCode = 81 , last iCode = 81
visited 0 : hasFcall = 0

defines bitVector :bitvector Size = 153 bSize = 20
Bits on { (97) }

local defines bitVector :bitvector Size = 153 bSize = 20
Bits on { (97) }

pointers Set bitvector :
in pointers Set bitvector :
inDefs Set bitvector :bitvector Size = 153 bSize = 20
Bits on { (2) (3) (4) (5) (7) (8) (9) (10) (11) (15) (18) (20) (24) (27) (29) (33) (36) (38) (42) (45) (49) (50) (72) (73) (81) }

outDefs Set bitvector :bitvector Size = 153 bSize = 20
Bits on { (2) (3) (4) (5) (7) (8) (9) (10) (11) (15) (18) (20) (24) (27) (29) (33) (36) (38) (42) (45) (49) (50) (72) (73) (97) }

usesDefs Set bitvector :bitvector Size = 153 bSize = 20
Bits on { }

----------------------------------------------------------------
main.c(201:81:97:0:0)		_GO_DONE [k26 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{bit bit {0,1}} := 0x1 {literal bit {0,1}}

----------------------------------------------------------------
Basic Block _iffalse_7 : loop Depth = 0 noPath = 0 , lastinLoop = 0
depth 1st num 20 : bbnum = 19 1st iCode = 82 , last iCode = 94
visited 0 : hasFcall = 0

defines bitVector :bitvector Size = 153 bSize = 20
Bits on { }

local defines bitVector :
pointers Set bitvector :
in pointers Set bitvector :
inDefs Set bitvector :bitvector Size = 153 bSize = 20
Bits on { (2) (3) (4) (5) (7) (8) (9) (10) (11) (15) (18) (20) (24) (27) (29) (33) (36) (38) (42) (45) (49) (50) (72) (73) (81) (97) }

outDefs Set bitvector :bitvector Size = 153 bSize = 20
Bits on { (2) (3) (4) (5) (7) (8) (9) (10) (11) (15) (18) (20) (24) (27) (29) (33) (36) (38) (42) (45) (49) (50) (72) (73) (81) (97) }

usesDefs Set bitvector :bitvector Size = 153 bSize = 20
Bits on { (33) (36) }

----------------------------------------------------------------
main.c(201:82:98:0:0)	 _iffalse_7($19) :
main.c(202:83:99:0:0)	 nop main.c(202:84:100:0:0)	 nop main.c(202:85:101:0:0)	 nop main.c(202:86:102:0:0)	 nop main.c(202:87:103:0:0)	 nop main.c(203:88:104:0:0)	 nop main.c(203:89:105:0:0)	 nop main.c(203:90:106:0:0)	 nop main.c(203:91:107:0:0)	 nop main.c(203:92:108:0:0)	 nop main.c(203:93:109:0:0)	 nop main.c(204:94:110:0:0)		if _sensorSample_phase2_1_1 [k19 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{bit bit {0,1}} == 0 goto _iffalse_8($21)

----------------------------------------------------------------
Basic Block _eBBlock7 : loop Depth = 0 noPath = 0 , lastinLoop = 0
depth 1st num 21 : bbnum = 20 1st iCode = 95 , last iCode = 95
visited 0 : hasFcall = 0

defines bitVector :bitvector Size = 153 bSize = 20
Bits on { (114) }

local defines bitVector :bitvector Size = 153 bSize = 20
Bits on { (114) }

pointers Set bitvector :
in pointers Set bitvector :
inDefs Set bitvector :bitvector Size = 153 bSize = 20
Bits on { (2) (3) (4) (5) (7) (8) (9) (10) (11) (15) (18) (20) (24) (27) (29) (33) (36) (38) (42) (45) (49) (50) (72) (73) (81) (97) }

outDefs Set bitvector :bitvector Size = 153 bSize = 20
Bits on { (2) (3) (4) (5) (7) (8) (9) (10) (11) (15) (18) (20) (24) (27) (29) (33) (36) (38) (42) (45) (49) (50) (72) (73) (114) }

usesDefs Set bitvector :bitvector Size = 153 bSize = 20
Bits on { }

----------------------------------------------------------------
main.c(205:95:114:0:0)		_GO_DONE [k26 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{bit bit {0,1}} := 0x1 {literal bit {0,1}}

----------------------------------------------------------------
Basic Block _iffalse_8 : loop Depth = 0 noPath = 0 , lastinLoop = 0
depth 1st num 22 : bbnum = 21 1st iCode = 96 , last iCode = 107
visited 0 : hasFcall = 0

defines bitVector :bitvector Size = 153 bSize = 20
Bits on { }

local defines bitVector :
pointers Set bitvector :
in pointers Set bitvector :
inDefs Set bitvector :bitvector Size = 153 bSize = 20
Bits on { (2) (3) (4) (5) (7) (8) (9) (10) (11) (15) (18) (20) (24) (27) (29) (33) (36) (38) (42) (45) (49) (50) (72) (73) (81) (97) (114) }

outDefs Set bitvector :bitvector Size = 153 bSize = 20
Bits on { (2) (3) (4) (5) (7) (8) (9) (10) (11) (15) (18) (20) (24) (27) (29) (33) (36) (38) (42) (45) (49) (50) (72) (73) (81) (97) (114) }

usesDefs Set bitvector :bitvector Size = 153 bSize = 20
Bits on { (42) (45) }

----------------------------------------------------------------
main.c(205:96:115:0:0)	 _iffalse_8($21) :
main.c(206:97:116:0:0)	 nop main.c(206:98:117:0:0)	 nop main.c(206:99:118:0:0)	 nop main.c(206:100:119:0:0)	 nop main.c(206:101:120:0:0)	 nop main.c(207:102:121:0:0)	 nop main.c(207:103:122:0:0)	 nop main.c(207:104:123:0:0)	 nop main.c(207:105:124:0:0)	 nop main.c(207:106:125:0:0)	 nop main.c(208:107:126:0:0)		if _sensorSample_phase3_1_1 [k21 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{bit bit {0,1}} == 0 goto preHeaderLbl1($39)

----------------------------------------------------------------
Basic Block _eBBlock8 : loop Depth = 0 noPath = 0 , lastinLoop = 0
depth 1st num 23 : bbnum = 22 1st iCode = 108 , last iCode = 108
visited 0 : hasFcall = 0

defines bitVector :bitvector Size = 153 bSize = 20
Bits on { (130) }

local defines bitVector :bitvector Size = 153 bSize = 20
Bits on { (130) }

pointers Set bitvector :
in pointers Set bitvector :
inDefs Set bitvector :bitvector Size = 153 bSize = 20
Bits on { (2) (3) (4) (5) (7) (8) (9) (10) (11) (15) (18) (20) (24) (27) (29) (33) (36) (38) (42) (45) (49) (50) (72) (73) (81) (97) (114) }

outDefs Set bitvector :bitvector Size = 153 bSize = 20
Bits on { (2) (3) (4) (5) (7) (8) (9) (10) (11) (15) (18) (20) (24) (27) (29) (33) (36) (38) (42) (45) (49) (50) (72) (73) (130) }

usesDefs Set bitvector :bitvector Size = 153 bSize = 20
Bits on { }

----------------------------------------------------------------
main.c(209:108:130:0:0)		_GO_DONE [k26 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{bit bit {0,1}} := 0x1 {literal bit {0,1}}

----------------------------------------------------------------
Basic Block preHeaderLbl1 : loop Depth = 0 noPath = 0 , lastinLoop = 0
depth 1st num 24 : bbnum = 23 1st iCode = 109 , last iCode = 109
visited 0 : hasFcall = 0

defines bitVector :bitvector Size = 153 bSize = 20
Bits on { }

local defines bitVector :
pointers Set bitvector :
in pointers Set bitvector :
inDefs Set bitvector :bitvector Size = 153 bSize = 20
Bits on { (2) (3) (4) (5) (7) (8) (9) (10) (11) (15) (18) (20) (24) (27) (29) (33) (36) (38) (42) (45) (49) (50) (72) (73) (81) (97) (114) (130) }

outDefs Set bitvector :bitvector Size = 153 bSize = 20
Bits on { (2) (3) (4) (5) (7) (8) (9) (10) (11) (15) (18) (20) (24) (27) (29) (33) (36) (38) (42) (45) (49) (50) (72) (73) (81) (97) (114) (130) }

usesDefs Set bitvector :bitvector Size = 153 bSize = 20
Bits on { }

----------------------------------------------------------------
main.c(212:109:151:0:0)	 preHeaderLbl1($39) :

----------------------------------------------------------------
Basic Block _whilecontinue_1 : loop Depth = 1 noPath = 0 , lastinLoop = 0
depth 1st num 25 : bbnum = 24 1st iCode = 110 , last iCode = 111
visited 0 : hasFcall = 0

defines bitVector :bitvector Size = 153 bSize = 20
Bits on { }

local defines bitVector :
pointers Set bitvector :
in pointers Set bitvector :
inDefs Set bitvector :bitvector Size = 153 bSize = 20
Bits on { (2) (3) (4) (5) (7) (8) (9) (10) (11) (15) (18) (20) (24) (27) (29) (33) (36) (38) (42) (45) (49) (50) (72) (73) (80) (81) (96) (97) (113) (114) (129) (130) }

outDefs Set bitvector :bitvector Size = 153 bSize = 20
Bits on { (2) (3) (4) (5) (7) (8) (9) (10) (11) (15) (18) (20) (24) (27) (29) (33) (36) (38) (42) (45) (49) (50) (72) (73) (80) (81) (96) (97) (113) (114) (129) (130) }

usesDefs Set bitvector :bitvector Size = 153 bSize = 20
Bits on { }

----------------------------------------------------------------
main.c(212:110:132:1:0)	 _whilecontinue_1($24) :
main.c(212:111:133:1:0)		if _NOT_DONE [k31 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{bit bit {0,1}} != 0 goto _whilecontinue_1($24)

----------------------------------------------------------------
Basic Block _eBBlock9 : loop Depth = 0 noPath = 0 , lastinLoop = 0
depth 1st num 26 : bbnum = 25 1st iCode = 112 , last iCode = 119
visited 0 : hasFcall = 0

defines bitVector :bitvector Size = 153 bSize = 20
Bits on { (138) (139) (140) (141) (142) (144) (145) }

local defines bitVector :bitvector Size = 153 bSize = 20
Bits on { (138) (139) (140) (141) (142) (144) (145) }

pointers Set bitvector :
in pointers Set bitvector :
inDefs Set bitvector :bitvector Size = 153 bSize = 20
Bits on { (2) (3) (4) (5) (7) (8) (9) (10) (11) (15) (18) (20) (24) (27) (29) (33) (36) (38) (42) (45) (49) (50) (72) (73) (80) (81) (96) (97) (113) (114) (129) (130) }

outDefs Set bitvector :bitvector Size = 153 bSize = 20
Bits on { (2) (3) (4) (5) (7) (8) (10) (11) (15) (18) (20) (24) (27) (29) (33) (36) (38) (42) (45) (49) (50) (72) (73) (80) (81) (96) (97) (113) (114) (129) (130) (138) (139) (140) (141) (142) (144) (145) }

usesDefs Set bitvector :bitvector Size = 153 bSize = 20
Bits on { (139) (140) (141) (142) (144) (145) }

----------------------------------------------------------------
main.c(215:112:138:0:0)		_TRISB [k10 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{sfr volatile unsigned char} := 0xf0 {literal unsigned char}
main.c(217:113:139:0:0)		iTemp17 [k32 lr113:115 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sensorSample_h_1_1} := _ADRESH [k34 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{sfr volatile unsigned char}
main.c(218:114:140:0:0)		iTemp18 [k35 lr114:117 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sensorSample_l_1_1} := _ADRESL [k37 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{sfr volatile unsigned char}
main.c(219:115:141:0:0)		iTemp19 [k38 lr115:116 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int} = (int)iTemp17 [k32 lr113:115 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sensorSample_h_1_1}
main.c(219:116:142:0:0)		iTemp20 [k39 lr116:118 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int} = iTemp19 [k38 lr115:116 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int} << 0x8 {literal unsigned char}
main.c(219:117:144:0:0)		iTemp22 [k41 lr117:118 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{unsigned int} = (unsigned int)iTemp18 [k35 lr114:117 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sensorSample_l_1_1}
main.c(219:118:145:0:0)		iTemp23 [k42 lr118:119 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{unsigned int} = iTemp20 [k39 lr116:118 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int} | iTemp22 [k41 lr117:118 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{unsigned int}
main.c(219:119:147:0:0)		ret iTemp23 [k42 lr118:119 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{unsigned int}

----------------------------------------------------------------
Basic Block _return : loop Depth = 0 noPath = 0 , lastinLoop = 0
depth 1st num 27 : bbnum = 26 1st iCode = 120 , last iCode = 121
visited 0 : hasFcall = 0

defines bitVector :bitvector Size = 153 bSize = 20
Bits on { }

local defines bitVector :
pointers Set bitvector :
in pointers Set bitvector :
inDefs Set bitvector :bitvector Size = 153 bSize = 20
Bits on { (2) (3) (4) (5) (7) (8) (10) (11) (15) (18) (20) (24) (27) (29) (33) (36) (38) (42) (45) (49) (50) (72) (73) (80) (81) (96) (97) (113) (114) (129) (130) (138) (139) (140) (141) (142) (144) (145) }

outDefs Set bitvector :bitvector Size = 153 bSize = 20
Bits on { (2) (3) (4) (5) (7) (8) (10) (11) (15) (18) (20) (24) (27) (29) (33) (36) (38) (42) (45) (49) (50) (72) (73) (80) (81) (96) (97) (113) (114) (129) (130) (138) (139) (140) (141) (142) (144) (145) }

usesDefs Set bitvector :bitvector Size = 153 bSize = 20
Bits on { }

----------------------------------------------------------------
main.c(219:120:148:0:0)	 _return($27) :
main.c(219:121:149:0:0)		eproc _sensorSample [k1 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int function    }
packRegisters
packRegsForAssign
  result:_PORTB [k8 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{sfr volatile unsigned char}
  left:
  right:iTemp3 [k9 lr6:7 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{unsigned char}
allocDirReg symbol name _PORTB
  637  storage class 3 
 642  integral
 646  specifier
_PORTB [k8 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{sfr volatile unsigned char}
  -- added _PORTB to hash, size = 1
  -- and it is at a fixed address 0x06
  2695 - result is not temp
  2760 - dic result key == ic right key -- pointer set=N
  packing. removing iTemp3
  replacing with iTemp3
  result:_PORTB [k8 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{sfr volatile unsigned char}
  left:
  right:iTemp3 [k9 lr6:7 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{unsigned char}
packRegsForAssign
  result:_TRISB [k10 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{sfr volatile unsigned char}
  left:
  right:iTemp5 [k12 lr9:10 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{unsigned char}
allocDirReg symbol name _TRISB
  637  storage class 3 
 642  integral
 646  specifier
_TRISB [k10 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{sfr volatile unsigned char}
  -- added _TRISB to hash, size = 1
  -- and it is at a fixed address 0x86
  2695 - result is not temp
  2760 - dic result key == ic right key -- pointer set=N
  packing. removing iTemp5
  replacing with iTemp5
  result:_TRISB [k10 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{sfr volatile unsigned char}
  left:
  right:iTemp5 [k12 lr9:10 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{unsigned char}
packRegsForAssign
  result:_ADCON0 [k13 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{sfr volatile unsigned char}
  left:
  right:iTemp1 [k4 lr4:11 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sensorSample_receiver_1_1}
allocDirReg symbol name _ADCON0
  637  storage class 3 
 642  integral
 646  specifier
_ADCON0 [k13 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{sfr volatile unsigned char}
  -- added _ADCON0 to hash, size = 1
  -- and it is at a fixed address 0x1f
  2695 - result is not temp
  2750 - dic is VOLATILE 
  result:_ADCON0 [k13 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{sfr volatile unsigned char}
  left:
  right:iTemp1 [k4 lr4:11 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sensorSample_receiver_1_1}
packRegsForAssign
  result:_ADCON0 [k13 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{sfr volatile unsigned char}
  left:
  right:iTemp1 [k4 lr4:11 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sensorSample_receiver_1_1}
allocDirReg symbol name _ADCON0
  637  storage class 3 
 642  integral
 646  specifier
_ADCON0 [k13 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{sfr volatile unsigned char}
  -- added _ADCON0 to hash, size = 1
  -- and it is at a fixed address 0x1f
  2695 - result is not temp
  2750 - dic is VOLATILE 
  result:_ADCON0 [k13 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{sfr volatile unsigned char}
  left:
  right:iTemp1 [k4 lr4:11 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sensorSample_receiver_1_1}
  left:_sensorSample [k1 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int function    }
        Symbol type: int function    
  result:iTemp0 [k2 lr3:66 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sensorSample_transmitter_1_1}
          Symbol type: unsigned char
  3594 - pointer reg req = 0
packForReceive
  result:iTemp0 [k2 lr3:66 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sensorSample_transmitter_1_1}
  left:
  right:
    used on left
  result:iTemp1 [k4 lr4:11 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sensorSample_receiver_1_1}
          Symbol type: unsigned char
  3594 - pointer reg req = 0
packForReceive
  result:iTemp1 [k4 lr4:11 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sensorSample_receiver_1_1}
  left:
  right:
    used on right
  hey we can remove this unnecessary assign
  result:iTemp2 [k6 lr5:33 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sensorSample_phase_1_1}
          Symbol type: unsigned char
  3594 - pointer reg req = 0
packForReceive
  result:iTemp2 [k6 lr5:33 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sensorSample_phase_1_1}
  left:
  right:
    used on left
  left:_PORTB [k8 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{sfr volatile unsigned char}
    is volatile
sfr         Symbol type: sfr volatile unsigned char
  result:_PORTB [k8 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{sfr volatile unsigned char}
          Symbol type: sfr volatile unsigned char
  3594 - pointer reg req = 0
isBitwiseOptimizable
  left:iTemp0 [k2 lr3:66 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sensorSample_transmitter_1_1}
        Symbol type: unsigned char
  result:iTemp4 [k11 lr8:9 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{unsigned char}
          Symbol type: unsigned char
  3594 - pointer reg req = 0
  left:iTemp4 [k11 lr8:9 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{unsigned char}
        Symbol type: unsigned char
  result:_TRISB [k10 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{sfr volatile unsigned char}
          Symbol type: sfr volatile unsigned char
  3594 - pointer reg req = 0
isBitwiseOptimizable
  right:iTemp1 [k4 lr4:11 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sensorSample_receiver_1_1}
         Symbol type: unsigned char
  result:_ADCON0 [k13 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{sfr volatile unsigned char}
          Symbol type: sfr volatile unsigned char
  3594 - pointer reg req = 0
  left:iTemp2 [k6 lr5:33 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sensorSample_phase_1_1}
        Symbol type: unsigned char
  result:iTemp6 [k14 lr12:13 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{char}
          Symbol type: char
  3594 - pointer reg req = 0
  3611
  left:iTemp6 [k14 lr12:13 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{char}
        Symbol type: char
  3594 - pointer reg req = 0
packRegisters
packRegsForAssign
  result:_sensorSample_phase0_1_1 [k15 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{bit bit {0,1}}
  left:
  right:0x1 {literal bit {0,1}}
allocDirReg symbol name _sensorSample_phase0_1_1
  637  storage class 13 
 642  integral
 646  specifier
_sensorSample_phase0_1_1 [k15 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{bit bit {0,1}}
  -- added _sensorSample_phase0_1_1 to hash, size = 1
  2695 - result is not temp
  2705 - not packing - right is not temp
allocDirReg BAD, op is NULL
  result:_sensorSample_phase0_1_1 [k15 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{bit bit {0,1}}
  left:
  right:0x1 {literal bit {0,1}}
  result:_sensorSample_phase0_1_1 [k15 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{bit bit {0,1}}
          Symbol type: bit bit {0,1}
  3594 - pointer reg req = 0
packRegisters
packRegsForAssign
  result:_sensorSample_phase0_1_1 [k15 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{bit bit {0,1}}
  left:
  right:0x0 {literal bit {0,1}}
allocDirReg symbol name _sensorSample_phase0_1_1
  637  storage class 13 
 642  integral
 646  specifier
_sensorSample_phase0_1_1 [k15 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{bit bit {0,1}}
  2695 - result is not temp
  2705 - not packing - right is not temp
allocDirReg BAD, op is NULL
  result:_sensorSample_phase0_1_1 [k15 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{bit bit {0,1}}
  left:
  right:0x0 {literal bit {0,1}}
  result:_sensorSample_phase0_1_1 [k15 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{bit bit {0,1}}
          Symbol type: bit bit {0,1}
  3594 - pointer reg req = 0
packRegisters
  left:iTemp2 [k6 lr5:33 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sensorSample_phase_1_1}
        Symbol type: unsigned char
  result:iTemp7 [k16 lr19:20 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{char}
          Symbol type: char
  3594 - pointer reg req = 0
  3611
  left:iTemp7 [k16 lr19:20 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{char}
        Symbol type: char
  3594 - pointer reg req = 0
packRegisters
packRegsForAssign
  result:_sensorSample_phase1_1_1 [k17 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{bit bit {0,1}}
  left:
  right:0x1 {literal bit {0,1}}
allocDirReg symbol name _sensorSample_phase1_1_1
  637  storage class 13 
 642  integral
 646  specifier
_sensorSample_phase1_1_1 [k17 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{bit bit {0,1}}
  -- added _sensorSample_phase1_1_1 to hash, size = 1
  2695 - result is not temp
  2705 - not packing - right is not temp
allocDirReg BAD, op is NULL
  result:_sensorSample_phase1_1_1 [k17 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{bit bit {0,1}}
  left:
  right:0x1 {literal bit {0,1}}
  result:_sensorSample_phase1_1_1 [k17 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{bit bit {0,1}}
          Symbol type: bit bit {0,1}
  3594 - pointer reg req = 0
packRegisters
packRegsForAssign
  result:_sensorSample_phase1_1_1 [k17 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{bit bit {0,1}}
  left:
  right:0x0 {literal bit {0,1}}
allocDirReg symbol name _sensorSample_phase1_1_1
  637  storage class 13 
 642  integral
 646  specifier
_sensorSample_phase1_1_1 [k17 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{bit bit {0,1}}
  2695 - result is not temp
  2705 - not packing - right is not temp
allocDirReg BAD, op is NULL
  result:_sensorSample_phase1_1_1 [k17 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{bit bit {0,1}}
  left:
  right:0x0 {literal bit {0,1}}
  result:_sensorSample_phase1_1_1 [k17 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{bit bit {0,1}}
          Symbol type: bit bit {0,1}
  3594 - pointer reg req = 0
packRegisters
  left:iTemp2 [k6 lr5:33 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sensorSample_phase_1_1}
        Symbol type: unsigned char
  result:iTemp8 [k18 lr26:27 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{char}
          Symbol type: char
  3594 - pointer reg req = 0
  3611
  left:iTemp8 [k18 lr26:27 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{char}
        Symbol type: char
  3594 - pointer reg req = 0
packRegisters
packRegsForAssign
  result:_sensorSample_phase2_1_1 [k19 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{bit bit {0,1}}
  left:
  right:0x1 {literal bit {0,1}}
allocDirReg symbol name _sensorSample_phase2_1_1
  637  storage class 13 
 642  integral
 646  specifier
_sensorSample_phase2_1_1 [k19 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{bit bit {0,1}}
  -- added _sensorSample_phase2_1_1 to hash, size = 1
  2695 - result is not temp
  2705 - not packing - right is not temp
allocDirReg BAD, op is NULL
  result:_sensorSample_phase2_1_1 [k19 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{bit bit {0,1}}
  left:
  right:0x1 {literal bit {0,1}}
  result:_sensorSample_phase2_1_1 [k19 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{bit bit {0,1}}
          Symbol type: bit bit {0,1}
  3594 - pointer reg req = 0
packRegisters
packRegsForAssign
  result:_sensorSample_phase2_1_1 [k19 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{bit bit {0,1}}
  left:
  right:0x0 {literal bit {0,1}}
allocDirReg symbol name _sensorSample_phase2_1_1
  637  storage class 13 
 642  integral
 646  specifier
_sensorSample_phase2_1_1 [k19 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{bit bit {0,1}}
  2695 - result is not temp
  2705 - not packing - right is not temp
allocDirReg BAD, op is NULL
  result:_sensorSample_phase2_1_1 [k19 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{bit bit {0,1}}
  left:
  right:0x0 {literal bit {0,1}}
  result:_sensorSample_phase2_1_1 [k19 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{bit bit {0,1}}
          Symbol type: bit bit {0,1}
  3594 - pointer reg req = 0
packRegisters
  left:iTemp2 [k6 lr5:33 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sensorSample_phase_1_1}
        Symbol type: unsigned char
  result:iTemp9 [k20 lr33:34 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{char}
          Symbol type: char
  3594 - pointer reg req = 0
  3611
  left:iTemp9 [k20 lr33:34 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{char}
        Symbol type: char
  3594 - pointer reg req = 0
packRegisters
packRegsForAssign
  result:_sensorSample_phase3_1_1 [k21 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{bit bit {0,1}}
  left:
  right:0x1 {literal bit {0,1}}
allocDirReg symbol name _sensorSample_phase3_1_1
  637  storage class 13 
 642  integral
 646  specifier
_sensorSample_phase3_1_1 [k21 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{bit bit {0,1}}
  -- added _sensorSample_phase3_1_1 to hash, size = 1
  2695 - result is not temp
  2705 - not packing - right is not temp
allocDirReg BAD, op is NULL
  result:_sensorSample_phase3_1_1 [k21 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{bit bit {0,1}}
  left:
  right:0x1 {literal bit {0,1}}
  result:_sensorSample_phase3_1_1 [k21 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{bit bit {0,1}}
          Symbol type: bit bit {0,1}
  3594 - pointer reg req = 0
packRegisters
packRegsForAssign
  result:_sensorSample_phase3_1_1 [k21 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{bit bit {0,1}}
  left:
  right:0x0 {literal bit {0,1}}
allocDirReg symbol name _sensorSample_phase3_1_1
  637  storage class 13 
 642  integral
 646  specifier
_sensorSample_phase3_1_1 [k21 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{bit bit {0,1}}
  2695 - result is not temp
  2705 - not packing - right is not temp
allocDirReg BAD, op is NULL
  result:_sensorSample_phase3_1_1 [k21 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{bit bit {0,1}}
  left:
  right:0x0 {literal bit {0,1}}
  result:_sensorSample_phase3_1_1 [k21 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{bit bit {0,1}}
          Symbol type: bit bit {0,1}
  3594 - pointer reg req = 0
packRegisters
packRegisters
packRegsForAssign
  result:iTemp12 [k25 lr41:66 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sensorSample_halfcycles_1_1}
  left:
  right:0x3 {literal unsigned char}
  2705 - not packing - right is not temp
allocDirReg BAD, op is NULL
  result:iTemp12 [k25 lr41:66 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sensorSample_halfcycles_1_1}
  left:
  right:0x3 {literal unsigned char}
  result:iTemp12 [k25 lr41:66 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sensorSample_halfcycles_1_1}
          Symbol type: unsigned char
  3594 - pointer reg req = 0
packRegisters
packRegsForAssign
  result:_PORTB [k8 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{sfr volatile unsigned char}
  left:
  right:iTemp11 [k24 lr43:44 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{unsigned char}
allocDirReg symbol name _PORTB
  637  storage class 3 
 642  integral
 646  specifier
_PORTB [k8 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{sfr volatile unsigned char}
  -- added _PORTB to hash, size = 1
  -- and it is at a fixed address 0x06
  2695 - result is not temp
  2760 - dic result key == ic right key -- pointer set=N
  packing. removing iTemp11
  replacing with iTemp11
  result:_PORTB [k8 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{sfr volatile unsigned char}
  left:
  right:iTemp11 [k24 lr43:44 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{unsigned char}
  left:_PORTB [k8 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{sfr volatile unsigned char}
    is volatile
sfr         Symbol type: sfr volatile unsigned char
  right:iTemp0 [k2 lr3:66 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sensorSample_transmitter_1_1}
         Symbol type: unsigned char
  result:_PORTB [k8 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{sfr volatile unsigned char}
          Symbol type: sfr volatile unsigned char
  3594 - pointer reg req = 0
isBitwiseOptimizable
  left:iTemp12 [k25 lr41:66 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sensorSample_halfcycles_1_1}
        Symbol type: unsigned char
  result:iTemp12 [k25 lr41:66 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sensorSample_halfcycles_1_1}
          Symbol type: unsigned char
  3594 - pointer reg req = 0
packRegsForSupport
packRegsForAccUse
  packRegsForAccUse:3181
  packRegsForAccUse:3188
  left:iTemp12 [k25 lr41:66 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sensorSample_halfcycles_1_1}
        Symbol type: unsigned char
  3594 - pointer reg req = 0
packRegisters
  left:_sensorSample_phase0_1_1 [k15 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{bit bit {0,1}}
bit         Symbol type: bit bit {0,1}
  3594 - pointer reg req = 0
packRegisters
packRegsForAssign
  result:_GO_DONE [k26 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{bit bit {0,1}}
  left:
  right:0x1 {literal bit {0,1}}
allocDirReg symbol name _GO_DONE
  637  storage class 13 
 642  integral
 646  specifier
_GO_DONE [k26 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{bit bit {0,1}}
  -- added _GO_DONE to hash, size = 1
  -- and it is at a fixed address 0xfa
  2695 - result is not temp
  2705 - not packing - right is not temp
allocDirReg BAD, op is NULL
  result:_GO_DONE [k26 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{bit bit {0,1}}
  left:
  right:0x1 {literal bit {0,1}}
  result:_GO_DONE [k26 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{bit bit {0,1}}
          Symbol type: bit bit {0,1}
  3594 - pointer reg req = 0
packRegisters
  left:_sensorSample_phase1_1_1 [k17 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{bit bit {0,1}}
bit         Symbol type: bit bit {0,1}
  3594 - pointer reg req = 0
packRegisters
packRegsForAssign
  result:_GO_DONE [k26 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{bit bit {0,1}}
  left:
  right:0x1 {literal bit {0,1}}
allocDirReg symbol name _GO_DONE
  637  storage class 13 
 642  integral
 646  specifier
_GO_DONE [k26 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{bit bit {0,1}}
  -- and it is at a fixed address 0xfa
  2695 - result is not temp
  2705 - not packing - right is not temp
allocDirReg BAD, op is NULL
  result:_GO_DONE [k26 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{bit bit {0,1}}
  left:
  right:0x1 {literal bit {0,1}}
  result:_GO_DONE [k26 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{bit bit {0,1}}
          Symbol type: bit bit {0,1}
  3594 - pointer reg req = 0
packRegisters
  left:_sensorSample_phase2_1_1 [k19 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{bit bit {0,1}}
bit         Symbol type: bit bit {0,1}
  3594 - pointer reg req = 0
packRegisters
packRegsForAssign
  result:_GO_DONE [k26 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{bit bit {0,1}}
  left:
  right:0x1 {literal bit {0,1}}
allocDirReg symbol name _GO_DONE
  637  storage class 13 
 642  integral
 646  specifier
_GO_DONE [k26 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{bit bit {0,1}}
  -- and it is at a fixed address 0xfa
  2695 - result is not temp
  2705 - not packing - right is not temp
allocDirReg BAD, op is NULL
  result:_GO_DONE [k26 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{bit bit {0,1}}
  left:
  right:0x1 {literal bit {0,1}}
  result:_GO_DONE [k26 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{bit bit {0,1}}
          Symbol type: bit bit {0,1}
  3594 - pointer reg req = 0
packRegisters
  left:_sensorSample_phase3_1_1 [k21 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{bit bit {0,1}}
bit         Symbol type: bit bit {0,1}
  3594 - pointer reg req = 0
packRegisters
packRegsForAssign
  result:_GO_DONE [k26 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{bit bit {0,1}}
  left:
  right:0x1 {literal bit {0,1}}
allocDirReg symbol name _GO_DONE
  637  storage class 13 
 642  integral
 646  specifier
_GO_DONE [k26 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{bit bit {0,1}}
  -- and it is at a fixed address 0xfa
  2695 - result is not temp
  2705 - not packing - right is not temp
allocDirReg BAD, op is NULL
  result:_GO_DONE [k26 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{bit bit {0,1}}
  left:
  right:0x1 {literal bit {0,1}}
  result:_GO_DONE [k26 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{bit bit {0,1}}
          Symbol type: bit bit {0,1}
  3594 - pointer reg req = 0
packRegisters
packRegisters
  left:_NOT_DONE [k31 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{bit bit {0,1}}
bit         Symbol type: bit bit {0,1}
  3594 - pointer reg req = 0
packRegisters
packRegsForAssign
  result:_TRISB [k10 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{sfr volatile unsigned char}
  left:
  right:0xf0 {literal unsigned char}
allocDirReg symbol name _TRISB
  637  storage class 3 
 642  integral
 646  specifier
_TRISB [k10 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{sfr volatile unsigned char}
  -- added _TRISB to hash, size = 1
  -- and it is at a fixed address 0x86
  2695 - result is not temp
  2705 - not packing - right is not temp
allocDirReg BAD, op is NULL
  result:_TRISB [k10 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{sfr volatile unsigned char}
  left:
  right:0xf0 {literal unsigned char}
packRegsForAssign
  result:iTemp17 [k32 lr113:115 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sensorSample_h_1_1}
  left:
  right:_ADRESH [k34 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{sfr volatile unsigned char}
  2705 - not packing - right is not temp
allocDirReg symbol name _ADRESH
  637  storage class 3 
 642  integral
 646  specifier
_ADRESH [k34 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{sfr volatile unsigned char}
  -- added _ADRESH to hash, size = 1
  -- and it is at a fixed address 0x1e
  result:iTemp17 [k32 lr113:115 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sensorSample_h_1_1}
  left:
  right:_ADRESH [k34 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{sfr volatile unsigned char}
packRegsForAssign
  result:iTemp18 [k35 lr114:117 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sensorSample_l_1_1}
  left:
  right:_ADRESL [k37 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{sfr volatile unsigned char}
  2705 - not packing - right is not temp
allocDirReg symbol name _ADRESL
  637  storage class 3 
 642  integral
 646  specifier
_ADRESL [k37 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{sfr volatile unsigned char}
  -- added _ADRESL to hash, size = 1
  -- and it is at a fixed address 0x9e
  result:iTemp18 [k35 lr114:117 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sensorSample_l_1_1}
  left:
  right:_ADRESL [k37 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{sfr volatile unsigned char}
  result:_TRISB [k10 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{sfr volatile unsigned char}
          Symbol type: sfr volatile unsigned char
  3594 - pointer reg req = 0
  right:_ADRESH [k34 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{sfr volatile unsigned char}
         Symbol type: sfr volatile unsigned char
  result:iTemp17 [k32 lr113:115 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sensorSample_h_1_1}
          Symbol type: unsigned char
  3594 - pointer reg req = 0
  right:_ADRESL [k37 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{sfr volatile unsigned char}
         Symbol type: sfr volatile unsigned char
  result:iTemp18 [k35 lr114:117 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sensorSample_l_1_1}
          Symbol type: unsigned char
  3594 - pointer reg req = 0
  right:iTemp17 [k32 lr113:115 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sensorSample_h_1_1}
         Symbol type: unsigned char
  result:iTemp19 [k38 lr115:116 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int}
          Symbol type: int
  3594 - pointer reg req = 0
  3662 - casting
packRegsForOneuse
  left:iTemp19 [k38 lr115:116 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int}
        Symbol type: int
  result:iTemp20 [k39 lr116:118 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int}
          Symbol type: int
  3594 - pointer reg req = 0
packRegsForAccUse
  packRegsForAccUse:3181
  packRegsForAccUse:3188
  right:iTemp18 [k35 lr114:117 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sensorSample_l_1_1}
         Symbol type: unsigned char
  result:iTemp22 [k41 lr117:118 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{unsigned int}
          Symbol type: unsigned int
  3594 - pointer reg req = 0
  3662 - casting
packRegsForOneuse
  left:iTemp20 [k39 lr116:118 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int}
        Symbol type: int
  right:iTemp22 [k41 lr117:118 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{unsigned int}
         Symbol type: unsigned int
  result:iTemp23 [k42 lr118:119 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{unsigned int}
          Symbol type: unsigned int
  3594 - pointer reg req = 0
isBitwiseOptimizable
packRegsForAccUse
  packRegsForAccUse:3181
  packRegsForAccUse:3188
  left:iTemp23 [k42 lr118:119 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{unsigned int}
        Symbol type: unsigned int
  3594 - pointer reg req = 0
packRegisters
  left:_sensorSample [k1 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int function    }
        Symbol type: int function    
dir registers allocated so far:
  -- #0 reg = s0xFFF  key 0, rIdx = 4095, size 1
  -- #1 reg = s0x1000  key 0, rIdx = 4096, size 1
  -- #2 reg = s0x1001  key 0, rIdx = 4097, size 1
  -- #3 reg = s0x1002  key 0, rIdx = 4098, size 1
  -- #4 reg = s0x1003  key 0, rIdx = 4099, size 1
  -- #5 reg = s0x1004  key 0, rIdx = 4100, size 1
  -- #6 reg = s0x1005  key 0, rIdx = 4101, size 1
  -- #7 reg = s0x1006  key 0, rIdx = 4102, size 1
  -- #8 reg = r0x20  key 0, rIdx = 32, size 1
  -- #9 reg = r0x21  key 0, rIdx = 33, size 1
  -- #10 reg = r0x22  key 0, rIdx = 34, size 1
  -- #11 reg = r0x23  key 0, rIdx = 35, size 1
  -- #12 reg = r0x24  key 0, rIdx = 36, size 1
  -- #13 reg = r0x25  key 0, rIdx = 37, size 1
  -- #14 reg = r0x26  key 0, rIdx = 38, size 1
  -- #15 reg = r0x27  key 0, rIdx = 39, size 1
  -- #16 reg = r0x28  key 0, rIdx = 40, size 1
  -- #17 reg = r0x29  key 0, rIdx = 41, size 1
  -- #18 reg = r0x2A  key 0, rIdx = 42, size 1
  -- #19 reg = r0x2B  key 0, rIdx = 43, size 1
  -- #20 reg = r0x2C  key 0, rIdx = 44, size 1
  -- #21 reg = r0x2D  key 0, rIdx = 45, size 1
  -- #22 reg = r0x2E  key 0, rIdx = 46, size 1
  -- #23 reg = r0x2F  key 0, rIdx = 47, size 1
  -- #24 reg = r0x30  key 0, rIdx = 48, size 1
  -- #25 reg = r0x31  key 0, rIdx = 49, size 1
  -- #26 reg = r0x32  key 0, rIdx = 50, size 1
  -- #27 reg = r0x33  key 0, rIdx = 51, size 1
  -- #28 reg = r0x34  key 0, rIdx = 52, size 1
  -- #29 reg = r0x35  key 0, rIdx = 53, size 1
  -- #30 reg = r0x36  key 0, rIdx = 54, size 1
  -- #31 reg = r0x37  key 0, rIdx = 55, size 1
  -- #32 reg = r0x38  key 0, rIdx = 56, size 1
  -- #33 reg = r0x39  key 0, rIdx = 57, size 1
  -- #34 reg = r0x3A  key 0, rIdx = 58, size 1
  -- #35 reg = r0x3B  key 0, rIdx = 59, size 1
  -- #36 reg = r0x3C  key 0, rIdx = 60, size 1
  -- #37 reg = r0x3D  key 0, rIdx = 61, size 1
  -- #38 reg = r0x3E  key 0, rIdx = 62, size 1
  -- #39 reg = r0x3F  key 0, rIdx = 63, size 1
  -- #40 reg = r0x40  key 0, rIdx = 64, size 1
  -- #41 reg = r0x41  key 0, rIdx = 65, size 1
  -- #42 reg = r0x42  key 0, rIdx = 66, size 1
  -- #43 reg = r0x43  key 0, rIdx = 67, size 1
  -- #44 reg = r0x44  key 0, rIdx = 68, size 1
  -- #45 reg = r0x45  key 0, rIdx = 69, size 1
  -- #46 reg = r0x46  key 0, rIdx = 70, size 1
  -- #47 reg = r0x47  key 0, rIdx = 71, size 1
  -- #48 reg = r0x48  key 0, rIdx = 72, size 1
  -- #49 reg = r0x49  key 0, rIdx = 73, size 1
  -- #50 reg = r0x4A  key 0, rIdx = 74, size 1
  -- #51 reg = r0x4B  key 0, rIdx = 75, size 1
  -- #52 reg = r0x4C  key 0, rIdx = 76, size 1
  -- #53 reg = r0x4D  key 0, rIdx = 77, size 1
  -- #54 reg = r0x4E  key 0, rIdx = 78, size 1
  -- #55 reg = r0x4F  key 0, rIdx = 79, size 1
  -- #56 reg = r0x50  key 0, rIdx = 80, size 1
  -- #57 reg = r0x51  key 0, rIdx = 81, size 1
  -- #58 reg = r0x52  key 0, rIdx = 82, size 1
  -- #59 reg = r0x53  key 0, rIdx = 83, size 1
  -- #60 reg = r0x54  key 0, rIdx = 84, size 1
  -- #61 reg = r0x55  key 0, rIdx = 85, size 1
  -- #62 reg = r0x56  key 0, rIdx = 86, size 1
  -- #63 reg = r0x57  key 0, rIdx = 87, size 1
  -- #64 reg = r0x58  key 0, rIdx = 88, size 1
  -- #65 reg = r0x59  key 0, rIdx = 89, size 1
  -- #66 reg = r0x5A  key 0, rIdx = 90, size 1
  -- #67 reg = r0x5B  key 0, rIdx = 91, size 1
  -- #68 reg = r0x5C  key 0, rIdx = 92, size 1
  -- #69 reg = r0x5D  key 0, rIdx = 93, size 1
  -- #70 reg = r0x5E  key 0, rIdx = 94, size 1
  -- #71 reg = r0x5F  key 0, rIdx = 95, size 1
  -- #72 reg = r0x60  key 0, rIdx = 96, size 1
  -- #73 reg = r0x61  key 0, rIdx = 97, size 1
  -- #74 reg = r0x62  key 0, rIdx = 98, size 1
  -- #75 reg = _ADRESH  key 0, rIdx = 33, size 1
  -- #76 reg = _ADRESL  key 5, rIdx = 34, size 1
  -- #77 reg = _PCLATH  key 6, rIdx = 10, size 1
  -- #78 reg = _TRISA  key 7, rIdx = 1, size 1
  -- #79 reg = _TRISB  key 8, rIdx = 2, size 1
  -- #80 reg = _TRISB  key 8, rIdx = 23, size 1
  -- #81 reg = _TRISB  key 8, rIdx = 32, size 1
  -- #82 reg = _TRISC  key 9, rIdx = 3, size 1
  -- #83 reg = _TRISD  key 10, rIdx = 4, size 1
  -- #84 reg = _PORTA  key 10, rIdx = 6, size 1
  -- #85 reg = _TRISE  key 11, rIdx = 5, size 1
  -- #86 reg = _PORTB  key 11, rIdx = 7, size 1
  -- #87 reg = _PORTB  key 11, rIdx = 22, size 1
  -- #88 reg = _PORTB  key 11, rIdx = 30, size 1
  -- #89 reg = _PORTC  key 12, rIdx = 8, size 1
  -- #90 reg = _GIE  key 12, rIdx = 13, size 1
  -- #91 reg = _PORTD  key 13, rIdx = 9, size 1
  -- #92 reg = _PORTE  key 14, rIdx = 10, size 1
  -- #93 reg = _sensorSample_phase0_1_1  key 15, rIdx = 26, size 1
  -- #94 reg = _sensorSample_phase1_1_1  key 16, rIdx = 27, size 1
  -- #95 reg = _sensorSample_phase2_1_1  key 17, rIdx = 28, size 1
  -- #96 reg = _sensorSample_phase3_1_1  key 18, rIdx = 29, size 1
  -- #97 reg = _ADCON0  key 27, rIdx = 12, size 1
  -- #98 reg = _ADCON0  key 27, rIdx = 24, size 1
  -- #99 reg = _ADCON0  key 27, rIdx = 25, size 1
  -- #100 reg = _ADCON1  key 28, rIdx = 11, size 1
  -- #101 reg = SSAVE  key 32, rIdx = 32765, size 1
  -- #102 reg = WSAVE  key 36, rIdx = 32766, size 1
  -- #103 reg = _reading0  key 44, rIdx = 14, size 2
  -- #104 reg = _GO_DONE  key 44, rIdx = 31, size 1
  -- #105 reg = _reading1  key 45, rIdx = 15, size 2
  -- #106 reg = _reading2  key 46, rIdx = 16, size 2
  -- #107 reg = INTCON  key 47, rIdx = 11, size 1
  -- #108 reg = _reading3  key 47, rIdx = 17, size 2
  -- #109 reg = _STATUS  key 48, rIdx = 3, size 1
  -- #110 reg = PCL  key 48, rIdx = 2, size 1
  -- #111 reg = _reading4  key 48, rIdx = 18, size 2
  -- #112 reg = KZ  key 49, rIdx = 32767, size 1
  -- #113 reg = _reading5  key 49, rIdx = 19, size 2
  -- #114 reg = _reading6  key 50, rIdx = 20, size 2
  -- #115 reg = _reading7  key 51, rIdx = 21, size 2
  -- #116 reg = INDF  key 56, rIdx = 0, size 1
  -- #117 reg = _Light0  key 56, rIdx = 0, size 1
  -- #118 reg = FSR  key 60, rIdx = 4, size 1
regTypeNum
  2421 - iTemp0
  2431 - itemp register
  reg name iTemp0,  reg type REG_GPR
  2421 - iTemp1
  2431 - itemp register
  reg name iTemp1,  reg type REG_GPR
  2421 - iTemp2
  2431 - itemp register
  reg name iTemp2,  reg type REG_GPR
  2421 - iTemp3
  2431 - itemp register
  reg name iTemp3,  reg type REG_GPR
  2421 - iTemp4
  2431 - itemp register
  reg name iTemp4,  reg type REG_GPR
  2421 - iTemp5
  2431 - itemp register
  reg name iTemp5,  reg type REG_GPR
  2421 - iTemp6
  2431 - itemp register
  2421 - iTemp7
  2431 - itemp register
  2421 - iTemp8
  2431 - itemp register
  2421 - iTemp9
  2431 - itemp register
  2421 - iTemp11
  2431 - itemp register
  reg name iTemp11,  reg type REG_GPR
  2421 - iTemp12
  2431 - itemp register
  reg name iTemp12,  reg type REG_GPR
  2421 - iTemp17
  2431 - itemp register
  reg name iTemp17,  reg type REG_GPR
  2421 - iTemp18
  2431 - itemp register
  reg name iTemp18,  reg type REG_GPR
  2421 - iTemp19
  2431 - itemp register
  reg name iTemp19,  reg type REG_GPR
  2421 - iTemp20
  2431 - itemp register
  reg name iTemp20,  reg type REG_GPR
  2421 - iTemp22
  2431 - itemp register
  reg name iTemp22,  reg type REG_GPR
  2421 - iTemp23
  2431 - itemp register
  reg name iTemp23,  reg type REG_GPR
serialRegAssign
  op: LABEL
deassignLRs
  op: unkown op 9 	
deassignLRs
  op: RECEIVE
deassignLRs
willCauseSpill
computeSpillable
When I get clever, I'll optimize the receive logic
  2125 - 
bitvector Size = 58 bSize = 8
Bits on { (2) }
getRegGpr
allocReg of type REG_GPR
  2141 - 
  2155 - 
  op: RECEIVE
deassignLRs
willCauseSpill
computeSpillable
When I get clever, I'll optimize the receive logic
  2125 - 
bitvector Size = 58 bSize = 8
Bits on { (2) (4) }
getRegGpr
allocReg of type REG_GPR
  2141 - 
  2155 - 
  op: RECEIVE
deassignLRs
willCauseSpill
computeSpillable
When I get clever, I'll optimize the receive logic
  2125 - 
bitvector Size = 58 bSize = 8
Bits on { (2) (4) (6) }
getRegGpr
allocReg of type REG_GPR
  2141 - 
  2155 - 
  op: BITWISEAND
deassignLRs
  op: ~
deassignLRs
willCauseSpill
computeSpillable
  2125 - 
bitvector Size = 58 bSize = 8
Bits on { (2) (4) (6) (11) }
getRegGpr
allocReg of type REG_GPR
  2141 - 
positionRegs
  2155 - 
  op: BITWISEAND
deassignLRs
freeReg
  op: =
deassignLRs
freeReg
  op: EQ_OP
deassignLRs
  op: IFX
deassignLRs
  op: =
deassignLRs
  op: GOTO
deassignLRs
  op: LABEL
deassignLRs
  op: =
deassignLRs
  op: LABEL
deassignLRs
  op: EQ_OP
deassignLRs
  op: IFX
deassignLRs
  op: =
deassignLRs
  op: GOTO
deassignLRs
  op: LABEL
deassignLRs
  op: =
deassignLRs
  op: LABEL
deassignLRs
  op: EQ_OP
deassignLRs
  op: IFX
deassignLRs
  op: =
deassignLRs
  op: GOTO
deassignLRs
  op: LABEL
deassignLRs
  op: =
deassignLRs
  op: LABEL
deassignLRs
  op: EQ_OP
deassignLRs
freeReg
  op: IFX
deassignLRs
  op: =
deassignLRs
  op: GOTO
deassignLRs
  op: LABEL
deassignLRs
  op: =
deassignLRs
  op: LABEL
deassignLRs
  op: LABEL
deassignLRs
  op: =
deassignLRs
willCauseSpill
computeSpillable
  2125 - 
bitvector Size = 58 bSize = 8
Bits on { (2) (25) }
getRegGpr
allocReg of type REG_GPR
  2141 - 
  2155 - 
  op: LABEL
deassignLRs
  op: ^
deassignLRs
  op: INLINEASM
deassignLRs
  op: INLINEASM
deassignLRs
  op: INLINEASM
deassignLRs
  op: INLINEASM
deassignLRs
  op: INLINEASM
deassignLRs
  op: INLINEASM
deassignLRs
  op: INLINEASM
deassignLRs
  op: INLINEASM
deassignLRs
  op: INLINEASM
deassignLRs
  op: INLINEASM
deassignLRs
  op: INLINEASM
deassignLRs
  op: INLINEASM
deassignLRs
  op: INLINEASM
deassignLRs
  op: INLINEASM
deassignLRs
  op: INLINEASM
deassignLRs
  op: INLINEASM
deassignLRs
  op: INLINEASM
deassignLRs
  op: INLINEASM
deassignLRs
  op: INLINEASM
deassignLRs
  op: INLINEASM
deassignLRs
  op: -
deassignLRs
  op: IFX
deassignLRs
freeReg
freeReg
  op: IFX
deassignLRs
  op: =
deassignLRs
  op: LABEL
deassignLRs
  op: INLINEASM
deassignLRs
  op: INLINEASM
deassignLRs
  op: INLINEASM
deassignLRs
  op: INLINEASM
deassignLRs
  op: INLINEASM
deassignLRs
  op: INLINEASM
deassignLRs
  op: INLINEASM
deassignLRs
  op: INLINEASM
deassignLRs
  op: INLINEASM
deassignLRs
  op: INLINEASM
deassignLRs
  op: IFX
deassignLRs
  op: =
deassignLRs
  op: LABEL
deassignLRs
  op: INLINEASM
deassignLRs
  op: INLINEASM
deassignLRs
  op: INLINEASM
deassignLRs
  op: INLINEASM
deassignLRs
  op: INLINEASM
deassignLRs
  op: INLINEASM
deassignLRs
  op: INLINEASM
deassignLRs
  op: INLINEASM
deassignLRs
  op: INLINEASM
deassignLRs
  op: INLINEASM
deassignLRs
  op: INLINEASM
deassignLRs
  op: IFX
deassignLRs
  op: =
deassignLRs
  op: LABEL
deassignLRs
  op: INLINEASM
deassignLRs
  op: INLINEASM
deassignLRs
  op: INLINEASM
deassignLRs
  op: INLINEASM
deassignLRs
  op: INLINEASM
deassignLRs
  op: INLINEASM
deassignLRs
  op: INLINEASM
deassignLRs
  op: INLINEASM
deassignLRs
  op: INLINEASM
deassignLRs
  op: INLINEASM
deassignLRs
  op: IFX
deassignLRs
  op: =
deassignLRs
  op: LABEL
deassignLRs
  op: LABEL
deassignLRs
  op: IFX
deassignLRs
  op: =
deassignLRs
  op: =
deassignLRs
willCauseSpill
computeSpillable
  2125 - 
bitvector Size = 58 bSize = 8
Bits on { (32) }
getRegGpr
allocReg of type REG_GPR
  2141 - 
  2155 - 
  op: =
deassignLRs
willCauseSpill
computeSpillable
  2125 - 
bitvector Size = 58 bSize = 8
Bits on { (32) (35) }
getRegGpr
allocReg of type REG_GPR
  2141 - 
  2155 - 
  op: CAST
deassignLRs
nfreeRegsType
getRegGpr
allocReg of type REG_GPR
  op: LEFT_OP
deassignLRs
nfreeRegsType
  op: CAST
deassignLRs
nfreeRegsType
getRegGpr
allocReg of type REG_GPR
  op: |
deassignLRs
nfreeRegsType
freeReg
freeReg
  op: RETURN
deassignLRs
freeReg
freeReg
  op: LABEL
deassignLRs
  op: ENDFUNCTION
deassignLRs
createRegMask
regsUsedIniCode
rUmaskForOp
regsUsedIniCode
rUmaskForOp
regsUsedIniCode
rUmaskForOp
regsUsedIniCode
rUmaskForOp
rUmaskForOp
rUmaskForOp
regsUsedIniCode
rUmaskForOp
rUmaskForOp
regsUsedIniCode
rUmaskForOp
rUmaskForOp
rUmaskForOp
regsUsedIniCode
rUmaskForOp
rUmaskForOp
regsUsedIniCode
rUmaskForOp
rUmaskForOp
rUmaskForOp
regsUsedIniCode
rUmaskForOp
regsUsedIniCode
rUmaskForOp
rUmaskForOp
regsUsedIniCode
rUmaskForOp
rUmaskForOp
regsUsedIniCode
rUmaskForOp
rUmaskForOp
rUmaskForOp
regsUsedIniCode
rUmaskForOp
regsUsedIniCode
rUmaskForOp
rUmaskForOp
regsUsedIniCode
rUmaskForOp
rUmaskForOp
regsUsedIniCode
rUmaskForOp
rUmaskForOp
rUmaskForOp
regsUsedIniCode
rUmaskForOp
regsUsedIniCode
rUmaskForOp
rUmaskForOp
regsUsedIniCode
rUmaskForOp
rUmaskForOp
regsUsedIniCode
rUmaskForOp
rUmaskForOp
rUmaskForOp
regsUsedIniCode
rUmaskForOp
regsUsedIniCode
rUmaskForOp
rUmaskForOp
regsUsedIniCode
rUmaskForOp
rUmaskForOp
regsUsedIniCode
rUmaskForOp
rUmaskForOp
regsUsedIniCode
rUmaskForOp
rUmaskForOp
rUmaskForOp
regsUsedIniCode
rUmaskForOp
rUmaskForOp
rUmaskForOp
regsUsedIniCode
rUmaskForOp
regsUsedIniCode
rUmaskForOp
regsUsedIniCode
rUmaskForOp
rUmaskForOp
regsUsedIniCode
rUmaskForOp
regsUsedIniCode
rUmaskForOp
rUmaskForOp
regsUsedIniCode
rUmaskForOp
regsUsedIniCode
rUmaskForOp
rUmaskForOp
regsUsedIniCode
rUmaskForOp
regsUsedIniCode
rUmaskForOp
rUmaskForOp
regsUsedIniCode
rUmaskForOp
regsUsedIniCode
rUmaskForOp
rUmaskForOp
regsUsedIniCode
rUmaskForOp
rUmaskForOp
regsUsedIniCode
rUmaskForOp
rUmaskForOp
regsUsedIniCode
rUmaskForOp
rUmaskForOp
rUmaskForOp
regsUsedIniCode
rUmaskForOp
rUmaskForOp
rUmaskForOp
regsUsedIniCode
rUmaskForOp
rUmaskForOp
rUmaskForOp
regsUsedIniCode
rUmaskForOp
rUmaskForOp
rUmaskForOp
regsUsedIniCode
rUmaskForOp
ebbs after optimizing:

----------------------------------------------------------------
Basic Block _entry : loop Depth = 0 noPath = 0 , lastinLoop = 0
depth 1st num 1 : bbnum = 0 1st iCode = 1 , last iCode = 13
visited 0 : hasFcall = 0

defines bitVector :bitvector Size = 153 bSize = 20
Bits on { (2) (3) (4) (5) (6) (7) (8) (9) (10) (11) }

local defines bitVector :bitvector Size = 153 bSize = 20
Bits on { (2) (3) (4) (5) (6) (7) (8) (9) (10) (11) }

pointers Set bitvector :
in pointers Set bitvector :
inDefs Set bitvector :
outDefs Set bitvector :bitvector Size = 153 bSize = 20
Bits on { (2) (3) (4) (5) (6) (7) (8) (9) (10) (11) }

usesDefs Set bitvector :bitvector Size = 153 bSize = 20
Bits on { (2) (3) (4) (5) (7) (8) (11) }

----------------------------------------------------------------
main.c(165:1:0:0:0)	 _entry($28) :
main.c(165:2:1:0:0)		proc _sensorSample [k1 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int function    }
main.c(165:3:2:0:0)	iTemp0 [k2 lr3:66 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sensorSample_transmitter_1_1}[r0x63 ] = recv 
main.c(165:4:3:0:0)	iTemp1 [k4 lr4:11 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sensorSample_receiver_1_1}[r0x64 ] = recv 
main.c(165:5:4:0:0)	iTemp2 [k6 lr5:33 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sensorSample_phase_1_1}[r0x65 ] = recv 
main.c(171:6:5:0:0)		_PORTB [k8 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{sfr volatile unsigned char} = _PORTB [k8 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{sfr volatile unsigned char} & 0xf {literal unsigned char}
main.c(172:8:7:0:0)		iTemp4 [k11 lr8:9 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{unsigned char}[r0x66 ] = ~ iTemp0 [k2 lr3:66 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sensorSample_transmitter_1_1}[r0x63 ]
main.c(172:9:8:0:0)		_TRISB [k10 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{sfr volatile unsigned char} = iTemp4 [k11 lr8:9 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{unsigned char}[r0x66 ] & 0xf0 {literal unsigned char}
main.c(173:11:10:0:0)		_ADCON0 [k13 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{sfr volatile unsigned char} := iTemp1 [k4 lr4:11 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sensorSample_receiver_1_1}[r0x64 ]
main.c(176:12:11:0:0)		iTemp6 [k14 lr12:13 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{char} = iTemp2 [k6 lr5:33 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sensorSample_phase_1_1}[r0x65 ] == 0x0 {literal unsigned char}
main.c(176:13:12:0:0)		if iTemp6 [k14 lr12:13 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{char} == 0 goto _iffalse_2($2)
main.c(176:14:15:0:0)		_sensorSample_phase0_1_1 [k15 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{bit bit {0,1}} := 0x1 {literal bit {0,1}}
main.c(176:15:16:0:0)		 goto _ifend_2($3)
main.c(176:16:17:0:0)	 _iffalse_2($2) :
main.c(176:17:18:0:0)		_sensorSample_phase0_1_1 [k15 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{bit bit {0,1}} := 0x0 {literal bit {0,1}}
main.c(176:18:19:0:0)	 _ifend_2($3) :
main.c(177:19:20:0:0)		iTemp7 [k16 lr19:20 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{char} = iTemp2 [k6 lr5:33 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sensorSample_phase_1_1}[r0x65 ] == 0x1 {literal unsigned char}
main.c(177:20:21:0:0)		if iTemp7 [k16 lr19:20 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{char} == 0 goto _iffalse_3($5)
main.c(177:21:24:0:0)		_sensorSample_phase1_1_1 [k17 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{bit bit {0,1}} := 0x1 {literal bit {0,1}}
main.c(177:22:25:0:0)		 goto _ifend_3($6)
main.c(177:23:26:0:0)	 _iffalse_3($5) :
main.c(177:24:27:0:0)		_sensorSample_phase1_1_1 [k17 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{bit bit {0,1}} := 0x0 {literal bit {0,1}}
main.c(177:25:28:0:0)	 _ifend_3($6) :
main.c(178:26:29:0:0)		iTemp8 [k18 lr26:27 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{char} = iTemp2 [k6 lr5:33 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sensorSample_phase_1_1}[r0x65 ] == 0x2 {literal unsigned char}
main.c(178:27:30:0:0)		if iTemp8 [k18 lr26:27 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{char} == 0 goto _iffalse_4($8)
main.c(178:28:33:0:0)		_sensorSample_phase2_1_1 [k19 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{bit bit {0,1}} := 0x1 {literal bit {0,1}}
main.c(178:29:34:0:0)		 goto _ifend_4($9)
main.c(178:30:35:0:0)	 _iffalse_4($8) :
main.c(178:31:36:0:0)		_sensorSample_phase2_1_1 [k19 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{bit bit {0,1}} := 0x0 {literal bit {0,1}}
main.c(178:32:37:0:0)	 _ifend_4($9) :
main.c(179:33:38:0:0)		iTemp9 [k20 lr33:34 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{char} = iTemp2 [k6 lr5:33 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sensorSample_phase_1_1}[r0x65 ] == 0x3 {literal unsigned char}
main.c(179:34:39:0:0)		if iTemp9 [k20 lr33:34 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{char} == 0 goto _iffalse_5($11)
main.c(179:35:42:0:0)		_sensorSample_phase3_1_1 [k21 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{bit bit {0,1}} := 0x1 {literal bit {0,1}}
main.c(179:36:43:0:0)		 goto preHeaderLbl0($33)
main.c(179:37:44:0:0)	 _iffalse_5($11) :
main.c(179:38:45:0:0)		_sensorSample_phase3_1_1 [k21 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{bit bit {0,1}} := 0x0 {literal bit {0,1}}
main.c(186:40:150:0:0)	 preHeaderLbl0($33) :
main.c(186:41:152:0:0)		iTemp12 [k25 lr41:66 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sensorSample_halfcycles_1_1}[r0x67 ] := 0x3 {literal unsigned char}
main.c(186:42:48:1:0)	 _dobody_1($13) :
main.c(187:43:49:1:0)		_PORTB [k8 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{sfr volatile unsigned char} = _PORTB [k8 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{sfr volatile unsigned char} ^ iTemp0 [k2 lr3:66 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sensorSample_transmitter_1_1}[r0x63 ]
main.c(188:45:51:1:0)	 nop main.c(188:46:52:1:0)	 nop main.c(188:47:53:1:0)	 nop main.c(189:48:54:1:0)	 nop main.c(189:49:55:1:0)	 nop main.c(189:50:56:1:0)	 nop main.c(189:51:57:1:0)	 nop main.c(189:52:58:1:0)	 nop main.c(190:53:59:1:0)	 nop main.c(190:54:60:1:0)	 nop main.c(190:55:61:1:0)	 nop main.c(190:56:62:1:0)	 nop main.c(190:57:63:1:0)	 nop main.c(191:58:64:1:0)	 nop main.c(191:59:65:1:0)	 nop main.c(191:60:66:1:0)	 nop main.c(191:61:67:1:0)	 nop main.c(191:62:68:1:0)	 nop main.c(192:63:69:1:0)	 nop main.c(192:64:70:1:0)	 nop main.c(193:65:72:1:0)		iTemp12 [k25 lr41:66 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sensorSample_halfcycles_1_1}[r0x67 ] = iTemp12 [k25 lr41:66 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sensorSample_halfcycles_1_1}[r0x67 ] - 0x1 {literal unsigned char}
main.c(193:66:74:1:0)		if iTemp12 [k25 lr41:66 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sensorSample_halfcycles_1_1}[r0x67 ] != 0 goto _dobody_1($13)
main.c(196:67:77:0:0)		if _sensorSample_phase0_1_1 [k15 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{bit bit {0,1}} == 0 goto _iffalse_6($17)
main.c(197:68:81:0:0)		_GO_DONE [k26 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{bit bit {0,1}} := 0x1 {literal bit {0,1}}
main.c(197:69:82:0:0)	 _iffalse_6($17) :
main.c(198:70:83:0:0)	 nop main.c(198:71:84:0:0)	 nop main.c(198:72:85:0:0)	 nop main.c(198:73:86:0:0)	 nop main.c(198:74:87:0:0)	 nop main.c(199:75:88:0:0)	 nop main.c(199:76:89:0:0)	 nop main.c(199:77:90:0:0)	 nop main.c(199:78:91:0:0)	 nop main.c(199:79:92:0:0)	 nop main.c(200:80:93:0:0)		if _sensorSample_phase1_1_1 [k17 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{bit bit {0,1}} == 0 goto _iffalse_7($19)
main.c(201:81:97:0:0)		_GO_DONE [k26 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{bit bit {0,1}} := 0x1 {literal bit {0,1}}
main.c(201:82:98:0:0)	 _iffalse_7($19) :
main.c(202:83:99:0:0)	 nop main.c(202:84:100:0:0)	 nop main.c(202:85:101:0:0)	 nop main.c(202:86:102:0:0)	 nop main.c(202:87:103:0:0)	 nop main.c(203:88:104:0:0)	 nop main.c(203:89:105:0:0)	 nop main.c(203:90:106:0:0)	 nop main.c(203:91:107:0:0)	 nop main.c(203:92:108:0:0)	 nop main.c(203:93:109:0:0)	 nop main.c(204:94:110:0:0)		if _sensorSample_phase2_1_1 [k19 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{bit bit {0,1}} == 0 goto _iffalse_8($21)
main.c(205:95:114:0:0)		_GO_DONE [k26 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{bit bit {0,1}} := 0x1 {literal bit {0,1}}
main.c(205:96:115:0:0)	 _iffalse_8($21) :
main.c(206:97:116:0:0)	 nop main.c(206:98:117:0:0)	 nop main.c(206:99:118:0:0)	 nop main.c(206:100:119:0:0)	 nop main.c(206:101:120:0:0)	 nop main.c(207:102:121:0:0)	 nop main.c(207:103:122:0:0)	 nop main.c(207:104:123:0:0)	 nop main.c(207:105:124:0:0)	 nop main.c(207:106:125:0:0)	 nop main.c(208:107:126:0:0)		if _sensorSample_phase3_1_1 [k21 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{bit bit {0,1}} == 0 goto _whilecontinue_1($24)
main.c(209:108:130:0:0)		_GO_DONE [k26 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{bit bit {0,1}} := 0x1 {literal bit {0,1}}
main.c(212:110:132:1:0)	 _whilecontinue_1($24) :
main.c(212:111:133:1:0)		if _NOT_DONE [k31 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{bit bit {0,1}} != 0 goto _whilecontinue_1($24)
main.c(215:112:138:0:0)		_TRISB [k10 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{sfr volatile unsigned char} := 0xf0 {literal unsigned char}
main.c(217:113:139:0:0)		iTemp17 [k32 lr113:115 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sensorSample_h_1_1}[r0x68 ] := _ADRESH [k34 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{sfr volatile unsigned char}
main.c(218:114:140:0:0)		iTemp18 [k35 lr114:117 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sensorSample_l_1_1}[r0x69 ] := _ADRESL [k37 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{sfr volatile unsigned char}
main.c(219:115:141:0:0)		iTemp19 [k38 lr115:116 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int}[r0x68 r0x6A ] = (int)iTemp17 [k32 lr113:115 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sensorSample_h_1_1}[r0x68 ]
main.c(219:116:142:0:0)		iTemp20 [k39 lr116:118 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int}[r0x68 r0x6A ] = iTemp19 [k38 lr115:116 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int}[r0x68 r0x6A ] << 0x8 {literal unsigned char}
main.c(219:117:144:0:0)		iTemp22 [k41 lr117:118 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{unsigned int}[r0x69 r0x6B ] = (unsigned int)iTemp18 [k35 lr114:117 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sensorSample_l_1_1}[r0x69 ]
main.c(219:118:145:0:0)		iTemp23 [k42 lr118:119 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{unsigned int}[r0x68 r0x6A ] = iTemp20 [k39 lr116:118 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int}[r0x68 r0x6A ] | iTemp22 [k41 lr117:118 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{unsigned int}[r0x69 r0x6B ]
main.c(219:119:147:0:0)		ret iTemp23 [k42 lr118:119 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{unsigned int}[r0x68 r0x6A ]
main.c(219:120:148:0:0)	 _return($27) :
main.c(219:121:149:0:0)		eproc _sensorSample [k1 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int function    }

----------------------------------------------------------------
Basic Block _eBBlock0 : loop Depth = 0 noPath = 0 , lastinLoop = 0
depth 1st num 2 : bbnum = 1 1st iCode = 14 , last iCode = 15
visited 0 : hasFcall = 0

defines bitVector :bitvector Size = 153 bSize = 20
Bits on { (15) }

local defines bitVector :bitvector Size = 153 bSize = 20
Bits on { (15) }

pointers Set bitvector :
in pointers Set bitvector :
inDefs Set bitvector :bitvector Size = 153 bSize = 20
Bits on { (2) (3) (4) (5) (6) (7) (8) (9) (10) (11) }

outDefs Set bitvector :bitvector Size = 153 bSize = 20
Bits on { (2) (3) (4) (5) (6) (7) (8) (9) (10) (11) (15) }

usesDefs Set bitvector :bitvector Size = 153 bSize = 20
Bits on { }

----------------------------------------------------------------
main.c(176:14:15:0:0)		_sensorSample_phase0_1_1 [k15 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{bit bit {0,1}} := 0x1 {literal bit {0,1}}
main.c(176:15:16:0:0)		 goto _ifend_2($3)
main.c(176:16:17:0:0)	 _iffalse_2($2) :
main.c(176:17:18:0:0)		_sensorSample_phase0_1_1 [k15 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{bit bit {0,1}} := 0x0 {literal bit {0,1}}
main.c(176:18:19:0:0)	 _ifend_2($3) :
main.c(177:19:20:0:0)		iTemp7 [k16 lr19:20 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{char} = iTemp2 [k6 lr5:33 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sensorSample_phase_1_1}[r0x65 ] == 0x1 {literal unsigned char}
main.c(177:20:21:0:0)		if iTemp7 [k16 lr19:20 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{char} == 0 goto _iffalse_3($5)
main.c(177:21:24:0:0)		_sensorSample_phase1_1_1 [k17 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{bit bit {0,1}} := 0x1 {literal bit {0,1}}
main.c(177:22:25:0:0)		 goto _ifend_3($6)
main.c(177:23:26:0:0)	 _iffalse_3($5) :
main.c(177:24:27:0:0)		_sensorSample_phase1_1_1 [k17 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{bit bit {0,1}} := 0x0 {literal bit {0,1}}
main.c(177:25:28:0:0)	 _ifend_3($6) :
main.c(178:26:29:0:0)		iTemp8 [k18 lr26:27 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{char} = iTemp2 [k6 lr5:33 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sensorSample_phase_1_1}[r0x65 ] == 0x2 {literal unsigned char}
main.c(178:27:30:0:0)		if iTemp8 [k18 lr26:27 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{char} == 0 goto _iffalse_4($8)
main.c(178:28:33:0:0)		_sensorSample_phase2_1_1 [k19 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{bit bit {0,1}} := 0x1 {literal bit {0,1}}
main.c(178:29:34:0:0)		 goto _ifend_4($9)
main.c(178:30:35:0:0)	 _iffalse_4($8) :
main.c(178:31:36:0:0)		_sensorSample_phase2_1_1 [k19 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{bit bit {0,1}} := 0x0 {literal bit {0,1}}
main.c(178:32:37:0:0)	 _ifend_4($9) :
main.c(179:33:38:0:0)		iTemp9 [k20 lr33:34 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{char} = iTemp2 [k6 lr5:33 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sensorSample_phase_1_1}[r0x65 ] == 0x3 {literal unsigned char}
main.c(179:34:39:0:0)		if iTemp9 [k20 lr33:34 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{char} == 0 goto _iffalse_5($11)
main.c(179:35:42:0:0)		_sensorSample_phase3_1_1 [k21 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{bit bit {0,1}} := 0x1 {literal bit {0,1}}
main.c(179:36:43:0:0)		 goto preHeaderLbl0($33)
main.c(179:37:44:0:0)	 _iffalse_5($11) :
main.c(179:38:45:0:0)		_sensorSample_phase3_1_1 [k21 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{bit bit {0,1}} := 0x0 {literal bit {0,1}}
main.c(186:40:150:0:0)	 preHeaderLbl0($33) :
main.c(186:41:152:0:0)		iTemp12 [k25 lr41:66 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sensorSample_halfcycles_1_1}[r0x67 ] := 0x3 {literal unsigned char}
main.c(186:42:48:1:0)	 _dobody_1($13) :
main.c(187:43:49:1:0)		_PORTB [k8 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{sfr volatile unsigned char} = _PORTB [k8 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{sfr volatile unsigned char} ^ iTemp0 [k2 lr3:66 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sensorSample_transmitter_1_1}[r0x63 ]
main.c(188:45:51:1:0)	 nop main.c(188:46:52:1:0)	 nop main.c(188:47:53:1:0)	 nop main.c(189:48:54:1:0)	 nop main.c(189:49:55:1:0)	 nop main.c(189:50:56:1:0)	 nop main.c(189:51:57:1:0)	 nop main.c(189:52:58:1:0)	 nop main.c(190:53:59:1:0)	 nop main.c(190:54:60:1:0)	 nop main.c(190:55:61:1:0)	 nop main.c(190:56:62:1:0)	 nop main.c(190:57:63:1:0)	 nop main.c(191:58:64:1:0)	 nop main.c(191:59:65:1:0)	 nop main.c(191:60:66:1:0)	 nop main.c(191:61:67:1:0)	 nop main.c(191:62:68:1:0)	 nop main.c(192:63:69:1:0)	 nop main.c(192:64:70:1:0)	 nop main.c(193:65:72:1:0)		iTemp12 [k25 lr41:66 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sensorSample_halfcycles_1_1}[r0x67 ] = iTemp12 [k25 lr41:66 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sensorSample_halfcycles_1_1}[r0x67 ] - 0x1 {literal unsigned char}
main.c(193:66:74:1:0)		if iTemp12 [k25 lr41:66 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sensorSample_halfcycles_1_1}[r0x67 ] != 0 goto _dobody_1($13)
main.c(196:67:77:0:0)		if _sensorSample_phase0_1_1 [k15 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{bit bit {0,1}} == 0 goto _iffalse_6($17)
main.c(197:68:81:0:0)		_GO_DONE [k26 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{bit bit {0,1}} := 0x1 {literal bit {0,1}}
main.c(197:69:82:0:0)	 _iffalse_6($17) :
main.c(198:70:83:0:0)	 nop main.c(198:71:84:0:0)	 nop main.c(198:72:85:0:0)	 nop main.c(198:73:86:0:0)	 nop main.c(198:74:87:0:0)	 nop main.c(199:75:88:0:0)	 nop main.c(199:76:89:0:0)	 nop main.c(199:77:90:0:0)	 nop main.c(199:78:91:0:0)	 nop main.c(199:79:92:0:0)	 nop main.c(200:80:93:0:0)		if _sensorSample_phase1_1_1 [k17 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{bit bit {0,1}} == 0 goto _iffalse_7($19)
main.c(201:81:97:0:0)		_GO_DONE [k26 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{bit bit {0,1}} := 0x1 {literal bit {0,1}}
main.c(201:82:98:0:0)	 _iffalse_7($19) :
main.c(202:83:99:0:0)	 nop main.c(202:84:100:0:0)	 nop main.c(202:85:101:0:0)	 nop main.c(202:86:102:0:0)	 nop main.c(202:87:103:0:0)	 nop main.c(203:88:104:0:0)	 nop main.c(203:89:105:0:0)	 nop main.c(203:90:106:0:0)	 nop main.c(203:91:107:0:0)	 nop main.c(203:92:108:0:0)	 nop main.c(203:93:109:0:0)	 nop main.c(204:94:110:0:0)		if _sensorSample_phase2_1_1 [k19 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{bit bit {0,1}} == 0 goto _iffalse_8($21)
main.c(205:95:114:0:0)		_GO_DONE [k26 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{bit bit {0,1}} := 0x1 {literal bit {0,1}}
main.c(205:96:115:0:0)	 _iffalse_8($21) :
main.c(206:97:116:0:0)	 nop main.c(206:98:117:0:0)	 nop main.c(206:99:118:0:0)	 nop main.c(206:100:119:0:0)	 nop main.c(206:101:120:0:0)	 nop main.c(207:102:121:0:0)	 nop main.c(207:103:122:0:0)	 nop main.c(207:104:123:0:0)	 nop main.c(207:105:124:0:0)	 nop main.c(207:106:125:0:0)	 nop main.c(208:107:126:0:0)		if _sensorSample_phase3_1_1 [k21 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{bit bit {0,1}} == 0 goto _whilecontinue_1($24)
main.c(209:108:130:0:0)		_GO_DONE [k26 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{bit bit {0,1}} := 0x1 {literal bit {0,1}}
main.c(212:110:132:1:0)	 _whilecontinue_1($24) :
main.c(212:111:133:1:0)		if _NOT_DONE [k31 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{bit bit {0,1}} != 0 goto _whilecontinue_1($24)
main.c(215:112:138:0:0)		_TRISB [k10 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{sfr volatile unsigned char} := 0xf0 {literal unsigned char}
main.c(217:113:139:0:0)		iTemp17 [k32 lr113:115 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sensorSample_h_1_1}[r0x68 ] := _ADRESH [k34 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{sfr volatile unsigned char}
main.c(218:114:140:0:0)		iTemp18 [k35 lr114:117 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sensorSample_l_1_1}[r0x69 ] := _ADRESL [k37 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{sfr volatile unsigned char}
main.c(219:115:141:0:0)		iTemp19 [k38 lr115:116 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int}[r0x68 r0x6A ] = (int)iTemp17 [k32 lr113:115 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sensorSample_h_1_1}[r0x68 ]
main.c(219:116:142:0:0)		iTemp20 [k39 lr116:118 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int}[r0x68 r0x6A ] = iTemp19 [k38 lr115:116 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int}[r0x68 r0x6A ] << 0x8 {literal unsigned char}
main.c(219:117:144:0:0)		iTemp22 [k41 lr117:118 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{unsigned int}[r0x69 r0x6B ] = (unsigned int)iTemp18 [k35 lr114:117 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sensorSample_l_1_1}[r0x69 ]
main.c(219:118:145:0:0)		iTemp23 [k42 lr118:119 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{unsigned int}[r0x68 r0x6A ] = iTemp20 [k39 lr116:118 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int}[r0x68 r0x6A ] | iTemp22 [k41 lr117:118 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{unsigned int}[r0x69 r0x6B ]
main.c(219:119:147:0:0)		ret iTemp23 [k42 lr118:119 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{unsigned int}[r0x68 r0x6A ]
main.c(219:120:148:0:0)	 _return($27) :
main.c(219:121:149:0:0)		eproc _sensorSample [k1 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int function    }

----------------------------------------------------------------
Basic Block _iffalse_2 : loop Depth = 0 noPath = 0 , lastinLoop = 0
depth 1st num 3 : bbnum = 2 1st iCode = 16 , last iCode = 17
visited 0 : hasFcall = 0

defines bitVector :bitvector Size = 153 bSize = 20
Bits on { (18) }

local defines bitVector :bitvector Size = 153 bSize = 20
Bits on { (18) }

pointers Set bitvector :
in pointers Set bitvector :
inDefs Set bitvector :bitvector Size = 153 bSize = 20
Bits on { (2) (3) (4) (5) (6) (7) (8) (9) (10) (11) }

outDefs Set bitvector :bitvector Size = 153 bSize = 20
Bits on { (2) (3) (4) (5) (6) (7) (8) (9) (10) (11) (18) }

usesDefs Set bitvector :bitvector Size = 153 bSize = 20
Bits on { }

----------------------------------------------------------------
main.c(176:16:17:0:0)	 _iffalse_2($2) :
main.c(176:17:18:0:0)		_sensorSample_phase0_1_1 [k15 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{bit bit {0,1}} := 0x0 {literal bit {0,1}}
main.c(176:18:19:0:0)	 _ifend_2($3) :
main.c(177:19:20:0:0)		iTemp7 [k16 lr19:20 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{char} = iTemp2 [k6 lr5:33 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sensorSample_phase_1_1}[r0x65 ] == 0x1 {literal unsigned char}
main.c(177:20:21:0:0)		if iTemp7 [k16 lr19:20 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{char} == 0 goto _iffalse_3($5)
main.c(177:21:24:0:0)		_sensorSample_phase1_1_1 [k17 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{bit bit {0,1}} := 0x1 {literal bit {0,1}}
main.c(177:22:25:0:0)		 goto _ifend_3($6)
main.c(177:23:26:0:0)	 _iffalse_3($5) :
main.c(177:24:27:0:0)		_sensorSample_phase1_1_1 [k17 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{bit bit {0,1}} := 0x0 {literal bit {0,1}}
main.c(177:25:28:0:0)	 _ifend_3($6) :
main.c(178:26:29:0:0)		iTemp8 [k18 lr26:27 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{char} = iTemp2 [k6 lr5:33 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sensorSample_phase_1_1}[r0x65 ] == 0x2 {literal unsigned char}
main.c(178:27:30:0:0)		if iTemp8 [k18 lr26:27 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{char} == 0 goto _iffalse_4($8)
main.c(178:28:33:0:0)		_sensorSample_phase2_1_1 [k19 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{bit bit {0,1}} := 0x1 {literal bit {0,1}}
main.c(178:29:34:0:0)		 goto _ifend_4($9)
main.c(178:30:35:0:0)	 _iffalse_4($8) :
main.c(178:31:36:0:0)		_sensorSample_phase2_1_1 [k19 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{bit bit {0,1}} := 0x0 {literal bit {0,1}}
main.c(178:32:37:0:0)	 _ifend_4($9) :
main.c(179:33:38:0:0)		iTemp9 [k20 lr33:34 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{char} = iTemp2 [k6 lr5:33 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sensorSample_phase_1_1}[r0x65 ] == 0x3 {literal unsigned char}
main.c(179:34:39:0:0)		if iTemp9 [k20 lr33:34 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{char} == 0 goto _iffalse_5($11)
main.c(179:35:42:0:0)		_sensorSample_phase3_1_1 [k21 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{bit bit {0,1}} := 0x1 {literal bit {0,1}}
main.c(179:36:43:0:0)		 goto preHeaderLbl0($33)
main.c(179:37:44:0:0)	 _iffalse_5($11) :
main.c(179:38:45:0:0)		_sensorSample_phase3_1_1 [k21 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{bit bit {0,1}} := 0x0 {literal bit {0,1}}
main.c(186:40:150:0:0)	 preHeaderLbl0($33) :
main.c(186:41:152:0:0)		iTemp12 [k25 lr41:66 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sensorSample_halfcycles_1_1}[r0x67 ] := 0x3 {literal unsigned char}
main.c(186:42:48:1:0)	 _dobody_1($13) :
main.c(187:43:49:1:0)		_PORTB [k8 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{sfr volatile unsigned char} = _PORTB [k8 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{sfr volatile unsigned char} ^ iTemp0 [k2 lr3:66 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sensorSample_transmitter_1_1}[r0x63 ]
main.c(188:45:51:1:0)	 nop main.c(188:46:52:1:0)	 nop main.c(188:47:53:1:0)	 nop main.c(189:48:54:1:0)	 nop main.c(189:49:55:1:0)	 nop main.c(189:50:56:1:0)	 nop main.c(189:51:57:1:0)	 nop main.c(189:52:58:1:0)	 nop main.c(190:53:59:1:0)	 nop main.c(190:54:60:1:0)	 nop main.c(190:55:61:1:0)	 nop main.c(190:56:62:1:0)	 nop main.c(190:57:63:1:0)	 nop main.c(191:58:64:1:0)	 nop main.c(191:59:65:1:0)	 nop main.c(191:60:66:1:0)	 nop main.c(191:61:67:1:0)	 nop main.c(191:62:68:1:0)	 nop main.c(192:63:69:1:0)	 nop main.c(192:64:70:1:0)	 nop main.c(193:65:72:1:0)		iTemp12 [k25 lr41:66 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sensorSample_halfcycles_1_1}[r0x67 ] = iTemp12 [k25 lr41:66 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sensorSample_halfcycles_1_1}[r0x67 ] - 0x1 {literal unsigned char}
main.c(193:66:74:1:0)		if iTemp12 [k25 lr41:66 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sensorSample_halfcycles_1_1}[r0x67 ] != 0 goto _dobody_1($13)
main.c(196:67:77:0:0)		if _sensorSample_phase0_1_1 [k15 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{bit bit {0,1}} == 0 goto _iffalse_6($17)
main.c(197:68:81:0:0)		_GO_DONE [k26 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{bit bit {0,1}} := 0x1 {literal bit {0,1}}
main.c(197:69:82:0:0)	 _iffalse_6($17) :
main.c(198:70:83:0:0)	 nop main.c(198:71:84:0:0)	 nop main.c(198:72:85:0:0)	 nop main.c(198:73:86:0:0)	 nop main.c(198:74:87:0:0)	 nop main.c(199:75:88:0:0)	 nop main.c(199:76:89:0:0)	 nop main.c(199:77:90:0:0)	 nop main.c(199:78:91:0:0)	 nop main.c(199:79:92:0:0)	 nop main.c(200:80:93:0:0)		if _sensorSample_phase1_1_1 [k17 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{bit bit {0,1}} == 0 goto _iffalse_7($19)
main.c(201:81:97:0:0)		_GO_DONE [k26 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{bit bit {0,1}} := 0x1 {literal bit {0,1}}
main.c(201:82:98:0:0)	 _iffalse_7($19) :
main.c(202:83:99:0:0)	 nop main.c(202:84:100:0:0)	 nop main.c(202:85:101:0:0)	 nop main.c(202:86:102:0:0)	 nop main.c(202:87:103:0:0)	 nop main.c(203:88:104:0:0)	 nop main.c(203:89:105:0:0)	 nop main.c(203:90:106:0:0)	 nop main.c(203:91:107:0:0)	 nop main.c(203:92:108:0:0)	 nop main.c(203:93:109:0:0)	 nop main.c(204:94:110:0:0)		if _sensorSample_phase2_1_1 [k19 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{bit bit {0,1}} == 0 goto _iffalse_8($21)
main.c(205:95:114:0:0)		_GO_DONE [k26 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{bit bit {0,1}} := 0x1 {literal bit {0,1}}
main.c(205:96:115:0:0)	 _iffalse_8($21) :
main.c(206:97:116:0:0)	 nop main.c(206:98:117:0:0)	 nop main.c(206:99:118:0:0)	 nop main.c(206:100:119:0:0)	 nop main.c(206:101:120:0:0)	 nop main.c(207:102:121:0:0)	 nop main.c(207:103:122:0:0)	 nop main.c(207:104:123:0:0)	 nop main.c(207:105:124:0:0)	 nop main.c(207:106:125:0:0)	 nop main.c(208:107:126:0:0)		if _sensorSample_phase3_1_1 [k21 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{bit bit {0,1}} == 0 goto _whilecontinue_1($24)
main.c(209:108:130:0:0)		_GO_DONE [k26 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{bit bit {0,1}} := 0x1 {literal bit {0,1}}
main.c(212:110:132:1:0)	 _whilecontinue_1($24) :
main.c(212:111:133:1:0)		if _NOT_DONE [k31 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{bit bit {0,1}} != 0 goto _whilecontinue_1($24)
main.c(215:112:138:0:0)		_TRISB [k10 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{sfr volatile unsigned char} := 0xf0 {literal unsigned char}
main.c(217:113:139:0:0)		iTemp17 [k32 lr113:115 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sensorSample_h_1_1}[r0x68 ] := _ADRESH [k34 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{sfr volatile unsigned char}
main.c(218:114:140:0:0)		iTemp18 [k35 lr114:117 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sensorSample_l_1_1}[r0x69 ] := _ADRESL [k37 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{sfr volatile unsigned char}
main.c(219:115:141:0:0)		iTemp19 [k38 lr115:116 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int}[r0x68 r0x6A ] = (int)iTemp17 [k32 lr113:115 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sensorSample_h_1_1}[r0x68 ]
main.c(219:116:142:0:0)		iTemp20 [k39 lr116:118 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int}[r0x68 r0x6A ] = iTemp19 [k38 lr115:116 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int}[r0x68 r0x6A ] << 0x8 {literal unsigned char}
main.c(219:117:144:0:0)		iTemp22 [k41 lr117:118 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{unsigned int}[r0x69 r0x6B ] = (unsigned int)iTemp18 [k35 lr114:117 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sensorSample_l_1_1}[r0x69 ]
main.c(219:118:145:0:0)		iTemp23 [k42 lr118:119 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{unsigned int}[r0x68 r0x6A ] = iTemp20 [k39 lr116:118 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int}[r0x68 r0x6A ] | iTemp22 [k41 lr117:118 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{unsigned int}[r0x69 r0x6B ]
main.c(219:119:147:0:0)		ret iTemp23 [k42 lr118:119 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{unsigned int}[r0x68 r0x6A ]
main.c(219:120:148:0:0)	 _return($27) :
main.c(219:121:149:0:0)		eproc _sensorSample [k1 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int function    }

----------------------------------------------------------------
Basic Block _ifend_2 : loop Depth = 0 noPath = 0 , lastinLoop = 0
depth 1st num 4 : bbnum = 3 1st iCode = 18 , last iCode = 20
visited 0 : hasFcall = 0

defines bitVector :bitvector Size = 153 bSize = 20
Bits on { (20) }

local defines bitVector :bitvector Size = 153 bSize = 20
Bits on { (20) }

pointers Set bitvector :
in pointers Set bitvector :
inDefs Set bitvector :bitvector Size = 153 bSize = 20
Bits on { (2) (3) (4) (5) (6) (7) (8) (9) (10) (11) (15) (18) }

outDefs Set bitvector :bitvector Size = 153 bSize = 20
Bits on { (2) (3) (4) (5) (6) (7) (8) (9) (10) (11) (15) (18) (20) }

usesDefs Set bitvector :bitvector Size = 153 bSize = 20
Bits on { (4) (20) }

----------------------------------------------------------------
main.c(176:18:19:0:0)	 _ifend_2($3) :
main.c(177:19:20:0:0)		iTemp7 [k16 lr19:20 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{char} = iTemp2 [k6 lr5:33 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sensorSample_phase_1_1}[r0x65 ] == 0x1 {literal unsigned char}
main.c(177:20:21:0:0)		if iTemp7 [k16 lr19:20 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{char} == 0 goto _iffalse_3($5)
main.c(177:21:24:0:0)		_sensorSample_phase1_1_1 [k17 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{bit bit {0,1}} := 0x1 {literal bit {0,1}}
main.c(177:22:25:0:0)		 goto _ifend_3($6)
main.c(177:23:26:0:0)	 _iffalse_3($5) :
main.c(177:24:27:0:0)		_sensorSample_phase1_1_1 [k17 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{bit bit {0,1}} := 0x0 {literal bit {0,1}}
main.c(177:25:28:0:0)	 _ifend_3($6) :
main.c(178:26:29:0:0)		iTemp8 [k18 lr26:27 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{char} = iTemp2 [k6 lr5:33 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sensorSample_phase_1_1}[r0x65 ] == 0x2 {literal unsigned char}
main.c(178:27:30:0:0)		if iTemp8 [k18 lr26:27 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{char} == 0 goto _iffalse_4($8)
main.c(178:28:33:0:0)		_sensorSample_phase2_1_1 [k19 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{bit bit {0,1}} := 0x1 {literal bit {0,1}}
main.c(178:29:34:0:0)		 goto _ifend_4($9)
main.c(178:30:35:0:0)	 _iffalse_4($8) :
main.c(178:31:36:0:0)		_sensorSample_phase2_1_1 [k19 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{bit bit {0,1}} := 0x0 {literal bit {0,1}}
main.c(178:32:37:0:0)	 _ifend_4($9) :
main.c(179:33:38:0:0)		iTemp9 [k20 lr33:34 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{char} = iTemp2 [k6 lr5:33 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sensorSample_phase_1_1}[r0x65 ] == 0x3 {literal unsigned char}
main.c(179:34:39:0:0)		if iTemp9 [k20 lr33:34 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{char} == 0 goto _iffalse_5($11)
main.c(179:35:42:0:0)		_sensorSample_phase3_1_1 [k21 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{bit bit {0,1}} := 0x1 {literal bit {0,1}}
main.c(179:36:43:0:0)		 goto preHeaderLbl0($33)
main.c(179:37:44:0:0)	 _iffalse_5($11) :
main.c(179:38:45:0:0)		_sensorSample_phase3_1_1 [k21 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{bit bit {0,1}} := 0x0 {literal bit {0,1}}
main.c(186:40:150:0:0)	 preHeaderLbl0($33) :
main.c(186:41:152:0:0)		iTemp12 [k25 lr41:66 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sensorSample_halfcycles_1_1}[r0x67 ] := 0x3 {literal unsigned char}
main.c(186:42:48:1:0)	 _dobody_1($13) :
main.c(187:43:49:1:0)		_PORTB [k8 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{sfr volatile unsigned char} = _PORTB [k8 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{sfr volatile unsigned char} ^ iTemp0 [k2 lr3:66 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sensorSample_transmitter_1_1}[r0x63 ]
main.c(188:45:51:1:0)	 nop main.c(188:46:52:1:0)	 nop main.c(188:47:53:1:0)	 nop main.c(189:48:54:1:0)	 nop main.c(189:49:55:1:0)	 nop main.c(189:50:56:1:0)	 nop main.c(189:51:57:1:0)	 nop main.c(189:52:58:1:0)	 nop main.c(190:53:59:1:0)	 nop main.c(190:54:60:1:0)	 nop main.c(190:55:61:1:0)	 nop main.c(190:56:62:1:0)	 nop main.c(190:57:63:1:0)	 nop main.c(191:58:64:1:0)	 nop main.c(191:59:65:1:0)	 nop main.c(191:60:66:1:0)	 nop main.c(191:61:67:1:0)	 nop main.c(191:62:68:1:0)	 nop main.c(192:63:69:1:0)	 nop main.c(192:64:70:1:0)	 nop main.c(193:65:72:1:0)		iTemp12 [k25 lr41:66 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sensorSample_halfcycles_1_1}[r0x67 ] = iTemp12 [k25 lr41:66 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sensorSample_halfcycles_1_1}[r0x67 ] - 0x1 {literal unsigned char}
main.c(193:66:74:1:0)		if iTemp12 [k25 lr41:66 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sensorSample_halfcycles_1_1}[r0x67 ] != 0 goto _dobody_1($13)
main.c(196:67:77:0:0)		if _sensorSample_phase0_1_1 [k15 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{bit bit {0,1}} == 0 goto _iffalse_6($17)
main.c(197:68:81:0:0)		_GO_DONE [k26 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{bit bit {0,1}} := 0x1 {literal bit {0,1}}
main.c(197:69:82:0:0)	 _iffalse_6($17) :
main.c(198:70:83:0:0)	 nop main.c(198:71:84:0:0)	 nop main.c(198:72:85:0:0)	 nop main.c(198:73:86:0:0)	 nop main.c(198:74:87:0:0)	 nop main.c(199:75:88:0:0)	 nop main.c(199:76:89:0:0)	 nop main.c(199:77:90:0:0)	 nop main.c(199:78:91:0:0)	 nop main.c(199:79:92:0:0)	 nop main.c(200:80:93:0:0)		if _sensorSample_phase1_1_1 [k17 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{bit bit {0,1}} == 0 goto _iffalse_7($19)
main.c(201:81:97:0:0)		_GO_DONE [k26 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{bit bit {0,1}} := 0x1 {literal bit {0,1}}
main.c(201:82:98:0:0)	 _iffalse_7($19) :
main.c(202:83:99:0:0)	 nop main.c(202:84:100:0:0)	 nop main.c(202:85:101:0:0)	 nop main.c(202:86:102:0:0)	 nop main.c(202:87:103:0:0)	 nop main.c(203:88:104:0:0)	 nop main.c(203:89:105:0:0)	 nop main.c(203:90:106:0:0)	 nop main.c(203:91:107:0:0)	 nop main.c(203:92:108:0:0)	 nop main.c(203:93:109:0:0)	 nop main.c(204:94:110:0:0)		if _sensorSample_phase2_1_1 [k19 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{bit bit {0,1}} == 0 goto _iffalse_8($21)
main.c(205:95:114:0:0)		_GO_DONE [k26 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{bit bit {0,1}} := 0x1 {literal bit {0,1}}
main.c(205:96:115:0:0)	 _iffalse_8($21) :
main.c(206:97:116:0:0)	 nop main.c(206:98:117:0:0)	 nop main.c(206:99:118:0:0)	 nop main.c(206:100:119:0:0)	 nop main.c(206:101:120:0:0)	 nop main.c(207:102:121:0:0)	 nop main.c(207:103:122:0:0)	 nop main.c(207:104:123:0:0)	 nop main.c(207:105:124:0:0)	 nop main.c(207:106:125:0:0)	 nop main.c(208:107:126:0:0)		if _sensorSample_phase3_1_1 [k21 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{bit bit {0,1}} == 0 goto _whilecontinue_1($24)
main.c(209:108:130:0:0)		_GO_DONE [k26 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{bit bit {0,1}} := 0x1 {literal bit {0,1}}
main.c(212:110:132:1:0)	 _whilecontinue_1($24) :
main.c(212:111:133:1:0)		if _NOT_DONE [k31 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{bit bit {0,1}} != 0 goto _whilecontinue_1($24)
main.c(215:112:138:0:0)		_TRISB [k10 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{sfr volatile unsigned char} := 0xf0 {literal unsigned char}
main.c(217:113:139:0:0)		iTemp17 [k32 lr113:115 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sensorSample_h_1_1}[r0x68 ] := _ADRESH [k34 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{sfr volatile unsigned char}
main.c(218:114:140:0:0)		iTemp18 [k35 lr114:117 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sensorSample_l_1_1}[r0x69 ] := _ADRESL [k37 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{sfr volatile unsigned char}
main.c(219:115:141:0:0)		iTemp19 [k38 lr115:116 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int}[r0x68 r0x6A ] = (int)iTemp17 [k32 lr113:115 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sensorSample_h_1_1}[r0x68 ]
main.c(219:116:142:0:0)		iTemp20 [k39 lr116:118 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int}[r0x68 r0x6A ] = iTemp19 [k38 lr115:116 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int}[r0x68 r0x6A ] << 0x8 {literal unsigned char}
main.c(219:117:144:0:0)		iTemp22 [k41 lr117:118 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{unsigned int}[r0x69 r0x6B ] = (unsigned int)iTemp18 [k35 lr114:117 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sensorSample_l_1_1}[r0x69 ]
main.c(219:118:145:0:0)		iTemp23 [k42 lr118:119 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{unsigned int}[r0x68 r0x6A ] = iTemp20 [k39 lr116:118 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int}[r0x68 r0x6A ] | iTemp22 [k41 lr117:118 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{unsigned int}[r0x69 r0x6B ]
main.c(219:119:147:0:0)		ret iTemp23 [k42 lr118:119 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{unsigned int}[r0x68 r0x6A ]
main.c(219:120:148:0:0)	 _return($27) :
main.c(219:121:149:0:0)		eproc _sensorSample [k1 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int function    }

----------------------------------------------------------------
Basic Block _eBBlock1 : loop Depth = 0 noPath = 0 , lastinLoop = 0
depth 1st num 5 : bbnum = 4 1st iCode = 21 , last iCode = 22
visited 0 : hasFcall = 0

defines bitVector :bitvector Size = 153 bSize = 20
Bits on { (24) }

local defines bitVector :bitvector Size = 153 bSize = 20
Bits on { (24) }

pointers Set bitvector :
in pointers Set bitvector :
inDefs Set bitvector :bitvector Size = 153 bSize = 20
Bits on { (2) (3) (4) (5) (6) (7) (8) (9) (10) (11) (15) (18) (20) }

outDefs Set bitvector :bitvector Size = 153 bSize = 20
Bits on { (2) (3) (4) (5) (6) (7) (8) (9) (10) (11) (15) (18) (20) (24) }

usesDefs Set bitvector :bitvector Size = 153 bSize = 20
Bits on { }

----------------------------------------------------------------
main.c(177:21:24:0:0)		_sensorSample_phase1_1_1 [k17 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{bit bit {0,1}} := 0x1 {literal bit {0,1}}
main.c(177:22:25:0:0)		 goto _ifend_3($6)
main.c(177:23:26:0:0)	 _iffalse_3($5) :
main.c(177:24:27:0:0)		_sensorSample_phase1_1_1 [k17 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{bit bit {0,1}} := 0x0 {literal bit {0,1}}
main.c(177:25:28:0:0)	 _ifend_3($6) :
main.c(178:26:29:0:0)		iTemp8 [k18 lr26:27 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{char} = iTemp2 [k6 lr5:33 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sensorSample_phase_1_1}[r0x65 ] == 0x2 {literal unsigned char}
main.c(178:27:30:0:0)		if iTemp8 [k18 lr26:27 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{char} == 0 goto _iffalse_4($8)
main.c(178:28:33:0:0)		_sensorSample_phase2_1_1 [k19 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{bit bit {0,1}} := 0x1 {literal bit {0,1}}
main.c(178:29:34:0:0)		 goto _ifend_4($9)
main.c(178:30:35:0:0)	 _iffalse_4($8) :
main.c(178:31:36:0:0)		_sensorSample_phase2_1_1 [k19 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{bit bit {0,1}} := 0x0 {literal bit {0,1}}
main.c(178:32:37:0:0)	 _ifend_4($9) :
main.c(179:33:38:0:0)		iTemp9 [k20 lr33:34 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{char} = iTemp2 [k6 lr5:33 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sensorSample_phase_1_1}[r0x65 ] == 0x3 {literal unsigned char}
main.c(179:34:39:0:0)		if iTemp9 [k20 lr33:34 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{char} == 0 goto _iffalse_5($11)
main.c(179:35:42:0:0)		_sensorSample_phase3_1_1 [k21 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{bit bit {0,1}} := 0x1 {literal bit {0,1}}
main.c(179:36:43:0:0)		 goto preHeaderLbl0($33)
main.c(179:37:44:0:0)	 _iffalse_5($11) :
main.c(179:38:45:0:0)		_sensorSample_phase3_1_1 [k21 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{bit bit {0,1}} := 0x0 {literal bit {0,1}}
main.c(186:40:150:0:0)	 preHeaderLbl0($33) :
main.c(186:41:152:0:0)		iTemp12 [k25 lr41:66 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sensorSample_halfcycles_1_1}[r0x67 ] := 0x3 {literal unsigned char}
main.c(186:42:48:1:0)	 _dobody_1($13) :
main.c(187:43:49:1:0)		_PORTB [k8 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{sfr volatile unsigned char} = _PORTB [k8 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{sfr volatile unsigned char} ^ iTemp0 [k2 lr3:66 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sensorSample_transmitter_1_1}[r0x63 ]
main.c(188:45:51:1:0)	 nop main.c(188:46:52:1:0)	 nop main.c(188:47:53:1:0)	 nop main.c(189:48:54:1:0)	 nop main.c(189:49:55:1:0)	 nop main.c(189:50:56:1:0)	 nop main.c(189:51:57:1:0)	 nop main.c(189:52:58:1:0)	 nop main.c(190:53:59:1:0)	 nop main.c(190:54:60:1:0)	 nop main.c(190:55:61:1:0)	 nop main.c(190:56:62:1:0)	 nop main.c(190:57:63:1:0)	 nop main.c(191:58:64:1:0)	 nop main.c(191:59:65:1:0)	 nop main.c(191:60:66:1:0)	 nop main.c(191:61:67:1:0)	 nop main.c(191:62:68:1:0)	 nop main.c(192:63:69:1:0)	 nop main.c(192:64:70:1:0)	 nop main.c(193:65:72:1:0)		iTemp12 [k25 lr41:66 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sensorSample_halfcycles_1_1}[r0x67 ] = iTemp12 [k25 lr41:66 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sensorSample_halfcycles_1_1}[r0x67 ] - 0x1 {literal unsigned char}
main.c(193:66:74:1:0)		if iTemp12 [k25 lr41:66 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sensorSample_halfcycles_1_1}[r0x67 ] != 0 goto _dobody_1($13)
main.c(196:67:77:0:0)		if _sensorSample_phase0_1_1 [k15 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{bit bit {0,1}} == 0 goto _iffalse_6($17)
main.c(197:68:81:0:0)		_GO_DONE [k26 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{bit bit {0,1}} := 0x1 {literal bit {0,1}}
main.c(197:69:82:0:0)	 _iffalse_6($17) :
main.c(198:70:83:0:0)	 nop main.c(198:71:84:0:0)	 nop main.c(198:72:85:0:0)	 nop main.c(198:73:86:0:0)	 nop main.c(198:74:87:0:0)	 nop main.c(199:75:88:0:0)	 nop main.c(199:76:89:0:0)	 nop main.c(199:77:90:0:0)	 nop main.c(199:78:91:0:0)	 nop main.c(199:79:92:0:0)	 nop main.c(200:80:93:0:0)		if _sensorSample_phase1_1_1 [k17 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{bit bit {0,1}} == 0 goto _iffalse_7($19)
main.c(201:81:97:0:0)		_GO_DONE [k26 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{bit bit {0,1}} := 0x1 {literal bit {0,1}}
main.c(201:82:98:0:0)	 _iffalse_7($19) :
main.c(202:83:99:0:0)	 nop main.c(202:84:100:0:0)	 nop main.c(202:85:101:0:0)	 nop main.c(202:86:102:0:0)	 nop main.c(202:87:103:0:0)	 nop main.c(203:88:104:0:0)	 nop main.c(203:89:105:0:0)	 nop main.c(203:90:106:0:0)	 nop main.c(203:91:107:0:0)	 nop main.c(203:92:108:0:0)	 nop main.c(203:93:109:0:0)	 nop main.c(204:94:110:0:0)		if _sensorSample_phase2_1_1 [k19 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{bit bit {0,1}} == 0 goto _iffalse_8($21)
main.c(205:95:114:0:0)		_GO_DONE [k26 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{bit bit {0,1}} := 0x1 {literal bit {0,1}}
main.c(205:96:115:0:0)	 _iffalse_8($21) :
main.c(206:97:116:0:0)	 nop main.c(206:98:117:0:0)	 nop main.c(206:99:118:0:0)	 nop main.c(206:100:119:0:0)	 nop main.c(206:101:120:0:0)	 nop main.c(207:102:121:0:0)	 nop main.c(207:103:122:0:0)	 nop main.c(207:104:123:0:0)	 nop main.c(207:105:124:0:0)	 nop main.c(207:106:125:0:0)	 nop main.c(208:107:126:0:0)		if _sensorSample_phase3_1_1 [k21 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{bit bit {0,1}} == 0 goto _whilecontinue_1($24)
main.c(209:108:130:0:0)		_GO_DONE [k26 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{bit bit {0,1}} := 0x1 {literal bit {0,1}}
main.c(212:110:132:1:0)	 _whilecontinue_1($24) :
main.c(212:111:133:1:0)		if _NOT_DONE [k31 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{bit bit {0,1}} != 0 goto _whilecontinue_1($24)
main.c(215:112:138:0:0)		_TRISB [k10 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{sfr volatile unsigned char} := 0xf0 {literal unsigned char}
main.c(217:113:139:0:0)		iTemp17 [k32 lr113:115 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sensorSample_h_1_1}[r0x68 ] := _ADRESH [k34 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{sfr volatile unsigned char}
main.c(218:114:140:0:0)		iTemp18 [k35 lr114:117 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sensorSample_l_1_1}[r0x69 ] := _ADRESL [k37 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{sfr volatile unsigned char}
main.c(219:115:141:0:0)		iTemp19 [k38 lr115:116 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int}[r0x68 r0x6A ] = (int)iTemp17 [k32 lr113:115 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sensorSample_h_1_1}[r0x68 ]
main.c(219:116:142:0:0)		iTemp20 [k39 lr116:118 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int}[r0x68 r0x6A ] = iTemp19 [k38 lr115:116 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int}[r0x68 r0x6A ] << 0x8 {literal unsigned char}
main.c(219:117:144:0:0)		iTemp22 [k41 lr117:118 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{unsigned int}[r0x69 r0x6B ] = (unsigned int)iTemp18 [k35 lr114:117 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sensorSample_l_1_1}[r0x69 ]
main.c(219:118:145:0:0)		iTemp23 [k42 lr118:119 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{unsigned int}[r0x68 r0x6A ] = iTemp20 [k39 lr116:118 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int}[r0x68 r0x6A ] | iTemp22 [k41 lr117:118 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{unsigned int}[r0x69 r0x6B ]
main.c(219:119:147:0:0)		ret iTemp23 [k42 lr118:119 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{unsigned int}[r0x68 r0x6A ]
main.c(219:120:148:0:0)	 _return($27) :
main.c(219:121:149:0:0)		eproc _sensorSample [k1 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int function    }

----------------------------------------------------------------
Basic Block _iffalse_3 : loop Depth = 0 noPath = 0 , lastinLoop = 0
depth 1st num 6 : bbnum = 5 1st iCode = 23 , last iCode = 24
visited 0 : hasFcall = 0

defines bitVector :bitvector Size = 153 bSize = 20
Bits on { (27) }

local defines bitVector :bitvector Size = 153 bSize = 20
Bits on { (27) }

pointers Set bitvector :
in pointers Set bitvector :
inDefs Set bitvector :bitvector Size = 153 bSize = 20
Bits on { (2) (3) (4) (5) (6) (7) (8) (9) (10) (11) (15) (18) (20) }

outDefs Set bitvector :bitvector Size = 153 bSize = 20
Bits on { (2) (3) (4) (5) (6) (7) (8) (9) (10) (11) (15) (18) (20) (27) }

usesDefs Set bitvector :bitvector Size = 153 bSize = 20
Bits on { }

----------------------------------------------------------------
main.c(177:23:26:0:0)	 _iffalse_3($5) :
main.c(177:24:27:0:0)		_sensorSample_phase1_1_1 [k17 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{bit bit {0,1}} := 0x0 {literal bit {0,1}}
main.c(177:25:28:0:0)	 _ifend_3($6) :
main.c(178:26:29:0:0)		iTemp8 [k18 lr26:27 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{char} = iTemp2 [k6 lr5:33 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sensorSample_phase_1_1}[r0x65 ] == 0x2 {literal unsigned char}
main.c(178:27:30:0:0)		if iTemp8 [k18 lr26:27 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{char} == 0 goto _iffalse_4($8)
main.c(178:28:33:0:0)		_sensorSample_phase2_1_1 [k19 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{bit bit {0,1}} := 0x1 {literal bit {0,1}}
main.c(178:29:34:0:0)		 goto _ifend_4($9)
main.c(178:30:35:0:0)	 _iffalse_4($8) :
main.c(178:31:36:0:0)		_sensorSample_phase2_1_1 [k19 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{bit bit {0,1}} := 0x0 {literal bit {0,1}}
main.c(178:32:37:0:0)	 _ifend_4($9) :
main.c(179:33:38:0:0)		iTemp9 [k20 lr33:34 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{char} = iTemp2 [k6 lr5:33 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sensorSample_phase_1_1}[r0x65 ] == 0x3 {literal unsigned char}
main.c(179:34:39:0:0)		if iTemp9 [k20 lr33:34 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{char} == 0 goto _iffalse_5($11)
main.c(179:35:42:0:0)		_sensorSample_phase3_1_1 [k21 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{bit bit {0,1}} := 0x1 {literal bit {0,1}}
main.c(179:36:43:0:0)		 goto preHeaderLbl0($33)
main.c(179:37:44:0:0)	 _iffalse_5($11) :
main.c(179:38:45:0:0)		_sensorSample_phase3_1_1 [k21 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{bit bit {0,1}} := 0x0 {literal bit {0,1}}
main.c(186:40:150:0:0)	 preHeaderLbl0($33) :
main.c(186:41:152:0:0)		iTemp12 [k25 lr41:66 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sensorSample_halfcycles_1_1}[r0x67 ] := 0x3 {literal unsigned char}
main.c(186:42:48:1:0)	 _dobody_1($13) :
main.c(187:43:49:1:0)		_PORTB [k8 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{sfr volatile unsigned char} = _PORTB [k8 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{sfr volatile unsigned char} ^ iTemp0 [k2 lr3:66 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sensorSample_transmitter_1_1}[r0x63 ]
main.c(188:45:51:1:0)	 nop main.c(188:46:52:1:0)	 nop main.c(188:47:53:1:0)	 nop main.c(189:48:54:1:0)	 nop main.c(189:49:55:1:0)	 nop main.c(189:50:56:1:0)	 nop main.c(189:51:57:1:0)	 nop main.c(189:52:58:1:0)	 nop main.c(190:53:59:1:0)	 nop main.c(190:54:60:1:0)	 nop main.c(190:55:61:1:0)	 nop main.c(190:56:62:1:0)	 nop main.c(190:57:63:1:0)	 nop main.c(191:58:64:1:0)	 nop main.c(191:59:65:1:0)	 nop main.c(191:60:66:1:0)	 nop main.c(191:61:67:1:0)	 nop main.c(191:62:68:1:0)	 nop main.c(192:63:69:1:0)	 nop main.c(192:64:70:1:0)	 nop main.c(193:65:72:1:0)		iTemp12 [k25 lr41:66 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sensorSample_halfcycles_1_1}[r0x67 ] = iTemp12 [k25 lr41:66 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sensorSample_halfcycles_1_1}[r0x67 ] - 0x1 {literal unsigned char}
main.c(193:66:74:1:0)		if iTemp12 [k25 lr41:66 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sensorSample_halfcycles_1_1}[r0x67 ] != 0 goto _dobody_1($13)
main.c(196:67:77:0:0)		if _sensorSample_phase0_1_1 [k15 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{bit bit {0,1}} == 0 goto _iffalse_6($17)
main.c(197:68:81:0:0)		_GO_DONE [k26 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{bit bit {0,1}} := 0x1 {literal bit {0,1}}
main.c(197:69:82:0:0)	 _iffalse_6($17) :
main.c(198:70:83:0:0)	 nop main.c(198:71:84:0:0)	 nop main.c(198:72:85:0:0)	 nop main.c(198:73:86:0:0)	 nop main.c(198:74:87:0:0)	 nop main.c(199:75:88:0:0)	 nop main.c(199:76:89:0:0)	 nop main.c(199:77:90:0:0)	 nop main.c(199:78:91:0:0)	 nop main.c(199:79:92:0:0)	 nop main.c(200:80:93:0:0)		if _sensorSample_phase1_1_1 [k17 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{bit bit {0,1}} == 0 goto _iffalse_7($19)
main.c(201:81:97:0:0)		_GO_DONE [k26 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{bit bit {0,1}} := 0x1 {literal bit {0,1}}
main.c(201:82:98:0:0)	 _iffalse_7($19) :
main.c(202:83:99:0:0)	 nop main.c(202:84:100:0:0)	 nop main.c(202:85:101:0:0)	 nop main.c(202:86:102:0:0)	 nop main.c(202:87:103:0:0)	 nop main.c(203:88:104:0:0)	 nop main.c(203:89:105:0:0)	 nop main.c(203:90:106:0:0)	 nop main.c(203:91:107:0:0)	 nop main.c(203:92:108:0:0)	 nop main.c(203:93:109:0:0)	 nop main.c(204:94:110:0:0)		if _sensorSample_phase2_1_1 [k19 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{bit bit {0,1}} == 0 goto _iffalse_8($21)
main.c(205:95:114:0:0)		_GO_DONE [k26 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{bit bit {0,1}} := 0x1 {literal bit {0,1}}
main.c(205:96:115:0:0)	 _iffalse_8($21) :
main.c(206:97:116:0:0)	 nop main.c(206:98:117:0:0)	 nop main.c(206:99:118:0:0)	 nop main.c(206:100:119:0:0)	 nop main.c(206:101:120:0:0)	 nop main.c(207:102:121:0:0)	 nop main.c(207:103:122:0:0)	 nop main.c(207:104:123:0:0)	 nop main.c(207:105:124:0:0)	 nop main.c(207:106:125:0:0)	 nop main.c(208:107:126:0:0)		if _sensorSample_phase3_1_1 [k21 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{bit bit {0,1}} == 0 goto _whilecontinue_1($24)
main.c(209:108:130:0:0)		_GO_DONE [k26 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{bit bit {0,1}} := 0x1 {literal bit {0,1}}
main.c(212:110:132:1:0)	 _whilecontinue_1($24) :
main.c(212:111:133:1:0)		if _NOT_DONE [k31 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{bit bit {0,1}} != 0 goto _whilecontinue_1($24)
main.c(215:112:138:0:0)		_TRISB [k10 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{sfr volatile unsigned char} := 0xf0 {literal unsigned char}
main.c(217:113:139:0:0)		iTemp17 [k32 lr113:115 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sensorSample_h_1_1}[r0x68 ] := _ADRESH [k34 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{sfr volatile unsigned char}
main.c(218:114:140:0:0)		iTemp18 [k35 lr114:117 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sensorSample_l_1_1}[r0x69 ] := _ADRESL [k37 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{sfr volatile unsigned char}
main.c(219:115:141:0:0)		iTemp19 [k38 lr115:116 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int}[r0x68 r0x6A ] = (int)iTemp17 [k32 lr113:115 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sensorSample_h_1_1}[r0x68 ]
main.c(219:116:142:0:0)		iTemp20 [k39 lr116:118 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int}[r0x68 r0x6A ] = iTemp19 [k38 lr115:116 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int}[r0x68 r0x6A ] << 0x8 {literal unsigned char}
main.c(219:117:144:0:0)		iTemp22 [k41 lr117:118 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{unsigned int}[r0x69 r0x6B ] = (unsigned int)iTemp18 [k35 lr114:117 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sensorSample_l_1_1}[r0x69 ]
main.c(219:118:145:0:0)		iTemp23 [k42 lr118:119 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{unsigned int}[r0x68 r0x6A ] = iTemp20 [k39 lr116:118 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int}[r0x68 r0x6A ] | iTemp22 [k41 lr117:118 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{unsigned int}[r0x69 r0x6B ]
main.c(219:119:147:0:0)		ret iTemp23 [k42 lr118:119 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{unsigned int}[r0x68 r0x6A ]
main.c(219:120:148:0:0)	 _return($27) :
main.c(219:121:149:0:0)		eproc _sensorSample [k1 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int function    }

----------------------------------------------------------------
Basic Block _ifend_3 : loop Depth = 0 noPath = 0 , lastinLoop = 0
depth 1st num 7 : bbnum = 6 1st iCode = 25 , last iCode = 27
visited 0 : hasFcall = 0

defines bitVector :bitvector Size = 153 bSize = 20
Bits on { (29) }

local defines bitVector :bitvector Size = 153 bSize = 20
Bits on { (29) }

pointers Set bitvector :
in pointers Set bitvector :
inDefs Set bitvector :bitvector Size = 153 bSize = 20
Bits on { (2) (3) (4) (5) (6) (7) (8) (9) (10) (11) (15) (18) (20) (24) (27) }

outDefs Set bitvector :bitvector Size = 153 bSize = 20
Bits on { (2) (3) (4) (5) (6) (7) (8) (9) (10) (11) (15) (18) (20) (24) (27) (29) }

usesDefs Set bitvector :bitvector Size = 153 bSize = 20
Bits on { (4) (29) }

----------------------------------------------------------------
main.c(177:25:28:0:0)	 _ifend_3($6) :
main.c(178:26:29:0:0)		iTemp8 [k18 lr26:27 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{char} = iTemp2 [k6 lr5:33 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sensorSample_phase_1_1}[r0x65 ] == 0x2 {literal unsigned char}
main.c(178:27:30:0:0)		if iTemp8 [k18 lr26:27 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{char} == 0 goto _iffalse_4($8)
main.c(178:28:33:0:0)		_sensorSample_phase2_1_1 [k19 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{bit bit {0,1}} := 0x1 {literal bit {0,1}}
main.c(178:29:34:0:0)		 goto _ifend_4($9)
main.c(178:30:35:0:0)	 _iffalse_4($8) :
main.c(178:31:36:0:0)		_sensorSample_phase2_1_1 [k19 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{bit bit {0,1}} := 0x0 {literal bit {0,1}}
main.c(178:32:37:0:0)	 _ifend_4($9) :
main.c(179:33:38:0:0)		iTemp9 [k20 lr33:34 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{char} = iTemp2 [k6 lr5:33 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sensorSample_phase_1_1}[r0x65 ] == 0x3 {literal unsigned char}
main.c(179:34:39:0:0)		if iTemp9 [k20 lr33:34 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{char} == 0 goto _iffalse_5($11)
main.c(179:35:42:0:0)		_sensorSample_phase3_1_1 [k21 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{bit bit {0,1}} := 0x1 {literal bit {0,1}}
main.c(179:36:43:0:0)		 goto preHeaderLbl0($33)
main.c(179:37:44:0:0)	 _iffalse_5($11) :
main.c(179:38:45:0:0)		_sensorSample_phase3_1_1 [k21 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{bit bit {0,1}} := 0x0 {literal bit {0,1}}
main.c(186:40:150:0:0)	 preHeaderLbl0($33) :
main.c(186:41:152:0:0)		iTemp12 [k25 lr41:66 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sensorSample_halfcycles_1_1}[r0x67 ] := 0x3 {literal unsigned char}
main.c(186:42:48:1:0)	 _dobody_1($13) :
main.c(187:43:49:1:0)		_PORTB [k8 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{sfr volatile unsigned char} = _PORTB [k8 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{sfr volatile unsigned char} ^ iTemp0 [k2 lr3:66 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sensorSample_transmitter_1_1}[r0x63 ]
main.c(188:45:51:1:0)	 nop main.c(188:46:52:1:0)	 nop main.c(188:47:53:1:0)	 nop main.c(189:48:54:1:0)	 nop main.c(189:49:55:1:0)	 nop main.c(189:50:56:1:0)	 nop main.c(189:51:57:1:0)	 nop main.c(189:52:58:1:0)	 nop main.c(190:53:59:1:0)	 nop main.c(190:54:60:1:0)	 nop main.c(190:55:61:1:0)	 nop main.c(190:56:62:1:0)	 nop main.c(190:57:63:1:0)	 nop main.c(191:58:64:1:0)	 nop main.c(191:59:65:1:0)	 nop main.c(191:60:66:1:0)	 nop main.c(191:61:67:1:0)	 nop main.c(191:62:68:1:0)	 nop main.c(192:63:69:1:0)	 nop main.c(192:64:70:1:0)	 nop main.c(193:65:72:1:0)		iTemp12 [k25 lr41:66 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sensorSample_halfcycles_1_1}[r0x67 ] = iTemp12 [k25 lr41:66 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sensorSample_halfcycles_1_1}[r0x67 ] - 0x1 {literal unsigned char}
main.c(193:66:74:1:0)		if iTemp12 [k25 lr41:66 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sensorSample_halfcycles_1_1}[r0x67 ] != 0 goto _dobody_1($13)
main.c(196:67:77:0:0)		if _sensorSample_phase0_1_1 [k15 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{bit bit {0,1}} == 0 goto _iffalse_6($17)
main.c(197:68:81:0:0)		_GO_DONE [k26 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{bit bit {0,1}} := 0x1 {literal bit {0,1}}
main.c(197:69:82:0:0)	 _iffalse_6($17) :
main.c(198:70:83:0:0)	 nop main.c(198:71:84:0:0)	 nop main.c(198:72:85:0:0)	 nop main.c(198:73:86:0:0)	 nop main.c(198:74:87:0:0)	 nop main.c(199:75:88:0:0)	 nop main.c(199:76:89:0:0)	 nop main.c(199:77:90:0:0)	 nop main.c(199:78:91:0:0)	 nop main.c(199:79:92:0:0)	 nop main.c(200:80:93:0:0)		if _sensorSample_phase1_1_1 [k17 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{bit bit {0,1}} == 0 goto _iffalse_7($19)
main.c(201:81:97:0:0)		_GO_DONE [k26 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{bit bit {0,1}} := 0x1 {literal bit {0,1}}
main.c(201:82:98:0:0)	 _iffalse_7($19) :
main.c(202:83:99:0:0)	 nop main.c(202:84:100:0:0)	 nop main.c(202:85:101:0:0)	 nop main.c(202:86:102:0:0)	 nop main.c(202:87:103:0:0)	 nop main.c(203:88:104:0:0)	 nop main.c(203:89:105:0:0)	 nop main.c(203:90:106:0:0)	 nop main.c(203:91:107:0:0)	 nop main.c(203:92:108:0:0)	 nop main.c(203:93:109:0:0)	 nop main.c(204:94:110:0:0)		if _sensorSample_phase2_1_1 [k19 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{bit bit {0,1}} == 0 goto _iffalse_8($21)
main.c(205:95:114:0:0)		_GO_DONE [k26 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{bit bit {0,1}} := 0x1 {literal bit {0,1}}
main.c(205:96:115:0:0)	 _iffalse_8($21) :
main.c(206:97:116:0:0)	 nop main.c(206:98:117:0:0)	 nop main.c(206:99:118:0:0)	 nop main.c(206:100:119:0:0)	 nop main.c(206:101:120:0:0)	 nop main.c(207:102:121:0:0)	 nop main.c(207:103:122:0:0)	 nop main.c(207:104:123:0:0)	 nop main.c(207:105:124:0:0)	 nop main.c(207:106:125:0:0)	 nop main.c(208:107:126:0:0)		if _sensorSample_phase3_1_1 [k21 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{bit bit {0,1}} == 0 goto _whilecontinue_1($24)
main.c(209:108:130:0:0)		_GO_DONE [k26 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{bit bit {0,1}} := 0x1 {literal bit {0,1}}
main.c(212:110:132:1:0)	 _whilecontinue_1($24) :
main.c(212:111:133:1:0)		if _NOT_DONE [k31 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{bit bit {0,1}} != 0 goto _whilecontinue_1($24)
main.c(215:112:138:0:0)		_TRISB [k10 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{sfr volatile unsigned char} := 0xf0 {literal unsigned char}
main.c(217:113:139:0:0)		iTemp17 [k32 lr113:115 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sensorSample_h_1_1}[r0x68 ] := _ADRESH [k34 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{sfr volatile unsigned char}
main.c(218:114:140:0:0)		iTemp18 [k35 lr114:117 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sensorSample_l_1_1}[r0x69 ] := _ADRESL [k37 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{sfr volatile unsigned char}
main.c(219:115:141:0:0)		iTemp19 [k38 lr115:116 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int}[r0x68 r0x6A ] = (int)iTemp17 [k32 lr113:115 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sensorSample_h_1_1}[r0x68 ]
main.c(219:116:142:0:0)		iTemp20 [k39 lr116:118 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int}[r0x68 r0x6A ] = iTemp19 [k38 lr115:116 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int}[r0x68 r0x6A ] << 0x8 {literal unsigned char}
main.c(219:117:144:0:0)		iTemp22 [k41 lr117:118 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{unsigned int}[r0x69 r0x6B ] = (unsigned int)iTemp18 [k35 lr114:117 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sensorSample_l_1_1}[r0x69 ]
main.c(219:118:145:0:0)		iTemp23 [k42 lr118:119 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{unsigned int}[r0x68 r0x6A ] = iTemp20 [k39 lr116:118 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int}[r0x68 r0x6A ] | iTemp22 [k41 lr117:118 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{unsigned int}[r0x69 r0x6B ]
main.c(219:119:147:0:0)		ret iTemp23 [k42 lr118:119 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{unsigned int}[r0x68 r0x6A ]
main.c(219:120:148:0:0)	 _return($27) :
main.c(219:121:149:0:0)		eproc _sensorSample [k1 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int function    }

----------------------------------------------------------------
Basic Block _eBBlock2 : loop Depth = 0 noPath = 0 , lastinLoop = 0
depth 1st num 8 : bbnum = 7 1st iCode = 28 , last iCode = 29
visited 0 : hasFcall = 0

defines bitVector :bitvector Size = 153 bSize = 20
Bits on { (33) }

local defines bitVector :bitvector Size = 153 bSize = 20
Bits on { (33) }

pointers Set bitvector :
in pointers Set bitvector :
inDefs Set bitvector :bitvector Size = 153 bSize = 20
Bits on { (2) (3) (4) (5) (6) (7) (8) (9) (10) (11) (15) (18) (20) (24) (27) (29) }

outDefs Set bitvector :bitvector Size = 153 bSize = 20
Bits on { (2) (3) (4) (5) (6) (7) (8) (9) (10) (11) (15) (18) (20) (24) (27) (29) (33) }

usesDefs Set bitvector :bitvector Size = 153 bSize = 20
Bits on { }

----------------------------------------------------------------
main.c(178:28:33:0:0)		_sensorSample_phase2_1_1 [k19 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{bit bit {0,1}} := 0x1 {literal bit {0,1}}
main.c(178:29:34:0:0)		 goto _ifend_4($9)
main.c(178:30:35:0:0)	 _iffalse_4($8) :
main.c(178:31:36:0:0)		_sensorSample_phase2_1_1 [k19 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{bit bit {0,1}} := 0x0 {literal bit {0,1}}
main.c(178:32:37:0:0)	 _ifend_4($9) :
main.c(179:33:38:0:0)		iTemp9 [k20 lr33:34 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{char} = iTemp2 [k6 lr5:33 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sensorSample_phase_1_1}[r0x65 ] == 0x3 {literal unsigned char}
main.c(179:34:39:0:0)		if iTemp9 [k20 lr33:34 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{char} == 0 goto _iffalse_5($11)
main.c(179:35:42:0:0)		_sensorSample_phase3_1_1 [k21 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{bit bit {0,1}} := 0x1 {literal bit {0,1}}
main.c(179:36:43:0:0)		 goto preHeaderLbl0($33)
main.c(179:37:44:0:0)	 _iffalse_5($11) :
main.c(179:38:45:0:0)		_sensorSample_phase3_1_1 [k21 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{bit bit {0,1}} := 0x0 {literal bit {0,1}}
main.c(186:40:150:0:0)	 preHeaderLbl0($33) :
main.c(186:41:152:0:0)		iTemp12 [k25 lr41:66 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sensorSample_halfcycles_1_1}[r0x67 ] := 0x3 {literal unsigned char}
main.c(186:42:48:1:0)	 _dobody_1($13) :
main.c(187:43:49:1:0)		_PORTB [k8 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{sfr volatile unsigned char} = _PORTB [k8 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{sfr volatile unsigned char} ^ iTemp0 [k2 lr3:66 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sensorSample_transmitter_1_1}[r0x63 ]
main.c(188:45:51:1:0)	 nop main.c(188:46:52:1:0)	 nop main.c(188:47:53:1:0)	 nop main.c(189:48:54:1:0)	 nop main.c(189:49:55:1:0)	 nop main.c(189:50:56:1:0)	 nop main.c(189:51:57:1:0)	 nop main.c(189:52:58:1:0)	 nop main.c(190:53:59:1:0)	 nop main.c(190:54:60:1:0)	 nop main.c(190:55:61:1:0)	 nop main.c(190:56:62:1:0)	 nop main.c(190:57:63:1:0)	 nop main.c(191:58:64:1:0)	 nop main.c(191:59:65:1:0)	 nop main.c(191:60:66:1:0)	 nop main.c(191:61:67:1:0)	 nop main.c(191:62:68:1:0)	 nop main.c(192:63:69:1:0)	 nop main.c(192:64:70:1:0)	 nop main.c(193:65:72:1:0)		iTemp12 [k25 lr41:66 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sensorSample_halfcycles_1_1}[r0x67 ] = iTemp12 [k25 lr41:66 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sensorSample_halfcycles_1_1}[r0x67 ] - 0x1 {literal unsigned char}
main.c(193:66:74:1:0)		if iTemp12 [k25 lr41:66 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sensorSample_halfcycles_1_1}[r0x67 ] != 0 goto _dobody_1($13)
main.c(196:67:77:0:0)		if _sensorSample_phase0_1_1 [k15 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{bit bit {0,1}} == 0 goto _iffalse_6($17)
main.c(197:68:81:0:0)		_GO_DONE [k26 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{bit bit {0,1}} := 0x1 {literal bit {0,1}}
main.c(197:69:82:0:0)	 _iffalse_6($17) :
main.c(198:70:83:0:0)	 nop main.c(198:71:84:0:0)	 nop main.c(198:72:85:0:0)	 nop main.c(198:73:86:0:0)	 nop main.c(198:74:87:0:0)	 nop main.c(199:75:88:0:0)	 nop main.c(199:76:89:0:0)	 nop main.c(199:77:90:0:0)	 nop main.c(199:78:91:0:0)	 nop main.c(199:79:92:0:0)	 nop main.c(200:80:93:0:0)		if _sensorSample_phase1_1_1 [k17 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{bit bit {0,1}} == 0 goto _iffalse_7($19)
main.c(201:81:97:0:0)		_GO_DONE [k26 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{bit bit {0,1}} := 0x1 {literal bit {0,1}}
main.c(201:82:98:0:0)	 _iffalse_7($19) :
main.c(202:83:99:0:0)	 nop main.c(202:84:100:0:0)	 nop main.c(202:85:101:0:0)	 nop main.c(202:86:102:0:0)	 nop main.c(202:87:103:0:0)	 nop main.c(203:88:104:0:0)	 nop main.c(203:89:105:0:0)	 nop main.c(203:90:106:0:0)	 nop main.c(203:91:107:0:0)	 nop main.c(203:92:108:0:0)	 nop main.c(203:93:109:0:0)	 nop main.c(204:94:110:0:0)		if _sensorSample_phase2_1_1 [k19 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{bit bit {0,1}} == 0 goto _iffalse_8($21)
main.c(205:95:114:0:0)		_GO_DONE [k26 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{bit bit {0,1}} := 0x1 {literal bit {0,1}}
main.c(205:96:115:0:0)	 _iffalse_8($21) :
main.c(206:97:116:0:0)	 nop main.c(206:98:117:0:0)	 nop main.c(206:99:118:0:0)	 nop main.c(206:100:119:0:0)	 nop main.c(206:101:120:0:0)	 nop main.c(207:102:121:0:0)	 nop main.c(207:103:122:0:0)	 nop main.c(207:104:123:0:0)	 nop main.c(207:105:124:0:0)	 nop main.c(207:106:125:0:0)	 nop main.c(208:107:126:0:0)		if _sensorSample_phase3_1_1 [k21 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{bit bit {0,1}} == 0 goto _whilecontinue_1($24)
main.c(209:108:130:0:0)		_GO_DONE [k26 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{bit bit {0,1}} := 0x1 {literal bit {0,1}}
main.c(212:110:132:1:0)	 _whilecontinue_1($24) :
main.c(212:111:133:1:0)		if _NOT_DONE [k31 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{bit bit {0,1}} != 0 goto _whilecontinue_1($24)
main.c(215:112:138:0:0)		_TRISB [k10 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{sfr volatile unsigned char} := 0xf0 {literal unsigned char}
main.c(217:113:139:0:0)		iTemp17 [k32 lr113:115 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sensorSample_h_1_1}[r0x68 ] := _ADRESH [k34 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{sfr volatile unsigned char}
main.c(218:114:140:0:0)		iTemp18 [k35 lr114:117 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sensorSample_l_1_1}[r0x69 ] := _ADRESL [k37 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{sfr volatile unsigned char}
main.c(219:115:141:0:0)		iTemp19 [k38 lr115:116 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int}[r0x68 r0x6A ] = (int)iTemp17 [k32 lr113:115 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sensorSample_h_1_1}[r0x68 ]
main.c(219:116:142:0:0)		iTemp20 [k39 lr116:118 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int}[r0x68 r0x6A ] = iTemp19 [k38 lr115:116 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int}[r0x68 r0x6A ] << 0x8 {literal unsigned char}
main.c(219:117:144:0:0)		iTemp22 [k41 lr117:118 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{unsigned int}[r0x69 r0x6B ] = (unsigned int)iTemp18 [k35 lr114:117 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sensorSample_l_1_1}[r0x69 ]
main.c(219:118:145:0:0)		iTemp23 [k42 lr118:119 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{unsigned int}[r0x68 r0x6A ] = iTemp20 [k39 lr116:118 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int}[r0x68 r0x6A ] | iTemp22 [k41 lr117:118 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{unsigned int}[r0x69 r0x6B ]
main.c(219:119:147:0:0)		ret iTemp23 [k42 lr118:119 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{unsigned int}[r0x68 r0x6A ]
main.c(219:120:148:0:0)	 _return($27) :
main.c(219:121:149:0:0)		eproc _sensorSample [k1 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int function    }

----------------------------------------------------------------
Basic Block _iffalse_4 : loop Depth = 0 noPath = 0 , lastinLoop = 0
depth 1st num 9 : bbnum = 8 1st iCode = 30 , last iCode = 31
visited 0 : hasFcall = 0

defines bitVector :bitvector Size = 153 bSize = 20
Bits on { (36) }

local defines bitVector :bitvector Size = 153 bSize = 20
Bits on { (36) }

pointers Set bitvector :
in pointers Set bitvector :
inDefs Set bitvector :bitvector Size = 153 bSize = 20
Bits on { (2) (3) (4) (5) (6) (7) (8) (9) (10) (11) (15) (18) (20) (24) (27) (29) }

outDefs Set bitvector :bitvector Size = 153 bSize = 20
Bits on { (2) (3) (4) (5) (6) (7) (8) (9) (10) (11) (15) (18) (20) (24) (27) (29) (36) }

usesDefs Set bitvector :bitvector Size = 153 bSize = 20
Bits on { }

----------------------------------------------------------------
main.c(178:30:35:0:0)	 _iffalse_4($8) :
main.c(178:31:36:0:0)		_sensorSample_phase2_1_1 [k19 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{bit bit {0,1}} := 0x0 {literal bit {0,1}}
main.c(178:32:37:0:0)	 _ifend_4($9) :
main.c(179:33:38:0:0)		iTemp9 [k20 lr33:34 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{char} = iTemp2 [k6 lr5:33 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sensorSample_phase_1_1}[r0x65 ] == 0x3 {literal unsigned char}
main.c(179:34:39:0:0)		if iTemp9 [k20 lr33:34 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{char} == 0 goto _iffalse_5($11)
main.c(179:35:42:0:0)		_sensorSample_phase3_1_1 [k21 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{bit bit {0,1}} := 0x1 {literal bit {0,1}}
main.c(179:36:43:0:0)		 goto preHeaderLbl0($33)
main.c(179:37:44:0:0)	 _iffalse_5($11) :
main.c(179:38:45:0:0)		_sensorSample_phase3_1_1 [k21 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{bit bit {0,1}} := 0x0 {literal bit {0,1}}
main.c(186:40:150:0:0)	 preHeaderLbl0($33) :
main.c(186:41:152:0:0)		iTemp12 [k25 lr41:66 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sensorSample_halfcycles_1_1}[r0x67 ] := 0x3 {literal unsigned char}
main.c(186:42:48:1:0)	 _dobody_1($13) :
main.c(187:43:49:1:0)		_PORTB [k8 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{sfr volatile unsigned char} = _PORTB [k8 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{sfr volatile unsigned char} ^ iTemp0 [k2 lr3:66 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sensorSample_transmitter_1_1}[r0x63 ]
main.c(188:45:51:1:0)	 nop main.c(188:46:52:1:0)	 nop main.c(188:47:53:1:0)	 nop main.c(189:48:54:1:0)	 nop main.c(189:49:55:1:0)	 nop main.c(189:50:56:1:0)	 nop main.c(189:51:57:1:0)	 nop main.c(189:52:58:1:0)	 nop main.c(190:53:59:1:0)	 nop main.c(190:54:60:1:0)	 nop main.c(190:55:61:1:0)	 nop main.c(190:56:62:1:0)	 nop main.c(190:57:63:1:0)	 nop main.c(191:58:64:1:0)	 nop main.c(191:59:65:1:0)	 nop main.c(191:60:66:1:0)	 nop main.c(191:61:67:1:0)	 nop main.c(191:62:68:1:0)	 nop main.c(192:63:69:1:0)	 nop main.c(192:64:70:1:0)	 nop main.c(193:65:72:1:0)		iTemp12 [k25 lr41:66 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sensorSample_halfcycles_1_1}[r0x67 ] = iTemp12 [k25 lr41:66 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sensorSample_halfcycles_1_1}[r0x67 ] - 0x1 {literal unsigned char}
main.c(193:66:74:1:0)		if iTemp12 [k25 lr41:66 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sensorSample_halfcycles_1_1}[r0x67 ] != 0 goto _dobody_1($13)
main.c(196:67:77:0:0)		if _sensorSample_phase0_1_1 [k15 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{bit bit {0,1}} == 0 goto _iffalse_6($17)
main.c(197:68:81:0:0)		_GO_DONE [k26 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{bit bit {0,1}} := 0x1 {literal bit {0,1}}
main.c(197:69:82:0:0)	 _iffalse_6($17) :
main.c(198:70:83:0:0)	 nop main.c(198:71:84:0:0)	 nop main.c(198:72:85:0:0)	 nop main.c(198:73:86:0:0)	 nop main.c(198:74:87:0:0)	 nop main.c(199:75:88:0:0)	 nop main.c(199:76:89:0:0)	 nop main.c(199:77:90:0:0)	 nop main.c(199:78:91:0:0)	 nop main.c(199:79:92:0:0)	 nop main.c(200:80:93:0:0)		if _sensorSample_phase1_1_1 [k17 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{bit bit {0,1}} == 0 goto _iffalse_7($19)
main.c(201:81:97:0:0)		_GO_DONE [k26 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{bit bit {0,1}} := 0x1 {literal bit {0,1}}
main.c(201:82:98:0:0)	 _iffalse_7($19) :
main.c(202:83:99:0:0)	 nop main.c(202:84:100:0:0)	 nop main.c(202:85:101:0:0)	 nop main.c(202:86:102:0:0)	 nop main.c(202:87:103:0:0)	 nop main.c(203:88:104:0:0)	 nop main.c(203:89:105:0:0)	 nop main.c(203:90:106:0:0)	 nop main.c(203:91:107:0:0)	 nop main.c(203:92:108:0:0)	 nop main.c(203:93:109:0:0)	 nop main.c(204:94:110:0:0)		if _sensorSample_phase2_1_1 [k19 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{bit bit {0,1}} == 0 goto _iffalse_8($21)
main.c(205:95:114:0:0)		_GO_DONE [k26 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{bit bit {0,1}} := 0x1 {literal bit {0,1}}
main.c(205:96:115:0:0)	 _iffalse_8($21) :
main.c(206:97:116:0:0)	 nop main.c(206:98:117:0:0)	 nop main.c(206:99:118:0:0)	 nop main.c(206:100:119:0:0)	 nop main.c(206:101:120:0:0)	 nop main.c(207:102:121:0:0)	 nop main.c(207:103:122:0:0)	 nop main.c(207:104:123:0:0)	 nop main.c(207:105:124:0:0)	 nop main.c(207:106:125:0:0)	 nop main.c(208:107:126:0:0)		if _sensorSample_phase3_1_1 [k21 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{bit bit {0,1}} == 0 goto _whilecontinue_1($24)
main.c(209:108:130:0:0)		_GO_DONE [k26 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{bit bit {0,1}} := 0x1 {literal bit {0,1}}
main.c(212:110:132:1:0)	 _whilecontinue_1($24) :
main.c(212:111:133:1:0)		if _NOT_DONE [k31 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{bit bit {0,1}} != 0 goto _whilecontinue_1($24)
main.c(215:112:138:0:0)		_TRISB [k10 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{sfr volatile unsigned char} := 0xf0 {literal unsigned char}
main.c(217:113:139:0:0)		iTemp17 [k32 lr113:115 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sensorSample_h_1_1}[r0x68 ] := _ADRESH [k34 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{sfr volatile unsigned char}
main.c(218:114:140:0:0)		iTemp18 [k35 lr114:117 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sensorSample_l_1_1}[r0x69 ] := _ADRESL [k37 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{sfr volatile unsigned char}
main.c(219:115:141:0:0)		iTemp19 [k38 lr115:116 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int}[r0x68 r0x6A ] = (int)iTemp17 [k32 lr113:115 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sensorSample_h_1_1}[r0x68 ]
main.c(219:116:142:0:0)		iTemp20 [k39 lr116:118 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int}[r0x68 r0x6A ] = iTemp19 [k38 lr115:116 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int}[r0x68 r0x6A ] << 0x8 {literal unsigned char}
main.c(219:117:144:0:0)		iTemp22 [k41 lr117:118 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{unsigned int}[r0x69 r0x6B ] = (unsigned int)iTemp18 [k35 lr114:117 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sensorSample_l_1_1}[r0x69 ]
main.c(219:118:145:0:0)		iTemp23 [k42 lr118:119 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{unsigned int}[r0x68 r0x6A ] = iTemp20 [k39 lr116:118 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int}[r0x68 r0x6A ] | iTemp22 [k41 lr117:118 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{unsigned int}[r0x69 r0x6B ]
main.c(219:119:147:0:0)		ret iTemp23 [k42 lr118:119 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{unsigned int}[r0x68 r0x6A ]
main.c(219:120:148:0:0)	 _return($27) :
main.c(219:121:149:0:0)		eproc _sensorSample [k1 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int function    }

----------------------------------------------------------------
Basic Block _ifend_4 : loop Depth = 0 noPath = 0 , lastinLoop = 0
depth 1st num 10 : bbnum = 9 1st iCode = 32 , last iCode = 34
visited 0 : hasFcall = 0

defines bitVector :bitvector Size = 153 bSize = 20
Bits on { (38) }

local defines bitVector :bitvector Size = 153 bSize = 20
Bits on { (38) }

pointers Set bitvector :
in pointers Set bitvector :
inDefs Set bitvector :bitvector Size = 153 bSize = 20
Bits on { (2) (3) (4) (5) (6) (7) (8) (9) (10) (11) (15) (18) (20) (24) (27) (29) (33) (36) }

outDefs Set bitvector :bitvector Size = 153 bSize = 20
Bits on { (2) (3) (4) (5) (6) (7) (8) (9) (10) (11) (15) (18) (20) (24) (27) (29) (33) (36) (38) }

usesDefs Set bitvector :bitvector Size = 153 bSize = 20
Bits on { (4) (38) }

----------------------------------------------------------------
main.c(178:32:37:0:0)	 _ifend_4($9) :
main.c(179:33:38:0:0)		iTemp9 [k20 lr33:34 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{char} = iTemp2 [k6 lr5:33 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sensorSample_phase_1_1}[r0x65 ] == 0x3 {literal unsigned char}
main.c(179:34:39:0:0)		if iTemp9 [k20 lr33:34 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{char} == 0 goto _iffalse_5($11)
main.c(179:35:42:0:0)		_sensorSample_phase3_1_1 [k21 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{bit bit {0,1}} := 0x1 {literal bit {0,1}}
main.c(179:36:43:0:0)		 goto preHeaderLbl0($33)
main.c(179:37:44:0:0)	 _iffalse_5($11) :
main.c(179:38:45:0:0)		_sensorSample_phase3_1_1 [k21 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{bit bit {0,1}} := 0x0 {literal bit {0,1}}
main.c(186:40:150:0:0)	 preHeaderLbl0($33) :
main.c(186:41:152:0:0)		iTemp12 [k25 lr41:66 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sensorSample_halfcycles_1_1}[r0x67 ] := 0x3 {literal unsigned char}
main.c(186:42:48:1:0)	 _dobody_1($13) :
main.c(187:43:49:1:0)		_PORTB [k8 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{sfr volatile unsigned char} = _PORTB [k8 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{sfr volatile unsigned char} ^ iTemp0 [k2 lr3:66 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sensorSample_transmitter_1_1}[r0x63 ]
main.c(188:45:51:1:0)	 nop main.c(188:46:52:1:0)	 nop main.c(188:47:53:1:0)	 nop main.c(189:48:54:1:0)	 nop main.c(189:49:55:1:0)	 nop main.c(189:50:56:1:0)	 nop main.c(189:51:57:1:0)	 nop main.c(189:52:58:1:0)	 nop main.c(190:53:59:1:0)	 nop main.c(190:54:60:1:0)	 nop main.c(190:55:61:1:0)	 nop main.c(190:56:62:1:0)	 nop main.c(190:57:63:1:0)	 nop main.c(191:58:64:1:0)	 nop main.c(191:59:65:1:0)	 nop main.c(191:60:66:1:0)	 nop main.c(191:61:67:1:0)	 nop main.c(191:62:68:1:0)	 nop main.c(192:63:69:1:0)	 nop main.c(192:64:70:1:0)	 nop main.c(193:65:72:1:0)		iTemp12 [k25 lr41:66 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sensorSample_halfcycles_1_1}[r0x67 ] = iTemp12 [k25 lr41:66 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sensorSample_halfcycles_1_1}[r0x67 ] - 0x1 {literal unsigned char}
main.c(193:66:74:1:0)		if iTemp12 [k25 lr41:66 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sensorSample_halfcycles_1_1}[r0x67 ] != 0 goto _dobody_1($13)
main.c(196:67:77:0:0)		if _sensorSample_phase0_1_1 [k15 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{bit bit {0,1}} == 0 goto _iffalse_6($17)
main.c(197:68:81:0:0)		_GO_DONE [k26 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{bit bit {0,1}} := 0x1 {literal bit {0,1}}
main.c(197:69:82:0:0)	 _iffalse_6($17) :
main.c(198:70:83:0:0)	 nop main.c(198:71:84:0:0)	 nop main.c(198:72:85:0:0)	 nop main.c(198:73:86:0:0)	 nop main.c(198:74:87:0:0)	 nop main.c(199:75:88:0:0)	 nop main.c(199:76:89:0:0)	 nop main.c(199:77:90:0:0)	 nop main.c(199:78:91:0:0)	 nop main.c(199:79:92:0:0)	 nop main.c(200:80:93:0:0)		if _sensorSample_phase1_1_1 [k17 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{bit bit {0,1}} == 0 goto _iffalse_7($19)
main.c(201:81:97:0:0)		_GO_DONE [k26 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{bit bit {0,1}} := 0x1 {literal bit {0,1}}
main.c(201:82:98:0:0)	 _iffalse_7($19) :
main.c(202:83:99:0:0)	 nop main.c(202:84:100:0:0)	 nop main.c(202:85:101:0:0)	 nop main.c(202:86:102:0:0)	 nop main.c(202:87:103:0:0)	 nop main.c(203:88:104:0:0)	 nop main.c(203:89:105:0:0)	 nop main.c(203:90:106:0:0)	 nop main.c(203:91:107:0:0)	 nop main.c(203:92:108:0:0)	 nop main.c(203:93:109:0:0)	 nop main.c(204:94:110:0:0)		if _sensorSample_phase2_1_1 [k19 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{bit bit {0,1}} == 0 goto _iffalse_8($21)
main.c(205:95:114:0:0)		_GO_DONE [k26 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{bit bit {0,1}} := 0x1 {literal bit {0,1}}
main.c(205:96:115:0:0)	 _iffalse_8($21) :
main.c(206:97:116:0:0)	 nop main.c(206:98:117:0:0)	 nop main.c(206:99:118:0:0)	 nop main.c(206:100:119:0:0)	 nop main.c(206:101:120:0:0)	 nop main.c(207:102:121:0:0)	 nop main.c(207:103:122:0:0)	 nop main.c(207:104:123:0:0)	 nop main.c(207:105:124:0:0)	 nop main.c(207:106:125:0:0)	 nop main.c(208:107:126:0:0)		if _sensorSample_phase3_1_1 [k21 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{bit bit {0,1}} == 0 goto _whilecontinue_1($24)
main.c(209:108:130:0:0)		_GO_DONE [k26 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{bit bit {0,1}} := 0x1 {literal bit {0,1}}
main.c(212:110:132:1:0)	 _whilecontinue_1($24) :
main.c(212:111:133:1:0)		if _NOT_DONE [k31 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{bit bit {0,1}} != 0 goto _whilecontinue_1($24)
main.c(215:112:138:0:0)		_TRISB [k10 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{sfr volatile unsigned char} := 0xf0 {literal unsigned char}
main.c(217:113:139:0:0)		iTemp17 [k32 lr113:115 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sensorSample_h_1_1}[r0x68 ] := _ADRESH [k34 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{sfr volatile unsigned char}
main.c(218:114:140:0:0)		iTemp18 [k35 lr114:117 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sensorSample_l_1_1}[r0x69 ] := _ADRESL [k37 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{sfr volatile unsigned char}
main.c(219:115:141:0:0)		iTemp19 [k38 lr115:116 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int}[r0x68 r0x6A ] = (int)iTemp17 [k32 lr113:115 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sensorSample_h_1_1}[r0x68 ]
main.c(219:116:142:0:0)		iTemp20 [k39 lr116:118 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int}[r0x68 r0x6A ] = iTemp19 [k38 lr115:116 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int}[r0x68 r0x6A ] << 0x8 {literal unsigned char}
main.c(219:117:144:0:0)		iTemp22 [k41 lr117:118 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{unsigned int}[r0x69 r0x6B ] = (unsigned int)iTemp18 [k35 lr114:117 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sensorSample_l_1_1}[r0x69 ]
main.c(219:118:145:0:0)		iTemp23 [k42 lr118:119 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{unsigned int}[r0x68 r0x6A ] = iTemp20 [k39 lr116:118 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int}[r0x68 r0x6A ] | iTemp22 [k41 lr117:118 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{unsigned int}[r0x69 r0x6B ]
main.c(219:119:147:0:0)		ret iTemp23 [k42 lr118:119 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{unsigned int}[r0x68 r0x6A ]
main.c(219:120:148:0:0)	 _return($27) :
main.c(219:121:149:0:0)		eproc _sensorSample [k1 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int function    }

----------------------------------------------------------------
Basic Block _eBBlock3 : loop Depth = 0 noPath = 0 , lastinLoop = 0
depth 1st num 11 : bbnum = 10 1st iCode = 35 , last iCode = 36
visited 0 : hasFcall = 0

defines bitVector :bitvector Size = 153 bSize = 20
Bits on { (42) }

local defines bitVector :bitvector Size = 153 bSize = 20
Bits on { (42) }

pointers Set bitvector :
in pointers Set bitvector :
inDefs Set bitvector :bitvector Size = 153 bSize = 20
Bits on { (2) (3) (4) (5) (6) (7) (8) (9) (10) (11) (15) (18) (20) (24) (27) (29) (33) (36) (38) }

outDefs Set bitvector :bitvector Size = 153 bSize = 20
Bits on { (2) (3) (4) (5) (6) (7) (8) (9) (10) (11) (15) (18) (20) (24) (27) (29) (33) (36) (38) (42) }

usesDefs Set bitvector :bitvector Size = 153 bSize = 20
Bits on { }

----------------------------------------------------------------
main.c(179:35:42:0:0)		_sensorSample_phase3_1_1 [k21 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{bit bit {0,1}} := 0x1 {literal bit {0,1}}
main.c(179:36:43:0:0)		 goto preHeaderLbl0($33)
main.c(179:37:44:0:0)	 _iffalse_5($11) :
main.c(179:38:45:0:0)		_sensorSample_phase3_1_1 [k21 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{bit bit {0,1}} := 0x0 {literal bit {0,1}}
main.c(186:40:150:0:0)	 preHeaderLbl0($33) :
main.c(186:41:152:0:0)		iTemp12 [k25 lr41:66 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sensorSample_halfcycles_1_1}[r0x67 ] := 0x3 {literal unsigned char}
main.c(186:42:48:1:0)	 _dobody_1($13) :
main.c(187:43:49:1:0)		_PORTB [k8 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{sfr volatile unsigned char} = _PORTB [k8 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{sfr volatile unsigned char} ^ iTemp0 [k2 lr3:66 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sensorSample_transmitter_1_1}[r0x63 ]
main.c(188:45:51:1:0)	 nop main.c(188:46:52:1:0)	 nop main.c(188:47:53:1:0)	 nop main.c(189:48:54:1:0)	 nop main.c(189:49:55:1:0)	 nop main.c(189:50:56:1:0)	 nop main.c(189:51:57:1:0)	 nop main.c(189:52:58:1:0)	 nop main.c(190:53:59:1:0)	 nop main.c(190:54:60:1:0)	 nop main.c(190:55:61:1:0)	 nop main.c(190:56:62:1:0)	 nop main.c(190:57:63:1:0)	 nop main.c(191:58:64:1:0)	 nop main.c(191:59:65:1:0)	 nop main.c(191:60:66:1:0)	 nop main.c(191:61:67:1:0)	 nop main.c(191:62:68:1:0)	 nop main.c(192:63:69:1:0)	 nop main.c(192:64:70:1:0)	 nop main.c(193:65:72:1:0)		iTemp12 [k25 lr41:66 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sensorSample_halfcycles_1_1}[r0x67 ] = iTemp12 [k25 lr41:66 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sensorSample_halfcycles_1_1}[r0x67 ] - 0x1 {literal unsigned char}
main.c(193:66:74:1:0)		if iTemp12 [k25 lr41:66 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sensorSample_halfcycles_1_1}[r0x67 ] != 0 goto _dobody_1($13)
main.c(196:67:77:0:0)		if _sensorSample_phase0_1_1 [k15 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{bit bit {0,1}} == 0 goto _iffalse_6($17)
main.c(197:68:81:0:0)		_GO_DONE [k26 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{bit bit {0,1}} := 0x1 {literal bit {0,1}}
main.c(197:69:82:0:0)	 _iffalse_6($17) :
main.c(198:70:83:0:0)	 nop main.c(198:71:84:0:0)	 nop main.c(198:72:85:0:0)	 nop main.c(198:73:86:0:0)	 nop main.c(198:74:87:0:0)	 nop main.c(199:75:88:0:0)	 nop main.c(199:76:89:0:0)	 nop main.c(199:77:90:0:0)	 nop main.c(199:78:91:0:0)	 nop main.c(199:79:92:0:0)	 nop main.c(200:80:93:0:0)		if _sensorSample_phase1_1_1 [k17 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{bit bit {0,1}} == 0 goto _iffalse_7($19)
main.c(201:81:97:0:0)		_GO_DONE [k26 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{bit bit {0,1}} := 0x1 {literal bit {0,1}}
main.c(201:82:98:0:0)	 _iffalse_7($19) :
main.c(202:83:99:0:0)	 nop main.c(202:84:100:0:0)	 nop main.c(202:85:101:0:0)	 nop main.c(202:86:102:0:0)	 nop main.c(202:87:103:0:0)	 nop main.c(203:88:104:0:0)	 nop main.c(203:89:105:0:0)	 nop main.c(203:90:106:0:0)	 nop main.c(203:91:107:0:0)	 nop main.c(203:92:108:0:0)	 nop main.c(203:93:109:0:0)	 nop main.c(204:94:110:0:0)		if _sensorSample_phase2_1_1 [k19 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{bit bit {0,1}} == 0 goto _iffalse_8($21)
main.c(205:95:114:0:0)		_GO_DONE [k26 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{bit bit {0,1}} := 0x1 {literal bit {0,1}}
main.c(205:96:115:0:0)	 _iffalse_8($21) :
main.c(206:97:116:0:0)	 nop main.c(206:98:117:0:0)	 nop main.c(206:99:118:0:0)	 nop main.c(206:100:119:0:0)	 nop main.c(206:101:120:0:0)	 nop main.c(207:102:121:0:0)	 nop main.c(207:103:122:0:0)	 nop main.c(207:104:123:0:0)	 nop main.c(207:105:124:0:0)	 nop main.c(207:106:125:0:0)	 nop main.c(208:107:126:0:0)		if _sensorSample_phase3_1_1 [k21 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{bit bit {0,1}} == 0 goto _whilecontinue_1($24)
main.c(209:108:130:0:0)		_GO_DONE [k26 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{bit bit {0,1}} := 0x1 {literal bit {0,1}}
main.c(212:110:132:1:0)	 _whilecontinue_1($24) :
main.c(212:111:133:1:0)		if _NOT_DONE [k31 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{bit bit {0,1}} != 0 goto _whilecontinue_1($24)
main.c(215:112:138:0:0)		_TRISB [k10 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{sfr volatile unsigned char} := 0xf0 {literal unsigned char}
main.c(217:113:139:0:0)		iTemp17 [k32 lr113:115 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sensorSample_h_1_1}[r0x68 ] := _ADRESH [k34 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{sfr volatile unsigned char}
main.c(218:114:140:0:0)		iTemp18 [k35 lr114:117 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sensorSample_l_1_1}[r0x69 ] := _ADRESL [k37 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{sfr volatile unsigned char}
main.c(219:115:141:0:0)		iTemp19 [k38 lr115:116 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int}[r0x68 r0x6A ] = (int)iTemp17 [k32 lr113:115 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sensorSample_h_1_1}[r0x68 ]
main.c(219:116:142:0:0)		iTemp20 [k39 lr116:118 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int}[r0x68 r0x6A ] = iTemp19 [k38 lr115:116 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int}[r0x68 r0x6A ] << 0x8 {literal unsigned char}
main.c(219:117:144:0:0)		iTemp22 [k41 lr117:118 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{unsigned int}[r0x69 r0x6B ] = (unsigned int)iTemp18 [k35 lr114:117 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sensorSample_l_1_1}[r0x69 ]
main.c(219:118:145:0:0)		iTemp23 [k42 lr118:119 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{unsigned int}[r0x68 r0x6A ] = iTemp20 [k39 lr116:118 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int}[r0x68 r0x6A ] | iTemp22 [k41 lr117:118 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{unsigned int}[r0x69 r0x6B ]
main.c(219:119:147:0:0)		ret iTemp23 [k42 lr118:119 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{unsigned int}[r0x68 r0x6A ]
main.c(219:120:148:0:0)	 _return($27) :
main.c(219:121:149:0:0)		eproc _sensorSample [k1 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int function    }

----------------------------------------------------------------
Basic Block _iffalse_5 : loop Depth = 0 noPath = 0 , lastinLoop = 0
depth 1st num 12 : bbnum = 11 1st iCode = 37 , last iCode = 38
visited 0 : hasFcall = 0

defines bitVector :bitvector Size = 153 bSize = 20
Bits on { (45) }

local defines bitVector :bitvector Size = 153 bSize = 20
Bits on { (45) }

pointers Set bitvector :
in pointers Set bitvector :
inDefs Set bitvector :bitvector Size = 153 bSize = 20
Bits on { (2) (3) (4) (5) (6) (7) (8) (9) (10) (11) (15) (18) (20) (24) (27) (29) (33) (36) (38) }

outDefs Set bitvector :bitvector Size = 153 bSize = 20
Bits on { (2) (3) (4) (5) (6) (7) (8) (9) (10) (11) (15) (18) (20) (24) (27) (29) (33) (36) (38) (45) }

usesDefs Set bitvector :bitvector Size = 153 bSize = 20
Bits on { }

----------------------------------------------------------------
main.c(179:37:44:0:0)	 _iffalse_5($11) :
main.c(179:38:45:0:0)		_sensorSample_phase3_1_1 [k21 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{bit bit {0,1}} := 0x0 {literal bit {0,1}}
main.c(186:40:150:0:0)	 preHeaderLbl0($33) :
main.c(186:41:152:0:0)		iTemp12 [k25 lr41:66 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sensorSample_halfcycles_1_1}[r0x67 ] := 0x3 {literal unsigned char}
main.c(186:42:48:1:0)	 _dobody_1($13) :
main.c(187:43:49:1:0)		_PORTB [k8 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{sfr volatile unsigned char} = _PORTB [k8 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{sfr volatile unsigned char} ^ iTemp0 [k2 lr3:66 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sensorSample_transmitter_1_1}[r0x63 ]
main.c(188:45:51:1:0)	 nop main.c(188:46:52:1:0)	 nop main.c(188:47:53:1:0)	 nop main.c(189:48:54:1:0)	 nop main.c(189:49:55:1:0)	 nop main.c(189:50:56:1:0)	 nop main.c(189:51:57:1:0)	 nop main.c(189:52:58:1:0)	 nop main.c(190:53:59:1:0)	 nop main.c(190:54:60:1:0)	 nop main.c(190:55:61:1:0)	 nop main.c(190:56:62:1:0)	 nop main.c(190:57:63:1:0)	 nop main.c(191:58:64:1:0)	 nop main.c(191:59:65:1:0)	 nop main.c(191:60:66:1:0)	 nop main.c(191:61:67:1:0)	 nop main.c(191:62:68:1:0)	 nop main.c(192:63:69:1:0)	 nop main.c(192:64:70:1:0)	 nop main.c(193:65:72:1:0)		iTemp12 [k25 lr41:66 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sensorSample_halfcycles_1_1}[r0x67 ] = iTemp12 [k25 lr41:66 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sensorSample_halfcycles_1_1}[r0x67 ] - 0x1 {literal unsigned char}
main.c(193:66:74:1:0)		if iTemp12 [k25 lr41:66 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sensorSample_halfcycles_1_1}[r0x67 ] != 0 goto _dobody_1($13)
main.c(196:67:77:0:0)		if _sensorSample_phase0_1_1 [k15 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{bit bit {0,1}} == 0 goto _iffalse_6($17)
main.c(197:68:81:0:0)		_GO_DONE [k26 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{bit bit {0,1}} := 0x1 {literal bit {0,1}}
main.c(197:69:82:0:0)	 _iffalse_6($17) :
main.c(198:70:83:0:0)	 nop main.c(198:71:84:0:0)	 nop main.c(198:72:85:0:0)	 nop main.c(198:73:86:0:0)	 nop main.c(198:74:87:0:0)	 nop main.c(199:75:88:0:0)	 nop main.c(199:76:89:0:0)	 nop main.c(199:77:90:0:0)	 nop main.c(199:78:91:0:0)	 nop main.c(199:79:92:0:0)	 nop main.c(200:80:93:0:0)		if _sensorSample_phase1_1_1 [k17 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{bit bit {0,1}} == 0 goto _iffalse_7($19)
main.c(201:81:97:0:0)		_GO_DONE [k26 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{bit bit {0,1}} := 0x1 {literal bit {0,1}}
main.c(201:82:98:0:0)	 _iffalse_7($19) :
main.c(202:83:99:0:0)	 nop main.c(202:84:100:0:0)	 nop main.c(202:85:101:0:0)	 nop main.c(202:86:102:0:0)	 nop main.c(202:87:103:0:0)	 nop main.c(203:88:104:0:0)	 nop main.c(203:89:105:0:0)	 nop main.c(203:90:106:0:0)	 nop main.c(203:91:107:0:0)	 nop main.c(203:92:108:0:0)	 nop main.c(203:93:109:0:0)	 nop main.c(204:94:110:0:0)		if _sensorSample_phase2_1_1 [k19 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{bit bit {0,1}} == 0 goto _iffalse_8($21)
main.c(205:95:114:0:0)		_GO_DONE [k26 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{bit bit {0,1}} := 0x1 {literal bit {0,1}}
main.c(205:96:115:0:0)	 _iffalse_8($21) :
main.c(206:97:116:0:0)	 nop main.c(206:98:117:0:0)	 nop main.c(206:99:118:0:0)	 nop main.c(206:100:119:0:0)	 nop main.c(206:101:120:0:0)	 nop main.c(207:102:121:0:0)	 nop main.c(207:103:122:0:0)	 nop main.c(207:104:123:0:0)	 nop main.c(207:105:124:0:0)	 nop main.c(207:106:125:0:0)	 nop main.c(208:107:126:0:0)		if _sensorSample_phase3_1_1 [k21 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{bit bit {0,1}} == 0 goto _whilecontinue_1($24)
main.c(209:108:130:0:0)		_GO_DONE [k26 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{bit bit {0,1}} := 0x1 {literal bit {0,1}}
main.c(212:110:132:1:0)	 _whilecontinue_1($24) :
main.c(212:111:133:1:0)		if _NOT_DONE [k31 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{bit bit {0,1}} != 0 goto _whilecontinue_1($24)
main.c(215:112:138:0:0)		_TRISB [k10 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{sfr volatile unsigned char} := 0xf0 {literal unsigned char}
main.c(217:113:139:0:0)		iTemp17 [k32 lr113:115 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sensorSample_h_1_1}[r0x68 ] := _ADRESH [k34 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{sfr volatile unsigned char}
main.c(218:114:140:0:0)		iTemp18 [k35 lr114:117 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sensorSample_l_1_1}[r0x69 ] := _ADRESL [k37 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{sfr volatile unsigned char}
main.c(219:115:141:0:0)		iTemp19 [k38 lr115:116 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int}[r0x68 r0x6A ] = (int)iTemp17 [k32 lr113:115 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sensorSample_h_1_1}[r0x68 ]
main.c(219:116:142:0:0)		iTemp20 [k39 lr116:118 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int}[r0x68 r0x6A ] = iTemp19 [k38 lr115:116 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int}[r0x68 r0x6A ] << 0x8 {literal unsigned char}
main.c(219:117:144:0:0)		iTemp22 [k41 lr117:118 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{unsigned int}[r0x69 r0x6B ] = (unsigned int)iTemp18 [k35 lr114:117 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sensorSample_l_1_1}[r0x69 ]
main.c(219:118:145:0:0)		iTemp23 [k42 lr118:119 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{unsigned int}[r0x68 r0x6A ] = iTemp20 [k39 lr116:118 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int}[r0x68 r0x6A ] | iTemp22 [k41 lr117:118 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{unsigned int}[r0x69 r0x6B ]
main.c(219:119:147:0:0)		ret iTemp23 [k42 lr118:119 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{unsigned int}[r0x68 r0x6A ]
main.c(219:120:148:0:0)	 _return($27) :
main.c(219:121:149:0:0)		eproc _sensorSample [k1 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int function    }

----------------------------------------------------------------
Basic Block _ifend_5 : loop Depth = 0 noPath = 0 , lastinLoop = 0
depth 1st num 13 : bbnum = 12 1st iCode = 39 , last iCode = 39
visited 0 : hasFcall = 0

defines bitVector :bitvector Size = 153 bSize = 20
Bits on { }

local defines bitVector :bitvector Size = 153 bSize = 20
Bits on { (47) }

pointers Set bitvector :
in pointers Set bitvector :
inDefs Set bitvector :bitvector Size = 153 bSize = 20
Bits on { (2) (3) (4) (5) (6) (7) (8) (9) (10) (11) (15) (18) (20) (24) (27) (29) (33) (36) (38) (42) (45) }

outDefs Set bitvector :bitvector Size = 153 bSize = 20
Bits on { (2) (3) (4) (5) (6) (7) (8) (9) (10) (11) (15) (18) (20) (24) (27) (29) (33) (36) (38) (42) (45) }

usesDefs Set bitvector :bitvector Size = 153 bSize = 20
Bits on { }

----------------------------------------------------------------
main.c(179:39:46:0:0)	 _ifend_5($12) :
main.c(186:40:150:0:0)	 preHeaderLbl0($33) :
main.c(186:41:152:0:0)		iTemp12 [k25 lr41:66 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sensorSample_halfcycles_1_1}[r0x67 ] := 0x3 {literal unsigned char}
main.c(186:42:48:1:0)	 _dobody_1($13) :
main.c(187:43:49:1:0)		_PORTB [k8 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{sfr volatile unsigned char} = _PORTB [k8 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{sfr volatile unsigned char} ^ iTemp0 [k2 lr3:66 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sensorSample_transmitter_1_1}[r0x63 ]
main.c(188:45:51:1:0)	 nop main.c(188:46:52:1:0)	 nop main.c(188:47:53:1:0)	 nop main.c(189:48:54:1:0)	 nop main.c(189:49:55:1:0)	 nop main.c(189:50:56:1:0)	 nop main.c(189:51:57:1:0)	 nop main.c(189:52:58:1:0)	 nop main.c(190:53:59:1:0)	 nop main.c(190:54:60:1:0)	 nop main.c(190:55:61:1:0)	 nop main.c(190:56:62:1:0)	 nop main.c(190:57:63:1:0)	 nop main.c(191:58:64:1:0)	 nop main.c(191:59:65:1:0)	 nop main.c(191:60:66:1:0)	 nop main.c(191:61:67:1:0)	 nop main.c(191:62:68:1:0)	 nop main.c(192:63:69:1:0)	 nop main.c(192:64:70:1:0)	 nop main.c(193:65:72:1:0)		iTemp12 [k25 lr41:66 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sensorSample_halfcycles_1_1}[r0x67 ] = iTemp12 [k25 lr41:66 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sensorSample_halfcycles_1_1}[r0x67 ] - 0x1 {literal unsigned char}
main.c(193:66:74:1:0)		if iTemp12 [k25 lr41:66 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sensorSample_halfcycles_1_1}[r0x67 ] != 0 goto _dobody_1($13)
main.c(196:67:77:0:0)		if _sensorSample_phase0_1_1 [k15 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{bit bit {0,1}} == 0 goto _iffalse_6($17)
main.c(197:68:81:0:0)		_GO_DONE [k26 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{bit bit {0,1}} := 0x1 {literal bit {0,1}}
main.c(197:69:82:0:0)	 _iffalse_6($17) :
main.c(198:70:83:0:0)	 nop main.c(198:71:84:0:0)	 nop main.c(198:72:85:0:0)	 nop main.c(198:73:86:0:0)	 nop main.c(198:74:87:0:0)	 nop main.c(199:75:88:0:0)	 nop main.c(199:76:89:0:0)	 nop main.c(199:77:90:0:0)	 nop main.c(199:78:91:0:0)	 nop main.c(199:79:92:0:0)	 nop main.c(200:80:93:0:0)		if _sensorSample_phase1_1_1 [k17 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{bit bit {0,1}} == 0 goto _iffalse_7($19)
main.c(201:81:97:0:0)		_GO_DONE [k26 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{bit bit {0,1}} := 0x1 {literal bit {0,1}}
main.c(201:82:98:0:0)	 _iffalse_7($19) :
main.c(202:83:99:0:0)	 nop main.c(202:84:100:0:0)	 nop main.c(202:85:101:0:0)	 nop main.c(202:86:102:0:0)	 nop main.c(202:87:103:0:0)	 nop main.c(203:88:104:0:0)	 nop main.c(203:89:105:0:0)	 nop main.c(203:90:106:0:0)	 nop main.c(203:91:107:0:0)	 nop main.c(203:92:108:0:0)	 nop main.c(203:93:109:0:0)	 nop main.c(204:94:110:0:0)		if _sensorSample_phase2_1_1 [k19 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{bit bit {0,1}} == 0 goto _iffalse_8($21)
main.c(205:95:114:0:0)		_GO_DONE [k26 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{bit bit {0,1}} := 0x1 {literal bit {0,1}}
main.c(205:96:115:0:0)	 _iffalse_8($21) :
main.c(206:97:116:0:0)	 nop main.c(206:98:117:0:0)	 nop main.c(206:99:118:0:0)	 nop main.c(206:100:119:0:0)	 nop main.c(206:101:120:0:0)	 nop main.c(207:102:121:0:0)	 nop main.c(207:103:122:0:0)	 nop main.c(207:104:123:0:0)	 nop main.c(207:105:124:0:0)	 nop main.c(207:106:125:0:0)	 nop main.c(208:107:126:0:0)		if _sensorSample_phase3_1_1 [k21 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{bit bit {0,1}} == 0 goto _whilecontinue_1($24)
main.c(209:108:130:0:0)		_GO_DONE [k26 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{bit bit {0,1}} := 0x1 {literal bit {0,1}}
main.c(212:110:132:1:0)	 _whilecontinue_1($24) :
main.c(212:111:133:1:0)		if _NOT_DONE [k31 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{bit bit {0,1}} != 0 goto _whilecontinue_1($24)
main.c(215:112:138:0:0)		_TRISB [k10 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{sfr volatile unsigned char} := 0xf0 {literal unsigned char}
main.c(217:113:139:0:0)		iTemp17 [k32 lr113:115 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sensorSample_h_1_1}[r0x68 ] := _ADRESH [k34 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{sfr volatile unsigned char}
main.c(218:114:140:0:0)		iTemp18 [k35 lr114:117 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sensorSample_l_1_1}[r0x69 ] := _ADRESL [k37 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{sfr volatile unsigned char}
main.c(219:115:141:0:0)		iTemp19 [k38 lr115:116 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int}[r0x68 r0x6A ] = (int)iTemp17 [k32 lr113:115 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sensorSample_h_1_1}[r0x68 ]
main.c(219:116:142:0:0)		iTemp20 [k39 lr116:118 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int}[r0x68 r0x6A ] = iTemp19 [k38 lr115:116 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int}[r0x68 r0x6A ] << 0x8 {literal unsigned char}
main.c(219:117:144:0:0)		iTemp22 [k41 lr117:118 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{unsigned int}[r0x69 r0x6B ] = (unsigned int)iTemp18 [k35 lr114:117 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sensorSample_l_1_1}[r0x69 ]
main.c(219:118:145:0:0)		iTemp23 [k42 lr118:119 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{unsigned int}[r0x68 r0x6A ] = iTemp20 [k39 lr116:118 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int}[r0x68 r0x6A ] | iTemp22 [k41 lr117:118 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{unsigned int}[r0x69 r0x6B ]
main.c(219:119:147:0:0)		ret iTemp23 [k42 lr118:119 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{unsigned int}[r0x68 r0x6A ]
main.c(219:120:148:0:0)	 _return($27) :
main.c(219:121:149:0:0)		eproc _sensorSample [k1 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int function    }

----------------------------------------------------------------
Basic Block preHeaderLbl0 : loop Depth = 0 noPath = 0 , lastinLoop = 0
depth 1st num 14 : bbnum = 13 1st iCode = 40 , last iCode = 41
visited 0 : hasFcall = 0

defines bitVector :bitvector Size = 153 bSize = 20
Bits on { (152) }

local defines bitVector :bitvector Size = 153 bSize = 20
Bits on { (152) }

pointers Set bitvector :
in pointers Set bitvector :
inDefs Set bitvector :bitvector Size = 153 bSize = 20
Bits on { (2) (3) (4) (5) (6) (7) (8) (9) (10) (11) (15) (18) (20) (24) (27) (29) (33) (36) (38) (42) (45) (47) }

outDefs Set bitvector :bitvector Size = 153 bSize = 20
Bits on { (2) (3) (4) (5) (6) (7) (8) (9) (10) (11) (15) (18) (20) (24) (27) (29) (33) (36) (38) (42) (45) (47) (152) }

usesDefs Set bitvector :bitvector Size = 153 bSize = 20
Bits on { }

----------------------------------------------------------------
main.c(186:40:150:0:0)	 preHeaderLbl0($33) :
main.c(186:41:152:0:0)		iTemp12 [k25 lr41:66 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sensorSample_halfcycles_1_1}[r0x67 ] := 0x3 {literal unsigned char}
main.c(186:42:48:1:0)	 _dobody_1($13) :
main.c(187:43:49:1:0)		_PORTB [k8 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{sfr volatile unsigned char} = _PORTB [k8 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{sfr volatile unsigned char} ^ iTemp0 [k2 lr3:66 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sensorSample_transmitter_1_1}[r0x63 ]
main.c(188:45:51:1:0)	 nop main.c(188:46:52:1:0)	 nop main.c(188:47:53:1:0)	 nop main.c(189:48:54:1:0)	 nop main.c(189:49:55:1:0)	 nop main.c(189:50:56:1:0)	 nop main.c(189:51:57:1:0)	 nop main.c(189:52:58:1:0)	 nop main.c(190:53:59:1:0)	 nop main.c(190:54:60:1:0)	 nop main.c(190:55:61:1:0)	 nop main.c(190:56:62:1:0)	 nop main.c(190:57:63:1:0)	 nop main.c(191:58:64:1:0)	 nop main.c(191:59:65:1:0)	 nop main.c(191:60:66:1:0)	 nop main.c(191:61:67:1:0)	 nop main.c(191:62:68:1:0)	 nop main.c(192:63:69:1:0)	 nop main.c(192:64:70:1:0)	 nop main.c(193:65:72:1:0)		iTemp12 [k25 lr41:66 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sensorSample_halfcycles_1_1}[r0x67 ] = iTemp12 [k25 lr41:66 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sensorSample_halfcycles_1_1}[r0x67 ] - 0x1 {literal unsigned char}
main.c(193:66:74:1:0)		if iTemp12 [k25 lr41:66 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sensorSample_halfcycles_1_1}[r0x67 ] != 0 goto _dobody_1($13)
main.c(196:67:77:0:0)		if _sensorSample_phase0_1_1 [k15 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{bit bit {0,1}} == 0 goto _iffalse_6($17)
main.c(197:68:81:0:0)		_GO_DONE [k26 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{bit bit {0,1}} := 0x1 {literal bit {0,1}}
main.c(197:69:82:0:0)	 _iffalse_6($17) :
main.c(198:70:83:0:0)	 nop main.c(198:71:84:0:0)	 nop main.c(198:72:85:0:0)	 nop main.c(198:73:86:0:0)	 nop main.c(198:74:87:0:0)	 nop main.c(199:75:88:0:0)	 nop main.c(199:76:89:0:0)	 nop main.c(199:77:90:0:0)	 nop main.c(199:78:91:0:0)	 nop main.c(199:79:92:0:0)	 nop main.c(200:80:93:0:0)		if _sensorSample_phase1_1_1 [k17 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{bit bit {0,1}} == 0 goto _iffalse_7($19)
main.c(201:81:97:0:0)		_GO_DONE [k26 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{bit bit {0,1}} := 0x1 {literal bit {0,1}}
main.c(201:82:98:0:0)	 _iffalse_7($19) :
main.c(202:83:99:0:0)	 nop main.c(202:84:100:0:0)	 nop main.c(202:85:101:0:0)	 nop main.c(202:86:102:0:0)	 nop main.c(202:87:103:0:0)	 nop main.c(203:88:104:0:0)	 nop main.c(203:89:105:0:0)	 nop main.c(203:90:106:0:0)	 nop main.c(203:91:107:0:0)	 nop main.c(203:92:108:0:0)	 nop main.c(203:93:109:0:0)	 nop main.c(204:94:110:0:0)		if _sensorSample_phase2_1_1 [k19 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{bit bit {0,1}} == 0 goto _iffalse_8($21)
main.c(205:95:114:0:0)		_GO_DONE [k26 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{bit bit {0,1}} := 0x1 {literal bit {0,1}}
main.c(205:96:115:0:0)	 _iffalse_8($21) :
main.c(206:97:116:0:0)	 nop main.c(206:98:117:0:0)	 nop main.c(206:99:118:0:0)	 nop main.c(206:100:119:0:0)	 nop main.c(206:101:120:0:0)	 nop main.c(207:102:121:0:0)	 nop main.c(207:103:122:0:0)	 nop main.c(207:104:123:0:0)	 nop main.c(207:105:124:0:0)	 nop main.c(207:106:125:0:0)	 nop main.c(208:107:126:0:0)		if _sensorSample_phase3_1_1 [k21 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{bit bit {0,1}} == 0 goto _whilecontinue_1($24)
main.c(209:108:130:0:0)		_GO_DONE [k26 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{bit bit {0,1}} := 0x1 {literal bit {0,1}}
main.c(212:110:132:1:0)	 _whilecontinue_1($24) :
main.c(212:111:133:1:0)		if _NOT_DONE [k31 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{bit bit {0,1}} != 0 goto _whilecontinue_1($24)
main.c(215:112:138:0:0)		_TRISB [k10 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{sfr volatile unsigned char} := 0xf0 {literal unsigned char}
main.c(217:113:139:0:0)		iTemp17 [k32 lr113:115 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sensorSample_h_1_1}[r0x68 ] := _ADRESH [k34 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{sfr volatile unsigned char}
main.c(218:114:140:0:0)		iTemp18 [k35 lr114:117 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sensorSample_l_1_1}[r0x69 ] := _ADRESL [k37 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{sfr volatile unsigned char}
main.c(219:115:141:0:0)		iTemp19 [k38 lr115:116 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int}[r0x68 r0x6A ] = (int)iTemp17 [k32 lr113:115 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sensorSample_h_1_1}[r0x68 ]
main.c(219:116:142:0:0)		iTemp20 [k39 lr116:118 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int}[r0x68 r0x6A ] = iTemp19 [k38 lr115:116 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int}[r0x68 r0x6A ] << 0x8 {literal unsigned char}
main.c(219:117:144:0:0)		iTemp22 [k41 lr117:118 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{unsigned int}[r0x69 r0x6B ] = (unsigned int)iTemp18 [k35 lr114:117 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sensorSample_l_1_1}[r0x69 ]
main.c(219:118:145:0:0)		iTemp23 [k42 lr118:119 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{unsigned int}[r0x68 r0x6A ] = iTemp20 [k39 lr116:118 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int}[r0x68 r0x6A ] | iTemp22 [k41 lr117:118 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{unsigned int}[r0x69 r0x6B ]
main.c(219:119:147:0:0)		ret iTemp23 [k42 lr118:119 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{unsigned int}[r0x68 r0x6A ]
main.c(219:120:148:0:0)	 _return($27) :
main.c(219:121:149:0:0)		eproc _sensorSample [k1 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int function    }

----------------------------------------------------------------
Basic Block _dobody_1 : loop Depth = 1 noPath = 0 , lastinLoop = 1
depth 1st num 15 : bbnum = 14 1st iCode = 42 , last iCode = 66
visited 0 : hasFcall = 0

defines bitVector :bitvector Size = 153 bSize = 20
Bits on { (49) (50) (72) }

local defines bitVector :bitvector Size = 153 bSize = 20
Bits on { (49) (50) (72) (73) }

pointers Set bitvector :
in pointers Set bitvector :
inDefs Set bitvector :bitvector Size = 153 bSize = 20
Bits on { (2) (3) (4) (5) (6) (7) (8) (9) (10) (11) (15) (18) (20) (24) (27) (29) (33) (36) (38) (42) (45) (47) (49) (50) (72) (73) (152) }

outDefs Set bitvector :bitvector Size = 153 bSize = 20
Bits on { (2) (3) (4) (5) (7) (8) (9) (10) (11) (15) (18) (20) (24) (27) (29) (33) (36) (38) (42) (45) (49) (50) (72) }

usesDefs Set bitvector :bitvector Size = 153 bSize = 20
Bits on { (2) (6) (49) (50) (72) (152) }

----------------------------------------------------------------
main.c(186:42:48:1:0)	 _dobody_1($13) :
main.c(187:43:49:1:0)		_PORTB [k8 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{sfr volatile unsigned char} = _PORTB [k8 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{sfr volatile unsigned char} ^ iTemp0 [k2 lr3:66 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sensorSample_transmitter_1_1}[r0x63 ]
main.c(188:45:51:1:0)	 nop main.c(188:46:52:1:0)	 nop main.c(188:47:53:1:0)	 nop main.c(189:48:54:1:0)	 nop main.c(189:49:55:1:0)	 nop main.c(189:50:56:1:0)	 nop main.c(189:51:57:1:0)	 nop main.c(189:52:58:1:0)	 nop main.c(190:53:59:1:0)	 nop main.c(190:54:60:1:0)	 nop main.c(190:55:61:1:0)	 nop main.c(190:56:62:1:0)	 nop main.c(190:57:63:1:0)	 nop main.c(191:58:64:1:0)	 nop main.c(191:59:65:1:0)	 nop main.c(191:60:66:1:0)	 nop main.c(191:61:67:1:0)	 nop main.c(191:62:68:1:0)	 nop main.c(192:63:69:1:0)	 nop main.c(192:64:70:1:0)	 nop main.c(193:65:72:1:0)		iTemp12 [k25 lr41:66 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sensorSample_halfcycles_1_1}[r0x67 ] = iTemp12 [k25 lr41:66 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sensorSample_halfcycles_1_1}[r0x67 ] - 0x1 {literal unsigned char}
main.c(193:66:74:1:0)		if iTemp12 [k25 lr41:66 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sensorSample_halfcycles_1_1}[r0x67 ] != 0 goto _dobody_1($13)
main.c(196:67:77:0:0)		if _sensorSample_phase0_1_1 [k15 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{bit bit {0,1}} == 0 goto _iffalse_6($17)
main.c(197:68:81:0:0)		_GO_DONE [k26 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{bit bit {0,1}} := 0x1 {literal bit {0,1}}
main.c(197:69:82:0:0)	 _iffalse_6($17) :
main.c(198:70:83:0:0)	 nop main.c(198:71:84:0:0)	 nop main.c(198:72:85:0:0)	 nop main.c(198:73:86:0:0)	 nop main.c(198:74:87:0:0)	 nop main.c(199:75:88:0:0)	 nop main.c(199:76:89:0:0)	 nop main.c(199:77:90:0:0)	 nop main.c(199:78:91:0:0)	 nop main.c(199:79:92:0:0)	 nop main.c(200:80:93:0:0)		if _sensorSample_phase1_1_1 [k17 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{bit bit {0,1}} == 0 goto _iffalse_7($19)
main.c(201:81:97:0:0)		_GO_DONE [k26 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{bit bit {0,1}} := 0x1 {literal bit {0,1}}
main.c(201:82:98:0:0)	 _iffalse_7($19) :
main.c(202:83:99:0:0)	 nop main.c(202:84:100:0:0)	 nop main.c(202:85:101:0:0)	 nop main.c(202:86:102:0:0)	 nop main.c(202:87:103:0:0)	 nop main.c(203:88:104:0:0)	 nop main.c(203:89:105:0:0)	 nop main.c(203:90:106:0:0)	 nop main.c(203:91:107:0:0)	 nop main.c(203:92:108:0:0)	 nop main.c(203:93:109:0:0)	 nop main.c(204:94:110:0:0)		if _sensorSample_phase2_1_1 [k19 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{bit bit {0,1}} == 0 goto _iffalse_8($21)
main.c(205:95:114:0:0)		_GO_DONE [k26 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{bit bit {0,1}} := 0x1 {literal bit {0,1}}
main.c(205:96:115:0:0)	 _iffalse_8($21) :
main.c(206:97:116:0:0)	 nop main.c(206:98:117:0:0)	 nop main.c(206:99:118:0:0)	 nop main.c(206:100:119:0:0)	 nop main.c(206:101:120:0:0)	 nop main.c(207:102:121:0:0)	 nop main.c(207:103:122:0:0)	 nop main.c(207:104:123:0:0)	 nop main.c(207:105:124:0:0)	 nop main.c(207:106:125:0:0)	 nop main.c(208:107:126:0:0)		if _sensorSample_phase3_1_1 [k21 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{bit bit {0,1}} == 0 goto _whilecontinue_1($24)
main.c(209:108:130:0:0)		_GO_DONE [k26 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{bit bit {0,1}} := 0x1 {literal bit {0,1}}
main.c(212:110:132:1:0)	 _whilecontinue_1($24) :
main.c(212:111:133:1:0)		if _NOT_DONE [k31 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{bit bit {0,1}} != 0 goto _whilecontinue_1($24)
main.c(215:112:138:0:0)		_TRISB [k10 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{sfr volatile unsigned char} := 0xf0 {literal unsigned char}
main.c(217:113:139:0:0)		iTemp17 [k32 lr113:115 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sensorSample_h_1_1}[r0x68 ] := _ADRESH [k34 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{sfr volatile unsigned char}
main.c(218:114:140:0:0)		iTemp18 [k35 lr114:117 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sensorSample_l_1_1}[r0x69 ] := _ADRESL [k37 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{sfr volatile unsigned char}
main.c(219:115:141:0:0)		iTemp19 [k38 lr115:116 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int}[r0x68 r0x6A ] = (int)iTemp17 [k32 lr113:115 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sensorSample_h_1_1}[r0x68 ]
main.c(219:116:142:0:0)		iTemp20 [k39 lr116:118 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int}[r0x68 r0x6A ] = iTemp19 [k38 lr115:116 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int}[r0x68 r0x6A ] << 0x8 {literal unsigned char}
main.c(219:117:144:0:0)		iTemp22 [k41 lr117:118 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{unsigned int}[r0x69 r0x6B ] = (unsigned int)iTemp18 [k35 lr114:117 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sensorSample_l_1_1}[r0x69 ]
main.c(219:118:145:0:0)		iTemp23 [k42 lr118:119 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{unsigned int}[r0x68 r0x6A ] = iTemp20 [k39 lr116:118 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int}[r0x68 r0x6A ] | iTemp22 [k41 lr117:118 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{unsigned int}[r0x69 r0x6B ]
main.c(219:119:147:0:0)		ret iTemp23 [k42 lr118:119 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{unsigned int}[r0x68 r0x6A ]
main.c(219:120:148:0:0)	 _return($27) :
main.c(219:121:149:0:0)		eproc _sensorSample [k1 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int function    }

----------------------------------------------------------------
Basic Block _eBBlock4 : loop Depth = 0 noPath = 0 , lastinLoop = 0
depth 1st num 16 : bbnum = 15 1st iCode = 67 , last iCode = 67
visited 0 : hasFcall = 0

defines bitVector :bitvector Size = 153 bSize = 20
Bits on { }

local defines bitVector :
pointers Set bitvector :
in pointers Set bitvector :
inDefs Set bitvector :bitvector Size = 153 bSize = 20
Bits on { (2) (3) (4) (5) (7) (8) (9) (10) (11) (15) (18) (20) (24) (27) (29) (33) (36) (38) (42) (45) (49) (50) (72) (73) }

outDefs Set bitvector :bitvector Size = 153 bSize = 20
Bits on { (2) (3) (4) (5) (7) (8) (9) (10) (11) (15) (18) (20) (24) (27) (29) (33) (36) (38) (42) (45) (49) (50) (72) (73) }

usesDefs Set bitvector :bitvector Size = 153 bSize = 20
Bits on { (15) (18) }

----------------------------------------------------------------
main.c(196:67:77:0:0)		if _sensorSample_phase0_1_1 [k15 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{bit bit {0,1}} == 0 goto _iffalse_6($17)
main.c(197:68:81:0:0)		_GO_DONE [k26 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{bit bit {0,1}} := 0x1 {literal bit {0,1}}
main.c(197:69:82:0:0)	 _iffalse_6($17) :
main.c(198:70:83:0:0)	 nop main.c(198:71:84:0:0)	 nop main.c(198:72:85:0:0)	 nop main.c(198:73:86:0:0)	 nop main.c(198:74:87:0:0)	 nop main.c(199:75:88:0:0)	 nop main.c(199:76:89:0:0)	 nop main.c(199:77:90:0:0)	 nop main.c(199:78:91:0:0)	 nop main.c(199:79:92:0:0)	 nop main.c(200:80:93:0:0)		if _sensorSample_phase1_1_1 [k17 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{bit bit {0,1}} == 0 goto _iffalse_7($19)
main.c(201:81:97:0:0)		_GO_DONE [k26 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{bit bit {0,1}} := 0x1 {literal bit {0,1}}
main.c(201:82:98:0:0)	 _iffalse_7($19) :
main.c(202:83:99:0:0)	 nop main.c(202:84:100:0:0)	 nop main.c(202:85:101:0:0)	 nop main.c(202:86:102:0:0)	 nop main.c(202:87:103:0:0)	 nop main.c(203:88:104:0:0)	 nop main.c(203:89:105:0:0)	 nop main.c(203:90:106:0:0)	 nop main.c(203:91:107:0:0)	 nop main.c(203:92:108:0:0)	 nop main.c(203:93:109:0:0)	 nop main.c(204:94:110:0:0)		if _sensorSample_phase2_1_1 [k19 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{bit bit {0,1}} == 0 goto _iffalse_8($21)
main.c(205:95:114:0:0)		_GO_DONE [k26 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{bit bit {0,1}} := 0x1 {literal bit {0,1}}
main.c(205:96:115:0:0)	 _iffalse_8($21) :
main.c(206:97:116:0:0)	 nop main.c(206:98:117:0:0)	 nop main.c(206:99:118:0:0)	 nop main.c(206:100:119:0:0)	 nop main.c(206:101:120:0:0)	 nop main.c(207:102:121:0:0)	 nop main.c(207:103:122:0:0)	 nop main.c(207:104:123:0:0)	 nop main.c(207:105:124:0:0)	 nop main.c(207:106:125:0:0)	 nop main.c(208:107:126:0:0)		if _sensorSample_phase3_1_1 [k21 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{bit bit {0,1}} == 0 goto _whilecontinue_1($24)
main.c(209:108:130:0:0)		_GO_DONE [k26 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{bit bit {0,1}} := 0x1 {literal bit {0,1}}
main.c(212:110:132:1:0)	 _whilecontinue_1($24) :
main.c(212:111:133:1:0)		if _NOT_DONE [k31 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{bit bit {0,1}} != 0 goto _whilecontinue_1($24)
main.c(215:112:138:0:0)		_TRISB [k10 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{sfr volatile unsigned char} := 0xf0 {literal unsigned char}
main.c(217:113:139:0:0)		iTemp17 [k32 lr113:115 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sensorSample_h_1_1}[r0x68 ] := _ADRESH [k34 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{sfr volatile unsigned char}
main.c(218:114:140:0:0)		iTemp18 [k35 lr114:117 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sensorSample_l_1_1}[r0x69 ] := _ADRESL [k37 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{sfr volatile unsigned char}
main.c(219:115:141:0:0)		iTemp19 [k38 lr115:116 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int}[r0x68 r0x6A ] = (int)iTemp17 [k32 lr113:115 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sensorSample_h_1_1}[r0x68 ]
main.c(219:116:142:0:0)		iTemp20 [k39 lr116:118 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int}[r0x68 r0x6A ] = iTemp19 [k38 lr115:116 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int}[r0x68 r0x6A ] << 0x8 {literal unsigned char}
main.c(219:117:144:0:0)		iTemp22 [k41 lr117:118 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{unsigned int}[r0x69 r0x6B ] = (unsigned int)iTemp18 [k35 lr114:117 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sensorSample_l_1_1}[r0x69 ]
main.c(219:118:145:0:0)		iTemp23 [k42 lr118:119 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{unsigned int}[r0x68 r0x6A ] = iTemp20 [k39 lr116:118 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int}[r0x68 r0x6A ] | iTemp22 [k41 lr117:118 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{unsigned int}[r0x69 r0x6B ]
main.c(219:119:147:0:0)		ret iTemp23 [k42 lr118:119 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{unsigned int}[r0x68 r0x6A ]
main.c(219:120:148:0:0)	 _return($27) :
main.c(219:121:149:0:0)		eproc _sensorSample [k1 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int function    }

----------------------------------------------------------------
Basic Block _eBBlock5 : loop Depth = 0 noPath = 0 , lastinLoop = 0
depth 1st num 17 : bbnum = 16 1st iCode = 68 , last iCode = 68
visited 0 : hasFcall = 0

defines bitVector :bitvector Size = 153 bSize = 20
Bits on { (81) }

local defines bitVector :bitvector Size = 153 bSize = 20
Bits on { (81) }

pointers Set bitvector :
in pointers Set bitvector :
inDefs Set bitvector :bitvector Size = 153 bSize = 20
Bits on { (2) (3) (4) (5) (7) (8) (9) (10) (11) (15) (18) (20) (24) (27) (29) (33) (36) (38) (42) (45) (49) (50) (72) (73) }

outDefs Set bitvector :bitvector Size = 153 bSize = 20
Bits on { (2) (3) (4) (5) (7) (8) (9) (10) (11) (15) (18) (20) (24) (27) (29) (33) (36) (38) (42) (45) (49) (50) (72) (73) (81) }

usesDefs Set bitvector :bitvector Size = 153 bSize = 20
Bits on { }

----------------------------------------------------------------
main.c(197:68:81:0:0)		_GO_DONE [k26 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{bit bit {0,1}} := 0x1 {literal bit {0,1}}
main.c(197:69:82:0:0)	 _iffalse_6($17) :
main.c(198:70:83:0:0)	 nop main.c(198:71:84:0:0)	 nop main.c(198:72:85:0:0)	 nop main.c(198:73:86:0:0)	 nop main.c(198:74:87:0:0)	 nop main.c(199:75:88:0:0)	 nop main.c(199:76:89:0:0)	 nop main.c(199:77:90:0:0)	 nop main.c(199:78:91:0:0)	 nop main.c(199:79:92:0:0)	 nop main.c(200:80:93:0:0)		if _sensorSample_phase1_1_1 [k17 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{bit bit {0,1}} == 0 goto _iffalse_7($19)
main.c(201:81:97:0:0)		_GO_DONE [k26 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{bit bit {0,1}} := 0x1 {literal bit {0,1}}
main.c(201:82:98:0:0)	 _iffalse_7($19) :
main.c(202:83:99:0:0)	 nop main.c(202:84:100:0:0)	 nop main.c(202:85:101:0:0)	 nop main.c(202:86:102:0:0)	 nop main.c(202:87:103:0:0)	 nop main.c(203:88:104:0:0)	 nop main.c(203:89:105:0:0)	 nop main.c(203:90:106:0:0)	 nop main.c(203:91:107:0:0)	 nop main.c(203:92:108:0:0)	 nop main.c(203:93:109:0:0)	 nop main.c(204:94:110:0:0)		if _sensorSample_phase2_1_1 [k19 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{bit bit {0,1}} == 0 goto _iffalse_8($21)
main.c(205:95:114:0:0)		_GO_DONE [k26 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{bit bit {0,1}} := 0x1 {literal bit {0,1}}
main.c(205:96:115:0:0)	 _iffalse_8($21) :
main.c(206:97:116:0:0)	 nop main.c(206:98:117:0:0)	 nop main.c(206:99:118:0:0)	 nop main.c(206:100:119:0:0)	 nop main.c(206:101:120:0:0)	 nop main.c(207:102:121:0:0)	 nop main.c(207:103:122:0:0)	 nop main.c(207:104:123:0:0)	 nop main.c(207:105:124:0:0)	 nop main.c(207:106:125:0:0)	 nop main.c(208:107:126:0:0)		if _sensorSample_phase3_1_1 [k21 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{bit bit {0,1}} == 0 goto _whilecontinue_1($24)
main.c(209:108:130:0:0)		_GO_DONE [k26 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{bit bit {0,1}} := 0x1 {literal bit {0,1}}
main.c(212:110:132:1:0)	 _whilecontinue_1($24) :
main.c(212:111:133:1:0)		if _NOT_DONE [k31 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{bit bit {0,1}} != 0 goto _whilecontinue_1($24)
main.c(215:112:138:0:0)		_TRISB [k10 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{sfr volatile unsigned char} := 0xf0 {literal unsigned char}
main.c(217:113:139:0:0)		iTemp17 [k32 lr113:115 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sensorSample_h_1_1}[r0x68 ] := _ADRESH [k34 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{sfr volatile unsigned char}
main.c(218:114:140:0:0)		iTemp18 [k35 lr114:117 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sensorSample_l_1_1}[r0x69 ] := _ADRESL [k37 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{sfr volatile unsigned char}
main.c(219:115:141:0:0)		iTemp19 [k38 lr115:116 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int}[r0x68 r0x6A ] = (int)iTemp17 [k32 lr113:115 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sensorSample_h_1_1}[r0x68 ]
main.c(219:116:142:0:0)		iTemp20 [k39 lr116:118 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int}[r0x68 r0x6A ] = iTemp19 [k38 lr115:116 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int}[r0x68 r0x6A ] << 0x8 {literal unsigned char}
main.c(219:117:144:0:0)		iTemp22 [k41 lr117:118 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{unsigned int}[r0x69 r0x6B ] = (unsigned int)iTemp18 [k35 lr114:117 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sensorSample_l_1_1}[r0x69 ]
main.c(219:118:145:0:0)		iTemp23 [k42 lr118:119 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{unsigned int}[r0x68 r0x6A ] = iTemp20 [k39 lr116:118 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int}[r0x68 r0x6A ] | iTemp22 [k41 lr117:118 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{unsigned int}[r0x69 r0x6B ]
main.c(219:119:147:0:0)		ret iTemp23 [k42 lr118:119 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{unsigned int}[r0x68 r0x6A ]
main.c(219:120:148:0:0)	 _return($27) :
main.c(219:121:149:0:0)		eproc _sensorSample [k1 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int function    }

----------------------------------------------------------------
Basic Block _iffalse_6 : loop Depth = 0 noPath = 0 , lastinLoop = 0
depth 1st num 18 : bbnum = 17 1st iCode = 69 , last iCode = 80
visited 0 : hasFcall = 0

defines bitVector :bitvector Size = 153 bSize = 20
Bits on { }

local defines bitVector :
pointers Set bitvector :
in pointers Set bitvector :
inDefs Set bitvector :bitvector Size = 153 bSize = 20
Bits on { (2) (3) (4) (5) (7) (8) (9) (10) (11) (15) (18) (20) (24) (27) (29) (33) (36) (38) (42) (45) (49) (50) (72) (73) (81) }

outDefs Set bitvector :bitvector Size = 153 bSize = 20
Bits on { (2) (3) (4) (5) (7) (8) (9) (10) (11) (15) (18) (20) (24) (27) (29) (33) (36) (38) (42) (45) (49) (50) (72) (73) (81) }

usesDefs Set bitvector :bitvector Size = 153 bSize = 20
Bits on { (24) (27) }

----------------------------------------------------------------
main.c(197:69:82:0:0)	 _iffalse_6($17) :
main.c(198:70:83:0:0)	 nop main.c(198:71:84:0:0)	 nop main.c(198:72:85:0:0)	 nop main.c(198:73:86:0:0)	 nop main.c(198:74:87:0:0)	 nop main.c(199:75:88:0:0)	 nop main.c(199:76:89:0:0)	 nop main.c(199:77:90:0:0)	 nop main.c(199:78:91:0:0)	 nop main.c(199:79:92:0:0)	 nop main.c(200:80:93:0:0)		if _sensorSample_phase1_1_1 [k17 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{bit bit {0,1}} == 0 goto _iffalse_7($19)
main.c(201:81:97:0:0)		_GO_DONE [k26 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{bit bit {0,1}} := 0x1 {literal bit {0,1}}
main.c(201:82:98:0:0)	 _iffalse_7($19) :
main.c(202:83:99:0:0)	 nop main.c(202:84:100:0:0)	 nop main.c(202:85:101:0:0)	 nop main.c(202:86:102:0:0)	 nop main.c(202:87:103:0:0)	 nop main.c(203:88:104:0:0)	 nop main.c(203:89:105:0:0)	 nop main.c(203:90:106:0:0)	 nop main.c(203:91:107:0:0)	 nop main.c(203:92:108:0:0)	 nop main.c(203:93:109:0:0)	 nop main.c(204:94:110:0:0)		if _sensorSample_phase2_1_1 [k19 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{bit bit {0,1}} == 0 goto _iffalse_8($21)
main.c(205:95:114:0:0)		_GO_DONE [k26 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{bit bit {0,1}} := 0x1 {literal bit {0,1}}
main.c(205:96:115:0:0)	 _iffalse_8($21) :
main.c(206:97:116:0:0)	 nop main.c(206:98:117:0:0)	 nop main.c(206:99:118:0:0)	 nop main.c(206:100:119:0:0)	 nop main.c(206:101:120:0:0)	 nop main.c(207:102:121:0:0)	 nop main.c(207:103:122:0:0)	 nop main.c(207:104:123:0:0)	 nop main.c(207:105:124:0:0)	 nop main.c(207:106:125:0:0)	 nop main.c(208:107:126:0:0)		if _sensorSample_phase3_1_1 [k21 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{bit bit {0,1}} == 0 goto _whilecontinue_1($24)
main.c(209:108:130:0:0)		_GO_DONE [k26 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{bit bit {0,1}} := 0x1 {literal bit {0,1}}
main.c(212:110:132:1:0)	 _whilecontinue_1($24) :
main.c(212:111:133:1:0)		if _NOT_DONE [k31 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{bit bit {0,1}} != 0 goto _whilecontinue_1($24)
main.c(215:112:138:0:0)		_TRISB [k10 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{sfr volatile unsigned char} := 0xf0 {literal unsigned char}
main.c(217:113:139:0:0)		iTemp17 [k32 lr113:115 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sensorSample_h_1_1}[r0x68 ] := _ADRESH [k34 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{sfr volatile unsigned char}
main.c(218:114:140:0:0)		iTemp18 [k35 lr114:117 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sensorSample_l_1_1}[r0x69 ] := _ADRESL [k37 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{sfr volatile unsigned char}
main.c(219:115:141:0:0)		iTemp19 [k38 lr115:116 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int}[r0x68 r0x6A ] = (int)iTemp17 [k32 lr113:115 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sensorSample_h_1_1}[r0x68 ]
main.c(219:116:142:0:0)		iTemp20 [k39 lr116:118 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int}[r0x68 r0x6A ] = iTemp19 [k38 lr115:116 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int}[r0x68 r0x6A ] << 0x8 {literal unsigned char}
main.c(219:117:144:0:0)		iTemp22 [k41 lr117:118 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{unsigned int}[r0x69 r0x6B ] = (unsigned int)iTemp18 [k35 lr114:117 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sensorSample_l_1_1}[r0x69 ]
main.c(219:118:145:0:0)		iTemp23 [k42 lr118:119 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{unsigned int}[r0x68 r0x6A ] = iTemp20 [k39 lr116:118 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int}[r0x68 r0x6A ] | iTemp22 [k41 lr117:118 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{unsigned int}[r0x69 r0x6B ]
main.c(219:119:147:0:0)		ret iTemp23 [k42 lr118:119 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{unsigned int}[r0x68 r0x6A ]
main.c(219:120:148:0:0)	 _return($27) :
main.c(219:121:149:0:0)		eproc _sensorSample [k1 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int function    }

----------------------------------------------------------------
Basic Block _eBBlock6 : loop Depth = 0 noPath = 0 , lastinLoop = 0
depth 1st num 19 : bbnum = 18 1st iCode = 81 , last iCode = 81
visited 0 : hasFcall = 0

defines bitVector :bitvector Size = 153 bSize = 20
Bits on { (97) }

local defines bitVector :bitvector Size = 153 bSize = 20
Bits on { (97) }

pointers Set bitvector :
in pointers Set bitvector :
inDefs Set bitvector :bitvector Size = 153 bSize = 20
Bits on { (2) (3) (4) (5) (7) (8) (9) (10) (11) (15) (18) (20) (24) (27) (29) (33) (36) (38) (42) (45) (49) (50) (72) (73) (81) }

outDefs Set bitvector :bitvector Size = 153 bSize = 20
Bits on { (2) (3) (4) (5) (7) (8) (9) (10) (11) (15) (18) (20) (24) (27) (29) (33) (36) (38) (42) (45) (49) (50) (72) (73) (97) }

usesDefs Set bitvector :bitvector Size = 153 bSize = 20
Bits on { }

----------------------------------------------------------------
main.c(201:81:97:0:0)		_GO_DONE [k26 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{bit bit {0,1}} := 0x1 {literal bit {0,1}}
main.c(201:82:98:0:0)	 _iffalse_7($19) :
main.c(202:83:99:0:0)	 nop main.c(202:84:100:0:0)	 nop main.c(202:85:101:0:0)	 nop main.c(202:86:102:0:0)	 nop main.c(202:87:103:0:0)	 nop main.c(203:88:104:0:0)	 nop main.c(203:89:105:0:0)	 nop main.c(203:90:106:0:0)	 nop main.c(203:91:107:0:0)	 nop main.c(203:92:108:0:0)	 nop main.c(203:93:109:0:0)	 nop main.c(204:94:110:0:0)		if _sensorSample_phase2_1_1 [k19 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{bit bit {0,1}} == 0 goto _iffalse_8($21)
main.c(205:95:114:0:0)		_GO_DONE [k26 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{bit bit {0,1}} := 0x1 {literal bit {0,1}}
main.c(205:96:115:0:0)	 _iffalse_8($21) :
main.c(206:97:116:0:0)	 nop main.c(206:98:117:0:0)	 nop main.c(206:99:118:0:0)	 nop main.c(206:100:119:0:0)	 nop main.c(206:101:120:0:0)	 nop main.c(207:102:121:0:0)	 nop main.c(207:103:122:0:0)	 nop main.c(207:104:123:0:0)	 nop main.c(207:105:124:0:0)	 nop main.c(207:106:125:0:0)	 nop main.c(208:107:126:0:0)		if _sensorSample_phase3_1_1 [k21 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{bit bit {0,1}} == 0 goto _whilecontinue_1($24)
main.c(209:108:130:0:0)		_GO_DONE [k26 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{bit bit {0,1}} := 0x1 {literal bit {0,1}}
main.c(212:110:132:1:0)	 _whilecontinue_1($24) :
main.c(212:111:133:1:0)		if _NOT_DONE [k31 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{bit bit {0,1}} != 0 goto _whilecontinue_1($24)
main.c(215:112:138:0:0)		_TRISB [k10 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{sfr volatile unsigned char} := 0xf0 {literal unsigned char}
main.c(217:113:139:0:0)		iTemp17 [k32 lr113:115 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sensorSample_h_1_1}[r0x68 ] := _ADRESH [k34 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{sfr volatile unsigned char}
main.c(218:114:140:0:0)		iTemp18 [k35 lr114:117 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sensorSample_l_1_1}[r0x69 ] := _ADRESL [k37 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{sfr volatile unsigned char}
main.c(219:115:141:0:0)		iTemp19 [k38 lr115:116 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int}[r0x68 r0x6A ] = (int)iTemp17 [k32 lr113:115 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sensorSample_h_1_1}[r0x68 ]
main.c(219:116:142:0:0)		iTemp20 [k39 lr116:118 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int}[r0x68 r0x6A ] = iTemp19 [k38 lr115:116 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int}[r0x68 r0x6A ] << 0x8 {literal unsigned char}
main.c(219:117:144:0:0)		iTemp22 [k41 lr117:118 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{unsigned int}[r0x69 r0x6B ] = (unsigned int)iTemp18 [k35 lr114:117 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sensorSample_l_1_1}[r0x69 ]
main.c(219:118:145:0:0)		iTemp23 [k42 lr118:119 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{unsigned int}[r0x68 r0x6A ] = iTemp20 [k39 lr116:118 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int}[r0x68 r0x6A ] | iTemp22 [k41 lr117:118 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{unsigned int}[r0x69 r0x6B ]
main.c(219:119:147:0:0)		ret iTemp23 [k42 lr118:119 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{unsigned int}[r0x68 r0x6A ]
main.c(219:120:148:0:0)	 _return($27) :
main.c(219:121:149:0:0)		eproc _sensorSample [k1 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int function    }

----------------------------------------------------------------
Basic Block _iffalse_7 : loop Depth = 0 noPath = 0 , lastinLoop = 0
depth 1st num 20 : bbnum = 19 1st iCode = 82 , last iCode = 94
visited 0 : hasFcall = 0

defines bitVector :bitvector Size = 153 bSize = 20
Bits on { }

local defines bitVector :
pointers Set bitvector :
in pointers Set bitvector :
inDefs Set bitvector :bitvector Size = 153 bSize = 20
Bits on { (2) (3) (4) (5) (7) (8) (9) (10) (11) (15) (18) (20) (24) (27) (29) (33) (36) (38) (42) (45) (49) (50) (72) (73) (81) (97) }

outDefs Set bitvector :bitvector Size = 153 bSize = 20
Bits on { (2) (3) (4) (5) (7) (8) (9) (10) (11) (15) (18) (20) (24) (27) (29) (33) (36) (38) (42) (45) (49) (50) (72) (73) (81) (97) }

usesDefs Set bitvector :bitvector Size = 153 bSize = 20
Bits on { (33) (36) }

----------------------------------------------------------------
main.c(201:82:98:0:0)	 _iffalse_7($19) :
main.c(202:83:99:0:0)	 nop main.c(202:84:100:0:0)	 nop main.c(202:85:101:0:0)	 nop main.c(202:86:102:0:0)	 nop main.c(202:87:103:0:0)	 nop main.c(203:88:104:0:0)	 nop main.c(203:89:105:0:0)	 nop main.c(203:90:106:0:0)	 nop main.c(203:91:107:0:0)	 nop main.c(203:92:108:0:0)	 nop main.c(203:93:109:0:0)	 nop main.c(204:94:110:0:0)		if _sensorSample_phase2_1_1 [k19 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{bit bit {0,1}} == 0 goto _iffalse_8($21)
main.c(205:95:114:0:0)		_GO_DONE [k26 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{bit bit {0,1}} := 0x1 {literal bit {0,1}}
main.c(205:96:115:0:0)	 _iffalse_8($21) :
main.c(206:97:116:0:0)	 nop main.c(206:98:117:0:0)	 nop main.c(206:99:118:0:0)	 nop main.c(206:100:119:0:0)	 nop main.c(206:101:120:0:0)	 nop main.c(207:102:121:0:0)	 nop main.c(207:103:122:0:0)	 nop main.c(207:104:123:0:0)	 nop main.c(207:105:124:0:0)	 nop main.c(207:106:125:0:0)	 nop main.c(208:107:126:0:0)		if _sensorSample_phase3_1_1 [k21 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{bit bit {0,1}} == 0 goto _whilecontinue_1($24)
main.c(209:108:130:0:0)		_GO_DONE [k26 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{bit bit {0,1}} := 0x1 {literal bit {0,1}}
main.c(212:110:132:1:0)	 _whilecontinue_1($24) :
main.c(212:111:133:1:0)		if _NOT_DONE [k31 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{bit bit {0,1}} != 0 goto _whilecontinue_1($24)
main.c(215:112:138:0:0)		_TRISB [k10 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{sfr volatile unsigned char} := 0xf0 {literal unsigned char}
main.c(217:113:139:0:0)		iTemp17 [k32 lr113:115 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sensorSample_h_1_1}[r0x68 ] := _ADRESH [k34 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{sfr volatile unsigned char}
main.c(218:114:140:0:0)		iTemp18 [k35 lr114:117 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sensorSample_l_1_1}[r0x69 ] := _ADRESL [k37 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{sfr volatile unsigned char}
main.c(219:115:141:0:0)		iTemp19 [k38 lr115:116 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int}[r0x68 r0x6A ] = (int)iTemp17 [k32 lr113:115 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sensorSample_h_1_1}[r0x68 ]
main.c(219:116:142:0:0)		iTemp20 [k39 lr116:118 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int}[r0x68 r0x6A ] = iTemp19 [k38 lr115:116 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int}[r0x68 r0x6A ] << 0x8 {literal unsigned char}
main.c(219:117:144:0:0)		iTemp22 [k41 lr117:118 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{unsigned int}[r0x69 r0x6B ] = (unsigned int)iTemp18 [k35 lr114:117 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sensorSample_l_1_1}[r0x69 ]
main.c(219:118:145:0:0)		iTemp23 [k42 lr118:119 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{unsigned int}[r0x68 r0x6A ] = iTemp20 [k39 lr116:118 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int}[r0x68 r0x6A ] | iTemp22 [k41 lr117:118 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{unsigned int}[r0x69 r0x6B ]
main.c(219:119:147:0:0)		ret iTemp23 [k42 lr118:119 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{unsigned int}[r0x68 r0x6A ]
main.c(219:120:148:0:0)	 _return($27) :
main.c(219:121:149:0:0)		eproc _sensorSample [k1 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int function    }

----------------------------------------------------------------
Basic Block _eBBlock7 : loop Depth = 0 noPath = 0 , lastinLoop = 0
depth 1st num 21 : bbnum = 20 1st iCode = 95 , last iCode = 95
visited 0 : hasFcall = 0

defines bitVector :bitvector Size = 153 bSize = 20
Bits on { (114) }

local defines bitVector :bitvector Size = 153 bSize = 20
Bits on { (114) }

pointers Set bitvector :
in pointers Set bitvector :
inDefs Set bitvector :bitvector Size = 153 bSize = 20
Bits on { (2) (3) (4) (5) (7) (8) (9) (10) (11) (15) (18) (20) (24) (27) (29) (33) (36) (38) (42) (45) (49) (50) (72) (73) (81) (97) }

outDefs Set bitvector :bitvector Size = 153 bSize = 20
Bits on { (2) (3) (4) (5) (7) (8) (9) (10) (11) (15) (18) (20) (24) (27) (29) (33) (36) (38) (42) (45) (49) (50) (72) (73) (114) }

usesDefs Set bitvector :bitvector Size = 153 bSize = 20
Bits on { }

----------------------------------------------------------------
main.c(205:95:114:0:0)		_GO_DONE [k26 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{bit bit {0,1}} := 0x1 {literal bit {0,1}}
main.c(205:96:115:0:0)	 _iffalse_8($21) :
main.c(206:97:116:0:0)	 nop main.c(206:98:117:0:0)	 nop main.c(206:99:118:0:0)	 nop main.c(206:100:119:0:0)	 nop main.c(206:101:120:0:0)	 nop main.c(207:102:121:0:0)	 nop main.c(207:103:122:0:0)	 nop main.c(207:104:123:0:0)	 nop main.c(207:105:124:0:0)	 nop main.c(207:106:125:0:0)	 nop main.c(208:107:126:0:0)		if _sensorSample_phase3_1_1 [k21 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{bit bit {0,1}} == 0 goto _whilecontinue_1($24)
main.c(209:108:130:0:0)		_GO_DONE [k26 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{bit bit {0,1}} := 0x1 {literal bit {0,1}}
main.c(212:110:132:1:0)	 _whilecontinue_1($24) :
main.c(212:111:133:1:0)		if _NOT_DONE [k31 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{bit bit {0,1}} != 0 goto _whilecontinue_1($24)
main.c(215:112:138:0:0)		_TRISB [k10 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{sfr volatile unsigned char} := 0xf0 {literal unsigned char}
main.c(217:113:139:0:0)		iTemp17 [k32 lr113:115 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sensorSample_h_1_1}[r0x68 ] := _ADRESH [k34 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{sfr volatile unsigned char}
main.c(218:114:140:0:0)		iTemp18 [k35 lr114:117 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sensorSample_l_1_1}[r0x69 ] := _ADRESL [k37 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{sfr volatile unsigned char}
main.c(219:115:141:0:0)		iTemp19 [k38 lr115:116 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int}[r0x68 r0x6A ] = (int)iTemp17 [k32 lr113:115 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sensorSample_h_1_1}[r0x68 ]
main.c(219:116:142:0:0)		iTemp20 [k39 lr116:118 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int}[r0x68 r0x6A ] = iTemp19 [k38 lr115:116 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int}[r0x68 r0x6A ] << 0x8 {literal unsigned char}
main.c(219:117:144:0:0)		iTemp22 [k41 lr117:118 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{unsigned int}[r0x69 r0x6B ] = (unsigned int)iTemp18 [k35 lr114:117 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sensorSample_l_1_1}[r0x69 ]
main.c(219:118:145:0:0)		iTemp23 [k42 lr118:119 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{unsigned int}[r0x68 r0x6A ] = iTemp20 [k39 lr116:118 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int}[r0x68 r0x6A ] | iTemp22 [k41 lr117:118 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{unsigned int}[r0x69 r0x6B ]
main.c(219:119:147:0:0)		ret iTemp23 [k42 lr118:119 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{unsigned int}[r0x68 r0x6A ]
main.c(219:120:148:0:0)	 _return($27) :
main.c(219:121:149:0:0)		eproc _sensorSample [k1 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int function    }

----------------------------------------------------------------
Basic Block _iffalse_8 : loop Depth = 0 noPath = 0 , lastinLoop = 0
depth 1st num 22 : bbnum = 21 1st iCode = 96 , last iCode = 107
visited 0 : hasFcall = 0

defines bitVector :bitvector Size = 153 bSize = 20
Bits on { }

local defines bitVector :
pointers Set bitvector :
in pointers Set bitvector :
inDefs Set bitvector :bitvector Size = 153 bSize = 20
Bits on { (2) (3) (4) (5) (7) (8) (9) (10) (11) (15) (18) (20) (24) (27) (29) (33) (36) (38) (42) (45) (49) (50) (72) (73) (81) (97) (114) }

outDefs Set bitvector :bitvector Size = 153 bSize = 20
Bits on { (2) (3) (4) (5) (7) (8) (9) (10) (11) (15) (18) (20) (24) (27) (29) (33) (36) (38) (42) (45) (49) (50) (72) (73) (81) (97) (114) }

usesDefs Set bitvector :bitvector Size = 153 bSize = 20
Bits on { (42) (45) }

----------------------------------------------------------------
main.c(205:96:115:0:0)	 _iffalse_8($21) :
main.c(206:97:116:0:0)	 nop main.c(206:98:117:0:0)	 nop main.c(206:99:118:0:0)	 nop main.c(206:100:119:0:0)	 nop main.c(206:101:120:0:0)	 nop main.c(207:102:121:0:0)	 nop main.c(207:103:122:0:0)	 nop main.c(207:104:123:0:0)	 nop main.c(207:105:124:0:0)	 nop main.c(207:106:125:0:0)	 nop main.c(208:107:126:0:0)		if _sensorSample_phase3_1_1 [k21 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{bit bit {0,1}} == 0 goto _whilecontinue_1($24)
main.c(209:108:130:0:0)		_GO_DONE [k26 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{bit bit {0,1}} := 0x1 {literal bit {0,1}}
main.c(212:110:132:1:0)	 _whilecontinue_1($24) :
main.c(212:111:133:1:0)		if _NOT_DONE [k31 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{bit bit {0,1}} != 0 goto _whilecontinue_1($24)
main.c(215:112:138:0:0)		_TRISB [k10 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{sfr volatile unsigned char} := 0xf0 {literal unsigned char}
main.c(217:113:139:0:0)		iTemp17 [k32 lr113:115 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sensorSample_h_1_1}[r0x68 ] := _ADRESH [k34 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{sfr volatile unsigned char}
main.c(218:114:140:0:0)		iTemp18 [k35 lr114:117 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sensorSample_l_1_1}[r0x69 ] := _ADRESL [k37 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{sfr volatile unsigned char}
main.c(219:115:141:0:0)		iTemp19 [k38 lr115:116 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int}[r0x68 r0x6A ] = (int)iTemp17 [k32 lr113:115 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sensorSample_h_1_1}[r0x68 ]
main.c(219:116:142:0:0)		iTemp20 [k39 lr116:118 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int}[r0x68 r0x6A ] = iTemp19 [k38 lr115:116 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int}[r0x68 r0x6A ] << 0x8 {literal unsigned char}
main.c(219:117:144:0:0)		iTemp22 [k41 lr117:118 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{unsigned int}[r0x69 r0x6B ] = (unsigned int)iTemp18 [k35 lr114:117 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sensorSample_l_1_1}[r0x69 ]
main.c(219:118:145:0:0)		iTemp23 [k42 lr118:119 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{unsigned int}[r0x68 r0x6A ] = iTemp20 [k39 lr116:118 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int}[r0x68 r0x6A ] | iTemp22 [k41 lr117:118 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{unsigned int}[r0x69 r0x6B ]
main.c(219:119:147:0:0)		ret iTemp23 [k42 lr118:119 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{unsigned int}[r0x68 r0x6A ]
main.c(219:120:148:0:0)	 _return($27) :
main.c(219:121:149:0:0)		eproc _sensorSample [k1 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int function    }

----------------------------------------------------------------
Basic Block _eBBlock8 : loop Depth = 0 noPath = 0 , lastinLoop = 0
depth 1st num 23 : bbnum = 22 1st iCode = 108 , last iCode = 108
visited 0 : hasFcall = 0

defines bitVector :bitvector Size = 153 bSize = 20
Bits on { (130) }

local defines bitVector :bitvector Size = 153 bSize = 20
Bits on { (130) }

pointers Set bitvector :
in pointers Set bitvector :
inDefs Set bitvector :bitvector Size = 153 bSize = 20
Bits on { (2) (3) (4) (5) (7) (8) (9) (10) (11) (15) (18) (20) (24) (27) (29) (33) (36) (38) (42) (45) (49) (50) (72) (73) (81) (97) (114) }

outDefs Set bitvector :bitvector Size = 153 bSize = 20
Bits on { (2) (3) (4) (5) (7) (8) (9) (10) (11) (15) (18) (20) (24) (27) (29) (33) (36) (38) (42) (45) (49) (50) (72) (73) (130) }

usesDefs Set bitvector :bitvector Size = 153 bSize = 20
Bits on { }

----------------------------------------------------------------
main.c(209:108:130:0:0)		_GO_DONE [k26 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{bit bit {0,1}} := 0x1 {literal bit {0,1}}
main.c(212:110:132:1:0)	 _whilecontinue_1($24) :
main.c(212:111:133:1:0)		if _NOT_DONE [k31 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{bit bit {0,1}} != 0 goto _whilecontinue_1($24)
main.c(215:112:138:0:0)		_TRISB [k10 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{sfr volatile unsigned char} := 0xf0 {literal unsigned char}
main.c(217:113:139:0:0)		iTemp17 [k32 lr113:115 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sensorSample_h_1_1}[r0x68 ] := _ADRESH [k34 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{sfr volatile unsigned char}
main.c(218:114:140:0:0)		iTemp18 [k35 lr114:117 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sensorSample_l_1_1}[r0x69 ] := _ADRESL [k37 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{sfr volatile unsigned char}
main.c(219:115:141:0:0)		iTemp19 [k38 lr115:116 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int}[r0x68 r0x6A ] = (int)iTemp17 [k32 lr113:115 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sensorSample_h_1_1}[r0x68 ]
main.c(219:116:142:0:0)		iTemp20 [k39 lr116:118 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int}[r0x68 r0x6A ] = iTemp19 [k38 lr115:116 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int}[r0x68 r0x6A ] << 0x8 {literal unsigned char}
main.c(219:117:144:0:0)		iTemp22 [k41 lr117:118 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{unsigned int}[r0x69 r0x6B ] = (unsigned int)iTemp18 [k35 lr114:117 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sensorSample_l_1_1}[r0x69 ]
main.c(219:118:145:0:0)		iTemp23 [k42 lr118:119 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{unsigned int}[r0x68 r0x6A ] = iTemp20 [k39 lr116:118 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int}[r0x68 r0x6A ] | iTemp22 [k41 lr117:118 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{unsigned int}[r0x69 r0x6B ]
main.c(219:119:147:0:0)		ret iTemp23 [k42 lr118:119 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{unsigned int}[r0x68 r0x6A ]
main.c(219:120:148:0:0)	 _return($27) :
main.c(219:121:149:0:0)		eproc _sensorSample [k1 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int function    }

----------------------------------------------------------------
Basic Block preHeaderLbl1 : loop Depth = 0 noPath = 0 , lastinLoop = 0
depth 1st num 24 : bbnum = 23 1st iCode = 109 , last iCode = 109
visited 0 : hasFcall = 0

defines bitVector :bitvector Size = 153 bSize = 20
Bits on { }

local defines bitVector :
pointers Set bitvector :
in pointers Set bitvector :
inDefs Set bitvector :bitvector Size = 153 bSize = 20
Bits on { (2) (3) (4) (5) (7) (8) (9) (10) (11) (15) (18) (20) (24) (27) (29) (33) (36) (38) (42) (45) (49) (50) (72) (73) (81) (97) (114) (130) }

outDefs Set bitvector :bitvector Size = 153 bSize = 20
Bits on { (2) (3) (4) (5) (7) (8) (9) (10) (11) (15) (18) (20) (24) (27) (29) (33) (36) (38) (42) (45) (49) (50) (72) (73) (81) (97) (114) (130) }

usesDefs Set bitvector :bitvector Size = 153 bSize = 20
Bits on { }

----------------------------------------------------------------
main.c(212:109:151:0:0)	 preHeaderLbl1($39) :
main.c(212:110:132:1:0)	 _whilecontinue_1($24) :
main.c(212:111:133:1:0)		if _NOT_DONE [k31 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{bit bit {0,1}} != 0 goto _whilecontinue_1($24)
main.c(215:112:138:0:0)		_TRISB [k10 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{sfr volatile unsigned char} := 0xf0 {literal unsigned char}
main.c(217:113:139:0:0)		iTemp17 [k32 lr113:115 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sensorSample_h_1_1}[r0x68 ] := _ADRESH [k34 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{sfr volatile unsigned char}
main.c(218:114:140:0:0)		iTemp18 [k35 lr114:117 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sensorSample_l_1_1}[r0x69 ] := _ADRESL [k37 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{sfr volatile unsigned char}
main.c(219:115:141:0:0)		iTemp19 [k38 lr115:116 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int}[r0x68 r0x6A ] = (int)iTemp17 [k32 lr113:115 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sensorSample_h_1_1}[r0x68 ]
main.c(219:116:142:0:0)		iTemp20 [k39 lr116:118 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int}[r0x68 r0x6A ] = iTemp19 [k38 lr115:116 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int}[r0x68 r0x6A ] << 0x8 {literal unsigned char}
main.c(219:117:144:0:0)		iTemp22 [k41 lr117:118 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{unsigned int}[r0x69 r0x6B ] = (unsigned int)iTemp18 [k35 lr114:117 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sensorSample_l_1_1}[r0x69 ]
main.c(219:118:145:0:0)		iTemp23 [k42 lr118:119 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{unsigned int}[r0x68 r0x6A ] = iTemp20 [k39 lr116:118 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int}[r0x68 r0x6A ] | iTemp22 [k41 lr117:118 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{unsigned int}[r0x69 r0x6B ]
main.c(219:119:147:0:0)		ret iTemp23 [k42 lr118:119 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{unsigned int}[r0x68 r0x6A ]
main.c(219:120:148:0:0)	 _return($27) :
main.c(219:121:149:0:0)		eproc _sensorSample [k1 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int function    }

----------------------------------------------------------------
Basic Block _whilecontinue_1 : loop Depth = 1 noPath = 0 , lastinLoop = 0
depth 1st num 25 : bbnum = 24 1st iCode = 110 , last iCode = 111
visited 0 : hasFcall = 0

defines bitVector :bitvector Size = 153 bSize = 20
Bits on { }

local defines bitVector :
pointers Set bitvector :
in pointers Set bitvector :
inDefs Set bitvector :bitvector Size = 153 bSize = 20
Bits on { (2) (3) (4) (5) (7) (8) (9) (10) (11) (15) (18) (20) (24) (27) (29) (33) (36) (38) (42) (45) (49) (50) (72) (73) (80) (81) (96) (97) (113) (114) (129) (130) }

outDefs Set bitvector :bitvector Size = 153 bSize = 20
Bits on { (2) (3) (4) (5) (7) (8) (9) (10) (11) (15) (18) (20) (24) (27) (29) (33) (36) (38) (42) (45) (49) (50) (72) (73) (80) (81) (96) (97) (113) (114) (129) (130) }

usesDefs Set bitvector :bitvector Size = 153 bSize = 20
Bits on { }

----------------------------------------------------------------
main.c(212:110:132:1:0)	 _whilecontinue_1($24) :
main.c(212:111:133:1:0)		if _NOT_DONE [k31 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{bit bit {0,1}} != 0 goto _whilecontinue_1($24)
main.c(215:112:138:0:0)		_TRISB [k10 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{sfr volatile unsigned char} := 0xf0 {literal unsigned char}
main.c(217:113:139:0:0)		iTemp17 [k32 lr113:115 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sensorSample_h_1_1}[r0x68 ] := _ADRESH [k34 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{sfr volatile unsigned char}
main.c(218:114:140:0:0)		iTemp18 [k35 lr114:117 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sensorSample_l_1_1}[r0x69 ] := _ADRESL [k37 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{sfr volatile unsigned char}
main.c(219:115:141:0:0)		iTemp19 [k38 lr115:116 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int}[r0x68 r0x6A ] = (int)iTemp17 [k32 lr113:115 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sensorSample_h_1_1}[r0x68 ]
main.c(219:116:142:0:0)		iTemp20 [k39 lr116:118 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int}[r0x68 r0x6A ] = iTemp19 [k38 lr115:116 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int}[r0x68 r0x6A ] << 0x8 {literal unsigned char}
main.c(219:117:144:0:0)		iTemp22 [k41 lr117:118 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{unsigned int}[r0x69 r0x6B ] = (unsigned int)iTemp18 [k35 lr114:117 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sensorSample_l_1_1}[r0x69 ]
main.c(219:118:145:0:0)		iTemp23 [k42 lr118:119 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{unsigned int}[r0x68 r0x6A ] = iTemp20 [k39 lr116:118 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int}[r0x68 r0x6A ] | iTemp22 [k41 lr117:118 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{unsigned int}[r0x69 r0x6B ]
main.c(219:119:147:0:0)		ret iTemp23 [k42 lr118:119 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{unsigned int}[r0x68 r0x6A ]
main.c(219:120:148:0:0)	 _return($27) :
main.c(219:121:149:0:0)		eproc _sensorSample [k1 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int function    }

----------------------------------------------------------------
Basic Block _eBBlock9 : loop Depth = 0 noPath = 0 , lastinLoop = 0
depth 1st num 26 : bbnum = 25 1st iCode = 112 , last iCode = 119
visited 0 : hasFcall = 0

defines bitVector :bitvector Size = 153 bSize = 20
Bits on { (138) (139) (140) (141) (142) (144) (145) }

local defines bitVector :bitvector Size = 153 bSize = 20
Bits on { (138) (139) (140) (141) (142) (144) (145) }

pointers Set bitvector :
in pointers Set bitvector :
inDefs Set bitvector :bitvector Size = 153 bSize = 20
Bits on { (2) (3) (4) (5) (7) (8) (9) (10) (11) (15) (18) (20) (24) (27) (29) (33) (36) (38) (42) (45) (49) (50) (72) (73) (80) (81) (96) (97) (113) (114) (129) (130) }

outDefs Set bitvector :bitvector Size = 153 bSize = 20
Bits on { (2) (3) (4) (5) (7) (8) (10) (11) (15) (18) (20) (24) (27) (29) (33) (36) (38) (42) (45) (49) (50) (72) (73) (80) (81) (96) (97) (113) (114) (129) (130) (138) (139) (140) (141) (142) (144) (145) }

usesDefs Set bitvector :bitvector Size = 153 bSize = 20
Bits on { (139) (140) (141) (142) (144) (145) }

----------------------------------------------------------------
main.c(215:112:138:0:0)		_TRISB [k10 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{sfr volatile unsigned char} := 0xf0 {literal unsigned char}
main.c(217:113:139:0:0)		iTemp17 [k32 lr113:115 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sensorSample_h_1_1}[r0x68 ] := _ADRESH [k34 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{sfr volatile unsigned char}
main.c(218:114:140:0:0)		iTemp18 [k35 lr114:117 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sensorSample_l_1_1}[r0x69 ] := _ADRESL [k37 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{sfr volatile unsigned char}
main.c(219:115:141:0:0)		iTemp19 [k38 lr115:116 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int}[r0x68 r0x6A ] = (int)iTemp17 [k32 lr113:115 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sensorSample_h_1_1}[r0x68 ]
main.c(219:116:142:0:0)		iTemp20 [k39 lr116:118 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int}[r0x68 r0x6A ] = iTemp19 [k38 lr115:116 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int}[r0x68 r0x6A ] << 0x8 {literal unsigned char}
main.c(219:117:144:0:0)		iTemp22 [k41 lr117:118 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{unsigned int}[r0x69 r0x6B ] = (unsigned int)iTemp18 [k35 lr114:117 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sensorSample_l_1_1}[r0x69 ]
main.c(219:118:145:0:0)		iTemp23 [k42 lr118:119 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{unsigned int}[r0x68 r0x6A ] = iTemp20 [k39 lr116:118 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int}[r0x68 r0x6A ] | iTemp22 [k41 lr117:118 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{unsigned int}[r0x69 r0x6B ]
main.c(219:119:147:0:0)		ret iTemp23 [k42 lr118:119 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{unsigned int}[r0x68 r0x6A ]
main.c(219:120:148:0:0)	 _return($27) :
main.c(219:121:149:0:0)		eproc _sensorSample [k1 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int function    }

----------------------------------------------------------------
Basic Block _return : loop Depth = 0 noPath = 0 , lastinLoop = 0
depth 1st num 27 : bbnum = 26 1st iCode = 120 , last iCode = 121
visited 0 : hasFcall = 0

defines bitVector :bitvector Size = 153 bSize = 20
Bits on { }

local defines bitVector :
pointers Set bitvector :
in pointers Set bitvector :
inDefs Set bitvector :bitvector Size = 153 bSize = 20
Bits on { (2) (3) (4) (5) (7) (8) (10) (11) (15) (18) (20) (24) (27) (29) (33) (36) (38) (42) (45) (49) (50) (72) (73) (80) (81) (96) (97) (113) (114) (129) (130) (138) (139) (140) (141) (142) (144) (145) }

outDefs Set bitvector :bitvector Size = 153 bSize = 20
Bits on { (2) (3) (4) (5) (7) (8) (10) (11) (15) (18) (20) (24) (27) (29) (33) (36) (38) (42) (45) (49) (50) (72) (73) (80) (81) (96) (97) (113) (114) (129) (130) (138) (139) (140) (141) (142) (144) (145) }

usesDefs Set bitvector :bitvector Size = 153 bSize = 20
Bits on { }

----------------------------------------------------------------
main.c(219:120:148:0:0)	 _return($27) :
main.c(219:121:149:0:0)		eproc _sensorSample [k1 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int function    }
typeRegWithIdx - requesting index = 0x63
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
Found a Stack Register!
typeRegWithIdx - requesting index = 0x64
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x1000
typeRegWithIdx - requesting index = 0x1000
typeRegWithIdx - requesting index = 0x1000
Found a Stack Register!
typeRegWithIdx - requesting index = 0x65
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x66
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x66
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x64
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x65
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x65
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x65
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x65
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x67
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x63
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x67
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x68
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x69
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x68
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x68
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6a
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x68
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6a
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x68
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x69
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x69
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6b
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x69
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x68
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6b
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6a
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x68
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
Found a Stack Register!
typeRegWithIdx - requesting index = 0x6a
Found a Dynamic Register!
leaving
<><><><><><><><><><><><><><><><><>
<><><><><><><><><><><><><><><><><>
starting	ralloc.c:pic14_assignRegisters
ebbs before optimizing:

----------------------------------------------------------------
Basic Block _entry : loop Depth = 0 noPath = 0 , lastinLoop = 0
depth 1st num 1 : bbnum = 0 1st iCode = 1 , last iCode = 2
visited 0 : hasFcall = 0

defines bitVector :bitvector Size = 15 bSize = 2
Bits on { }

local defines bitVector :bitvector Size = 15 bSize = 2
Bits on { (2) }

pointers Set bitvector :
in pointers Set bitvector :
inDefs Set bitvector :
outDefs Set bitvector :bitvector Size = 15 bSize = 2
Bits on { }

usesDefs Set bitvector :bitvector Size = 15 bSize = 2
Bits on { }

----------------------------------------------------------------
main.c(232:1:0:0:0)	 _entry($5) :
main.c(232:2:1:0:0)		proc _serialBitDelay [k1 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{void function    }

----------------------------------------------------------------
Basic Block preHeaderLbl0 : loop Depth = 0 noPath = 0 , lastinLoop = 0
depth 1st num 2 : bbnum = 1 1st iCode = 3 , last iCode = 4
visited 0 : hasFcall = 0

defines bitVector :bitvector Size = 15 bSize = 2
Bits on { (14) }

local defines bitVector :bitvector Size = 15 bSize = 2
Bits on { (14) }

pointers Set bitvector :
in pointers Set bitvector :
inDefs Set bitvector :bitvector Size = 15 bSize = 2
Bits on { (2) }

outDefs Set bitvector :bitvector Size = 15 bSize = 2
Bits on { (2) (14) }

usesDefs Set bitvector :bitvector Size = 15 bSize = 2
Bits on { }

----------------------------------------------------------------
main.c(234:3:13:0:0)	 preHeaderLbl0($6) :
main.c(234:4:14:0:0)		iTemp1 [k4 lr4:8 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _serialBitDelay_i_1_1} := 0x40 {literal unsigned char}

----------------------------------------------------------------
Basic Block _dobody_2 : loop Depth = 1 noPath = 0 , lastinLoop = 1
depth 1st num 3 : bbnum = 2 1st iCode = 5 , last iCode = 8
visited 0 : hasFcall = 0

defines bitVector :bitvector Size = 15 bSize = 2
Bits on { (6) }

local defines bitVector :bitvector Size = 15 bSize = 2
Bits on { (6) (7) }

pointers Set bitvector :
in pointers Set bitvector :
inDefs Set bitvector :bitvector Size = 15 bSize = 2
Bits on { (2) (6) (7) (14) }

outDefs Set bitvector :bitvector Size = 15 bSize = 2
Bits on { (6) }

usesDefs Set bitvector :bitvector Size = 15 bSize = 2
Bits on { (6) (14) }

----------------------------------------------------------------
main.c(234:5:3:1:0)	 _dobody_2($1) :
main.c(237:6:4:1:0)	
      nop
    main.c(238:7:6:1:0)		iTemp1 [k4 lr4:8 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _serialBitDelay_i_1_1} = iTemp1 [k4 lr4:8 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _serialBitDelay_i_1_1} - 0x1 {literal unsigned char}
main.c(238:8:8:1:0)		if iTemp1 [k4 lr4:8 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _serialBitDelay_i_1_1} != 0 goto _dobody_2($1)

----------------------------------------------------------------
Basic Block _return : loop Depth = 0 noPath = 0 , lastinLoop = 0
depth 1st num 4 : bbnum = 3 1st iCode = 9 , last iCode = 10
visited 0 : hasFcall = 0

defines bitVector :bitvector Size = 15 bSize = 2
Bits on { }

local defines bitVector :
pointers Set bitvector :
in pointers Set bitvector :
inDefs Set bitvector :bitvector Size = 15 bSize = 2
Bits on { (6) (7) }

outDefs Set bitvector :bitvector Size = 15 bSize = 2
Bits on { (6) (7) }

usesDefs Set bitvector :bitvector Size = 15 bSize = 2
Bits on { }

----------------------------------------------------------------
main.c(238:9:11:0:0)	 _return($4) :
main.c(238:10:12:0:0)		eproc _serialBitDelay [k1 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{void function    }
packRegisters
  left:_serialBitDelay [k1 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{void function    }
        Symbol type: void function    
packRegisters
packRegsForAssign
  result:iTemp1 [k4 lr4:8 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _serialBitDelay_i_1_1}
  left:
  right:0x40 {literal unsigned char}
  2705 - not packing - right is not temp
allocDirReg BAD, op is NULL
  result:iTemp1 [k4 lr4:8 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _serialBitDelay_i_1_1}
  left:
  right:0x40 {literal unsigned char}
  result:iTemp1 [k4 lr4:8 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _serialBitDelay_i_1_1}
          Symbol type: unsigned char
  3594 - pointer reg req = 0
packRegisters
  left:iTemp1 [k4 lr4:8 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _serialBitDelay_i_1_1}
        Symbol type: unsigned char
  result:iTemp1 [k4 lr4:8 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _serialBitDelay_i_1_1}
          Symbol type: unsigned char
  3594 - pointer reg req = 0
packRegsForSupport
packRegsForAccUse
  packRegsForAccUse:3181
  packRegsForAccUse:3188
  left:iTemp1 [k4 lr4:8 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _serialBitDelay_i_1_1}
        Symbol type: unsigned char
  3594 - pointer reg req = 0
packRegisters
  left:_serialBitDelay [k1 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{void function    }
        Symbol type: void function    
dir registers allocated so far:
  -- #0 reg = s0xFFF  key 0, rIdx = 4095, size 1
  -- #1 reg = s0x1000  key 0, rIdx = 4096, size 1
  -- #2 reg = s0x1001  key 0, rIdx = 4097, size 1
  -- #3 reg = s0x1002  key 0, rIdx = 4098, size 1
  -- #4 reg = s0x1003  key 0, rIdx = 4099, size 1
  -- #5 reg = s0x1004  key 0, rIdx = 4100, size 1
  -- #6 reg = s0x1005  key 0, rIdx = 4101, size 1
  -- #7 reg = s0x1006  key 0, rIdx = 4102, size 1
  -- #8 reg = r0x20  key 0, rIdx = 32, size 1
  -- #9 reg = r0x21  key 0, rIdx = 33, size 1
  -- #10 reg = r0x22  key 0, rIdx = 34, size 1
  -- #11 reg = r0x23  key 0, rIdx = 35, size 1
  -- #12 reg = r0x24  key 0, rIdx = 36, size 1
  -- #13 reg = r0x25  key 0, rIdx = 37, size 1
  -- #14 reg = r0x26  key 0, rIdx = 38, size 1
  -- #15 reg = r0x27  key 0, rIdx = 39, size 1
  -- #16 reg = r0x28  key 0, rIdx = 40, size 1
  -- #17 reg = r0x29  key 0, rIdx = 41, size 1
  -- #18 reg = r0x2A  key 0, rIdx = 42, size 1
  -- #19 reg = r0x2B  key 0, rIdx = 43, size 1
  -- #20 reg = r0x2C  key 0, rIdx = 44, size 1
  -- #21 reg = r0x2D  key 0, rIdx = 45, size 1
  -- #22 reg = r0x2E  key 0, rIdx = 46, size 1
  -- #23 reg = r0x2F  key 0, rIdx = 47, size 1
  -- #24 reg = r0x30  key 0, rIdx = 48, size 1
  -- #25 reg = r0x31  key 0, rIdx = 49, size 1
  -- #26 reg = r0x32  key 0, rIdx = 50, size 1
  -- #27 reg = r0x33  key 0, rIdx = 51, size 1
  -- #28 reg = r0x34  key 0, rIdx = 52, size 1
  -- #29 reg = r0x35  key 0, rIdx = 53, size 1
  -- #30 reg = r0x36  key 0, rIdx = 54, size 1
  -- #31 reg = r0x37  key 0, rIdx = 55, size 1
  -- #32 reg = r0x38  key 0, rIdx = 56, size 1
  -- #33 reg = r0x39  key 0, rIdx = 57, size 1
  -- #34 reg = r0x3A  key 0, rIdx = 58, size 1
  -- #35 reg = r0x3B  key 0, rIdx = 59, size 1
  -- #36 reg = r0x3C  key 0, rIdx = 60, size 1
  -- #37 reg = r0x3D  key 0, rIdx = 61, size 1
  -- #38 reg = r0x3E  key 0, rIdx = 62, size 1
  -- #39 reg = r0x3F  key 0, rIdx = 63, size 1
  -- #40 reg = r0x40  key 0, rIdx = 64, size 1
  -- #41 reg = r0x41  key 0, rIdx = 65, size 1
  -- #42 reg = r0x42  key 0, rIdx = 66, size 1
  -- #43 reg = r0x43  key 0, rIdx = 67, size 1
  -- #44 reg = r0x44  key 0, rIdx = 68, size 1
  -- #45 reg = r0x45  key 0, rIdx = 69, size 1
  -- #46 reg = r0x46  key 0, rIdx = 70, size 1
  -- #47 reg = r0x47  key 0, rIdx = 71, size 1
  -- #48 reg = r0x48  key 0, rIdx = 72, size 1
  -- #49 reg = r0x49  key 0, rIdx = 73, size 1
  -- #50 reg = r0x4A  key 0, rIdx = 74, size 1
  -- #51 reg = r0x4B  key 0, rIdx = 75, size 1
  -- #52 reg = r0x4C  key 0, rIdx = 76, size 1
  -- #53 reg = r0x4D  key 0, rIdx = 77, size 1
  -- #54 reg = r0x4E  key 0, rIdx = 78, size 1
  -- #55 reg = r0x4F  key 0, rIdx = 79, size 1
  -- #56 reg = r0x50  key 0, rIdx = 80, size 1
  -- #57 reg = r0x51  key 0, rIdx = 81, size 1
  -- #58 reg = r0x52  key 0, rIdx = 82, size 1
  -- #59 reg = r0x53  key 0, rIdx = 83, size 1
  -- #60 reg = r0x54  key 0, rIdx = 84, size 1
  -- #61 reg = r0x55  key 0, rIdx = 85, size 1
  -- #62 reg = r0x56  key 0, rIdx = 86, size 1
  -- #63 reg = r0x57  key 0, rIdx = 87, size 1
  -- #64 reg = r0x58  key 0, rIdx = 88, size 1
  -- #65 reg = r0x59  key 0, rIdx = 89, size 1
  -- #66 reg = r0x5A  key 0, rIdx = 90, size 1
  -- #67 reg = r0x5B  key 0, rIdx = 91, size 1
  -- #68 reg = r0x5C  key 0, rIdx = 92, size 1
  -- #69 reg = r0x5D  key 0, rIdx = 93, size 1
  -- #70 reg = r0x5E  key 0, rIdx = 94, size 1
  -- #71 reg = r0x5F  key 0, rIdx = 95, size 1
  -- #72 reg = r0x60  key 0, rIdx = 96, size 1
  -- #73 reg = r0x61  key 0, rIdx = 97, size 1
  -- #74 reg = r0x62  key 0, rIdx = 98, size 1
  -- #75 reg = _ADRESH  key 0, rIdx = 33, size 1
  -- #76 reg = r0x63  key 0, rIdx = 99, size 1
  -- #77 reg = r0x64  key 0, rIdx = 100, size 1
  -- #78 reg = r0x65  key 0, rIdx = 101, size 1
  -- #79 reg = r0x66  key 0, rIdx = 102, size 1
  -- #80 reg = r0x67  key 0, rIdx = 103, size 1
  -- #81 reg = r0x68  key 0, rIdx = 104, size 1
  -- #82 reg = r0x69  key 0, rIdx = 105, size 1
  -- #83 reg = r0x6A  key 0, rIdx = 106, size 1
  -- #84 reg = r0x6B  key 0, rIdx = 107, size 1
  -- #85 reg = _ADRESL  key 5, rIdx = 34, size 1
  -- #86 reg = _PCLATH  key 6, rIdx = 10, size 1
  -- #87 reg = _TRISA  key 7, rIdx = 1, size 1
  -- #88 reg = _TRISB  key 8, rIdx = 2, size 1
  -- #89 reg = _TRISB  key 8, rIdx = 23, size 1
  -- #90 reg = _TRISB  key 8, rIdx = 32, size 1
  -- #91 reg = _TRISC  key 9, rIdx = 3, size 1
  -- #92 reg = _TRISD  key 10, rIdx = 4, size 1
  -- #93 reg = _PORTA  key 10, rIdx = 6, size 1
  -- #94 reg = _TRISE  key 11, rIdx = 5, size 1
  -- #95 reg = _PORTB  key 11, rIdx = 7, size 1
  -- #96 reg = _PORTB  key 11, rIdx = 22, size 1
  -- #97 reg = _PORTB  key 11, rIdx = 30, size 1
  -- #98 reg = _PORTC  key 12, rIdx = 8, size 1
  -- #99 reg = _GIE  key 12, rIdx = 13, size 1
  -- #100 reg = _PORTD  key 13, rIdx = 9, size 1
  -- #101 reg = _PORTE  key 14, rIdx = 10, size 1
  -- #102 reg = _sensorSample_phase0_1_1  key 15, rIdx = 26, size 1
  -- #103 reg = _sensorSample_phase1_1_1  key 16, rIdx = 27, size 1
  -- #104 reg = _sensorSample_phase2_1_1  key 17, rIdx = 28, size 1
  -- #105 reg = _sensorSample_phase3_1_1  key 18, rIdx = 29, size 1
  -- #106 reg = _ADCON0  key 27, rIdx = 12, size 1
  -- #107 reg = _ADCON0  key 27, rIdx = 24, size 1
  -- #108 reg = _ADCON0  key 27, rIdx = 25, size 1
  -- #109 reg = _ADCON1  key 28, rIdx = 11, size 1
  -- #110 reg = SSAVE  key 32, rIdx = 32765, size 1
  -- #111 reg = WSAVE  key 36, rIdx = 32766, size 1
  -- #112 reg = _reading0  key 44, rIdx = 14, size 2
  -- #113 reg = _GO_DONE  key 44, rIdx = 31, size 1
  -- #114 reg = _reading1  key 45, rIdx = 15, size 2
  -- #115 reg = _reading2  key 46, rIdx = 16, size 2
  -- #116 reg = INTCON  key 47, rIdx = 11, size 1
  -- #117 reg = _reading3  key 47, rIdx = 17, size 2
  -- #118 reg = _STATUS  key 48, rIdx = 3, size 1
  -- #119 reg = PCL  key 48, rIdx = 2, size 1
  -- #120 reg = _reading4  key 48, rIdx = 18, size 2
  -- #121 reg = KZ  key 49, rIdx = 32767, size 1
  -- #122 reg = _reading5  key 49, rIdx = 19, size 2
  -- #123 reg = _reading6  key 50, rIdx = 20, size 2
  -- #124 reg = _reading7  key 51, rIdx = 21, size 2
  -- #125 reg = INDF  key 56, rIdx = 0, size 1
  -- #126 reg = _Light0  key 56, rIdx = 0, size 1
  -- #127 reg = FSR  key 60, rIdx = 4, size 1
regTypeNum
  2421 - iTemp1
  2431 - itemp register
  reg name iTemp1,  reg type REG_GPR
serialRegAssign
  op: LABEL
deassignLRs
  op: unkown op 9 	
deassignLRs
  op: LABEL
deassignLRs
  op: =
deassignLRs
willCauseSpill
computeSpillable
  2125 - 
bitvector Size = 58 bSize = 8
Bits on { (4) }
getRegGpr
allocReg of type REG_GPR
  2141 - 
  2155 - 
  op: LABEL
deassignLRs
  op: INLINEASM
deassignLRs
  op: -
deassignLRs
  op: IFX
deassignLRs
freeReg
  op: LABEL
deassignLRs
  op: ENDFUNCTION
deassignLRs
createRegMask
regsUsedIniCode
rUmaskForOp
rUmaskForOp
regsUsedIniCode
rUmaskForOp
rUmaskForOp
rUmaskForOp
regsUsedIniCode
rUmaskForOp
ebbs after optimizing:

----------------------------------------------------------------
Basic Block _entry : loop Depth = 0 noPath = 0 , lastinLoop = 0
depth 1st num 1 : bbnum = 0 1st iCode = 1 , last iCode = 2
visited 0 : hasFcall = 0

defines bitVector :bitvector Size = 15 bSize = 2
Bits on { }

local defines bitVector :bitvector Size = 15 bSize = 2
Bits on { (2) }

pointers Set bitvector :
in pointers Set bitvector :
inDefs Set bitvector :
outDefs Set bitvector :bitvector Size = 15 bSize = 2
Bits on { }

usesDefs Set bitvector :bitvector Size = 15 bSize = 2
Bits on { }

----------------------------------------------------------------
main.c(232:1:0:0:0)	 _entry($5) :
main.c(232:2:1:0:0)		proc _serialBitDelay [k1 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{void function    }
main.c(234:4:14:0:0)		iTemp1 [k4 lr4:8 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _serialBitDelay_i_1_1}[r0x6C ] := 0x40 {literal unsigned char}
main.c(234:5:3:1:0)	 _dobody_2($1) :
main.c(237:6:4:1:0)	
      nop
    main.c(238:7:6:1:0)		iTemp1 [k4 lr4:8 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _serialBitDelay_i_1_1}[r0x6C ] = iTemp1 [k4 lr4:8 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _serialBitDelay_i_1_1}[r0x6C ] - 0x1 {literal unsigned char}
main.c(238:8:8:1:0)		if iTemp1 [k4 lr4:8 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _serialBitDelay_i_1_1}[r0x6C ] != 0 goto _dobody_2($1)
main.c(238:9:11:0:0)	 _return($4) :
main.c(238:10:12:0:0)		eproc _serialBitDelay [k1 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{void function    }

----------------------------------------------------------------
Basic Block preHeaderLbl0 : loop Depth = 0 noPath = 0 , lastinLoop = 0
depth 1st num 2 : bbnum = 1 1st iCode = 3 , last iCode = 4
visited 0 : hasFcall = 0

defines bitVector :bitvector Size = 15 bSize = 2
Bits on { (14) }

local defines bitVector :bitvector Size = 15 bSize = 2
Bits on { (14) }

pointers Set bitvector :
in pointers Set bitvector :
inDefs Set bitvector :bitvector Size = 15 bSize = 2
Bits on { (2) }

outDefs Set bitvector :bitvector Size = 15 bSize = 2
Bits on { (2) (14) }

usesDefs Set bitvector :bitvector Size = 15 bSize = 2
Bits on { }

----------------------------------------------------------------
main.c(234:3:13:0:0)	 preHeaderLbl0($6) :
main.c(234:4:14:0:0)		iTemp1 [k4 lr4:8 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _serialBitDelay_i_1_1}[r0x6C ] := 0x40 {literal unsigned char}
main.c(234:5:3:1:0)	 _dobody_2($1) :
main.c(237:6:4:1:0)	
      nop
    main.c(238:7:6:1:0)		iTemp1 [k4 lr4:8 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _serialBitDelay_i_1_1}[r0x6C ] = iTemp1 [k4 lr4:8 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _serialBitDelay_i_1_1}[r0x6C ] - 0x1 {literal unsigned char}
main.c(238:8:8:1:0)		if iTemp1 [k4 lr4:8 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _serialBitDelay_i_1_1}[r0x6C ] != 0 goto _dobody_2($1)
main.c(238:9:11:0:0)	 _return($4) :
main.c(238:10:12:0:0)		eproc _serialBitDelay [k1 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{void function    }

----------------------------------------------------------------
Basic Block _dobody_2 : loop Depth = 1 noPath = 0 , lastinLoop = 1
depth 1st num 3 : bbnum = 2 1st iCode = 5 , last iCode = 8
visited 0 : hasFcall = 0

defines bitVector :bitvector Size = 15 bSize = 2
Bits on { (6) }

local defines bitVector :bitvector Size = 15 bSize = 2
Bits on { (6) (7) }

pointers Set bitvector :
in pointers Set bitvector :
inDefs Set bitvector :bitvector Size = 15 bSize = 2
Bits on { (2) (6) (7) (14) }

outDefs Set bitvector :bitvector Size = 15 bSize = 2
Bits on { (6) }

usesDefs Set bitvector :bitvector Size = 15 bSize = 2
Bits on { (6) (14) }

----------------------------------------------------------------
main.c(234:5:3:1:0)	 _dobody_2($1) :
main.c(237:6:4:1:0)	
      nop
    main.c(238:7:6:1:0)		iTemp1 [k4 lr4:8 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _serialBitDelay_i_1_1}[r0x6C ] = iTemp1 [k4 lr4:8 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _serialBitDelay_i_1_1}[r0x6C ] - 0x1 {literal unsigned char}
main.c(238:8:8:1:0)		if iTemp1 [k4 lr4:8 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _serialBitDelay_i_1_1}[r0x6C ] != 0 goto _dobody_2($1)
main.c(238:9:11:0:0)	 _return($4) :
main.c(238:10:12:0:0)		eproc _serialBitDelay [k1 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{void function    }

----------------------------------------------------------------
Basic Block _return : loop Depth = 0 noPath = 0 , lastinLoop = 0
depth 1st num 4 : bbnum = 3 1st iCode = 9 , last iCode = 10
visited 0 : hasFcall = 0

defines bitVector :bitvector Size = 15 bSize = 2
Bits on { }

local defines bitVector :
pointers Set bitvector :
in pointers Set bitvector :
inDefs Set bitvector :bitvector Size = 15 bSize = 2
Bits on { (6) (7) }

outDefs Set bitvector :bitvector Size = 15 bSize = 2
Bits on { (6) (7) }

usesDefs Set bitvector :bitvector Size = 15 bSize = 2
Bits on { }

----------------------------------------------------------------
main.c(238:9:11:0:0)	 _return($4) :
main.c(238:10:12:0:0)		eproc _serialBitDelay [k1 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{void function    }
typeRegWithIdx - requesting index = 0x6c
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6c
Found a Dynamic Register!
leaving
<><><><><><><><><><><><><><><><><>
<><><><><><><><><><><><><><><><><>
starting	ralloc.c:pic14_assignRegisters
ebbs before optimizing:

----------------------------------------------------------------
Basic Block _entry : loop Depth = 0 noPath = 0 , lastinLoop = 0
depth 1st num 1 : bbnum = 0 1st iCode = 1 , last iCode = 6
visited 0 : hasFcall = 1

defines bitVector :bitvector Size = 39 bSize = 5
Bits on { (2) (4) (6) }

local defines bitVector :bitvector Size = 39 bSize = 5
Bits on { (2) (4) (6) (7) (8) }

pointers Set bitvector :
in pointers Set bitvector :
inDefs Set bitvector :
outDefs Set bitvector :bitvector Size = 39 bSize = 5
Bits on { (2) (4) (6) (7) }

usesDefs Set bitvector :bitvector Size = 39 bSize = 5
Bits on { (2) (7) }

----------------------------------------------------------------
main.c(241:1:0:0:0)	 _entry($8) :
main.c(241:2:1:0:0)		proc _putc [k1 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{void function    }
main.c(241:3:2:0:0)	iTemp0 [k2 lr3:21 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _putc_byte_1_1} = recv 
main.c(245:4:4:0:0)		_SerialPort [k4 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{bit bit {0,1}} := 0x0 {literal bit {0,1}}
main.c(246:5:6:0:0)		_SerialTris [k6 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{bit bit {0,1}} := 0x0 {literal bit {0,1}}
main.c(247:6:7:0:0)		iTemp3 [k9 lr6:6 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{void} = call _serialBitDelay [k8 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{void function    }

----------------------------------------------------------------
Basic Block preHeaderLbl0 : loop Depth = 0 noPath = 0 , lastinLoop = 0
depth 1st num 2 : bbnum = 1 1st iCode = 7 , last iCode = 8
visited 0 : hasFcall = 0

defines bitVector :bitvector Size = 39 bSize = 5
Bits on { (38) }

local defines bitVector :bitvector Size = 39 bSize = 5
Bits on { (38) }

pointers Set bitvector :
in pointers Set bitvector :
inDefs Set bitvector :bitvector Size = 39 bSize = 5
Bits on { (2) (4) (6) (7) (8) }

outDefs Set bitvector :bitvector Size = 39 bSize = 5
Bits on { (2) (4) (6) (7) (8) (38) }

usesDefs Set bitvector :bitvector Size = 39 bSize = 5
Bits on { }

----------------------------------------------------------------
main.c(251:7:37:0:0)	 preHeaderLbl0($10) :
main.c(251:8:38:0:0)		iTemp10 [k17 lr8:21 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _putc_bitnum_1_1} := 0x8 {literal unsigned char}

----------------------------------------------------------------
Basic Block _dobody_3 : loop Depth = 1 noPath = 0 , lastinLoop = 0
depth 1st num 3 : bbnum = 2 1st iCode = 9 , last iCode = 11
visited 0 : hasFcall = 0

defines bitVector :bitvector Size = 39 bSize = 5
Bits on { (10) }

local defines bitVector :bitvector Size = 39 bSize = 5
Bits on { (10) }

pointers Set bitvector :
in pointers Set bitvector :
inDefs Set bitvector :bitvector Size = 39 bSize = 5
Bits on { (2) (3) (4) (5) (6) (7) (8) (10) (14) (15) (18) (19) (21) (22) (23) (25) (26) (38) }

outDefs Set bitvector :bitvector Size = 39 bSize = 5
Bits on { (2) (3) (4) (5) (6) (7) (8) (10) (14) (15) (18) (19) (21) (22) (23) (25) (26) (38) }

usesDefs Set bitvector :bitvector Size = 39 bSize = 5
Bits on { (2) (10) (22) }

----------------------------------------------------------------
main.c(251:9:9:1:0)	 _dobody_3($4) :
main.c(252:10:10:1:0)		iTemp5 [k12 lr10:11 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{unsigned char} = iTemp0 [k2 lr3:21 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _putc_byte_1_1} & 0x1 {literal unsigned char}
main.c(252:11:11:1:0)		if iTemp5 [k12 lr10:11 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{unsigned char} == 0 goto _iffalse_10($2)

----------------------------------------------------------------
Basic Block _eBBlock0 : loop Depth = 1 noPath = 0 , lastinLoop = 0
depth 1st num 4 : bbnum = 3 1st iCode = 12 , last iCode = 13
visited 0 : hasFcall = 0

defines bitVector :bitvector Size = 39 bSize = 5
Bits on { (15) }

local defines bitVector :bitvector Size = 39 bSize = 5
Bits on { (15) }

pointers Set bitvector :
in pointers Set bitvector :
inDefs Set bitvector :bitvector Size = 39 bSize = 5
Bits on { (2) (3) (4) (5) (6) (7) (8) (10) (14) (15) (18) (19) (21) (22) (23) (25) (26) (38) }

outDefs Set bitvector :bitvector Size = 39 bSize = 5
Bits on { (2) (3) (5) (6) (7) (8) (10) (14) (15) (18) (21) (22) (23) (25) (26) (38) }

usesDefs Set bitvector :bitvector Size = 39 bSize = 5
Bits on { }

----------------------------------------------------------------
main.c(253:12:15:1:0)		_SerialPort [k4 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{bit bit {0,1}} := 0x1 {literal bit {0,1}}
main.c(253:13:16:1:0)		 goto _ifend_10($3)

----------------------------------------------------------------
Basic Block _iffalse_10 : loop Depth = 1 noPath = 0 , lastinLoop = 0
depth 1st num 5 : bbnum = 4 1st iCode = 14 , last iCode = 15
visited 0 : hasFcall = 0

defines bitVector :bitvector Size = 39 bSize = 5
Bits on { (19) }

local defines bitVector :bitvector Size = 39 bSize = 5
Bits on { (19) }

pointers Set bitvector :
in pointers Set bitvector :
inDefs Set bitvector :bitvector Size = 39 bSize = 5
Bits on { (2) (3) (4) (5) (6) (7) (8) (10) (14) (15) (18) (19) (21) (22) (23) (25) (26) (38) }

outDefs Set bitvector :bitvector Size = 39 bSize = 5
Bits on { (2) (3) (5) (6) (7) (8) (10) (14) (18) (19) (21) (22) (23) (25) (26) (38) }

usesDefs Set bitvector :bitvector Size = 39 bSize = 5
Bits on { }

----------------------------------------------------------------
main.c(253:14:17:1:0)	 _iffalse_10($2) :
main.c(255:15:19:1:0)		_SerialPort [k4 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{bit bit {0,1}} := 0x0 {literal bit {0,1}}

----------------------------------------------------------------
Basic Block _ifend_10 : loop Depth = 1 noPath = 0 , lastinLoop = 1
depth 1st num 6 : bbnum = 5 1st iCode = 16 , last iCode = 21
visited 0 : hasFcall = 1

defines bitVector :bitvector Size = 39 bSize = 5
Bits on { (21) (22) (25) }

local defines bitVector :bitvector Size = 39 bSize = 5
Bits on { (21) (22) (23) (25) (26) }

pointers Set bitvector :
in pointers Set bitvector :
inDefs Set bitvector :bitvector Size = 39 bSize = 5
Bits on { (2) (3) (5) (6) (7) (8) (10) (14) (15) (18) (19) (21) (22) (23) (25) (26) (38) }

outDefs Set bitvector :bitvector Size = 39 bSize = 5
Bits on { (3) (5) (6) (7) (10) (14) (15) (18) (19) (21) (22) (23) (25) }

usesDefs Set bitvector :bitvector Size = 39 bSize = 5
Bits on { (2) (21) (22) (23) (25) (38) }

----------------------------------------------------------------
main.c(255:16:20:1:0)	 _ifend_10($3) :
main.c(256:17:21:1:0)		iTemp8 [k15 lr17:18 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{unsigned char} = iTemp0 [k2 lr3:21 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _putc_byte_1_1} >> 0x1 {literal unsigned char}
main.c(256:18:22:1:0)		iTemp0 [k2 lr3:21 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _putc_byte_1_1} := iTemp8 [k15 lr17:18 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{unsigned char}
main.c(257:19:23:1:0)		iTemp9 [k16 lr19:19 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{void} = call _serialBitDelay [k8 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{void function    }
main.c(258:20:25:1:0)		iTemp10 [k17 lr8:21 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _putc_bitnum_1_1} = iTemp10 [k17 lr8:21 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _putc_bitnum_1_1} - 0x1 {literal unsigned char}
main.c(258:21:27:1:0)		if iTemp10 [k17 lr8:21 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _putc_bitnum_1_1} != 0 goto _dobody_3($4)

----------------------------------------------------------------
Basic Block _eBBlock1 : loop Depth = 0 noPath = 0 , lastinLoop = 0
depth 1st num 7 : bbnum = 6 1st iCode = 22 , last iCode = 24
visited 0 : hasFcall = 1

defines bitVector :bitvector Size = 39 bSize = 5
Bits on { (31) (34) }

local defines bitVector :bitvector Size = 39 bSize = 5
Bits on { (31) (32) (34) }

pointers Set bitvector :
in pointers Set bitvector :
inDefs Set bitvector :bitvector Size = 39 bSize = 5
Bits on { (3) (5) (6) (7) (10) (14) (15) (18) (19) (21) (22) (23) (25) (26) }

outDefs Set bitvector :bitvector Size = 39 bSize = 5
Bits on { (3) (5) (7) (10) (14) (18) (21) (22) (23) (25) (26) (31) (32) (34) }

usesDefs Set bitvector :bitvector Size = 39 bSize = 5
Bits on { (32) }

----------------------------------------------------------------
main.c(261:22:31:0:0)		_SerialPort [k4 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{bit bit {0,1}} := 0x1 {literal bit {0,1}}
main.c(262:23:32:0:0)		iTemp12 [k19 lr23:23 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{void} = call _serialBitDelay [k8 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{void function    }
main.c(263:24:34:0:0)		_SerialTris [k6 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{bit bit {0,1}} := 0x1 {literal bit {0,1}}

----------------------------------------------------------------
Basic Block _return : loop Depth = 0 noPath = 0 , lastinLoop = 0
depth 1st num 8 : bbnum = 7 1st iCode = 25 , last iCode = 26
visited 0 : hasFcall = 0

defines bitVector :bitvector Size = 39 bSize = 5
Bits on { }

local defines bitVector :
pointers Set bitvector :
in pointers Set bitvector :
inDefs Set bitvector :bitvector Size = 39 bSize = 5
Bits on { (3) (5) (7) (10) (14) (18) (21) (22) (23) (25) (26) (31) (32) (34) }

outDefs Set bitvector :bitvector Size = 39 bSize = 5
Bits on { (3) (5) (7) (10) (14) (18) (21) (22) (23) (25) (26) (31) (32) (34) }

usesDefs Set bitvector :bitvector Size = 39 bSize = 5
Bits on { }

----------------------------------------------------------------
main.c(263:25:35:0:0)	 _return($7) :
main.c(263:26:36:0:0)		eproc _putc [k1 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{void function    }
packRegisters
packRegsForAssign
  result:_SerialPort [k4 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{bit bit {0,1}}
  left:
  right:0x0 {literal bit {0,1}}
allocDirReg symbol name _SerialPort
  637  storage class 13 
 642  integral
 646  specifier
_SerialPort [k4 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{bit bit {0,1}}
  -- added _SerialPort to hash, size = 1
  -- and it is at a fixed address 0x4a
  2695 - result is not temp
  2705 - not packing - right is not temp
allocDirReg BAD, op is NULL
  result:_SerialPort [k4 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{bit bit {0,1}}
  left:
  right:0x0 {literal bit {0,1}}
packRegsForAssign
  result:_SerialTris [k6 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{bit bit {0,1}}
  left:
  right:0x0 {literal bit {0,1}}
allocDirReg symbol name _SerialTris
  637  storage class 13 
 642  integral
 646  specifier
_SerialTris [k6 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{bit bit {0,1}}
  -- added _SerialTris to hash, size = 1
  -- and it is at a fixed address 0x44a
  2695 - result is not temp
  2705 - not packing - right is not temp
allocDirReg BAD, op is NULL
  result:_SerialTris [k6 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{bit bit {0,1}}
  left:
  right:0x0 {literal bit {0,1}}
  left:_putc [k1 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{void function    }
        Symbol type: void function    
  result:iTemp0 [k2 lr3:21 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _putc_byte_1_1}
          Symbol type: unsigned char
  3594 - pointer reg req = 0
packForReceive
  result:iTemp0 [k2 lr3:21 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _putc_byte_1_1}
  left:
  right:
  hey we can remove this unnecessary assign
  result:_SerialPort [k4 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{bit bit {0,1}}
          Symbol type: bit bit {0,1}
  3594 - pointer reg req = 0
  result:_SerialTris [k6 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{bit bit {0,1}}
          Symbol type: bit bit {0,1}
  3594 - pointer reg req = 0
  left:_serialBitDelay [k8 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{void function    }
        Symbol type: void function    
  result:iTemp3 [k9 lr6:6 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{void}
          Symbol type: void
  3594 - pointer reg req = 0
packRegisters
packRegsForAssign
  result:iTemp10 [k17 lr8:21 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _putc_bitnum_1_1}
  left:
  right:0x8 {literal unsigned char}
  2705 - not packing - right is not temp
allocDirReg BAD, op is NULL
  result:iTemp10 [k17 lr8:21 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _putc_bitnum_1_1}
  left:
  right:0x8 {literal unsigned char}
  result:iTemp10 [k17 lr8:21 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _putc_bitnum_1_1}
          Symbol type: unsigned char
  3594 - pointer reg req = 0
packRegisters
  left:iTemp0 [k2 lr3:21 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _putc_byte_1_1}
        Symbol type: unsigned char
  result:iTemp5 [k12 lr10:11 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{unsigned char}
          Symbol type: unsigned char
  3594 - pointer reg req = 0
isBitwiseOptimizable
  3611
  left:iTemp5 [k12 lr10:11 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{unsigned char}
        Symbol type: unsigned char
  3594 - pointer reg req = 0
packRegisters
packRegsForAssign
  result:_SerialPort [k4 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{bit bit {0,1}}
  left:
  right:0x1 {literal bit {0,1}}
allocDirReg symbol name _SerialPort
  637  storage class 13 
 642  integral
 646  specifier
_SerialPort [k4 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{bit bit {0,1}}
  -- and it is at a fixed address 0x4a
  2695 - result is not temp
  2705 - not packing - right is not temp
allocDirReg BAD, op is NULL
  result:_SerialPort [k4 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{bit bit {0,1}}
  left:
  right:0x1 {literal bit {0,1}}
  result:_SerialPort [k4 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{bit bit {0,1}}
          Symbol type: bit bit {0,1}
  3594 - pointer reg req = 0
packRegisters
packRegsForAssign
  result:_SerialPort [k4 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{bit bit {0,1}}
  left:
  right:0x0 {literal bit {0,1}}
allocDirReg symbol name _SerialPort
  637  storage class 13 
 642  integral
 646  specifier
_SerialPort [k4 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{bit bit {0,1}}
  -- and it is at a fixed address 0x4a
  2695 - result is not temp
  2705 - not packing - right is not temp
allocDirReg BAD, op is NULL
  result:_SerialPort [k4 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{bit bit {0,1}}
  left:
  right:0x0 {literal bit {0,1}}
  result:_SerialPort [k4 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{bit bit {0,1}}
          Symbol type: bit bit {0,1}
  3594 - pointer reg req = 0
packRegisters
packRegsForAssign
  result:iTemp0 [k2 lr3:21 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _putc_byte_1_1}
  left:
  right:iTemp8 [k15 lr17:18 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{unsigned char}
  2760 - dic result key == ic right key -- pointer set=N
  packing. removing iTemp8
  replacing with iTemp8
  result:iTemp0 [k2 lr3:21 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _putc_byte_1_1}
  left:
  right:iTemp8 [k15 lr17:18 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{unsigned char}
  left:iTemp0 [k2 lr3:21 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _putc_byte_1_1}
        Symbol type: unsigned char
  result:iTemp0 [k2 lr3:21 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _putc_byte_1_1}
          Symbol type: unsigned char
  3594 - pointer reg req = 0
packRegsForAccUse
  packRegsForAccUse:3181
  packRegsForAccUse:3188
  left:_serialBitDelay [k8 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{void function    }
        Symbol type: void function    
  result:iTemp9 [k16 lr19:19 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{void}
          Symbol type: void
  3594 - pointer reg req = 0
  left:iTemp10 [k17 lr8:21 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _putc_bitnum_1_1}
        Symbol type: unsigned char
  result:iTemp10 [k17 lr8:21 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _putc_bitnum_1_1}
          Symbol type: unsigned char
  3594 - pointer reg req = 0
packRegsForSupport
packRegsForAccUse
  packRegsForAccUse:3181
  packRegsForAccUse:3188
  left:iTemp10 [k17 lr8:21 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _putc_bitnum_1_1}
        Symbol type: unsigned char
  3594 - pointer reg req = 0
packRegisters
packRegsForAssign
  result:_SerialPort [k4 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{bit bit {0,1}}
  left:
  right:0x1 {literal bit {0,1}}
allocDirReg symbol name _SerialPort
  637  storage class 13 
 642  integral
 646  specifier
_SerialPort [k4 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{bit bit {0,1}}
  -- and it is at a fixed address 0x4a
  2695 - result is not temp
  2705 - not packing - right is not temp
allocDirReg BAD, op is NULL
  result:_SerialPort [k4 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{bit bit {0,1}}
  left:
  right:0x1 {literal bit {0,1}}
packRegsForAssign
  result:_SerialTris [k6 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{bit bit {0,1}}
  left:
  right:0x1 {literal bit {0,1}}
allocDirReg symbol name _SerialTris
  637  storage class 13 
 642  integral
 646  specifier
_SerialTris [k6 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{bit bit {0,1}}
  -- and it is at a fixed address 0x44a
  2695 - result is not temp
  2705 - not packing - right is not temp
allocDirReg BAD, op is NULL
  result:_SerialTris [k6 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{bit bit {0,1}}
  left:
  right:0x1 {literal bit {0,1}}
  result:_SerialPort [k4 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{bit bit {0,1}}
          Symbol type: bit bit {0,1}
  3594 - pointer reg req = 0
  left:_serialBitDelay [k8 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{void function    }
        Symbol type: void function    
  result:iTemp12 [k19 lr23:23 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{void}
          Symbol type: void
  3594 - pointer reg req = 0
  result:_SerialTris [k6 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{bit bit {0,1}}
          Symbol type: bit bit {0,1}
  3594 - pointer reg req = 0
packRegisters
  left:_putc [k1 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{void function    }
        Symbol type: void function    
dir registers allocated so far:
  -- #0 reg = s0xFFF  key 0, rIdx = 4095, size 1
  -- #1 reg = s0x1000  key 0, rIdx = 4096, size 1
  -- #2 reg = s0x1001  key 0, rIdx = 4097, size 1
  -- #3 reg = s0x1002  key 0, rIdx = 4098, size 1
  -- #4 reg = s0x1003  key 0, rIdx = 4099, size 1
  -- #5 reg = s0x1004  key 0, rIdx = 4100, size 1
  -- #6 reg = s0x1005  key 0, rIdx = 4101, size 1
  -- #7 reg = s0x1006  key 0, rIdx = 4102, size 1
  -- #8 reg = r0x20  key 0, rIdx = 32, size 1
  -- #9 reg = r0x21  key 0, rIdx = 33, size 1
  -- #10 reg = r0x22  key 0, rIdx = 34, size 1
  -- #11 reg = r0x23  key 0, rIdx = 35, size 1
  -- #12 reg = r0x24  key 0, rIdx = 36, size 1
  -- #13 reg = r0x25  key 0, rIdx = 37, size 1
  -- #14 reg = r0x26  key 0, rIdx = 38, size 1
  -- #15 reg = r0x27  key 0, rIdx = 39, size 1
  -- #16 reg = r0x28  key 0, rIdx = 40, size 1
  -- #17 reg = r0x29  key 0, rIdx = 41, size 1
  -- #18 reg = r0x2A  key 0, rIdx = 42, size 1
  -- #19 reg = r0x2B  key 0, rIdx = 43, size 1
  -- #20 reg = r0x2C  key 0, rIdx = 44, size 1
  -- #21 reg = r0x2D  key 0, rIdx = 45, size 1
  -- #22 reg = r0x2E  key 0, rIdx = 46, size 1
  -- #23 reg = r0x2F  key 0, rIdx = 47, size 1
  -- #24 reg = r0x30  key 0, rIdx = 48, size 1
  -- #25 reg = r0x31  key 0, rIdx = 49, size 1
  -- #26 reg = r0x32  key 0, rIdx = 50, size 1
  -- #27 reg = r0x33  key 0, rIdx = 51, size 1
  -- #28 reg = r0x34  key 0, rIdx = 52, size 1
  -- #29 reg = r0x35  key 0, rIdx = 53, size 1
  -- #30 reg = r0x36  key 0, rIdx = 54, size 1
  -- #31 reg = r0x37  key 0, rIdx = 55, size 1
  -- #32 reg = r0x38  key 0, rIdx = 56, size 1
  -- #33 reg = r0x39  key 0, rIdx = 57, size 1
  -- #34 reg = r0x3A  key 0, rIdx = 58, size 1
  -- #35 reg = r0x3B  key 0, rIdx = 59, size 1
  -- #36 reg = r0x3C  key 0, rIdx = 60, size 1
  -- #37 reg = r0x3D  key 0, rIdx = 61, size 1
  -- #38 reg = r0x3E  key 0, rIdx = 62, size 1
  -- #39 reg = r0x3F  key 0, rIdx = 63, size 1
  -- #40 reg = r0x40  key 0, rIdx = 64, size 1
  -- #41 reg = r0x41  key 0, rIdx = 65, size 1
  -- #42 reg = r0x42  key 0, rIdx = 66, size 1
  -- #43 reg = r0x43  key 0, rIdx = 67, size 1
  -- #44 reg = r0x44  key 0, rIdx = 68, size 1
  -- #45 reg = r0x45  key 0, rIdx = 69, size 1
  -- #46 reg = r0x46  key 0, rIdx = 70, size 1
  -- #47 reg = r0x47  key 0, rIdx = 71, size 1
  -- #48 reg = r0x48  key 0, rIdx = 72, size 1
  -- #49 reg = r0x49  key 0, rIdx = 73, size 1
  -- #50 reg = r0x4A  key 0, rIdx = 74, size 1
  -- #51 reg = r0x4B  key 0, rIdx = 75, size 1
  -- #52 reg = r0x4C  key 0, rIdx = 76, size 1
  -- #53 reg = r0x4D  key 0, rIdx = 77, size 1
  -- #54 reg = r0x4E  key 0, rIdx = 78, size 1
  -- #55 reg = r0x4F  key 0, rIdx = 79, size 1
  -- #56 reg = r0x50  key 0, rIdx = 80, size 1
  -- #57 reg = r0x51  key 0, rIdx = 81, size 1
  -- #58 reg = r0x52  key 0, rIdx = 82, size 1
  -- #59 reg = r0x53  key 0, rIdx = 83, size 1
  -- #60 reg = r0x54  key 0, rIdx = 84, size 1
  -- #61 reg = r0x55  key 0, rIdx = 85, size 1
  -- #62 reg = r0x56  key 0, rIdx = 86, size 1
  -- #63 reg = r0x57  key 0, rIdx = 87, size 1
  -- #64 reg = r0x58  key 0, rIdx = 88, size 1
  -- #65 reg = r0x59  key 0, rIdx = 89, size 1
  -- #66 reg = r0x5A  key 0, rIdx = 90, size 1
  -- #67 reg = r0x5B  key 0, rIdx = 91, size 1
  -- #68 reg = r0x5C  key 0, rIdx = 92, size 1
  -- #69 reg = r0x5D  key 0, rIdx = 93, size 1
  -- #70 reg = r0x5E  key 0, rIdx = 94, size 1
  -- #71 reg = r0x5F  key 0, rIdx = 95, size 1
  -- #72 reg = r0x60  key 0, rIdx = 96, size 1
  -- #73 reg = r0x61  key 0, rIdx = 97, size 1
  -- #74 reg = r0x62  key 0, rIdx = 98, size 1
  -- #75 reg = _ADRESH  key 0, rIdx = 33, size 1
  -- #76 reg = r0x63  key 0, rIdx = 99, size 1
  -- #77 reg = r0x64  key 0, rIdx = 100, size 1
  -- #78 reg = r0x65  key 0, rIdx = 101, size 1
  -- #79 reg = r0x66  key 0, rIdx = 102, size 1
  -- #80 reg = r0x67  key 0, rIdx = 103, size 1
  -- #81 reg = r0x68  key 0, rIdx = 104, size 1
  -- #82 reg = r0x69  key 0, rIdx = 105, size 1
  -- #83 reg = r0x6A  key 0, rIdx = 106, size 1
  -- #84 reg = r0x6B  key 0, rIdx = 107, size 1
  -- #85 reg = r0x6C  key 0, rIdx = 108, size 1
  -- #86 reg = _ADRESL  key 5, rIdx = 34, size 1
  -- #87 reg = _PCLATH  key 6, rIdx = 10, size 1
  -- #88 reg = _TRISA  key 7, rIdx = 1, size 1
  -- #89 reg = _TRISB  key 8, rIdx = 2, size 1
  -- #90 reg = _TRISB  key 8, rIdx = 23, size 1
  -- #91 reg = _TRISB  key 8, rIdx = 32, size 1
  -- #92 reg = _TRISC  key 9, rIdx = 3, size 1
  -- #93 reg = _TRISD  key 10, rIdx = 4, size 1
  -- #94 reg = _PORTA  key 10, rIdx = 6, size 1
  -- #95 reg = _TRISE  key 11, rIdx = 5, size 1
  -- #96 reg = _PORTB  key 11, rIdx = 7, size 1
  -- #97 reg = _PORTB  key 11, rIdx = 22, size 1
  -- #98 reg = _PORTB  key 11, rIdx = 30, size 1
  -- #99 reg = _PORTC  key 12, rIdx = 8, size 1
  -- #100 reg = _GIE  key 12, rIdx = 13, size 1
  -- #101 reg = _PORTD  key 13, rIdx = 9, size 1
  -- #102 reg = _PORTE  key 14, rIdx = 10, size 1
  -- #103 reg = _sensorSample_phase0_1_1  key 15, rIdx = 26, size 1
  -- #104 reg = _sensorSample_phase1_1_1  key 16, rIdx = 27, size 1
  -- #105 reg = _sensorSample_phase2_1_1  key 17, rIdx = 28, size 1
  -- #106 reg = _sensorSample_phase3_1_1  key 18, rIdx = 29, size 1
  -- #107 reg = _ADCON0  key 27, rIdx = 12, size 1
  -- #108 reg = _ADCON0  key 27, rIdx = 24, size 1
  -- #109 reg = _ADCON0  key 27, rIdx = 25, size 1
  -- #110 reg = _ADCON1  key 28, rIdx = 11, size 1
  -- #111 reg = SSAVE  key 32, rIdx = 32765, size 1
  -- #112 reg = WSAVE  key 36, rIdx = 32766, size 1
  -- #113 reg = _reading0  key 44, rIdx = 14, size 2
  -- #114 reg = _GO_DONE  key 44, rIdx = 31, size 1
  -- #115 reg = _reading1  key 45, rIdx = 15, size 2
  -- #116 reg = _reading2  key 46, rIdx = 16, size 2
  -- #117 reg = INTCON  key 47, rIdx = 11, size 1
  -- #118 reg = _reading3  key 47, rIdx = 17, size 2
  -- #119 reg = _STATUS  key 48, rIdx = 3, size 1
  -- #120 reg = PCL  key 48, rIdx = 2, size 1
  -- #121 reg = _reading4  key 48, rIdx = 18, size 2
  -- #122 reg = KZ  key 49, rIdx = 32767, size 1
  -- #123 reg = _reading5  key 49, rIdx = 19, size 2
  -- #124 reg = _reading6  key 50, rIdx = 20, size 2
  -- #125 reg = _reading7  key 51, rIdx = 21, size 2
  -- #126 reg = _SerialTris  key 54, rIdx = 36, size 1
  -- #127 reg = INDF  key 56, rIdx = 0, size 1
  -- #128 reg = _Light0  key 56, rIdx = 0, size 1
  -- #129 reg = _SerialPort  key 57, rIdx = 35, size 1
  -- #130 reg = FSR  key 60, rIdx = 4, size 1
regTypeNum
  2421 - iTemp0
  2431 - itemp register
  reg name iTemp0,  reg type REG_GPR
  2421 - iTemp3
  2421 - iTemp5
  2431 - itemp register
  2421 - iTemp8
  2431 - itemp register
  reg name iTemp8,  reg type REG_GPR
  2421 - iTemp9
  2421 - iTemp10
  2431 - itemp register
  reg name iTemp10,  reg type REG_GPR
  2421 - iTemp12
serialRegAssign
  op: LABEL
deassignLRs
  op: unkown op 9 	
deassignLRs
  op: RECEIVE
deassignLRs
willCauseSpill
computeSpillable
When I get clever, I'll optimize the receive logic
  2125 - 
bitvector Size = 58 bSize = 8
Bits on { (2) }
getRegGpr
allocReg of type REG_GPR
  2141 - 
  2155 - 
  op: =
deassignLRs
  op: =
deassignLRs
  op: CALL
deassignLRs
  op: LABEL
deassignLRs
  op: =
deassignLRs
willCauseSpill
computeSpillable
  2125 - 
bitvector Size = 58 bSize = 8
Bits on { (2) (17) }
getRegGpr
allocReg of type REG_GPR
  2141 - 
  2155 - 
  op: LABEL
deassignLRs
  op: BITWISEAND
deassignLRs
  op: IFX
deassignLRs
  op: =
deassignLRs
  op: GOTO
deassignLRs
  op: LABEL
deassignLRs
  op: =
deassignLRs
  op: LABEL
deassignLRs
  op: RIGHT_OP
deassignLRs
  op: CALL
deassignLRs
  op: -
deassignLRs
  op: IFX
deassignLRs
freeReg
freeReg
  op: =
deassignLRs
  op: CALL
deassignLRs
  op: =
deassignLRs
  op: LABEL
deassignLRs
  op: ENDFUNCTION
deassignLRs
createRegMask
regsUsedIniCode
rUmaskForOp
regsUsedIniCode
rUmaskForOp
rUmaskForOp
regsUsedIniCode
rUmaskForOp
rUmaskForOp
regsUsedIniCode
rUmaskForOp
rUmaskForOp
regsUsedIniCode
rUmaskForOp
rUmaskForOp
regsUsedIniCode
rUmaskForOp
rUmaskForOp
rUmaskForOp
regsUsedIniCode
rUmaskForOp
regsUsedIniCode
rUmaskForOp
rUmaskForOp
regsUsedIniCode
rUmaskForOp
rUmaskForOp
regsUsedIniCode
rUmaskForOp
rUmaskForOp
rUmaskForOp
regsUsedIniCode
rUmaskForOp
rUmaskForOp
regsUsedIniCode
rUmaskForOp
rUmaskForOp
rUmaskForOp
regsUsedIniCode
rUmaskForOp
regsUsedIniCode
rUmaskForOp
rUmaskForOp
regsUsedIniCode
rUmaskForOp
rUmaskForOp
regsUsedIniCode
rUmaskForOp
rUmaskForOp
ebbs after optimizing:

----------------------------------------------------------------
Basic Block _entry : loop Depth = 0 noPath = 0 , lastinLoop = 0
depth 1st num 1 : bbnum = 0 1st iCode = 1 , last iCode = 6
visited 0 : hasFcall = 1

defines bitVector :bitvector Size = 39 bSize = 5
Bits on { (2) (4) (6) }

local defines bitVector :bitvector Size = 39 bSize = 5
Bits on { (2) (4) (6) (7) (8) }

pointers Set bitvector :
in pointers Set bitvector :
inDefs Set bitvector :
outDefs Set bitvector :bitvector Size = 39 bSize = 5
Bits on { (2) (4) (6) (7) }

usesDefs Set bitvector :bitvector Size = 39 bSize = 5
Bits on { (2) (7) }

----------------------------------------------------------------
main.c(241:1:0:0:0)	 _entry($8) :
main.c(241:2:1:0:0)		proc _putc [k1 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{void function    }
main.c(241:3:2:0:0)	iTemp0 [k2 lr3:21 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _putc_byte_1_1}[r0x6D ] = recv 
main.c(245:4:4:0:0)		_SerialPort [k4 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{bit bit {0,1}} := 0x0 {literal bit {0,1}}
main.c(246:5:6:0:0)		_SerialTris [k6 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{bit bit {0,1}} := 0x0 {literal bit {0,1}}
main.c(247:6:7:0:0)		iTemp3 [k9 lr6:6 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{void} = call _serialBitDelay [k8 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{void function    }
main.c(251:8:38:0:0)		iTemp10 [k17 lr8:21 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _putc_bitnum_1_1}[r0x6E ] := 0x8 {literal unsigned char}
main.c(251:9:9:1:0)	 _dobody_3($4) :
main.c(252:10:10:1:0)		iTemp5 [k12 lr10:11 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{unsigned char} = iTemp0 [k2 lr3:21 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _putc_byte_1_1}[r0x6D ] & 0x1 {literal unsigned char}
main.c(252:11:11:1:0)		if iTemp5 [k12 lr10:11 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{unsigned char} == 0 goto _iffalse_10($2)
main.c(253:12:15:1:0)		_SerialPort [k4 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{bit bit {0,1}} := 0x1 {literal bit {0,1}}
main.c(253:13:16:1:0)		 goto _ifend_10($3)
main.c(253:14:17:1:0)	 _iffalse_10($2) :
main.c(255:15:19:1:0)		_SerialPort [k4 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{bit bit {0,1}} := 0x0 {literal bit {0,1}}
main.c(255:16:20:1:0)	 _ifend_10($3) :
main.c(256:17:21:1:0)		iTemp0 [k2 lr3:21 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _putc_byte_1_1}[r0x6D ] = iTemp0 [k2 lr3:21 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _putc_byte_1_1}[r0x6D ] >> 0x1 {literal unsigned char}
main.c(257:19:23:1:0)		iTemp9 [k16 lr19:19 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{void} = call _serialBitDelay [k8 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{void function    }
main.c(258:20:25:1:0)		iTemp10 [k17 lr8:21 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _putc_bitnum_1_1}[r0x6E ] = iTemp10 [k17 lr8:21 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _putc_bitnum_1_1}[r0x6E ] - 0x1 {literal unsigned char}
main.c(258:21:27:1:0)		if iTemp10 [k17 lr8:21 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _putc_bitnum_1_1}[r0x6E ] != 0 goto _dobody_3($4)
main.c(261:22:31:0:0)		_SerialPort [k4 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{bit bit {0,1}} := 0x1 {literal bit {0,1}}
main.c(262:23:32:0:0)		iTemp12 [k19 lr23:23 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{void} = call _serialBitDelay [k8 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{void function    }
main.c(263:24:34:0:0)		_SerialTris [k6 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{bit bit {0,1}} := 0x1 {literal bit {0,1}}
main.c(263:25:35:0:0)	 _return($7) :
main.c(263:26:36:0:0)		eproc _putc [k1 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{void function    }

----------------------------------------------------------------
Basic Block preHeaderLbl0 : loop Depth = 0 noPath = 0 , lastinLoop = 0
depth 1st num 2 : bbnum = 1 1st iCode = 7 , last iCode = 8
visited 0 : hasFcall = 0

defines bitVector :bitvector Size = 39 bSize = 5
Bits on { (38) }

local defines bitVector :bitvector Size = 39 bSize = 5
Bits on { (38) }

pointers Set bitvector :
in pointers Set bitvector :
inDefs Set bitvector :bitvector Size = 39 bSize = 5
Bits on { (2) (4) (6) (7) (8) }

outDefs Set bitvector :bitvector Size = 39 bSize = 5
Bits on { (2) (4) (6) (7) (8) (38) }

usesDefs Set bitvector :bitvector Size = 39 bSize = 5
Bits on { }

----------------------------------------------------------------
main.c(251:7:37:0:0)	 preHeaderLbl0($10) :
main.c(251:8:38:0:0)		iTemp10 [k17 lr8:21 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _putc_bitnum_1_1}[r0x6E ] := 0x8 {literal unsigned char}
main.c(251:9:9:1:0)	 _dobody_3($4) :
main.c(252:10:10:1:0)		iTemp5 [k12 lr10:11 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{unsigned char} = iTemp0 [k2 lr3:21 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _putc_byte_1_1}[r0x6D ] & 0x1 {literal unsigned char}
main.c(252:11:11:1:0)		if iTemp5 [k12 lr10:11 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{unsigned char} == 0 goto _iffalse_10($2)
main.c(253:12:15:1:0)		_SerialPort [k4 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{bit bit {0,1}} := 0x1 {literal bit {0,1}}
main.c(253:13:16:1:0)		 goto _ifend_10($3)
main.c(253:14:17:1:0)	 _iffalse_10($2) :
main.c(255:15:19:1:0)		_SerialPort [k4 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{bit bit {0,1}} := 0x0 {literal bit {0,1}}
main.c(255:16:20:1:0)	 _ifend_10($3) :
main.c(256:17:21:1:0)		iTemp0 [k2 lr3:21 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _putc_byte_1_1}[r0x6D ] = iTemp0 [k2 lr3:21 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _putc_byte_1_1}[r0x6D ] >> 0x1 {literal unsigned char}
main.c(257:19:23:1:0)		iTemp9 [k16 lr19:19 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{void} = call _serialBitDelay [k8 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{void function    }
main.c(258:20:25:1:0)		iTemp10 [k17 lr8:21 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _putc_bitnum_1_1}[r0x6E ] = iTemp10 [k17 lr8:21 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _putc_bitnum_1_1}[r0x6E ] - 0x1 {literal unsigned char}
main.c(258:21:27:1:0)		if iTemp10 [k17 lr8:21 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _putc_bitnum_1_1}[r0x6E ] != 0 goto _dobody_3($4)
main.c(261:22:31:0:0)		_SerialPort [k4 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{bit bit {0,1}} := 0x1 {literal bit {0,1}}
main.c(262:23:32:0:0)		iTemp12 [k19 lr23:23 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{void} = call _serialBitDelay [k8 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{void function    }
main.c(263:24:34:0:0)		_SerialTris [k6 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{bit bit {0,1}} := 0x1 {literal bit {0,1}}
main.c(263:25:35:0:0)	 _return($7) :
main.c(263:26:36:0:0)		eproc _putc [k1 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{void function    }

----------------------------------------------------------------
Basic Block _dobody_3 : loop Depth = 1 noPath = 0 , lastinLoop = 0
depth 1st num 3 : bbnum = 2 1st iCode = 9 , last iCode = 11
visited 0 : hasFcall = 0

defines bitVector :bitvector Size = 39 bSize = 5
Bits on { (10) }

local defines bitVector :bitvector Size = 39 bSize = 5
Bits on { (10) }

pointers Set bitvector :
in pointers Set bitvector :
inDefs Set bitvector :bitvector Size = 39 bSize = 5
Bits on { (2) (3) (4) (5) (6) (7) (8) (10) (14) (15) (18) (19) (21) (22) (23) (25) (26) (38) }

outDefs Set bitvector :bitvector Size = 39 bSize = 5
Bits on { (2) (3) (4) (5) (6) (7) (8) (10) (14) (15) (18) (19) (21) (22) (23) (25) (26) (38) }

usesDefs Set bitvector :bitvector Size = 39 bSize = 5
Bits on { (2) (10) (22) }

----------------------------------------------------------------
main.c(251:9:9:1:0)	 _dobody_3($4) :
main.c(252:10:10:1:0)		iTemp5 [k12 lr10:11 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{unsigned char} = iTemp0 [k2 lr3:21 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _putc_byte_1_1}[r0x6D ] & 0x1 {literal unsigned char}
main.c(252:11:11:1:0)		if iTemp5 [k12 lr10:11 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{unsigned char} == 0 goto _iffalse_10($2)
main.c(253:12:15:1:0)		_SerialPort [k4 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{bit bit {0,1}} := 0x1 {literal bit {0,1}}
main.c(253:13:16:1:0)		 goto _ifend_10($3)
main.c(253:14:17:1:0)	 _iffalse_10($2) :
main.c(255:15:19:1:0)		_SerialPort [k4 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{bit bit {0,1}} := 0x0 {literal bit {0,1}}
main.c(255:16:20:1:0)	 _ifend_10($3) :
main.c(256:17:21:1:0)		iTemp0 [k2 lr3:21 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _putc_byte_1_1}[r0x6D ] = iTemp0 [k2 lr3:21 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _putc_byte_1_1}[r0x6D ] >> 0x1 {literal unsigned char}
main.c(257:19:23:1:0)		iTemp9 [k16 lr19:19 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{void} = call _serialBitDelay [k8 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{void function    }
main.c(258:20:25:1:0)		iTemp10 [k17 lr8:21 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _putc_bitnum_1_1}[r0x6E ] = iTemp10 [k17 lr8:21 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _putc_bitnum_1_1}[r0x6E ] - 0x1 {literal unsigned char}
main.c(258:21:27:1:0)		if iTemp10 [k17 lr8:21 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _putc_bitnum_1_1}[r0x6E ] != 0 goto _dobody_3($4)
main.c(261:22:31:0:0)		_SerialPort [k4 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{bit bit {0,1}} := 0x1 {literal bit {0,1}}
main.c(262:23:32:0:0)		iTemp12 [k19 lr23:23 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{void} = call _serialBitDelay [k8 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{void function    }
main.c(263:24:34:0:0)		_SerialTris [k6 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{bit bit {0,1}} := 0x1 {literal bit {0,1}}
main.c(263:25:35:0:0)	 _return($7) :
main.c(263:26:36:0:0)		eproc _putc [k1 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{void function    }

----------------------------------------------------------------
Basic Block _eBBlock0 : loop Depth = 1 noPath = 0 , lastinLoop = 0
depth 1st num 4 : bbnum = 3 1st iCode = 12 , last iCode = 13
visited 0 : hasFcall = 0

defines bitVector :bitvector Size = 39 bSize = 5
Bits on { (15) }

local defines bitVector :bitvector Size = 39 bSize = 5
Bits on { (15) }

pointers Set bitvector :
in pointers Set bitvector :
inDefs Set bitvector :bitvector Size = 39 bSize = 5
Bits on { (2) (3) (4) (5) (6) (7) (8) (10) (14) (15) (18) (19) (21) (22) (23) (25) (26) (38) }

outDefs Set bitvector :bitvector Size = 39 bSize = 5
Bits on { (2) (3) (5) (6) (7) (8) (10) (14) (15) (18) (21) (22) (23) (25) (26) (38) }

usesDefs Set bitvector :bitvector Size = 39 bSize = 5
Bits on { }

----------------------------------------------------------------
main.c(253:12:15:1:0)		_SerialPort [k4 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{bit bit {0,1}} := 0x1 {literal bit {0,1}}
main.c(253:13:16:1:0)		 goto _ifend_10($3)
main.c(253:14:17:1:0)	 _iffalse_10($2) :
main.c(255:15:19:1:0)		_SerialPort [k4 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{bit bit {0,1}} := 0x0 {literal bit {0,1}}
main.c(255:16:20:1:0)	 _ifend_10($3) :
main.c(256:17:21:1:0)		iTemp0 [k2 lr3:21 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _putc_byte_1_1}[r0x6D ] = iTemp0 [k2 lr3:21 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _putc_byte_1_1}[r0x6D ] >> 0x1 {literal unsigned char}
main.c(257:19:23:1:0)		iTemp9 [k16 lr19:19 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{void} = call _serialBitDelay [k8 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{void function    }
main.c(258:20:25:1:0)		iTemp10 [k17 lr8:21 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _putc_bitnum_1_1}[r0x6E ] = iTemp10 [k17 lr8:21 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _putc_bitnum_1_1}[r0x6E ] - 0x1 {literal unsigned char}
main.c(258:21:27:1:0)		if iTemp10 [k17 lr8:21 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _putc_bitnum_1_1}[r0x6E ] != 0 goto _dobody_3($4)
main.c(261:22:31:0:0)		_SerialPort [k4 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{bit bit {0,1}} := 0x1 {literal bit {0,1}}
main.c(262:23:32:0:0)		iTemp12 [k19 lr23:23 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{void} = call _serialBitDelay [k8 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{void function    }
main.c(263:24:34:0:0)		_SerialTris [k6 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{bit bit {0,1}} := 0x1 {literal bit {0,1}}
main.c(263:25:35:0:0)	 _return($7) :
main.c(263:26:36:0:0)		eproc _putc [k1 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{void function    }

----------------------------------------------------------------
Basic Block _iffalse_10 : loop Depth = 1 noPath = 0 , lastinLoop = 0
depth 1st num 5 : bbnum = 4 1st iCode = 14 , last iCode = 15
visited 0 : hasFcall = 0

defines bitVector :bitvector Size = 39 bSize = 5
Bits on { (19) }

local defines bitVector :bitvector Size = 39 bSize = 5
Bits on { (19) }

pointers Set bitvector :
in pointers Set bitvector :
inDefs Set bitvector :bitvector Size = 39 bSize = 5
Bits on { (2) (3) (4) (5) (6) (7) (8) (10) (14) (15) (18) (19) (21) (22) (23) (25) (26) (38) }

outDefs Set bitvector :bitvector Size = 39 bSize = 5
Bits on { (2) (3) (5) (6) (7) (8) (10) (14) (18) (19) (21) (22) (23) (25) (26) (38) }

usesDefs Set bitvector :bitvector Size = 39 bSize = 5
Bits on { }

----------------------------------------------------------------
main.c(253:14:17:1:0)	 _iffalse_10($2) :
main.c(255:15:19:1:0)		_SerialPort [k4 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{bit bit {0,1}} := 0x0 {literal bit {0,1}}
main.c(255:16:20:1:0)	 _ifend_10($3) :
main.c(256:17:21:1:0)		iTemp0 [k2 lr3:21 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _putc_byte_1_1}[r0x6D ] = iTemp0 [k2 lr3:21 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _putc_byte_1_1}[r0x6D ] >> 0x1 {literal unsigned char}
main.c(257:19:23:1:0)		iTemp9 [k16 lr19:19 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{void} = call _serialBitDelay [k8 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{void function    }
main.c(258:20:25:1:0)		iTemp10 [k17 lr8:21 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _putc_bitnum_1_1}[r0x6E ] = iTemp10 [k17 lr8:21 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _putc_bitnum_1_1}[r0x6E ] - 0x1 {literal unsigned char}
main.c(258:21:27:1:0)		if iTemp10 [k17 lr8:21 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _putc_bitnum_1_1}[r0x6E ] != 0 goto _dobody_3($4)
main.c(261:22:31:0:0)		_SerialPort [k4 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{bit bit {0,1}} := 0x1 {literal bit {0,1}}
main.c(262:23:32:0:0)		iTemp12 [k19 lr23:23 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{void} = call _serialBitDelay [k8 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{void function    }
main.c(263:24:34:0:0)		_SerialTris [k6 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{bit bit {0,1}} := 0x1 {literal bit {0,1}}
main.c(263:25:35:0:0)	 _return($7) :
main.c(263:26:36:0:0)		eproc _putc [k1 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{void function    }

----------------------------------------------------------------
Basic Block _ifend_10 : loop Depth = 1 noPath = 0 , lastinLoop = 1
depth 1st num 6 : bbnum = 5 1st iCode = 16 , last iCode = 21
visited 0 : hasFcall = 1

defines bitVector :bitvector Size = 39 bSize = 5
Bits on { (21) (22) (25) }

local defines bitVector :bitvector Size = 39 bSize = 5
Bits on { (21) (22) (23) (25) (26) }

pointers Set bitvector :
in pointers Set bitvector :
inDefs Set bitvector :bitvector Size = 39 bSize = 5
Bits on { (2) (3) (5) (6) (7) (8) (10) (14) (15) (18) (19) (21) (22) (23) (25) (26) (38) }

outDefs Set bitvector :bitvector Size = 39 bSize = 5
Bits on { (3) (5) (6) (7) (10) (14) (15) (18) (19) (21) (22) (23) (25) }

usesDefs Set bitvector :bitvector Size = 39 bSize = 5
Bits on { (2) (21) (22) (23) (25) (38) }

----------------------------------------------------------------
main.c(255:16:20:1:0)	 _ifend_10($3) :
main.c(256:17:21:1:0)		iTemp0 [k2 lr3:21 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _putc_byte_1_1}[r0x6D ] = iTemp0 [k2 lr3:21 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _putc_byte_1_1}[r0x6D ] >> 0x1 {literal unsigned char}
main.c(257:19:23:1:0)		iTemp9 [k16 lr19:19 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{void} = call _serialBitDelay [k8 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{void function    }
main.c(258:20:25:1:0)		iTemp10 [k17 lr8:21 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _putc_bitnum_1_1}[r0x6E ] = iTemp10 [k17 lr8:21 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _putc_bitnum_1_1}[r0x6E ] - 0x1 {literal unsigned char}
main.c(258:21:27:1:0)		if iTemp10 [k17 lr8:21 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _putc_bitnum_1_1}[r0x6E ] != 0 goto _dobody_3($4)
main.c(261:22:31:0:0)		_SerialPort [k4 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{bit bit {0,1}} := 0x1 {literal bit {0,1}}
main.c(262:23:32:0:0)		iTemp12 [k19 lr23:23 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{void} = call _serialBitDelay [k8 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{void function    }
main.c(263:24:34:0:0)		_SerialTris [k6 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{bit bit {0,1}} := 0x1 {literal bit {0,1}}
main.c(263:25:35:0:0)	 _return($7) :
main.c(263:26:36:0:0)		eproc _putc [k1 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{void function    }

----------------------------------------------------------------
Basic Block _eBBlock1 : loop Depth = 0 noPath = 0 , lastinLoop = 0
depth 1st num 7 : bbnum = 6 1st iCode = 22 , last iCode = 24
visited 0 : hasFcall = 1

defines bitVector :bitvector Size = 39 bSize = 5
Bits on { (31) (34) }

local defines bitVector :bitvector Size = 39 bSize = 5
Bits on { (31) (32) (34) }

pointers Set bitvector :
in pointers Set bitvector :
inDefs Set bitvector :bitvector Size = 39 bSize = 5
Bits on { (3) (5) (6) (7) (10) (14) (15) (18) (19) (21) (22) (23) (25) (26) }

outDefs Set bitvector :bitvector Size = 39 bSize = 5
Bits on { (3) (5) (7) (10) (14) (18) (21) (22) (23) (25) (26) (31) (32) (34) }

usesDefs Set bitvector :bitvector Size = 39 bSize = 5
Bits on { (32) }

----------------------------------------------------------------
main.c(261:22:31:0:0)		_SerialPort [k4 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{bit bit {0,1}} := 0x1 {literal bit {0,1}}
main.c(262:23:32:0:0)		iTemp12 [k19 lr23:23 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{void} = call _serialBitDelay [k8 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{void function    }
main.c(263:24:34:0:0)		_SerialTris [k6 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{bit bit {0,1}} := 0x1 {literal bit {0,1}}
main.c(263:25:35:0:0)	 _return($7) :
main.c(263:26:36:0:0)		eproc _putc [k1 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{void function    }

----------------------------------------------------------------
Basic Block _return : loop Depth = 0 noPath = 0 , lastinLoop = 0
depth 1st num 8 : bbnum = 7 1st iCode = 25 , last iCode = 26
visited 0 : hasFcall = 0

defines bitVector :bitvector Size = 39 bSize = 5
Bits on { }

local defines bitVector :
pointers Set bitvector :
in pointers Set bitvector :
inDefs Set bitvector :bitvector Size = 39 bSize = 5
Bits on { (3) (5) (7) (10) (14) (18) (21) (22) (23) (25) (26) (31) (32) (34) }

outDefs Set bitvector :bitvector Size = 39 bSize = 5
Bits on { (3) (5) (7) (10) (14) (18) (21) (22) (23) (25) (26) (31) (32) (34) }

usesDefs Set bitvector :bitvector Size = 39 bSize = 5
Bits on { }

----------------------------------------------------------------
main.c(263:25:35:0:0)	 _return($7) :
main.c(263:26:36:0:0)		eproc _putc [k1 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{void function    }
typeRegWithIdx - requesting index = 0x6d
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6e
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6d
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6e
Found a Dynamic Register!
leaving
<><><><><><><><><><><><><><><><><>
<><><><><><><><><><><><><><><><><>
starting	ralloc.c:pic14_assignRegisters
ebbs before optimizing:

----------------------------------------------------------------
Basic Block _entry : loop Depth = 0 noPath = 0 , lastinLoop = 0
depth 1st num 1 : bbnum = 0 1st iCode = 1 , last iCode = 102
visited 0 : hasFcall = 1

defines bitVector :bitvector Size = 105 bSize = 14
Bits on { (5) (6) (8) (14) (15) (17) (18) (20) (26) (27) (29) (30) (32) (38) (39) (41) (42) (44) (50) (51) (53) (54) (56) (62) (63) (65) (66) (68) (74) (75) (77) (78) (80) (86) (87) (89) (90) (91) (92) (93) (98) (99) (100) }

local defines bitVector :bitvector Size = 105 bSize = 14
Bits on { (4) (5) (6) (7) (8) (9) (11) (13) (14) (15) (16) (17) (18) (19) (20) (21) (23) (25) (26) (27) (28) (29) (30) (31) (32) (33) (35) (37) (38) (39) (40) (41) (42) (43) (44) (45) (47) (49) (50) (51) (52) (53) (54) (55) (56) (57) (59) (61) (62) (63) (64) (65) (66) (67) (68) (69) (71) (73) (74) (75) (76) (77) (78) (79) (80) (81) (83) (85) (86) (87) (88) (89) (90) (91) (92) (93) (95) (97) (98) (99) (100) (102) }

pointers Set bitvector :
in pointers Set bitvector :
inDefs Set bitvector :
outDefs Set bitvector :bitvector Size = 105 bSize = 14
Bits on { (4) (5) (6) (8) (11) (13) (14) (15) (17) (18) (20) (23) (25) (26) (27) (29) (30) (32) (35) (37) (38) (39) (41) (42) (44) (47) (49) (50) (51) (53) (54) (56) (59) (61) (62) (63) (65) (66) (68) (71) (73) (74) (75) (77) (78) (80) (83) (85) (86) (87) (89) (90) (91) (92) (93) (95) (97) (98) (99) (100) (102) }

usesDefs Set bitvector :bitvector Size = 105 bSize = 14
Bits on { (4) (5) (6) (7) (8) (9) (11) (13) (14) (15) (16) (17) (18) (19) (20) (21) (23) (25) (26) (27) (28) (29) (30) (31) (32) (33) (35) (37) (38) (39) (40) (41) (42) (43) (44) (45) (47) (49) (50) (51) (52) (53) (54) (55) (56) (57) (59) (61) (62) (63) (64) (65) (66) (67) (68) (69) (71) (73) (74) (75) (76) (77) (78) (79) (80) (81) (83) (85) (86) (87) (88) (89) (90) (91) (92) (93) (95) (97) (98) (99) (100) (102) }

----------------------------------------------------------------
main.c(267:1:0:0:0)	 _entry($2) :
main.c(267:2:1:0:0)		proc _sendPacket [k1 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{void function    }
main.c(272:3:3:0:0)		send 0x80 {literal unsigned char}{argreg = 1}
main.c(272:4:4:0:0)		iTemp1 [k5 lr4:4 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{void} = call _putc [k4 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{void function    }
main.c(274:5:5:0:0)		iTemp3 [k9 lr5:6 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int} = _reading0 [k8 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int} >> 0x8 {literal unsigned char}
main.c(274:6:6:0:0)		iTemp4 [k10 lr6:7 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sendPacket_h_1_1} = (unsigned char)iTemp3 [k9 lr5:6 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int}
main.c(274:7:7:0:0)		iTemp2 [k6 lr7:98 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sendPacket_h_1_1} := iTemp4 [k10 lr6:7 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sendPacket_h_1_1}
main.c(275:8:8:0:0)		iTemp6 [k13 lr8:9 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sendPacket_l_1_1} = (unsigned char)_reading0 [k8 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int}
main.c(275:9:9:0:0)		iTemp5 [k11 lr9:98 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sendPacket_l_1_1} := iTemp6 [k13 lr8:9 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sendPacket_l_1_1}
main.c(276:10:10:0:0)		send iTemp2 [k6 lr7:98 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sendPacket_h_1_1}{argreg = 1}
main.c(276:11:11:0:0)		iTemp7 [k14 lr11:11 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{void} = call _putc [k4 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{void function    }
main.c(277:12:12:0:0)		send iTemp5 [k11 lr9:98 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sendPacket_l_1_1}{argreg = 1}
main.c(277:13:13:0:0)		iTemp8 [k15 lr13:13 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{void} = call _putc [k4 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{void function    }
main.c(278:14:14:0:0)		iTemp9 [k16 lr14:15 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{unsigned char} = iTemp2 [k6 lr7:98 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sendPacket_h_1_1} + iTemp5 [k11 lr9:98 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sendPacket_l_1_1}
main.c(278:15:15:0:0)		iTemp10 [k17 lr15:16 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sendPacket_checksum_1_1} := iTemp9 [k16 lr14:15 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{unsigned char}
main.c(278:16:16:0:0)		iTemp0 [k2 lr16:101 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sendPacket_checksum_1_1} := iTemp10 [k17 lr15:16 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sendPacket_checksum_1_1}
main.c(280:17:17:0:0)		iTemp11 [k19 lr17:18 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int} = _reading1 [k18 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int} >> 0x8 {literal unsigned char}
main.c(280:18:18:0:0)		iTemp12 [k20 lr18:19 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{unsigned char} = (unsigned char)iTemp11 [k19 lr17:18 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int}
main.c(280:19:19:0:0)		iTemp2 [k6 lr7:98 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sendPacket_h_1_1} := iTemp12 [k20 lr18:19 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{unsigned char}
main.c(281:20:20:0:0)		iTemp13 [k21 lr20:21 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{unsigned char} = (unsigned char)_reading1 [k18 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int}
main.c(281:21:21:0:0)		iTemp5 [k11 lr9:98 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sendPacket_l_1_1} := iTemp13 [k21 lr20:21 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{unsigned char}
main.c(282:22:22:0:0)		send iTemp2 [k6 lr7:98 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sendPacket_h_1_1}{argreg = 1}
main.c(282:23:23:0:0)		iTemp14 [k22 lr23:23 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{void} = call _putc [k4 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{void function    }
main.c(283:24:24:0:0)		send iTemp5 [k11 lr9:98 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sendPacket_l_1_1}{argreg = 1}
main.c(283:25:25:0:0)		iTemp15 [k23 lr25:25 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{void} = call _putc [k4 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{void function    }
main.c(284:26:26:0:0)		iTemp16 [k24 lr26:27 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{unsigned char} = iTemp2 [k6 lr7:98 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sendPacket_h_1_1} + iTemp5 [k11 lr9:98 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sendPacket_l_1_1}
main.c(284:27:27:0:0)		iTemp17 [k25 lr27:28 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{unsigned char} = iTemp0 [k2 lr16:101 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sendPacket_checksum_1_1} + iTemp16 [k24 lr26:27 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{unsigned char}
main.c(284:28:28:0:0)		iTemp0 [k2 lr16:101 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sendPacket_checksum_1_1} := iTemp17 [k25 lr27:28 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{unsigned char}
main.c(286:29:29:0:0)		iTemp18 [k27 lr29:30 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int} = _reading2 [k26 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int} >> 0x8 {literal unsigned char}
main.c(286:30:30:0:0)		iTemp19 [k28 lr30:31 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{unsigned char} = (unsigned char)iTemp18 [k27 lr29:30 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int}
main.c(286:31:31:0:0)		iTemp2 [k6 lr7:98 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sendPacket_h_1_1} := iTemp19 [k28 lr30:31 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{unsigned char}
main.c(287:32:32:0:0)		iTemp20 [k29 lr32:33 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{unsigned char} = (unsigned char)_reading2 [k26 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int}
main.c(287:33:33:0:0)		iTemp5 [k11 lr9:98 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sendPacket_l_1_1} := iTemp20 [k29 lr32:33 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{unsigned char}
main.c(288:34:34:0:0)		send iTemp2 [k6 lr7:98 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sendPacket_h_1_1}{argreg = 1}
main.c(288:35:35:0:0)		iTemp21 [k30 lr35:35 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{void} = call _putc [k4 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{void function    }
main.c(289:36:36:0:0)		send iTemp5 [k11 lr9:98 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sendPacket_l_1_1}{argreg = 1}
main.c(289:37:37:0:0)		iTemp22 [k31 lr37:37 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{void} = call _putc [k4 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{void function    }
main.c(290:38:38:0:0)		iTemp23 [k32 lr38:39 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{unsigned char} = iTemp2 [k6 lr7:98 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sendPacket_h_1_1} + iTemp5 [k11 lr9:98 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sendPacket_l_1_1}
main.c(290:39:39:0:0)		iTemp24 [k33 lr39:40 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{unsigned char} = iTemp0 [k2 lr16:101 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sendPacket_checksum_1_1} + iTemp23 [k32 lr38:39 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{unsigned char}
main.c(290:40:40:0:0)		iTemp0 [k2 lr16:101 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sendPacket_checksum_1_1} := iTemp24 [k33 lr39:40 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{unsigned char}
main.c(292:41:41:0:0)		iTemp25 [k35 lr41:42 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int} = _reading3 [k34 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int} >> 0x8 {literal unsigned char}
main.c(292:42:42:0:0)		iTemp26 [k36 lr42:43 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{unsigned char} = (unsigned char)iTemp25 [k35 lr41:42 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int}
main.c(292:43:43:0:0)		iTemp2 [k6 lr7:98 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sendPacket_h_1_1} := iTemp26 [k36 lr42:43 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{unsigned char}
main.c(293:44:44:0:0)		iTemp27 [k37 lr44:45 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{unsigned char} = (unsigned char)_reading3 [k34 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int}
main.c(293:45:45:0:0)		iTemp5 [k11 lr9:98 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sendPacket_l_1_1} := iTemp27 [k37 lr44:45 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{unsigned char}
main.c(294:46:46:0:0)		send iTemp2 [k6 lr7:98 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sendPacket_h_1_1}{argreg = 1}
main.c(294:47:47:0:0)		iTemp28 [k38 lr47:47 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{void} = call _putc [k4 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{void function    }
main.c(295:48:48:0:0)		send iTemp5 [k11 lr9:98 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sendPacket_l_1_1}{argreg = 1}
main.c(295:49:49:0:0)		iTemp29 [k39 lr49:49 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{void} = call _putc [k4 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{void function    }
main.c(296:50:50:0:0)		iTemp30 [k40 lr50:51 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{unsigned char} = iTemp2 [k6 lr7:98 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sendPacket_h_1_1} + iTemp5 [k11 lr9:98 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sendPacket_l_1_1}
main.c(296:51:51:0:0)		iTemp31 [k41 lr51:52 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{unsigned char} = iTemp0 [k2 lr16:101 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sendPacket_checksum_1_1} + iTemp30 [k40 lr50:51 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{unsigned char}
main.c(296:52:52:0:0)		iTemp0 [k2 lr16:101 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sendPacket_checksum_1_1} := iTemp31 [k41 lr51:52 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{unsigned char}
main.c(298:53:53:0:0)		iTemp32 [k43 lr53:54 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int} = _reading4 [k42 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int} >> 0x8 {literal unsigned char}
main.c(298:54:54:0:0)		iTemp33 [k44 lr54:55 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{unsigned char} = (unsigned char)iTemp32 [k43 lr53:54 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int}
main.c(298:55:55:0:0)		iTemp2 [k6 lr7:98 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sendPacket_h_1_1} := iTemp33 [k44 lr54:55 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{unsigned char}
main.c(299:56:56:0:0)		iTemp34 [k45 lr56:57 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{unsigned char} = (unsigned char)_reading4 [k42 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int}
main.c(299:57:57:0:0)		iTemp5 [k11 lr9:98 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sendPacket_l_1_1} := iTemp34 [k45 lr56:57 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{unsigned char}
main.c(300:58:58:0:0)		send iTemp2 [k6 lr7:98 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sendPacket_h_1_1}{argreg = 1}
main.c(300:59:59:0:0)		iTemp35 [k46 lr59:59 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{void} = call _putc [k4 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{void function    }
main.c(301:60:60:0:0)		send iTemp5 [k11 lr9:98 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sendPacket_l_1_1}{argreg = 1}
main.c(301:61:61:0:0)		iTemp36 [k47 lr61:61 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{void} = call _putc [k4 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{void function    }
main.c(302:62:62:0:0)		iTemp37 [k48 lr62:63 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{unsigned char} = iTemp2 [k6 lr7:98 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sendPacket_h_1_1} + iTemp5 [k11 lr9:98 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sendPacket_l_1_1}
main.c(302:63:63:0:0)		iTemp38 [k49 lr63:64 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{unsigned char} = iTemp0 [k2 lr16:101 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sendPacket_checksum_1_1} + iTemp37 [k48 lr62:63 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{unsigned char}
main.c(302:64:64:0:0)		iTemp0 [k2 lr16:101 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sendPacket_checksum_1_1} := iTemp38 [k49 lr63:64 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{unsigned char}
main.c(304:65:65:0:0)		iTemp39 [k51 lr65:66 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int} = _reading5 [k50 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int} >> 0x8 {literal unsigned char}
main.c(304:66:66:0:0)		iTemp40 [k52 lr66:67 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{unsigned char} = (unsigned char)iTemp39 [k51 lr65:66 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int}
main.c(304:67:67:0:0)		iTemp2 [k6 lr7:98 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sendPacket_h_1_1} := iTemp40 [k52 lr66:67 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{unsigned char}
main.c(305:68:68:0:0)		iTemp41 [k53 lr68:69 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{unsigned char} = (unsigned char)_reading5 [k50 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int}
main.c(305:69:69:0:0)		iTemp5 [k11 lr9:98 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sendPacket_l_1_1} := iTemp41 [k53 lr68:69 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{unsigned char}
main.c(306:70:70:0:0)		send iTemp2 [k6 lr7:98 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sendPacket_h_1_1}{argreg = 1}
main.c(306:71:71:0:0)		iTemp42 [k54 lr71:71 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{void} = call _putc [k4 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{void function    }
main.c(307:72:72:0:0)		send iTemp5 [k11 lr9:98 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sendPacket_l_1_1}{argreg = 1}
main.c(307:73:73:0:0)		iTemp43 [k55 lr73:73 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{void} = call _putc [k4 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{void function    }
main.c(308:74:74:0:0)		iTemp44 [k56 lr74:75 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{unsigned char} = iTemp2 [k6 lr7:98 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sendPacket_h_1_1} + iTemp5 [k11 lr9:98 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sendPacket_l_1_1}
main.c(308:75:75:0:0)		iTemp45 [k57 lr75:76 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{unsigned char} = iTemp0 [k2 lr16:101 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sendPacket_checksum_1_1} + iTemp44 [k56 lr74:75 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{unsigned char}
main.c(308:76:76:0:0)		iTemp0 [k2 lr16:101 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sendPacket_checksum_1_1} := iTemp45 [k57 lr75:76 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{unsigned char}
main.c(310:77:77:0:0)		iTemp46 [k59 lr77:78 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int} = _reading6 [k58 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int} >> 0x8 {literal unsigned char}
main.c(310:78:78:0:0)		iTemp47 [k60 lr78:79 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{unsigned char} = (unsigned char)iTemp46 [k59 lr77:78 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int}
main.c(310:79:79:0:0)		iTemp2 [k6 lr7:98 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sendPacket_h_1_1} := iTemp47 [k60 lr78:79 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{unsigned char}
main.c(311:80:80:0:0)		iTemp48 [k61 lr80:81 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{unsigned char} = (unsigned char)_reading6 [k58 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int}
main.c(311:81:81:0:0)		iTemp5 [k11 lr9:98 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sendPacket_l_1_1} := iTemp48 [k61 lr80:81 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{unsigned char}
main.c(312:82:82:0:0)		send iTemp2 [k6 lr7:98 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sendPacket_h_1_1}{argreg = 1}
main.c(312:83:83:0:0)		iTemp49 [k62 lr83:83 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{void} = call _putc [k4 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{void function    }
main.c(313:84:84:0:0)		send iTemp5 [k11 lr9:98 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sendPacket_l_1_1}{argreg = 1}
main.c(313:85:85:0:0)		iTemp50 [k63 lr85:85 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{void} = call _putc [k4 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{void function    }
main.c(314:86:86:0:0)		iTemp51 [k64 lr86:87 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{unsigned char} = iTemp2 [k6 lr7:98 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sendPacket_h_1_1} + iTemp5 [k11 lr9:98 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sendPacket_l_1_1}
main.c(314:87:87:0:0)		iTemp52 [k65 lr87:88 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{unsigned char} = iTemp0 [k2 lr16:101 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sendPacket_checksum_1_1} + iTemp51 [k64 lr86:87 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{unsigned char}
main.c(314:88:88:0:0)		iTemp0 [k2 lr16:101 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sendPacket_checksum_1_1} := iTemp52 [k65 lr87:88 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{unsigned char}
main.c(316:89:89:0:0)		iTemp53 [k67 lr89:90 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int} = _reading7 [k66 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int} >> 0x8 {literal unsigned char}
main.c(316:90:90:0:0)		iTemp54 [k68 lr90:91 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{unsigned char} = (unsigned char)iTemp53 [k67 lr89:90 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int}
main.c(316:91:91:0:0)		iTemp2 [k6 lr7:98 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sendPacket_h_1_1} := iTemp54 [k68 lr90:91 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{unsigned char}
main.c(317:92:92:0:0)		iTemp55 [k69 lr92:93 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{unsigned char} = (unsigned char)_reading7 [k66 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int}
main.c(317:93:93:0:0)		iTemp5 [k11 lr9:98 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sendPacket_l_1_1} := iTemp55 [k69 lr92:93 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{unsigned char}
main.c(318:94:94:0:0)		send iTemp2 [k6 lr7:98 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sendPacket_h_1_1}{argreg = 1}
main.c(318:95:95:0:0)		iTemp56 [k70 lr95:95 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{void} = call _putc [k4 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{void function    }
main.c(319:96:96:0:0)		send iTemp5 [k11 lr9:98 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sendPacket_l_1_1}{argreg = 1}
main.c(319:97:97:0:0)		iTemp57 [k71 lr97:97 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{void} = call _putc [k4 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{void function    }
main.c(320:98:98:0:0)		iTemp58 [k72 lr98:99 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{unsigned char} = iTemp2 [k6 lr7:98 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sendPacket_h_1_1} + iTemp5 [k11 lr9:98 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sendPacket_l_1_1}
main.c(320:99:99:0:0)		iTemp59 [k73 lr99:100 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{unsigned char} = iTemp0 [k2 lr16:101 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sendPacket_checksum_1_1} + iTemp58 [k72 lr98:99 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{unsigned char}
main.c(320:100:100:0:0)		iTemp0 [k2 lr16:101 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sendPacket_checksum_1_1} := iTemp59 [k73 lr99:100 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{unsigned char}
main.c(323:101:101:0:0)		send iTemp0 [k2 lr16:101 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sendPacket_checksum_1_1}{argreg = 1}
main.c(323:102:102:0:0)		iTemp60 [k74 lr102:102 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{void} = call _putc [k4 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{void function    }

----------------------------------------------------------------
Basic Block _return : loop Depth = 0 noPath = 0 , lastinLoop = 0
depth 1st num 2 : bbnum = 1 1st iCode = 103 , last iCode = 104
visited 0 : hasFcall = 0

defines bitVector :bitvector Size = 105 bSize = 14
Bits on { }

local defines bitVector :
pointers Set bitvector :
in pointers Set bitvector :
inDefs Set bitvector :bitvector Size = 105 bSize = 14
Bits on { (4) (5) (6) (8) (11) (13) (14) (15) (17) (18) (20) (23) (25) (26) (27) (29) (30) (32) (35) (37) (38) (39) (41) (42) (44) (47) (49) (50) (51) (53) (54) (56) (59) (61) (62) (63) (65) (66) (68) (71) (73) (74) (75) (77) (78) (80) (83) (85) (86) (87) (89) (90) (91) (92) (93) (95) (97) (98) (99) (100) (102) }

outDefs Set bitvector :bitvector Size = 105 bSize = 14
Bits on { (4) (5) (6) (8) (11) (13) (14) (15) (17) (18) (20) (23) (25) (26) (27) (29) (30) (32) (35) (37) (38) (39) (41) (42) (44) (47) (49) (50) (51) (53) (54) (56) (59) (61) (62) (63) (65) (66) (68) (71) (73) (74) (75) (77) (78) (80) (83) (85) (86) (87) (89) (90) (91) (92) (93) (95) (97) (98) (99) (100) (102) }

usesDefs Set bitvector :bitvector Size = 105 bSize = 14
Bits on { }

----------------------------------------------------------------
main.c(323:103:103:0:0)	 _return($1) :
main.c(323:104:104:0:0)		eproc _sendPacket [k1 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{void function    }
packRegisters
packRegsForAssign
  result:iTemp2 [k6 lr7:98 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sendPacket_h_1_1}
  left:
  right:iTemp4 [k10 lr6:7 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sendPacket_h_1_1}
  2760 - dic result key == ic right key -- pointer set=N
  packing. removing iTemp4
  replacing with iTemp4
  result:iTemp2 [k6 lr6:98 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sendPacket_h_1_1}
  left:
  right:iTemp4 [k10 lr6:7 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sendPacket_h_1_1}
packRegsForAssign
  result:iTemp5 [k11 lr9:98 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sendPacket_l_1_1}
  left:
  right:iTemp6 [k13 lr8:9 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sendPacket_l_1_1}
  2760 - dic result key == ic right key -- pointer set=N
  packing. removing iTemp6
  replacing with iTemp6
  result:iTemp5 [k11 lr8:98 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sendPacket_l_1_1}
  left:
  right:iTemp6 [k13 lr8:9 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sendPacket_l_1_1}
packRegsForAssign
  result:iTemp10 [k17 lr15:16 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sendPacket_checksum_1_1}
  left:
  right:iTemp9 [k16 lr14:15 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{unsigned char}
  2760 - dic result key == ic right key -- pointer set=N
  packing. removing iTemp9
  replacing with iTemp9
  result:iTemp10 [k17 lr14:16 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sendPacket_checksum_1_1}
  left:
  right:iTemp9 [k16 lr14:15 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{unsigned char}
packRegsForAssign
  result:iTemp0 [k2 lr16:101 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sendPacket_checksum_1_1}
  left:
  right:iTemp10 [k17 lr14:16 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sendPacket_checksum_1_1}
  2760 - dic result key == ic right key -- pointer set=N
  packing. removing iTemp10
  replacing with iTemp10
  result:iTemp0 [k2 lr14:101 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sendPacket_checksum_1_1}
  left:
  right:iTemp10 [k17 lr14:16 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sendPacket_checksum_1_1}
packRegsForAssign
  result:iTemp2 [k6 lr6:98 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sendPacket_h_1_1}
  left:
  right:iTemp12 [k20 lr18:19 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{unsigned char}
  2760 - dic result key == ic right key -- pointer set=N
  packing. removing iTemp12
  replacing with iTemp12
  result:iTemp2 [k6 lr6:98 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sendPacket_h_1_1}
  left:
  right:iTemp12 [k20 lr18:19 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{unsigned char}
packRegsForAssign
  result:iTemp5 [k11 lr8:98 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sendPacket_l_1_1}
  left:
  right:iTemp13 [k21 lr20:21 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{unsigned char}
  2760 - dic result key == ic right key -- pointer set=N
  packing. removing iTemp13
  replacing with iTemp13
  result:iTemp5 [k11 lr8:98 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sendPacket_l_1_1}
  left:
  right:iTemp13 [k21 lr20:21 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{unsigned char}
packRegsForAssign
  result:iTemp0 [k2 lr14:101 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sendPacket_checksum_1_1}
  left:
  right:iTemp17 [k25 lr27:28 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{unsigned char}
  2760 - dic result key == ic right key -- pointer set=N
  packing. removing iTemp17
  replacing with iTemp17
  result:iTemp0 [k2 lr14:101 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sendPacket_checksum_1_1}
  left:
  right:iTemp17 [k25 lr27:28 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{unsigned char}
packRegsForAssign
  result:iTemp2 [k6 lr6:98 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sendPacket_h_1_1}
  left:
  right:iTemp19 [k28 lr30:31 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{unsigned char}
  2760 - dic result key == ic right key -- pointer set=N
  packing. removing iTemp19
  replacing with iTemp19
  result:iTemp2 [k6 lr6:98 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sendPacket_h_1_1}
  left:
  right:iTemp19 [k28 lr30:31 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{unsigned char}
packRegsForAssign
  result:iTemp5 [k11 lr8:98 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sendPacket_l_1_1}
  left:
  right:iTemp20 [k29 lr32:33 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{unsigned char}
  2760 - dic result key == ic right key -- pointer set=N
  packing. removing iTemp20
  replacing with iTemp20
  result:iTemp5 [k11 lr8:98 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sendPacket_l_1_1}
  left:
  right:iTemp20 [k29 lr32:33 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{unsigned char}
packRegsForAssign
  result:iTemp0 [k2 lr14:101 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sendPacket_checksum_1_1}
  left:
  right:iTemp24 [k33 lr39:40 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{unsigned char}
  2760 - dic result key == ic right key -- pointer set=N
  packing. removing iTemp24
  replacing with iTemp24
  result:iTemp0 [k2 lr14:101 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sendPacket_checksum_1_1}
  left:
  right:iTemp24 [k33 lr39:40 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{unsigned char}
packRegsForAssign
  result:iTemp2 [k6 lr6:98 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sendPacket_h_1_1}
  left:
  right:iTemp26 [k36 lr42:43 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{unsigned char}
  2760 - dic result key == ic right key -- pointer set=N
  packing. removing iTemp26
  replacing with iTemp26
  result:iTemp2 [k6 lr6:98 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sendPacket_h_1_1}
  left:
  right:iTemp26 [k36 lr42:43 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{unsigned char}
packRegsForAssign
  result:iTemp5 [k11 lr8:98 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sendPacket_l_1_1}
  left:
  right:iTemp27 [k37 lr44:45 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{unsigned char}
  2760 - dic result key == ic right key -- pointer set=N
  packing. removing iTemp27
  replacing with iTemp27
  result:iTemp5 [k11 lr8:98 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sendPacket_l_1_1}
  left:
  right:iTemp27 [k37 lr44:45 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{unsigned char}
packRegsForAssign
  result:iTemp0 [k2 lr14:101 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sendPacket_checksum_1_1}
  left:
  right:iTemp31 [k41 lr51:52 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{unsigned char}
  2760 - dic result key == ic right key -- pointer set=N
  packing. removing iTemp31
  replacing with iTemp31
  result:iTemp0 [k2 lr14:101 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sendPacket_checksum_1_1}
  left:
  right:iTemp31 [k41 lr51:52 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{unsigned char}
packRegsForAssign
  result:iTemp2 [k6 lr6:98 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sendPacket_h_1_1}
  left:
  right:iTemp33 [k44 lr54:55 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{unsigned char}
  2760 - dic result key == ic right key -- pointer set=N
  packing. removing iTemp33
  replacing with iTemp33
  result:iTemp2 [k6 lr6:98 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sendPacket_h_1_1}
  left:
  right:iTemp33 [k44 lr54:55 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{unsigned char}
packRegsForAssign
  result:iTemp5 [k11 lr8:98 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sendPacket_l_1_1}
  left:
  right:iTemp34 [k45 lr56:57 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{unsigned char}
  2760 - dic result key == ic right key -- pointer set=N
  packing. removing iTemp34
  replacing with iTemp34
  result:iTemp5 [k11 lr8:98 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sendPacket_l_1_1}
  left:
  right:iTemp34 [k45 lr56:57 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{unsigned char}
packRegsForAssign
  result:iTemp0 [k2 lr14:101 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sendPacket_checksum_1_1}
  left:
  right:iTemp38 [k49 lr63:64 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{unsigned char}
  2760 - dic result key == ic right key -- pointer set=N
  packing. removing iTemp38
  replacing with iTemp38
  result:iTemp0 [k2 lr14:101 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sendPacket_checksum_1_1}
  left:
  right:iTemp38 [k49 lr63:64 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{unsigned char}
packRegsForAssign
  result:iTemp2 [k6 lr6:98 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sendPacket_h_1_1}
  left:
  right:iTemp40 [k52 lr66:67 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{unsigned char}
  2760 - dic result key == ic right key -- pointer set=N
  packing. removing iTemp40
  replacing with iTemp40
  result:iTemp2 [k6 lr6:98 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sendPacket_h_1_1}
  left:
  right:iTemp40 [k52 lr66:67 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{unsigned char}
packRegsForAssign
  result:iTemp5 [k11 lr8:98 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sendPacket_l_1_1}
  left:
  right:iTemp41 [k53 lr68:69 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{unsigned char}
  2760 - dic result key == ic right key -- pointer set=N
  packing. removing iTemp41
  replacing with iTemp41
  result:iTemp5 [k11 lr8:98 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sendPacket_l_1_1}
  left:
  right:iTemp41 [k53 lr68:69 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{unsigned char}
packRegsForAssign
  result:iTemp0 [k2 lr14:101 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sendPacket_checksum_1_1}
  left:
  right:iTemp45 [k57 lr75:76 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{unsigned char}
  2760 - dic result key == ic right key -- pointer set=N
  packing. removing iTemp45
  replacing with iTemp45
  result:iTemp0 [k2 lr14:101 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sendPacket_checksum_1_1}
  left:
  right:iTemp45 [k57 lr75:76 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{unsigned char}
packRegsForAssign
  result:iTemp2 [k6 lr6:98 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sendPacket_h_1_1}
  left:
  right:iTemp47 [k60 lr78:79 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{unsigned char}
  2760 - dic result key == ic right key -- pointer set=N
  packing. removing iTemp47
  replacing with iTemp47
  result:iTemp2 [k6 lr6:98 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sendPacket_h_1_1}
  left:
  right:iTemp47 [k60 lr78:79 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{unsigned char}
packRegsForAssign
  result:iTemp5 [k11 lr8:98 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sendPacket_l_1_1}
  left:
  right:iTemp48 [k61 lr80:81 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{unsigned char}
  2760 - dic result key == ic right key -- pointer set=N
  packing. removing iTemp48
  replacing with iTemp48
  result:iTemp5 [k11 lr8:98 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sendPacket_l_1_1}
  left:
  right:iTemp48 [k61 lr80:81 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{unsigned char}
packRegsForAssign
  result:iTemp0 [k2 lr14:101 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sendPacket_checksum_1_1}
  left:
  right:iTemp52 [k65 lr87:88 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{unsigned char}
  2760 - dic result key == ic right key -- pointer set=N
  packing. removing iTemp52
  replacing with iTemp52
  result:iTemp0 [k2 lr14:101 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sendPacket_checksum_1_1}
  left:
  right:iTemp52 [k65 lr87:88 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{unsigned char}
packRegsForAssign
  result:iTemp2 [k6 lr6:98 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sendPacket_h_1_1}
  left:
  right:iTemp54 [k68 lr90:91 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{unsigned char}
  2760 - dic result key == ic right key -- pointer set=N
  packing. removing iTemp54
  replacing with iTemp54
  result:iTemp2 [k6 lr6:98 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sendPacket_h_1_1}
  left:
  right:iTemp54 [k68 lr90:91 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{unsigned char}
packRegsForAssign
  result:iTemp5 [k11 lr8:98 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sendPacket_l_1_1}
  left:
  right:iTemp55 [k69 lr92:93 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{unsigned char}
  2760 - dic result key == ic right key -- pointer set=N
  packing. removing iTemp55
  replacing with iTemp55
  result:iTemp5 [k11 lr8:98 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sendPacket_l_1_1}
  left:
  right:iTemp55 [k69 lr92:93 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{unsigned char}
packRegsForAssign
  result:iTemp0 [k2 lr14:101 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sendPacket_checksum_1_1}
  left:
  right:iTemp59 [k73 lr99:100 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{unsigned char}
  2760 - dic result key == ic right key -- pointer set=N
  packing. removing iTemp59
  replacing with iTemp59
  result:iTemp0 [k2 lr14:101 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sendPacket_checksum_1_1}
  left:
  right:iTemp59 [k73 lr99:100 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{unsigned char}
  left:_sendPacket [k1 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{void function    }
        Symbol type: void function    
  3594 - pointer reg req = 0
  left:_putc [k4 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{void function    }
        Symbol type: void function    
  result:iTemp1 [k5 lr4:4 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{void}
          Symbol type: void
  3594 - pointer reg req = 0
  left:_reading0 [k8 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int}
        Symbol type: int
  result:iTemp3 [k9 lr5:6 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int}
          Symbol type: int
  3594 - pointer reg req = 0
packRegsForAccUse
  packRegsForAccUse:3181
  packRegsForAccUse:3188
  right:iTemp3 [k9 lr5:6 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int}
         Symbol type: int
  result:iTemp2 [k6 lr6:98 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sendPacket_h_1_1}
          Symbol type: unsigned char
  3594 - pointer reg req = 0
  3662 - casting
packRegsForOneuse
  right:_reading0 [k8 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int}
         Symbol type: int
  result:iTemp5 [k11 lr8:98 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sendPacket_l_1_1}
          Symbol type: unsigned char
  3594 - pointer reg req = 0
  3662 - casting
packRegsForOneuse
  left:iTemp2 [k6 lr6:98 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sendPacket_h_1_1}
        Symbol type: unsigned char
  3594 - pointer reg req = 0
  left:_putc [k4 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{void function    }
        Symbol type: void function    
  result:iTemp7 [k14 lr11:11 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{void}
          Symbol type: void
  3594 - pointer reg req = 0
  left:iTemp5 [k11 lr8:98 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sendPacket_l_1_1}
        Symbol type: unsigned char
  3594 - pointer reg req = 0
  left:_putc [k4 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{void function    }
        Symbol type: void function    
  result:iTemp8 [k15 lr13:13 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{void}
          Symbol type: void
  3594 - pointer reg req = 0
  left:iTemp2 [k6 lr6:98 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sendPacket_h_1_1}
        Symbol type: unsigned char
  right:iTemp5 [k11 lr8:98 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sendPacket_l_1_1}
         Symbol type: unsigned char
  result:iTemp0 [k2 lr14:101 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sendPacket_checksum_1_1}
          Symbol type: unsigned char
  3594 - pointer reg req = 0
packRegsForSupport
packRegsForAccUse
  packRegsForAccUse:3181
  packRegsForAccUse:3188
  left:_reading1 [k18 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int}
        Symbol type: int
  result:iTemp11 [k19 lr17:18 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int}
          Symbol type: int
  3594 - pointer reg req = 0
packRegsForAccUse
  packRegsForAccUse:3181
  packRegsForAccUse:3188
  right:iTemp11 [k19 lr17:18 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int}
         Symbol type: int
  result:iTemp2 [k6 lr6:98 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sendPacket_h_1_1}
          Symbol type: unsigned char
  3594 - pointer reg req = 0
  3662 - casting
packRegsForOneuse
  right:_reading1 [k18 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int}
         Symbol type: int
  result:iTemp5 [k11 lr8:98 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sendPacket_l_1_1}
          Symbol type: unsigned char
  3594 - pointer reg req = 0
  3662 - casting
packRegsForOneuse
  left:iTemp2 [k6 lr6:98 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sendPacket_h_1_1}
        Symbol type: unsigned char
  3594 - pointer reg req = 0
  left:_putc [k4 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{void function    }
        Symbol type: void function    
  result:iTemp14 [k22 lr23:23 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{void}
          Symbol type: void
  3594 - pointer reg req = 0
  left:iTemp5 [k11 lr8:98 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sendPacket_l_1_1}
        Symbol type: unsigned char
  3594 - pointer reg req = 0
  left:_putc [k4 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{void function    }
        Symbol type: void function    
  result:iTemp15 [k23 lr25:25 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{void}
          Symbol type: void
  3594 - pointer reg req = 0
  left:iTemp2 [k6 lr6:98 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sendPacket_h_1_1}
        Symbol type: unsigned char
  right:iTemp5 [k11 lr8:98 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sendPacket_l_1_1}
         Symbol type: unsigned char
  result:iTemp16 [k24 lr26:27 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{unsigned char}
          Symbol type: unsigned char
  3594 - pointer reg req = 0
packRegsForSupport
packRegsForAccUse
  packRegsForAccUse:3181
  packRegsForAccUse:3188
  packRegsForAccUse:3218
  packRegsForAccUse:3235
  packRegsForAccUse:3243
  packRegsForAccUse:3284
  packRegsForAccUse:3294
packRegsForAccUse - Yes we are using the accumulator
  left:iTemp0 [k2 lr14:101 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sendPacket_checksum_1_1}
        Symbol type: unsigned char
  right:iTemp16 [k24 lr26:27 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{unsigned char}
         Symbol type: unsigned char
  result:iTemp0 [k2 lr14:101 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sendPacket_checksum_1_1}
          Symbol type: unsigned char
  3594 - pointer reg req = 0
packRegsForSupport
findAssignToSym
packRegsForAccUse
  packRegsForAccUse:3181
  packRegsForAccUse:3188
  left:_reading2 [k26 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int}
        Symbol type: int
  result:iTemp18 [k27 lr29:30 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int}
          Symbol type: int
  3594 - pointer reg req = 0
packRegsForAccUse
  packRegsForAccUse:3181
  packRegsForAccUse:3188
  right:iTemp18 [k27 lr29:30 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int}
         Symbol type: int
  result:iTemp2 [k6 lr6:98 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sendPacket_h_1_1}
          Symbol type: unsigned char
  3594 - pointer reg req = 0
  3662 - casting
packRegsForOneuse
  right:_reading2 [k26 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int}
         Symbol type: int
  result:iTemp5 [k11 lr8:98 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sendPacket_l_1_1}
          Symbol type: unsigned char
  3594 - pointer reg req = 0
  3662 - casting
packRegsForOneuse
  left:iTemp2 [k6 lr6:98 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sendPacket_h_1_1}
        Symbol type: unsigned char
  3594 - pointer reg req = 0
  left:_putc [k4 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{void function    }
        Symbol type: void function    
  result:iTemp21 [k30 lr35:35 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{void}
          Symbol type: void
  3594 - pointer reg req = 0
  left:iTemp5 [k11 lr8:98 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sendPacket_l_1_1}
        Symbol type: unsigned char
  3594 - pointer reg req = 0
  left:_putc [k4 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{void function    }
        Symbol type: void function    
  result:iTemp22 [k31 lr37:37 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{void}
          Symbol type: void
  3594 - pointer reg req = 0
  left:iTemp2 [k6 lr6:98 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sendPacket_h_1_1}
        Symbol type: unsigned char
  right:iTemp5 [k11 lr8:98 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sendPacket_l_1_1}
         Symbol type: unsigned char
  result:iTemp23 [k32 lr38:39 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{unsigned char}
          Symbol type: unsigned char
  3594 - pointer reg req = 0
packRegsForSupport
packRegsForAccUse
  packRegsForAccUse:3181
  packRegsForAccUse:3188
  packRegsForAccUse:3218
  packRegsForAccUse:3235
  packRegsForAccUse:3243
  packRegsForAccUse:3284
  packRegsForAccUse:3294
packRegsForAccUse - Yes we are using the accumulator
  left:iTemp0 [k2 lr14:101 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sendPacket_checksum_1_1}
        Symbol type: unsigned char
  right:iTemp23 [k32 lr38:39 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{unsigned char}
         Symbol type: unsigned char
  result:iTemp0 [k2 lr14:101 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sendPacket_checksum_1_1}
          Symbol type: unsigned char
  3594 - pointer reg req = 0
packRegsForSupport
findAssignToSym
packRegsForAccUse
  packRegsForAccUse:3181
  packRegsForAccUse:3188
  left:_reading3 [k34 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int}
        Symbol type: int
  result:iTemp25 [k35 lr41:42 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int}
          Symbol type: int
  3594 - pointer reg req = 0
packRegsForAccUse
  packRegsForAccUse:3181
  packRegsForAccUse:3188
  right:iTemp25 [k35 lr41:42 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int}
         Symbol type: int
  result:iTemp2 [k6 lr6:98 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sendPacket_h_1_1}
          Symbol type: unsigned char
  3594 - pointer reg req = 0
  3662 - casting
packRegsForOneuse
  right:_reading3 [k34 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int}
         Symbol type: int
  result:iTemp5 [k11 lr8:98 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sendPacket_l_1_1}
          Symbol type: unsigned char
  3594 - pointer reg req = 0
  3662 - casting
packRegsForOneuse
  left:iTemp2 [k6 lr6:98 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sendPacket_h_1_1}
        Symbol type: unsigned char
  3594 - pointer reg req = 0
  left:_putc [k4 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{void function    }
        Symbol type: void function    
  result:iTemp28 [k38 lr47:47 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{void}
          Symbol type: void
  3594 - pointer reg req = 0
  left:iTemp5 [k11 lr8:98 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sendPacket_l_1_1}
        Symbol type: unsigned char
  3594 - pointer reg req = 0
  left:_putc [k4 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{void function    }
        Symbol type: void function    
  result:iTemp29 [k39 lr49:49 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{void}
          Symbol type: void
  3594 - pointer reg req = 0
  left:iTemp2 [k6 lr6:98 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sendPacket_h_1_1}
        Symbol type: unsigned char
  right:iTemp5 [k11 lr8:98 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sendPacket_l_1_1}
         Symbol type: unsigned char
  result:iTemp30 [k40 lr50:51 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{unsigned char}
          Symbol type: unsigned char
  3594 - pointer reg req = 0
packRegsForSupport
packRegsForAccUse
  packRegsForAccUse:3181
  packRegsForAccUse:3188
  packRegsForAccUse:3218
  packRegsForAccUse:3235
  packRegsForAccUse:3243
  packRegsForAccUse:3284
  packRegsForAccUse:3294
packRegsForAccUse - Yes we are using the accumulator
  left:iTemp0 [k2 lr14:101 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sendPacket_checksum_1_1}
        Symbol type: unsigned char
  right:iTemp30 [k40 lr50:51 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{unsigned char}
         Symbol type: unsigned char
  result:iTemp0 [k2 lr14:101 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sendPacket_checksum_1_1}
          Symbol type: unsigned char
  3594 - pointer reg req = 0
packRegsForSupport
findAssignToSym
packRegsForAccUse
  packRegsForAccUse:3181
  packRegsForAccUse:3188
  left:_reading4 [k42 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int}
        Symbol type: int
  result:iTemp32 [k43 lr53:54 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int}
          Symbol type: int
  3594 - pointer reg req = 0
packRegsForAccUse
  packRegsForAccUse:3181
  packRegsForAccUse:3188
  right:iTemp32 [k43 lr53:54 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int}
         Symbol type: int
  result:iTemp2 [k6 lr6:98 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sendPacket_h_1_1}
          Symbol type: unsigned char
  3594 - pointer reg req = 0
  3662 - casting
packRegsForOneuse
  right:_reading4 [k42 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int}
         Symbol type: int
  result:iTemp5 [k11 lr8:98 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sendPacket_l_1_1}
          Symbol type: unsigned char
  3594 - pointer reg req = 0
  3662 - casting
packRegsForOneuse
  left:iTemp2 [k6 lr6:98 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sendPacket_h_1_1}
        Symbol type: unsigned char
  3594 - pointer reg req = 0
  left:_putc [k4 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{void function    }
        Symbol type: void function    
  result:iTemp35 [k46 lr59:59 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{void}
          Symbol type: void
  3594 - pointer reg req = 0
  left:iTemp5 [k11 lr8:98 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sendPacket_l_1_1}
        Symbol type: unsigned char
  3594 - pointer reg req = 0
  left:_putc [k4 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{void function    }
        Symbol type: void function    
  result:iTemp36 [k47 lr61:61 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{void}
          Symbol type: void
  3594 - pointer reg req = 0
  left:iTemp2 [k6 lr6:98 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sendPacket_h_1_1}
        Symbol type: unsigned char
  right:iTemp5 [k11 lr8:98 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sendPacket_l_1_1}
         Symbol type: unsigned char
  result:iTemp37 [k48 lr62:63 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{unsigned char}
          Symbol type: unsigned char
  3594 - pointer reg req = 0
packRegsForSupport
packRegsForAccUse
  packRegsForAccUse:3181
  packRegsForAccUse:3188
  packRegsForAccUse:3218
  packRegsForAccUse:3235
  packRegsForAccUse:3243
  packRegsForAccUse:3284
  packRegsForAccUse:3294
packRegsForAccUse - Yes we are using the accumulator
  left:iTemp0 [k2 lr14:101 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sendPacket_checksum_1_1}
        Symbol type: unsigned char
  right:iTemp37 [k48 lr62:63 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{unsigned char}
         Symbol type: unsigned char
  result:iTemp0 [k2 lr14:101 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sendPacket_checksum_1_1}
          Symbol type: unsigned char
  3594 - pointer reg req = 0
packRegsForSupport
findAssignToSym
packRegsForAccUse
  packRegsForAccUse:3181
  packRegsForAccUse:3188
  left:_reading5 [k50 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int}
        Symbol type: int
  result:iTemp39 [k51 lr65:66 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int}
          Symbol type: int
  3594 - pointer reg req = 0
packRegsForAccUse
  packRegsForAccUse:3181
  packRegsForAccUse:3188
  right:iTemp39 [k51 lr65:66 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int}
         Symbol type: int
  result:iTemp2 [k6 lr6:98 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sendPacket_h_1_1}
          Symbol type: unsigned char
  3594 - pointer reg req = 0
  3662 - casting
packRegsForOneuse
  right:_reading5 [k50 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int}
         Symbol type: int
  result:iTemp5 [k11 lr8:98 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sendPacket_l_1_1}
          Symbol type: unsigned char
  3594 - pointer reg req = 0
  3662 - casting
packRegsForOneuse
  left:iTemp2 [k6 lr6:98 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sendPacket_h_1_1}
        Symbol type: unsigned char
  3594 - pointer reg req = 0
  left:_putc [k4 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{void function    }
        Symbol type: void function    
  result:iTemp42 [k54 lr71:71 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{void}
          Symbol type: void
  3594 - pointer reg req = 0
  left:iTemp5 [k11 lr8:98 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sendPacket_l_1_1}
        Symbol type: unsigned char
  3594 - pointer reg req = 0
  left:_putc [k4 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{void function    }
        Symbol type: void function    
  result:iTemp43 [k55 lr73:73 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{void}
          Symbol type: void
  3594 - pointer reg req = 0
  left:iTemp2 [k6 lr6:98 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sendPacket_h_1_1}
        Symbol type: unsigned char
  right:iTemp5 [k11 lr8:98 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sendPacket_l_1_1}
         Symbol type: unsigned char
  result:iTemp44 [k56 lr74:75 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{unsigned char}
          Symbol type: unsigned char
  3594 - pointer reg req = 0
packRegsForSupport
packRegsForAccUse
  packRegsForAccUse:3181
  packRegsForAccUse:3188
  packRegsForAccUse:3218
  packRegsForAccUse:3235
  packRegsForAccUse:3243
  packRegsForAccUse:3284
  packRegsForAccUse:3294
packRegsForAccUse - Yes we are using the accumulator
  left:iTemp0 [k2 lr14:101 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sendPacket_checksum_1_1}
        Symbol type: unsigned char
  right:iTemp44 [k56 lr74:75 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{unsigned char}
         Symbol type: unsigned char
  result:iTemp0 [k2 lr14:101 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sendPacket_checksum_1_1}
          Symbol type: unsigned char
  3594 - pointer reg req = 0
packRegsForSupport
findAssignToSym
packRegsForAccUse
  packRegsForAccUse:3181
  packRegsForAccUse:3188
  left:_reading6 [k58 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int}
        Symbol type: int
  result:iTemp46 [k59 lr77:78 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int}
          Symbol type: int
  3594 - pointer reg req = 0
packRegsForAccUse
  packRegsForAccUse:3181
  packRegsForAccUse:3188
  right:iTemp46 [k59 lr77:78 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int}
         Symbol type: int
  result:iTemp2 [k6 lr6:98 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sendPacket_h_1_1}
          Symbol type: unsigned char
  3594 - pointer reg req = 0
  3662 - casting
packRegsForOneuse
  right:_reading6 [k58 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int}
         Symbol type: int
  result:iTemp5 [k11 lr8:98 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sendPacket_l_1_1}
          Symbol type: unsigned char
  3594 - pointer reg req = 0
  3662 - casting
packRegsForOneuse
  left:iTemp2 [k6 lr6:98 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sendPacket_h_1_1}
        Symbol type: unsigned char
  3594 - pointer reg req = 0
  left:_putc [k4 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{void function    }
        Symbol type: void function    
  result:iTemp49 [k62 lr83:83 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{void}
          Symbol type: void
  3594 - pointer reg req = 0
  left:iTemp5 [k11 lr8:98 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sendPacket_l_1_1}
        Symbol type: unsigned char
  3594 - pointer reg req = 0
  left:_putc [k4 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{void function    }
        Symbol type: void function    
  result:iTemp50 [k63 lr85:85 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{void}
          Symbol type: void
  3594 - pointer reg req = 0
  left:iTemp2 [k6 lr6:98 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sendPacket_h_1_1}
        Symbol type: unsigned char
  right:iTemp5 [k11 lr8:98 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sendPacket_l_1_1}
         Symbol type: unsigned char
  result:iTemp51 [k64 lr86:87 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{unsigned char}
          Symbol type: unsigned char
  3594 - pointer reg req = 0
packRegsForSupport
packRegsForAccUse
  packRegsForAccUse:3181
  packRegsForAccUse:3188
  packRegsForAccUse:3218
  packRegsForAccUse:3235
  packRegsForAccUse:3243
  packRegsForAccUse:3284
  packRegsForAccUse:3294
packRegsForAccUse - Yes we are using the accumulator
  left:iTemp0 [k2 lr14:101 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sendPacket_checksum_1_1}
        Symbol type: unsigned char
  right:iTemp51 [k64 lr86:87 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{unsigned char}
         Symbol type: unsigned char
  result:iTemp0 [k2 lr14:101 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sendPacket_checksum_1_1}
          Symbol type: unsigned char
  3594 - pointer reg req = 0
packRegsForSupport
findAssignToSym
packRegsForAccUse
  packRegsForAccUse:3181
  packRegsForAccUse:3188
  left:_reading7 [k66 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int}
        Symbol type: int
  result:iTemp53 [k67 lr89:90 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int}
          Symbol type: int
  3594 - pointer reg req = 0
packRegsForAccUse
  packRegsForAccUse:3181
  packRegsForAccUse:3188
  right:iTemp53 [k67 lr89:90 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int}
         Symbol type: int
  result:iTemp2 [k6 lr6:98 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sendPacket_h_1_1}
          Symbol type: unsigned char
  3594 - pointer reg req = 0
  3662 - casting
packRegsForOneuse
  right:_reading7 [k66 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int}
         Symbol type: int
  result:iTemp5 [k11 lr8:98 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sendPacket_l_1_1}
          Symbol type: unsigned char
  3594 - pointer reg req = 0
  3662 - casting
packRegsForOneuse
  left:iTemp2 [k6 lr6:98 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sendPacket_h_1_1}
        Symbol type: unsigned char
  3594 - pointer reg req = 0
  left:_putc [k4 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{void function    }
        Symbol type: void function    
  result:iTemp56 [k70 lr95:95 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{void}
          Symbol type: void
  3594 - pointer reg req = 0
  left:iTemp5 [k11 lr8:98 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sendPacket_l_1_1}
        Symbol type: unsigned char
  3594 - pointer reg req = 0
  left:_putc [k4 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{void function    }
        Symbol type: void function    
  result:iTemp57 [k71 lr97:97 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{void}
          Symbol type: void
  3594 - pointer reg req = 0
  left:iTemp2 [k6 lr6:98 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sendPacket_h_1_1}
        Symbol type: unsigned char
  right:iTemp5 [k11 lr8:98 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sendPacket_l_1_1}
         Symbol type: unsigned char
  result:iTemp58 [k72 lr98:99 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{unsigned char}
          Symbol type: unsigned char
  3594 - pointer reg req = 0
packRegsForSupport
findAssignToSym
findAssignToSym
packRegsForAccUse
  packRegsForAccUse:3181
  packRegsForAccUse:3188
  packRegsForAccUse:3218
  packRegsForAccUse:3235
  packRegsForAccUse:3243
  packRegsForAccUse:3284
  packRegsForAccUse:3294
packRegsForAccUse - Yes we are using the accumulator
  left:iTemp0 [k2 lr14:101 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sendPacket_checksum_1_1}
        Symbol type: unsigned char
  right:iTemp58 [k72 lr98:99 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{unsigned char}
         Symbol type: unsigned char
  result:iTemp0 [k2 lr14:101 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sendPacket_checksum_1_1}
          Symbol type: unsigned char
  3594 - pointer reg req = 0
packRegsForSupport
findAssignToSym
packRegsForAccUse
  packRegsForAccUse:3181
  packRegsForAccUse:3188
  left:iTemp0 [k2 lr14:101 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sendPacket_checksum_1_1}
        Symbol type: unsigned char
  3594 - pointer reg req = 0
  left:_putc [k4 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{void function    }
        Symbol type: void function    
  result:iTemp60 [k74 lr102:102 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{void}
          Symbol type: void
  3594 - pointer reg req = 0
packRegisters
  left:_sendPacket [k1 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{void function    }
        Symbol type: void function    
dir registers allocated so far:
  -- #0 reg = s0xFFF  key 0, rIdx = 4095, size 1
  -- #1 reg = s0x1000  key 0, rIdx = 4096, size 1
  -- #2 reg = s0x1001  key 0, rIdx = 4097, size 1
  -- #3 reg = s0x1002  key 0, rIdx = 4098, size 1
  -- #4 reg = s0x1003  key 0, rIdx = 4099, size 1
  -- #5 reg = s0x1004  key 0, rIdx = 4100, size 1
  -- #6 reg = s0x1005  key 0, rIdx = 4101, size 1
  -- #7 reg = s0x1006  key 0, rIdx = 4102, size 1
  -- #8 reg = r0x20  key 0, rIdx = 32, size 1
  -- #9 reg = r0x21  key 0, rIdx = 33, size 1
  -- #10 reg = r0x22  key 0, rIdx = 34, size 1
  -- #11 reg = r0x23  key 0, rIdx = 35, size 1
  -- #12 reg = r0x24  key 0, rIdx = 36, size 1
  -- #13 reg = r0x25  key 0, rIdx = 37, size 1
  -- #14 reg = r0x26  key 0, rIdx = 38, size 1
  -- #15 reg = r0x27  key 0, rIdx = 39, size 1
  -- #16 reg = r0x28  key 0, rIdx = 40, size 1
  -- #17 reg = r0x29  key 0, rIdx = 41, size 1
  -- #18 reg = r0x2A  key 0, rIdx = 42, size 1
  -- #19 reg = r0x2B  key 0, rIdx = 43, size 1
  -- #20 reg = r0x2C  key 0, rIdx = 44, size 1
  -- #21 reg = r0x2D  key 0, rIdx = 45, size 1
  -- #22 reg = r0x2E  key 0, rIdx = 46, size 1
  -- #23 reg = r0x2F  key 0, rIdx = 47, size 1
  -- #24 reg = r0x30  key 0, rIdx = 48, size 1
  -- #25 reg = r0x31  key 0, rIdx = 49, size 1
  -- #26 reg = r0x32  key 0, rIdx = 50, size 1
  -- #27 reg = r0x33  key 0, rIdx = 51, size 1
  -- #28 reg = r0x34  key 0, rIdx = 52, size 1
  -- #29 reg = r0x35  key 0, rIdx = 53, size 1
  -- #30 reg = r0x36  key 0, rIdx = 54, size 1
  -- #31 reg = r0x37  key 0, rIdx = 55, size 1
  -- #32 reg = r0x38  key 0, rIdx = 56, size 1
  -- #33 reg = r0x39  key 0, rIdx = 57, size 1
  -- #34 reg = r0x3A  key 0, rIdx = 58, size 1
  -- #35 reg = r0x3B  key 0, rIdx = 59, size 1
  -- #36 reg = r0x3C  key 0, rIdx = 60, size 1
  -- #37 reg = r0x3D  key 0, rIdx = 61, size 1
  -- #38 reg = r0x3E  key 0, rIdx = 62, size 1
  -- #39 reg = r0x3F  key 0, rIdx = 63, size 1
  -- #40 reg = r0x40  key 0, rIdx = 64, size 1
  -- #41 reg = r0x41  key 0, rIdx = 65, size 1
  -- #42 reg = r0x42  key 0, rIdx = 66, size 1
  -- #43 reg = r0x43  key 0, rIdx = 67, size 1
  -- #44 reg = r0x44  key 0, rIdx = 68, size 1
  -- #45 reg = r0x45  key 0, rIdx = 69, size 1
  -- #46 reg = r0x46  key 0, rIdx = 70, size 1
  -- #47 reg = r0x47  key 0, rIdx = 71, size 1
  -- #48 reg = r0x48  key 0, rIdx = 72, size 1
  -- #49 reg = r0x49  key 0, rIdx = 73, size 1
  -- #50 reg = r0x4A  key 0, rIdx = 74, size 1
  -- #51 reg = r0x4B  key 0, rIdx = 75, size 1
  -- #52 reg = r0x4C  key 0, rIdx = 76, size 1
  -- #53 reg = r0x4D  key 0, rIdx = 77, size 1
  -- #54 reg = r0x4E  key 0, rIdx = 78, size 1
  -- #55 reg = r0x4F  key 0, rIdx = 79, size 1
  -- #56 reg = r0x50  key 0, rIdx = 80, size 1
  -- #57 reg = r0x51  key 0, rIdx = 81, size 1
  -- #58 reg = r0x52  key 0, rIdx = 82, size 1
  -- #59 reg = r0x53  key 0, rIdx = 83, size 1
  -- #60 reg = r0x54  key 0, rIdx = 84, size 1
  -- #61 reg = r0x55  key 0, rIdx = 85, size 1
  -- #62 reg = r0x56  key 0, rIdx = 86, size 1
  -- #63 reg = r0x57  key 0, rIdx = 87, size 1
  -- #64 reg = r0x58  key 0, rIdx = 88, size 1
  -- #65 reg = r0x59  key 0, rIdx = 89, size 1
  -- #66 reg = r0x5A  key 0, rIdx = 90, size 1
  -- #67 reg = r0x5B  key 0, rIdx = 91, size 1
  -- #68 reg = r0x5C  key 0, rIdx = 92, size 1
  -- #69 reg = r0x5D  key 0, rIdx = 93, size 1
  -- #70 reg = r0x5E  key 0, rIdx = 94, size 1
  -- #71 reg = r0x5F  key 0, rIdx = 95, size 1
  -- #72 reg = r0x60  key 0, rIdx = 96, size 1
  -- #73 reg = r0x61  key 0, rIdx = 97, size 1
  -- #74 reg = r0x62  key 0, rIdx = 98, size 1
  -- #75 reg = _ADRESH  key 0, rIdx = 33, size 1
  -- #76 reg = r0x63  key 0, rIdx = 99, size 1
  -- #77 reg = r0x64  key 0, rIdx = 100, size 1
  -- #78 reg = r0x65  key 0, rIdx = 101, size 1
  -- #79 reg = r0x66  key 0, rIdx = 102, size 1
  -- #80 reg = r0x67  key 0, rIdx = 103, size 1
  -- #81 reg = r0x68  key 0, rIdx = 104, size 1
  -- #82 reg = r0x69  key 0, rIdx = 105, size 1
  -- #83 reg = r0x6A  key 0, rIdx = 106, size 1
  -- #84 reg = r0x6B  key 0, rIdx = 107, size 1
  -- #85 reg = r0x6C  key 0, rIdx = 108, size 1
  -- #86 reg = r0x6D  key 0, rIdx = 109, size 1
  -- #87 reg = r0x6E  key 0, rIdx = 110, size 1
  -- #88 reg = _ADRESL  key 5, rIdx = 34, size 1
  -- #89 reg = _PCLATH  key 6, rIdx = 10, size 1
  -- #90 reg = _TRISA  key 7, rIdx = 1, size 1
  -- #91 reg = _TRISB  key 8, rIdx = 2, size 1
  -- #92 reg = _TRISB  key 8, rIdx = 23, size 1
  -- #93 reg = _TRISB  key 8, rIdx = 32, size 1
  -- #94 reg = _TRISC  key 9, rIdx = 3, size 1
  -- #95 reg = _TRISD  key 10, rIdx = 4, size 1
  -- #96 reg = _PORTA  key 10, rIdx = 6, size 1
  -- #97 reg = _TRISE  key 11, rIdx = 5, size 1
  -- #98 reg = _PORTB  key 11, rIdx = 7, size 1
  -- #99 reg = _PORTB  key 11, rIdx = 22, size 1
  -- #100 reg = _PORTB  key 11, rIdx = 30, size 1
  -- #101 reg = _PORTC  key 12, rIdx = 8, size 1
  -- #102 reg = _GIE  key 12, rIdx = 13, size 1
  -- #103 reg = _PORTD  key 13, rIdx = 9, size 1
  -- #104 reg = _PORTE  key 14, rIdx = 10, size 1
  -- #105 reg = _sensorSample_phase0_1_1  key 15, rIdx = 26, size 1
  -- #106 reg = _sensorSample_phase1_1_1  key 16, rIdx = 27, size 1
  -- #107 reg = _sensorSample_phase2_1_1  key 17, rIdx = 28, size 1
  -- #108 reg = _sensorSample_phase3_1_1  key 18, rIdx = 29, size 1
  -- #109 reg = _ADCON0  key 27, rIdx = 12, size 1
  -- #110 reg = _ADCON0  key 27, rIdx = 24, size 1
  -- #111 reg = _ADCON0  key 27, rIdx = 25, size 1
  -- #112 reg = _ADCON1  key 28, rIdx = 11, size 1
  -- #113 reg = SSAVE  key 32, rIdx = 32765, size 1
  -- #114 reg = WSAVE  key 36, rIdx = 32766, size 1
  -- #115 reg = _reading0  key 44, rIdx = 14, size 2
  -- #116 reg = _GO_DONE  key 44, rIdx = 31, size 1
  -- #117 reg = _reading1  key 45, rIdx = 15, size 2
  -- #118 reg = _reading2  key 46, rIdx = 16, size 2
  -- #119 reg = INTCON  key 47, rIdx = 11, size 1
  -- #120 reg = _reading3  key 47, rIdx = 17, size 2
  -- #121 reg = _STATUS  key 48, rIdx = 3, size 1
  -- #122 reg = PCL  key 48, rIdx = 2, size 1
  -- #123 reg = _reading4  key 48, rIdx = 18, size 2
  -- #124 reg = KZ  key 49, rIdx = 32767, size 1
  -- #125 reg = _reading5  key 49, rIdx = 19, size 2
  -- #126 reg = _reading6  key 50, rIdx = 20, size 2
  -- #127 reg = _reading7  key 51, rIdx = 21, size 2
  -- #128 reg = _SerialTris  key 54, rIdx = 36, size 1
  -- #129 reg = INDF  key 56, rIdx = 0, size 1
  -- #130 reg = _Light0  key 56, rIdx = 0, size 1
  -- #131 reg = _SerialPort  key 57, rIdx = 35, size 1
  -- #132 reg = FSR  key 60, rIdx = 4, size 1
regTypeNum
  2421 - iTemp0
  2431 - itemp register
  reg name iTemp0,  reg type REG_GPR
  2421 - iTemp1
  2421 - iTemp2
  2431 - itemp register
  reg name iTemp2,  reg type REG_GPR
  2421 - iTemp3
  2431 - itemp register
  reg name iTemp3,  reg type REG_GPR
  2421 - iTemp4
  2431 - itemp register
  reg name iTemp4,  reg type REG_GPR
  2421 - iTemp5
  2431 - itemp register
  reg name iTemp5,  reg type REG_GPR
  2421 - iTemp6
  2431 - itemp register
  reg name iTemp6,  reg type REG_GPR
  2421 - iTemp7
  2421 - iTemp8
  2421 - iTemp9
  2431 - itemp register
  reg name iTemp9,  reg type REG_GPR
  2421 - iTemp10
  2431 - itemp register
  reg name iTemp10,  reg type REG_GPR
  2421 - iTemp11
  2431 - itemp register
  reg name iTemp11,  reg type REG_GPR
  2421 - iTemp12
  2431 - itemp register
  reg name iTemp12,  reg type REG_GPR
  2421 - iTemp13
  2431 - itemp register
  reg name iTemp13,  reg type REG_GPR
  2421 - iTemp14
  2421 - iTemp15
  2421 - iTemp16
  2431 - itemp register
  2442 - no reg needed - used as a return
  2421 - iTemp17
  2431 - itemp register
  reg name iTemp17,  reg type REG_GPR
  2421 - iTemp18
  2431 - itemp register
  reg name iTemp18,  reg type REG_GPR
  2421 - iTemp19
  2431 - itemp register
  reg name iTemp19,  reg type REG_GPR
  2421 - iTemp20
  2431 - itemp register
  reg name iTemp20,  reg type REG_GPR
  2421 - iTemp21
  2421 - iTemp22
  2421 - iTemp23
  2431 - itemp register
  2442 - no reg needed - used as a return
  2421 - iTemp24
  2431 - itemp register
  reg name iTemp24,  reg type REG_GPR
  2421 - iTemp25
  2431 - itemp register
  reg name iTemp25,  reg type REG_GPR
  2421 - iTemp26
  2431 - itemp register
  reg name iTemp26,  reg type REG_GPR
  2421 - iTemp27
  2431 - itemp register
  reg name iTemp27,  reg type REG_GPR
  2421 - iTemp28
  2421 - iTemp29
  2421 - iTemp30
  2431 - itemp register
  2442 - no reg needed - used as a return
  2421 - iTemp31
  2431 - itemp register
  reg name iTemp31,  reg type REG_GPR
  2421 - iTemp32
  2431 - itemp register
  reg name iTemp32,  reg type REG_GPR
  2421 - iTemp33
  2431 - itemp register
  reg name iTemp33,  reg type REG_GPR
  2421 - iTemp34
  2431 - itemp register
  reg name iTemp34,  reg type REG_GPR
  2421 - iTemp35
  2421 - iTemp36
  2421 - iTemp37
  2431 - itemp register
  2442 - no reg needed - used as a return
  2421 - iTemp38
  2431 - itemp register
  reg name iTemp38,  reg type REG_GPR
  2421 - iTemp39
  2431 - itemp register
  reg name iTemp39,  reg type REG_GPR
  2421 - iTemp40
  2431 - itemp register
  reg name iTemp40,  reg type REG_GPR
  2421 - iTemp41
  2431 - itemp register
  reg name iTemp41,  reg type REG_GPR
  2421 - iTemp42
  2421 - iTemp43
  2421 - iTemp44
  2431 - itemp register
  2442 - no reg needed - used as a return
  2421 - iTemp45
  2431 - itemp register
  reg name iTemp45,  reg type REG_GPR
  2421 - iTemp46
  2431 - itemp register
  reg name iTemp46,  reg type REG_GPR
  2421 - iTemp47
  2431 - itemp register
  reg name iTemp47,  reg type REG_GPR
  2421 - iTemp48
  2431 - itemp register
  reg name iTemp48,  reg type REG_GPR
  2421 - iTemp49
  2421 - iTemp50
  2421 - iTemp51
  2431 - itemp register
  2442 - no reg needed - used as a return
  2421 - iTemp52
  2431 - itemp register
  reg name iTemp52,  reg type REG_GPR
  2421 - iTemp53
  2431 - itemp register
  reg name iTemp53,  reg type REG_GPR
  2421 - iTemp54
  2431 - itemp register
  reg name iTemp54,  reg type REG_GPR
  2421 - iTemp55
  2431 - itemp register
  reg name iTemp55,  reg type REG_GPR
  2421 - iTemp56
  2421 - iTemp57
  2421 - iTemp58
  2431 - itemp register
  2442 - no reg needed - used as a return
  2421 - iTemp59
  2431 - itemp register
  reg name iTemp59,  reg type REG_GPR
  2421 - iTemp60
serialRegAssign
  op: LABEL
deassignLRs
  op: unkown op 9 	
deassignLRs
  op: SEND
deassignLRs
  op: CALL
deassignLRs
  op: RIGHT_OP
deassignLRs
willCauseSpill
computeSpillable
  2125 - 
bitvector Size = 74 bSize = 10
Bits on { (9) }
getRegGpr
allocReg of type REG_GPR
getRegGpr
allocReg of type REG_GPR
  2141 - 
  2155 - 
  op: CAST
deassignLRs
nfreeRegsType
  op: CAST
deassignLRs
willCauseSpill
computeSpillable
  2125 - 
bitvector Size = 74 bSize = 10
Bits on { (6) (11) }
getRegGpr
allocReg of type REG_GPR
  2141 - 
  2155 - 
  op: SEND
deassignLRs
  op: CALL
deassignLRs
  op: SEND
deassignLRs
  op: CALL
deassignLRs
  op: +
deassignLRs
willCauseSpill
computeSpillable
  2125 - 
bitvector Size = 74 bSize = 10
Bits on { (2) (6) (11) }
getRegGpr
allocReg of type REG_GPR
  2141 - 
positionRegs
positionRegs
  2155 - 
  op: RIGHT_OP
deassignLRs
willCauseSpill
computeSpillable
  2125 - 
bitvector Size = 74 bSize = 10
Bits on { (2) (6) (11) (19) }
getRegGpr
allocReg of type REG_GPR
getRegGpr
allocReg of type REG_GPR
  2141 - 
  2155 - 
  op: CAST
deassignLRs
freeReg
freeReg
  op: CAST
deassignLRs
  op: SEND
deassignLRs
  op: CALL
deassignLRs
  op: SEND
deassignLRs
  op: CALL
deassignLRs
  op: +
deassignLRs
  op: +
deassignLRs
  op: RIGHT_OP
deassignLRs
willCauseSpill
computeSpillable
  2125 - 
bitvector Size = 74 bSize = 10
Bits on { (2) (6) (11) (27) }
getRegGpr
allocReg of type REG_GPR
getRegGpr
allocReg of type REG_GPR
  2141 - 
  2155 - 
  op: CAST
deassignLRs
freeReg
freeReg
  op: CAST
deassignLRs
  op: SEND
deassignLRs
  op: CALL
deassignLRs
  op: SEND
deassignLRs
  op: CALL
deassignLRs
  op: +
deassignLRs
  op: +
deassignLRs
  op: RIGHT_OP
deassignLRs
willCauseSpill
computeSpillable
  2125 - 
bitvector Size = 74 bSize = 10
Bits on { (2) (6) (11) (35) }
getRegGpr
allocReg of type REG_GPR
getRegGpr
allocReg of type REG_GPR
  2141 - 
  2155 - 
  op: CAST
deassignLRs
freeReg
freeReg
  op: CAST
deassignLRs
  op: SEND
deassignLRs
  op: CALL
deassignLRs
  op: SEND
deassignLRs
  op: CALL
deassignLRs
  op: +
deassignLRs
  op: +
deassignLRs
  op: RIGHT_OP
deassignLRs
willCauseSpill
computeSpillable
  2125 - 
bitvector Size = 74 bSize = 10
Bits on { (2) (6) (11) (43) }
getRegGpr
allocReg of type REG_GPR
getRegGpr
allocReg of type REG_GPR
  2141 - 
  2155 - 
  op: CAST
deassignLRs
freeReg
freeReg
  op: CAST
deassignLRs
  op: SEND
deassignLRs
  op: CALL
deassignLRs
  op: SEND
deassignLRs
  op: CALL
deassignLRs
  op: +
deassignLRs
  op: +
deassignLRs
  op: RIGHT_OP
deassignLRs
willCauseSpill
computeSpillable
  2125 - 
bitvector Size = 74 bSize = 10
Bits on { (2) (6) (11) (51) }
getRegGpr
allocReg of type REG_GPR
getRegGpr
allocReg of type REG_GPR
  2141 - 
  2155 - 
  op: CAST
deassignLRs
freeReg
freeReg
  op: CAST
deassignLRs
  op: SEND
deassignLRs
  op: CALL
deassignLRs
  op: SEND
deassignLRs
  op: CALL
deassignLRs
  op: +
deassignLRs
  op: +
deassignLRs
  op: RIGHT_OP
deassignLRs
willCauseSpill
computeSpillable
  2125 - 
bitvector Size = 74 bSize = 10
Bits on { (2) (6) (11) (59) }
getRegGpr
allocReg of type REG_GPR
getRegGpr
allocReg of type REG_GPR
  2141 - 
  2155 - 
  op: CAST
deassignLRs
freeReg
freeReg
  op: CAST
deassignLRs
  op: SEND
deassignLRs
  op: CALL
deassignLRs
  op: SEND
deassignLRs
  op: CALL
deassignLRs
  op: +
deassignLRs
  op: +
deassignLRs
  op: RIGHT_OP
deassignLRs
willCauseSpill
computeSpillable
  2125 - 
bitvector Size = 74 bSize = 10
Bits on { (2) (6) (11) (67) }
getRegGpr
allocReg of type REG_GPR
getRegGpr
allocReg of type REG_GPR
  2141 - 
  2155 - 
  op: CAST
deassignLRs
freeReg
freeReg
  op: CAST
deassignLRs
  op: SEND
deassignLRs
  op: CALL
deassignLRs
  op: SEND
deassignLRs
  op: CALL
deassignLRs
  op: +
deassignLRs
freeReg
freeReg
  op: +
deassignLRs
  op: SEND
deassignLRs
freeReg
  op: CALL
deassignLRs
  op: LABEL
deassignLRs
  op: ENDFUNCTION
deassignLRs
createRegMask
regsUsedIniCode
rUmaskForOp
regsUsedIniCode
rUmaskForOp
rUmaskForOp
regsUsedIniCode
rUmaskForOp
rUmaskForOp
rUmaskForOp
regsUsedIniCode
rUmaskForOp
rUmaskForOp
rUmaskForOp
regsUsedIniCode
rUmaskForOp
rUmaskForOp
rUmaskForOp
regsUsedIniCode
rUmaskForOp
regsUsedIniCode
rUmaskForOp
rUmaskForOp
regsUsedIniCode
rUmaskForOp
regsUsedIniCode
rUmaskForOp
rUmaskForOp
regsUsedIniCode
rUmaskForOp
rUmaskForOp
rUmaskForOp
regsUsedIniCode
rUmaskForOp
rUmaskForOp
rUmaskForOp
regsUsedIniCode
rUmaskForOp
rUmaskForOp
rUmaskForOp
regsUsedIniCode
rUmaskForOp
rUmaskForOp
rUmaskForOp
regsUsedIniCode
rUmaskForOp
regsUsedIniCode
rUmaskForOp
rUmaskForOp
regsUsedIniCode
rUmaskForOp
regsUsedIniCode
rUmaskForOp
rUmaskForOp
regsUsedIniCode
rUmaskForOp
rUmaskForOp
rUmaskForOp
regsUsedIniCode
rUmaskForOp
rUmaskForOp
rUmaskForOp
regsUsedIniCode
rUmaskForOp
rUmaskForOp
rUmaskForOp
regsUsedIniCode
rUmaskForOp
rUmaskForOp
rUmaskForOp
regsUsedIniCode
rUmaskForOp
rUmaskForOp
rUmaskForOp
regsUsedIniCode
rUmaskForOp
regsUsedIniCode
rUmaskForOp
rUmaskForOp
regsUsedIniCode
rUmaskForOp
regsUsedIniCode
rUmaskForOp
rUmaskForOp
regsUsedIniCode
rUmaskForOp
rUmaskForOp
rUmaskForOp
regsUsedIniCode
rUmaskForOp
rUmaskForOp
rUmaskForOp
regsUsedIniCode
rUmaskForOp
rUmaskForOp
rUmaskForOp
regsUsedIniCode
rUmaskForOp
rUmaskForOp
rUmaskForOp
regsUsedIniCode
rUmaskForOp
rUmaskForOp
rUmaskForOp
regsUsedIniCode
rUmaskForOp
regsUsedIniCode
rUmaskForOp
rUmaskForOp
regsUsedIniCode
rUmaskForOp
regsUsedIniCode
rUmaskForOp
rUmaskForOp
regsUsedIniCode
rUmaskForOp
rUmaskForOp
rUmaskForOp
regsUsedIniCode
rUmaskForOp
rUmaskForOp
rUmaskForOp
regsUsedIniCode
rUmaskForOp
rUmaskForOp
rUmaskForOp
regsUsedIniCode
rUmaskForOp
rUmaskForOp
rUmaskForOp
regsUsedIniCode
rUmaskForOp
rUmaskForOp
rUmaskForOp
regsUsedIniCode
rUmaskForOp
regsUsedIniCode
rUmaskForOp
rUmaskForOp
regsUsedIniCode
rUmaskForOp
regsUsedIniCode
rUmaskForOp
rUmaskForOp
regsUsedIniCode
rUmaskForOp
rUmaskForOp
rUmaskForOp
regsUsedIniCode
rUmaskForOp
rUmaskForOp
rUmaskForOp
regsUsedIniCode
rUmaskForOp
rUmaskForOp
rUmaskForOp
regsUsedIniCode
rUmaskForOp
rUmaskForOp
rUmaskForOp
regsUsedIniCode
rUmaskForOp
rUmaskForOp
rUmaskForOp
regsUsedIniCode
rUmaskForOp
regsUsedIniCode
rUmaskForOp
rUmaskForOp
regsUsedIniCode
rUmaskForOp
regsUsedIniCode
rUmaskForOp
rUmaskForOp
regsUsedIniCode
rUmaskForOp
rUmaskForOp
rUmaskForOp
regsUsedIniCode
rUmaskForOp
rUmaskForOp
rUmaskForOp
regsUsedIniCode
rUmaskForOp
rUmaskForOp
rUmaskForOp
regsUsedIniCode
rUmaskForOp
rUmaskForOp
rUmaskForOp
regsUsedIniCode
rUmaskForOp
rUmaskForOp
rUmaskForOp
regsUsedIniCode
rUmaskForOp
regsUsedIniCode
rUmaskForOp
rUmaskForOp
regsUsedIniCode
rUmaskForOp
regsUsedIniCode
rUmaskForOp
rUmaskForOp
regsUsedIniCode
rUmaskForOp
rUmaskForOp
rUmaskForOp
regsUsedIniCode
rUmaskForOp
rUmaskForOp
rUmaskForOp
regsUsedIniCode
rUmaskForOp
rUmaskForOp
rUmaskForOp
regsUsedIniCode
rUmaskForOp
rUmaskForOp
rUmaskForOp
regsUsedIniCode
rUmaskForOp
rUmaskForOp
rUmaskForOp
regsUsedIniCode
rUmaskForOp
regsUsedIniCode
rUmaskForOp
rUmaskForOp
regsUsedIniCode
rUmaskForOp
regsUsedIniCode
rUmaskForOp
rUmaskForOp
regsUsedIniCode
rUmaskForOp
rUmaskForOp
rUmaskForOp
regsUsedIniCode
rUmaskForOp
rUmaskForOp
rUmaskForOp
regsUsedIniCode
rUmaskForOp
regsUsedIniCode
rUmaskForOp
rUmaskForOp
ebbs after optimizing:

----------------------------------------------------------------
Basic Block _entry : loop Depth = 0 noPath = 0 , lastinLoop = 0
depth 1st num 1 : bbnum = 0 1st iCode = 1 , last iCode = 102
visited 0 : hasFcall = 1

defines bitVector :bitvector Size = 105 bSize = 14
Bits on { (5) (6) (8) (14) (15) (17) (18) (20) (26) (27) (29) (30) (32) (38) (39) (41) (42) (44) (50) (51) (53) (54) (56) (62) (63) (65) (66) (68) (74) (75) (77) (78) (80) (86) (87) (89) (90) (91) (92) (93) (98) (99) (100) }

local defines bitVector :bitvector Size = 105 bSize = 14
Bits on { (4) (5) (6) (7) (8) (9) (11) (13) (14) (15) (16) (17) (18) (19) (20) (21) (23) (25) (26) (27) (28) (29) (30) (31) (32) (33) (35) (37) (38) (39) (40) (41) (42) (43) (44) (45) (47) (49) (50) (51) (52) (53) (54) (55) (56) (57) (59) (61) (62) (63) (64) (65) (66) (67) (68) (69) (71) (73) (74) (75) (76) (77) (78) (79) (80) (81) (83) (85) (86) (87) (88) (89) (90) (91) (92) (93) (95) (97) (98) (99) (100) (102) }

pointers Set bitvector :
in pointers Set bitvector :
inDefs Set bitvector :
outDefs Set bitvector :bitvector Size = 105 bSize = 14
Bits on { (4) (5) (6) (8) (11) (13) (14) (15) (17) (18) (20) (23) (25) (26) (27) (29) (30) (32) (35) (37) (38) (39) (41) (42) (44) (47) (49) (50) (51) (53) (54) (56) (59) (61) (62) (63) (65) (66) (68) (71) (73) (74) (75) (77) (78) (80) (83) (85) (86) (87) (89) (90) (91) (92) (93) (95) (97) (98) (99) (100) (102) }

usesDefs Set bitvector :bitvector Size = 105 bSize = 14
Bits on { (4) (5) (6) (7) (8) (9) (11) (13) (14) (15) (16) (17) (18) (19) (20) (21) (23) (25) (26) (27) (28) (29) (30) (31) (32) (33) (35) (37) (38) (39) (40) (41) (42) (43) (44) (45) (47) (49) (50) (51) (52) (53) (54) (55) (56) (57) (59) (61) (62) (63) (64) (65) (66) (67) (68) (69) (71) (73) (74) (75) (76) (77) (78) (79) (80) (81) (83) (85) (86) (87) (88) (89) (90) (91) (92) (93) (95) (97) (98) (99) (100) (102) }

----------------------------------------------------------------
main.c(267:1:0:0:0)	 _entry($2) :
main.c(267:2:1:0:0)		proc _sendPacket [k1 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{void function    }
main.c(272:3:3:0:0)		send 0x80 {literal unsigned char}{argreg = 1}
main.c(272:4:4:0:0)		iTemp1 [k5 lr4:4 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{void} = call _putc [k4 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{void function    }
main.c(274:5:5:0:0)		iTemp3 [k9 lr5:6 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int}[r0x6F r0x70 ] = _reading0 [k8 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int} >> 0x8 {literal unsigned char}
main.c(274:6:6:0:0)		iTemp2 [k6 lr6:98 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sendPacket_h_1_1}[r0x6F ] = (unsigned char)iTemp3 [k9 lr5:6 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int}[r0x6F r0x70 ]
main.c(275:8:8:0:0)		iTemp5 [k11 lr8:98 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sendPacket_l_1_1}[r0x71 ] = (unsigned char)_reading0 [k8 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int}
main.c(276:10:10:0:0)		send iTemp2 [k6 lr6:98 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sendPacket_h_1_1}[r0x6F ]{argreg = 1}
main.c(276:11:11:0:0)		iTemp7 [k14 lr11:11 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{void} = call _putc [k4 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{void function    }
main.c(277:12:12:0:0)		send iTemp5 [k11 lr8:98 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sendPacket_l_1_1}[r0x71 ]{argreg = 1}
main.c(277:13:13:0:0)		iTemp8 [k15 lr13:13 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{void} = call _putc [k4 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{void function    }
main.c(278:14:14:0:0)		iTemp0 [k2 lr14:101 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sendPacket_checksum_1_1}[r0x72 ] = iTemp2 [k6 lr6:98 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sendPacket_h_1_1}[r0x6F ] + iTemp5 [k11 lr8:98 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sendPacket_l_1_1}[r0x71 ]
main.c(280:17:17:0:0)		iTemp11 [k19 lr17:18 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int}[r0x73 r0x74 ] = _reading1 [k18 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int} >> 0x8 {literal unsigned char}
main.c(280:18:18:0:0)		iTemp2 [k6 lr6:98 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sendPacket_h_1_1}[r0x6F ] = (unsigned char)iTemp11 [k19 lr17:18 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int}[r0x73 r0x74 ]
main.c(281:20:20:0:0)		iTemp5 [k11 lr8:98 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sendPacket_l_1_1}[r0x71 ] = (unsigned char)_reading1 [k18 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int}
main.c(282:22:22:0:0)		send iTemp2 [k6 lr6:98 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sendPacket_h_1_1}[r0x6F ]{argreg = 1}
main.c(282:23:23:0:0)		iTemp14 [k22 lr23:23 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{void} = call _putc [k4 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{void function    }
main.c(283:24:24:0:0)		send iTemp5 [k11 lr8:98 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sendPacket_l_1_1}[r0x71 ]{argreg = 1}
main.c(283:25:25:0:0)		iTemp15 [k23 lr25:25 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{void} = call _putc [k4 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{void function    }
main.c(284:26:26:0:0)		iTemp16 [k24 lr26:27 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{unsigned char} = iTemp2 [k6 lr6:98 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sendPacket_h_1_1}[r0x6F ] + iTemp5 [k11 lr8:98 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sendPacket_l_1_1}[r0x71 ]
main.c(284:27:27:0:0)		iTemp0 [k2 lr14:101 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sendPacket_checksum_1_1}[r0x72 ] = iTemp0 [k2 lr14:101 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sendPacket_checksum_1_1}[r0x72 ] + iTemp16 [k24 lr26:27 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{unsigned char}
main.c(286:29:29:0:0)		iTemp18 [k27 lr29:30 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int}[r0x75 r0x76 ] = _reading2 [k26 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int} >> 0x8 {literal unsigned char}
main.c(286:30:30:0:0)		iTemp2 [k6 lr6:98 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sendPacket_h_1_1}[r0x6F ] = (unsigned char)iTemp18 [k27 lr29:30 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int}[r0x75 r0x76 ]
main.c(287:32:32:0:0)		iTemp5 [k11 lr8:98 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sendPacket_l_1_1}[r0x71 ] = (unsigned char)_reading2 [k26 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int}
main.c(288:34:34:0:0)		send iTemp2 [k6 lr6:98 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sendPacket_h_1_1}[r0x6F ]{argreg = 1}
main.c(288:35:35:0:0)		iTemp21 [k30 lr35:35 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{void} = call _putc [k4 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{void function    }
main.c(289:36:36:0:0)		send iTemp5 [k11 lr8:98 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sendPacket_l_1_1}[r0x71 ]{argreg = 1}
main.c(289:37:37:0:0)		iTemp22 [k31 lr37:37 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{void} = call _putc [k4 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{void function    }
main.c(290:38:38:0:0)		iTemp23 [k32 lr38:39 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{unsigned char} = iTemp2 [k6 lr6:98 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sendPacket_h_1_1}[r0x6F ] + iTemp5 [k11 lr8:98 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sendPacket_l_1_1}[r0x71 ]
main.c(290:39:39:0:0)		iTemp0 [k2 lr14:101 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sendPacket_checksum_1_1}[r0x72 ] = iTemp0 [k2 lr14:101 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sendPacket_checksum_1_1}[r0x72 ] + iTemp23 [k32 lr38:39 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{unsigned char}
main.c(292:41:41:0:0)		iTemp25 [k35 lr41:42 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int}[r0x77 r0x78 ] = _reading3 [k34 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int} >> 0x8 {literal unsigned char}
main.c(292:42:42:0:0)		iTemp2 [k6 lr6:98 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sendPacket_h_1_1}[r0x6F ] = (unsigned char)iTemp25 [k35 lr41:42 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int}[r0x77 r0x78 ]
main.c(293:44:44:0:0)		iTemp5 [k11 lr8:98 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sendPacket_l_1_1}[r0x71 ] = (unsigned char)_reading3 [k34 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int}
main.c(294:46:46:0:0)		send iTemp2 [k6 lr6:98 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sendPacket_h_1_1}[r0x6F ]{argreg = 1}
main.c(294:47:47:0:0)		iTemp28 [k38 lr47:47 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{void} = call _putc [k4 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{void function    }
main.c(295:48:48:0:0)		send iTemp5 [k11 lr8:98 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sendPacket_l_1_1}[r0x71 ]{argreg = 1}
main.c(295:49:49:0:0)		iTemp29 [k39 lr49:49 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{void} = call _putc [k4 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{void function    }
main.c(296:50:50:0:0)		iTemp30 [k40 lr50:51 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{unsigned char} = iTemp2 [k6 lr6:98 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sendPacket_h_1_1}[r0x6F ] + iTemp5 [k11 lr8:98 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sendPacket_l_1_1}[r0x71 ]
main.c(296:51:51:0:0)		iTemp0 [k2 lr14:101 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sendPacket_checksum_1_1}[r0x72 ] = iTemp0 [k2 lr14:101 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sendPacket_checksum_1_1}[r0x72 ] + iTemp30 [k40 lr50:51 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{unsigned char}
main.c(298:53:53:0:0)		iTemp32 [k43 lr53:54 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int}[r0x79 r0x7A ] = _reading4 [k42 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int} >> 0x8 {literal unsigned char}
main.c(298:54:54:0:0)		iTemp2 [k6 lr6:98 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sendPacket_h_1_1}[r0x6F ] = (unsigned char)iTemp32 [k43 lr53:54 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int}[r0x79 r0x7A ]
main.c(299:56:56:0:0)		iTemp5 [k11 lr8:98 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sendPacket_l_1_1}[r0x71 ] = (unsigned char)_reading4 [k42 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int}
main.c(300:58:58:0:0)		send iTemp2 [k6 lr6:98 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sendPacket_h_1_1}[r0x6F ]{argreg = 1}
main.c(300:59:59:0:0)		iTemp35 [k46 lr59:59 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{void} = call _putc [k4 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{void function    }
main.c(301:60:60:0:0)		send iTemp5 [k11 lr8:98 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sendPacket_l_1_1}[r0x71 ]{argreg = 1}
main.c(301:61:61:0:0)		iTemp36 [k47 lr61:61 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{void} = call _putc [k4 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{void function    }
main.c(302:62:62:0:0)		iTemp37 [k48 lr62:63 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{unsigned char} = iTemp2 [k6 lr6:98 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sendPacket_h_1_1}[r0x6F ] + iTemp5 [k11 lr8:98 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sendPacket_l_1_1}[r0x71 ]
main.c(302:63:63:0:0)		iTemp0 [k2 lr14:101 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sendPacket_checksum_1_1}[r0x72 ] = iTemp0 [k2 lr14:101 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sendPacket_checksum_1_1}[r0x72 ] + iTemp37 [k48 lr62:63 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{unsigned char}
main.c(304:65:65:0:0)		iTemp39 [k51 lr65:66 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int}[r0x7B r0x7C ] = _reading5 [k50 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int} >> 0x8 {literal unsigned char}
main.c(304:66:66:0:0)		iTemp2 [k6 lr6:98 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sendPacket_h_1_1}[r0x6F ] = (unsigned char)iTemp39 [k51 lr65:66 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int}[r0x7B r0x7C ]
main.c(305:68:68:0:0)		iTemp5 [k11 lr8:98 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sendPacket_l_1_1}[r0x71 ] = (unsigned char)_reading5 [k50 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int}
main.c(306:70:70:0:0)		send iTemp2 [k6 lr6:98 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sendPacket_h_1_1}[r0x6F ]{argreg = 1}
main.c(306:71:71:0:0)		iTemp42 [k54 lr71:71 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{void} = call _putc [k4 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{void function    }
main.c(307:72:72:0:0)		send iTemp5 [k11 lr8:98 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sendPacket_l_1_1}[r0x71 ]{argreg = 1}
main.c(307:73:73:0:0)		iTemp43 [k55 lr73:73 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{void} = call _putc [k4 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{void function    }
main.c(308:74:74:0:0)		iTemp44 [k56 lr74:75 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{unsigned char} = iTemp2 [k6 lr6:98 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sendPacket_h_1_1}[r0x6F ] + iTemp5 [k11 lr8:98 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sendPacket_l_1_1}[r0x71 ]
main.c(308:75:75:0:0)		iTemp0 [k2 lr14:101 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sendPacket_checksum_1_1}[r0x72 ] = iTemp0 [k2 lr14:101 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sendPacket_checksum_1_1}[r0x72 ] + iTemp44 [k56 lr74:75 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{unsigned char}
main.c(310:77:77:0:0)		iTemp46 [k59 lr77:78 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int}[r0x7D r0x7E ] = _reading6 [k58 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int} >> 0x8 {literal unsigned char}
main.c(310:78:78:0:0)		iTemp2 [k6 lr6:98 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sendPacket_h_1_1}[r0x6F ] = (unsigned char)iTemp46 [k59 lr77:78 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int}[r0x7D r0x7E ]
main.c(311:80:80:0:0)		iTemp5 [k11 lr8:98 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sendPacket_l_1_1}[r0x71 ] = (unsigned char)_reading6 [k58 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int}
main.c(312:82:82:0:0)		send iTemp2 [k6 lr6:98 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sendPacket_h_1_1}[r0x6F ]{argreg = 1}
main.c(312:83:83:0:0)		iTemp49 [k62 lr83:83 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{void} = call _putc [k4 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{void function    }
main.c(313:84:84:0:0)		send iTemp5 [k11 lr8:98 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sendPacket_l_1_1}[r0x71 ]{argreg = 1}
main.c(313:85:85:0:0)		iTemp50 [k63 lr85:85 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{void} = call _putc [k4 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{void function    }
main.c(314:86:86:0:0)		iTemp51 [k64 lr86:87 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{unsigned char} = iTemp2 [k6 lr6:98 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sendPacket_h_1_1}[r0x6F ] + iTemp5 [k11 lr8:98 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sendPacket_l_1_1}[r0x71 ]
main.c(314:87:87:0:0)		iTemp0 [k2 lr14:101 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sendPacket_checksum_1_1}[r0x72 ] = iTemp0 [k2 lr14:101 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sendPacket_checksum_1_1}[r0x72 ] + iTemp51 [k64 lr86:87 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{unsigned char}
main.c(316:89:89:0:0)		iTemp53 [k67 lr89:90 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int}[r0x7F r0x80 ] = _reading7 [k66 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int} >> 0x8 {literal unsigned char}
main.c(316:90:90:0:0)		iTemp2 [k6 lr6:98 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sendPacket_h_1_1}[r0x6F ] = (unsigned char)iTemp53 [k67 lr89:90 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int}[r0x7F r0x80 ]
main.c(317:92:92:0:0)		iTemp5 [k11 lr8:98 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sendPacket_l_1_1}[r0x71 ] = (unsigned char)_reading7 [k66 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int}
main.c(318:94:94:0:0)		send iTemp2 [k6 lr6:98 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sendPacket_h_1_1}[r0x6F ]{argreg = 1}
main.c(318:95:95:0:0)		iTemp56 [k70 lr95:95 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{void} = call _putc [k4 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{void function    }
main.c(319:96:96:0:0)		send iTemp5 [k11 lr8:98 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sendPacket_l_1_1}[r0x71 ]{argreg = 1}
main.c(319:97:97:0:0)		iTemp57 [k71 lr97:97 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{void} = call _putc [k4 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{void function    }
main.c(320:98:98:0:0)		iTemp58 [k72 lr98:99 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{unsigned char} = iTemp2 [k6 lr6:98 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sendPacket_h_1_1}[r0x6F ] + iTemp5 [k11 lr8:98 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sendPacket_l_1_1}[r0x71 ]
main.c(320:99:99:0:0)		iTemp0 [k2 lr14:101 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sendPacket_checksum_1_1}[r0x72 ] = iTemp0 [k2 lr14:101 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sendPacket_checksum_1_1}[r0x72 ] + iTemp58 [k72 lr98:99 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{unsigned char}
main.c(323:101:101:0:0)		send iTemp0 [k2 lr14:101 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sendPacket_checksum_1_1}[r0x72 ]{argreg = 1}
main.c(323:102:102:0:0)		iTemp60 [k74 lr102:102 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{void} = call _putc [k4 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{void function    }
main.c(323:103:103:0:0)	 _return($1) :
main.c(323:104:104:0:0)		eproc _sendPacket [k1 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{void function    }

----------------------------------------------------------------
Basic Block _return : loop Depth = 0 noPath = 0 , lastinLoop = 0
depth 1st num 2 : bbnum = 1 1st iCode = 103 , last iCode = 104
visited 0 : hasFcall = 0

defines bitVector :bitvector Size = 105 bSize = 14
Bits on { }

local defines bitVector :
pointers Set bitvector :
in pointers Set bitvector :
inDefs Set bitvector :bitvector Size = 105 bSize = 14
Bits on { (4) (5) (6) (8) (11) (13) (14) (15) (17) (18) (20) (23) (25) (26) (27) (29) (30) (32) (35) (37) (38) (39) (41) (42) (44) (47) (49) (50) (51) (53) (54) (56) (59) (61) (62) (63) (65) (66) (68) (71) (73) (74) (75) (77) (78) (80) (83) (85) (86) (87) (89) (90) (91) (92) (93) (95) (97) (98) (99) (100) (102) }

outDefs Set bitvector :bitvector Size = 105 bSize = 14
Bits on { (4) (5) (6) (8) (11) (13) (14) (15) (17) (18) (20) (23) (25) (26) (27) (29) (30) (32) (35) (37) (38) (39) (41) (42) (44) (47) (49) (50) (51) (53) (54) (56) (59) (61) (62) (63) (65) (66) (68) (71) (73) (74) (75) (77) (78) (80) (83) (85) (86) (87) (89) (90) (91) (92) (93) (95) (97) (98) (99) (100) (102) }

usesDefs Set bitvector :bitvector Size = 105 bSize = 14
Bits on { }

----------------------------------------------------------------
main.c(323:103:103:0:0)	 _return($1) :
main.c(323:104:104:0:0)		eproc _sendPacket [k1 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{void function    }
typeRegWithIdx - requesting index = 0x6f
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x70
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x70
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6f
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6f
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x71
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6f
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x71
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x71
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6f
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x72
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x73
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x74
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x74
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x73
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6f
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x71
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6f
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x71
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x71
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6f
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x72
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x75
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x76
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x76
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x75
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6f
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x71
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6f
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x71
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x71
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6f
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x72
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x77
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x78
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x78
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x77
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6f
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x71
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6f
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x71
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x71
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6f
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x72
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x79
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x7a
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x7a
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x79
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6f
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x71
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6f
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x71
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x71
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6f
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x72
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x7b
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x7c
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x7c
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x7b
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6f
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x71
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6f
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x71
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x71
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6f
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x72
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x7d
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x7e
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x7e
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x7d
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6f
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x71
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6f
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x71
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x71
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6f
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x72
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x7f
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x80
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x80
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x7f
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6f
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x71
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6f
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x71
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x71
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6f
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x72
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x72
Found a Dynamic Register!
leaving
<><><><><><><><><><><><><><><><><>
typeRegWithIdx - requesting index = 0x6f
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x70
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6f
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6f
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6f
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x70
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6f
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x71
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6f
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x71
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6f
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x71
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x73
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x74
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6f
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x73
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6f
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x71
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6f
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x71
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6f
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x71
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x75
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x76
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6f
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x75
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6f
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x71
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6f
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x71
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6f
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x71
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x77
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x78
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6f
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x77
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6f
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x71
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6f
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x71
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6f
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x71
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x79
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x7a
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6f
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x79
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6f
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x71
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6f
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x71
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6f
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x71
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x7b
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x7c
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6f
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x7b
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6f
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x71
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6f
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x71
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6f
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x71
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x7d
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x7e
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6f
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x7d
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6f
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x71
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6f
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x71
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6f
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x71
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x7f
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x80
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6f
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x7f
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6f
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x71
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6f
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x71
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6f
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x71
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
Found a Stack Register!
typeRegWithIdx - requesting index = 0x63
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
Found a Stack Register!
typeRegWithIdx - requesting index = 0x63
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x1000
typeRegWithIdx - requesting index = 0x1000
typeRegWithIdx - requesting index = 0x1000
Found a Stack Register!
typeRegWithIdx - requesting index = 0x64
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x64
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
Found a Stack Register!
typeRegWithIdx - requesting index = 0x1000
typeRegWithIdx - requesting index = 0x1000
typeRegWithIdx - requesting index = 0x1000
Found a Stack Register!
typeRegWithIdx - requesting index = 0x64
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x1000
typeRegWithIdx - requesting index = 0x1000
typeRegWithIdx - requesting index = 0x1000
Found a Stack Register!
typeRegWithIdx - requesting index = 0x64
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x65
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x1000
typeRegWithIdx - requesting index = 0x1000
typeRegWithIdx - requesting index = 0x1000
Found a Stack Register!
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
Found a Stack Register!
typeRegWithIdx - requesting index = 0x63
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
Found a Stack Register!
typeRegWithIdx - requesting index = 0x63
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x1000
typeRegWithIdx - requesting index = 0x1000
typeRegWithIdx - requesting index = 0x1000
Found a Stack Register!
typeRegWithIdx - requesting index = 0x64
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x64
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
Found a Stack Register!
typeRegWithIdx - requesting index = 0x1000
typeRegWithIdx - requesting index = 0x1000
typeRegWithIdx - requesting index = 0x1000
Found a Stack Register!
typeRegWithIdx - requesting index = 0x64
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x1000
typeRegWithIdx - requesting index = 0x1000
typeRegWithIdx - requesting index = 0x1000
Found a Stack Register!
typeRegWithIdx - requesting index = 0x64
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x65
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x1000
typeRegWithIdx - requesting index = 0x1000
typeRegWithIdx - requesting index = 0x1000
Found a Stack Register!
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
Found a Stack Register!
typeRegWithIdx - requesting index = 0x63
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
Found a Stack Register!
typeRegWithIdx - requesting index = 0x63
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x1000
typeRegWithIdx - requesting index = 0x1000
typeRegWithIdx - requesting index = 0x1000
Found a Stack Register!
typeRegWithIdx - requesting index = 0x64
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x64
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
Found a Stack Register!
typeRegWithIdx - requesting index = 0x1000
typeRegWithIdx - requesting index = 0x1000
typeRegWithIdx - requesting index = 0x1000
Found a Stack Register!
typeRegWithIdx - requesting index = 0x64
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x1000
typeRegWithIdx - requesting index = 0x1000
typeRegWithIdx - requesting index = 0x1000
Found a Stack Register!
typeRegWithIdx - requesting index = 0x64
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x65
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x1000
typeRegWithIdx - requesting index = 0x1000
typeRegWithIdx - requesting index = 0x1000
Found a Stack Register!
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
Found a Stack Register!
typeRegWithIdx - requesting index = 0x63
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
Found a Stack Register!
typeRegWithIdx - requesting index = 0x63
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x1000
typeRegWithIdx - requesting index = 0x1000
typeRegWithIdx - requesting index = 0x1000
Found a Stack Register!
typeRegWithIdx - requesting index = 0x64
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x64
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
Found a Stack Register!
typeRegWithIdx - requesting index = 0x1000
typeRegWithIdx - requesting index = 0x1000
typeRegWithIdx - requesting index = 0x1000
Found a Stack Register!
typeRegWithIdx - requesting index = 0x64
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x1000
typeRegWithIdx - requesting index = 0x1000
typeRegWithIdx - requesting index = 0x1000
Found a Stack Register!
typeRegWithIdx - requesting index = 0x64
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x65
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x1000
typeRegWithIdx - requesting index = 0x1000
typeRegWithIdx - requesting index = 0x1000
Found a Stack Register!
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
Found a Stack Register!
typeRegWithIdx - requesting index = 0x63
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
Found a Stack Register!
typeRegWithIdx - requesting index = 0x63
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x1000
typeRegWithIdx - requesting index = 0x1000
typeRegWithIdx - requesting index = 0x1000
Found a Stack Register!
typeRegWithIdx - requesting index = 0x64
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x64
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
Found a Stack Register!
typeRegWithIdx - requesting index = 0x1000
typeRegWithIdx - requesting index = 0x1000
typeRegWithIdx - requesting index = 0x1000
Found a Stack Register!
typeRegWithIdx - requesting index = 0x64
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x1000
typeRegWithIdx - requesting index = 0x1000
typeRegWithIdx - requesting index = 0x1000
Found a Stack Register!
typeRegWithIdx - requesting index = 0x64
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x65
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x1000
typeRegWithIdx - requesting index = 0x1000
typeRegWithIdx - requesting index = 0x1000
Found a Stack Register!
typeRegWithIdx - requesting index = 0x68
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x69
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x68
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x69
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x68
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x69
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x68
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x68
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
Found a Stack Register!
typeRegWithIdx - requesting index = 0x63
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
Found a Stack Register!
typeRegWithIdx - requesting index = 0x63
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x1000
typeRegWithIdx - requesting index = 0x1000
typeRegWithIdx - requesting index = 0x1000
Found a Stack Register!
typeRegWithIdx - requesting index = 0x64
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x64
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
Found a Stack Register!
typeRegWithIdx - requesting index = 0x1000
typeRegWithIdx - requesting index = 0x1000
typeRegWithIdx - requesting index = 0x1000
Found a Stack Register!
typeRegWithIdx - requesting index = 0x64
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x1000
typeRegWithIdx - requesting index = 0x1000
typeRegWithIdx - requesting index = 0x1000
Found a Stack Register!
typeRegWithIdx - requesting index = 0x64
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x65
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x1000
typeRegWithIdx - requesting index = 0x1000
typeRegWithIdx - requesting index = 0x1000
Found a Stack Register!
typeRegWithIdx - requesting index = 0x68
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x69
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x68
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x69
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x68
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x69
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6a
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x68
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x69
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x69
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
Found a Stack Register!
typeRegWithIdx - requesting index = 0x63
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
Found a Stack Register!
typeRegWithIdx - requesting index = 0x63
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x1000
typeRegWithIdx - requesting index = 0x1000
typeRegWithIdx - requesting index = 0x1000
Found a Stack Register!
typeRegWithIdx - requesting index = 0x64
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x64
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
Found a Stack Register!
typeRegWithIdx - requesting index = 0x1000
typeRegWithIdx - requesting index = 0x1000
typeRegWithIdx - requesting index = 0x1000
Found a Stack Register!
typeRegWithIdx - requesting index = 0x64
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x1000
typeRegWithIdx - requesting index = 0x1000
typeRegWithIdx - requesting index = 0x1000
Found a Stack Register!
typeRegWithIdx - requesting index = 0x64
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x65
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x1000
typeRegWithIdx - requesting index = 0x1000
typeRegWithIdx - requesting index = 0x1000
Found a Stack Register!
typeRegWithIdx - requesting index = 0x68
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x69
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x68
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x69
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x68
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x69
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6a
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x68
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6a
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
Found a Stack Register!
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
Found a Stack Register!
typeRegWithIdx - requesting index = 0x68
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6a
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
Found a Stack Register!
typeRegWithIdx - requesting index = 0x6a
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
Found a Stack Register!
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
Found a Stack Register!
typeRegWithIdx - requesting index = 0x53
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
Found a Stack Register!
typeRegWithIdx - requesting index = 0x53
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x54
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
Found a Stack Register!
typeRegWithIdx - requesting index = 0x54
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x1000
typeRegWithIdx - requesting index = 0x1000
typeRegWithIdx - requesting index = 0x1000
Found a Stack Register!
typeRegWithIdx - requesting index = 0x54
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x1000
typeRegWithIdx - requesting index = 0x1000
typeRegWithIdx - requesting index = 0x1000
Found a Stack Register!
typeRegWithIdx - requesting index = 0x53
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
Found a Stack Register!
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
Found a Stack Register!
typeRegWithIdx - requesting index = 0x54
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x53
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
Found a Stack Register!
typeRegWithIdx - requesting index = 0x53
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
Found a Stack Register!
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
Found a Stack Register!
typeRegWithIdx - requesting index = 0x5c
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
Found a Stack Register!
typeRegWithIdx - requesting index = 0x5c
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x5b
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x5b
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
Found a Stack Register!
typeRegWithIdx - requesting index = 0x53
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
Found a Stack Register!
typeRegWithIdx - requesting index = 0x53
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x54
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
Found a Stack Register!
typeRegWithIdx - requesting index = 0x54
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x1000
typeRegWithIdx - requesting index = 0x1000
typeRegWithIdx - requesting index = 0x1000
Found a Stack Register!
typeRegWithIdx - requesting index = 0x54
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x1000
typeRegWithIdx - requesting index = 0x1000
typeRegWithIdx - requesting index = 0x1000
Found a Stack Register!
typeRegWithIdx - requesting index = 0x53
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
Found a Stack Register!
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
Found a Stack Register!
typeRegWithIdx - requesting index = 0x54
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x53
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
Found a Stack Register!
typeRegWithIdx - requesting index = 0x53
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
Found a Stack Register!
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
Found a Stack Register!
typeRegWithIdx - requesting index = 0x5c
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
Found a Stack Register!
typeRegWithIdx - requesting index = 0x5c
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x5b
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
Found a Stack Register!
typeRegWithIdx - requesting index = 0x54
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x1000
typeRegWithIdx - requesting index = 0x1000
typeRegWithIdx - requesting index = 0x1000
Found a Stack Register!
typeRegWithIdx - requesting index = 0x54
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x1000
typeRegWithIdx - requesting index = 0x1000
typeRegWithIdx - requesting index = 0x1000
Found a Stack Register!
typeRegWithIdx - requesting index = 0x53
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
Found a Stack Register!
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
Found a Stack Register!
typeRegWithIdx - requesting index = 0x54
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x53
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
Found a Stack Register!
typeRegWithIdx - requesting index = 0x53
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
Found a Stack Register!
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
Found a Stack Register!
typeRegWithIdx - requesting index = 0x5e
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
Found a Stack Register!
typeRegWithIdx - requesting index = 0x5e
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x5d
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x5d
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
Found a Stack Register!
typeRegWithIdx - requesting index = 0x53
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
Found a Stack Register!
typeRegWithIdx - requesting index = 0x53
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x54
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
Found a Stack Register!
typeRegWithIdx - requesting index = 0x54
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x1000
typeRegWithIdx - requesting index = 0x1000
typeRegWithIdx - requesting index = 0x1000
Found a Stack Register!
typeRegWithIdx - requesting index = 0x54
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x1000
typeRegWithIdx - requesting index = 0x1000
typeRegWithIdx - requesting index = 0x1000
Found a Stack Register!
typeRegWithIdx - requesting index = 0x53
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
Found a Stack Register!
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
Found a Stack Register!
typeRegWithIdx - requesting index = 0x54
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x53
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
Found a Stack Register!
typeRegWithIdx - requesting index = 0x53
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
Found a Stack Register!
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
Found a Stack Register!
typeRegWithIdx - requesting index = 0x5c
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
Found a Stack Register!
typeRegWithIdx - requesting index = 0x5c
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x5b
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
Found a Stack Register!
typeRegWithIdx - requesting index = 0x54
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x1000
typeRegWithIdx - requesting index = 0x1000
typeRegWithIdx - requesting index = 0x1000
Found a Stack Register!
typeRegWithIdx - requesting index = 0x54
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x1000
typeRegWithIdx - requesting index = 0x1000
typeRegWithIdx - requesting index = 0x1000
Found a Stack Register!
typeRegWithIdx - requesting index = 0x53
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
Found a Stack Register!
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
Found a Stack Register!
typeRegWithIdx - requesting index = 0x54
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x53
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
Found a Stack Register!
typeRegWithIdx - requesting index = 0x53
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
Found a Stack Register!
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
Found a Stack Register!
typeRegWithIdx - requesting index = 0x5e
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
Found a Stack Register!
typeRegWithIdx - requesting index = 0x5e
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x5d
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
Found a Stack Register!
typeRegWithIdx - requesting index = 0x54
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x1000
typeRegWithIdx - requesting index = 0x1000
typeRegWithIdx - requesting index = 0x1000
Found a Stack Register!
typeRegWithIdx - requesting index = 0x54
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x1000
typeRegWithIdx - requesting index = 0x1000
typeRegWithIdx - requesting index = 0x1000
Found a Stack Register!
typeRegWithIdx - requesting index = 0x53
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
Found a Stack Register!
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
Found a Stack Register!
typeRegWithIdx - requesting index = 0x54
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x53
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
Found a Stack Register!
typeRegWithIdx - requesting index = 0x53
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
Found a Stack Register!
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
Found a Stack Register!
typeRegWithIdx - requesting index = 0x60
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
Found a Stack Register!
typeRegWithIdx - requesting index = 0x60
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x5f
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x5f
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
Found a Stack Register!
typeRegWithIdx - requesting index = 0x53
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
Found a Stack Register!
typeRegWithIdx - requesting index = 0x53
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x54
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
Found a Stack Register!
typeRegWithIdx - requesting index = 0x54
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x1000
typeRegWithIdx - requesting index = 0x1000
typeRegWithIdx - requesting index = 0x1000
Found a Stack Register!
typeRegWithIdx - requesting index = 0x54
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x1000
typeRegWithIdx - requesting index = 0x1000
typeRegWithIdx - requesting index = 0x1000
Found a Stack Register!
typeRegWithIdx - requesting index = 0x53
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
Found a Stack Register!
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
Found a Stack Register!
typeRegWithIdx - requesting index = 0x54
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x53
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
Found a Stack Register!
typeRegWithIdx - requesting index = 0x53
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
Found a Stack Register!
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
Found a Stack Register!
typeRegWithIdx - requesting index = 0x5c
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
Found a Stack Register!
typeRegWithIdx - requesting index = 0x5c
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x5b
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
Found a Stack Register!
typeRegWithIdx - requesting index = 0x54
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x1000
typeRegWithIdx - requesting index = 0x1000
typeRegWithIdx - requesting index = 0x1000
Found a Stack Register!
typeRegWithIdx - requesting index = 0x54
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x1000
typeRegWithIdx - requesting index = 0x1000
typeRegWithIdx - requesting index = 0x1000
Found a Stack Register!
typeRegWithIdx - requesting index = 0x53
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
Found a Stack Register!
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
Found a Stack Register!
typeRegWithIdx - requesting index = 0x54
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x53
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
Found a Stack Register!
typeRegWithIdx - requesting index = 0x53
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
Found a Stack Register!
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
Found a Stack Register!
typeRegWithIdx - requesting index = 0x5e
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
Found a Stack Register!
typeRegWithIdx - requesting index = 0x5e
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x5d
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
Found a Stack Register!
typeRegWithIdx - requesting index = 0x54
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x1000
typeRegWithIdx - requesting index = 0x1000
typeRegWithIdx - requesting index = 0x1000
Found a Stack Register!
typeRegWithIdx - requesting index = 0x54
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x1000
typeRegWithIdx - requesting index = 0x1000
typeRegWithIdx - requesting index = 0x1000
Found a Stack Register!
typeRegWithIdx - requesting index = 0x53
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
Found a Stack Register!
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
Found a Stack Register!
typeRegWithIdx - requesting index = 0x54
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x53
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
Found a Stack Register!
typeRegWithIdx - requesting index = 0x53
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
Found a Stack Register!
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
Found a Stack Register!
typeRegWithIdx - requesting index = 0x60
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
Found a Stack Register!
typeRegWithIdx - requesting index = 0x60
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x5f
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
Found a Stack Register!
typeRegWithIdx - requesting index = 0x54
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x1000
typeRegWithIdx - requesting index = 0x1000
typeRegWithIdx - requesting index = 0x1000
Found a Stack Register!
typeRegWithIdx - requesting index = 0x54
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x1000
typeRegWithIdx - requesting index = 0x1000
typeRegWithIdx - requesting index = 0x1000
Found a Stack Register!
typeRegWithIdx - requesting index = 0x53
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
Found a Stack Register!
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
Found a Stack Register!
typeRegWithIdx - requesting index = 0x54
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x53
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
Found a Stack Register!
typeRegWithIdx - requesting index = 0x53
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
Found a Stack Register!
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
Found a Stack Register!
typeRegWithIdx - requesting index = 0x62
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
Found a Stack Register!
typeRegWithIdx - requesting index = 0x62
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x61
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x61
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
Found a Stack Register!
typeRegWithIdx - requesting index = 0x53
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
Found a Stack Register!
typeRegWithIdx - requesting index = 0x53
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x54
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
Found a Stack Register!
typeRegWithIdx - requesting index = 0x54
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x1000
typeRegWithIdx - requesting index = 0x1000
typeRegWithIdx - requesting index = 0x1000
Found a Stack Register!
typeRegWithIdx - requesting index = 0x54
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x1000
typeRegWithIdx - requesting index = 0x1000
typeRegWithIdx - requesting index = 0x1000
Found a Stack Register!
typeRegWithIdx - requesting index = 0x53
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
Found a Stack Register!
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
Found a Stack Register!
typeRegWithIdx - requesting index = 0x54
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x53
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
Found a Stack Register!
typeRegWithIdx - requesting index = 0x53
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
Found a Stack Register!
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
Found a Stack Register!
typeRegWithIdx - requesting index = 0x5c
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
Found a Stack Register!
typeRegWithIdx - requesting index = 0x5c
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x5b
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
Found a Stack Register!
typeRegWithIdx - requesting index = 0x54
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x1000
typeRegWithIdx - requesting index = 0x1000
typeRegWithIdx - requesting index = 0x1000
Found a Stack Register!
typeRegWithIdx - requesting index = 0x54
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x1000
typeRegWithIdx - requesting index = 0x1000
typeRegWithIdx - requesting index = 0x1000
Found a Stack Register!
typeRegWithIdx - requesting index = 0x53
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
Found a Stack Register!
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
Found a Stack Register!
typeRegWithIdx - requesting index = 0x54
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x53
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
Found a Stack Register!
typeRegWithIdx - requesting index = 0x53
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
Found a Stack Register!
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
Found a Stack Register!
typeRegWithIdx - requesting index = 0x5e
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
Found a Stack Register!
typeRegWithIdx - requesting index = 0x5e
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x5d
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
Found a Stack Register!
typeRegWithIdx - requesting index = 0x54
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x1000
typeRegWithIdx - requesting index = 0x1000
typeRegWithIdx - requesting index = 0x1000
Found a Stack Register!
typeRegWithIdx - requesting index = 0x54
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x1000
typeRegWithIdx - requesting index = 0x1000
typeRegWithIdx - requesting index = 0x1000
Found a Stack Register!
typeRegWithIdx - requesting index = 0x53
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
Found a Stack Register!
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
Found a Stack Register!
typeRegWithIdx - requesting index = 0x54
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x53
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
Found a Stack Register!
typeRegWithIdx - requesting index = 0x53
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
Found a Stack Register!
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
Found a Stack Register!
typeRegWithIdx - requesting index = 0x60
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
Found a Stack Register!
typeRegWithIdx - requesting index = 0x60
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x5f
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
Found a Stack Register!
typeRegWithIdx - requesting index = 0x54
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x1000
typeRegWithIdx - requesting index = 0x1000
typeRegWithIdx - requesting index = 0x1000
Found a Stack Register!
typeRegWithIdx - requesting index = 0x54
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x1000
typeRegWithIdx - requesting index = 0x1000
typeRegWithIdx - requesting index = 0x1000
Found a Stack Register!
typeRegWithIdx - requesting index = 0x53
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
Found a Stack Register!
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
Found a Stack Register!
typeRegWithIdx - requesting index = 0x54
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x53
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
Found a Stack Register!
typeRegWithIdx - requesting index = 0x53
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
Found a Stack Register!
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
Found a Stack Register!
typeRegWithIdx - requesting index = 0x62
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
Found a Stack Register!
typeRegWithIdx - requesting index = 0x62
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x61
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
Found a Stack Register!
typeRegWithIdx - requesting index = 0x59
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x5a
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x56
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
Found a Stack Register!
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
Found a Stack Register!
typeRegWithIdx - requesting index = 0x55
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x56
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
Found a Stack Register!
typeRegWithIdx - requesting index = 0x56
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
Found a Stack Register!
typeRegWithIdx - requesting index = 0x6f
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x70
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6f
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x71
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6f
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x71
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6f
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x71
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x73
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x74
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6f
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x73
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6f
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x71
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6f
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x71
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6f
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x71
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x75
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x76
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6f
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x75
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6f
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x71
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6f
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x71
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6f
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x71
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x77
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x78
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6f
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x77
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6f
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x71
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6f
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x71
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6f
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x71
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x79
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x7a
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6f
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x79
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6f
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x71
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6f
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x71
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6f
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x71
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x7b
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x7c
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6f
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x7b
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6f
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x71
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6f
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x71
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6f
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x71
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x7d
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x7e
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6f
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x7d
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6f
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x71
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6f
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x71
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6f
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x71
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x7f
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x80
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6f
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x7f
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6f
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x71
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6f
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x71
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6f
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x71
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
Found a Stack Register!
typeRegWithIdx - requesting index = 0x63
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
Found a Stack Register!
typeRegWithIdx - requesting index = 0x63
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x1000
typeRegWithIdx - requesting index = 0x1000
typeRegWithIdx - requesting index = 0x1000
Found a Stack Register!
typeRegWithIdx - requesting index = 0x64
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x64
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
Found a Stack Register!
typeRegWithIdx - requesting index = 0x1000
typeRegWithIdx - requesting index = 0x1000
typeRegWithIdx - requesting index = 0x1000
Found a Stack Register!
typeRegWithIdx - requesting index = 0x64
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x1000
typeRegWithIdx - requesting index = 0x1000
typeRegWithIdx - requesting index = 0x1000
Found a Stack Register!
typeRegWithIdx - requesting index = 0x64
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x65
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x1000
typeRegWithIdx - requesting index = 0x1000
typeRegWithIdx - requesting index = 0x1000
Found a Stack Register!
typeRegWithIdx - requesting index = 0x68
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x69
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x68
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x69
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x68
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x69
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6a
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x68
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6a
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
Found a Stack Register!
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
Found a Stack Register!
typeRegWithIdx - requesting index = 0x68
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6a
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
Found a Stack Register!
typeRegWithIdx - requesting index = 0x6a
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
Found a Stack Register!
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
Found a Stack Register!
typeRegWithIdx - requesting index = 0x53
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
Found a Stack Register!
typeRegWithIdx - requesting index = 0x53
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x54
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
Found a Stack Register!
typeRegWithIdx - requesting index = 0x54
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x1000
typeRegWithIdx - requesting index = 0x1000
typeRegWithIdx - requesting index = 0x1000
Found a Stack Register!
typeRegWithIdx - requesting index = 0x54
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x1000
typeRegWithIdx - requesting index = 0x1000
typeRegWithIdx - requesting index = 0x1000
Found a Stack Register!
typeRegWithIdx - requesting index = 0x53
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
Found a Stack Register!
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
Found a Stack Register!
typeRegWithIdx - requesting index = 0x54
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x53
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
Found a Stack Register!
typeRegWithIdx - requesting index = 0x53
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
Found a Stack Register!
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
Found a Stack Register!
typeRegWithIdx - requesting index = 0x5c
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
Found a Stack Register!
typeRegWithIdx - requesting index = 0x5c
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x5b
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
Found a Stack Register!
typeRegWithIdx - requesting index = 0x54
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x1000
typeRegWithIdx - requesting index = 0x1000
typeRegWithIdx - requesting index = 0x1000
Found a Stack Register!
typeRegWithIdx - requesting index = 0x54
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x1000
typeRegWithIdx - requesting index = 0x1000
typeRegWithIdx - requesting index = 0x1000
Found a Stack Register!
typeRegWithIdx - requesting index = 0x53
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
Found a Stack Register!
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
Found a Stack Register!
typeRegWithIdx - requesting index = 0x54
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x53
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
Found a Stack Register!
typeRegWithIdx - requesting index = 0x53
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
Found a Stack Register!
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
Found a Stack Register!
typeRegWithIdx - requesting index = 0x5e
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
Found a Stack Register!
typeRegWithIdx - requesting index = 0x5e
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x5d
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
Found a Stack Register!
typeRegWithIdx - requesting index = 0x54
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x1000
typeRegWithIdx - requesting index = 0x1000
typeRegWithIdx - requesting index = 0x1000
Found a Stack Register!
typeRegWithIdx - requesting index = 0x54
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x1000
typeRegWithIdx - requesting index = 0x1000
typeRegWithIdx - requesting index = 0x1000
Found a Stack Register!
typeRegWithIdx - requesting index = 0x53
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
Found a Stack Register!
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
Found a Stack Register!
typeRegWithIdx - requesting index = 0x54
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x53
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
Found a Stack Register!
typeRegWithIdx - requesting index = 0x53
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
Found a Stack Register!
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
Found a Stack Register!
typeRegWithIdx - requesting index = 0x60
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
Found a Stack Register!
typeRegWithIdx - requesting index = 0x60
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x5f
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
Found a Stack Register!
typeRegWithIdx - requesting index = 0x54
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x1000
typeRegWithIdx - requesting index = 0x1000
typeRegWithIdx - requesting index = 0x1000
Found a Stack Register!
typeRegWithIdx - requesting index = 0x54
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x1000
typeRegWithIdx - requesting index = 0x1000
typeRegWithIdx - requesting index = 0x1000
Found a Stack Register!
typeRegWithIdx - requesting index = 0x53
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
Found a Stack Register!
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
Found a Stack Register!
typeRegWithIdx - requesting index = 0x54
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x53
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
Found a Stack Register!
typeRegWithIdx - requesting index = 0x53
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
Found a Stack Register!
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
Found a Stack Register!
typeRegWithIdx - requesting index = 0x62
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
Found a Stack Register!
typeRegWithIdx - requesting index = 0x62
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x61
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
Found a Stack Register!
typeRegWithIdx - requesting index = 0x59
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x5a
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x56
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
Found a Stack Register!
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
Found a Stack Register!
typeRegWithIdx - requesting index = 0x55
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x56
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
Found a Stack Register!
typeRegWithIdx - requesting index = 0x56
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
Found a Stack Register!
typeRegWithIdx - requesting index = 0x6c
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6c
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6c
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6d
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6e
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6c
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6c
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6c
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6c
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6d
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6e
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6c
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6c
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6c
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6c
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6d
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6e
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6c
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6c
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6c
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6c
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6d
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6e
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6c
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6c
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6c
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6c
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6d
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6e
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6c
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6c
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6c
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6c
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6d
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6e
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6c
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6c
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6c
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6c
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6d
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6e
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6c
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6c
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6c
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6c
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6d
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6e
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6c
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6c
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6c
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6c
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6d
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6e
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6c
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6c
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6c
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6c
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6d
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6e
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6c
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6c
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6c
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6c
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6d
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6e
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6c
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6c
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6c
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6c
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6d
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6e
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6c
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6c
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6c
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6c
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6d
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6e
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6c
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6c
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6c
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6c
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6d
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6e
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6c
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6c
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6c
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6c
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6d
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6e
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6c
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6c
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6c
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6c
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6d
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6e
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6c
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6c
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6c
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6c
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6d
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6e
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6c
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6c
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6c
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6c
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6d
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6e
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6c
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6f
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x70
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x71
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x72
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x73
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x74
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x75
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x76
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x77
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x78
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x79
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x7a
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x7b
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x7c
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x7d
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x7e
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x7f
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x80
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6d
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6e
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6c
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x63
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
Found a Stack Register!
typeRegWithIdx - requesting index = 0x64
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x1000
typeRegWithIdx - requesting index = 0x1000
typeRegWithIdx - requesting index = 0x1000
Found a Stack Register!
typeRegWithIdx - requesting index = 0x65
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x66
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x67
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x68
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x69
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6a
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6b
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x63
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
Found a Stack Register!
typeRegWithIdx - requesting index = 0x64
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x1000
typeRegWithIdx - requesting index = 0x1000
typeRegWithIdx - requesting index = 0x1000
Found a Stack Register!
typeRegWithIdx - requesting index = 0x65
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x66
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x67
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x68
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x69
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6a
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6b
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x63
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
Found a Stack Register!
typeRegWithIdx - requesting index = 0x64
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x1000
typeRegWithIdx - requesting index = 0x1000
typeRegWithIdx - requesting index = 0x1000
Found a Stack Register!
typeRegWithIdx - requesting index = 0x65
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x66
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x67
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x68
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x69
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6a
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6b
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x63
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
Found a Stack Register!
typeRegWithIdx - requesting index = 0x64
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x1000
typeRegWithIdx - requesting index = 0x1000
typeRegWithIdx - requesting index = 0x1000
Found a Stack Register!
typeRegWithIdx - requesting index = 0x65
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x66
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x67
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x68
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x69
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6a
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6b
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x63
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
Found a Stack Register!
typeRegWithIdx - requesting index = 0x64
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x1000
typeRegWithIdx - requesting index = 0x1000
typeRegWithIdx - requesting index = 0x1000
Found a Stack Register!
typeRegWithIdx - requesting index = 0x65
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x66
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x67
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x68
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x69
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6a
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6b
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x53
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
Found a Stack Register!
typeRegWithIdx - requesting index = 0x54
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x55
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x56
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x57
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x58
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x59
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x5a
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x1000
typeRegWithIdx - requesting index = 0x1000
typeRegWithIdx - requesting index = 0x1000
Found a Stack Register!
typeRegWithIdx - requesting index = 0x5c
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x5b
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x5e
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x5d
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x60
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x5f
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x62
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x61
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x63
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x64
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x65
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x66
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x67
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x68
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x69
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6a
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6b
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x63
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
Found a Stack Register!
typeRegWithIdx - requesting index = 0x64
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x1000
typeRegWithIdx - requesting index = 0x1000
typeRegWithIdx - requesting index = 0x1000
Found a Stack Register!
typeRegWithIdx - requesting index = 0x65
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x66
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x67
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x68
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x69
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6a
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6b
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x63
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
Found a Stack Register!
typeRegWithIdx - requesting index = 0x64
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x1000
typeRegWithIdx - requesting index = 0x1000
typeRegWithIdx - requesting index = 0x1000
Found a Stack Register!
typeRegWithIdx - requesting index = 0x65
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x66
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x67
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x68
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x69
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6a
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6b
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x63
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
Found a Stack Register!
typeRegWithIdx - requesting index = 0x64
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x1000
typeRegWithIdx - requesting index = 0x1000
typeRegWithIdx - requesting index = 0x1000
Found a Stack Register!
typeRegWithIdx - requesting index = 0x65
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x66
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x67
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x68
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x69
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6a
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6b
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x63
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
Found a Stack Register!
typeRegWithIdx - requesting index = 0x64
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x1000
typeRegWithIdx - requesting index = 0x1000
typeRegWithIdx - requesting index = 0x1000
Found a Stack Register!
typeRegWithIdx - requesting index = 0x65
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x66
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x67
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x68
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x69
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6a
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6b
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x53
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
Found a Stack Register!
typeRegWithIdx - requesting index = 0x54
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x55
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x56
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x57
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x58
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x59
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x5a
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x1000
typeRegWithIdx - requesting index = 0x1000
typeRegWithIdx - requesting index = 0x1000
Found a Stack Register!
typeRegWithIdx - requesting index = 0x5c
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x5b
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x5e
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x5d
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x60
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x5f
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x62
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x61
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x63
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x64
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x65
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x66
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x67
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x68
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x69
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6a
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6b
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x63
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
Found a Stack Register!
typeRegWithIdx - requesting index = 0x64
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x1000
typeRegWithIdx - requesting index = 0x1000
typeRegWithIdx - requesting index = 0x1000
Found a Stack Register!
typeRegWithIdx - requesting index = 0x65
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x66
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x67
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x68
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x69
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6a
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6b
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x63
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
Found a Stack Register!
typeRegWithIdx - requesting index = 0x64
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x1000
typeRegWithIdx - requesting index = 0x1000
typeRegWithIdx - requesting index = 0x1000
Found a Stack Register!
typeRegWithIdx - requesting index = 0x65
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x66
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x67
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x68
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x69
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6a
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6b
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x63
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
Found a Stack Register!
typeRegWithIdx - requesting index = 0x64
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x1000
typeRegWithIdx - requesting index = 0x1000
typeRegWithIdx - requesting index = 0x1000
Found a Stack Register!
typeRegWithIdx - requesting index = 0x65
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x66
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x67
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x68
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x69
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6a
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6b
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x63
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
Found a Stack Register!
typeRegWithIdx - requesting index = 0x64
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x1000
typeRegWithIdx - requesting index = 0x1000
typeRegWithIdx - requesting index = 0x1000
Found a Stack Register!
typeRegWithIdx - requesting index = 0x65
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x66
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x67
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x68
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x69
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6a
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6b
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x53
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
Found a Stack Register!
typeRegWithIdx - requesting index = 0x54
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x55
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x56
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x57
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x58
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x59
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x5a
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x1000
typeRegWithIdx - requesting index = 0x1000
typeRegWithIdx - requesting index = 0x1000
Found a Stack Register!
typeRegWithIdx - requesting index = 0x5c
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x5b
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x5e
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x5d
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x60
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x5f
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x62
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x61
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x63
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x64
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x65
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x66
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x67
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x68
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x69
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6a
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6b
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x63
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
Found a Stack Register!
typeRegWithIdx - requesting index = 0x64
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x1000
typeRegWithIdx - requesting index = 0x1000
typeRegWithIdx - requesting index = 0x1000
Found a Stack Register!
typeRegWithIdx - requesting index = 0x65
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x66
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x67
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x68
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x69
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6a
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6b
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x63
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
Found a Stack Register!
typeRegWithIdx - requesting index = 0x64
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x1000
typeRegWithIdx - requesting index = 0x1000
typeRegWithIdx - requesting index = 0x1000
Found a Stack Register!
typeRegWithIdx - requesting index = 0x65
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x66
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x67
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x68
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x69
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6a
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6b
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x63
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
Found a Stack Register!
typeRegWithIdx - requesting index = 0x64
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x1000
typeRegWithIdx - requesting index = 0x1000
typeRegWithIdx - requesting index = 0x1000
Found a Stack Register!
typeRegWithIdx - requesting index = 0x65
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x66
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x67
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x68
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x69
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6a
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6b
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x63
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
Found a Stack Register!
typeRegWithIdx - requesting index = 0x64
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x1000
typeRegWithIdx - requesting index = 0x1000
typeRegWithIdx - requesting index = 0x1000
Found a Stack Register!
typeRegWithIdx - requesting index = 0x65
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x66
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x67
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x68
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x69
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6a
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6b
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x53
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
Found a Stack Register!
typeRegWithIdx - requesting index = 0x54
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x55
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x56
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x57
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x58
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x59
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x5a
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x1000
typeRegWithIdx - requesting index = 0x1000
typeRegWithIdx - requesting index = 0x1000
Found a Stack Register!
typeRegWithIdx - requesting index = 0x5c
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x5b
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x5e
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x5d
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x60
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x5f
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x62
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x61
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x63
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x64
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x65
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x66
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x67
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x68
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x69
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6a
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6b
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x63
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
Found a Stack Register!
typeRegWithIdx - requesting index = 0x64
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x1000
typeRegWithIdx - requesting index = 0x1000
typeRegWithIdx - requesting index = 0x1000
Found a Stack Register!
typeRegWithIdx - requesting index = 0x65
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x66
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x67
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x68
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x69
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6a
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6b
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x63
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
Found a Stack Register!
typeRegWithIdx - requesting index = 0x64
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x1000
typeRegWithIdx - requesting index = 0x1000
typeRegWithIdx - requesting index = 0x1000
Found a Stack Register!
typeRegWithIdx - requesting index = 0x65
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x66
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x67
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x68
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x69
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6a
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6b
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x63
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
Found a Stack Register!
typeRegWithIdx - requesting index = 0x64
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x1000
typeRegWithIdx - requesting index = 0x1000
typeRegWithIdx - requesting index = 0x1000
Found a Stack Register!
typeRegWithIdx - requesting index = 0x65
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x66
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x67
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x68
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x69
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6a
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6b
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x63
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
Found a Stack Register!
typeRegWithIdx - requesting index = 0x64
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x1000
typeRegWithIdx - requesting index = 0x1000
typeRegWithIdx - requesting index = 0x1000
Found a Stack Register!
typeRegWithIdx - requesting index = 0x65
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x66
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x67
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x68
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x69
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6a
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6b
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x53
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
Found a Stack Register!
typeRegWithIdx - requesting index = 0x54
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x55
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x56
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x57
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x58
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x59
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x5a
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x1000
typeRegWithIdx - requesting index = 0x1000
typeRegWithIdx - requesting index = 0x1000
Found a Stack Register!
typeRegWithIdx - requesting index = 0x5c
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x5b
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x5e
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x5d
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x60
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x5f
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x62
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x61
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x63
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x64
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x65
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x66
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x67
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x68
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x69
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6a
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6b
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x63
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
Found a Stack Register!
typeRegWithIdx - requesting index = 0x64
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x1000
typeRegWithIdx - requesting index = 0x1000
typeRegWithIdx - requesting index = 0x1000
Found a Stack Register!
typeRegWithIdx - requesting index = 0x65
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x66
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x67
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x68
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x69
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6a
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6b
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x63
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
Found a Stack Register!
typeRegWithIdx - requesting index = 0x64
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x1000
typeRegWithIdx - requesting index = 0x1000
typeRegWithIdx - requesting index = 0x1000
Found a Stack Register!
typeRegWithIdx - requesting index = 0x65
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x66
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x67
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x68
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x69
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6a
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6b
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x63
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
Found a Stack Register!
typeRegWithIdx - requesting index = 0x64
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x1000
typeRegWithIdx - requesting index = 0x1000
typeRegWithIdx - requesting index = 0x1000
Found a Stack Register!
typeRegWithIdx - requesting index = 0x65
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x66
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x67
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x68
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x69
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6a
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6b
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x63
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
Found a Stack Register!
typeRegWithIdx - requesting index = 0x64
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x1000
typeRegWithIdx - requesting index = 0x1000
typeRegWithIdx - requesting index = 0x1000
Found a Stack Register!
typeRegWithIdx - requesting index = 0x65
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x66
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x67
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x68
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x69
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6a
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6b
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x53
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
Found a Stack Register!
typeRegWithIdx - requesting index = 0x54
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x55
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x56
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x57
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x58
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x59
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x5a
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x1000
typeRegWithIdx - requesting index = 0x1000
typeRegWithIdx - requesting index = 0x1000
Found a Stack Register!
typeRegWithIdx - requesting index = 0x5c
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x5b
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x5e
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x5d
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x60
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x5f
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x62
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x61
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x63
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x64
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x65
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x66
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x67
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x68
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x69
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6a
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6b
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x63
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
Found a Stack Register!
typeRegWithIdx - requesting index = 0x64
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x1000
typeRegWithIdx - requesting index = 0x1000
typeRegWithIdx - requesting index = 0x1000
Found a Stack Register!
typeRegWithIdx - requesting index = 0x65
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x66
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x67
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x68
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x69
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6a
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6b
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x63
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
Found a Stack Register!
typeRegWithIdx - requesting index = 0x64
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x1000
typeRegWithIdx - requesting index = 0x1000
typeRegWithIdx - requesting index = 0x1000
Found a Stack Register!
typeRegWithIdx - requesting index = 0x65
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x66
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x67
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x68
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x69
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6a
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6b
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x63
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
Found a Stack Register!
typeRegWithIdx - requesting index = 0x64
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x1000
typeRegWithIdx - requesting index = 0x1000
typeRegWithIdx - requesting index = 0x1000
Found a Stack Register!
typeRegWithIdx - requesting index = 0x65
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x66
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x67
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x68
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x69
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6a
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6b
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x63
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
Found a Stack Register!
typeRegWithIdx - requesting index = 0x64
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x1000
typeRegWithIdx - requesting index = 0x1000
typeRegWithIdx - requesting index = 0x1000
Found a Stack Register!
typeRegWithIdx - requesting index = 0x65
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x66
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x67
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x68
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x69
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6a
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6b
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x53
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
Found a Stack Register!
typeRegWithIdx - requesting index = 0x54
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x55
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x56
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x57
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x58
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x59
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x5a
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x1000
typeRegWithIdx - requesting index = 0x1000
typeRegWithIdx - requesting index = 0x1000
Found a Stack Register!
typeRegWithIdx - requesting index = 0x5c
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x5b
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x5e
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x5d
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x60
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x5f
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x62
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x61
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x63
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x64
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x65
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x66
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x67
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x68
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x69
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6a
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6b
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x63
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
Found a Stack Register!
typeRegWithIdx - requesting index = 0x64
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x1000
typeRegWithIdx - requesting index = 0x1000
typeRegWithIdx - requesting index = 0x1000
Found a Stack Register!
typeRegWithIdx - requesting index = 0x65
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x66
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x67
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x68
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x69
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6a
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6b
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x63
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
Found a Stack Register!
typeRegWithIdx - requesting index = 0x64
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x1000
typeRegWithIdx - requesting index = 0x1000
typeRegWithIdx - requesting index = 0x1000
Found a Stack Register!
typeRegWithIdx - requesting index = 0x65
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x66
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x67
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x68
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x69
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6a
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6b
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x63
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
Found a Stack Register!
typeRegWithIdx - requesting index = 0x64
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x1000
typeRegWithIdx - requesting index = 0x1000
typeRegWithIdx - requesting index = 0x1000
Found a Stack Register!
typeRegWithIdx - requesting index = 0x65
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x66
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x67
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x68
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x69
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6a
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6b
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x63
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
Found a Stack Register!
typeRegWithIdx - requesting index = 0x64
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x1000
typeRegWithIdx - requesting index = 0x1000
typeRegWithIdx - requesting index = 0x1000
Found a Stack Register!
typeRegWithIdx - requesting index = 0x65
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x66
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x67
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x68
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x69
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6a
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6b
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x53
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
Found a Stack Register!
typeRegWithIdx - requesting index = 0x54
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x55
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x56
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x57
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x58
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x59
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x5a
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x1000
typeRegWithIdx - requesting index = 0x1000
typeRegWithIdx - requesting index = 0x1000
Found a Stack Register!
typeRegWithIdx - requesting index = 0x5c
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x5b
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x5e
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x5d
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x60
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x5f
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x62
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x61
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x63
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x64
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x65
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x66
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x67
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x68
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x69
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6a
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6b
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x63
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
Found a Stack Register!
typeRegWithIdx - requesting index = 0x64
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x1000
typeRegWithIdx - requesting index = 0x1000
typeRegWithIdx - requesting index = 0x1000
Found a Stack Register!
typeRegWithIdx - requesting index = 0x65
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x66
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x67
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x68
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x69
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6a
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6b
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x63
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
Found a Stack Register!
typeRegWithIdx - requesting index = 0x64
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x1000
typeRegWithIdx - requesting index = 0x1000
typeRegWithIdx - requesting index = 0x1000
Found a Stack Register!
typeRegWithIdx - requesting index = 0x65
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x66
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x67
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x68
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x69
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6a
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6b
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x63
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
Found a Stack Register!
typeRegWithIdx - requesting index = 0x64
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x1000
typeRegWithIdx - requesting index = 0x1000
typeRegWithIdx - requesting index = 0x1000
Found a Stack Register!
typeRegWithIdx - requesting index = 0x65
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x66
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x67
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x68
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x69
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6a
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6b
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x63
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
Found a Stack Register!
typeRegWithIdx - requesting index = 0x64
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x1000
typeRegWithIdx - requesting index = 0x1000
typeRegWithIdx - requesting index = 0x1000
Found a Stack Register!
typeRegWithIdx - requesting index = 0x65
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x66
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x67
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x68
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x69
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6a
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6b
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x53
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
Found a Stack Register!
typeRegWithIdx - requesting index = 0x54
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x55
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x56
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x57
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x58
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x59
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x5a
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x1000
typeRegWithIdx - requesting index = 0x1000
typeRegWithIdx - requesting index = 0x1000
Found a Stack Register!
typeRegWithIdx - requesting index = 0x5c
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x5b
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x5e
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x5d
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x60
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x5f
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x62
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x61
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x63
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x64
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x65
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x66
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x67
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x68
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x69
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6a
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6b
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
Found a Stack Register!
typeRegWithIdx - requesting index = 0x53
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x54
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x55
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x56
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x57
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x58
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x59
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x5a
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x5c
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x5b
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x5e
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x5d
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x60
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x5f
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x62
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x61
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x63
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x64
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x65
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x66
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x67
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x68
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x69
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6a
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6b
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x63
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
Found a Stack Register!
typeRegWithIdx - requesting index = 0x64
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x1000
typeRegWithIdx - requesting index = 0x1000
typeRegWithIdx - requesting index = 0x1000
Found a Stack Register!
typeRegWithIdx - requesting index = 0x65
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x66
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x67
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x68
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x69
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6a
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6b
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x63
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
Found a Stack Register!
typeRegWithIdx - requesting index = 0x64
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x1000
typeRegWithIdx - requesting index = 0x1000
typeRegWithIdx - requesting index = 0x1000
Found a Stack Register!
typeRegWithIdx - requesting index = 0x65
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x66
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x67
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x68
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x69
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6a
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6b
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x63
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
Found a Stack Register!
typeRegWithIdx - requesting index = 0x64
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x1000
typeRegWithIdx - requesting index = 0x1000
typeRegWithIdx - requesting index = 0x1000
Found a Stack Register!
typeRegWithIdx - requesting index = 0x65
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x66
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x67
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x68
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x69
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6a
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6b
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x63
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
Found a Stack Register!
typeRegWithIdx - requesting index = 0x64
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x1000
typeRegWithIdx - requesting index = 0x1000
typeRegWithIdx - requesting index = 0x1000
Found a Stack Register!
typeRegWithIdx - requesting index = 0x65
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x66
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x67
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x68
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x69
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6a
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6b
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x53
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
Found a Stack Register!
typeRegWithIdx - requesting index = 0x54
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x55
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x56
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x57
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x58
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x59
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x5a
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x1000
typeRegWithIdx - requesting index = 0x1000
typeRegWithIdx - requesting index = 0x1000
Found a Stack Register!
typeRegWithIdx - requesting index = 0x5c
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x5b
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x5e
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x5d
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x60
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x5f
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x62
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x61
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x63
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x64
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x65
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x66
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x67
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x68
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x69
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6a
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6b
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x63
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
Found a Stack Register!
typeRegWithIdx - requesting index = 0x64
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x1000
typeRegWithIdx - requesting index = 0x1000
typeRegWithIdx - requesting index = 0x1000
Found a Stack Register!
typeRegWithIdx - requesting index = 0x65
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x66
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x67
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x68
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x69
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6a
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6b
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x63
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
Found a Stack Register!
typeRegWithIdx - requesting index = 0x64
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x1000
typeRegWithIdx - requesting index = 0x1000
typeRegWithIdx - requesting index = 0x1000
Found a Stack Register!
typeRegWithIdx - requesting index = 0x65
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x66
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x67
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x68
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x69
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6a
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6b
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x63
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
Found a Stack Register!
typeRegWithIdx - requesting index = 0x64
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x1000
typeRegWithIdx - requesting index = 0x1000
typeRegWithIdx - requesting index = 0x1000
Found a Stack Register!
typeRegWithIdx - requesting index = 0x65
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x66
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x67
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x68
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x69
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6a
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6b
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x63
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
Found a Stack Register!
typeRegWithIdx - requesting index = 0x64
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x1000
typeRegWithIdx - requesting index = 0x1000
typeRegWithIdx - requesting index = 0x1000
Found a Stack Register!
typeRegWithIdx - requesting index = 0x65
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x66
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x67
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x68
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x69
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6a
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6b
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x53
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
Found a Stack Register!
typeRegWithIdx - requesting index = 0x54
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x55
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x56
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x57
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x58
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x59
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x5a
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x1000
typeRegWithIdx - requesting index = 0x1000
typeRegWithIdx - requesting index = 0x1000
Found a Stack Register!
typeRegWithIdx - requesting index = 0x5c
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x5b
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x5e
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x5d
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x60
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x5f
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x62
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x61
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x63
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x64
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x65
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x66
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x67
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x68
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x69
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6a
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6b
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x63
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
Found a Stack Register!
typeRegWithIdx - requesting index = 0x64
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x1000
typeRegWithIdx - requesting index = 0x1000
typeRegWithIdx - requesting index = 0x1000
Found a Stack Register!
typeRegWithIdx - requesting index = 0x65
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x66
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x67
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x68
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x69
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6a
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6b
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x63
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
Found a Stack Register!
typeRegWithIdx - requesting index = 0x64
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x1000
typeRegWithIdx - requesting index = 0x1000
typeRegWithIdx - requesting index = 0x1000
Found a Stack Register!
typeRegWithIdx - requesting index = 0x65
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x66
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x67
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x68
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x69
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6a
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6b
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x63
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
Found a Stack Register!
typeRegWithIdx - requesting index = 0x64
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x1000
typeRegWithIdx - requesting index = 0x1000
typeRegWithIdx - requesting index = 0x1000
Found a Stack Register!
typeRegWithIdx - requesting index = 0x65
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x66
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x67
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x68
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x69
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6a
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6b
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x63
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
Found a Stack Register!
typeRegWithIdx - requesting index = 0x64
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x1000
typeRegWithIdx - requesting index = 0x1000
typeRegWithIdx - requesting index = 0x1000
Found a Stack Register!
typeRegWithIdx - requesting index = 0x65
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x66
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x67
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x68
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x69
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6a
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6b
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x53
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
Found a Stack Register!
typeRegWithIdx - requesting index = 0x54
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x55
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x56
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x57
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x58
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x59
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x5a
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x1000
typeRegWithIdx - requesting index = 0x1000
typeRegWithIdx - requesting index = 0x1000
Found a Stack Register!
typeRegWithIdx - requesting index = 0x5c
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x5b
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x5e
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x5d
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x60
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x5f
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x62
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x61
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x63
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x64
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x65
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x66
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x67
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x68
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x69
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6a
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6b
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x63
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
Found a Stack Register!
typeRegWithIdx - requesting index = 0x64
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x1000
typeRegWithIdx - requesting index = 0x1000
typeRegWithIdx - requesting index = 0x1000
Found a Stack Register!
typeRegWithIdx - requesting index = 0x65
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x66
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x67
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x68
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x69
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6a
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6b
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x63
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
Found a Stack Register!
typeRegWithIdx - requesting index = 0x64
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x1000
typeRegWithIdx - requesting index = 0x1000
typeRegWithIdx - requesting index = 0x1000
Found a Stack Register!
typeRegWithIdx - requesting index = 0x65
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x66
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x67
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x68
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x69
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6a
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6b
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x63
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
Found a Stack Register!
typeRegWithIdx - requesting index = 0x64
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x1000
typeRegWithIdx - requesting index = 0x1000
typeRegWithIdx - requesting index = 0x1000
Found a Stack Register!
typeRegWithIdx - requesting index = 0x65
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x66
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x67
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x68
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x69
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6a
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6b
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x63
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
Found a Stack Register!
typeRegWithIdx - requesting index = 0x64
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x1000
typeRegWithIdx - requesting index = 0x1000
typeRegWithIdx - requesting index = 0x1000
Found a Stack Register!
typeRegWithIdx - requesting index = 0x65
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x66
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x67
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x68
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x69
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6a
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6b
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x53
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
Found a Stack Register!
typeRegWithIdx - requesting index = 0x54
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x55
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x56
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x57
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x58
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x59
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x5a
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x1000
typeRegWithIdx - requesting index = 0x1000
typeRegWithIdx - requesting index = 0x1000
Found a Stack Register!
typeRegWithIdx - requesting index = 0x5c
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x5b
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x5e
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x5d
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x60
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x5f
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x62
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x61
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x63
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x64
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x65
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x66
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x67
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x68
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x69
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6a
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6b
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x63
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
Found a Stack Register!
typeRegWithIdx - requesting index = 0x64
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x1000
typeRegWithIdx - requesting index = 0x1000
typeRegWithIdx - requesting index = 0x1000
Found a Stack Register!
typeRegWithIdx - requesting index = 0x65
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x66
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x67
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x68
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x69
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6a
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6b
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x63
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
Found a Stack Register!
typeRegWithIdx - requesting index = 0x64
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x1000
typeRegWithIdx - requesting index = 0x1000
typeRegWithIdx - requesting index = 0x1000
Found a Stack Register!
typeRegWithIdx - requesting index = 0x65
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x66
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x67
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x68
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x69
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6a
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6b
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x63
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
Found a Stack Register!
typeRegWithIdx - requesting index = 0x64
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x1000
typeRegWithIdx - requesting index = 0x1000
typeRegWithIdx - requesting index = 0x1000
Found a Stack Register!
typeRegWithIdx - requesting index = 0x65
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x66
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x67
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x68
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x69
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6a
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6b
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x63
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
Found a Stack Register!
typeRegWithIdx - requesting index = 0x64
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x1000
typeRegWithIdx - requesting index = 0x1000
typeRegWithIdx - requesting index = 0x1000
Found a Stack Register!
typeRegWithIdx - requesting index = 0x65
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x66
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x67
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x68
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x69
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6a
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6b
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x53
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
Found a Stack Register!
typeRegWithIdx - requesting index = 0x54
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x55
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x56
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x57
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x58
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x59
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x5a
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x1000
typeRegWithIdx - requesting index = 0x1000
typeRegWithIdx - requesting index = 0x1000
Found a Stack Register!
typeRegWithIdx - requesting index = 0x5c
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x5b
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x5e
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x5d
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x60
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x5f
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x62
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x61
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x63
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x64
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x65
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x66
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x67
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x68
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x69
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6a
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6b
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x63
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
Found a Stack Register!
typeRegWithIdx - requesting index = 0x64
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x1000
typeRegWithIdx - requesting index = 0x1000
typeRegWithIdx - requesting index = 0x1000
Found a Stack Register!
typeRegWithIdx - requesting index = 0x65
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x66
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x67
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x68
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x69
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6a
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6b
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x63
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
Found a Stack Register!
typeRegWithIdx - requesting index = 0x64
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x1000
typeRegWithIdx - requesting index = 0x1000
typeRegWithIdx - requesting index = 0x1000
Found a Stack Register!
typeRegWithIdx - requesting index = 0x65
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x66
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x67
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x68
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x69
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6a
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6b
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x63
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
Found a Stack Register!
typeRegWithIdx - requesting index = 0x64
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x1000
typeRegWithIdx - requesting index = 0x1000
typeRegWithIdx - requesting index = 0x1000
Found a Stack Register!
typeRegWithIdx - requesting index = 0x65
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x66
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x67
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x68
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x69
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6a
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6b
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x63
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
Found a Stack Register!
typeRegWithIdx - requesting index = 0x64
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x1000
typeRegWithIdx - requesting index = 0x1000
typeRegWithIdx - requesting index = 0x1000
Found a Stack Register!
typeRegWithIdx - requesting index = 0x65
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x66
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x67
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x68
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x69
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6a
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6b
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x53
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
Found a Stack Register!
typeRegWithIdx - requesting index = 0x54
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x55
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x56
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x57
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x58
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x59
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x5a
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x1000
typeRegWithIdx - requesting index = 0x1000
typeRegWithIdx - requesting index = 0x1000
Found a Stack Register!
typeRegWithIdx - requesting index = 0x5c
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x5b
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x5e
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x5d
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x60
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x5f
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x62
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x61
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x63
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x64
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x65
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x66
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x67
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x68
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x69
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6a
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6b
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x63
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
Found a Stack Register!
typeRegWithIdx - requesting index = 0x64
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x1000
typeRegWithIdx - requesting index = 0x1000
typeRegWithIdx - requesting index = 0x1000
Found a Stack Register!
typeRegWithIdx - requesting index = 0x65
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x66
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x67
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x68
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x69
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6a
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6b
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x63
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
Found a Stack Register!
typeRegWithIdx - requesting index = 0x64
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x1000
typeRegWithIdx - requesting index = 0x1000
typeRegWithIdx - requesting index = 0x1000
Found a Stack Register!
typeRegWithIdx - requesting index = 0x65
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x66
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x67
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x68
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x69
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6a
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6b
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x63
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
Found a Stack Register!
typeRegWithIdx - requesting index = 0x64
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x1000
typeRegWithIdx - requesting index = 0x1000
typeRegWithIdx - requesting index = 0x1000
Found a Stack Register!
typeRegWithIdx - requesting index = 0x65
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x66
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x67
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x68
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x69
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6a
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6b
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x63
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
Found a Stack Register!
typeRegWithIdx - requesting index = 0x64
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x1000
typeRegWithIdx - requesting index = 0x1000
typeRegWithIdx - requesting index = 0x1000
Found a Stack Register!
typeRegWithIdx - requesting index = 0x65
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x66
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x67
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x68
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x69
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6a
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6b
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x53
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
Found a Stack Register!
typeRegWithIdx - requesting index = 0x54
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x55
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x56
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x57
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x58
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x59
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x5a
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x1000
typeRegWithIdx - requesting index = 0x1000
typeRegWithIdx - requesting index = 0x1000
Found a Stack Register!
typeRegWithIdx - requesting index = 0x5c
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x5b
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x5e
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x5d
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x60
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x5f
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x62
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x61
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x63
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x64
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x65
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x66
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x67
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x68
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x69
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6a
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6b
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x63
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
Found a Stack Register!
typeRegWithIdx - requesting index = 0x64
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x1000
typeRegWithIdx - requesting index = 0x1000
typeRegWithIdx - requesting index = 0x1000
Found a Stack Register!
typeRegWithIdx - requesting index = 0x65
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x66
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x67
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x68
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x69
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6a
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6b
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x63
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
Found a Stack Register!
typeRegWithIdx - requesting index = 0x64
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x1000
typeRegWithIdx - requesting index = 0x1000
typeRegWithIdx - requesting index = 0x1000
Found a Stack Register!
typeRegWithIdx - requesting index = 0x65
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x66
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x67
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x68
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x69
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6a
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6b
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x63
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
Found a Stack Register!
typeRegWithIdx - requesting index = 0x64
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x1000
typeRegWithIdx - requesting index = 0x1000
typeRegWithIdx - requesting index = 0x1000
Found a Stack Register!
typeRegWithIdx - requesting index = 0x65
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x66
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x67
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x68
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x69
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6a
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6b
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x63
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
Found a Stack Register!
typeRegWithIdx - requesting index = 0x64
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x1000
typeRegWithIdx - requesting index = 0x1000
typeRegWithIdx - requesting index = 0x1000
Found a Stack Register!
typeRegWithIdx - requesting index = 0x65
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x66
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x67
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x68
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x69
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6a
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6b
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x53
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
Found a Stack Register!
typeRegWithIdx - requesting index = 0x54
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x55
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x56
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x57
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x58
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x59
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x5a
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x1000
typeRegWithIdx - requesting index = 0x1000
typeRegWithIdx - requesting index = 0x1000
Found a Stack Register!
typeRegWithIdx - requesting index = 0x5c
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x5b
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x5e
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x5d
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x60
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x5f
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x62
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x61
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x63
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x64
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x65
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x66
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x67
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x68
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x69
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6a
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6b
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
Found a Stack Register!
typeRegWithIdx - requesting index = 0x53
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x54
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x55
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x56
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x57
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x58
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x59
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x5a
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x5c
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x5b
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x5e
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x5d
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x60
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x5f
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x62
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x61
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x63
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x64
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x65
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x66
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x67
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x68
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x69
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6a
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6b
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6c
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6c
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6c
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6d
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6e
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6c
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6c
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6c
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6c
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6d
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6e
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6c
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6c
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6c
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6c
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6d
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6e
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6c
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6c
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6c
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6c
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6d
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6e
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6c
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6c
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6c
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6c
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6d
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6e
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6c
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6c
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6c
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6c
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6d
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6e
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6c
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6c
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6c
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6c
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6d
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6e
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6c
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6c
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6c
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6c
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6d
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6e
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6c
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6c
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6c
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6c
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6d
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6e
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6c
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6c
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6c
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6c
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6d
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6e
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6c
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6c
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6c
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6c
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6d
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6e
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6c
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6c
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6c
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6c
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6d
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6e
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6c
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6c
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6c
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6c
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6d
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6e
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6c
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6c
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6c
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6c
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6d
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6e
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6c
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6c
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6c
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6c
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6d
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6e
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6c
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6c
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6c
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6c
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6d
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6e
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6c
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6c
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6c
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6c
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6d
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6e
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6c
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6c
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6c
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6c
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6d
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6e
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6c
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6f
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x70
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x71
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x72
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x73
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x74
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x75
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x76
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x77
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x78
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x79
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x7a
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x7b
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x7c
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x7d
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x7e
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x7f
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x80
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6d
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6e
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6c
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6f
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x70
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x71
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x72
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x73
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x74
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x75
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x76
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x77
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x78
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x79
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x7a
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x7b
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x7c
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x7d
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x7e
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x7f
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x80
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6d
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6e
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6c
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6f
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x70
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6f
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x71
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6f
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x71
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6f
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x71
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x73
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x74
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6f
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x73
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6f
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x71
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6f
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x71
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6f
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x71
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x75
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x76
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6f
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x75
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6f
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x71
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6f
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x71
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6f
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x71
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x77
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x78
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6f
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x77
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6f
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x71
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6f
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x71
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6f
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x71
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x79
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x7a
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6f
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x79
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6f
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x71
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6f
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x71
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6f
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x71
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x7b
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x7c
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6f
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x7b
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6f
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x71
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6f
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x71
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6f
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x71
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x7d
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x7e
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6f
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x7d
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6f
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x71
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6f
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x71
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6f
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x71
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x7f
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x80
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6f
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x7f
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6f
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x71
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6f
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x71
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6f
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x71
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
Found a Stack Register!
typeRegWithIdx - requesting index = 0x63
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
Found a Stack Register!
typeRegWithIdx - requesting index = 0x63
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x1000
typeRegWithIdx - requesting index = 0x1000
typeRegWithIdx - requesting index = 0x1000
Found a Stack Register!
typeRegWithIdx - requesting index = 0x64
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x64
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
Found a Stack Register!
typeRegWithIdx - requesting index = 0x1000
typeRegWithIdx - requesting index = 0x1000
typeRegWithIdx - requesting index = 0x1000
Found a Stack Register!
typeRegWithIdx - requesting index = 0x64
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x1000
typeRegWithIdx - requesting index = 0x1000
typeRegWithIdx - requesting index = 0x1000
Found a Stack Register!
typeRegWithIdx - requesting index = 0x64
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x65
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x1000
typeRegWithIdx - requesting index = 0x1000
typeRegWithIdx - requesting index = 0x1000
Found a Stack Register!
typeRegWithIdx - requesting index = 0x68
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x69
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x68
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x69
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x68
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x69
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6a
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x68
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6a
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
Found a Stack Register!
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
Found a Stack Register!
typeRegWithIdx - requesting index = 0x68
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6a
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
Found a Stack Register!
typeRegWithIdx - requesting index = 0x6a
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
Found a Stack Register!
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
Found a Stack Register!
typeRegWithIdx - requesting index = 0x53
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
Found a Stack Register!
typeRegWithIdx - requesting index = 0x53
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x54
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
Found a Stack Register!
typeRegWithIdx - requesting index = 0x54
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x1000
typeRegWithIdx - requesting index = 0x1000
typeRegWithIdx - requesting index = 0x1000
Found a Stack Register!
typeRegWithIdx - requesting index = 0x54
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x1000
typeRegWithIdx - requesting index = 0x1000
typeRegWithIdx - requesting index = 0x1000
Found a Stack Register!
typeRegWithIdx - requesting index = 0x53
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
Found a Stack Register!
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
Found a Stack Register!
typeRegWithIdx - requesting index = 0x54
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x53
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
Found a Stack Register!
typeRegWithIdx - requesting index = 0x53
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
Found a Stack Register!
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
Found a Stack Register!
typeRegWithIdx - requesting index = 0x5c
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
Found a Stack Register!
typeRegWithIdx - requesting index = 0x5c
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x5b
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
Found a Stack Register!
typeRegWithIdx - requesting index = 0x54
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x1000
typeRegWithIdx - requesting index = 0x1000
typeRegWithIdx - requesting index = 0x1000
Found a Stack Register!
typeRegWithIdx - requesting index = 0x54
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x1000
typeRegWithIdx - requesting index = 0x1000
typeRegWithIdx - requesting index = 0x1000
Found a Stack Register!
typeRegWithIdx - requesting index = 0x53
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
Found a Stack Register!
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
Found a Stack Register!
typeRegWithIdx - requesting index = 0x54
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x53
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
Found a Stack Register!
typeRegWithIdx - requesting index = 0x53
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
Found a Stack Register!
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
Found a Stack Register!
typeRegWithIdx - requesting index = 0x5e
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
Found a Stack Register!
typeRegWithIdx - requesting index = 0x5e
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x5d
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
Found a Stack Register!
typeRegWithIdx - requesting index = 0x54
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x1000
typeRegWithIdx - requesting index = 0x1000
typeRegWithIdx - requesting index = 0x1000
Found a Stack Register!
typeRegWithIdx - requesting index = 0x54
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x1000
typeRegWithIdx - requesting index = 0x1000
typeRegWithIdx - requesting index = 0x1000
Found a Stack Register!
typeRegWithIdx - requesting index = 0x53
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
Found a Stack Register!
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
Found a Stack Register!
typeRegWithIdx - requesting index = 0x54
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x53
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
Found a Stack Register!
typeRegWithIdx - requesting index = 0x53
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
Found a Stack Register!
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
Found a Stack Register!
typeRegWithIdx - requesting index = 0x60
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
Found a Stack Register!
typeRegWithIdx - requesting index = 0x60
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x5f
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
Found a Stack Register!
typeRegWithIdx - requesting index = 0x54
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x1000
typeRegWithIdx - requesting index = 0x1000
typeRegWithIdx - requesting index = 0x1000
Found a Stack Register!
typeRegWithIdx - requesting index = 0x54
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x1000
typeRegWithIdx - requesting index = 0x1000
typeRegWithIdx - requesting index = 0x1000
Found a Stack Register!
typeRegWithIdx - requesting index = 0x53
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
Found a Stack Register!
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
Found a Stack Register!
typeRegWithIdx - requesting index = 0x54
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x53
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
Found a Stack Register!
typeRegWithIdx - requesting index = 0x53
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
Found a Stack Register!
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
Found a Stack Register!
typeRegWithIdx - requesting index = 0x62
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
Found a Stack Register!
typeRegWithIdx - requesting index = 0x62
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x61
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
Found a Stack Register!
typeRegWithIdx - requesting index = 0x59
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x5a
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x56
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
Found a Stack Register!
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
Found a Stack Register!
typeRegWithIdx - requesting index = 0x55
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x56
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
Found a Stack Register!
typeRegWithIdx - requesting index = 0x56
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
Found a Stack Register!
typeRegWithIdx - requesting index = 0x6f
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x70
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x70
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6f
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x71
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6f
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x71
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x71
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6f
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x72
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x73
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x74
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x74
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x73
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6f
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x71
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6f
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x71
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x71
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6f
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x72
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x75
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x76
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x76
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x75
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6f
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x71
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6f
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x71
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x71
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6f
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x72
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x77
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x78
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x78
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x77
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6f
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x71
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6f
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x71
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x71
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6f
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x72
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x79
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x7a
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x7a
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x79
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6f
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x71
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6f
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x71
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x71
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6f
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x72
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x7b
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x7c
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x7c
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x7b
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6f
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x71
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6f
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x71
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x71
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6f
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x72
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x7d
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x7e
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x7e
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x7d
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6f
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x71
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6f
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x71
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x71
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6f
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x72
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x7f
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x80
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x80
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x7f
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6f
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x71
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6f
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x71
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x71
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6f
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x72
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x72
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6d
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6e
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6d
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6e
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6c
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6c
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x63
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
Found a Stack Register!
typeRegWithIdx - requesting index = 0x64
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x1000
typeRegWithIdx - requesting index = 0x1000
typeRegWithIdx - requesting index = 0x1000
Found a Stack Register!
typeRegWithIdx - requesting index = 0x65
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x66
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x66
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x64
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x65
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x65
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x65
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x65
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x67
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x63
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x67
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x68
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x69
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x68
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6a
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x68
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6a
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x68
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x69
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6b
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x69
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x68
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6b
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6a
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x68
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
Found a Stack Register!
typeRegWithIdx - requesting index = 0x6a
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x53
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
Found a Stack Register!
typeRegWithIdx - requesting index = 0x54
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x55
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x56
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x57
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x58
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x59
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x5a
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x1000
typeRegWithIdx - requesting index = 0x1000
typeRegWithIdx - requesting index = 0x1000
Found a Stack Register!
typeRegWithIdx - requesting index = 0x54
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
Found a Stack Register!
typeRegWithIdx - requesting index = 0x53
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x5c
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
Found a Stack Register!
typeRegWithIdx - requesting index = 0x5b
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x55
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x5c
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x5c
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x56
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x1000
typeRegWithIdx - requesting index = 0x1000
typeRegWithIdx - requesting index = 0x1000
Found a Stack Register!
typeRegWithIdx - requesting index = 0x54
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
Found a Stack Register!
typeRegWithIdx - requesting index = 0x53
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x5e
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
Found a Stack Register!
typeRegWithIdx - requesting index = 0x5d
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x57
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x5e
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x5e
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x58
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x1000
typeRegWithIdx - requesting index = 0x1000
typeRegWithIdx - requesting index = 0x1000
Found a Stack Register!
typeRegWithIdx - requesting index = 0x54
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
Found a Stack Register!
typeRegWithIdx - requesting index = 0x53
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x60
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
Found a Stack Register!
typeRegWithIdx - requesting index = 0x5f
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x55
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x60
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x60
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x56
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x1000
typeRegWithIdx - requesting index = 0x1000
typeRegWithIdx - requesting index = 0x1000
Found a Stack Register!
typeRegWithIdx - requesting index = 0x54
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
Found a Stack Register!
typeRegWithIdx - requesting index = 0x53
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x62
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
Found a Stack Register!
typeRegWithIdx - requesting index = 0x61
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x57
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x62
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x62
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x58
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x59
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x5a
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x59
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x5a
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x56
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x55
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x56
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x55
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x56
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x58
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x57
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x58
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x57
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x58
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x57
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x55
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x58
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x58
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x56
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x55
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
Found a Stack Register!
typeRegWithIdx - requesting index = 0x56
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
Found a Stack Register!
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
Found a Stack Register!
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
Found a Stack Register!
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
Found a Stack Register!
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
Found a Stack Register!
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
Found a Stack Register!
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
Found a Stack Register!
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
Found a Stack Register!
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
Found a Stack Register!
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
Found a Stack Register!
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
Found a Stack Register!
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
Found a Stack Register!
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
Found a Stack Register!
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
Found a Stack Register!
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
Found a Stack Register!
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
Found a Stack Register!
typeRegWithIdx - requesting index = 0x6f
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x70
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6f
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x71
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6f
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x71
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6f
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x71
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x73
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x74
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6f
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x73
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6f
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x71
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6f
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x71
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6f
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x71
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x75
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x76
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6f
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x75
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6f
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x71
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6f
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x71
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6f
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x71
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x77
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x78
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6f
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x77
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6f
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x71
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6f
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x71
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6f
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x71
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x79
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x7a
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6f
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x79
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6f
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x71
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6f
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x71
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6f
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x71
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x7b
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x7c
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6f
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x7b
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6f
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x71
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6f
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x71
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6f
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x71
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x7d
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x7e
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6f
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x7d
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6f
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x71
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6f
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x71
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6f
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x71
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x7f
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x80
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6f
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x7f
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6f
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x71
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6f
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x71
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6f
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x71
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
Found a Stack Register!
typeRegWithIdx - requesting index = 0x63
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
Found a Stack Register!
typeRegWithIdx - requesting index = 0x63
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x65
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x1000
typeRegWithIdx - requesting index = 0x1000
typeRegWithIdx - requesting index = 0x1000
Found a Stack Register!
typeRegWithIdx - requesting index = 0x68
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x69
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x68
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x69
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x68
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x69
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6a
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x68
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x69
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x69
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
Found a Stack Register!
typeRegWithIdx - requesting index = 0x63
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
Found a Stack Register!
typeRegWithIdx - requesting index = 0x63
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x65
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x1000
typeRegWithIdx - requesting index = 0x1000
typeRegWithIdx - requesting index = 0x1000
Found a Stack Register!
typeRegWithIdx - requesting index = 0x68
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x69
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x68
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x69
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x68
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x69
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6a
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x68
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6a
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
Found a Stack Register!
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
Found a Stack Register!
typeRegWithIdx - requesting index = 0x68
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6a
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
Found a Stack Register!
typeRegWithIdx - requesting index = 0x6a
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
Found a Stack Register!
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
Found a Stack Register!
typeRegWithIdx - requesting index = 0x53
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
Found a Stack Register!
typeRegWithIdx - requesting index = 0x53
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x54
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
Found a Stack Register!
typeRegWithIdx - requesting index = 0x54
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x1000
typeRegWithIdx - requesting index = 0x1000
typeRegWithIdx - requesting index = 0x1000
Found a Stack Register!
typeRegWithIdx - requesting index = 0x54
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x1000
typeRegWithIdx - requesting index = 0x1000
typeRegWithIdx - requesting index = 0x1000
Found a Stack Register!
typeRegWithIdx - requesting index = 0x53
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
Found a Stack Register!
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
Found a Stack Register!
typeRegWithIdx - requesting index = 0x54
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x53
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
Found a Stack Register!
typeRegWithIdx - requesting index = 0x53
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
Found a Stack Register!
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
Found a Stack Register!
typeRegWithIdx - requesting index = 0x5c
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
Found a Stack Register!
typeRegWithIdx - requesting index = 0x5c
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x54
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x1000
typeRegWithIdx - requesting index = 0x1000
typeRegWithIdx - requesting index = 0x1000
Found a Stack Register!
typeRegWithIdx - requesting index = 0x54
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x1000
typeRegWithIdx - requesting index = 0x1000
typeRegWithIdx - requesting index = 0x1000
Found a Stack Register!
typeRegWithIdx - requesting index = 0x53
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
Found a Stack Register!
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
Found a Stack Register!
typeRegWithIdx - requesting index = 0x54
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x53
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
Found a Stack Register!
typeRegWithIdx - requesting index = 0x53
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
Found a Stack Register!
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
Found a Stack Register!
typeRegWithIdx - requesting index = 0x5e
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
Found a Stack Register!
typeRegWithIdx - requesting index = 0x5e
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x54
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x1000
typeRegWithIdx - requesting index = 0x1000
typeRegWithIdx - requesting index = 0x1000
Found a Stack Register!
typeRegWithIdx - requesting index = 0x54
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x1000
typeRegWithIdx - requesting index = 0x1000
typeRegWithIdx - requesting index = 0x1000
Found a Stack Register!
typeRegWithIdx - requesting index = 0x53
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
Found a Stack Register!
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
Found a Stack Register!
typeRegWithIdx - requesting index = 0x54
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x53
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
Found a Stack Register!
typeRegWithIdx - requesting index = 0x53
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
Found a Stack Register!
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
Found a Stack Register!
typeRegWithIdx - requesting index = 0x60
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
Found a Stack Register!
typeRegWithIdx - requesting index = 0x60
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x54
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x1000
typeRegWithIdx - requesting index = 0x1000
typeRegWithIdx - requesting index = 0x1000
Found a Stack Register!
typeRegWithIdx - requesting index = 0x54
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x1000
typeRegWithIdx - requesting index = 0x1000
typeRegWithIdx - requesting index = 0x1000
Found a Stack Register!
typeRegWithIdx - requesting index = 0x53
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
Found a Stack Register!
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
Found a Stack Register!
typeRegWithIdx - requesting index = 0x54
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x53
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
Found a Stack Register!
typeRegWithIdx - requesting index = 0x53
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
Found a Stack Register!
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
Found a Stack Register!
typeRegWithIdx - requesting index = 0x62
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
Found a Stack Register!
typeRegWithIdx - requesting index = 0x62
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x59
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x5a
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x56
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
Found a Stack Register!
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
Found a Stack Register!
typeRegWithIdx - requesting index = 0x55
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x56
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
Found a Stack Register!
typeRegWithIdx - requesting index = 0x56
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
Found a Stack Register!
typeRegWithIdx - requesting index = 0x6f
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x70
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6f
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x71
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6f
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x71
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6f
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x71
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x73
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x74
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6f
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x73
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6f
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x71
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6f
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x71
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6f
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x71
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x75
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x76
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6f
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x75
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6f
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x71
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6f
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x71
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6f
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x71
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x77
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x78
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6f
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x77
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6f
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x71
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6f
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x71
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6f
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x71
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x79
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x7a
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6f
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x79
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6f
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x71
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6f
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x71
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6f
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x71
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x7b
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x7c
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6f
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x7b
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6f
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x71
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6f
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x71
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6f
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x71
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x7d
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x7e
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6f
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x7d
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6f
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x71
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6f
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x71
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6f
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x71
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x7f
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x80
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6f
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x7f
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6f
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x71
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6f
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x71
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6f
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x71
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
Found a Stack Register!
typeRegWithIdx - requesting index = 0x63
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
Found a Stack Register!
typeRegWithIdx - requesting index = 0x63
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x65
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x1000
typeRegWithIdx - requesting index = 0x1000
typeRegWithIdx - requesting index = 0x1000
Found a Stack Register!
typeRegWithIdx - requesting index = 0x68
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x69
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x68
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x69
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x68
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x69
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6a
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x68
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6a
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
Found a Stack Register!
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
Found a Stack Register!
typeRegWithIdx - requesting index = 0x68
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6a
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
Found a Stack Register!
typeRegWithIdx - requesting index = 0x6a
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
Found a Stack Register!
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
Found a Stack Register!
typeRegWithIdx - requesting index = 0x53
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
Found a Stack Register!
typeRegWithIdx - requesting index = 0x53
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x54
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
Found a Stack Register!
typeRegWithIdx - requesting index = 0x54
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x1000
typeRegWithIdx - requesting index = 0x1000
typeRegWithIdx - requesting index = 0x1000
Found a Stack Register!
typeRegWithIdx - requesting index = 0x54
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x1000
typeRegWithIdx - requesting index = 0x1000
typeRegWithIdx - requesting index = 0x1000
Found a Stack Register!
typeRegWithIdx - requesting index = 0x53
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
Found a Stack Register!
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
Found a Stack Register!
typeRegWithIdx - requesting index = 0x54
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x53
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
Found a Stack Register!
typeRegWithIdx - requesting index = 0x53
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
Found a Stack Register!
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
Found a Stack Register!
typeRegWithIdx - requesting index = 0x5c
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
Found a Stack Register!
typeRegWithIdx - requesting index = 0x5c
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x54
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x1000
typeRegWithIdx - requesting index = 0x1000
typeRegWithIdx - requesting index = 0x1000
Found a Stack Register!
typeRegWithIdx - requesting index = 0x54
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x1000
typeRegWithIdx - requesting index = 0x1000
typeRegWithIdx - requesting index = 0x1000
Found a Stack Register!
typeRegWithIdx - requesting index = 0x53
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
Found a Stack Register!
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
Found a Stack Register!
typeRegWithIdx - requesting index = 0x54
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x53
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
Found a Stack Register!
typeRegWithIdx - requesting index = 0x53
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
Found a Stack Register!
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
Found a Stack Register!
typeRegWithIdx - requesting index = 0x5e
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
Found a Stack Register!
typeRegWithIdx - requesting index = 0x5e
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x54
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x1000
typeRegWithIdx - requesting index = 0x1000
typeRegWithIdx - requesting index = 0x1000
Found a Stack Register!
typeRegWithIdx - requesting index = 0x54
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x1000
typeRegWithIdx - requesting index = 0x1000
typeRegWithIdx - requesting index = 0x1000
Found a Stack Register!
typeRegWithIdx - requesting index = 0x53
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
Found a Stack Register!
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
Found a Stack Register!
typeRegWithIdx - requesting index = 0x54
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x53
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
Found a Stack Register!
typeRegWithIdx - requesting index = 0x53
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
Found a Stack Register!
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
Found a Stack Register!
typeRegWithIdx - requesting index = 0x60
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
Found a Stack Register!
typeRegWithIdx - requesting index = 0x60
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x54
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x1000
typeRegWithIdx - requesting index = 0x1000
typeRegWithIdx - requesting index = 0x1000
Found a Stack Register!
typeRegWithIdx - requesting index = 0x54
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x1000
typeRegWithIdx - requesting index = 0x1000
typeRegWithIdx - requesting index = 0x1000
Found a Stack Register!
typeRegWithIdx - requesting index = 0x53
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
Found a Stack Register!
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
Found a Stack Register!
typeRegWithIdx - requesting index = 0x54
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x53
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
Found a Stack Register!
typeRegWithIdx - requesting index = 0x53
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
Found a Stack Register!
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
Found a Stack Register!
typeRegWithIdx - requesting index = 0x62
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
Found a Stack Register!
typeRegWithIdx - requesting index = 0x62
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x59
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x5a
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x56
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
Found a Stack Register!
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
Found a Stack Register!
typeRegWithIdx - requesting index = 0x55
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x56
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
Found a Stack Register!
typeRegWithIdx - requesting index = 0x56
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
Found a Stack Register!
typeRegWithIdx - requesting index = 0x6f
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x70
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6f
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x71
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6f
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x71
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6f
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x71
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x73
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x74
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6f
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x73
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6f
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x71
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6f
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x71
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6f
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x71
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x75
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x76
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6f
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x75
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6f
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x71
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6f
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x71
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6f
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x71
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x77
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x78
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6f
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x77
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6f
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x71
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6f
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x71
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6f
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x71
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x79
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x71
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x79
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x71
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x79
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x71
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6f
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x79
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x7b
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x71
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x7b
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x71
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x7b
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x71
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6f
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x7b
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x7d
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x71
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x7d
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x71
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x7d
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x71
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6f
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x7d
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x7f
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x71
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x7f
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x71
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x7f
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x71
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6f
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x7f
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
Found a Stack Register!
typeRegWithIdx - requesting index = 0x63
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
Found a Stack Register!
typeRegWithIdx - requesting index = 0x63
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x65
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x1000
typeRegWithIdx - requesting index = 0x1000
typeRegWithIdx - requesting index = 0x1000
Found a Stack Register!
typeRegWithIdx - requesting index = 0x6a
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x68
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6a
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
Found a Stack Register!
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
Found a Stack Register!
typeRegWithIdx - requesting index = 0x68
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6a
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
Found a Stack Register!
typeRegWithIdx - requesting index = 0x6a
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
Found a Stack Register!
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
Found a Stack Register!
typeRegWithIdx - requesting index = 0x53
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
Found a Stack Register!
typeRegWithIdx - requesting index = 0x53
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x54
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
Found a Stack Register!
typeRegWithIdx - requesting index = 0x54
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x1000
typeRegWithIdx - requesting index = 0x1000
typeRegWithIdx - requesting index = 0x1000
Found a Stack Register!
typeRegWithIdx - requesting index = 0x54
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x1000
typeRegWithIdx - requesting index = 0x1000
typeRegWithIdx - requesting index = 0x1000
Found a Stack Register!
typeRegWithIdx - requesting index = 0x53
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
Found a Stack Register!
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
Found a Stack Register!
typeRegWithIdx - requesting index = 0x54
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x53
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
Found a Stack Register!
typeRegWithIdx - requesting index = 0x53
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
Found a Stack Register!
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
Found a Stack Register!
typeRegWithIdx - requesting index = 0x5c
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
Found a Stack Register!
typeRegWithIdx - requesting index = 0x5c
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x54
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x1000
typeRegWithIdx - requesting index = 0x1000
typeRegWithIdx - requesting index = 0x1000
Found a Stack Register!
typeRegWithIdx - requesting index = 0x54
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x1000
typeRegWithIdx - requesting index = 0x1000
typeRegWithIdx - requesting index = 0x1000
Found a Stack Register!
typeRegWithIdx - requesting index = 0x53
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
Found a Stack Register!
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
Found a Stack Register!
typeRegWithIdx - requesting index = 0x54
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x53
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
Found a Stack Register!
typeRegWithIdx - requesting index = 0x53
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
Found a Stack Register!
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
Found a Stack Register!
typeRegWithIdx - requesting index = 0x5e
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
Found a Stack Register!
typeRegWithIdx - requesting index = 0x5e
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x54
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x1000
typeRegWithIdx - requesting index = 0x1000
typeRegWithIdx - requesting index = 0x1000
Found a Stack Register!
typeRegWithIdx - requesting index = 0x54
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x1000
typeRegWithIdx - requesting index = 0x1000
typeRegWithIdx - requesting index = 0x1000
Found a Stack Register!
typeRegWithIdx - requesting index = 0x53
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
Found a Stack Register!
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
Found a Stack Register!
typeRegWithIdx - requesting index = 0x54
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x53
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
Found a Stack Register!
typeRegWithIdx - requesting index = 0x53
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
Found a Stack Register!
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
Found a Stack Register!
typeRegWithIdx - requesting index = 0x60
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
Found a Stack Register!
typeRegWithIdx - requesting index = 0x60
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x54
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x1000
typeRegWithIdx - requesting index = 0x1000
typeRegWithIdx - requesting index = 0x1000
Found a Stack Register!
typeRegWithIdx - requesting index = 0x54
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x1000
typeRegWithIdx - requesting index = 0x1000
typeRegWithIdx - requesting index = 0x1000
Found a Stack Register!
typeRegWithIdx - requesting index = 0x53
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
Found a Stack Register!
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
Found a Stack Register!
typeRegWithIdx - requesting index = 0x54
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x53
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
Found a Stack Register!
typeRegWithIdx - requesting index = 0x53
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
Found a Stack Register!
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
Found a Stack Register!
typeRegWithIdx - requesting index = 0x62
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
Found a Stack Register!
typeRegWithIdx - requesting index = 0x62
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x59
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x5a
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x56
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
Found a Stack Register!
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
Found a Stack Register!
typeRegWithIdx - requesting index = 0x55
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x56
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
Found a Stack Register!
typeRegWithIdx - requesting index = 0x56
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
Found a Stack Register!
typeRegWithIdx - requesting index = 0x6
typeRegWithIdx - requesting index = 0xb
typeRegWithIdx - requesting index = 0x1f
typeRegWithIdx - requesting index = 0x9
typeRegWithIdx - requesting index = 0x89
typeRegWithIdx - requesting index = 0x6f
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x70
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x70
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6f
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x71
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6f
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x71
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x71
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6f
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x72
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x73
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x74
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x74
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x73
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6f
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x71
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6f
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x71
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x71
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6f
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x72
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x75
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x76
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x76
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x75
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6f
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x71
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6f
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x71
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x71
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6f
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x72
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x77
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x78
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x78
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x77
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6f
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x71
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6f
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x71
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x71
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6f
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x72
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x79
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x7a
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x7a
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x71
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x79
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6f
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x71
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x71
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6f
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x72
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x7b
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x7c
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x7c
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x71
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x7b
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6f
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x71
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x71
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6f
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x72
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x7d
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x7e
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x7e
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x71
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x7d
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6f
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x71
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x71
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6f
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x72
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x7f
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x80
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x80
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x71
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x7f
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6f
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x71
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x71
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6f
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x72
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x72
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6d
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6e
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6d
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6e
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6c
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6c
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x63
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
Found a Stack Register!
typeRegWithIdx - requesting index = 0x66
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x66
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x1000
typeRegWithIdx - requesting index = 0x1000
typeRegWithIdx - requesting index = 0x1000
Found a Stack Register!
typeRegWithIdx - requesting index = 0x65
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x65
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x65
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x65
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x67
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x63
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x67
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6a
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x68
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6a
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x68
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x68
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6a
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x68
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
Found a Stack Register!
typeRegWithIdx - requesting index = 0x6a
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x53
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
Found a Stack Register!
typeRegWithIdx - requesting index = 0x54
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x55
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x56
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x57
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x58
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x59
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x5a
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x1000
typeRegWithIdx - requesting index = 0x1000
typeRegWithIdx - requesting index = 0x1000
Found a Stack Register!
typeRegWithIdx - requesting index = 0x54
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
Found a Stack Register!
typeRegWithIdx - requesting index = 0x53
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x5c
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
Found a Stack Register!
typeRegWithIdx - requesting index = 0x55
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x5c
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x5c
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x56
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x1000
typeRegWithIdx - requesting index = 0x1000
typeRegWithIdx - requesting index = 0x1000
Found a Stack Register!
typeRegWithIdx - requesting index = 0x54
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
Found a Stack Register!
typeRegWithIdx - requesting index = 0x53
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x5e
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
Found a Stack Register!
typeRegWithIdx - requesting index = 0x57
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x5e
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x5e
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x58
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x1000
typeRegWithIdx - requesting index = 0x1000
typeRegWithIdx - requesting index = 0x1000
Found a Stack Register!
typeRegWithIdx - requesting index = 0x54
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
Found a Stack Register!
typeRegWithIdx - requesting index = 0x53
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x60
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
Found a Stack Register!
typeRegWithIdx - requesting index = 0x55
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x60
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x60
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x56
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x1000
typeRegWithIdx - requesting index = 0x1000
typeRegWithIdx - requesting index = 0x1000
Found a Stack Register!
typeRegWithIdx - requesting index = 0x54
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
Found a Stack Register!
typeRegWithIdx - requesting index = 0x53
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x62
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
Found a Stack Register!
typeRegWithIdx - requesting index = 0x57
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x62
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x62
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x58
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x59
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x5a
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x59
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x5a
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x56
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x55
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x56
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x55
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x56
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x58
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x57
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x58
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x57
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x58
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x57
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x55
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x58
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x58
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x56
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x55
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
Found a Stack Register!
typeRegWithIdx - requesting index = 0x56
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
Found a Stack Register!
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
Found a Stack Register!
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
Found a Stack Register!
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
Found a Stack Register!
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
Found a Stack Register!
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
Found a Stack Register!
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
Found a Stack Register!
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
Found a Stack Register!
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
Found a Stack Register!
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
Found a Stack Register!
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
Found a Stack Register!
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
Found a Stack Register!
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
Found a Stack Register!
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
Found a Stack Register!
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
Found a Stack Register!
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
Found a Stack Register!
