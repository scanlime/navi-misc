<><><><><><><><><><><><><><><><><>
starting	ralloc.c:pic14_assignRegisters
ebbs before optimizing:

----------------------------------------------------------------
Basic Block _entry : loop Depth = 0 noPath = 0 , lastinLoop = 0
depth 1st num 2 : bbnum = 0 1st iCode = 1 , last iCode = 4
visited 0 : hasFcall = 1

defines bitVector :bitvector Size = 15 bSize = 2
Bits on { (4) }

local defines bitVector :bitvector Size = 15 bSize = 2
Bits on { (2) (4) }

pointers Set bitvector :
in pointers Set bitvector :
inDefs Set bitvector :
outDefs Set bitvector :bitvector Size = 15 bSize = 2
Bits on { (2) (4) }

usesDefs Set bitvector :bitvector Size = 15 bSize = 2
Bits on { (2) }

----------------------------------------------------------------
main.c(96:1:0:0:0)	 _entry($5) :
main.c(96:2:1:0:0)		proc _main [k1 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{void function    }
main.c(97:3:2:0:0)		iTemp0 [k3 lr3:3 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{void} = call _initialize [k2 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{void function    }
main.c(99:4:4:0:0)		_Light0 [k4 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{bit bit {0,1}} := 0x1 {literal bit {0,1}}

----------------------------------------------------------------
Basic Block preHeaderLbl0 : loop Depth = 0 noPath = 0 , lastinLoop = 0
depth 1st num 3 : bbnum = 1 1st iCode = 5 , last iCode = 5
visited 0 : hasFcall = 0

defines bitVector :bitvector Size = 15 bSize = 2
Bits on { }

local defines bitVector :
pointers Set bitvector :
in pointers Set bitvector :
inDefs Set bitvector :bitvector Size = 15 bSize = 2
Bits on { (2) (4) }

outDefs Set bitvector :bitvector Size = 15 bSize = 2
Bits on { (2) (4) }

usesDefs Set bitvector :bitvector Size = 15 bSize = 2
Bits on { }

----------------------------------------------------------------
main.c(118:5:14:0:0)	 preHeaderLbl0($6) :

----------------------------------------------------------------
Basic Block _whilebody_0 : loop Depth = 1 noPath = 0 , lastinLoop = 0
depth 1st num 4 : bbnum = 2 1st iCode = 6 , last iCode = 9
visited 0 : hasFcall = 1

defines bitVector :bitvector Size = 15 bSize = 2
Bits on { }

local defines bitVector :bitvector Size = 15 bSize = 2
Bits on { (8) (9) }

pointers Set bitvector :
in pointers Set bitvector :
inDefs Set bitvector :bitvector Size = 15 bSize = 2
Bits on { (2) (3) (4) (8) (9) }

outDefs Set bitvector :bitvector Size = 15 bSize = 2
Bits on { (2) (3) (4) (8) (9) }

usesDefs Set bitvector :bitvector Size = 15 bSize = 2
Bits on { (8) (9) }

----------------------------------------------------------------
main.c(118:6:7:1:0)	 _whilebody_0($2) :
main.c(119:7:8:1:0)		iTemp2 [k7 lr7:7 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{void} = call _sampleAll [k6 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{void function    }
main.c(120:8:9:1:0)		iTemp3 [k9 lr8:8 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{void} = call _sendPacket [k8 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{void function    }
main.c(120:9:10:1:0)		 goto _whilebody_0($2)

----------------------------------------------------------------
Basic Block _return : loop Depth = 0 noPath = 1 , lastinLoop = 0
depth 1st num 0 : bbnum = 3 1st iCode = 10 , last iCode = 11
visited 0 : hasFcall = 0

defines bitVector :
local defines bitVector :
pointers Set bitvector :
in pointers Set bitvector :
inDefs Set bitvector :
outDefs Set bitvector :
usesDefs Set bitvector :
----------------------------------------------------------------
main.c(120:10:12:0:0)	 _return($4) :
main.c(120:11:13:0:0)		eproc _main [k1 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{void function    }
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
  result:iTemp0 [k3 lr3:3 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{void}
          Symbol type: void
  3594 - pointer reg req = 0
  result:_Light0 [k4 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{bit bit {0,1}}
          Symbol type: bit bit {0,1}
  3594 - pointer reg req = 0
packRegisters
packRegisters
  left:_sampleAll [k6 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{void function    }
        Symbol type: void function    
  result:iTemp2 [k7 lr7:7 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{void}
          Symbol type: void
  3594 - pointer reg req = 0
  left:_sendPacket [k8 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{void function    }
        Symbol type: void function    
  result:iTemp3 [k9 lr8:8 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{void}
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
depth 1st num 2 : bbnum = 0 1st iCode = 1 , last iCode = 4
visited 0 : hasFcall = 1

defines bitVector :bitvector Size = 15 bSize = 2
Bits on { (4) }

local defines bitVector :bitvector Size = 15 bSize = 2
Bits on { (2) (4) }

pointers Set bitvector :
in pointers Set bitvector :
inDefs Set bitvector :
outDefs Set bitvector :bitvector Size = 15 bSize = 2
Bits on { (2) (4) }

usesDefs Set bitvector :bitvector Size = 15 bSize = 2
Bits on { (2) }

----------------------------------------------------------------
main.c(96:1:0:0:0)	 _entry($5) :
main.c(96:2:1:0:0)		proc _main [k1 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{void function    }
main.c(97:3:2:0:0)		iTemp0 [k3 lr3:3 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{void} = call _initialize [k2 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{void function    }
main.c(99:4:4:0:0)		_Light0 [k4 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{bit bit {0,1}} := 0x1 {literal bit {0,1}}
main.c(118:6:7:1:0)	 _whilebody_0($2) :
main.c(119:7:8:1:0)		iTemp2 [k7 lr7:7 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{void} = call _sampleAll [k6 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{void function    }
main.c(120:8:9:1:0)		iTemp3 [k9 lr8:8 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{void} = call _sendPacket [k8 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{void function    }
main.c(120:9:10:1:0)		 goto _whilebody_0($2)
main.c(120:10:12:0:0)	 _return($4) :
main.c(120:11:13:0:0)		eproc _main [k1 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{void function    }

----------------------------------------------------------------
Basic Block preHeaderLbl0 : loop Depth = 0 noPath = 0 , lastinLoop = 0
depth 1st num 3 : bbnum = 1 1st iCode = 5 , last iCode = 5
visited 0 : hasFcall = 0

defines bitVector :bitvector Size = 15 bSize = 2
Bits on { }

local defines bitVector :
pointers Set bitvector :
in pointers Set bitvector :
inDefs Set bitvector :bitvector Size = 15 bSize = 2
Bits on { (2) (4) }

outDefs Set bitvector :bitvector Size = 15 bSize = 2
Bits on { (2) (4) }

usesDefs Set bitvector :bitvector Size = 15 bSize = 2
Bits on { }

----------------------------------------------------------------
main.c(118:5:14:0:0)	 preHeaderLbl0($6) :
main.c(118:6:7:1:0)	 _whilebody_0($2) :
main.c(119:7:8:1:0)		iTemp2 [k7 lr7:7 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{void} = call _sampleAll [k6 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{void function    }
main.c(120:8:9:1:0)		iTemp3 [k9 lr8:8 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{void} = call _sendPacket [k8 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{void function    }
main.c(120:9:10:1:0)		 goto _whilebody_0($2)
main.c(120:10:12:0:0)	 _return($4) :
main.c(120:11:13:0:0)		eproc _main [k1 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{void function    }

----------------------------------------------------------------
Basic Block _whilebody_0 : loop Depth = 1 noPath = 0 , lastinLoop = 0
depth 1st num 4 : bbnum = 2 1st iCode = 6 , last iCode = 9
visited 0 : hasFcall = 1

defines bitVector :bitvector Size = 15 bSize = 2
Bits on { }

local defines bitVector :bitvector Size = 15 bSize = 2
Bits on { (8) (9) }

pointers Set bitvector :
in pointers Set bitvector :
inDefs Set bitvector :bitvector Size = 15 bSize = 2
Bits on { (2) (3) (4) (8) (9) }

outDefs Set bitvector :bitvector Size = 15 bSize = 2
Bits on { (2) (3) (4) (8) (9) }

usesDefs Set bitvector :bitvector Size = 15 bSize = 2
Bits on { (8) (9) }

----------------------------------------------------------------
main.c(118:6:7:1:0)	 _whilebody_0($2) :
main.c(119:7:8:1:0)		iTemp2 [k7 lr7:7 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{void} = call _sampleAll [k6 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{void function    }
main.c(120:8:9:1:0)		iTemp3 [k9 lr8:8 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{void} = call _sendPacket [k8 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{void function    }
main.c(120:9:10:1:0)		 goto _whilebody_0($2)
main.c(120:10:12:0:0)	 _return($4) :
main.c(120:11:13:0:0)		eproc _main [k1 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{void function    }

----------------------------------------------------------------
Basic Block _return : loop Depth = 0 noPath = 1 , lastinLoop = 0
depth 1st num 0 : bbnum = 3 1st iCode = 10 , last iCode = 11
visited 0 : hasFcall = 0

defines bitVector :
local defines bitVector :
pointers Set bitvector :
in pointers Set bitvector :
inDefs Set bitvector :
outDefs Set bitvector :
usesDefs Set bitvector :
----------------------------------------------------------------
main.c(120:10:12:0:0)	 _return($4) :
main.c(120:11:13:0:0)		eproc _main [k1 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{void function    }
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
main.c(126:1:0:0:0)	 _entry($2) :
main.c(126:2:1:0:0)		proc _initialize [k1 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{void function    }
main.c(128:3:2:0:0)		_TRISA [k2 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{sfr volatile unsigned char} := 0xff {literal unsigned char}
main.c(129:4:3:0:0)		_TRISB [k3 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{sfr volatile unsigned char} := 0x0 {literal unsigned char}
main.c(130:5:4:0:0)		_TRISC [k4 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{sfr volatile unsigned char} := 0x0 {literal unsigned char}
main.c(131:6:5:0:0)		_TRISD [k5 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{sfr volatile unsigned char} := 0x0 {literal unsigned char}
main.c(132:7:6:0:0)		_TRISE [k6 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{sfr volatile unsigned char} := 0x4 {literal unsigned char}
main.c(135:8:7:0:0)		_PORTA [k7 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{sfr volatile unsigned char} := 0x0 {literal unsigned char}
main.c(136:9:8:0:0)		_PORTB [k8 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{sfr volatile unsigned char} := 0x0 {literal unsigned char}
main.c(137:10:9:0:0)		_PORTC [k9 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{sfr volatile unsigned char} := 0x0 {literal unsigned char}
main.c(138:11:10:0:0)		_PORTD [k10 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{sfr volatile unsigned char} := 0x0 {literal unsigned char}
main.c(139:12:11:0:0)		_PORTE [k11 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{sfr volatile unsigned char} := 0x0 {literal unsigned char}
main.c(142:13:12:0:0)		_ADCON1 [k12 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{sfr volatile unsigned char} := 0x82 {literal unsigned char}
main.c(143:14:13:0:0)		_ADCON0 [k13 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{sfr volatile unsigned char} := 0x81 {literal unsigned char}
main.c(144:15:15:0:0)		_GIE [k14 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{bit bit {0,1}} := 0x1 {literal bit {0,1}}

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
main.c(144:16:16:0:0)	 _return($1) :
main.c(144:17:17:0:0)		eproc _initialize [k1 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{void function    }
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
main.c(126:1:0:0:0)	 _entry($2) :
main.c(126:2:1:0:0)		proc _initialize [k1 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{void function    }
main.c(128:3:2:0:0)		_TRISA [k2 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{sfr volatile unsigned char} := 0xff {literal unsigned char}
main.c(129:4:3:0:0)		_TRISB [k3 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{sfr volatile unsigned char} := 0x0 {literal unsigned char}
main.c(130:5:4:0:0)		_TRISC [k4 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{sfr volatile unsigned char} := 0x0 {literal unsigned char}
main.c(131:6:5:0:0)		_TRISD [k5 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{sfr volatile unsigned char} := 0x0 {literal unsigned char}
main.c(132:7:6:0:0)		_TRISE [k6 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{sfr volatile unsigned char} := 0x4 {literal unsigned char}
main.c(135:8:7:0:0)		_PORTA [k7 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{sfr volatile unsigned char} := 0x0 {literal unsigned char}
main.c(136:9:8:0:0)		_PORTB [k8 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{sfr volatile unsigned char} := 0x0 {literal unsigned char}
main.c(137:10:9:0:0)		_PORTC [k9 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{sfr volatile unsigned char} := 0x0 {literal unsigned char}
main.c(138:11:10:0:0)		_PORTD [k10 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{sfr volatile unsigned char} := 0x0 {literal unsigned char}
main.c(139:12:11:0:0)		_PORTE [k11 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{sfr volatile unsigned char} := 0x0 {literal unsigned char}
main.c(142:13:12:0:0)		_ADCON1 [k12 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{sfr volatile unsigned char} := 0x82 {literal unsigned char}
main.c(143:14:13:0:0)		_ADCON0 [k13 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{sfr volatile unsigned char} := 0x81 {literal unsigned char}
main.c(144:15:15:0:0)		_GIE [k14 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{bit bit {0,1}} := 0x1 {literal bit {0,1}}
main.c(144:16:16:0:0)	 _return($1) :
main.c(144:17:17:0:0)		eproc _initialize [k1 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{void function    }

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
main.c(144:16:16:0:0)	 _return($1) :
main.c(144:17:17:0:0)		eproc _initialize [k1 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{void function    }
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
main.c(151:1:0:0:0)	 _entry($2) :
main.c(151:2:1:0:0)		proc _sampleAll [k1 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{void function    }
main.c(152:3:2:0:0)		send 0x10 {literal unsigned char}{argreg = 1}
main.c(152:4:3:0:0)		send 0x81 {literal unsigned char}{argreg = 1}
main.c(152:5:4:0:0)		iTemp0 [k4 lr5:6 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int} = call _integrateSample [k3 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int function    }
main.c(152:6:5:0:0)		iTemp1 [k5 lr6:7 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int}{ sir@ _reading0} := iTemp0 [k4 lr5:6 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int}
main.c(152:7:6:0:0)		_reading0 [k2 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{int} := iTemp1 [k5 lr6:7 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int}{ sir@ _reading0}
main.c(153:8:7:0:0)		send 0x20 {literal unsigned char}{argreg = 1}
main.c(153:9:8:0:0)		send 0x81 {literal unsigned char}{argreg = 1}
main.c(153:10:9:0:0)		iTemp2 [k7 lr10:11 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int} = call _integrateSample [k3 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int function    }
main.c(153:11:10:0:0)		iTemp3 [k8 lr11:12 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int}{ sir@ _reading1} := iTemp2 [k7 lr10:11 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int}
main.c(153:12:11:0:0)		_reading1 [k6 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{int} := iTemp3 [k8 lr11:12 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int}{ sir@ _reading1}
main.c(154:13:12:0:0)		send 0x40 {literal unsigned char}{argreg = 1}
main.c(154:14:13:0:0)		send 0x81 {literal unsigned char}{argreg = 1}
main.c(154:15:14:0:0)		iTemp4 [k10 lr15:16 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int} = call _integrateSample [k3 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int function    }
main.c(154:16:15:0:0)		iTemp5 [k11 lr16:17 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int}{ sir@ _reading2} := iTemp4 [k10 lr15:16 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int}
main.c(154:17:16:0:0)		_reading2 [k9 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{int} := iTemp5 [k11 lr16:17 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int}{ sir@ _reading2}
main.c(155:18:17:0:0)		send 0x80 {literal unsigned char}{argreg = 1}
main.c(155:19:18:0:0)		send 0x81 {literal unsigned char}{argreg = 1}
main.c(155:20:19:0:0)		iTemp6 [k13 lr20:21 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int} = call _integrateSample [k3 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int function    }
main.c(155:21:20:0:0)		iTemp7 [k14 lr21:22 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int}{ sir@ _reading3} := iTemp6 [k13 lr20:21 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int}
main.c(155:22:21:0:0)		_reading3 [k12 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{int} := iTemp7 [k14 lr21:22 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int}{ sir@ _reading3}
main.c(156:23:22:0:0)		send 0x10 {literal unsigned char}{argreg = 1}
main.c(156:24:23:0:0)		send 0x89 {literal unsigned char}{argreg = 1}
main.c(156:25:24:0:0)		iTemp8 [k16 lr25:26 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int} = call _integrateSample [k3 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int function    }
main.c(156:26:25:0:0)		iTemp9 [k17 lr26:27 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int}{ sir@ _reading4} := iTemp8 [k16 lr25:26 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int}
main.c(156:27:26:0:0)		_reading4 [k15 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{int} := iTemp9 [k17 lr26:27 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int}{ sir@ _reading4}
main.c(157:28:27:0:0)		send 0x20 {literal unsigned char}{argreg = 1}
main.c(157:29:28:0:0)		send 0x89 {literal unsigned char}{argreg = 1}
main.c(157:30:29:0:0)		iTemp10 [k19 lr30:31 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int} = call _integrateSample [k3 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int function    }
main.c(157:31:30:0:0)		iTemp11 [k20 lr31:32 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int}{ sir@ _reading5} := iTemp10 [k19 lr30:31 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int}
main.c(157:32:31:0:0)		_reading5 [k18 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{int} := iTemp11 [k20 lr31:32 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int}{ sir@ _reading5}
main.c(158:33:32:0:0)		send 0x40 {literal unsigned char}{argreg = 1}
main.c(158:34:33:0:0)		send 0x89 {literal unsigned char}{argreg = 1}
main.c(158:35:34:0:0)		iTemp12 [k22 lr35:36 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int} = call _integrateSample [k3 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int function    }
main.c(158:36:35:0:0)		iTemp13 [k23 lr36:37 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int}{ sir@ _reading6} := iTemp12 [k22 lr35:36 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int}
main.c(158:37:36:0:0)		_reading6 [k21 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{int} := iTemp13 [k23 lr36:37 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int}{ sir@ _reading6}
main.c(159:38:37:0:0)		send 0x80 {literal unsigned char}{argreg = 1}
main.c(159:39:38:0:0)		send 0x89 {literal unsigned char}{argreg = 1}
main.c(159:40:39:0:0)		iTemp14 [k25 lr40:41 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int} = call _integrateSample [k3 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int function    }
main.c(159:41:40:0:0)		iTemp15 [k26 lr41:42 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int}{ sir@ _reading7} := iTemp14 [k25 lr40:41 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int}
main.c(159:42:41:0:0)		_reading7 [k24 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{int} := iTemp15 [k26 lr41:42 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int}{ sir@ _reading7}

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
main.c(159:43:42:0:0)	 _return($1) :
main.c(159:44:43:0:0)		eproc _sampleAll [k1 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{void function    }
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
main.c(151:1:0:0:0)	 _entry($2) :
main.c(151:2:1:0:0)		proc _sampleAll [k1 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{void function    }
main.c(152:3:2:0:0)		send 0x10 {literal unsigned char}{argreg = 1}
main.c(152:4:3:0:0)		send 0x81 {literal unsigned char}{argreg = 1}
main.c(152:5:4:0:0)		_reading0 [k2 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{int} = call _integrateSample [k3 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int function    }
main.c(153:8:7:0:0)		send 0x20 {literal unsigned char}{argreg = 1}
main.c(153:9:8:0:0)		send 0x81 {literal unsigned char}{argreg = 1}
main.c(153:10:9:0:0)		_reading1 [k6 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{int} = call _integrateSample [k3 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int function    }
main.c(154:13:12:0:0)		send 0x40 {literal unsigned char}{argreg = 1}
main.c(154:14:13:0:0)		send 0x81 {literal unsigned char}{argreg = 1}
main.c(154:15:14:0:0)		_reading2 [k9 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{int} = call _integrateSample [k3 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int function    }
main.c(155:18:17:0:0)		send 0x80 {literal unsigned char}{argreg = 1}
main.c(155:19:18:0:0)		send 0x81 {literal unsigned char}{argreg = 1}
main.c(155:20:19:0:0)		_reading3 [k12 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{int} = call _integrateSample [k3 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int function    }
main.c(156:23:22:0:0)		send 0x10 {literal unsigned char}{argreg = 1}
main.c(156:24:23:0:0)		send 0x89 {literal unsigned char}{argreg = 1}
main.c(156:25:24:0:0)		_reading4 [k15 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{int} = call _integrateSample [k3 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int function    }
main.c(157:28:27:0:0)		send 0x20 {literal unsigned char}{argreg = 1}
main.c(157:29:28:0:0)		send 0x89 {literal unsigned char}{argreg = 1}
main.c(157:30:29:0:0)		_reading5 [k18 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{int} = call _integrateSample [k3 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int function    }
main.c(158:33:32:0:0)		send 0x40 {literal unsigned char}{argreg = 1}
main.c(158:34:33:0:0)		send 0x89 {literal unsigned char}{argreg = 1}
main.c(158:35:34:0:0)		_reading6 [k21 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{int} = call _integrateSample [k3 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int function    }
main.c(159:38:37:0:0)		send 0x80 {literal unsigned char}{argreg = 1}
main.c(159:39:38:0:0)		send 0x89 {literal unsigned char}{argreg = 1}
main.c(159:40:39:0:0)		_reading7 [k24 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{int} = call _integrateSample [k3 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int function    }
main.c(159:43:42:0:0)	 _return($1) :
main.c(159:44:43:0:0)		eproc _sampleAll [k1 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{void function    }

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
main.c(159:43:42:0:0)	 _return($1) :
main.c(159:44:43:0:0)		eproc _sampleAll [k1 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{void function    }
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
depth 1st num 1 : bbnum = 0 1st iCode = 1 , last iCode = 8
visited 0 : hasFcall = 0

defines bitVector :bitvector Size = 59 bSize = 8
Bits on { (2) (3) (4) (5) (6) (7) }

local defines bitVector :bitvector Size = 59 bSize = 8
Bits on { (2) (3) (4) (5) (6) (7) }

pointers Set bitvector :
in pointers Set bitvector :
inDefs Set bitvector :
outDefs Set bitvector :bitvector Size = 59 bSize = 8
Bits on { (2) (3) (4) (5) (6) (7) }

usesDefs Set bitvector :bitvector Size = 59 bSize = 8
Bits on { (2) (3) (6) }

----------------------------------------------------------------
main.c(163:1:0:0:0)	 _entry($9) :
main.c(163:2:1:0:0)		proc _integrateSample [k1 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int function    }
main.c(163:3:2:0:0)	iTemp0 [k2 lr3:37 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _integrateSample_transmitter_1_1} = recv 
main.c(163:4:3:0:0)	iTemp1 [k4 lr4:37 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _integrateSample_receiver_1_1} = recv 
main.c(164:5:4:0:0)		iTemp2 [k6 lr5:48 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{int}{ sir@ _integrateSample_i_1_1} := 0x0 {literal int}
main.c(164:6:5:0:0)		iTemp3 [k8 lr6:48 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{int}{ sir@ _integrateSample_q_1_1} := 0x0 {literal int}
main.c(165:7:6:0:0)		iTemp5 [k13 lr7:8 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int}{ sir@ _integrateSample_numsamples_1_1} = (int)_numIntegrationSamples [k12 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{unsigned char}
main.c(165:8:7:0:0)		iTemp4 [k10 lr8:10 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{int}{ sir@ _integrateSample_numsamples_1_1} := iTemp5 [k13 lr7:8 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int}{ sir@ _integrateSample_numsamples_1_1}

----------------------------------------------------------------
Basic Block preHeaderLbl0 : loop Depth = 0 noPath = 0 , lastinLoop = 0
depth 1st num 2 : bbnum = 1 1st iCode = 9 , last iCode = 10
visited 0 : hasFcall = 0

defines bitVector :bitvector Size = 59 bSize = 8
Bits on { (58) }

local defines bitVector :bitvector Size = 59 bSize = 8
Bits on { (58) }

pointers Set bitvector :
in pointers Set bitvector :
inDefs Set bitvector :bitvector Size = 59 bSize = 8
Bits on { (2) (3) (4) (5) (6) (7) }

outDefs Set bitvector :bitvector Size = 59 bSize = 8
Bits on { (2) (3) (4) (5) (6) (7) (58) }

usesDefs Set bitvector :bitvector Size = 59 bSize = 8
Bits on { (7) }

----------------------------------------------------------------
main.c(167:9:57:0:0)	 preHeaderLbl0($10) :
main.c(167:10:58:0:0)		iTemp14 [k23 lr10:37 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int}{ sir@ _integrateSample_numsamples_1_1} := iTemp4 [k10 lr8:10 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{int}{ sir@ _integrateSample_numsamples_1_1}

----------------------------------------------------------------
Basic Block _dobody_0 : loop Depth = 1 noPath = 0 , lastinLoop = 1
depth 1st num 3 : bbnum = 2 1st iCode = 11 , last iCode = 37
visited 0 : hasFcall = 1

defines bitVector :bitvector Size = 59 bSize = 8
Bits on { (12) (13) (18) (19) (24) (25) (26) (30) (31) (32) (34) }

local defines bitVector :bitvector Size = 59 bSize = 8
Bits on { (12) (13) (14) (18) (19) (20) (24) (25) (26) (30) (31) (32) (34) (35) }

pointers Set bitvector :
in pointers Set bitvector :
inDefs Set bitvector :bitvector Size = 59 bSize = 8
Bits on { (2) (3) (4) (5) (6) (7) (12) (13) (18) (19) (24) (25) (26) (30) (31) (32) (34) (35) (58) }

outDefs Set bitvector :bitvector Size = 59 bSize = 8
Bits on { (2) (3) (6) (12) (13) (18) (19) (24) (25) (26) (30) (31) (32) (34) }

usesDefs Set bitvector :bitvector Size = 59 bSize = 8
Bits on { (2) (3) (4) (5) (12) (13) (14) (18) (19) (20) (24) (25) (26) (30) (31) (32) (34) (58) }

----------------------------------------------------------------
main.c(167:11:8:1:0)	 _dobody_0($1) :
main.c(169:12:9:1:0)		send iTemp0 [k2 lr3:37 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _integrateSample_transmitter_1_1}{argreg = 1}
main.c(169:13:10:1:0)		send iTemp1 [k4 lr4:37 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _integrateSample_receiver_1_1}{argreg = 1}
main.c(169:14:11:1:0)		send 0x0 {literal unsigned char}{argreg = 1}
main.c(169:15:12:1:0)		iTemp6 [k15 lr15:16 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int} = call _sensorSample [k14 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int function    }
main.c(169:16:13:1:0)		iTemp7 [k16 lr16:17 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int} = iTemp2 [k6 lr5:48 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{int}{ sir@ _integrateSample_i_1_1} + iTemp6 [k15 lr15:16 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int}
main.c(169:17:14:1:0)		iTemp2 [k6 lr5:48 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{int}{ sir@ _integrateSample_i_1_1} := iTemp7 [k16 lr16:17 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int}
main.c(170:18:15:1:0)		send iTemp0 [k2 lr3:37 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _integrateSample_transmitter_1_1}{argreg = 1}
main.c(170:19:16:1:0)		send iTemp1 [k4 lr4:37 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _integrateSample_receiver_1_1}{argreg = 1}
main.c(170:20:17:1:0)		send 0x1 {literal unsigned char}{argreg = 1}
main.c(170:21:18:1:0)		iTemp8 [k17 lr21:22 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int} = call _sensorSample [k14 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int function    }
main.c(170:22:19:1:0)		iTemp9 [k18 lr22:23 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int} = iTemp3 [k8 lr6:48 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{int}{ sir@ _integrateSample_q_1_1} + iTemp8 [k17 lr21:22 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int}
main.c(170:23:20:1:0)		iTemp3 [k8 lr6:48 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{int}{ sir@ _integrateSample_q_1_1} := iTemp9 [k18 lr22:23 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int}
main.c(171:24:21:1:0)		send iTemp0 [k2 lr3:37 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _integrateSample_transmitter_1_1}{argreg = 1}
main.c(171:25:22:1:0)		send iTemp1 [k4 lr4:37 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _integrateSample_receiver_1_1}{argreg = 1}
main.c(171:26:23:1:0)		send 0x2 {literal unsigned char}{argreg = 1}
main.c(171:27:24:1:0)		iTemp10 [k19 lr27:28 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int} = call _sensorSample [k14 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int function    }
main.c(171:28:25:1:0)		iTemp11 [k20 lr28:29 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int} = iTemp2 [k6 lr5:48 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{int}{ sir@ _integrateSample_i_1_1} - iTemp10 [k19 lr27:28 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int}
main.c(171:29:26:1:0)		iTemp2 [k6 lr5:48 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{int}{ sir@ _integrateSample_i_1_1} := iTemp11 [k20 lr28:29 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int}
main.c(172:30:27:1:0)		send iTemp0 [k2 lr3:37 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _integrateSample_transmitter_1_1}{argreg = 1}
main.c(172:31:28:1:0)		send iTemp1 [k4 lr4:37 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _integrateSample_receiver_1_1}{argreg = 1}
main.c(172:32:29:1:0)		send 0x3 {literal unsigned char}{argreg = 1}
main.c(172:33:30:1:0)		iTemp12 [k21 lr33:34 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int} = call _sensorSample [k14 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int function    }
main.c(172:34:31:1:0)		iTemp13 [k22 lr34:35 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int} = iTemp3 [k8 lr6:48 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{int}{ sir@ _integrateSample_q_1_1} - iTemp12 [k21 lr33:34 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int}
main.c(172:35:32:1:0)		iTemp3 [k8 lr6:48 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{int}{ sir@ _integrateSample_q_1_1} := iTemp13 [k22 lr34:35 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int}
main.c(173:36:34:1:0)		iTemp14 [k23 lr10:37 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int}{ sir@ _integrateSample_numsamples_1_1} = iTemp14 [k23 lr10:37 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int}{ sir@ _integrateSample_numsamples_1_1} - 0x1 {literal unsigned char}
main.c(173:37:36:1:0)		if iTemp14 [k23 lr10:37 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int}{ sir@ _integrateSample_numsamples_1_1} != 0 goto _dobody_0($1)

----------------------------------------------------------------
Basic Block _eBBlock0 : loop Depth = 0 noPath = 0 , lastinLoop = 0
depth 1st num 4 : bbnum = 3 1st iCode = 38 , last iCode = 39
visited 0 : hasFcall = 0

defines bitVector :bitvector Size = 59 bSize = 8
Bits on { (39) }

local defines bitVector :bitvector Size = 59 bSize = 8
Bits on { (39) }

pointers Set bitvector :
in pointers Set bitvector :
inDefs Set bitvector :bitvector Size = 59 bSize = 8
Bits on { (2) (3) (6) (12) (13) (18) (19) (24) (25) (26) (30) (31) (32) (34) (35) }

outDefs Set bitvector :bitvector Size = 59 bSize = 8
Bits on { (2) (3) (6) (12) (13) (18) (19) (24) (25) (26) (30) (31) (32) (34) (35) (39) }

usesDefs Set bitvector :bitvector Size = 59 bSize = 8
Bits on { (26) (39) }

----------------------------------------------------------------
main.c(176:38:39:0:0)		iTemp15 [k24 lr38:39 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{char} = iTemp2 [k6 lr5:48 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{int}{ sir@ _integrateSample_i_1_1} < 0x0 {literal int}
main.c(176:39:40:0:0)		if iTemp15 [k24 lr38:39 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{char} == 0 goto _iffalse_0($5)

----------------------------------------------------------------
Basic Block _eBBlock1 : loop Depth = 0 noPath = 0 , lastinLoop = 0
depth 1st num 5 : bbnum = 4 1st iCode = 40 , last iCode = 41
visited 0 : hasFcall = 0

defines bitVector :bitvector Size = 59 bSize = 8
Bits on { (43) (44) }

local defines bitVector :bitvector Size = 59 bSize = 8
Bits on { (43) (44) }

pointers Set bitvector :
in pointers Set bitvector :
inDefs Set bitvector :bitvector Size = 59 bSize = 8
Bits on { (2) (3) (6) (12) (13) (18) (19) (24) (25) (26) (30) (31) (32) (34) (35) (39) }

outDefs Set bitvector :bitvector Size = 59 bSize = 8
Bits on { (2) (3) (6) (12) (13) (18) (19) (24) (25) (30) (31) (32) (34) (35) (39) (43) (44) }

usesDefs Set bitvector :bitvector Size = 59 bSize = 8
Bits on { (26) (43) }

----------------------------------------------------------------
main.c(176:40:43:0:0)		iTemp16 [k25 lr40:41 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int} = - iTemp2 [k6 lr5:48 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{int}{ sir@ _integrateSample_i_1_1}
main.c(176:41:44:0:0)		iTemp2 [k6 lr5:48 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{int}{ sir@ _integrateSample_i_1_1} := iTemp16 [k25 lr40:41 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int}

----------------------------------------------------------------
Basic Block _iffalse_0 : loop Depth = 0 noPath = 0 , lastinLoop = 0
depth 1st num 6 : bbnum = 5 1st iCode = 42 , last iCode = 44
visited 0 : hasFcall = 0

defines bitVector :bitvector Size = 59 bSize = 8
Bits on { (46) }

local defines bitVector :bitvector Size = 59 bSize = 8
Bits on { (46) }

pointers Set bitvector :
in pointers Set bitvector :
inDefs Set bitvector :bitvector Size = 59 bSize = 8
Bits on { (2) (3) (6) (12) (13) (18) (19) (24) (25) (26) (30) (31) (32) (34) (35) (39) (43) (44) }

outDefs Set bitvector :bitvector Size = 59 bSize = 8
Bits on { (2) (3) (6) (12) (13) (18) (19) (24) (25) (26) (30) (31) (32) (34) (35) (39) (43) (44) (46) }

usesDefs Set bitvector :bitvector Size = 59 bSize = 8
Bits on { (32) (46) }

----------------------------------------------------------------
main.c(176:42:45:0:0)	 _iffalse_0($5) :
main.c(177:43:46:0:0)		iTemp17 [k26 lr43:44 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{char} = iTemp3 [k8 lr6:48 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{int}{ sir@ _integrateSample_q_1_1} < 0x0 {literal int}
main.c(177:44:47:0:0)		if iTemp17 [k26 lr43:44 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{char} == 0 goto _iffalse_1($7)

----------------------------------------------------------------
Basic Block _eBBlock2 : loop Depth = 0 noPath = 0 , lastinLoop = 0
depth 1st num 7 : bbnum = 6 1st iCode = 45 , last iCode = 46
visited 0 : hasFcall = 0

defines bitVector :bitvector Size = 59 bSize = 8
Bits on { (50) (51) }

local defines bitVector :bitvector Size = 59 bSize = 8
Bits on { (50) (51) }

pointers Set bitvector :
in pointers Set bitvector :
inDefs Set bitvector :bitvector Size = 59 bSize = 8
Bits on { (2) (3) (6) (12) (13) (18) (19) (24) (25) (26) (30) (31) (32) (34) (35) (39) (43) (44) (46) }

outDefs Set bitvector :bitvector Size = 59 bSize = 8
Bits on { (2) (3) (6) (12) (13) (18) (19) (24) (25) (26) (30) (31) (34) (35) (39) (43) (44) (46) (50) (51) }

usesDefs Set bitvector :bitvector Size = 59 bSize = 8
Bits on { (32) (50) }

----------------------------------------------------------------
main.c(177:45:50:0:0)		iTemp18 [k27 lr45:46 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int} = - iTemp3 [k8 lr6:48 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{int}{ sir@ _integrateSample_q_1_1}
main.c(177:46:51:0:0)		iTemp3 [k8 lr6:48 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{int}{ sir@ _integrateSample_q_1_1} := iTemp18 [k27 lr45:46 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int}

----------------------------------------------------------------
Basic Block _iffalse_1 : loop Depth = 0 noPath = 0 , lastinLoop = 0
depth 1st num 8 : bbnum = 7 1st iCode = 47 , last iCode = 49
visited 0 : hasFcall = 0

defines bitVector :bitvector Size = 59 bSize = 8
Bits on { (53) }

local defines bitVector :bitvector Size = 59 bSize = 8
Bits on { (53) }

pointers Set bitvector :
in pointers Set bitvector :
inDefs Set bitvector :bitvector Size = 59 bSize = 8
Bits on { (2) (3) (6) (12) (13) (18) (19) (24) (25) (26) (30) (31) (32) (34) (35) (39) (43) (44) (46) (50) (51) }

outDefs Set bitvector :bitvector Size = 59 bSize = 8
Bits on { (2) (3) (6) (12) (13) (18) (19) (24) (25) (26) (30) (31) (32) (34) (35) (39) (43) (44) (46) (50) (51) (53) }

usesDefs Set bitvector :bitvector Size = 59 bSize = 8
Bits on { (26) (32) (44) (51) (53) }

----------------------------------------------------------------
main.c(177:47:52:0:0)	 _iffalse_1($7) :
main.c(178:48:53:0:0)		iTemp19 [k28 lr48:49 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int} = iTemp2 [k6 lr5:48 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{int}{ sir@ _integrateSample_i_1_1} + iTemp3 [k8 lr6:48 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{int}{ sir@ _integrateSample_q_1_1}
main.c(178:49:54:0:0)		ret iTemp19 [k28 lr48:49 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int}

----------------------------------------------------------------
Basic Block _return : loop Depth = 0 noPath = 0 , lastinLoop = 0
depth 1st num 9 : bbnum = 8 1st iCode = 50 , last iCode = 51
visited 0 : hasFcall = 0

defines bitVector :bitvector Size = 59 bSize = 8
Bits on { }

local defines bitVector :
pointers Set bitvector :
in pointers Set bitvector :
inDefs Set bitvector :bitvector Size = 59 bSize = 8
Bits on { (2) (3) (6) (12) (13) (18) (19) (24) (25) (26) (30) (31) (32) (34) (35) (39) (43) (44) (46) (50) (51) (53) }

outDefs Set bitvector :bitvector Size = 59 bSize = 8
Bits on { (2) (3) (6) (12) (13) (18) (19) (24) (25) (26) (30) (31) (32) (34) (35) (39) (43) (44) (46) (50) (51) (53) }

usesDefs Set bitvector :bitvector Size = 59 bSize = 8
Bits on { }

----------------------------------------------------------------
main.c(178:50:55:0:0)	 _return($8) :
main.c(178:51:56:0:0)		eproc _integrateSample [k1 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int function    }
packRegisters
packRegsForAssign
  result:iTemp2 [k6 lr5:48 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{int}{ sir@ _integrateSample_i_1_1}
  left:
  right:0x0 {literal int}
  2705 - not packing - right is not temp
allocDirReg BAD, op is NULL
  result:iTemp2 [k6 lr5:48 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{int}{ sir@ _integrateSample_i_1_1}
  left:
  right:0x0 {literal int}
packRegsForAssign
  result:iTemp3 [k8 lr6:48 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{int}{ sir@ _integrateSample_q_1_1}
  left:
  right:0x0 {literal int}
  2705 - not packing - right is not temp
allocDirReg BAD, op is NULL
  result:iTemp3 [k8 lr6:48 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{int}{ sir@ _integrateSample_q_1_1}
  left:
  right:0x0 {literal int}
packRegsForAssign
  result:iTemp4 [k10 lr8:10 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{int}{ sir@ _integrateSample_numsamples_1_1}
  left:
  right:iTemp5 [k13 lr7:8 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int}{ sir@ _integrateSample_numsamples_1_1}
  2760 - dic result key == ic right key -- pointer set=N
  packing. removing iTemp5
  replacing with iTemp5
  result:iTemp4 [k10 lr7:10 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{int}{ sir@ _integrateSample_numsamples_1_1}
  left:
  right:iTemp5 [k13 lr7:8 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int}{ sir@ _integrateSample_numsamples_1_1}
packRegsForAssign
  result:iTemp2 [k6 lr5:48 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{int}{ sir@ _integrateSample_i_1_1}
  left:
  right:0x0 {literal int}
  2705 - not packing - right is not temp
allocDirReg BAD, op is NULL
  result:iTemp2 [k6 lr5:48 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{int}{ sir@ _integrateSample_i_1_1}
  left:
  right:0x0 {literal int}
packRegsForAssign
  result:iTemp3 [k8 lr6:48 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{int}{ sir@ _integrateSample_q_1_1}
  left:
  right:0x0 {literal int}
  2705 - not packing - right is not temp
allocDirReg BAD, op is NULL
  result:iTemp3 [k8 lr6:48 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{int}{ sir@ _integrateSample_q_1_1}
  left:
  right:0x0 {literal int}
  left:_integrateSample [k1 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int function    }
        Symbol type: int function    
  result:iTemp0 [k2 lr3:37 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _integrateSample_transmitter_1_1}
          Symbol type: unsigned char
  3594 - pointer reg req = 0
packForReceive
  result:iTemp0 [k2 lr3:37 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _integrateSample_transmitter_1_1}
  left:
  right:
  hey we can remove this unnecessary assign
  result:iTemp1 [k4 lr4:37 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _integrateSample_receiver_1_1}
          Symbol type: unsigned char
  3594 - pointer reg req = 0
packForReceive
  result:iTemp1 [k4 lr4:37 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _integrateSample_receiver_1_1}
  left:
  right:
  hey we can remove this unnecessary assign
  result:iTemp2 [k6 lr5:48 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{int}{ sir@ _integrateSample_i_1_1}
          Symbol type: int
  3594 - pointer reg req = 0
  result:iTemp3 [k8 lr6:48 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{int}{ sir@ _integrateSample_q_1_1}
          Symbol type: int
  3594 - pointer reg req = 0
  right:_numIntegrationSamples [k12 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{unsigned char}
         Symbol type: unsigned char
  result:iTemp4 [k10 lr7:10 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{int}{ sir@ _integrateSample_numsamples_1_1}
          Symbol type: int
  3594 - pointer reg req = 0
  3662 - casting
packRegsForOneuse
packRegisters
packRegsForAssign
  result:iTemp14 [k23 lr10:37 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int}{ sir@ _integrateSample_numsamples_1_1}
  left:
  right:iTemp4 [k10 lr7:10 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{int}{ sir@ _integrateSample_numsamples_1_1}
  result:iTemp14 [k23 lr10:37 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int}{ sir@ _integrateSample_numsamples_1_1}
  left:
  right:iTemp4 [k10 lr7:10 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{int}{ sir@ _integrateSample_numsamples_1_1}
  right:iTemp4 [k10 lr7:10 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{int}{ sir@ _integrateSample_numsamples_1_1}
         Symbol type: int
  result:iTemp14 [k23 lr10:37 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int}{ sir@ _integrateSample_numsamples_1_1}
          Symbol type: int
  3594 - pointer reg req = 0
packRegisters
packRegsForAssign
  result:iTemp2 [k6 lr5:48 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{int}{ sir@ _integrateSample_i_1_1}
  left:
  right:iTemp7 [k16 lr16:17 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int}
  2760 - dic result key == ic right key -- pointer set=N
  packing. removing iTemp7
  replacing with iTemp7
  result:iTemp2 [k6 lr5:48 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{int}{ sir@ _integrateSample_i_1_1}
  left:
  right:iTemp7 [k16 lr16:17 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int}
packRegsForAssign
  result:iTemp3 [k8 lr6:48 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{int}{ sir@ _integrateSample_q_1_1}
  left:
  right:iTemp9 [k18 lr22:23 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int}
  2760 - dic result key == ic right key -- pointer set=N
  packing. removing iTemp9
  replacing with iTemp9
  result:iTemp3 [k8 lr6:48 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{int}{ sir@ _integrateSample_q_1_1}
  left:
  right:iTemp9 [k18 lr22:23 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int}
packRegsForAssign
  result:iTemp2 [k6 lr5:48 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{int}{ sir@ _integrateSample_i_1_1}
  left:
  right:iTemp11 [k20 lr28:29 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int}
  2760 - dic result key == ic right key -- pointer set=N
  packing. removing iTemp11
  replacing with iTemp11
  result:iTemp2 [k6 lr5:48 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{int}{ sir@ _integrateSample_i_1_1}
  left:
  right:iTemp11 [k20 lr28:29 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int}
packRegsForAssign
  result:iTemp3 [k8 lr6:48 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{int}{ sir@ _integrateSample_q_1_1}
  left:
  right:iTemp13 [k22 lr34:35 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int}
  2760 - dic result key == ic right key -- pointer set=N
  packing. removing iTemp13
  replacing with iTemp13
  result:iTemp3 [k8 lr6:48 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{int}{ sir@ _integrateSample_q_1_1}
  left:
  right:iTemp13 [k22 lr34:35 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int}
  left:iTemp0 [k2 lr3:37 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _integrateSample_transmitter_1_1}
        Symbol type: unsigned char
  3594 - pointer reg req = 0
  left:iTemp1 [k4 lr4:37 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _integrateSample_receiver_1_1}
        Symbol type: unsigned char
  3594 - pointer reg req = 0
  3594 - pointer reg req = 0
  left:_sensorSample [k14 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int function    }
        Symbol type: int function    
  result:iTemp6 [k15 lr15:16 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int}
          Symbol type: int
  3594 - pointer reg req = 0
  left:iTemp2 [k6 lr5:48 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{int}{ sir@ _integrateSample_i_1_1}
        Symbol type: int
  right:iTemp6 [k15 lr15:16 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int}
         Symbol type: int
  result:iTemp2 [k6 lr5:48 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{int}{ sir@ _integrateSample_i_1_1}
          Symbol type: int
  3594 - pointer reg req = 0
packRegsForSupport
findAssignToSym
packRegsForAccUse
  packRegsForAccUse:3181
  left:iTemp0 [k2 lr3:37 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _integrateSample_transmitter_1_1}
        Symbol type: unsigned char
  3594 - pointer reg req = 0
  left:iTemp1 [k4 lr4:37 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _integrateSample_receiver_1_1}
        Symbol type: unsigned char
  3594 - pointer reg req = 0
  3594 - pointer reg req = 0
  left:_sensorSample [k14 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int function    }
        Symbol type: int function    
  result:iTemp8 [k17 lr21:22 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int}
          Symbol type: int
  3594 - pointer reg req = 0
  left:iTemp3 [k8 lr6:48 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{int}{ sir@ _integrateSample_q_1_1}
        Symbol type: int
  right:iTemp8 [k17 lr21:22 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int}
         Symbol type: int
  result:iTemp3 [k8 lr6:48 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{int}{ sir@ _integrateSample_q_1_1}
          Symbol type: int
  3594 - pointer reg req = 0
packRegsForSupport
findAssignToSym
packRegsForAccUse
  packRegsForAccUse:3181
  left:iTemp0 [k2 lr3:37 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _integrateSample_transmitter_1_1}
        Symbol type: unsigned char
  3594 - pointer reg req = 0
  left:iTemp1 [k4 lr4:37 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _integrateSample_receiver_1_1}
        Symbol type: unsigned char
  3594 - pointer reg req = 0
  3594 - pointer reg req = 0
  left:_sensorSample [k14 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int function    }
        Symbol type: int function    
  result:iTemp10 [k19 lr27:28 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int}
          Symbol type: int
  3594 - pointer reg req = 0
  left:iTemp2 [k6 lr5:48 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{int}{ sir@ _integrateSample_i_1_1}
        Symbol type: int
  right:iTemp10 [k19 lr27:28 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int}
         Symbol type: int
  result:iTemp2 [k6 lr5:48 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{int}{ sir@ _integrateSample_i_1_1}
          Symbol type: int
  3594 - pointer reg req = 0
packRegsForSupport
findAssignToSym
packRegsForAccUse
  packRegsForAccUse:3181
  left:iTemp0 [k2 lr3:37 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _integrateSample_transmitter_1_1}
        Symbol type: unsigned char
  3594 - pointer reg req = 0
  left:iTemp1 [k4 lr4:37 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _integrateSample_receiver_1_1}
        Symbol type: unsigned char
  3594 - pointer reg req = 0
  3594 - pointer reg req = 0
  left:_sensorSample [k14 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int function    }
        Symbol type: int function    
  result:iTemp12 [k21 lr33:34 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int}
          Symbol type: int
  3594 - pointer reg req = 0
  left:iTemp3 [k8 lr6:48 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{int}{ sir@ _integrateSample_q_1_1}
        Symbol type: int
  right:iTemp12 [k21 lr33:34 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int}
         Symbol type: int
  result:iTemp3 [k8 lr6:48 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{int}{ sir@ _integrateSample_q_1_1}
          Symbol type: int
  3594 - pointer reg req = 0
packRegsForSupport
findAssignToSym
packRegsForAccUse
  packRegsForAccUse:3181
  left:iTemp14 [k23 lr10:37 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int}{ sir@ _integrateSample_numsamples_1_1}
        Symbol type: int
  result:iTemp14 [k23 lr10:37 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int}{ sir@ _integrateSample_numsamples_1_1}
          Symbol type: int
  3594 - pointer reg req = 0
packRegsForSupport
packRegsForAccUse
  packRegsForAccUse:3181
  left:iTemp14 [k23 lr10:37 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int}{ sir@ _integrateSample_numsamples_1_1}
        Symbol type: int
  3594 - pointer reg req = 0
packRegisters
  left:iTemp2 [k6 lr5:48 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{int}{ sir@ _integrateSample_i_1_1}
        Symbol type: int
  result:iTemp15 [k24 lr38:39 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{char}
          Symbol type: char
  3594 - pointer reg req = 0
  3611
  left:iTemp15 [k24 lr38:39 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{char}
        Symbol type: char
  3594 - pointer reg req = 0
packRegisters
packRegsForAssign
  result:iTemp2 [k6 lr5:48 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{int}{ sir@ _integrateSample_i_1_1}
  left:
  right:iTemp16 [k25 lr40:41 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int}
  2760 - dic result key == ic right key -- pointer set=N
  packing. removing iTemp16
  replacing with iTemp16
  result:iTemp2 [k6 lr5:48 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{int}{ sir@ _integrateSample_i_1_1}
  left:
  right:iTemp16 [k25 lr40:41 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int}
  left:iTemp2 [k6 lr5:48 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{int}{ sir@ _integrateSample_i_1_1}
        Symbol type: int
  result:iTemp2 [k6 lr5:48 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{int}{ sir@ _integrateSample_i_1_1}
          Symbol type: int
  3594 - pointer reg req = 0
packRegisters
  left:iTemp3 [k8 lr6:48 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{int}{ sir@ _integrateSample_q_1_1}
        Symbol type: int
  result:iTemp17 [k26 lr43:44 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{char}
          Symbol type: char
  3594 - pointer reg req = 0
  3611
  left:iTemp17 [k26 lr43:44 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{char}
        Symbol type: char
  3594 - pointer reg req = 0
packRegisters
packRegsForAssign
  result:iTemp3 [k8 lr6:48 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{int}{ sir@ _integrateSample_q_1_1}
  left:
  right:iTemp18 [k27 lr45:46 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int}
  2760 - dic result key == ic right key -- pointer set=N
  packing. removing iTemp18
  replacing with iTemp18
  result:iTemp3 [k8 lr6:48 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{int}{ sir@ _integrateSample_q_1_1}
  left:
  right:iTemp18 [k27 lr45:46 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int}
  left:iTemp3 [k8 lr6:48 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{int}{ sir@ _integrateSample_q_1_1}
        Symbol type: int
  result:iTemp3 [k8 lr6:48 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{int}{ sir@ _integrateSample_q_1_1}
          Symbol type: int
  3594 - pointer reg req = 0
packRegisters
  left:iTemp2 [k6 lr5:48 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{int}{ sir@ _integrateSample_i_1_1}
        Symbol type: int
  right:iTemp3 [k8 lr6:48 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{int}{ sir@ _integrateSample_q_1_1}
         Symbol type: int
  result:iTemp19 [k28 lr48:49 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int}
          Symbol type: int
  3594 - pointer reg req = 0
packRegsForSupport
findAssignToSym
findAssignToSym
packRegsForAccUse
  packRegsForAccUse:3181
  left:iTemp19 [k28 lr48:49 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int}
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
  2421 - iTemp16
  2431 - itemp register
  reg name iTemp16,  reg type REG_GPR
  2421 - iTemp17
  2431 - itemp register
  2421 - iTemp18
  2431 - itemp register
  reg name iTemp18,  reg type REG_GPR
  2421 - iTemp19
  2431 - itemp register
  reg name iTemp19,  reg type REG_GPR
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
bitvector Size = 59 bSize = 8
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
bitvector Size = 59 bSize = 8
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
bitvector Size = 59 bSize = 8
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
bitvector Size = 59 bSize = 8
Bits on { (2) (4) (6) (8) }
getRegGpr
allocReg of type REG_GPR
getRegGpr
allocReg of type REG_GPR
  2141 - 
  2155 - 
  op: CAST
deassignLRs
willCauseSpill
computeSpillable
  2125 - 
bitvector Size = 59 bSize = 8
Bits on { (2) (4) (6) (8) (10) }
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
freeReg
freeReg
willCauseSpill
computeSpillable
  2125 - 
bitvector Size = 59 bSize = 8
Bits on { (2) (4) (6) (8) (23) }
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
bitvector Size = 59 bSize = 8
Bits on { (2) (4) (6) (8) (15) (23) }
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
bitvector Size = 59 bSize = 8
Bits on { (2) (4) (6) (8) (17) (23) }
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
bitvector Size = 59 bSize = 8
Bits on { (2) (4) (6) (8) (19) (23) }
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
bitvector Size = 59 bSize = 8
Bits on { (2) (4) (6) (8) (21) (23) }
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
depth 1st num 1 : bbnum = 0 1st iCode = 1 , last iCode = 8
visited 0 : hasFcall = 0

defines bitVector :bitvector Size = 59 bSize = 8
Bits on { (2) (3) (4) (5) (6) (7) }

local defines bitVector :bitvector Size = 59 bSize = 8
Bits on { (2) (3) (4) (5) (6) (7) }

pointers Set bitvector :
in pointers Set bitvector :
inDefs Set bitvector :
outDefs Set bitvector :bitvector Size = 59 bSize = 8
Bits on { (2) (3) (4) (5) (6) (7) }

usesDefs Set bitvector :bitvector Size = 59 bSize = 8
Bits on { (2) (3) (6) }

----------------------------------------------------------------
main.c(163:1:0:0:0)	 _entry($9) :
main.c(163:2:1:0:0)		proc _integrateSample [k1 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int function    }
main.c(163:3:2:0:0)	iTemp0 [k2 lr3:37 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _integrateSample_transmitter_1_1}[r0x53 ] = recv 
main.c(163:4:3:0:0)	iTemp1 [k4 lr4:37 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _integrateSample_receiver_1_1}[r0x54 ] = recv 
main.c(164:5:4:0:0)		iTemp2 [k6 lr5:48 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{int}{ sir@ _integrateSample_i_1_1}[r0x55 r0x56 ] := 0x0 {literal int}
main.c(164:6:5:0:0)		iTemp3 [k8 lr6:48 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{int}{ sir@ _integrateSample_q_1_1}[r0x57 r0x58 ] := 0x0 {literal int}
main.c(165:7:6:0:0)		iTemp4 [k10 lr7:10 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{int}{ sir@ _integrateSample_numsamples_1_1}[r0x59 r0x5A ] = (int)_numIntegrationSamples [k12 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{unsigned char}
main.c(167:10:58:0:0)		iTemp14 [k23 lr10:37 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int}{ sir@ _integrateSample_numsamples_1_1}[r0x5B r0x5C ] := iTemp4 [k10 lr7:10 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{int}{ sir@ _integrateSample_numsamples_1_1}[r0x59 r0x5A ]
main.c(167:11:8:1:0)	 _dobody_0($1) :
main.c(169:12:9:1:0)		send iTemp0 [k2 lr3:37 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _integrateSample_transmitter_1_1}[r0x53 ]{argreg = 1}
main.c(169:13:10:1:0)		send iTemp1 [k4 lr4:37 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _integrateSample_receiver_1_1}[r0x54 ]{argreg = 1}
main.c(169:14:11:1:0)		send 0x0 {literal unsigned char}{argreg = 1}
main.c(169:15:12:1:0)		iTemp6 [k15 lr15:16 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int}[r0x5D r0x5E ] = call _sensorSample [k14 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int function    }
main.c(169:16:13:1:0)		iTemp2 [k6 lr5:48 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{int}{ sir@ _integrateSample_i_1_1}[r0x55 r0x56 ] = iTemp2 [k6 lr5:48 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{int}{ sir@ _integrateSample_i_1_1}[r0x55 r0x56 ] + iTemp6 [k15 lr15:16 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int}[r0x5D r0x5E ]
main.c(170:18:15:1:0)		send iTemp0 [k2 lr3:37 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _integrateSample_transmitter_1_1}[r0x53 ]{argreg = 1}
main.c(170:19:16:1:0)		send iTemp1 [k4 lr4:37 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _integrateSample_receiver_1_1}[r0x54 ]{argreg = 1}
main.c(170:20:17:1:0)		send 0x1 {literal unsigned char}{argreg = 1}
main.c(170:21:18:1:0)		iTemp8 [k17 lr21:22 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int}[r0x5F r0x60 ] = call _sensorSample [k14 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int function    }
main.c(170:22:19:1:0)		iTemp3 [k8 lr6:48 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{int}{ sir@ _integrateSample_q_1_1}[r0x57 r0x58 ] = iTemp3 [k8 lr6:48 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{int}{ sir@ _integrateSample_q_1_1}[r0x57 r0x58 ] + iTemp8 [k17 lr21:22 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int}[r0x5F r0x60 ]
main.c(171:24:21:1:0)		send iTemp0 [k2 lr3:37 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _integrateSample_transmitter_1_1}[r0x53 ]{argreg = 1}
main.c(171:25:22:1:0)		send iTemp1 [k4 lr4:37 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _integrateSample_receiver_1_1}[r0x54 ]{argreg = 1}
main.c(171:26:23:1:0)		send 0x2 {literal unsigned char}{argreg = 1}
main.c(171:27:24:1:0)		iTemp10 [k19 lr27:28 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int}[r0x61 r0x62 ] = call _sensorSample [k14 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int function    }
main.c(171:28:25:1:0)		iTemp2 [k6 lr5:48 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{int}{ sir@ _integrateSample_i_1_1}[r0x55 r0x56 ] = iTemp2 [k6 lr5:48 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{int}{ sir@ _integrateSample_i_1_1}[r0x55 r0x56 ] - iTemp10 [k19 lr27:28 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int}[r0x61 r0x62 ]
main.c(172:30:27:1:0)		send iTemp0 [k2 lr3:37 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _integrateSample_transmitter_1_1}[r0x53 ]{argreg = 1}
main.c(172:31:28:1:0)		send iTemp1 [k4 lr4:37 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _integrateSample_receiver_1_1}[r0x54 ]{argreg = 1}
main.c(172:32:29:1:0)		send 0x3 {literal unsigned char}{argreg = 1}
main.c(172:33:30:1:0)		iTemp12 [k21 lr33:34 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int}[r0x63 r0x64 ] = call _sensorSample [k14 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int function    }
main.c(172:34:31:1:0)		iTemp3 [k8 lr6:48 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{int}{ sir@ _integrateSample_q_1_1}[r0x57 r0x58 ] = iTemp3 [k8 lr6:48 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{int}{ sir@ _integrateSample_q_1_1}[r0x57 r0x58 ] - iTemp12 [k21 lr33:34 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int}[r0x63 r0x64 ]
main.c(173:36:34:1:0)		iTemp14 [k23 lr10:37 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int}{ sir@ _integrateSample_numsamples_1_1}[r0x5B r0x5C ] = iTemp14 [k23 lr10:37 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int}{ sir@ _integrateSample_numsamples_1_1}[r0x5B r0x5C ] - 0x1 {literal unsigned char}
main.c(173:37:36:1:0)		if iTemp14 [k23 lr10:37 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int}{ sir@ _integrateSample_numsamples_1_1}[r0x5B r0x5C ] != 0 goto _dobody_0($1)
main.c(176:38:39:0:0)		iTemp15 [k24 lr38:39 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{char} = iTemp2 [k6 lr5:48 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{int}{ sir@ _integrateSample_i_1_1}[r0x55 r0x56 ] < 0x0 {literal int}
main.c(176:39:40:0:0)		if iTemp15 [k24 lr38:39 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{char} == 0 goto _iffalse_0($5)
main.c(176:40:43:0:0)		iTemp2 [k6 lr5:48 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{int}{ sir@ _integrateSample_i_1_1}[r0x55 r0x56 ] = - iTemp2 [k6 lr5:48 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{int}{ sir@ _integrateSample_i_1_1}[r0x55 r0x56 ]
main.c(176:42:45:0:0)	 _iffalse_0($5) :
main.c(177:43:46:0:0)		iTemp17 [k26 lr43:44 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{char} = iTemp3 [k8 lr6:48 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{int}{ sir@ _integrateSample_q_1_1}[r0x57 r0x58 ] < 0x0 {literal int}
main.c(177:44:47:0:0)		if iTemp17 [k26 lr43:44 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{char} == 0 goto _iffalse_1($7)
main.c(177:45:50:0:0)		iTemp3 [k8 lr6:48 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{int}{ sir@ _integrateSample_q_1_1}[r0x57 r0x58 ] = - iTemp3 [k8 lr6:48 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{int}{ sir@ _integrateSample_q_1_1}[r0x57 r0x58 ]
main.c(177:47:52:0:0)	 _iffalse_1($7) :
main.c(178:48:53:0:0)		iTemp19 [k28 lr48:49 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int}[r0x55 r0x56 ] = iTemp2 [k6 lr5:48 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{int}{ sir@ _integrateSample_i_1_1}[r0x55 r0x56 ] + iTemp3 [k8 lr6:48 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{int}{ sir@ _integrateSample_q_1_1}[r0x57 r0x58 ]
main.c(178:49:54:0:0)		ret iTemp19 [k28 lr48:49 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int}[r0x55 r0x56 ]
main.c(178:50:55:0:0)	 _return($8) :
main.c(178:51:56:0:0)		eproc _integrateSample [k1 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int function    }

----------------------------------------------------------------
Basic Block preHeaderLbl0 : loop Depth = 0 noPath = 0 , lastinLoop = 0
depth 1st num 2 : bbnum = 1 1st iCode = 9 , last iCode = 10
visited 0 : hasFcall = 0

defines bitVector :bitvector Size = 59 bSize = 8
Bits on { (58) }

local defines bitVector :bitvector Size = 59 bSize = 8
Bits on { (58) }

pointers Set bitvector :
in pointers Set bitvector :
inDefs Set bitvector :bitvector Size = 59 bSize = 8
Bits on { (2) (3) (4) (5) (6) (7) }

outDefs Set bitvector :bitvector Size = 59 bSize = 8
Bits on { (2) (3) (4) (5) (6) (7) (58) }

usesDefs Set bitvector :bitvector Size = 59 bSize = 8
Bits on { (7) }

----------------------------------------------------------------
main.c(167:9:57:0:0)	 preHeaderLbl0($10) :
main.c(167:10:58:0:0)		iTemp14 [k23 lr10:37 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int}{ sir@ _integrateSample_numsamples_1_1}[r0x5B r0x5C ] := iTemp4 [k10 lr7:10 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{int}{ sir@ _integrateSample_numsamples_1_1}[r0x59 r0x5A ]
main.c(167:11:8:1:0)	 _dobody_0($1) :
main.c(169:12:9:1:0)		send iTemp0 [k2 lr3:37 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _integrateSample_transmitter_1_1}[r0x53 ]{argreg = 1}
main.c(169:13:10:1:0)		send iTemp1 [k4 lr4:37 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _integrateSample_receiver_1_1}[r0x54 ]{argreg = 1}
main.c(169:14:11:1:0)		send 0x0 {literal unsigned char}{argreg = 1}
main.c(169:15:12:1:0)		iTemp6 [k15 lr15:16 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int}[r0x5D r0x5E ] = call _sensorSample [k14 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int function    }
main.c(169:16:13:1:0)		iTemp2 [k6 lr5:48 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{int}{ sir@ _integrateSample_i_1_1}[r0x55 r0x56 ] = iTemp2 [k6 lr5:48 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{int}{ sir@ _integrateSample_i_1_1}[r0x55 r0x56 ] + iTemp6 [k15 lr15:16 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int}[r0x5D r0x5E ]
main.c(170:18:15:1:0)		send iTemp0 [k2 lr3:37 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _integrateSample_transmitter_1_1}[r0x53 ]{argreg = 1}
main.c(170:19:16:1:0)		send iTemp1 [k4 lr4:37 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _integrateSample_receiver_1_1}[r0x54 ]{argreg = 1}
main.c(170:20:17:1:0)		send 0x1 {literal unsigned char}{argreg = 1}
main.c(170:21:18:1:0)		iTemp8 [k17 lr21:22 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int}[r0x5F r0x60 ] = call _sensorSample [k14 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int function    }
main.c(170:22:19:1:0)		iTemp3 [k8 lr6:48 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{int}{ sir@ _integrateSample_q_1_1}[r0x57 r0x58 ] = iTemp3 [k8 lr6:48 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{int}{ sir@ _integrateSample_q_1_1}[r0x57 r0x58 ] + iTemp8 [k17 lr21:22 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int}[r0x5F r0x60 ]
main.c(171:24:21:1:0)		send iTemp0 [k2 lr3:37 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _integrateSample_transmitter_1_1}[r0x53 ]{argreg = 1}
main.c(171:25:22:1:0)		send iTemp1 [k4 lr4:37 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _integrateSample_receiver_1_1}[r0x54 ]{argreg = 1}
main.c(171:26:23:1:0)		send 0x2 {literal unsigned char}{argreg = 1}
main.c(171:27:24:1:0)		iTemp10 [k19 lr27:28 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int}[r0x61 r0x62 ] = call _sensorSample [k14 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int function    }
main.c(171:28:25:1:0)		iTemp2 [k6 lr5:48 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{int}{ sir@ _integrateSample_i_1_1}[r0x55 r0x56 ] = iTemp2 [k6 lr5:48 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{int}{ sir@ _integrateSample_i_1_1}[r0x55 r0x56 ] - iTemp10 [k19 lr27:28 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int}[r0x61 r0x62 ]
main.c(172:30:27:1:0)		send iTemp0 [k2 lr3:37 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _integrateSample_transmitter_1_1}[r0x53 ]{argreg = 1}
main.c(172:31:28:1:0)		send iTemp1 [k4 lr4:37 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _integrateSample_receiver_1_1}[r0x54 ]{argreg = 1}
main.c(172:32:29:1:0)		send 0x3 {literal unsigned char}{argreg = 1}
main.c(172:33:30:1:0)		iTemp12 [k21 lr33:34 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int}[r0x63 r0x64 ] = call _sensorSample [k14 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int function    }
main.c(172:34:31:1:0)		iTemp3 [k8 lr6:48 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{int}{ sir@ _integrateSample_q_1_1}[r0x57 r0x58 ] = iTemp3 [k8 lr6:48 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{int}{ sir@ _integrateSample_q_1_1}[r0x57 r0x58 ] - iTemp12 [k21 lr33:34 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int}[r0x63 r0x64 ]
main.c(173:36:34:1:0)		iTemp14 [k23 lr10:37 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int}{ sir@ _integrateSample_numsamples_1_1}[r0x5B r0x5C ] = iTemp14 [k23 lr10:37 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int}{ sir@ _integrateSample_numsamples_1_1}[r0x5B r0x5C ] - 0x1 {literal unsigned char}
main.c(173:37:36:1:0)		if iTemp14 [k23 lr10:37 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int}{ sir@ _integrateSample_numsamples_1_1}[r0x5B r0x5C ] != 0 goto _dobody_0($1)
main.c(176:38:39:0:0)		iTemp15 [k24 lr38:39 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{char} = iTemp2 [k6 lr5:48 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{int}{ sir@ _integrateSample_i_1_1}[r0x55 r0x56 ] < 0x0 {literal int}
main.c(176:39:40:0:0)		if iTemp15 [k24 lr38:39 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{char} == 0 goto _iffalse_0($5)
main.c(176:40:43:0:0)		iTemp2 [k6 lr5:48 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{int}{ sir@ _integrateSample_i_1_1}[r0x55 r0x56 ] = - iTemp2 [k6 lr5:48 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{int}{ sir@ _integrateSample_i_1_1}[r0x55 r0x56 ]
main.c(176:42:45:0:0)	 _iffalse_0($5) :
main.c(177:43:46:0:0)		iTemp17 [k26 lr43:44 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{char} = iTemp3 [k8 lr6:48 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{int}{ sir@ _integrateSample_q_1_1}[r0x57 r0x58 ] < 0x0 {literal int}
main.c(177:44:47:0:0)		if iTemp17 [k26 lr43:44 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{char} == 0 goto _iffalse_1($7)
main.c(177:45:50:0:0)		iTemp3 [k8 lr6:48 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{int}{ sir@ _integrateSample_q_1_1}[r0x57 r0x58 ] = - iTemp3 [k8 lr6:48 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{int}{ sir@ _integrateSample_q_1_1}[r0x57 r0x58 ]
main.c(177:47:52:0:0)	 _iffalse_1($7) :
main.c(178:48:53:0:0)		iTemp19 [k28 lr48:49 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int}[r0x55 r0x56 ] = iTemp2 [k6 lr5:48 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{int}{ sir@ _integrateSample_i_1_1}[r0x55 r0x56 ] + iTemp3 [k8 lr6:48 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{int}{ sir@ _integrateSample_q_1_1}[r0x57 r0x58 ]
main.c(178:49:54:0:0)		ret iTemp19 [k28 lr48:49 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int}[r0x55 r0x56 ]
main.c(178:50:55:0:0)	 _return($8) :
main.c(178:51:56:0:0)		eproc _integrateSample [k1 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int function    }

----------------------------------------------------------------
Basic Block _dobody_0 : loop Depth = 1 noPath = 0 , lastinLoop = 1
depth 1st num 3 : bbnum = 2 1st iCode = 11 , last iCode = 37
visited 0 : hasFcall = 1

defines bitVector :bitvector Size = 59 bSize = 8
Bits on { (12) (13) (18) (19) (24) (25) (26) (30) (31) (32) (34) }

local defines bitVector :bitvector Size = 59 bSize = 8
Bits on { (12) (13) (14) (18) (19) (20) (24) (25) (26) (30) (31) (32) (34) (35) }

pointers Set bitvector :
in pointers Set bitvector :
inDefs Set bitvector :bitvector Size = 59 bSize = 8
Bits on { (2) (3) (4) (5) (6) (7) (12) (13) (18) (19) (24) (25) (26) (30) (31) (32) (34) (35) (58) }

outDefs Set bitvector :bitvector Size = 59 bSize = 8
Bits on { (2) (3) (6) (12) (13) (18) (19) (24) (25) (26) (30) (31) (32) (34) }

usesDefs Set bitvector :bitvector Size = 59 bSize = 8
Bits on { (2) (3) (4) (5) (12) (13) (14) (18) (19) (20) (24) (25) (26) (30) (31) (32) (34) (58) }

----------------------------------------------------------------
main.c(167:11:8:1:0)	 _dobody_0($1) :
main.c(169:12:9:1:0)		send iTemp0 [k2 lr3:37 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _integrateSample_transmitter_1_1}[r0x53 ]{argreg = 1}
main.c(169:13:10:1:0)		send iTemp1 [k4 lr4:37 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _integrateSample_receiver_1_1}[r0x54 ]{argreg = 1}
main.c(169:14:11:1:0)		send 0x0 {literal unsigned char}{argreg = 1}
main.c(169:15:12:1:0)		iTemp6 [k15 lr15:16 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int}[r0x5D r0x5E ] = call _sensorSample [k14 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int function    }
main.c(169:16:13:1:0)		iTemp2 [k6 lr5:48 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{int}{ sir@ _integrateSample_i_1_1}[r0x55 r0x56 ] = iTemp2 [k6 lr5:48 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{int}{ sir@ _integrateSample_i_1_1}[r0x55 r0x56 ] + iTemp6 [k15 lr15:16 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int}[r0x5D r0x5E ]
main.c(170:18:15:1:0)		send iTemp0 [k2 lr3:37 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _integrateSample_transmitter_1_1}[r0x53 ]{argreg = 1}
main.c(170:19:16:1:0)		send iTemp1 [k4 lr4:37 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _integrateSample_receiver_1_1}[r0x54 ]{argreg = 1}
main.c(170:20:17:1:0)		send 0x1 {literal unsigned char}{argreg = 1}
main.c(170:21:18:1:0)		iTemp8 [k17 lr21:22 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int}[r0x5F r0x60 ] = call _sensorSample [k14 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int function    }
main.c(170:22:19:1:0)		iTemp3 [k8 lr6:48 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{int}{ sir@ _integrateSample_q_1_1}[r0x57 r0x58 ] = iTemp3 [k8 lr6:48 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{int}{ sir@ _integrateSample_q_1_1}[r0x57 r0x58 ] + iTemp8 [k17 lr21:22 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int}[r0x5F r0x60 ]
main.c(171:24:21:1:0)		send iTemp0 [k2 lr3:37 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _integrateSample_transmitter_1_1}[r0x53 ]{argreg = 1}
main.c(171:25:22:1:0)		send iTemp1 [k4 lr4:37 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _integrateSample_receiver_1_1}[r0x54 ]{argreg = 1}
main.c(171:26:23:1:0)		send 0x2 {literal unsigned char}{argreg = 1}
main.c(171:27:24:1:0)		iTemp10 [k19 lr27:28 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int}[r0x61 r0x62 ] = call _sensorSample [k14 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int function    }
main.c(171:28:25:1:0)		iTemp2 [k6 lr5:48 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{int}{ sir@ _integrateSample_i_1_1}[r0x55 r0x56 ] = iTemp2 [k6 lr5:48 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{int}{ sir@ _integrateSample_i_1_1}[r0x55 r0x56 ] - iTemp10 [k19 lr27:28 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int}[r0x61 r0x62 ]
main.c(172:30:27:1:0)		send iTemp0 [k2 lr3:37 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _integrateSample_transmitter_1_1}[r0x53 ]{argreg = 1}
main.c(172:31:28:1:0)		send iTemp1 [k4 lr4:37 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _integrateSample_receiver_1_1}[r0x54 ]{argreg = 1}
main.c(172:32:29:1:0)		send 0x3 {literal unsigned char}{argreg = 1}
main.c(172:33:30:1:0)		iTemp12 [k21 lr33:34 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int}[r0x63 r0x64 ] = call _sensorSample [k14 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int function    }
main.c(172:34:31:1:0)		iTemp3 [k8 lr6:48 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{int}{ sir@ _integrateSample_q_1_1}[r0x57 r0x58 ] = iTemp3 [k8 lr6:48 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{int}{ sir@ _integrateSample_q_1_1}[r0x57 r0x58 ] - iTemp12 [k21 lr33:34 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int}[r0x63 r0x64 ]
main.c(173:36:34:1:0)		iTemp14 [k23 lr10:37 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int}{ sir@ _integrateSample_numsamples_1_1}[r0x5B r0x5C ] = iTemp14 [k23 lr10:37 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int}{ sir@ _integrateSample_numsamples_1_1}[r0x5B r0x5C ] - 0x1 {literal unsigned char}
main.c(173:37:36:1:0)		if iTemp14 [k23 lr10:37 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int}{ sir@ _integrateSample_numsamples_1_1}[r0x5B r0x5C ] != 0 goto _dobody_0($1)
main.c(176:38:39:0:0)		iTemp15 [k24 lr38:39 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{char} = iTemp2 [k6 lr5:48 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{int}{ sir@ _integrateSample_i_1_1}[r0x55 r0x56 ] < 0x0 {literal int}
main.c(176:39:40:0:0)		if iTemp15 [k24 lr38:39 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{char} == 0 goto _iffalse_0($5)
main.c(176:40:43:0:0)		iTemp2 [k6 lr5:48 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{int}{ sir@ _integrateSample_i_1_1}[r0x55 r0x56 ] = - iTemp2 [k6 lr5:48 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{int}{ sir@ _integrateSample_i_1_1}[r0x55 r0x56 ]
main.c(176:42:45:0:0)	 _iffalse_0($5) :
main.c(177:43:46:0:0)		iTemp17 [k26 lr43:44 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{char} = iTemp3 [k8 lr6:48 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{int}{ sir@ _integrateSample_q_1_1}[r0x57 r0x58 ] < 0x0 {literal int}
main.c(177:44:47:0:0)		if iTemp17 [k26 lr43:44 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{char} == 0 goto _iffalse_1($7)
main.c(177:45:50:0:0)		iTemp3 [k8 lr6:48 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{int}{ sir@ _integrateSample_q_1_1}[r0x57 r0x58 ] = - iTemp3 [k8 lr6:48 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{int}{ sir@ _integrateSample_q_1_1}[r0x57 r0x58 ]
main.c(177:47:52:0:0)	 _iffalse_1($7) :
main.c(178:48:53:0:0)		iTemp19 [k28 lr48:49 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int}[r0x55 r0x56 ] = iTemp2 [k6 lr5:48 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{int}{ sir@ _integrateSample_i_1_1}[r0x55 r0x56 ] + iTemp3 [k8 lr6:48 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{int}{ sir@ _integrateSample_q_1_1}[r0x57 r0x58 ]
main.c(178:49:54:0:0)		ret iTemp19 [k28 lr48:49 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int}[r0x55 r0x56 ]
main.c(178:50:55:0:0)	 _return($8) :
main.c(178:51:56:0:0)		eproc _integrateSample [k1 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int function    }

----------------------------------------------------------------
Basic Block _eBBlock0 : loop Depth = 0 noPath = 0 , lastinLoop = 0
depth 1st num 4 : bbnum = 3 1st iCode = 38 , last iCode = 39
visited 0 : hasFcall = 0

defines bitVector :bitvector Size = 59 bSize = 8
Bits on { (39) }

local defines bitVector :bitvector Size = 59 bSize = 8
Bits on { (39) }

pointers Set bitvector :
in pointers Set bitvector :
inDefs Set bitvector :bitvector Size = 59 bSize = 8
Bits on { (2) (3) (6) (12) (13) (18) (19) (24) (25) (26) (30) (31) (32) (34) (35) }

outDefs Set bitvector :bitvector Size = 59 bSize = 8
Bits on { (2) (3) (6) (12) (13) (18) (19) (24) (25) (26) (30) (31) (32) (34) (35) (39) }

usesDefs Set bitvector :bitvector Size = 59 bSize = 8
Bits on { (26) (39) }

----------------------------------------------------------------
main.c(176:38:39:0:0)		iTemp15 [k24 lr38:39 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{char} = iTemp2 [k6 lr5:48 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{int}{ sir@ _integrateSample_i_1_1}[r0x55 r0x56 ] < 0x0 {literal int}
main.c(176:39:40:0:0)		if iTemp15 [k24 lr38:39 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{char} == 0 goto _iffalse_0($5)
main.c(176:40:43:0:0)		iTemp2 [k6 lr5:48 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{int}{ sir@ _integrateSample_i_1_1}[r0x55 r0x56 ] = - iTemp2 [k6 lr5:48 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{int}{ sir@ _integrateSample_i_1_1}[r0x55 r0x56 ]
main.c(176:42:45:0:0)	 _iffalse_0($5) :
main.c(177:43:46:0:0)		iTemp17 [k26 lr43:44 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{char} = iTemp3 [k8 lr6:48 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{int}{ sir@ _integrateSample_q_1_1}[r0x57 r0x58 ] < 0x0 {literal int}
main.c(177:44:47:0:0)		if iTemp17 [k26 lr43:44 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{char} == 0 goto _iffalse_1($7)
main.c(177:45:50:0:0)		iTemp3 [k8 lr6:48 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{int}{ sir@ _integrateSample_q_1_1}[r0x57 r0x58 ] = - iTemp3 [k8 lr6:48 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{int}{ sir@ _integrateSample_q_1_1}[r0x57 r0x58 ]
main.c(177:47:52:0:0)	 _iffalse_1($7) :
main.c(178:48:53:0:0)		iTemp19 [k28 lr48:49 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int}[r0x55 r0x56 ] = iTemp2 [k6 lr5:48 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{int}{ sir@ _integrateSample_i_1_1}[r0x55 r0x56 ] + iTemp3 [k8 lr6:48 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{int}{ sir@ _integrateSample_q_1_1}[r0x57 r0x58 ]
main.c(178:49:54:0:0)		ret iTemp19 [k28 lr48:49 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int}[r0x55 r0x56 ]
main.c(178:50:55:0:0)	 _return($8) :
main.c(178:51:56:0:0)		eproc _integrateSample [k1 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int function    }

----------------------------------------------------------------
Basic Block _eBBlock1 : loop Depth = 0 noPath = 0 , lastinLoop = 0
depth 1st num 5 : bbnum = 4 1st iCode = 40 , last iCode = 41
visited 0 : hasFcall = 0

defines bitVector :bitvector Size = 59 bSize = 8
Bits on { (43) (44) }

local defines bitVector :bitvector Size = 59 bSize = 8
Bits on { (43) (44) }

pointers Set bitvector :
in pointers Set bitvector :
inDefs Set bitvector :bitvector Size = 59 bSize = 8
Bits on { (2) (3) (6) (12) (13) (18) (19) (24) (25) (26) (30) (31) (32) (34) (35) (39) }

outDefs Set bitvector :bitvector Size = 59 bSize = 8
Bits on { (2) (3) (6) (12) (13) (18) (19) (24) (25) (30) (31) (32) (34) (35) (39) (43) (44) }

usesDefs Set bitvector :bitvector Size = 59 bSize = 8
Bits on { (26) (43) }

----------------------------------------------------------------
main.c(176:40:43:0:0)		iTemp2 [k6 lr5:48 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{int}{ sir@ _integrateSample_i_1_1}[r0x55 r0x56 ] = - iTemp2 [k6 lr5:48 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{int}{ sir@ _integrateSample_i_1_1}[r0x55 r0x56 ]
main.c(176:42:45:0:0)	 _iffalse_0($5) :
main.c(177:43:46:0:0)		iTemp17 [k26 lr43:44 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{char} = iTemp3 [k8 lr6:48 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{int}{ sir@ _integrateSample_q_1_1}[r0x57 r0x58 ] < 0x0 {literal int}
main.c(177:44:47:0:0)		if iTemp17 [k26 lr43:44 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{char} == 0 goto _iffalse_1($7)
main.c(177:45:50:0:0)		iTemp3 [k8 lr6:48 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{int}{ sir@ _integrateSample_q_1_1}[r0x57 r0x58 ] = - iTemp3 [k8 lr6:48 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{int}{ sir@ _integrateSample_q_1_1}[r0x57 r0x58 ]
main.c(177:47:52:0:0)	 _iffalse_1($7) :
main.c(178:48:53:0:0)		iTemp19 [k28 lr48:49 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int}[r0x55 r0x56 ] = iTemp2 [k6 lr5:48 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{int}{ sir@ _integrateSample_i_1_1}[r0x55 r0x56 ] + iTemp3 [k8 lr6:48 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{int}{ sir@ _integrateSample_q_1_1}[r0x57 r0x58 ]
main.c(178:49:54:0:0)		ret iTemp19 [k28 lr48:49 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int}[r0x55 r0x56 ]
main.c(178:50:55:0:0)	 _return($8) :
main.c(178:51:56:0:0)		eproc _integrateSample [k1 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int function    }

----------------------------------------------------------------
Basic Block _iffalse_0 : loop Depth = 0 noPath = 0 , lastinLoop = 0
depth 1st num 6 : bbnum = 5 1st iCode = 42 , last iCode = 44
visited 0 : hasFcall = 0

defines bitVector :bitvector Size = 59 bSize = 8
Bits on { (46) }

local defines bitVector :bitvector Size = 59 bSize = 8
Bits on { (46) }

pointers Set bitvector :
in pointers Set bitvector :
inDefs Set bitvector :bitvector Size = 59 bSize = 8
Bits on { (2) (3) (6) (12) (13) (18) (19) (24) (25) (26) (30) (31) (32) (34) (35) (39) (43) (44) }

outDefs Set bitvector :bitvector Size = 59 bSize = 8
Bits on { (2) (3) (6) (12) (13) (18) (19) (24) (25) (26) (30) (31) (32) (34) (35) (39) (43) (44) (46) }

usesDefs Set bitvector :bitvector Size = 59 bSize = 8
Bits on { (32) (46) }

----------------------------------------------------------------
main.c(176:42:45:0:0)	 _iffalse_0($5) :
main.c(177:43:46:0:0)		iTemp17 [k26 lr43:44 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{char} = iTemp3 [k8 lr6:48 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{int}{ sir@ _integrateSample_q_1_1}[r0x57 r0x58 ] < 0x0 {literal int}
main.c(177:44:47:0:0)		if iTemp17 [k26 lr43:44 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{char} == 0 goto _iffalse_1($7)
main.c(177:45:50:0:0)		iTemp3 [k8 lr6:48 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{int}{ sir@ _integrateSample_q_1_1}[r0x57 r0x58 ] = - iTemp3 [k8 lr6:48 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{int}{ sir@ _integrateSample_q_1_1}[r0x57 r0x58 ]
main.c(177:47:52:0:0)	 _iffalse_1($7) :
main.c(178:48:53:0:0)		iTemp19 [k28 lr48:49 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int}[r0x55 r0x56 ] = iTemp2 [k6 lr5:48 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{int}{ sir@ _integrateSample_i_1_1}[r0x55 r0x56 ] + iTemp3 [k8 lr6:48 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{int}{ sir@ _integrateSample_q_1_1}[r0x57 r0x58 ]
main.c(178:49:54:0:0)		ret iTemp19 [k28 lr48:49 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int}[r0x55 r0x56 ]
main.c(178:50:55:0:0)	 _return($8) :
main.c(178:51:56:0:0)		eproc _integrateSample [k1 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int function    }

----------------------------------------------------------------
Basic Block _eBBlock2 : loop Depth = 0 noPath = 0 , lastinLoop = 0
depth 1st num 7 : bbnum = 6 1st iCode = 45 , last iCode = 46
visited 0 : hasFcall = 0

defines bitVector :bitvector Size = 59 bSize = 8
Bits on { (50) (51) }

local defines bitVector :bitvector Size = 59 bSize = 8
Bits on { (50) (51) }

pointers Set bitvector :
in pointers Set bitvector :
inDefs Set bitvector :bitvector Size = 59 bSize = 8
Bits on { (2) (3) (6) (12) (13) (18) (19) (24) (25) (26) (30) (31) (32) (34) (35) (39) (43) (44) (46) }

outDefs Set bitvector :bitvector Size = 59 bSize = 8
Bits on { (2) (3) (6) (12) (13) (18) (19) (24) (25) (26) (30) (31) (34) (35) (39) (43) (44) (46) (50) (51) }

usesDefs Set bitvector :bitvector Size = 59 bSize = 8
Bits on { (32) (50) }

----------------------------------------------------------------
main.c(177:45:50:0:0)		iTemp3 [k8 lr6:48 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{int}{ sir@ _integrateSample_q_1_1}[r0x57 r0x58 ] = - iTemp3 [k8 lr6:48 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{int}{ sir@ _integrateSample_q_1_1}[r0x57 r0x58 ]
main.c(177:47:52:0:0)	 _iffalse_1($7) :
main.c(178:48:53:0:0)		iTemp19 [k28 lr48:49 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int}[r0x55 r0x56 ] = iTemp2 [k6 lr5:48 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{int}{ sir@ _integrateSample_i_1_1}[r0x55 r0x56 ] + iTemp3 [k8 lr6:48 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{int}{ sir@ _integrateSample_q_1_1}[r0x57 r0x58 ]
main.c(178:49:54:0:0)		ret iTemp19 [k28 lr48:49 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int}[r0x55 r0x56 ]
main.c(178:50:55:0:0)	 _return($8) :
main.c(178:51:56:0:0)		eproc _integrateSample [k1 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int function    }

----------------------------------------------------------------
Basic Block _iffalse_1 : loop Depth = 0 noPath = 0 , lastinLoop = 0
depth 1st num 8 : bbnum = 7 1st iCode = 47 , last iCode = 49
visited 0 : hasFcall = 0

defines bitVector :bitvector Size = 59 bSize = 8
Bits on { (53) }

local defines bitVector :bitvector Size = 59 bSize = 8
Bits on { (53) }

pointers Set bitvector :
in pointers Set bitvector :
inDefs Set bitvector :bitvector Size = 59 bSize = 8
Bits on { (2) (3) (6) (12) (13) (18) (19) (24) (25) (26) (30) (31) (32) (34) (35) (39) (43) (44) (46) (50) (51) }

outDefs Set bitvector :bitvector Size = 59 bSize = 8
Bits on { (2) (3) (6) (12) (13) (18) (19) (24) (25) (26) (30) (31) (32) (34) (35) (39) (43) (44) (46) (50) (51) (53) }

usesDefs Set bitvector :bitvector Size = 59 bSize = 8
Bits on { (26) (32) (44) (51) (53) }

----------------------------------------------------------------
main.c(177:47:52:0:0)	 _iffalse_1($7) :
main.c(178:48:53:0:0)		iTemp19 [k28 lr48:49 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int}[r0x55 r0x56 ] = iTemp2 [k6 lr5:48 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{int}{ sir@ _integrateSample_i_1_1}[r0x55 r0x56 ] + iTemp3 [k8 lr6:48 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{int}{ sir@ _integrateSample_q_1_1}[r0x57 r0x58 ]
main.c(178:49:54:0:0)		ret iTemp19 [k28 lr48:49 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int}[r0x55 r0x56 ]
main.c(178:50:55:0:0)	 _return($8) :
main.c(178:51:56:0:0)		eproc _integrateSample [k1 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int function    }

----------------------------------------------------------------
Basic Block _return : loop Depth = 0 noPath = 0 , lastinLoop = 0
depth 1st num 9 : bbnum = 8 1st iCode = 50 , last iCode = 51
visited 0 : hasFcall = 0

defines bitVector :bitvector Size = 59 bSize = 8
Bits on { }

local defines bitVector :
pointers Set bitvector :
in pointers Set bitvector :
inDefs Set bitvector :bitvector Size = 59 bSize = 8
Bits on { (2) (3) (6) (12) (13) (18) (19) (24) (25) (26) (30) (31) (32) (34) (35) (39) (43) (44) (46) (50) (51) (53) }

outDefs Set bitvector :bitvector Size = 59 bSize = 8
Bits on { (2) (3) (6) (12) (13) (18) (19) (24) (25) (26) (30) (31) (32) (34) (35) (39) (43) (44) (46) (50) (51) (53) }

usesDefs Set bitvector :bitvector Size = 59 bSize = 8
Bits on { }

----------------------------------------------------------------
main.c(178:50:55:0:0)	 _return($8) :
main.c(178:51:56:0:0)		eproc _integrateSample [k1 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int function    }
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
  -- added _numIntegrationSamples to hash, size = 1
typeRegWithIdx - requesting index = 0x59
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x5a
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x59
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x5b
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x5a
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x5c
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
typeRegWithIdx - requesting index = 0x55
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x5e
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x5e
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
typeRegWithIdx - requesting index = 0x57
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x60
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x60
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
typeRegWithIdx - requesting index = 0x55
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x62
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x62
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
typeRegWithIdx - requesting index = 0x64
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
Found a Stack Register!
typeRegWithIdx - requesting index = 0x63
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x63
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x57
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x64
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x64
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x58
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x5b
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x5c
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x5b
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x5c
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
depth 1st num 1 : bbnum = 0 1st iCode = 1 , last iCode = 11
visited 0 : hasFcall = 0

defines bitVector :bitvector Size = 151 bSize = 19
Bits on { (2) (3) (4) (5) (6) (7) (8) (9) }

local defines bitVector :bitvector Size = 151 bSize = 19
Bits on { (2) (3) (4) (5) (6) (7) (8) (9) }

pointers Set bitvector :
in pointers Set bitvector :
inDefs Set bitvector :
outDefs Set bitvector :bitvector Size = 151 bSize = 19
Bits on { (2) (3) (4) (5) (6) (7) (8) (9) }

usesDefs Set bitvector :bitvector Size = 151 bSize = 19
Bits on { (2) (3) (4) (5) (9) }

----------------------------------------------------------------
main.c(182:1:0:0:0)	 _entry($28) :
main.c(182:2:1:0:0)		proc _sensorSample [k1 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int function    }
main.c(182:3:2:0:0)	iTemp0 [k2 lr3:65 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sensorSample_transmitter_1_1} = recv 
main.c(182:4:3:0:0)	iTemp1 [k4 lr4:9 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sensorSample_receiver_1_1} = recv 
main.c(182:5:4:0:0)	iTemp2 [k6 lr5:31 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sensorSample_phase_1_1} = recv 
main.c(188:6:5:0:0)		iTemp3 [k9 lr6:7 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{unsigned char} = _PORTB [k8 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{sfr volatile unsigned char} & 0xf {literal unsigned char}
main.c(188:7:6:0:0)		_PORTB [k8 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{sfr volatile unsigned char} := iTemp3 [k9 lr6:7 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{unsigned char}
main.c(189:8:7:0:0)		_TRISB [k10 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{sfr volatile unsigned char} := 0x0 {literal unsigned char}
main.c(192:9:8:0:0)		_ADCON0 [k11 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{sfr volatile unsigned char} := iTemp1 [k4 lr4:9 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sensorSample_receiver_1_1}
main.c(195:10:9:0:0)		iTemp4 [k12 lr10:11 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{char} = iTemp2 [k6 lr5:31 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sensorSample_phase_1_1} == 0x0 {literal unsigned char}
main.c(195:11:10:0:0)		if iTemp4 [k12 lr10:11 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{char} == 0 goto _iffalse_2($2)

----------------------------------------------------------------
Basic Block _eBBlock0 : loop Depth = 0 noPath = 0 , lastinLoop = 0
depth 1st num 2 : bbnum = 1 1st iCode = 12 , last iCode = 13
visited 0 : hasFcall = 0

defines bitVector :bitvector Size = 151 bSize = 19
Bits on { (13) }

local defines bitVector :bitvector Size = 151 bSize = 19
Bits on { (13) }

pointers Set bitvector :
in pointers Set bitvector :
inDefs Set bitvector :bitvector Size = 151 bSize = 19
Bits on { (2) (3) (4) (5) (6) (7) (8) (9) }

outDefs Set bitvector :bitvector Size = 151 bSize = 19
Bits on { (2) (3) (4) (5) (6) (7) (8) (9) (13) }

usesDefs Set bitvector :bitvector Size = 151 bSize = 19
Bits on { }

----------------------------------------------------------------
main.c(195:12:13:0:0)		_sensorSample_phase0_1_1 [k13 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{bit bit {0,1}} := 0x1 {literal bit {0,1}}
main.c(195:13:14:0:0)		 goto _ifend_2($3)

----------------------------------------------------------------
Basic Block _iffalse_2 : loop Depth = 0 noPath = 0 , lastinLoop = 0
depth 1st num 3 : bbnum = 2 1st iCode = 14 , last iCode = 15
visited 0 : hasFcall = 0

defines bitVector :bitvector Size = 151 bSize = 19
Bits on { (16) }

local defines bitVector :bitvector Size = 151 bSize = 19
Bits on { (16) }

pointers Set bitvector :
in pointers Set bitvector :
inDefs Set bitvector :bitvector Size = 151 bSize = 19
Bits on { (2) (3) (4) (5) (6) (7) (8) (9) }

outDefs Set bitvector :bitvector Size = 151 bSize = 19
Bits on { (2) (3) (4) (5) (6) (7) (8) (9) (16) }

usesDefs Set bitvector :bitvector Size = 151 bSize = 19
Bits on { }

----------------------------------------------------------------
main.c(195:14:15:0:0)	 _iffalse_2($2) :
main.c(195:15:16:0:0)		_sensorSample_phase0_1_1 [k13 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{bit bit {0,1}} := 0x0 {literal bit {0,1}}

----------------------------------------------------------------
Basic Block _ifend_2 : loop Depth = 0 noPath = 0 , lastinLoop = 0
depth 1st num 4 : bbnum = 3 1st iCode = 16 , last iCode = 18
visited 0 : hasFcall = 0

defines bitVector :bitvector Size = 151 bSize = 19
Bits on { (18) }

local defines bitVector :bitvector Size = 151 bSize = 19
Bits on { (18) }

pointers Set bitvector :
in pointers Set bitvector :
inDefs Set bitvector :bitvector Size = 151 bSize = 19
Bits on { (2) (3) (4) (5) (6) (7) (8) (9) (13) (16) }

outDefs Set bitvector :bitvector Size = 151 bSize = 19
Bits on { (2) (3) (4) (5) (6) (7) (8) (9) (13) (16) (18) }

usesDefs Set bitvector :bitvector Size = 151 bSize = 19
Bits on { (4) (18) }

----------------------------------------------------------------
main.c(195:16:17:0:0)	 _ifend_2($3) :
main.c(196:17:18:0:0)		iTemp5 [k14 lr17:18 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{char} = iTemp2 [k6 lr5:31 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sensorSample_phase_1_1} == 0x1 {literal unsigned char}
main.c(196:18:19:0:0)		if iTemp5 [k14 lr17:18 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{char} == 0 goto _iffalse_3($5)

----------------------------------------------------------------
Basic Block _eBBlock1 : loop Depth = 0 noPath = 0 , lastinLoop = 0
depth 1st num 5 : bbnum = 4 1st iCode = 19 , last iCode = 20
visited 0 : hasFcall = 0

defines bitVector :bitvector Size = 151 bSize = 19
Bits on { (22) }

local defines bitVector :bitvector Size = 151 bSize = 19
Bits on { (22) }

pointers Set bitvector :
in pointers Set bitvector :
inDefs Set bitvector :bitvector Size = 151 bSize = 19
Bits on { (2) (3) (4) (5) (6) (7) (8) (9) (13) (16) (18) }

outDefs Set bitvector :bitvector Size = 151 bSize = 19
Bits on { (2) (3) (4) (5) (6) (7) (8) (9) (13) (16) (18) (22) }

usesDefs Set bitvector :bitvector Size = 151 bSize = 19
Bits on { }

----------------------------------------------------------------
main.c(196:19:22:0:0)		_sensorSample_phase1_1_1 [k15 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{bit bit {0,1}} := 0x1 {literal bit {0,1}}
main.c(196:20:23:0:0)		 goto _ifend_3($6)

----------------------------------------------------------------
Basic Block _iffalse_3 : loop Depth = 0 noPath = 0 , lastinLoop = 0
depth 1st num 6 : bbnum = 5 1st iCode = 21 , last iCode = 22
visited 0 : hasFcall = 0

defines bitVector :bitvector Size = 151 bSize = 19
Bits on { (25) }

local defines bitVector :bitvector Size = 151 bSize = 19
Bits on { (25) }

pointers Set bitvector :
in pointers Set bitvector :
inDefs Set bitvector :bitvector Size = 151 bSize = 19
Bits on { (2) (3) (4) (5) (6) (7) (8) (9) (13) (16) (18) }

outDefs Set bitvector :bitvector Size = 151 bSize = 19
Bits on { (2) (3) (4) (5) (6) (7) (8) (9) (13) (16) (18) (25) }

usesDefs Set bitvector :bitvector Size = 151 bSize = 19
Bits on { }

----------------------------------------------------------------
main.c(196:21:24:0:0)	 _iffalse_3($5) :
main.c(196:22:25:0:0)		_sensorSample_phase1_1_1 [k15 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{bit bit {0,1}} := 0x0 {literal bit {0,1}}

----------------------------------------------------------------
Basic Block _ifend_3 : loop Depth = 0 noPath = 0 , lastinLoop = 0
depth 1st num 7 : bbnum = 6 1st iCode = 23 , last iCode = 25
visited 0 : hasFcall = 0

defines bitVector :bitvector Size = 151 bSize = 19
Bits on { (27) }

local defines bitVector :bitvector Size = 151 bSize = 19
Bits on { (27) }

pointers Set bitvector :
in pointers Set bitvector :
inDefs Set bitvector :bitvector Size = 151 bSize = 19
Bits on { (2) (3) (4) (5) (6) (7) (8) (9) (13) (16) (18) (22) (25) }

outDefs Set bitvector :bitvector Size = 151 bSize = 19
Bits on { (2) (3) (4) (5) (6) (7) (8) (9) (13) (16) (18) (22) (25) (27) }

usesDefs Set bitvector :bitvector Size = 151 bSize = 19
Bits on { (4) (27) }

----------------------------------------------------------------
main.c(196:23:26:0:0)	 _ifend_3($6) :
main.c(197:24:27:0:0)		iTemp6 [k16 lr24:25 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{char} = iTemp2 [k6 lr5:31 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sensorSample_phase_1_1} == 0x2 {literal unsigned char}
main.c(197:25:28:0:0)		if iTemp6 [k16 lr24:25 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{char} == 0 goto _iffalse_4($8)

----------------------------------------------------------------
Basic Block _eBBlock2 : loop Depth = 0 noPath = 0 , lastinLoop = 0
depth 1st num 8 : bbnum = 7 1st iCode = 26 , last iCode = 27
visited 0 : hasFcall = 0

defines bitVector :bitvector Size = 151 bSize = 19
Bits on { (31) }

local defines bitVector :bitvector Size = 151 bSize = 19
Bits on { (31) }

pointers Set bitvector :
in pointers Set bitvector :
inDefs Set bitvector :bitvector Size = 151 bSize = 19
Bits on { (2) (3) (4) (5) (6) (7) (8) (9) (13) (16) (18) (22) (25) (27) }

outDefs Set bitvector :bitvector Size = 151 bSize = 19
Bits on { (2) (3) (4) (5) (6) (7) (8) (9) (13) (16) (18) (22) (25) (27) (31) }

usesDefs Set bitvector :bitvector Size = 151 bSize = 19
Bits on { }

----------------------------------------------------------------
main.c(197:26:31:0:0)		_sensorSample_phase2_1_1 [k17 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{bit bit {0,1}} := 0x1 {literal bit {0,1}}
main.c(197:27:32:0:0)		 goto _ifend_4($9)

----------------------------------------------------------------
Basic Block _iffalse_4 : loop Depth = 0 noPath = 0 , lastinLoop = 0
depth 1st num 9 : bbnum = 8 1st iCode = 28 , last iCode = 29
visited 0 : hasFcall = 0

defines bitVector :bitvector Size = 151 bSize = 19
Bits on { (34) }

local defines bitVector :bitvector Size = 151 bSize = 19
Bits on { (34) }

pointers Set bitvector :
in pointers Set bitvector :
inDefs Set bitvector :bitvector Size = 151 bSize = 19
Bits on { (2) (3) (4) (5) (6) (7) (8) (9) (13) (16) (18) (22) (25) (27) }

outDefs Set bitvector :bitvector Size = 151 bSize = 19
Bits on { (2) (3) (4) (5) (6) (7) (8) (9) (13) (16) (18) (22) (25) (27) (34) }

usesDefs Set bitvector :bitvector Size = 151 bSize = 19
Bits on { }

----------------------------------------------------------------
main.c(197:28:33:0:0)	 _iffalse_4($8) :
main.c(197:29:34:0:0)		_sensorSample_phase2_1_1 [k17 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{bit bit {0,1}} := 0x0 {literal bit {0,1}}

----------------------------------------------------------------
Basic Block _ifend_4 : loop Depth = 0 noPath = 0 , lastinLoop = 0
depth 1st num 10 : bbnum = 9 1st iCode = 30 , last iCode = 32
visited 0 : hasFcall = 0

defines bitVector :bitvector Size = 151 bSize = 19
Bits on { (36) }

local defines bitVector :bitvector Size = 151 bSize = 19
Bits on { (36) }

pointers Set bitvector :
in pointers Set bitvector :
inDefs Set bitvector :bitvector Size = 151 bSize = 19
Bits on { (2) (3) (4) (5) (6) (7) (8) (9) (13) (16) (18) (22) (25) (27) (31) (34) }

outDefs Set bitvector :bitvector Size = 151 bSize = 19
Bits on { (2) (3) (4) (5) (6) (7) (8) (9) (13) (16) (18) (22) (25) (27) (31) (34) (36) }

usesDefs Set bitvector :bitvector Size = 151 bSize = 19
Bits on { (4) (36) }

----------------------------------------------------------------
main.c(197:30:35:0:0)	 _ifend_4($9) :
main.c(198:31:36:0:0)		iTemp7 [k18 lr31:32 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{char} = iTemp2 [k6 lr5:31 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sensorSample_phase_1_1} == 0x3 {literal unsigned char}
main.c(198:32:37:0:0)		if iTemp7 [k18 lr31:32 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{char} == 0 goto _iffalse_5($11)

----------------------------------------------------------------
Basic Block _eBBlock3 : loop Depth = 0 noPath = 0 , lastinLoop = 0
depth 1st num 11 : bbnum = 10 1st iCode = 33 , last iCode = 34
visited 0 : hasFcall = 0

defines bitVector :bitvector Size = 151 bSize = 19
Bits on { (40) }

local defines bitVector :bitvector Size = 151 bSize = 19
Bits on { (40) }

pointers Set bitvector :
in pointers Set bitvector :
inDefs Set bitvector :bitvector Size = 151 bSize = 19
Bits on { (2) (3) (4) (5) (6) (7) (8) (9) (13) (16) (18) (22) (25) (27) (31) (34) (36) }

outDefs Set bitvector :bitvector Size = 151 bSize = 19
Bits on { (2) (3) (4) (5) (6) (7) (8) (9) (13) (16) (18) (22) (25) (27) (31) (34) (36) (40) }

usesDefs Set bitvector :bitvector Size = 151 bSize = 19
Bits on { }

----------------------------------------------------------------
main.c(198:33:40:0:0)		_sensorSample_phase3_1_1 [k19 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{bit bit {0,1}} := 0x1 {literal bit {0,1}}
main.c(198:34:41:0:0)		 goto _ifend_5($12)

----------------------------------------------------------------
Basic Block _iffalse_5 : loop Depth = 0 noPath = 0 , lastinLoop = 0
depth 1st num 12 : bbnum = 11 1st iCode = 35 , last iCode = 36
visited 0 : hasFcall = 0

defines bitVector :bitvector Size = 151 bSize = 19
Bits on { (43) }

local defines bitVector :bitvector Size = 151 bSize = 19
Bits on { (43) }

pointers Set bitvector :
in pointers Set bitvector :
inDefs Set bitvector :bitvector Size = 151 bSize = 19
Bits on { (2) (3) (4) (5) (6) (7) (8) (9) (13) (16) (18) (22) (25) (27) (31) (34) (36) }

outDefs Set bitvector :bitvector Size = 151 bSize = 19
Bits on { (2) (3) (4) (5) (6) (7) (8) (9) (13) (16) (18) (22) (25) (27) (31) (34) (36) (43) }

usesDefs Set bitvector :bitvector Size = 151 bSize = 19
Bits on { }

----------------------------------------------------------------
main.c(198:35:42:0:0)	 _iffalse_5($11) :
main.c(198:36:43:0:0)		_sensorSample_phase3_1_1 [k19 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{bit bit {0,1}} := 0x0 {literal bit {0,1}}

----------------------------------------------------------------
Basic Block _ifend_5 : loop Depth = 0 noPath = 0 , lastinLoop = 0
depth 1st num 13 : bbnum = 12 1st iCode = 37 , last iCode = 38
visited 0 : hasFcall = 0

defines bitVector :bitvector Size = 151 bSize = 19
Bits on { (45) }

local defines bitVector :bitvector Size = 151 bSize = 19
Bits on { (45) }

pointers Set bitvector :
in pointers Set bitvector :
inDefs Set bitvector :bitvector Size = 151 bSize = 19
Bits on { (2) (3) (4) (5) (6) (7) (8) (9) (13) (16) (18) (22) (25) (27) (31) (34) (36) (40) (43) }

outDefs Set bitvector :bitvector Size = 151 bSize = 19
Bits on { (2) (3) (4) (5) (6) (7) (8) (9) (13) (16) (18) (22) (25) (27) (31) (34) (36) (40) (43) (45) }

usesDefs Set bitvector :bitvector Size = 151 bSize = 19
Bits on { }

----------------------------------------------------------------
main.c(198:37:44:0:0)	 _ifend_5($12) :
main.c(204:38:45:0:0)		iTemp8 [k20 lr38:40 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sensorSample_halfcycles_1_1} := _numResonationHalfcycles [k22 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{unsigned char}

----------------------------------------------------------------
Basic Block preHeaderLbl0 : loop Depth = 0 noPath = 0 , lastinLoop = 0
depth 1st num 14 : bbnum = 13 1st iCode = 39 , last iCode = 40
visited 0 : hasFcall = 0

defines bitVector :bitvector Size = 151 bSize = 19
Bits on { (150) }

local defines bitVector :bitvector Size = 151 bSize = 19
Bits on { (150) }

pointers Set bitvector :
in pointers Set bitvector :
inDefs Set bitvector :bitvector Size = 151 bSize = 19
Bits on { (2) (3) (4) (5) (6) (7) (8) (9) (13) (16) (18) (22) (25) (27) (31) (34) (36) (40) (43) (45) }

outDefs Set bitvector :bitvector Size = 151 bSize = 19
Bits on { (2) (3) (4) (5) (6) (7) (8) (9) (13) (16) (18) (22) (25) (27) (31) (34) (36) (40) (43) (45) (150) }

usesDefs Set bitvector :bitvector Size = 151 bSize = 19
Bits on { (45) }

----------------------------------------------------------------
main.c(205:39:148:0:0)	 preHeaderLbl0($33) :
main.c(205:40:150:0:0)		iTemp10 [k24 lr40:65 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sensorSample_halfcycles_1_1} := iTemp8 [k20 lr38:40 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sensorSample_halfcycles_1_1}

----------------------------------------------------------------
Basic Block _dobody_1 : loop Depth = 1 noPath = 0 , lastinLoop = 1
depth 1st num 15 : bbnum = 14 1st iCode = 41 , last iCode = 65
visited 0 : hasFcall = 0

defines bitVector :bitvector Size = 151 bSize = 19
Bits on { (47) (48) (70) }

local defines bitVector :bitvector Size = 151 bSize = 19
Bits on { (47) (48) (70) (71) }

pointers Set bitvector :
in pointers Set bitvector :
inDefs Set bitvector :bitvector Size = 151 bSize = 19
Bits on { (2) (3) (4) (5) (6) (7) (8) (9) (13) (16) (18) (22) (25) (27) (31) (34) (36) (40) (43) (45) (47) (48) (70) (71) (150) }

outDefs Set bitvector :bitvector Size = 151 bSize = 19
Bits on { (2) (3) (4) (5) (7) (8) (9) (13) (16) (18) (22) (25) (27) (31) (34) (36) (40) (43) (47) (48) (70) }

usesDefs Set bitvector :bitvector Size = 151 bSize = 19
Bits on { (2) (6) (47) (48) (70) (150) }

----------------------------------------------------------------
main.c(205:41:46:1:0)	 _dobody_1($13) :
main.c(206:42:47:1:0)		iTemp9 [k23 lr42:43 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{unsigned char} = _PORTB [k8 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{sfr volatile unsigned char} ^ iTemp0 [k2 lr3:65 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sensorSample_transmitter_1_1}
main.c(206:43:48:1:0)		_PORTB [k8 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{sfr volatile unsigned char} := iTemp9 [k23 lr42:43 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{unsigned char}
main.c(207:44:49:1:0)	 nop main.c(207:45:50:1:0)	 nop main.c(207:46:51:1:0)	 nop main.c(208:47:52:1:0)	 nop main.c(208:48:53:1:0)	 nop main.c(208:49:54:1:0)	 nop main.c(208:50:55:1:0)	 nop main.c(208:51:56:1:0)	 nop main.c(209:52:57:1:0)	 nop main.c(209:53:58:1:0)	 nop main.c(209:54:59:1:0)	 nop main.c(209:55:60:1:0)	 nop main.c(209:56:61:1:0)	 nop main.c(210:57:62:1:0)	 nop main.c(210:58:63:1:0)	 nop main.c(210:59:64:1:0)	 nop main.c(210:60:65:1:0)	 nop main.c(210:61:66:1:0)	 nop main.c(211:62:67:1:0)	 nop main.c(211:63:68:1:0)	 nop main.c(212:64:70:1:0)		iTemp10 [k24 lr40:65 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sensorSample_halfcycles_1_1} = iTemp10 [k24 lr40:65 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sensorSample_halfcycles_1_1} - 0x1 {literal unsigned char}
main.c(212:65:72:1:0)		if iTemp10 [k24 lr40:65 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sensorSample_halfcycles_1_1} != 0 goto _dobody_1($13)

----------------------------------------------------------------
Basic Block _eBBlock4 : loop Depth = 0 noPath = 0 , lastinLoop = 0
depth 1st num 16 : bbnum = 15 1st iCode = 66 , last iCode = 66
visited 0 : hasFcall = 0

defines bitVector :bitvector Size = 151 bSize = 19
Bits on { }

local defines bitVector :
pointers Set bitvector :
in pointers Set bitvector :
inDefs Set bitvector :bitvector Size = 151 bSize = 19
Bits on { (2) (3) (4) (5) (7) (8) (9) (13) (16) (18) (22) (25) (27) (31) (34) (36) (40) (43) (47) (48) (70) (71) }

outDefs Set bitvector :bitvector Size = 151 bSize = 19
Bits on { (2) (3) (4) (5) (7) (8) (9) (13) (16) (18) (22) (25) (27) (31) (34) (36) (40) (43) (47) (48) (70) (71) }

usesDefs Set bitvector :bitvector Size = 151 bSize = 19
Bits on { (13) (16) }

----------------------------------------------------------------
main.c(215:66:75:0:0)		if _sensorSample_phase0_1_1 [k13 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{bit bit {0,1}} == 0 goto _iffalse_6($17)

----------------------------------------------------------------
Basic Block _eBBlock5 : loop Depth = 0 noPath = 0 , lastinLoop = 0
depth 1st num 17 : bbnum = 16 1st iCode = 67 , last iCode = 67
visited 0 : hasFcall = 0

defines bitVector :bitvector Size = 151 bSize = 19
Bits on { (79) }

local defines bitVector :bitvector Size = 151 bSize = 19
Bits on { (79) }

pointers Set bitvector :
in pointers Set bitvector :
inDefs Set bitvector :bitvector Size = 151 bSize = 19
Bits on { (2) (3) (4) (5) (7) (8) (9) (13) (16) (18) (22) (25) (27) (31) (34) (36) (40) (43) (47) (48) (70) (71) }

outDefs Set bitvector :bitvector Size = 151 bSize = 19
Bits on { (2) (3) (4) (5) (7) (8) (9) (13) (16) (18) (22) (25) (27) (31) (34) (36) (40) (43) (47) (48) (70) (71) (79) }

usesDefs Set bitvector :bitvector Size = 151 bSize = 19
Bits on { }

----------------------------------------------------------------
main.c(216:67:79:0:0)		_GO_DONE [k25 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{bit bit {0,1}} := 0x1 {literal bit {0,1}}

----------------------------------------------------------------
Basic Block _iffalse_6 : loop Depth = 0 noPath = 0 , lastinLoop = 0
depth 1st num 18 : bbnum = 17 1st iCode = 68 , last iCode = 79
visited 0 : hasFcall = 0

defines bitVector :bitvector Size = 151 bSize = 19
Bits on { }

local defines bitVector :
pointers Set bitvector :
in pointers Set bitvector :
inDefs Set bitvector :bitvector Size = 151 bSize = 19
Bits on { (2) (3) (4) (5) (7) (8) (9) (13) (16) (18) (22) (25) (27) (31) (34) (36) (40) (43) (47) (48) (70) (71) (79) }

outDefs Set bitvector :bitvector Size = 151 bSize = 19
Bits on { (2) (3) (4) (5) (7) (8) (9) (13) (16) (18) (22) (25) (27) (31) (34) (36) (40) (43) (47) (48) (70) (71) (79) }

usesDefs Set bitvector :bitvector Size = 151 bSize = 19
Bits on { (22) (25) }

----------------------------------------------------------------
main.c(216:68:80:0:0)	 _iffalse_6($17) :
main.c(217:69:81:0:0)	 nop main.c(217:70:82:0:0)	 nop main.c(217:71:83:0:0)	 nop main.c(217:72:84:0:0)	 nop main.c(217:73:85:0:0)	 nop main.c(218:74:86:0:0)	 nop main.c(218:75:87:0:0)	 nop main.c(218:76:88:0:0)	 nop main.c(218:77:89:0:0)	 nop main.c(218:78:90:0:0)	 nop main.c(219:79:91:0:0)		if _sensorSample_phase1_1_1 [k15 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{bit bit {0,1}} == 0 goto _iffalse_7($19)

----------------------------------------------------------------
Basic Block _eBBlock6 : loop Depth = 0 noPath = 0 , lastinLoop = 0
depth 1st num 19 : bbnum = 18 1st iCode = 80 , last iCode = 80
visited 0 : hasFcall = 0

defines bitVector :bitvector Size = 151 bSize = 19
Bits on { (95) }

local defines bitVector :bitvector Size = 151 bSize = 19
Bits on { (95) }

pointers Set bitvector :
in pointers Set bitvector :
inDefs Set bitvector :bitvector Size = 151 bSize = 19
Bits on { (2) (3) (4) (5) (7) (8) (9) (13) (16) (18) (22) (25) (27) (31) (34) (36) (40) (43) (47) (48) (70) (71) (79) }

outDefs Set bitvector :bitvector Size = 151 bSize = 19
Bits on { (2) (3) (4) (5) (7) (8) (9) (13) (16) (18) (22) (25) (27) (31) (34) (36) (40) (43) (47) (48) (70) (71) (95) }

usesDefs Set bitvector :bitvector Size = 151 bSize = 19
Bits on { }

----------------------------------------------------------------
main.c(220:80:95:0:0)		_GO_DONE [k25 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{bit bit {0,1}} := 0x1 {literal bit {0,1}}

----------------------------------------------------------------
Basic Block _iffalse_7 : loop Depth = 0 noPath = 0 , lastinLoop = 0
depth 1st num 20 : bbnum = 19 1st iCode = 81 , last iCode = 93
visited 0 : hasFcall = 0

defines bitVector :bitvector Size = 151 bSize = 19
Bits on { }

local defines bitVector :
pointers Set bitvector :
in pointers Set bitvector :
inDefs Set bitvector :bitvector Size = 151 bSize = 19
Bits on { (2) (3) (4) (5) (7) (8) (9) (13) (16) (18) (22) (25) (27) (31) (34) (36) (40) (43) (47) (48) (70) (71) (79) (95) }

outDefs Set bitvector :bitvector Size = 151 bSize = 19
Bits on { (2) (3) (4) (5) (7) (8) (9) (13) (16) (18) (22) (25) (27) (31) (34) (36) (40) (43) (47) (48) (70) (71) (79) (95) }

usesDefs Set bitvector :bitvector Size = 151 bSize = 19
Bits on { (31) (34) }

----------------------------------------------------------------
main.c(220:81:96:0:0)	 _iffalse_7($19) :
main.c(221:82:97:0:0)	 nop main.c(221:83:98:0:0)	 nop main.c(221:84:99:0:0)	 nop main.c(221:85:100:0:0)	 nop main.c(221:86:101:0:0)	 nop main.c(222:87:102:0:0)	 nop main.c(222:88:103:0:0)	 nop main.c(222:89:104:0:0)	 nop main.c(222:90:105:0:0)	 nop main.c(222:91:106:0:0)	 nop main.c(222:92:107:0:0)	 nop main.c(223:93:108:0:0)		if _sensorSample_phase2_1_1 [k17 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{bit bit {0,1}} == 0 goto _iffalse_8($21)

----------------------------------------------------------------
Basic Block _eBBlock7 : loop Depth = 0 noPath = 0 , lastinLoop = 0
depth 1st num 21 : bbnum = 20 1st iCode = 94 , last iCode = 94
visited 0 : hasFcall = 0

defines bitVector :bitvector Size = 151 bSize = 19
Bits on { (112) }

local defines bitVector :bitvector Size = 151 bSize = 19
Bits on { (112) }

pointers Set bitvector :
in pointers Set bitvector :
inDefs Set bitvector :bitvector Size = 151 bSize = 19
Bits on { (2) (3) (4) (5) (7) (8) (9) (13) (16) (18) (22) (25) (27) (31) (34) (36) (40) (43) (47) (48) (70) (71) (79) (95) }

outDefs Set bitvector :bitvector Size = 151 bSize = 19
Bits on { (2) (3) (4) (5) (7) (8) (9) (13) (16) (18) (22) (25) (27) (31) (34) (36) (40) (43) (47) (48) (70) (71) (112) }

usesDefs Set bitvector :bitvector Size = 151 bSize = 19
Bits on { }

----------------------------------------------------------------
main.c(224:94:112:0:0)		_GO_DONE [k25 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{bit bit {0,1}} := 0x1 {literal bit {0,1}}

----------------------------------------------------------------
Basic Block _iffalse_8 : loop Depth = 0 noPath = 0 , lastinLoop = 0
depth 1st num 22 : bbnum = 21 1st iCode = 95 , last iCode = 106
visited 0 : hasFcall = 0

defines bitVector :bitvector Size = 151 bSize = 19
Bits on { }

local defines bitVector :
pointers Set bitvector :
in pointers Set bitvector :
inDefs Set bitvector :bitvector Size = 151 bSize = 19
Bits on { (2) (3) (4) (5) (7) (8) (9) (13) (16) (18) (22) (25) (27) (31) (34) (36) (40) (43) (47) (48) (70) (71) (79) (95) (112) }

outDefs Set bitvector :bitvector Size = 151 bSize = 19
Bits on { (2) (3) (4) (5) (7) (8) (9) (13) (16) (18) (22) (25) (27) (31) (34) (36) (40) (43) (47) (48) (70) (71) (79) (95) (112) }

usesDefs Set bitvector :bitvector Size = 151 bSize = 19
Bits on { (40) (43) }

----------------------------------------------------------------
main.c(224:95:113:0:0)	 _iffalse_8($21) :
main.c(225:96:114:0:0)	 nop main.c(225:97:115:0:0)	 nop main.c(225:98:116:0:0)	 nop main.c(225:99:117:0:0)	 nop main.c(225:100:118:0:0)	 nop main.c(226:101:119:0:0)	 nop main.c(226:102:120:0:0)	 nop main.c(226:103:121:0:0)	 nop main.c(226:104:122:0:0)	 nop main.c(226:105:123:0:0)	 nop main.c(227:106:124:0:0)		if _sensorSample_phase3_1_1 [k19 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{bit bit {0,1}} == 0 goto preHeaderLbl1($39)

----------------------------------------------------------------
Basic Block _eBBlock8 : loop Depth = 0 noPath = 0 , lastinLoop = 0
depth 1st num 23 : bbnum = 22 1st iCode = 107 , last iCode = 107
visited 0 : hasFcall = 0

defines bitVector :bitvector Size = 151 bSize = 19
Bits on { (128) }

local defines bitVector :bitvector Size = 151 bSize = 19
Bits on { (128) }

pointers Set bitvector :
in pointers Set bitvector :
inDefs Set bitvector :bitvector Size = 151 bSize = 19
Bits on { (2) (3) (4) (5) (7) (8) (9) (13) (16) (18) (22) (25) (27) (31) (34) (36) (40) (43) (47) (48) (70) (71) (79) (95) (112) }

outDefs Set bitvector :bitvector Size = 151 bSize = 19
Bits on { (2) (3) (4) (5) (7) (8) (9) (13) (16) (18) (22) (25) (27) (31) (34) (36) (40) (43) (47) (48) (70) (71) (128) }

usesDefs Set bitvector :bitvector Size = 151 bSize = 19
Bits on { }

----------------------------------------------------------------
main.c(228:107:128:0:0)		_GO_DONE [k25 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{bit bit {0,1}} := 0x1 {literal bit {0,1}}

----------------------------------------------------------------
Basic Block preHeaderLbl1 : loop Depth = 0 noPath = 0 , lastinLoop = 0
depth 1st num 24 : bbnum = 23 1st iCode = 108 , last iCode = 108
visited 0 : hasFcall = 0

defines bitVector :bitvector Size = 151 bSize = 19
Bits on { }

local defines bitVector :
pointers Set bitvector :
in pointers Set bitvector :
inDefs Set bitvector :bitvector Size = 151 bSize = 19
Bits on { (2) (3) (4) (5) (7) (8) (9) (13) (16) (18) (22) (25) (27) (31) (34) (36) (40) (43) (47) (48) (70) (71) (79) (95) (112) (128) }

outDefs Set bitvector :bitvector Size = 151 bSize = 19
Bits on { (2) (3) (4) (5) (7) (8) (9) (13) (16) (18) (22) (25) (27) (31) (34) (36) (40) (43) (47) (48) (70) (71) (79) (95) (112) (128) }

usesDefs Set bitvector :bitvector Size = 151 bSize = 19
Bits on { }

----------------------------------------------------------------
main.c(231:108:149:0:0)	 preHeaderLbl1($39) :

----------------------------------------------------------------
Basic Block _whilecontinue_1 : loop Depth = 1 noPath = 0 , lastinLoop = 0
depth 1st num 25 : bbnum = 24 1st iCode = 109 , last iCode = 110
visited 0 : hasFcall = 0

defines bitVector :bitvector Size = 151 bSize = 19
Bits on { }

local defines bitVector :
pointers Set bitvector :
in pointers Set bitvector :
inDefs Set bitvector :bitvector Size = 151 bSize = 19
Bits on { (2) (3) (4) (5) (7) (8) (9) (13) (16) (18) (22) (25) (27) (31) (34) (36) (40) (43) (47) (48) (70) (71) (78) (79) (94) (95) (111) (112) (127) (128) }

outDefs Set bitvector :bitvector Size = 151 bSize = 19
Bits on { (2) (3) (4) (5) (7) (8) (9) (13) (16) (18) (22) (25) (27) (31) (34) (36) (40) (43) (47) (48) (70) (71) (78) (79) (94) (95) (111) (112) (127) (128) }

usesDefs Set bitvector :bitvector Size = 151 bSize = 19
Bits on { }

----------------------------------------------------------------
main.c(231:109:130:1:0)	 _whilecontinue_1($24) :
main.c(231:110:131:1:0)		if _NOT_DONE [k30 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{bit bit {0,1}} != 0 goto _whilecontinue_1($24)

----------------------------------------------------------------
Basic Block _eBBlock9 : loop Depth = 0 noPath = 0 , lastinLoop = 0
depth 1st num 26 : bbnum = 25 1st iCode = 111 , last iCode = 118
visited 0 : hasFcall = 0

defines bitVector :bitvector Size = 151 bSize = 19
Bits on { (136) (137) (138) (139) (140) (142) (143) }

local defines bitVector :bitvector Size = 151 bSize = 19
Bits on { (136) (137) (138) (139) (140) (142) (143) }

pointers Set bitvector :
in pointers Set bitvector :
inDefs Set bitvector :bitvector Size = 151 bSize = 19
Bits on { (2) (3) (4) (5) (7) (8) (9) (13) (16) (18) (22) (25) (27) (31) (34) (36) (40) (43) (47) (48) (70) (71) (78) (79) (94) (95) (111) (112) (127) (128) }

outDefs Set bitvector :bitvector Size = 151 bSize = 19
Bits on { (2) (3) (4) (5) (8) (9) (13) (16) (18) (22) (25) (27) (31) (34) (36) (40) (43) (47) (48) (70) (71) (78) (79) (94) (95) (111) (112) (127) (128) (136) (137) (138) (139) (140) (142) (143) }

usesDefs Set bitvector :bitvector Size = 151 bSize = 19
Bits on { (137) (138) (139) (140) (142) (143) }

----------------------------------------------------------------
main.c(234:111:136:0:0)		_TRISB [k10 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{sfr volatile unsigned char} := 0xf0 {literal unsigned char}
main.c(236:112:137:0:0)		iTemp15 [k31 lr112:114 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sensorSample_h_1_1} := _ADRESH [k33 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{sfr volatile unsigned char}
main.c(237:113:138:0:0)		iTemp16 [k34 lr113:116 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sensorSample_l_1_1} := _ADRESL [k36 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{sfr volatile unsigned char}
main.c(238:114:139:0:0)		iTemp17 [k37 lr114:115 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int} = (int)iTemp15 [k31 lr112:114 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sensorSample_h_1_1}
main.c(238:115:140:0:0)		iTemp18 [k38 lr115:117 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int} = iTemp17 [k37 lr114:115 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int} << 0x8 {literal unsigned char}
main.c(238:116:142:0:0)		iTemp20 [k40 lr116:117 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{unsigned int} = (unsigned int)iTemp16 [k34 lr113:116 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sensorSample_l_1_1}
main.c(238:117:143:0:0)		iTemp21 [k41 lr117:118 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{unsigned int} = iTemp18 [k38 lr115:117 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int} | iTemp20 [k40 lr116:117 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{unsigned int}
main.c(238:118:145:0:0)		ret iTemp21 [k41 lr117:118 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{unsigned int}

----------------------------------------------------------------
Basic Block _return : loop Depth = 0 noPath = 0 , lastinLoop = 0
depth 1st num 27 : bbnum = 26 1st iCode = 119 , last iCode = 120
visited 0 : hasFcall = 0

defines bitVector :bitvector Size = 151 bSize = 19
Bits on { }

local defines bitVector :
pointers Set bitvector :
in pointers Set bitvector :
inDefs Set bitvector :bitvector Size = 151 bSize = 19
Bits on { (2) (3) (4) (5) (8) (9) (13) (16) (18) (22) (25) (27) (31) (34) (36) (40) (43) (47) (48) (70) (71) (78) (79) (94) (95) (111) (112) (127) (128) (136) (137) (138) (139) (140) (142) (143) }

outDefs Set bitvector :bitvector Size = 151 bSize = 19
Bits on { (2) (3) (4) (5) (8) (9) (13) (16) (18) (22) (25) (27) (31) (34) (36) (40) (43) (47) (48) (70) (71) (78) (79) (94) (95) (111) (112) (127) (128) (136) (137) (138) (139) (140) (142) (143) }

usesDefs Set bitvector :bitvector Size = 151 bSize = 19
Bits on { }

----------------------------------------------------------------
main.c(238:119:146:0:0)	 _return($27) :
main.c(238:120:147:0:0)		eproc _sensorSample [k1 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int function    }
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
  right:0x0 {literal unsigned char}
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
  right:0x0 {literal unsigned char}
packRegsForAssign
  result:_ADCON0 [k11 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{sfr volatile unsigned char}
  left:
  right:iTemp1 [k4 lr4:9 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sensorSample_receiver_1_1}
allocDirReg symbol name _ADCON0
  637  storage class 3 
 642  integral
 646  specifier
_ADCON0 [k11 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{sfr volatile unsigned char}
  -- added _ADCON0 to hash, size = 1
  -- and it is at a fixed address 0x1f
  2695 - result is not temp
  2750 - dic is VOLATILE 
  result:_ADCON0 [k11 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{sfr volatile unsigned char}
  left:
  right:iTemp1 [k4 lr4:9 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sensorSample_receiver_1_1}
packRegsForAssign
  result:_TRISB [k10 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{sfr volatile unsigned char}
  left:
  right:0x0 {literal unsigned char}
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
  right:0x0 {literal unsigned char}
packRegsForAssign
  result:_ADCON0 [k11 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{sfr volatile unsigned char}
  left:
  right:iTemp1 [k4 lr4:9 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sensorSample_receiver_1_1}
allocDirReg symbol name _ADCON0
  637  storage class 3 
 642  integral
 646  specifier
_ADCON0 [k11 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{sfr volatile unsigned char}
  -- added _ADCON0 to hash, size = 1
  -- and it is at a fixed address 0x1f
  2695 - result is not temp
  2750 - dic is VOLATILE 
  result:_ADCON0 [k11 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{sfr volatile unsigned char}
  left:
  right:iTemp1 [k4 lr4:9 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sensorSample_receiver_1_1}
  left:_sensorSample [k1 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int function    }
        Symbol type: int function    
  result:iTemp0 [k2 lr3:65 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sensorSample_transmitter_1_1}
          Symbol type: unsigned char
  3594 - pointer reg req = 0
packForReceive
  result:iTemp0 [k2 lr3:65 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sensorSample_transmitter_1_1}
  left:
  right:
  hey we can remove this unnecessary assign
  result:iTemp1 [k4 lr4:9 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sensorSample_receiver_1_1}
          Symbol type: unsigned char
  3594 - pointer reg req = 0
packForReceive
  result:iTemp1 [k4 lr4:9 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sensorSample_receiver_1_1}
  left:
  right:
    used on right
  hey we can remove this unnecessary assign
  result:iTemp2 [k6 lr5:31 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sensorSample_phase_1_1}
          Symbol type: unsigned char
  3594 - pointer reg req = 0
packForReceive
  result:iTemp2 [k6 lr5:31 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sensorSample_phase_1_1}
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
  result:_TRISB [k10 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{sfr volatile unsigned char}
          Symbol type: sfr volatile unsigned char
  3594 - pointer reg req = 0
  right:iTemp1 [k4 lr4:9 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sensorSample_receiver_1_1}
         Symbol type: unsigned char
  result:_ADCON0 [k11 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{sfr volatile unsigned char}
          Symbol type: sfr volatile unsigned char
  3594 - pointer reg req = 0
  left:iTemp2 [k6 lr5:31 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sensorSample_phase_1_1}
        Symbol type: unsigned char
  result:iTemp4 [k12 lr10:11 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{char}
          Symbol type: char
  3594 - pointer reg req = 0
  3611
  left:iTemp4 [k12 lr10:11 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{char}
        Symbol type: char
  3594 - pointer reg req = 0
packRegisters
packRegsForAssign
  result:_sensorSample_phase0_1_1 [k13 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{bit bit {0,1}}
  left:
  right:0x1 {literal bit {0,1}}
allocDirReg symbol name _sensorSample_phase0_1_1
  637  storage class 13 
 642  integral
 646  specifier
_sensorSample_phase0_1_1 [k13 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{bit bit {0,1}}
  -- added _sensorSample_phase0_1_1 to hash, size = 1
  2695 - result is not temp
  2705 - not packing - right is not temp
allocDirReg BAD, op is NULL
  result:_sensorSample_phase0_1_1 [k13 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{bit bit {0,1}}
  left:
  right:0x1 {literal bit {0,1}}
  result:_sensorSample_phase0_1_1 [k13 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{bit bit {0,1}}
          Symbol type: bit bit {0,1}
  3594 - pointer reg req = 0
packRegisters
packRegsForAssign
  result:_sensorSample_phase0_1_1 [k13 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{bit bit {0,1}}
  left:
  right:0x0 {literal bit {0,1}}
allocDirReg symbol name _sensorSample_phase0_1_1
  637  storage class 13 
 642  integral
 646  specifier
_sensorSample_phase0_1_1 [k13 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{bit bit {0,1}}
  2695 - result is not temp
  2705 - not packing - right is not temp
allocDirReg BAD, op is NULL
  result:_sensorSample_phase0_1_1 [k13 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{bit bit {0,1}}
  left:
  right:0x0 {literal bit {0,1}}
  result:_sensorSample_phase0_1_1 [k13 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{bit bit {0,1}}
          Symbol type: bit bit {0,1}
  3594 - pointer reg req = 0
packRegisters
  left:iTemp2 [k6 lr5:31 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sensorSample_phase_1_1}
        Symbol type: unsigned char
  result:iTemp5 [k14 lr17:18 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{char}
          Symbol type: char
  3594 - pointer reg req = 0
  3611
  left:iTemp5 [k14 lr17:18 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{char}
        Symbol type: char
  3594 - pointer reg req = 0
packRegisters
packRegsForAssign
  result:_sensorSample_phase1_1_1 [k15 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{bit bit {0,1}}
  left:
  right:0x1 {literal bit {0,1}}
allocDirReg symbol name _sensorSample_phase1_1_1
  637  storage class 13 
 642  integral
 646  specifier
_sensorSample_phase1_1_1 [k15 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{bit bit {0,1}}
  -- added _sensorSample_phase1_1_1 to hash, size = 1
  2695 - result is not temp
  2705 - not packing - right is not temp
allocDirReg BAD, op is NULL
  result:_sensorSample_phase1_1_1 [k15 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{bit bit {0,1}}
  left:
  right:0x1 {literal bit {0,1}}
  result:_sensorSample_phase1_1_1 [k15 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{bit bit {0,1}}
          Symbol type: bit bit {0,1}
  3594 - pointer reg req = 0
packRegisters
packRegsForAssign
  result:_sensorSample_phase1_1_1 [k15 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{bit bit {0,1}}
  left:
  right:0x0 {literal bit {0,1}}
allocDirReg symbol name _sensorSample_phase1_1_1
  637  storage class 13 
 642  integral
 646  specifier
_sensorSample_phase1_1_1 [k15 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{bit bit {0,1}}
  2695 - result is not temp
  2705 - not packing - right is not temp
allocDirReg BAD, op is NULL
  result:_sensorSample_phase1_1_1 [k15 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{bit bit {0,1}}
  left:
  right:0x0 {literal bit {0,1}}
  result:_sensorSample_phase1_1_1 [k15 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{bit bit {0,1}}
          Symbol type: bit bit {0,1}
  3594 - pointer reg req = 0
packRegisters
  left:iTemp2 [k6 lr5:31 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sensorSample_phase_1_1}
        Symbol type: unsigned char
  result:iTemp6 [k16 lr24:25 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{char}
          Symbol type: char
  3594 - pointer reg req = 0
  3611
  left:iTemp6 [k16 lr24:25 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{char}
        Symbol type: char
  3594 - pointer reg req = 0
packRegisters
packRegsForAssign
  result:_sensorSample_phase2_1_1 [k17 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{bit bit {0,1}}
  left:
  right:0x1 {literal bit {0,1}}
allocDirReg symbol name _sensorSample_phase2_1_1
  637  storage class 13 
 642  integral
 646  specifier
_sensorSample_phase2_1_1 [k17 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{bit bit {0,1}}
  -- added _sensorSample_phase2_1_1 to hash, size = 1
  2695 - result is not temp
  2705 - not packing - right is not temp
allocDirReg BAD, op is NULL
  result:_sensorSample_phase2_1_1 [k17 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{bit bit {0,1}}
  left:
  right:0x1 {literal bit {0,1}}
  result:_sensorSample_phase2_1_1 [k17 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{bit bit {0,1}}
          Symbol type: bit bit {0,1}
  3594 - pointer reg req = 0
packRegisters
packRegsForAssign
  result:_sensorSample_phase2_1_1 [k17 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{bit bit {0,1}}
  left:
  right:0x0 {literal bit {0,1}}
allocDirReg symbol name _sensorSample_phase2_1_1
  637  storage class 13 
 642  integral
 646  specifier
_sensorSample_phase2_1_1 [k17 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{bit bit {0,1}}
  2695 - result is not temp
  2705 - not packing - right is not temp
allocDirReg BAD, op is NULL
  result:_sensorSample_phase2_1_1 [k17 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{bit bit {0,1}}
  left:
  right:0x0 {literal bit {0,1}}
  result:_sensorSample_phase2_1_1 [k17 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{bit bit {0,1}}
          Symbol type: bit bit {0,1}
  3594 - pointer reg req = 0
packRegisters
  left:iTemp2 [k6 lr5:31 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sensorSample_phase_1_1}
        Symbol type: unsigned char
  result:iTemp7 [k18 lr31:32 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{char}
          Symbol type: char
  3594 - pointer reg req = 0
  3611
  left:iTemp7 [k18 lr31:32 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{char}
        Symbol type: char
  3594 - pointer reg req = 0
packRegisters
packRegsForAssign
  result:_sensorSample_phase3_1_1 [k19 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{bit bit {0,1}}
  left:
  right:0x1 {literal bit {0,1}}
allocDirReg symbol name _sensorSample_phase3_1_1
  637  storage class 13 
 642  integral
 646  specifier
_sensorSample_phase3_1_1 [k19 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{bit bit {0,1}}
  -- added _sensorSample_phase3_1_1 to hash, size = 1
  2695 - result is not temp
  2705 - not packing - right is not temp
allocDirReg BAD, op is NULL
  result:_sensorSample_phase3_1_1 [k19 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{bit bit {0,1}}
  left:
  right:0x1 {literal bit {0,1}}
  result:_sensorSample_phase3_1_1 [k19 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{bit bit {0,1}}
          Symbol type: bit bit {0,1}
  3594 - pointer reg req = 0
packRegisters
packRegsForAssign
  result:_sensorSample_phase3_1_1 [k19 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{bit bit {0,1}}
  left:
  right:0x0 {literal bit {0,1}}
allocDirReg symbol name _sensorSample_phase3_1_1
  637  storage class 13 
 642  integral
 646  specifier
_sensorSample_phase3_1_1 [k19 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{bit bit {0,1}}
  2695 - result is not temp
  2705 - not packing - right is not temp
allocDirReg BAD, op is NULL
  result:_sensorSample_phase3_1_1 [k19 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{bit bit {0,1}}
  left:
  right:0x0 {literal bit {0,1}}
  result:_sensorSample_phase3_1_1 [k19 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{bit bit {0,1}}
          Symbol type: bit bit {0,1}
  3594 - pointer reg req = 0
packRegisters
packRegsForAssign
  result:iTemp8 [k20 lr38:40 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sensorSample_halfcycles_1_1}
  left:
  right:_numResonationHalfcycles [k22 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{unsigned char}
  2705 - not packing - right is not temp
allocDirReg symbol name _numResonationHalfcycles
  637  storage class 0 
 642  integral
 646  specifier
_numResonationHalfcycles [k22 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{unsigned char}
  -- added _numResonationHalfcycles to hash, size = 1
  result:iTemp8 [k20 lr38:40 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sensorSample_halfcycles_1_1}
  left:
  right:_numResonationHalfcycles [k22 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{unsigned char}
  right:_numResonationHalfcycles [k22 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{unsigned char}
         Symbol type: unsigned char
  result:iTemp8 [k20 lr38:40 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sensorSample_halfcycles_1_1}
          Symbol type: unsigned char
  3594 - pointer reg req = 0
packRegisters
packRegsForAssign
  result:iTemp10 [k24 lr40:65 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sensorSample_halfcycles_1_1}
  left:
  right:iTemp8 [k20 lr38:40 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sensorSample_halfcycles_1_1}
  result:iTemp10 [k24 lr40:65 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sensorSample_halfcycles_1_1}
  left:
  right:iTemp8 [k20 lr38:40 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sensorSample_halfcycles_1_1}
  right:iTemp8 [k20 lr38:40 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sensorSample_halfcycles_1_1}
         Symbol type: unsigned char
  result:iTemp10 [k24 lr40:65 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sensorSample_halfcycles_1_1}
          Symbol type: unsigned char
  3594 - pointer reg req = 0
packRegisters
packRegsForAssign
  result:_PORTB [k8 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{sfr volatile unsigned char}
  left:
  right:iTemp9 [k23 lr42:43 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{unsigned char}
allocDirReg symbol name _PORTB
  637  storage class 3 
 642  integral
 646  specifier
_PORTB [k8 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{sfr volatile unsigned char}
  -- added _PORTB to hash, size = 1
  -- and it is at a fixed address 0x06
  2695 - result is not temp
  2760 - dic result key == ic right key -- pointer set=N
  packing. removing iTemp9
  replacing with iTemp9
  result:_PORTB [k8 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{sfr volatile unsigned char}
  left:
  right:iTemp9 [k23 lr42:43 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{unsigned char}
  left:_PORTB [k8 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{sfr volatile unsigned char}
    is volatile
sfr         Symbol type: sfr volatile unsigned char
  right:iTemp0 [k2 lr3:65 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sensorSample_transmitter_1_1}
         Symbol type: unsigned char
  result:_PORTB [k8 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{sfr volatile unsigned char}
          Symbol type: sfr volatile unsigned char
  3594 - pointer reg req = 0
isBitwiseOptimizable
  left:iTemp10 [k24 lr40:65 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sensorSample_halfcycles_1_1}
        Symbol type: unsigned char
  result:iTemp10 [k24 lr40:65 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sensorSample_halfcycles_1_1}
          Symbol type: unsigned char
  3594 - pointer reg req = 0
packRegsForSupport
packRegsForAccUse
  packRegsForAccUse:3181
  packRegsForAccUse:3188
  left:iTemp10 [k24 lr40:65 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sensorSample_halfcycles_1_1}
        Symbol type: unsigned char
  3594 - pointer reg req = 0
packRegisters
  left:_sensorSample_phase0_1_1 [k13 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{bit bit {0,1}}
bit         Symbol type: bit bit {0,1}
  3594 - pointer reg req = 0
packRegisters
packRegsForAssign
  result:_GO_DONE [k25 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{bit bit {0,1}}
  left:
  right:0x1 {literal bit {0,1}}
allocDirReg symbol name _GO_DONE
  637  storage class 13 
 642  integral
 646  specifier
_GO_DONE [k25 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{bit bit {0,1}}
  -- added _GO_DONE to hash, size = 1
  -- and it is at a fixed address 0xfa
  2695 - result is not temp
  2705 - not packing - right is not temp
allocDirReg BAD, op is NULL
  result:_GO_DONE [k25 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{bit bit {0,1}}
  left:
  right:0x1 {literal bit {0,1}}
  result:_GO_DONE [k25 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{bit bit {0,1}}
          Symbol type: bit bit {0,1}
  3594 - pointer reg req = 0
packRegisters
  left:_sensorSample_phase1_1_1 [k15 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{bit bit {0,1}}
bit         Symbol type: bit bit {0,1}
  3594 - pointer reg req = 0
packRegisters
packRegsForAssign
  result:_GO_DONE [k25 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{bit bit {0,1}}
  left:
  right:0x1 {literal bit {0,1}}
allocDirReg symbol name _GO_DONE
  637  storage class 13 
 642  integral
 646  specifier
_GO_DONE [k25 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{bit bit {0,1}}
  -- and it is at a fixed address 0xfa
  2695 - result is not temp
  2705 - not packing - right is not temp
allocDirReg BAD, op is NULL
  result:_GO_DONE [k25 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{bit bit {0,1}}
  left:
  right:0x1 {literal bit {0,1}}
  result:_GO_DONE [k25 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{bit bit {0,1}}
          Symbol type: bit bit {0,1}
  3594 - pointer reg req = 0
packRegisters
  left:_sensorSample_phase2_1_1 [k17 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{bit bit {0,1}}
bit         Symbol type: bit bit {0,1}
  3594 - pointer reg req = 0
packRegisters
packRegsForAssign
  result:_GO_DONE [k25 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{bit bit {0,1}}
  left:
  right:0x1 {literal bit {0,1}}
allocDirReg symbol name _GO_DONE
  637  storage class 13 
 642  integral
 646  specifier
_GO_DONE [k25 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{bit bit {0,1}}
  -- and it is at a fixed address 0xfa
  2695 - result is not temp
  2705 - not packing - right is not temp
allocDirReg BAD, op is NULL
  result:_GO_DONE [k25 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{bit bit {0,1}}
  left:
  right:0x1 {literal bit {0,1}}
  result:_GO_DONE [k25 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{bit bit {0,1}}
          Symbol type: bit bit {0,1}
  3594 - pointer reg req = 0
packRegisters
  left:_sensorSample_phase3_1_1 [k19 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{bit bit {0,1}}
bit         Symbol type: bit bit {0,1}
  3594 - pointer reg req = 0
packRegisters
packRegsForAssign
  result:_GO_DONE [k25 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{bit bit {0,1}}
  left:
  right:0x1 {literal bit {0,1}}
allocDirReg symbol name _GO_DONE
  637  storage class 13 
 642  integral
 646  specifier
_GO_DONE [k25 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{bit bit {0,1}}
  -- and it is at a fixed address 0xfa
  2695 - result is not temp
  2705 - not packing - right is not temp
allocDirReg BAD, op is NULL
  result:_GO_DONE [k25 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{bit bit {0,1}}
  left:
  right:0x1 {literal bit {0,1}}
  result:_GO_DONE [k25 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{bit bit {0,1}}
          Symbol type: bit bit {0,1}
  3594 - pointer reg req = 0
packRegisters
packRegisters
  left:_NOT_DONE [k30 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{bit bit {0,1}}
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
  result:iTemp15 [k31 lr112:114 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sensorSample_h_1_1}
  left:
  right:_ADRESH [k33 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{sfr volatile unsigned char}
  2705 - not packing - right is not temp
allocDirReg symbol name _ADRESH
  637  storage class 3 
 642  integral
 646  specifier
_ADRESH [k33 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{sfr volatile unsigned char}
  -- added _ADRESH to hash, size = 1
  -- and it is at a fixed address 0x1e
  result:iTemp15 [k31 lr112:114 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sensorSample_h_1_1}
  left:
  right:_ADRESH [k33 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{sfr volatile unsigned char}
packRegsForAssign
  result:iTemp16 [k34 lr113:116 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sensorSample_l_1_1}
  left:
  right:_ADRESL [k36 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{sfr volatile unsigned char}
  2705 - not packing - right is not temp
allocDirReg symbol name _ADRESL
  637  storage class 3 
 642  integral
 646  specifier
_ADRESL [k36 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{sfr volatile unsigned char}
  -- added _ADRESL to hash, size = 1
  -- and it is at a fixed address 0x9e
  result:iTemp16 [k34 lr113:116 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sensorSample_l_1_1}
  left:
  right:_ADRESL [k36 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{sfr volatile unsigned char}
  result:_TRISB [k10 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{sfr volatile unsigned char}
          Symbol type: sfr volatile unsigned char
  3594 - pointer reg req = 0
  right:_ADRESH [k33 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{sfr volatile unsigned char}
         Symbol type: sfr volatile unsigned char
  result:iTemp15 [k31 lr112:114 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sensorSample_h_1_1}
          Symbol type: unsigned char
  3594 - pointer reg req = 0
  right:_ADRESL [k36 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{sfr volatile unsigned char}
         Symbol type: sfr volatile unsigned char
  result:iTemp16 [k34 lr113:116 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sensorSample_l_1_1}
          Symbol type: unsigned char
  3594 - pointer reg req = 0
  right:iTemp15 [k31 lr112:114 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sensorSample_h_1_1}
         Symbol type: unsigned char
  result:iTemp17 [k37 lr114:115 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int}
          Symbol type: int
  3594 - pointer reg req = 0
  3662 - casting
packRegsForOneuse
  left:iTemp17 [k37 lr114:115 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int}
        Symbol type: int
  result:iTemp18 [k38 lr115:117 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int}
          Symbol type: int
  3594 - pointer reg req = 0
packRegsForAccUse
  packRegsForAccUse:3181
  packRegsForAccUse:3188
  right:iTemp16 [k34 lr113:116 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sensorSample_l_1_1}
         Symbol type: unsigned char
  result:iTemp20 [k40 lr116:117 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{unsigned int}
          Symbol type: unsigned int
  3594 - pointer reg req = 0
  3662 - casting
packRegsForOneuse
  left:iTemp18 [k38 lr115:117 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int}
        Symbol type: int
  right:iTemp20 [k40 lr116:117 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{unsigned int}
         Symbol type: unsigned int
  result:iTemp21 [k41 lr117:118 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{unsigned int}
          Symbol type: unsigned int
  3594 - pointer reg req = 0
isBitwiseOptimizable
packRegsForAccUse
  packRegsForAccUse:3181
  packRegsForAccUse:3188
  left:iTemp21 [k41 lr117:118 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{unsigned int}
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
  -- #75 reg = r0x63  key 0, rIdx = 99, size 1
  -- #76 reg = r0x64  key 0, rIdx = 100, size 1
  -- #77 reg = _ADRESH  key 0, rIdx = 36, size 1
  -- #78 reg = _ADRESL  key 5, rIdx = 37, size 1
  -- #79 reg = _PCLATH  key 6, rIdx = 10, size 1
  -- #80 reg = _TRISA  key 7, rIdx = 1, size 1
  -- #81 reg = _TRISB  key 8, rIdx = 2, size 1
  -- #82 reg = _TRISB  key 8, rIdx = 24, size 1
  -- #83 reg = _TRISB  key 8, rIdx = 26, size 1
  -- #84 reg = _TRISB  key 8, rIdx = 35, size 1
  -- #85 reg = _TRISC  key 9, rIdx = 3, size 1
  -- #86 reg = _TRISD  key 10, rIdx = 4, size 1
  -- #87 reg = _PORTA  key 10, rIdx = 6, size 1
  -- #88 reg = _TRISE  key 11, rIdx = 5, size 1
  -- #89 reg = _PORTB  key 11, rIdx = 7, size 1
  -- #90 reg = _PORTB  key 11, rIdx = 23, size 1
  -- #91 reg = _PORTB  key 11, rIdx = 33, size 1
  -- #92 reg = _PORTC  key 12, rIdx = 8, size 1
  -- #93 reg = _GIE  key 12, rIdx = 13, size 1
  -- #94 reg = _PORTD  key 13, rIdx = 9, size 1
  -- #95 reg = _PORTE  key 14, rIdx = 10, size 1
  -- #96 reg = _numResonationHalfcycles  key 14, rIdx = 32, size 1
  -- #97 reg = _sensorSample_phase0_1_1  key 15, rIdx = 28, size 1
  -- #98 reg = _sensorSample_phase1_1_1  key 16, rIdx = 29, size 1
  -- #99 reg = _sensorSample_phase2_1_1  key 17, rIdx = 30, size 1
  -- #100 reg = _sensorSample_phase3_1_1  key 18, rIdx = 31, size 1
  -- #101 reg = _ADCON0  key 27, rIdx = 12, size 1
  -- #102 reg = _ADCON0  key 27, rIdx = 25, size 1
  -- #103 reg = _ADCON0  key 27, rIdx = 27, size 1
  -- #104 reg = _ADCON1  key 28, rIdx = 11, size 1
  -- #105 reg = SSAVE  key 32, rIdx = 32765, size 1
  -- #106 reg = WSAVE  key 36, rIdx = 32766, size 1
  -- #107 reg = _reading0  key 44, rIdx = 14, size 2
  -- #108 reg = _GO_DONE  key 44, rIdx = 34, size 1
  -- #109 reg = _reading1  key 45, rIdx = 15, size 2
  -- #110 reg = _reading2  key 46, rIdx = 16, size 2
  -- #111 reg = INTCON  key 47, rIdx = 11, size 1
  -- #112 reg = _reading3  key 47, rIdx = 17, size 2
  -- #113 reg = _STATUS  key 48, rIdx = 3, size 1
  -- #114 reg = PCL  key 48, rIdx = 2, size 1
  -- #115 reg = _reading4  key 48, rIdx = 18, size 2
  -- #116 reg = KZ  key 49, rIdx = 32767, size 1
  -- #117 reg = _reading5  key 49, rIdx = 19, size 2
  -- #118 reg = _reading6  key 50, rIdx = 20, size 2
  -- #119 reg = _reading7  key 51, rIdx = 21, size 2
  -- #120 reg = INDF  key 56, rIdx = 0, size 1
  -- #121 reg = _Light0  key 56, rIdx = 0, size 1
  -- #122 reg = _numIntegrationSamples  key 56, rIdx = 22, size 1
  -- #123 reg = FSR  key 60, rIdx = 4, size 1
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
  2421 - iTemp5
  2431 - itemp register
  2421 - iTemp6
  2431 - itemp register
  2421 - iTemp7
  2431 - itemp register
  2421 - iTemp8
  2431 - itemp register
  reg name iTemp8,  reg type REG_GPR
  2421 - iTemp9
  2431 - itemp register
  reg name iTemp9,  reg type REG_GPR
  2421 - iTemp10
  2431 - itemp register
  reg name iTemp10,  reg type REG_GPR
  2421 - iTemp15
  2431 - itemp register
  reg name iTemp15,  reg type REG_GPR
  2421 - iTemp16
  2431 - itemp register
  reg name iTemp16,  reg type REG_GPR
  2421 - iTemp17
  2431 - itemp register
  reg name iTemp17,  reg type REG_GPR
  2421 - iTemp18
  2431 - itemp register
  reg name iTemp18,  reg type REG_GPR
  2421 - iTemp20
  2431 - itemp register
  reg name iTemp20,  reg type REG_GPR
  2421 - iTemp21
  2431 - itemp register
  reg name iTemp21,  reg type REG_GPR
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
bitvector Size = 59 bSize = 8
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
bitvector Size = 59 bSize = 8
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
bitvector Size = 59 bSize = 8
Bits on { (2) (4) (6) }
getRegGpr
allocReg of type REG_GPR
  2141 - 
  2155 - 
  op: BITWISEAND
deassignLRs
  op: =
deassignLRs
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
  op: =
deassignLRs
willCauseSpill
computeSpillable
  2125 - 
bitvector Size = 59 bSize = 8
Bits on { (2) (20) }
getRegGpr
allocReg of type REG_GPR
  2141 - 
  2155 - 
  op: LABEL
deassignLRs
  op: =
deassignLRs
freeReg
willCauseSpill
computeSpillable
  2125 - 
bitvector Size = 59 bSize = 8
Bits on { (2) (24) }
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
bitvector Size = 59 bSize = 8
Bits on { (31) }
getRegGpr
allocReg of type REG_GPR
  2141 - 
  2155 - 
  op: =
deassignLRs
willCauseSpill
computeSpillable
  2125 - 
bitvector Size = 59 bSize = 8
Bits on { (31) (34) }
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
depth 1st num 1 : bbnum = 0 1st iCode = 1 , last iCode = 11
visited 0 : hasFcall = 0

defines bitVector :bitvector Size = 151 bSize = 19
Bits on { (2) (3) (4) (5) (6) (7) (8) (9) }

local defines bitVector :bitvector Size = 151 bSize = 19
Bits on { (2) (3) (4) (5) (6) (7) (8) (9) }

pointers Set bitvector :
in pointers Set bitvector :
inDefs Set bitvector :
outDefs Set bitvector :bitvector Size = 151 bSize = 19
Bits on { (2) (3) (4) (5) (6) (7) (8) (9) }

usesDefs Set bitvector :bitvector Size = 151 bSize = 19
Bits on { (2) (3) (4) (5) (9) }

----------------------------------------------------------------
main.c(182:1:0:0:0)	 _entry($28) :
main.c(182:2:1:0:0)		proc _sensorSample [k1 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int function    }
main.c(182:3:2:0:0)	iTemp0 [k2 lr3:65 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sensorSample_transmitter_1_1}[r0x65 ] = recv 
main.c(182:4:3:0:0)	iTemp1 [k4 lr4:9 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sensorSample_receiver_1_1}[r0x66 ] = recv 
main.c(182:5:4:0:0)	iTemp2 [k6 lr5:31 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sensorSample_phase_1_1}[r0x67 ] = recv 
main.c(188:6:5:0:0)		_PORTB [k8 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{sfr volatile unsigned char} = _PORTB [k8 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{sfr volatile unsigned char} & 0xf {literal unsigned char}
main.c(189:8:7:0:0)		_TRISB [k10 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{sfr volatile unsigned char} := 0x0 {literal unsigned char}
main.c(192:9:8:0:0)		_ADCON0 [k11 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{sfr volatile unsigned char} := iTemp1 [k4 lr4:9 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sensorSample_receiver_1_1}[r0x66 ]
main.c(195:10:9:0:0)		iTemp4 [k12 lr10:11 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{char} = iTemp2 [k6 lr5:31 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sensorSample_phase_1_1}[r0x67 ] == 0x0 {literal unsigned char}
main.c(195:11:10:0:0)		if iTemp4 [k12 lr10:11 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{char} == 0 goto _iffalse_2($2)
main.c(195:12:13:0:0)		_sensorSample_phase0_1_1 [k13 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{bit bit {0,1}} := 0x1 {literal bit {0,1}}
main.c(195:13:14:0:0)		 goto _ifend_2($3)
main.c(195:14:15:0:0)	 _iffalse_2($2) :
main.c(195:15:16:0:0)		_sensorSample_phase0_1_1 [k13 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{bit bit {0,1}} := 0x0 {literal bit {0,1}}
main.c(195:16:17:0:0)	 _ifend_2($3) :
main.c(196:17:18:0:0)		iTemp5 [k14 lr17:18 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{char} = iTemp2 [k6 lr5:31 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sensorSample_phase_1_1}[r0x67 ] == 0x1 {literal unsigned char}
main.c(196:18:19:0:0)		if iTemp5 [k14 lr17:18 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{char} == 0 goto _iffalse_3($5)
main.c(196:19:22:0:0)		_sensorSample_phase1_1_1 [k15 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{bit bit {0,1}} := 0x1 {literal bit {0,1}}
main.c(196:20:23:0:0)		 goto _ifend_3($6)
main.c(196:21:24:0:0)	 _iffalse_3($5) :
main.c(196:22:25:0:0)		_sensorSample_phase1_1_1 [k15 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{bit bit {0,1}} := 0x0 {literal bit {0,1}}
main.c(196:23:26:0:0)	 _ifend_3($6) :
main.c(197:24:27:0:0)		iTemp6 [k16 lr24:25 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{char} = iTemp2 [k6 lr5:31 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sensorSample_phase_1_1}[r0x67 ] == 0x2 {literal unsigned char}
main.c(197:25:28:0:0)		if iTemp6 [k16 lr24:25 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{char} == 0 goto _iffalse_4($8)
main.c(197:26:31:0:0)		_sensorSample_phase2_1_1 [k17 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{bit bit {0,1}} := 0x1 {literal bit {0,1}}
main.c(197:27:32:0:0)		 goto _ifend_4($9)
main.c(197:28:33:0:0)	 _iffalse_4($8) :
main.c(197:29:34:0:0)		_sensorSample_phase2_1_1 [k17 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{bit bit {0,1}} := 0x0 {literal bit {0,1}}
main.c(197:30:35:0:0)	 _ifend_4($9) :
main.c(198:31:36:0:0)		iTemp7 [k18 lr31:32 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{char} = iTemp2 [k6 lr5:31 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sensorSample_phase_1_1}[r0x67 ] == 0x3 {literal unsigned char}
main.c(198:32:37:0:0)		if iTemp7 [k18 lr31:32 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{char} == 0 goto _iffalse_5($11)
main.c(198:33:40:0:0)		_sensorSample_phase3_1_1 [k19 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{bit bit {0,1}} := 0x1 {literal bit {0,1}}
main.c(198:34:41:0:0)		 goto _ifend_5($12)
main.c(198:35:42:0:0)	 _iffalse_5($11) :
main.c(198:36:43:0:0)		_sensorSample_phase3_1_1 [k19 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{bit bit {0,1}} := 0x0 {literal bit {0,1}}
main.c(198:37:44:0:0)	 _ifend_5($12) :
main.c(204:38:45:0:0)		iTemp8 [k20 lr38:40 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sensorSample_halfcycles_1_1}[r0x68 ] := _numResonationHalfcycles [k22 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{unsigned char}
main.c(205:40:150:0:0)		iTemp10 [k24 lr40:65 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sensorSample_halfcycles_1_1}[r0x69 ] := iTemp8 [k20 lr38:40 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sensorSample_halfcycles_1_1}[r0x68 ]
main.c(205:41:46:1:0)	 _dobody_1($13) :
main.c(206:42:47:1:0)		_PORTB [k8 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{sfr volatile unsigned char} = _PORTB [k8 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{sfr volatile unsigned char} ^ iTemp0 [k2 lr3:65 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sensorSample_transmitter_1_1}[r0x65 ]
main.c(207:44:49:1:0)	 nop main.c(207:45:50:1:0)	 nop main.c(207:46:51:1:0)	 nop main.c(208:47:52:1:0)	 nop main.c(208:48:53:1:0)	 nop main.c(208:49:54:1:0)	 nop main.c(208:50:55:1:0)	 nop main.c(208:51:56:1:0)	 nop main.c(209:52:57:1:0)	 nop main.c(209:53:58:1:0)	 nop main.c(209:54:59:1:0)	 nop main.c(209:55:60:1:0)	 nop main.c(209:56:61:1:0)	 nop main.c(210:57:62:1:0)	 nop main.c(210:58:63:1:0)	 nop main.c(210:59:64:1:0)	 nop main.c(210:60:65:1:0)	 nop main.c(210:61:66:1:0)	 nop main.c(211:62:67:1:0)	 nop main.c(211:63:68:1:0)	 nop main.c(212:64:70:1:0)		iTemp10 [k24 lr40:65 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sensorSample_halfcycles_1_1}[r0x69 ] = iTemp10 [k24 lr40:65 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sensorSample_halfcycles_1_1}[r0x69 ] - 0x1 {literal unsigned char}
main.c(212:65:72:1:0)		if iTemp10 [k24 lr40:65 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sensorSample_halfcycles_1_1}[r0x69 ] != 0 goto _dobody_1($13)
main.c(215:66:75:0:0)		if _sensorSample_phase0_1_1 [k13 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{bit bit {0,1}} == 0 goto _iffalse_6($17)
main.c(216:67:79:0:0)		_GO_DONE [k25 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{bit bit {0,1}} := 0x1 {literal bit {0,1}}
main.c(216:68:80:0:0)	 _iffalse_6($17) :
main.c(217:69:81:0:0)	 nop main.c(217:70:82:0:0)	 nop main.c(217:71:83:0:0)	 nop main.c(217:72:84:0:0)	 nop main.c(217:73:85:0:0)	 nop main.c(218:74:86:0:0)	 nop main.c(218:75:87:0:0)	 nop main.c(218:76:88:0:0)	 nop main.c(218:77:89:0:0)	 nop main.c(218:78:90:0:0)	 nop main.c(219:79:91:0:0)		if _sensorSample_phase1_1_1 [k15 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{bit bit {0,1}} == 0 goto _iffalse_7($19)
main.c(220:80:95:0:0)		_GO_DONE [k25 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{bit bit {0,1}} := 0x1 {literal bit {0,1}}
main.c(220:81:96:0:0)	 _iffalse_7($19) :
main.c(221:82:97:0:0)	 nop main.c(221:83:98:0:0)	 nop main.c(221:84:99:0:0)	 nop main.c(221:85:100:0:0)	 nop main.c(221:86:101:0:0)	 nop main.c(222:87:102:0:0)	 nop main.c(222:88:103:0:0)	 nop main.c(222:89:104:0:0)	 nop main.c(222:90:105:0:0)	 nop main.c(222:91:106:0:0)	 nop main.c(222:92:107:0:0)	 nop main.c(223:93:108:0:0)		if _sensorSample_phase2_1_1 [k17 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{bit bit {0,1}} == 0 goto _iffalse_8($21)
main.c(224:94:112:0:0)		_GO_DONE [k25 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{bit bit {0,1}} := 0x1 {literal bit {0,1}}
main.c(224:95:113:0:0)	 _iffalse_8($21) :
main.c(225:96:114:0:0)	 nop main.c(225:97:115:0:0)	 nop main.c(225:98:116:0:0)	 nop main.c(225:99:117:0:0)	 nop main.c(225:100:118:0:0)	 nop main.c(226:101:119:0:0)	 nop main.c(226:102:120:0:0)	 nop main.c(226:103:121:0:0)	 nop main.c(226:104:122:0:0)	 nop main.c(226:105:123:0:0)	 nop main.c(227:106:124:0:0)		if _sensorSample_phase3_1_1 [k19 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{bit bit {0,1}} == 0 goto _whilecontinue_1($24)
main.c(228:107:128:0:0)		_GO_DONE [k25 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{bit bit {0,1}} := 0x1 {literal bit {0,1}}
main.c(231:109:130:1:0)	 _whilecontinue_1($24) :
main.c(231:110:131:1:0)		if _NOT_DONE [k30 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{bit bit {0,1}} != 0 goto _whilecontinue_1($24)
main.c(234:111:136:0:0)		_TRISB [k10 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{sfr volatile unsigned char} := 0xf0 {literal unsigned char}
main.c(236:112:137:0:0)		iTemp15 [k31 lr112:114 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sensorSample_h_1_1}[r0x6A ] := _ADRESH [k33 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{sfr volatile unsigned char}
main.c(237:113:138:0:0)		iTemp16 [k34 lr113:116 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sensorSample_l_1_1}[r0x6B ] := _ADRESL [k36 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{sfr volatile unsigned char}
main.c(238:114:139:0:0)		iTemp17 [k37 lr114:115 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int}[r0x6A r0x6C ] = (int)iTemp15 [k31 lr112:114 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sensorSample_h_1_1}[r0x6A ]
main.c(238:115:140:0:0)		iTemp18 [k38 lr115:117 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int}[r0x6A r0x6C ] = iTemp17 [k37 lr114:115 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int}[r0x6A r0x6C ] << 0x8 {literal unsigned char}
main.c(238:116:142:0:0)		iTemp20 [k40 lr116:117 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{unsigned int}[r0x6B r0x6D ] = (unsigned int)iTemp16 [k34 lr113:116 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sensorSample_l_1_1}[r0x6B ]
main.c(238:117:143:0:0)		iTemp21 [k41 lr117:118 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{unsigned int}[r0x6A r0x6C ] = iTemp18 [k38 lr115:117 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int}[r0x6A r0x6C ] | iTemp20 [k40 lr116:117 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{unsigned int}[r0x6B r0x6D ]
main.c(238:118:145:0:0)		ret iTemp21 [k41 lr117:118 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{unsigned int}[r0x6A r0x6C ]
main.c(238:119:146:0:0)	 _return($27) :
main.c(238:120:147:0:0)		eproc _sensorSample [k1 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int function    }

----------------------------------------------------------------
Basic Block _eBBlock0 : loop Depth = 0 noPath = 0 , lastinLoop = 0
depth 1st num 2 : bbnum = 1 1st iCode = 12 , last iCode = 13
visited 0 : hasFcall = 0

defines bitVector :bitvector Size = 151 bSize = 19
Bits on { (13) }

local defines bitVector :bitvector Size = 151 bSize = 19
Bits on { (13) }

pointers Set bitvector :
in pointers Set bitvector :
inDefs Set bitvector :bitvector Size = 151 bSize = 19
Bits on { (2) (3) (4) (5) (6) (7) (8) (9) }

outDefs Set bitvector :bitvector Size = 151 bSize = 19
Bits on { (2) (3) (4) (5) (6) (7) (8) (9) (13) }

usesDefs Set bitvector :bitvector Size = 151 bSize = 19
Bits on { }

----------------------------------------------------------------
main.c(195:12:13:0:0)		_sensorSample_phase0_1_1 [k13 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{bit bit {0,1}} := 0x1 {literal bit {0,1}}
main.c(195:13:14:0:0)		 goto _ifend_2($3)
main.c(195:14:15:0:0)	 _iffalse_2($2) :
main.c(195:15:16:0:0)		_sensorSample_phase0_1_1 [k13 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{bit bit {0,1}} := 0x0 {literal bit {0,1}}
main.c(195:16:17:0:0)	 _ifend_2($3) :
main.c(196:17:18:0:0)		iTemp5 [k14 lr17:18 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{char} = iTemp2 [k6 lr5:31 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sensorSample_phase_1_1}[r0x67 ] == 0x1 {literal unsigned char}
main.c(196:18:19:0:0)		if iTemp5 [k14 lr17:18 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{char} == 0 goto _iffalse_3($5)
main.c(196:19:22:0:0)		_sensorSample_phase1_1_1 [k15 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{bit bit {0,1}} := 0x1 {literal bit {0,1}}
main.c(196:20:23:0:0)		 goto _ifend_3($6)
main.c(196:21:24:0:0)	 _iffalse_3($5) :
main.c(196:22:25:0:0)		_sensorSample_phase1_1_1 [k15 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{bit bit {0,1}} := 0x0 {literal bit {0,1}}
main.c(196:23:26:0:0)	 _ifend_3($6) :
main.c(197:24:27:0:0)		iTemp6 [k16 lr24:25 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{char} = iTemp2 [k6 lr5:31 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sensorSample_phase_1_1}[r0x67 ] == 0x2 {literal unsigned char}
main.c(197:25:28:0:0)		if iTemp6 [k16 lr24:25 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{char} == 0 goto _iffalse_4($8)
main.c(197:26:31:0:0)		_sensorSample_phase2_1_1 [k17 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{bit bit {0,1}} := 0x1 {literal bit {0,1}}
main.c(197:27:32:0:0)		 goto _ifend_4($9)
main.c(197:28:33:0:0)	 _iffalse_4($8) :
main.c(197:29:34:0:0)		_sensorSample_phase2_1_1 [k17 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{bit bit {0,1}} := 0x0 {literal bit {0,1}}
main.c(197:30:35:0:0)	 _ifend_4($9) :
main.c(198:31:36:0:0)		iTemp7 [k18 lr31:32 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{char} = iTemp2 [k6 lr5:31 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sensorSample_phase_1_1}[r0x67 ] == 0x3 {literal unsigned char}
main.c(198:32:37:0:0)		if iTemp7 [k18 lr31:32 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{char} == 0 goto _iffalse_5($11)
main.c(198:33:40:0:0)		_sensorSample_phase3_1_1 [k19 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{bit bit {0,1}} := 0x1 {literal bit {0,1}}
main.c(198:34:41:0:0)		 goto _ifend_5($12)
main.c(198:35:42:0:0)	 _iffalse_5($11) :
main.c(198:36:43:0:0)		_sensorSample_phase3_1_1 [k19 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{bit bit {0,1}} := 0x0 {literal bit {0,1}}
main.c(198:37:44:0:0)	 _ifend_5($12) :
main.c(204:38:45:0:0)		iTemp8 [k20 lr38:40 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sensorSample_halfcycles_1_1}[r0x68 ] := _numResonationHalfcycles [k22 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{unsigned char}
main.c(205:40:150:0:0)		iTemp10 [k24 lr40:65 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sensorSample_halfcycles_1_1}[r0x69 ] := iTemp8 [k20 lr38:40 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sensorSample_halfcycles_1_1}[r0x68 ]
main.c(205:41:46:1:0)	 _dobody_1($13) :
main.c(206:42:47:1:0)		_PORTB [k8 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{sfr volatile unsigned char} = _PORTB [k8 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{sfr volatile unsigned char} ^ iTemp0 [k2 lr3:65 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sensorSample_transmitter_1_1}[r0x65 ]
main.c(207:44:49:1:0)	 nop main.c(207:45:50:1:0)	 nop main.c(207:46:51:1:0)	 nop main.c(208:47:52:1:0)	 nop main.c(208:48:53:1:0)	 nop main.c(208:49:54:1:0)	 nop main.c(208:50:55:1:0)	 nop main.c(208:51:56:1:0)	 nop main.c(209:52:57:1:0)	 nop main.c(209:53:58:1:0)	 nop main.c(209:54:59:1:0)	 nop main.c(209:55:60:1:0)	 nop main.c(209:56:61:1:0)	 nop main.c(210:57:62:1:0)	 nop main.c(210:58:63:1:0)	 nop main.c(210:59:64:1:0)	 nop main.c(210:60:65:1:0)	 nop main.c(210:61:66:1:0)	 nop main.c(211:62:67:1:0)	 nop main.c(211:63:68:1:0)	 nop main.c(212:64:70:1:0)		iTemp10 [k24 lr40:65 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sensorSample_halfcycles_1_1}[r0x69 ] = iTemp10 [k24 lr40:65 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sensorSample_halfcycles_1_1}[r0x69 ] - 0x1 {literal unsigned char}
main.c(212:65:72:1:0)		if iTemp10 [k24 lr40:65 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sensorSample_halfcycles_1_1}[r0x69 ] != 0 goto _dobody_1($13)
main.c(215:66:75:0:0)		if _sensorSample_phase0_1_1 [k13 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{bit bit {0,1}} == 0 goto _iffalse_6($17)
main.c(216:67:79:0:0)		_GO_DONE [k25 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{bit bit {0,1}} := 0x1 {literal bit {0,1}}
main.c(216:68:80:0:0)	 _iffalse_6($17) :
main.c(217:69:81:0:0)	 nop main.c(217:70:82:0:0)	 nop main.c(217:71:83:0:0)	 nop main.c(217:72:84:0:0)	 nop main.c(217:73:85:0:0)	 nop main.c(218:74:86:0:0)	 nop main.c(218:75:87:0:0)	 nop main.c(218:76:88:0:0)	 nop main.c(218:77:89:0:0)	 nop main.c(218:78:90:0:0)	 nop main.c(219:79:91:0:0)		if _sensorSample_phase1_1_1 [k15 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{bit bit {0,1}} == 0 goto _iffalse_7($19)
main.c(220:80:95:0:0)		_GO_DONE [k25 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{bit bit {0,1}} := 0x1 {literal bit {0,1}}
main.c(220:81:96:0:0)	 _iffalse_7($19) :
main.c(221:82:97:0:0)	 nop main.c(221:83:98:0:0)	 nop main.c(221:84:99:0:0)	 nop main.c(221:85:100:0:0)	 nop main.c(221:86:101:0:0)	 nop main.c(222:87:102:0:0)	 nop main.c(222:88:103:0:0)	 nop main.c(222:89:104:0:0)	 nop main.c(222:90:105:0:0)	 nop main.c(222:91:106:0:0)	 nop main.c(222:92:107:0:0)	 nop main.c(223:93:108:0:0)		if _sensorSample_phase2_1_1 [k17 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{bit bit {0,1}} == 0 goto _iffalse_8($21)
main.c(224:94:112:0:0)		_GO_DONE [k25 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{bit bit {0,1}} := 0x1 {literal bit {0,1}}
main.c(224:95:113:0:0)	 _iffalse_8($21) :
main.c(225:96:114:0:0)	 nop main.c(225:97:115:0:0)	 nop main.c(225:98:116:0:0)	 nop main.c(225:99:117:0:0)	 nop main.c(225:100:118:0:0)	 nop main.c(226:101:119:0:0)	 nop main.c(226:102:120:0:0)	 nop main.c(226:103:121:0:0)	 nop main.c(226:104:122:0:0)	 nop main.c(226:105:123:0:0)	 nop main.c(227:106:124:0:0)		if _sensorSample_phase3_1_1 [k19 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{bit bit {0,1}} == 0 goto _whilecontinue_1($24)
main.c(228:107:128:0:0)		_GO_DONE [k25 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{bit bit {0,1}} := 0x1 {literal bit {0,1}}
main.c(231:109:130:1:0)	 _whilecontinue_1($24) :
main.c(231:110:131:1:0)		if _NOT_DONE [k30 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{bit bit {0,1}} != 0 goto _whilecontinue_1($24)
main.c(234:111:136:0:0)		_TRISB [k10 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{sfr volatile unsigned char} := 0xf0 {literal unsigned char}
main.c(236:112:137:0:0)		iTemp15 [k31 lr112:114 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sensorSample_h_1_1}[r0x6A ] := _ADRESH [k33 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{sfr volatile unsigned char}
main.c(237:113:138:0:0)		iTemp16 [k34 lr113:116 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sensorSample_l_1_1}[r0x6B ] := _ADRESL [k36 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{sfr volatile unsigned char}
main.c(238:114:139:0:0)		iTemp17 [k37 lr114:115 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int}[r0x6A r0x6C ] = (int)iTemp15 [k31 lr112:114 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sensorSample_h_1_1}[r0x6A ]
main.c(238:115:140:0:0)		iTemp18 [k38 lr115:117 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int}[r0x6A r0x6C ] = iTemp17 [k37 lr114:115 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int}[r0x6A r0x6C ] << 0x8 {literal unsigned char}
main.c(238:116:142:0:0)		iTemp20 [k40 lr116:117 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{unsigned int}[r0x6B r0x6D ] = (unsigned int)iTemp16 [k34 lr113:116 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sensorSample_l_1_1}[r0x6B ]
main.c(238:117:143:0:0)		iTemp21 [k41 lr117:118 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{unsigned int}[r0x6A r0x6C ] = iTemp18 [k38 lr115:117 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int}[r0x6A r0x6C ] | iTemp20 [k40 lr116:117 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{unsigned int}[r0x6B r0x6D ]
main.c(238:118:145:0:0)		ret iTemp21 [k41 lr117:118 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{unsigned int}[r0x6A r0x6C ]
main.c(238:119:146:0:0)	 _return($27) :
main.c(238:120:147:0:0)		eproc _sensorSample [k1 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int function    }

----------------------------------------------------------------
Basic Block _iffalse_2 : loop Depth = 0 noPath = 0 , lastinLoop = 0
depth 1st num 3 : bbnum = 2 1st iCode = 14 , last iCode = 15
visited 0 : hasFcall = 0

defines bitVector :bitvector Size = 151 bSize = 19
Bits on { (16) }

local defines bitVector :bitvector Size = 151 bSize = 19
Bits on { (16) }

pointers Set bitvector :
in pointers Set bitvector :
inDefs Set bitvector :bitvector Size = 151 bSize = 19
Bits on { (2) (3) (4) (5) (6) (7) (8) (9) }

outDefs Set bitvector :bitvector Size = 151 bSize = 19
Bits on { (2) (3) (4) (5) (6) (7) (8) (9) (16) }

usesDefs Set bitvector :bitvector Size = 151 bSize = 19
Bits on { }

----------------------------------------------------------------
main.c(195:14:15:0:0)	 _iffalse_2($2) :
main.c(195:15:16:0:0)		_sensorSample_phase0_1_1 [k13 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{bit bit {0,1}} := 0x0 {literal bit {0,1}}
main.c(195:16:17:0:0)	 _ifend_2($3) :
main.c(196:17:18:0:0)		iTemp5 [k14 lr17:18 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{char} = iTemp2 [k6 lr5:31 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sensorSample_phase_1_1}[r0x67 ] == 0x1 {literal unsigned char}
main.c(196:18:19:0:0)		if iTemp5 [k14 lr17:18 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{char} == 0 goto _iffalse_3($5)
main.c(196:19:22:0:0)		_sensorSample_phase1_1_1 [k15 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{bit bit {0,1}} := 0x1 {literal bit {0,1}}
main.c(196:20:23:0:0)		 goto _ifend_3($6)
main.c(196:21:24:0:0)	 _iffalse_3($5) :
main.c(196:22:25:0:0)		_sensorSample_phase1_1_1 [k15 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{bit bit {0,1}} := 0x0 {literal bit {0,1}}
main.c(196:23:26:0:0)	 _ifend_3($6) :
main.c(197:24:27:0:0)		iTemp6 [k16 lr24:25 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{char} = iTemp2 [k6 lr5:31 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sensorSample_phase_1_1}[r0x67 ] == 0x2 {literal unsigned char}
main.c(197:25:28:0:0)		if iTemp6 [k16 lr24:25 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{char} == 0 goto _iffalse_4($8)
main.c(197:26:31:0:0)		_sensorSample_phase2_1_1 [k17 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{bit bit {0,1}} := 0x1 {literal bit {0,1}}
main.c(197:27:32:0:0)		 goto _ifend_4($9)
main.c(197:28:33:0:0)	 _iffalse_4($8) :
main.c(197:29:34:0:0)		_sensorSample_phase2_1_1 [k17 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{bit bit {0,1}} := 0x0 {literal bit {0,1}}
main.c(197:30:35:0:0)	 _ifend_4($9) :
main.c(198:31:36:0:0)		iTemp7 [k18 lr31:32 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{char} = iTemp2 [k6 lr5:31 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sensorSample_phase_1_1}[r0x67 ] == 0x3 {literal unsigned char}
main.c(198:32:37:0:0)		if iTemp7 [k18 lr31:32 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{char} == 0 goto _iffalse_5($11)
main.c(198:33:40:0:0)		_sensorSample_phase3_1_1 [k19 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{bit bit {0,1}} := 0x1 {literal bit {0,1}}
main.c(198:34:41:0:0)		 goto _ifend_5($12)
main.c(198:35:42:0:0)	 _iffalse_5($11) :
main.c(198:36:43:0:0)		_sensorSample_phase3_1_1 [k19 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{bit bit {0,1}} := 0x0 {literal bit {0,1}}
main.c(198:37:44:0:0)	 _ifend_5($12) :
main.c(204:38:45:0:0)		iTemp8 [k20 lr38:40 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sensorSample_halfcycles_1_1}[r0x68 ] := _numResonationHalfcycles [k22 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{unsigned char}
main.c(205:40:150:0:0)		iTemp10 [k24 lr40:65 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sensorSample_halfcycles_1_1}[r0x69 ] := iTemp8 [k20 lr38:40 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sensorSample_halfcycles_1_1}[r0x68 ]
main.c(205:41:46:1:0)	 _dobody_1($13) :
main.c(206:42:47:1:0)		_PORTB [k8 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{sfr volatile unsigned char} = _PORTB [k8 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{sfr volatile unsigned char} ^ iTemp0 [k2 lr3:65 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sensorSample_transmitter_1_1}[r0x65 ]
main.c(207:44:49:1:0)	 nop main.c(207:45:50:1:0)	 nop main.c(207:46:51:1:0)	 nop main.c(208:47:52:1:0)	 nop main.c(208:48:53:1:0)	 nop main.c(208:49:54:1:0)	 nop main.c(208:50:55:1:0)	 nop main.c(208:51:56:1:0)	 nop main.c(209:52:57:1:0)	 nop main.c(209:53:58:1:0)	 nop main.c(209:54:59:1:0)	 nop main.c(209:55:60:1:0)	 nop main.c(209:56:61:1:0)	 nop main.c(210:57:62:1:0)	 nop main.c(210:58:63:1:0)	 nop main.c(210:59:64:1:0)	 nop main.c(210:60:65:1:0)	 nop main.c(210:61:66:1:0)	 nop main.c(211:62:67:1:0)	 nop main.c(211:63:68:1:0)	 nop main.c(212:64:70:1:0)		iTemp10 [k24 lr40:65 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sensorSample_halfcycles_1_1}[r0x69 ] = iTemp10 [k24 lr40:65 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sensorSample_halfcycles_1_1}[r0x69 ] - 0x1 {literal unsigned char}
main.c(212:65:72:1:0)		if iTemp10 [k24 lr40:65 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sensorSample_halfcycles_1_1}[r0x69 ] != 0 goto _dobody_1($13)
main.c(215:66:75:0:0)		if _sensorSample_phase0_1_1 [k13 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{bit bit {0,1}} == 0 goto _iffalse_6($17)
main.c(216:67:79:0:0)		_GO_DONE [k25 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{bit bit {0,1}} := 0x1 {literal bit {0,1}}
main.c(216:68:80:0:0)	 _iffalse_6($17) :
main.c(217:69:81:0:0)	 nop main.c(217:70:82:0:0)	 nop main.c(217:71:83:0:0)	 nop main.c(217:72:84:0:0)	 nop main.c(217:73:85:0:0)	 nop main.c(218:74:86:0:0)	 nop main.c(218:75:87:0:0)	 nop main.c(218:76:88:0:0)	 nop main.c(218:77:89:0:0)	 nop main.c(218:78:90:0:0)	 nop main.c(219:79:91:0:0)		if _sensorSample_phase1_1_1 [k15 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{bit bit {0,1}} == 0 goto _iffalse_7($19)
main.c(220:80:95:0:0)		_GO_DONE [k25 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{bit bit {0,1}} := 0x1 {literal bit {0,1}}
main.c(220:81:96:0:0)	 _iffalse_7($19) :
main.c(221:82:97:0:0)	 nop main.c(221:83:98:0:0)	 nop main.c(221:84:99:0:0)	 nop main.c(221:85:100:0:0)	 nop main.c(221:86:101:0:0)	 nop main.c(222:87:102:0:0)	 nop main.c(222:88:103:0:0)	 nop main.c(222:89:104:0:0)	 nop main.c(222:90:105:0:0)	 nop main.c(222:91:106:0:0)	 nop main.c(222:92:107:0:0)	 nop main.c(223:93:108:0:0)		if _sensorSample_phase2_1_1 [k17 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{bit bit {0,1}} == 0 goto _iffalse_8($21)
main.c(224:94:112:0:0)		_GO_DONE [k25 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{bit bit {0,1}} := 0x1 {literal bit {0,1}}
main.c(224:95:113:0:0)	 _iffalse_8($21) :
main.c(225:96:114:0:0)	 nop main.c(225:97:115:0:0)	 nop main.c(225:98:116:0:0)	 nop main.c(225:99:117:0:0)	 nop main.c(225:100:118:0:0)	 nop main.c(226:101:119:0:0)	 nop main.c(226:102:120:0:0)	 nop main.c(226:103:121:0:0)	 nop main.c(226:104:122:0:0)	 nop main.c(226:105:123:0:0)	 nop main.c(227:106:124:0:0)		if _sensorSample_phase3_1_1 [k19 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{bit bit {0,1}} == 0 goto _whilecontinue_1($24)
main.c(228:107:128:0:0)		_GO_DONE [k25 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{bit bit {0,1}} := 0x1 {literal bit {0,1}}
main.c(231:109:130:1:0)	 _whilecontinue_1($24) :
main.c(231:110:131:1:0)		if _NOT_DONE [k30 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{bit bit {0,1}} != 0 goto _whilecontinue_1($24)
main.c(234:111:136:0:0)		_TRISB [k10 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{sfr volatile unsigned char} := 0xf0 {literal unsigned char}
main.c(236:112:137:0:0)		iTemp15 [k31 lr112:114 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sensorSample_h_1_1}[r0x6A ] := _ADRESH [k33 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{sfr volatile unsigned char}
main.c(237:113:138:0:0)		iTemp16 [k34 lr113:116 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sensorSample_l_1_1}[r0x6B ] := _ADRESL [k36 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{sfr volatile unsigned char}
main.c(238:114:139:0:0)		iTemp17 [k37 lr114:115 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int}[r0x6A r0x6C ] = (int)iTemp15 [k31 lr112:114 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sensorSample_h_1_1}[r0x6A ]
main.c(238:115:140:0:0)		iTemp18 [k38 lr115:117 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int}[r0x6A r0x6C ] = iTemp17 [k37 lr114:115 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int}[r0x6A r0x6C ] << 0x8 {literal unsigned char}
main.c(238:116:142:0:0)		iTemp20 [k40 lr116:117 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{unsigned int}[r0x6B r0x6D ] = (unsigned int)iTemp16 [k34 lr113:116 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sensorSample_l_1_1}[r0x6B ]
main.c(238:117:143:0:0)		iTemp21 [k41 lr117:118 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{unsigned int}[r0x6A r0x6C ] = iTemp18 [k38 lr115:117 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int}[r0x6A r0x6C ] | iTemp20 [k40 lr116:117 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{unsigned int}[r0x6B r0x6D ]
main.c(238:118:145:0:0)		ret iTemp21 [k41 lr117:118 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{unsigned int}[r0x6A r0x6C ]
main.c(238:119:146:0:0)	 _return($27) :
main.c(238:120:147:0:0)		eproc _sensorSample [k1 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int function    }

----------------------------------------------------------------
Basic Block _ifend_2 : loop Depth = 0 noPath = 0 , lastinLoop = 0
depth 1st num 4 : bbnum = 3 1st iCode = 16 , last iCode = 18
visited 0 : hasFcall = 0

defines bitVector :bitvector Size = 151 bSize = 19
Bits on { (18) }

local defines bitVector :bitvector Size = 151 bSize = 19
Bits on { (18) }

pointers Set bitvector :
in pointers Set bitvector :
inDefs Set bitvector :bitvector Size = 151 bSize = 19
Bits on { (2) (3) (4) (5) (6) (7) (8) (9) (13) (16) }

outDefs Set bitvector :bitvector Size = 151 bSize = 19
Bits on { (2) (3) (4) (5) (6) (7) (8) (9) (13) (16) (18) }

usesDefs Set bitvector :bitvector Size = 151 bSize = 19
Bits on { (4) (18) }

----------------------------------------------------------------
main.c(195:16:17:0:0)	 _ifend_2($3) :
main.c(196:17:18:0:0)		iTemp5 [k14 lr17:18 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{char} = iTemp2 [k6 lr5:31 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sensorSample_phase_1_1}[r0x67 ] == 0x1 {literal unsigned char}
main.c(196:18:19:0:0)		if iTemp5 [k14 lr17:18 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{char} == 0 goto _iffalse_3($5)
main.c(196:19:22:0:0)		_sensorSample_phase1_1_1 [k15 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{bit bit {0,1}} := 0x1 {literal bit {0,1}}
main.c(196:20:23:0:0)		 goto _ifend_3($6)
main.c(196:21:24:0:0)	 _iffalse_3($5) :
main.c(196:22:25:0:0)		_sensorSample_phase1_1_1 [k15 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{bit bit {0,1}} := 0x0 {literal bit {0,1}}
main.c(196:23:26:0:0)	 _ifend_3($6) :
main.c(197:24:27:0:0)		iTemp6 [k16 lr24:25 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{char} = iTemp2 [k6 lr5:31 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sensorSample_phase_1_1}[r0x67 ] == 0x2 {literal unsigned char}
main.c(197:25:28:0:0)		if iTemp6 [k16 lr24:25 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{char} == 0 goto _iffalse_4($8)
main.c(197:26:31:0:0)		_sensorSample_phase2_1_1 [k17 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{bit bit {0,1}} := 0x1 {literal bit {0,1}}
main.c(197:27:32:0:0)		 goto _ifend_4($9)
main.c(197:28:33:0:0)	 _iffalse_4($8) :
main.c(197:29:34:0:0)		_sensorSample_phase2_1_1 [k17 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{bit bit {0,1}} := 0x0 {literal bit {0,1}}
main.c(197:30:35:0:0)	 _ifend_4($9) :
main.c(198:31:36:0:0)		iTemp7 [k18 lr31:32 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{char} = iTemp2 [k6 lr5:31 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sensorSample_phase_1_1}[r0x67 ] == 0x3 {literal unsigned char}
main.c(198:32:37:0:0)		if iTemp7 [k18 lr31:32 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{char} == 0 goto _iffalse_5($11)
main.c(198:33:40:0:0)		_sensorSample_phase3_1_1 [k19 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{bit bit {0,1}} := 0x1 {literal bit {0,1}}
main.c(198:34:41:0:0)		 goto _ifend_5($12)
main.c(198:35:42:0:0)	 _iffalse_5($11) :
main.c(198:36:43:0:0)		_sensorSample_phase3_1_1 [k19 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{bit bit {0,1}} := 0x0 {literal bit {0,1}}
main.c(198:37:44:0:0)	 _ifend_5($12) :
main.c(204:38:45:0:0)		iTemp8 [k20 lr38:40 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sensorSample_halfcycles_1_1}[r0x68 ] := _numResonationHalfcycles [k22 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{unsigned char}
main.c(205:40:150:0:0)		iTemp10 [k24 lr40:65 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sensorSample_halfcycles_1_1}[r0x69 ] := iTemp8 [k20 lr38:40 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sensorSample_halfcycles_1_1}[r0x68 ]
main.c(205:41:46:1:0)	 _dobody_1($13) :
main.c(206:42:47:1:0)		_PORTB [k8 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{sfr volatile unsigned char} = _PORTB [k8 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{sfr volatile unsigned char} ^ iTemp0 [k2 lr3:65 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sensorSample_transmitter_1_1}[r0x65 ]
main.c(207:44:49:1:0)	 nop main.c(207:45:50:1:0)	 nop main.c(207:46:51:1:0)	 nop main.c(208:47:52:1:0)	 nop main.c(208:48:53:1:0)	 nop main.c(208:49:54:1:0)	 nop main.c(208:50:55:1:0)	 nop main.c(208:51:56:1:0)	 nop main.c(209:52:57:1:0)	 nop main.c(209:53:58:1:0)	 nop main.c(209:54:59:1:0)	 nop main.c(209:55:60:1:0)	 nop main.c(209:56:61:1:0)	 nop main.c(210:57:62:1:0)	 nop main.c(210:58:63:1:0)	 nop main.c(210:59:64:1:0)	 nop main.c(210:60:65:1:0)	 nop main.c(210:61:66:1:0)	 nop main.c(211:62:67:1:0)	 nop main.c(211:63:68:1:0)	 nop main.c(212:64:70:1:0)		iTemp10 [k24 lr40:65 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sensorSample_halfcycles_1_1}[r0x69 ] = iTemp10 [k24 lr40:65 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sensorSample_halfcycles_1_1}[r0x69 ] - 0x1 {literal unsigned char}
main.c(212:65:72:1:0)		if iTemp10 [k24 lr40:65 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sensorSample_halfcycles_1_1}[r0x69 ] != 0 goto _dobody_1($13)
main.c(215:66:75:0:0)		if _sensorSample_phase0_1_1 [k13 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{bit bit {0,1}} == 0 goto _iffalse_6($17)
main.c(216:67:79:0:0)		_GO_DONE [k25 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{bit bit {0,1}} := 0x1 {literal bit {0,1}}
main.c(216:68:80:0:0)	 _iffalse_6($17) :
main.c(217:69:81:0:0)	 nop main.c(217:70:82:0:0)	 nop main.c(217:71:83:0:0)	 nop main.c(217:72:84:0:0)	 nop main.c(217:73:85:0:0)	 nop main.c(218:74:86:0:0)	 nop main.c(218:75:87:0:0)	 nop main.c(218:76:88:0:0)	 nop main.c(218:77:89:0:0)	 nop main.c(218:78:90:0:0)	 nop main.c(219:79:91:0:0)		if _sensorSample_phase1_1_1 [k15 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{bit bit {0,1}} == 0 goto _iffalse_7($19)
main.c(220:80:95:0:0)		_GO_DONE [k25 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{bit bit {0,1}} := 0x1 {literal bit {0,1}}
main.c(220:81:96:0:0)	 _iffalse_7($19) :
main.c(221:82:97:0:0)	 nop main.c(221:83:98:0:0)	 nop main.c(221:84:99:0:0)	 nop main.c(221:85:100:0:0)	 nop main.c(221:86:101:0:0)	 nop main.c(222:87:102:0:0)	 nop main.c(222:88:103:0:0)	 nop main.c(222:89:104:0:0)	 nop main.c(222:90:105:0:0)	 nop main.c(222:91:106:0:0)	 nop main.c(222:92:107:0:0)	 nop main.c(223:93:108:0:0)		if _sensorSample_phase2_1_1 [k17 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{bit bit {0,1}} == 0 goto _iffalse_8($21)
main.c(224:94:112:0:0)		_GO_DONE [k25 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{bit bit {0,1}} := 0x1 {literal bit {0,1}}
main.c(224:95:113:0:0)	 _iffalse_8($21) :
main.c(225:96:114:0:0)	 nop main.c(225:97:115:0:0)	 nop main.c(225:98:116:0:0)	 nop main.c(225:99:117:0:0)	 nop main.c(225:100:118:0:0)	 nop main.c(226:101:119:0:0)	 nop main.c(226:102:120:0:0)	 nop main.c(226:103:121:0:0)	 nop main.c(226:104:122:0:0)	 nop main.c(226:105:123:0:0)	 nop main.c(227:106:124:0:0)		if _sensorSample_phase3_1_1 [k19 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{bit bit {0,1}} == 0 goto _whilecontinue_1($24)
main.c(228:107:128:0:0)		_GO_DONE [k25 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{bit bit {0,1}} := 0x1 {literal bit {0,1}}
main.c(231:109:130:1:0)	 _whilecontinue_1($24) :
main.c(231:110:131:1:0)		if _NOT_DONE [k30 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{bit bit {0,1}} != 0 goto _whilecontinue_1($24)
main.c(234:111:136:0:0)		_TRISB [k10 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{sfr volatile unsigned char} := 0xf0 {literal unsigned char}
main.c(236:112:137:0:0)		iTemp15 [k31 lr112:114 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sensorSample_h_1_1}[r0x6A ] := _ADRESH [k33 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{sfr volatile unsigned char}
main.c(237:113:138:0:0)		iTemp16 [k34 lr113:116 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sensorSample_l_1_1}[r0x6B ] := _ADRESL [k36 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{sfr volatile unsigned char}
main.c(238:114:139:0:0)		iTemp17 [k37 lr114:115 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int}[r0x6A r0x6C ] = (int)iTemp15 [k31 lr112:114 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sensorSample_h_1_1}[r0x6A ]
main.c(238:115:140:0:0)		iTemp18 [k38 lr115:117 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int}[r0x6A r0x6C ] = iTemp17 [k37 lr114:115 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int}[r0x6A r0x6C ] << 0x8 {literal unsigned char}
main.c(238:116:142:0:0)		iTemp20 [k40 lr116:117 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{unsigned int}[r0x6B r0x6D ] = (unsigned int)iTemp16 [k34 lr113:116 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sensorSample_l_1_1}[r0x6B ]
main.c(238:117:143:0:0)		iTemp21 [k41 lr117:118 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{unsigned int}[r0x6A r0x6C ] = iTemp18 [k38 lr115:117 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int}[r0x6A r0x6C ] | iTemp20 [k40 lr116:117 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{unsigned int}[r0x6B r0x6D ]
main.c(238:118:145:0:0)		ret iTemp21 [k41 lr117:118 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{unsigned int}[r0x6A r0x6C ]
main.c(238:119:146:0:0)	 _return($27) :
main.c(238:120:147:0:0)		eproc _sensorSample [k1 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int function    }

----------------------------------------------------------------
Basic Block _eBBlock1 : loop Depth = 0 noPath = 0 , lastinLoop = 0
depth 1st num 5 : bbnum = 4 1st iCode = 19 , last iCode = 20
visited 0 : hasFcall = 0

defines bitVector :bitvector Size = 151 bSize = 19
Bits on { (22) }

local defines bitVector :bitvector Size = 151 bSize = 19
Bits on { (22) }

pointers Set bitvector :
in pointers Set bitvector :
inDefs Set bitvector :bitvector Size = 151 bSize = 19
Bits on { (2) (3) (4) (5) (6) (7) (8) (9) (13) (16) (18) }

outDefs Set bitvector :bitvector Size = 151 bSize = 19
Bits on { (2) (3) (4) (5) (6) (7) (8) (9) (13) (16) (18) (22) }

usesDefs Set bitvector :bitvector Size = 151 bSize = 19
Bits on { }

----------------------------------------------------------------
main.c(196:19:22:0:0)		_sensorSample_phase1_1_1 [k15 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{bit bit {0,1}} := 0x1 {literal bit {0,1}}
main.c(196:20:23:0:0)		 goto _ifend_3($6)
main.c(196:21:24:0:0)	 _iffalse_3($5) :
main.c(196:22:25:0:0)		_sensorSample_phase1_1_1 [k15 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{bit bit {0,1}} := 0x0 {literal bit {0,1}}
main.c(196:23:26:0:0)	 _ifend_3($6) :
main.c(197:24:27:0:0)		iTemp6 [k16 lr24:25 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{char} = iTemp2 [k6 lr5:31 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sensorSample_phase_1_1}[r0x67 ] == 0x2 {literal unsigned char}
main.c(197:25:28:0:0)		if iTemp6 [k16 lr24:25 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{char} == 0 goto _iffalse_4($8)
main.c(197:26:31:0:0)		_sensorSample_phase2_1_1 [k17 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{bit bit {0,1}} := 0x1 {literal bit {0,1}}
main.c(197:27:32:0:0)		 goto _ifend_4($9)
main.c(197:28:33:0:0)	 _iffalse_4($8) :
main.c(197:29:34:0:0)		_sensorSample_phase2_1_1 [k17 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{bit bit {0,1}} := 0x0 {literal bit {0,1}}
main.c(197:30:35:0:0)	 _ifend_4($9) :
main.c(198:31:36:0:0)		iTemp7 [k18 lr31:32 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{char} = iTemp2 [k6 lr5:31 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sensorSample_phase_1_1}[r0x67 ] == 0x3 {literal unsigned char}
main.c(198:32:37:0:0)		if iTemp7 [k18 lr31:32 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{char} == 0 goto _iffalse_5($11)
main.c(198:33:40:0:0)		_sensorSample_phase3_1_1 [k19 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{bit bit {0,1}} := 0x1 {literal bit {0,1}}
main.c(198:34:41:0:0)		 goto _ifend_5($12)
main.c(198:35:42:0:0)	 _iffalse_5($11) :
main.c(198:36:43:0:0)		_sensorSample_phase3_1_1 [k19 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{bit bit {0,1}} := 0x0 {literal bit {0,1}}
main.c(198:37:44:0:0)	 _ifend_5($12) :
main.c(204:38:45:0:0)		iTemp8 [k20 lr38:40 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sensorSample_halfcycles_1_1}[r0x68 ] := _numResonationHalfcycles [k22 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{unsigned char}
main.c(205:40:150:0:0)		iTemp10 [k24 lr40:65 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sensorSample_halfcycles_1_1}[r0x69 ] := iTemp8 [k20 lr38:40 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sensorSample_halfcycles_1_1}[r0x68 ]
main.c(205:41:46:1:0)	 _dobody_1($13) :
main.c(206:42:47:1:0)		_PORTB [k8 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{sfr volatile unsigned char} = _PORTB [k8 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{sfr volatile unsigned char} ^ iTemp0 [k2 lr3:65 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sensorSample_transmitter_1_1}[r0x65 ]
main.c(207:44:49:1:0)	 nop main.c(207:45:50:1:0)	 nop main.c(207:46:51:1:0)	 nop main.c(208:47:52:1:0)	 nop main.c(208:48:53:1:0)	 nop main.c(208:49:54:1:0)	 nop main.c(208:50:55:1:0)	 nop main.c(208:51:56:1:0)	 nop main.c(209:52:57:1:0)	 nop main.c(209:53:58:1:0)	 nop main.c(209:54:59:1:0)	 nop main.c(209:55:60:1:0)	 nop main.c(209:56:61:1:0)	 nop main.c(210:57:62:1:0)	 nop main.c(210:58:63:1:0)	 nop main.c(210:59:64:1:0)	 nop main.c(210:60:65:1:0)	 nop main.c(210:61:66:1:0)	 nop main.c(211:62:67:1:0)	 nop main.c(211:63:68:1:0)	 nop main.c(212:64:70:1:0)		iTemp10 [k24 lr40:65 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sensorSample_halfcycles_1_1}[r0x69 ] = iTemp10 [k24 lr40:65 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sensorSample_halfcycles_1_1}[r0x69 ] - 0x1 {literal unsigned char}
main.c(212:65:72:1:0)		if iTemp10 [k24 lr40:65 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sensorSample_halfcycles_1_1}[r0x69 ] != 0 goto _dobody_1($13)
main.c(215:66:75:0:0)		if _sensorSample_phase0_1_1 [k13 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{bit bit {0,1}} == 0 goto _iffalse_6($17)
main.c(216:67:79:0:0)		_GO_DONE [k25 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{bit bit {0,1}} := 0x1 {literal bit {0,1}}
main.c(216:68:80:0:0)	 _iffalse_6($17) :
main.c(217:69:81:0:0)	 nop main.c(217:70:82:0:0)	 nop main.c(217:71:83:0:0)	 nop main.c(217:72:84:0:0)	 nop main.c(217:73:85:0:0)	 nop main.c(218:74:86:0:0)	 nop main.c(218:75:87:0:0)	 nop main.c(218:76:88:0:0)	 nop main.c(218:77:89:0:0)	 nop main.c(218:78:90:0:0)	 nop main.c(219:79:91:0:0)		if _sensorSample_phase1_1_1 [k15 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{bit bit {0,1}} == 0 goto _iffalse_7($19)
main.c(220:80:95:0:0)		_GO_DONE [k25 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{bit bit {0,1}} := 0x1 {literal bit {0,1}}
main.c(220:81:96:0:0)	 _iffalse_7($19) :
main.c(221:82:97:0:0)	 nop main.c(221:83:98:0:0)	 nop main.c(221:84:99:0:0)	 nop main.c(221:85:100:0:0)	 nop main.c(221:86:101:0:0)	 nop main.c(222:87:102:0:0)	 nop main.c(222:88:103:0:0)	 nop main.c(222:89:104:0:0)	 nop main.c(222:90:105:0:0)	 nop main.c(222:91:106:0:0)	 nop main.c(222:92:107:0:0)	 nop main.c(223:93:108:0:0)		if _sensorSample_phase2_1_1 [k17 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{bit bit {0,1}} == 0 goto _iffalse_8($21)
main.c(224:94:112:0:0)		_GO_DONE [k25 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{bit bit {0,1}} := 0x1 {literal bit {0,1}}
main.c(224:95:113:0:0)	 _iffalse_8($21) :
main.c(225:96:114:0:0)	 nop main.c(225:97:115:0:0)	 nop main.c(225:98:116:0:0)	 nop main.c(225:99:117:0:0)	 nop main.c(225:100:118:0:0)	 nop main.c(226:101:119:0:0)	 nop main.c(226:102:120:0:0)	 nop main.c(226:103:121:0:0)	 nop main.c(226:104:122:0:0)	 nop main.c(226:105:123:0:0)	 nop main.c(227:106:124:0:0)		if _sensorSample_phase3_1_1 [k19 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{bit bit {0,1}} == 0 goto _whilecontinue_1($24)
main.c(228:107:128:0:0)		_GO_DONE [k25 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{bit bit {0,1}} := 0x1 {literal bit {0,1}}
main.c(231:109:130:1:0)	 _whilecontinue_1($24) :
main.c(231:110:131:1:0)		if _NOT_DONE [k30 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{bit bit {0,1}} != 0 goto _whilecontinue_1($24)
main.c(234:111:136:0:0)		_TRISB [k10 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{sfr volatile unsigned char} := 0xf0 {literal unsigned char}
main.c(236:112:137:0:0)		iTemp15 [k31 lr112:114 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sensorSample_h_1_1}[r0x6A ] := _ADRESH [k33 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{sfr volatile unsigned char}
main.c(237:113:138:0:0)		iTemp16 [k34 lr113:116 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sensorSample_l_1_1}[r0x6B ] := _ADRESL [k36 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{sfr volatile unsigned char}
main.c(238:114:139:0:0)		iTemp17 [k37 lr114:115 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int}[r0x6A r0x6C ] = (int)iTemp15 [k31 lr112:114 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sensorSample_h_1_1}[r0x6A ]
main.c(238:115:140:0:0)		iTemp18 [k38 lr115:117 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int}[r0x6A r0x6C ] = iTemp17 [k37 lr114:115 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int}[r0x6A r0x6C ] << 0x8 {literal unsigned char}
main.c(238:116:142:0:0)		iTemp20 [k40 lr116:117 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{unsigned int}[r0x6B r0x6D ] = (unsigned int)iTemp16 [k34 lr113:116 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sensorSample_l_1_1}[r0x6B ]
main.c(238:117:143:0:0)		iTemp21 [k41 lr117:118 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{unsigned int}[r0x6A r0x6C ] = iTemp18 [k38 lr115:117 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int}[r0x6A r0x6C ] | iTemp20 [k40 lr116:117 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{unsigned int}[r0x6B r0x6D ]
main.c(238:118:145:0:0)		ret iTemp21 [k41 lr117:118 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{unsigned int}[r0x6A r0x6C ]
main.c(238:119:146:0:0)	 _return($27) :
main.c(238:120:147:0:0)		eproc _sensorSample [k1 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int function    }

----------------------------------------------------------------
Basic Block _iffalse_3 : loop Depth = 0 noPath = 0 , lastinLoop = 0
depth 1st num 6 : bbnum = 5 1st iCode = 21 , last iCode = 22
visited 0 : hasFcall = 0

defines bitVector :bitvector Size = 151 bSize = 19
Bits on { (25) }

local defines bitVector :bitvector Size = 151 bSize = 19
Bits on { (25) }

pointers Set bitvector :
in pointers Set bitvector :
inDefs Set bitvector :bitvector Size = 151 bSize = 19
Bits on { (2) (3) (4) (5) (6) (7) (8) (9) (13) (16) (18) }

outDefs Set bitvector :bitvector Size = 151 bSize = 19
Bits on { (2) (3) (4) (5) (6) (7) (8) (9) (13) (16) (18) (25) }

usesDefs Set bitvector :bitvector Size = 151 bSize = 19
Bits on { }

----------------------------------------------------------------
main.c(196:21:24:0:0)	 _iffalse_3($5) :
main.c(196:22:25:0:0)		_sensorSample_phase1_1_1 [k15 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{bit bit {0,1}} := 0x0 {literal bit {0,1}}
main.c(196:23:26:0:0)	 _ifend_3($6) :
main.c(197:24:27:0:0)		iTemp6 [k16 lr24:25 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{char} = iTemp2 [k6 lr5:31 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sensorSample_phase_1_1}[r0x67 ] == 0x2 {literal unsigned char}
main.c(197:25:28:0:0)		if iTemp6 [k16 lr24:25 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{char} == 0 goto _iffalse_4($8)
main.c(197:26:31:0:0)		_sensorSample_phase2_1_1 [k17 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{bit bit {0,1}} := 0x1 {literal bit {0,1}}
main.c(197:27:32:0:0)		 goto _ifend_4($9)
main.c(197:28:33:0:0)	 _iffalse_4($8) :
main.c(197:29:34:0:0)		_sensorSample_phase2_1_1 [k17 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{bit bit {0,1}} := 0x0 {literal bit {0,1}}
main.c(197:30:35:0:0)	 _ifend_4($9) :
main.c(198:31:36:0:0)		iTemp7 [k18 lr31:32 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{char} = iTemp2 [k6 lr5:31 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sensorSample_phase_1_1}[r0x67 ] == 0x3 {literal unsigned char}
main.c(198:32:37:0:0)		if iTemp7 [k18 lr31:32 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{char} == 0 goto _iffalse_5($11)
main.c(198:33:40:0:0)		_sensorSample_phase3_1_1 [k19 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{bit bit {0,1}} := 0x1 {literal bit {0,1}}
main.c(198:34:41:0:0)		 goto _ifend_5($12)
main.c(198:35:42:0:0)	 _iffalse_5($11) :
main.c(198:36:43:0:0)		_sensorSample_phase3_1_1 [k19 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{bit bit {0,1}} := 0x0 {literal bit {0,1}}
main.c(198:37:44:0:0)	 _ifend_5($12) :
main.c(204:38:45:0:0)		iTemp8 [k20 lr38:40 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sensorSample_halfcycles_1_1}[r0x68 ] := _numResonationHalfcycles [k22 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{unsigned char}
main.c(205:40:150:0:0)		iTemp10 [k24 lr40:65 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sensorSample_halfcycles_1_1}[r0x69 ] := iTemp8 [k20 lr38:40 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sensorSample_halfcycles_1_1}[r0x68 ]
main.c(205:41:46:1:0)	 _dobody_1($13) :
main.c(206:42:47:1:0)		_PORTB [k8 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{sfr volatile unsigned char} = _PORTB [k8 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{sfr volatile unsigned char} ^ iTemp0 [k2 lr3:65 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sensorSample_transmitter_1_1}[r0x65 ]
main.c(207:44:49:1:0)	 nop main.c(207:45:50:1:0)	 nop main.c(207:46:51:1:0)	 nop main.c(208:47:52:1:0)	 nop main.c(208:48:53:1:0)	 nop main.c(208:49:54:1:0)	 nop main.c(208:50:55:1:0)	 nop main.c(208:51:56:1:0)	 nop main.c(209:52:57:1:0)	 nop main.c(209:53:58:1:0)	 nop main.c(209:54:59:1:0)	 nop main.c(209:55:60:1:0)	 nop main.c(209:56:61:1:0)	 nop main.c(210:57:62:1:0)	 nop main.c(210:58:63:1:0)	 nop main.c(210:59:64:1:0)	 nop main.c(210:60:65:1:0)	 nop main.c(210:61:66:1:0)	 nop main.c(211:62:67:1:0)	 nop main.c(211:63:68:1:0)	 nop main.c(212:64:70:1:0)		iTemp10 [k24 lr40:65 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sensorSample_halfcycles_1_1}[r0x69 ] = iTemp10 [k24 lr40:65 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sensorSample_halfcycles_1_1}[r0x69 ] - 0x1 {literal unsigned char}
main.c(212:65:72:1:0)		if iTemp10 [k24 lr40:65 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sensorSample_halfcycles_1_1}[r0x69 ] != 0 goto _dobody_1($13)
main.c(215:66:75:0:0)		if _sensorSample_phase0_1_1 [k13 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{bit bit {0,1}} == 0 goto _iffalse_6($17)
main.c(216:67:79:0:0)		_GO_DONE [k25 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{bit bit {0,1}} := 0x1 {literal bit {0,1}}
main.c(216:68:80:0:0)	 _iffalse_6($17) :
main.c(217:69:81:0:0)	 nop main.c(217:70:82:0:0)	 nop main.c(217:71:83:0:0)	 nop main.c(217:72:84:0:0)	 nop main.c(217:73:85:0:0)	 nop main.c(218:74:86:0:0)	 nop main.c(218:75:87:0:0)	 nop main.c(218:76:88:0:0)	 nop main.c(218:77:89:0:0)	 nop main.c(218:78:90:0:0)	 nop main.c(219:79:91:0:0)		if _sensorSample_phase1_1_1 [k15 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{bit bit {0,1}} == 0 goto _iffalse_7($19)
main.c(220:80:95:0:0)		_GO_DONE [k25 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{bit bit {0,1}} := 0x1 {literal bit {0,1}}
main.c(220:81:96:0:0)	 _iffalse_7($19) :
main.c(221:82:97:0:0)	 nop main.c(221:83:98:0:0)	 nop main.c(221:84:99:0:0)	 nop main.c(221:85:100:0:0)	 nop main.c(221:86:101:0:0)	 nop main.c(222:87:102:0:0)	 nop main.c(222:88:103:0:0)	 nop main.c(222:89:104:0:0)	 nop main.c(222:90:105:0:0)	 nop main.c(222:91:106:0:0)	 nop main.c(222:92:107:0:0)	 nop main.c(223:93:108:0:0)		if _sensorSample_phase2_1_1 [k17 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{bit bit {0,1}} == 0 goto _iffalse_8($21)
main.c(224:94:112:0:0)		_GO_DONE [k25 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{bit bit {0,1}} := 0x1 {literal bit {0,1}}
main.c(224:95:113:0:0)	 _iffalse_8($21) :
main.c(225:96:114:0:0)	 nop main.c(225:97:115:0:0)	 nop main.c(225:98:116:0:0)	 nop main.c(225:99:117:0:0)	 nop main.c(225:100:118:0:0)	 nop main.c(226:101:119:0:0)	 nop main.c(226:102:120:0:0)	 nop main.c(226:103:121:0:0)	 nop main.c(226:104:122:0:0)	 nop main.c(226:105:123:0:0)	 nop main.c(227:106:124:0:0)		if _sensorSample_phase3_1_1 [k19 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{bit bit {0,1}} == 0 goto _whilecontinue_1($24)
main.c(228:107:128:0:0)		_GO_DONE [k25 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{bit bit {0,1}} := 0x1 {literal bit {0,1}}
main.c(231:109:130:1:0)	 _whilecontinue_1($24) :
main.c(231:110:131:1:0)		if _NOT_DONE [k30 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{bit bit {0,1}} != 0 goto _whilecontinue_1($24)
main.c(234:111:136:0:0)		_TRISB [k10 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{sfr volatile unsigned char} := 0xf0 {literal unsigned char}
main.c(236:112:137:0:0)		iTemp15 [k31 lr112:114 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sensorSample_h_1_1}[r0x6A ] := _ADRESH [k33 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{sfr volatile unsigned char}
main.c(237:113:138:0:0)		iTemp16 [k34 lr113:116 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sensorSample_l_1_1}[r0x6B ] := _ADRESL [k36 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{sfr volatile unsigned char}
main.c(238:114:139:0:0)		iTemp17 [k37 lr114:115 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int}[r0x6A r0x6C ] = (int)iTemp15 [k31 lr112:114 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sensorSample_h_1_1}[r0x6A ]
main.c(238:115:140:0:0)		iTemp18 [k38 lr115:117 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int}[r0x6A r0x6C ] = iTemp17 [k37 lr114:115 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int}[r0x6A r0x6C ] << 0x8 {literal unsigned char}
main.c(238:116:142:0:0)		iTemp20 [k40 lr116:117 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{unsigned int}[r0x6B r0x6D ] = (unsigned int)iTemp16 [k34 lr113:116 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sensorSample_l_1_1}[r0x6B ]
main.c(238:117:143:0:0)		iTemp21 [k41 lr117:118 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{unsigned int}[r0x6A r0x6C ] = iTemp18 [k38 lr115:117 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int}[r0x6A r0x6C ] | iTemp20 [k40 lr116:117 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{unsigned int}[r0x6B r0x6D ]
main.c(238:118:145:0:0)		ret iTemp21 [k41 lr117:118 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{unsigned int}[r0x6A r0x6C ]
main.c(238:119:146:0:0)	 _return($27) :
main.c(238:120:147:0:0)		eproc _sensorSample [k1 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int function    }

----------------------------------------------------------------
Basic Block _ifend_3 : loop Depth = 0 noPath = 0 , lastinLoop = 0
depth 1st num 7 : bbnum = 6 1st iCode = 23 , last iCode = 25
visited 0 : hasFcall = 0

defines bitVector :bitvector Size = 151 bSize = 19
Bits on { (27) }

local defines bitVector :bitvector Size = 151 bSize = 19
Bits on { (27) }

pointers Set bitvector :
in pointers Set bitvector :
inDefs Set bitvector :bitvector Size = 151 bSize = 19
Bits on { (2) (3) (4) (5) (6) (7) (8) (9) (13) (16) (18) (22) (25) }

outDefs Set bitvector :bitvector Size = 151 bSize = 19
Bits on { (2) (3) (4) (5) (6) (7) (8) (9) (13) (16) (18) (22) (25) (27) }

usesDefs Set bitvector :bitvector Size = 151 bSize = 19
Bits on { (4) (27) }

----------------------------------------------------------------
main.c(196:23:26:0:0)	 _ifend_3($6) :
main.c(197:24:27:0:0)		iTemp6 [k16 lr24:25 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{char} = iTemp2 [k6 lr5:31 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sensorSample_phase_1_1}[r0x67 ] == 0x2 {literal unsigned char}
main.c(197:25:28:0:0)		if iTemp6 [k16 lr24:25 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{char} == 0 goto _iffalse_4($8)
main.c(197:26:31:0:0)		_sensorSample_phase2_1_1 [k17 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{bit bit {0,1}} := 0x1 {literal bit {0,1}}
main.c(197:27:32:0:0)		 goto _ifend_4($9)
main.c(197:28:33:0:0)	 _iffalse_4($8) :
main.c(197:29:34:0:0)		_sensorSample_phase2_1_1 [k17 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{bit bit {0,1}} := 0x0 {literal bit {0,1}}
main.c(197:30:35:0:0)	 _ifend_4($9) :
main.c(198:31:36:0:0)		iTemp7 [k18 lr31:32 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{char} = iTemp2 [k6 lr5:31 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sensorSample_phase_1_1}[r0x67 ] == 0x3 {literal unsigned char}
main.c(198:32:37:0:0)		if iTemp7 [k18 lr31:32 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{char} == 0 goto _iffalse_5($11)
main.c(198:33:40:0:0)		_sensorSample_phase3_1_1 [k19 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{bit bit {0,1}} := 0x1 {literal bit {0,1}}
main.c(198:34:41:0:0)		 goto _ifend_5($12)
main.c(198:35:42:0:0)	 _iffalse_5($11) :
main.c(198:36:43:0:0)		_sensorSample_phase3_1_1 [k19 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{bit bit {0,1}} := 0x0 {literal bit {0,1}}
main.c(198:37:44:0:0)	 _ifend_5($12) :
main.c(204:38:45:0:0)		iTemp8 [k20 lr38:40 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sensorSample_halfcycles_1_1}[r0x68 ] := _numResonationHalfcycles [k22 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{unsigned char}
main.c(205:40:150:0:0)		iTemp10 [k24 lr40:65 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sensorSample_halfcycles_1_1}[r0x69 ] := iTemp8 [k20 lr38:40 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sensorSample_halfcycles_1_1}[r0x68 ]
main.c(205:41:46:1:0)	 _dobody_1($13) :
main.c(206:42:47:1:0)		_PORTB [k8 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{sfr volatile unsigned char} = _PORTB [k8 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{sfr volatile unsigned char} ^ iTemp0 [k2 lr3:65 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sensorSample_transmitter_1_1}[r0x65 ]
main.c(207:44:49:1:0)	 nop main.c(207:45:50:1:0)	 nop main.c(207:46:51:1:0)	 nop main.c(208:47:52:1:0)	 nop main.c(208:48:53:1:0)	 nop main.c(208:49:54:1:0)	 nop main.c(208:50:55:1:0)	 nop main.c(208:51:56:1:0)	 nop main.c(209:52:57:1:0)	 nop main.c(209:53:58:1:0)	 nop main.c(209:54:59:1:0)	 nop main.c(209:55:60:1:0)	 nop main.c(209:56:61:1:0)	 nop main.c(210:57:62:1:0)	 nop main.c(210:58:63:1:0)	 nop main.c(210:59:64:1:0)	 nop main.c(210:60:65:1:0)	 nop main.c(210:61:66:1:0)	 nop main.c(211:62:67:1:0)	 nop main.c(211:63:68:1:0)	 nop main.c(212:64:70:1:0)		iTemp10 [k24 lr40:65 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sensorSample_halfcycles_1_1}[r0x69 ] = iTemp10 [k24 lr40:65 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sensorSample_halfcycles_1_1}[r0x69 ] - 0x1 {literal unsigned char}
main.c(212:65:72:1:0)		if iTemp10 [k24 lr40:65 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sensorSample_halfcycles_1_1}[r0x69 ] != 0 goto _dobody_1($13)
main.c(215:66:75:0:0)		if _sensorSample_phase0_1_1 [k13 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{bit bit {0,1}} == 0 goto _iffalse_6($17)
main.c(216:67:79:0:0)		_GO_DONE [k25 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{bit bit {0,1}} := 0x1 {literal bit {0,1}}
main.c(216:68:80:0:0)	 _iffalse_6($17) :
main.c(217:69:81:0:0)	 nop main.c(217:70:82:0:0)	 nop main.c(217:71:83:0:0)	 nop main.c(217:72:84:0:0)	 nop main.c(217:73:85:0:0)	 nop main.c(218:74:86:0:0)	 nop main.c(218:75:87:0:0)	 nop main.c(218:76:88:0:0)	 nop main.c(218:77:89:0:0)	 nop main.c(218:78:90:0:0)	 nop main.c(219:79:91:0:0)		if _sensorSample_phase1_1_1 [k15 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{bit bit {0,1}} == 0 goto _iffalse_7($19)
main.c(220:80:95:0:0)		_GO_DONE [k25 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{bit bit {0,1}} := 0x1 {literal bit {0,1}}
main.c(220:81:96:0:0)	 _iffalse_7($19) :
main.c(221:82:97:0:0)	 nop main.c(221:83:98:0:0)	 nop main.c(221:84:99:0:0)	 nop main.c(221:85:100:0:0)	 nop main.c(221:86:101:0:0)	 nop main.c(222:87:102:0:0)	 nop main.c(222:88:103:0:0)	 nop main.c(222:89:104:0:0)	 nop main.c(222:90:105:0:0)	 nop main.c(222:91:106:0:0)	 nop main.c(222:92:107:0:0)	 nop main.c(223:93:108:0:0)		if _sensorSample_phase2_1_1 [k17 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{bit bit {0,1}} == 0 goto _iffalse_8($21)
main.c(224:94:112:0:0)		_GO_DONE [k25 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{bit bit {0,1}} := 0x1 {literal bit {0,1}}
main.c(224:95:113:0:0)	 _iffalse_8($21) :
main.c(225:96:114:0:0)	 nop main.c(225:97:115:0:0)	 nop main.c(225:98:116:0:0)	 nop main.c(225:99:117:0:0)	 nop main.c(225:100:118:0:0)	 nop main.c(226:101:119:0:0)	 nop main.c(226:102:120:0:0)	 nop main.c(226:103:121:0:0)	 nop main.c(226:104:122:0:0)	 nop main.c(226:105:123:0:0)	 nop main.c(227:106:124:0:0)		if _sensorSample_phase3_1_1 [k19 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{bit bit {0,1}} == 0 goto _whilecontinue_1($24)
main.c(228:107:128:0:0)		_GO_DONE [k25 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{bit bit {0,1}} := 0x1 {literal bit {0,1}}
main.c(231:109:130:1:0)	 _whilecontinue_1($24) :
main.c(231:110:131:1:0)		if _NOT_DONE [k30 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{bit bit {0,1}} != 0 goto _whilecontinue_1($24)
main.c(234:111:136:0:0)		_TRISB [k10 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{sfr volatile unsigned char} := 0xf0 {literal unsigned char}
main.c(236:112:137:0:0)		iTemp15 [k31 lr112:114 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sensorSample_h_1_1}[r0x6A ] := _ADRESH [k33 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{sfr volatile unsigned char}
main.c(237:113:138:0:0)		iTemp16 [k34 lr113:116 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sensorSample_l_1_1}[r0x6B ] := _ADRESL [k36 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{sfr volatile unsigned char}
main.c(238:114:139:0:0)		iTemp17 [k37 lr114:115 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int}[r0x6A r0x6C ] = (int)iTemp15 [k31 lr112:114 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sensorSample_h_1_1}[r0x6A ]
main.c(238:115:140:0:0)		iTemp18 [k38 lr115:117 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int}[r0x6A r0x6C ] = iTemp17 [k37 lr114:115 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int}[r0x6A r0x6C ] << 0x8 {literal unsigned char}
main.c(238:116:142:0:0)		iTemp20 [k40 lr116:117 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{unsigned int}[r0x6B r0x6D ] = (unsigned int)iTemp16 [k34 lr113:116 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sensorSample_l_1_1}[r0x6B ]
main.c(238:117:143:0:0)		iTemp21 [k41 lr117:118 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{unsigned int}[r0x6A r0x6C ] = iTemp18 [k38 lr115:117 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int}[r0x6A r0x6C ] | iTemp20 [k40 lr116:117 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{unsigned int}[r0x6B r0x6D ]
main.c(238:118:145:0:0)		ret iTemp21 [k41 lr117:118 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{unsigned int}[r0x6A r0x6C ]
main.c(238:119:146:0:0)	 _return($27) :
main.c(238:120:147:0:0)		eproc _sensorSample [k1 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int function    }

----------------------------------------------------------------
Basic Block _eBBlock2 : loop Depth = 0 noPath = 0 , lastinLoop = 0
depth 1st num 8 : bbnum = 7 1st iCode = 26 , last iCode = 27
visited 0 : hasFcall = 0

defines bitVector :bitvector Size = 151 bSize = 19
Bits on { (31) }

local defines bitVector :bitvector Size = 151 bSize = 19
Bits on { (31) }

pointers Set bitvector :
in pointers Set bitvector :
inDefs Set bitvector :bitvector Size = 151 bSize = 19
Bits on { (2) (3) (4) (5) (6) (7) (8) (9) (13) (16) (18) (22) (25) (27) }

outDefs Set bitvector :bitvector Size = 151 bSize = 19
Bits on { (2) (3) (4) (5) (6) (7) (8) (9) (13) (16) (18) (22) (25) (27) (31) }

usesDefs Set bitvector :bitvector Size = 151 bSize = 19
Bits on { }

----------------------------------------------------------------
main.c(197:26:31:0:0)		_sensorSample_phase2_1_1 [k17 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{bit bit {0,1}} := 0x1 {literal bit {0,1}}
main.c(197:27:32:0:0)		 goto _ifend_4($9)
main.c(197:28:33:0:0)	 _iffalse_4($8) :
main.c(197:29:34:0:0)		_sensorSample_phase2_1_1 [k17 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{bit bit {0,1}} := 0x0 {literal bit {0,1}}
main.c(197:30:35:0:0)	 _ifend_4($9) :
main.c(198:31:36:0:0)		iTemp7 [k18 lr31:32 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{char} = iTemp2 [k6 lr5:31 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sensorSample_phase_1_1}[r0x67 ] == 0x3 {literal unsigned char}
main.c(198:32:37:0:0)		if iTemp7 [k18 lr31:32 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{char} == 0 goto _iffalse_5($11)
main.c(198:33:40:0:0)		_sensorSample_phase3_1_1 [k19 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{bit bit {0,1}} := 0x1 {literal bit {0,1}}
main.c(198:34:41:0:0)		 goto _ifend_5($12)
main.c(198:35:42:0:0)	 _iffalse_5($11) :
main.c(198:36:43:0:0)		_sensorSample_phase3_1_1 [k19 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{bit bit {0,1}} := 0x0 {literal bit {0,1}}
main.c(198:37:44:0:0)	 _ifend_5($12) :
main.c(204:38:45:0:0)		iTemp8 [k20 lr38:40 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sensorSample_halfcycles_1_1}[r0x68 ] := _numResonationHalfcycles [k22 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{unsigned char}
main.c(205:40:150:0:0)		iTemp10 [k24 lr40:65 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sensorSample_halfcycles_1_1}[r0x69 ] := iTemp8 [k20 lr38:40 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sensorSample_halfcycles_1_1}[r0x68 ]
main.c(205:41:46:1:0)	 _dobody_1($13) :
main.c(206:42:47:1:0)		_PORTB [k8 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{sfr volatile unsigned char} = _PORTB [k8 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{sfr volatile unsigned char} ^ iTemp0 [k2 lr3:65 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sensorSample_transmitter_1_1}[r0x65 ]
main.c(207:44:49:1:0)	 nop main.c(207:45:50:1:0)	 nop main.c(207:46:51:1:0)	 nop main.c(208:47:52:1:0)	 nop main.c(208:48:53:1:0)	 nop main.c(208:49:54:1:0)	 nop main.c(208:50:55:1:0)	 nop main.c(208:51:56:1:0)	 nop main.c(209:52:57:1:0)	 nop main.c(209:53:58:1:0)	 nop main.c(209:54:59:1:0)	 nop main.c(209:55:60:1:0)	 nop main.c(209:56:61:1:0)	 nop main.c(210:57:62:1:0)	 nop main.c(210:58:63:1:0)	 nop main.c(210:59:64:1:0)	 nop main.c(210:60:65:1:0)	 nop main.c(210:61:66:1:0)	 nop main.c(211:62:67:1:0)	 nop main.c(211:63:68:1:0)	 nop main.c(212:64:70:1:0)		iTemp10 [k24 lr40:65 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sensorSample_halfcycles_1_1}[r0x69 ] = iTemp10 [k24 lr40:65 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sensorSample_halfcycles_1_1}[r0x69 ] - 0x1 {literal unsigned char}
main.c(212:65:72:1:0)		if iTemp10 [k24 lr40:65 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sensorSample_halfcycles_1_1}[r0x69 ] != 0 goto _dobody_1($13)
main.c(215:66:75:0:0)		if _sensorSample_phase0_1_1 [k13 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{bit bit {0,1}} == 0 goto _iffalse_6($17)
main.c(216:67:79:0:0)		_GO_DONE [k25 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{bit bit {0,1}} := 0x1 {literal bit {0,1}}
main.c(216:68:80:0:0)	 _iffalse_6($17) :
main.c(217:69:81:0:0)	 nop main.c(217:70:82:0:0)	 nop main.c(217:71:83:0:0)	 nop main.c(217:72:84:0:0)	 nop main.c(217:73:85:0:0)	 nop main.c(218:74:86:0:0)	 nop main.c(218:75:87:0:0)	 nop main.c(218:76:88:0:0)	 nop main.c(218:77:89:0:0)	 nop main.c(218:78:90:0:0)	 nop main.c(219:79:91:0:0)		if _sensorSample_phase1_1_1 [k15 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{bit bit {0,1}} == 0 goto _iffalse_7($19)
main.c(220:80:95:0:0)		_GO_DONE [k25 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{bit bit {0,1}} := 0x1 {literal bit {0,1}}
main.c(220:81:96:0:0)	 _iffalse_7($19) :
main.c(221:82:97:0:0)	 nop main.c(221:83:98:0:0)	 nop main.c(221:84:99:0:0)	 nop main.c(221:85:100:0:0)	 nop main.c(221:86:101:0:0)	 nop main.c(222:87:102:0:0)	 nop main.c(222:88:103:0:0)	 nop main.c(222:89:104:0:0)	 nop main.c(222:90:105:0:0)	 nop main.c(222:91:106:0:0)	 nop main.c(222:92:107:0:0)	 nop main.c(223:93:108:0:0)		if _sensorSample_phase2_1_1 [k17 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{bit bit {0,1}} == 0 goto _iffalse_8($21)
main.c(224:94:112:0:0)		_GO_DONE [k25 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{bit bit {0,1}} := 0x1 {literal bit {0,1}}
main.c(224:95:113:0:0)	 _iffalse_8($21) :
main.c(225:96:114:0:0)	 nop main.c(225:97:115:0:0)	 nop main.c(225:98:116:0:0)	 nop main.c(225:99:117:0:0)	 nop main.c(225:100:118:0:0)	 nop main.c(226:101:119:0:0)	 nop main.c(226:102:120:0:0)	 nop main.c(226:103:121:0:0)	 nop main.c(226:104:122:0:0)	 nop main.c(226:105:123:0:0)	 nop main.c(227:106:124:0:0)		if _sensorSample_phase3_1_1 [k19 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{bit bit {0,1}} == 0 goto _whilecontinue_1($24)
main.c(228:107:128:0:0)		_GO_DONE [k25 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{bit bit {0,1}} := 0x1 {literal bit {0,1}}
main.c(231:109:130:1:0)	 _whilecontinue_1($24) :
main.c(231:110:131:1:0)		if _NOT_DONE [k30 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{bit bit {0,1}} != 0 goto _whilecontinue_1($24)
main.c(234:111:136:0:0)		_TRISB [k10 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{sfr volatile unsigned char} := 0xf0 {literal unsigned char}
main.c(236:112:137:0:0)		iTemp15 [k31 lr112:114 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sensorSample_h_1_1}[r0x6A ] := _ADRESH [k33 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{sfr volatile unsigned char}
main.c(237:113:138:0:0)		iTemp16 [k34 lr113:116 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sensorSample_l_1_1}[r0x6B ] := _ADRESL [k36 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{sfr volatile unsigned char}
main.c(238:114:139:0:0)		iTemp17 [k37 lr114:115 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int}[r0x6A r0x6C ] = (int)iTemp15 [k31 lr112:114 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sensorSample_h_1_1}[r0x6A ]
main.c(238:115:140:0:0)		iTemp18 [k38 lr115:117 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int}[r0x6A r0x6C ] = iTemp17 [k37 lr114:115 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int}[r0x6A r0x6C ] << 0x8 {literal unsigned char}
main.c(238:116:142:0:0)		iTemp20 [k40 lr116:117 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{unsigned int}[r0x6B r0x6D ] = (unsigned int)iTemp16 [k34 lr113:116 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sensorSample_l_1_1}[r0x6B ]
main.c(238:117:143:0:0)		iTemp21 [k41 lr117:118 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{unsigned int}[r0x6A r0x6C ] = iTemp18 [k38 lr115:117 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int}[r0x6A r0x6C ] | iTemp20 [k40 lr116:117 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{unsigned int}[r0x6B r0x6D ]
main.c(238:118:145:0:0)		ret iTemp21 [k41 lr117:118 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{unsigned int}[r0x6A r0x6C ]
main.c(238:119:146:0:0)	 _return($27) :
main.c(238:120:147:0:0)		eproc _sensorSample [k1 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int function    }

----------------------------------------------------------------
Basic Block _iffalse_4 : loop Depth = 0 noPath = 0 , lastinLoop = 0
depth 1st num 9 : bbnum = 8 1st iCode = 28 , last iCode = 29
visited 0 : hasFcall = 0

defines bitVector :bitvector Size = 151 bSize = 19
Bits on { (34) }

local defines bitVector :bitvector Size = 151 bSize = 19
Bits on { (34) }

pointers Set bitvector :
in pointers Set bitvector :
inDefs Set bitvector :bitvector Size = 151 bSize = 19
Bits on { (2) (3) (4) (5) (6) (7) (8) (9) (13) (16) (18) (22) (25) (27) }

outDefs Set bitvector :bitvector Size = 151 bSize = 19
Bits on { (2) (3) (4) (5) (6) (7) (8) (9) (13) (16) (18) (22) (25) (27) (34) }

usesDefs Set bitvector :bitvector Size = 151 bSize = 19
Bits on { }

----------------------------------------------------------------
main.c(197:28:33:0:0)	 _iffalse_4($8) :
main.c(197:29:34:0:0)		_sensorSample_phase2_1_1 [k17 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{bit bit {0,1}} := 0x0 {literal bit {0,1}}
main.c(197:30:35:0:0)	 _ifend_4($9) :
main.c(198:31:36:0:0)		iTemp7 [k18 lr31:32 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{char} = iTemp2 [k6 lr5:31 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sensorSample_phase_1_1}[r0x67 ] == 0x3 {literal unsigned char}
main.c(198:32:37:0:0)		if iTemp7 [k18 lr31:32 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{char} == 0 goto _iffalse_5($11)
main.c(198:33:40:0:0)		_sensorSample_phase3_1_1 [k19 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{bit bit {0,1}} := 0x1 {literal bit {0,1}}
main.c(198:34:41:0:0)		 goto _ifend_5($12)
main.c(198:35:42:0:0)	 _iffalse_5($11) :
main.c(198:36:43:0:0)		_sensorSample_phase3_1_1 [k19 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{bit bit {0,1}} := 0x0 {literal bit {0,1}}
main.c(198:37:44:0:0)	 _ifend_5($12) :
main.c(204:38:45:0:0)		iTemp8 [k20 lr38:40 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sensorSample_halfcycles_1_1}[r0x68 ] := _numResonationHalfcycles [k22 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{unsigned char}
main.c(205:40:150:0:0)		iTemp10 [k24 lr40:65 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sensorSample_halfcycles_1_1}[r0x69 ] := iTemp8 [k20 lr38:40 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sensorSample_halfcycles_1_1}[r0x68 ]
main.c(205:41:46:1:0)	 _dobody_1($13) :
main.c(206:42:47:1:0)		_PORTB [k8 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{sfr volatile unsigned char} = _PORTB [k8 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{sfr volatile unsigned char} ^ iTemp0 [k2 lr3:65 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sensorSample_transmitter_1_1}[r0x65 ]
main.c(207:44:49:1:0)	 nop main.c(207:45:50:1:0)	 nop main.c(207:46:51:1:0)	 nop main.c(208:47:52:1:0)	 nop main.c(208:48:53:1:0)	 nop main.c(208:49:54:1:0)	 nop main.c(208:50:55:1:0)	 nop main.c(208:51:56:1:0)	 nop main.c(209:52:57:1:0)	 nop main.c(209:53:58:1:0)	 nop main.c(209:54:59:1:0)	 nop main.c(209:55:60:1:0)	 nop main.c(209:56:61:1:0)	 nop main.c(210:57:62:1:0)	 nop main.c(210:58:63:1:0)	 nop main.c(210:59:64:1:0)	 nop main.c(210:60:65:1:0)	 nop main.c(210:61:66:1:0)	 nop main.c(211:62:67:1:0)	 nop main.c(211:63:68:1:0)	 nop main.c(212:64:70:1:0)		iTemp10 [k24 lr40:65 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sensorSample_halfcycles_1_1}[r0x69 ] = iTemp10 [k24 lr40:65 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sensorSample_halfcycles_1_1}[r0x69 ] - 0x1 {literal unsigned char}
main.c(212:65:72:1:0)		if iTemp10 [k24 lr40:65 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sensorSample_halfcycles_1_1}[r0x69 ] != 0 goto _dobody_1($13)
main.c(215:66:75:0:0)		if _sensorSample_phase0_1_1 [k13 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{bit bit {0,1}} == 0 goto _iffalse_6($17)
main.c(216:67:79:0:0)		_GO_DONE [k25 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{bit bit {0,1}} := 0x1 {literal bit {0,1}}
main.c(216:68:80:0:0)	 _iffalse_6($17) :
main.c(217:69:81:0:0)	 nop main.c(217:70:82:0:0)	 nop main.c(217:71:83:0:0)	 nop main.c(217:72:84:0:0)	 nop main.c(217:73:85:0:0)	 nop main.c(218:74:86:0:0)	 nop main.c(218:75:87:0:0)	 nop main.c(218:76:88:0:0)	 nop main.c(218:77:89:0:0)	 nop main.c(218:78:90:0:0)	 nop main.c(219:79:91:0:0)		if _sensorSample_phase1_1_1 [k15 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{bit bit {0,1}} == 0 goto _iffalse_7($19)
main.c(220:80:95:0:0)		_GO_DONE [k25 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{bit bit {0,1}} := 0x1 {literal bit {0,1}}
main.c(220:81:96:0:0)	 _iffalse_7($19) :
main.c(221:82:97:0:0)	 nop main.c(221:83:98:0:0)	 nop main.c(221:84:99:0:0)	 nop main.c(221:85:100:0:0)	 nop main.c(221:86:101:0:0)	 nop main.c(222:87:102:0:0)	 nop main.c(222:88:103:0:0)	 nop main.c(222:89:104:0:0)	 nop main.c(222:90:105:0:0)	 nop main.c(222:91:106:0:0)	 nop main.c(222:92:107:0:0)	 nop main.c(223:93:108:0:0)		if _sensorSample_phase2_1_1 [k17 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{bit bit {0,1}} == 0 goto _iffalse_8($21)
main.c(224:94:112:0:0)		_GO_DONE [k25 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{bit bit {0,1}} := 0x1 {literal bit {0,1}}
main.c(224:95:113:0:0)	 _iffalse_8($21) :
main.c(225:96:114:0:0)	 nop main.c(225:97:115:0:0)	 nop main.c(225:98:116:0:0)	 nop main.c(225:99:117:0:0)	 nop main.c(225:100:118:0:0)	 nop main.c(226:101:119:0:0)	 nop main.c(226:102:120:0:0)	 nop main.c(226:103:121:0:0)	 nop main.c(226:104:122:0:0)	 nop main.c(226:105:123:0:0)	 nop main.c(227:106:124:0:0)		if _sensorSample_phase3_1_1 [k19 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{bit bit {0,1}} == 0 goto _whilecontinue_1($24)
main.c(228:107:128:0:0)		_GO_DONE [k25 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{bit bit {0,1}} := 0x1 {literal bit {0,1}}
main.c(231:109:130:1:0)	 _whilecontinue_1($24) :
main.c(231:110:131:1:0)		if _NOT_DONE [k30 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{bit bit {0,1}} != 0 goto _whilecontinue_1($24)
main.c(234:111:136:0:0)		_TRISB [k10 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{sfr volatile unsigned char} := 0xf0 {literal unsigned char}
main.c(236:112:137:0:0)		iTemp15 [k31 lr112:114 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sensorSample_h_1_1}[r0x6A ] := _ADRESH [k33 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{sfr volatile unsigned char}
main.c(237:113:138:0:0)		iTemp16 [k34 lr113:116 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sensorSample_l_1_1}[r0x6B ] := _ADRESL [k36 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{sfr volatile unsigned char}
main.c(238:114:139:0:0)		iTemp17 [k37 lr114:115 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int}[r0x6A r0x6C ] = (int)iTemp15 [k31 lr112:114 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sensorSample_h_1_1}[r0x6A ]
main.c(238:115:140:0:0)		iTemp18 [k38 lr115:117 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int}[r0x6A r0x6C ] = iTemp17 [k37 lr114:115 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int}[r0x6A r0x6C ] << 0x8 {literal unsigned char}
main.c(238:116:142:0:0)		iTemp20 [k40 lr116:117 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{unsigned int}[r0x6B r0x6D ] = (unsigned int)iTemp16 [k34 lr113:116 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sensorSample_l_1_1}[r0x6B ]
main.c(238:117:143:0:0)		iTemp21 [k41 lr117:118 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{unsigned int}[r0x6A r0x6C ] = iTemp18 [k38 lr115:117 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int}[r0x6A r0x6C ] | iTemp20 [k40 lr116:117 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{unsigned int}[r0x6B r0x6D ]
main.c(238:118:145:0:0)		ret iTemp21 [k41 lr117:118 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{unsigned int}[r0x6A r0x6C ]
main.c(238:119:146:0:0)	 _return($27) :
main.c(238:120:147:0:0)		eproc _sensorSample [k1 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int function    }

----------------------------------------------------------------
Basic Block _ifend_4 : loop Depth = 0 noPath = 0 , lastinLoop = 0
depth 1st num 10 : bbnum = 9 1st iCode = 30 , last iCode = 32
visited 0 : hasFcall = 0

defines bitVector :bitvector Size = 151 bSize = 19
Bits on { (36) }

local defines bitVector :bitvector Size = 151 bSize = 19
Bits on { (36) }

pointers Set bitvector :
in pointers Set bitvector :
inDefs Set bitvector :bitvector Size = 151 bSize = 19
Bits on { (2) (3) (4) (5) (6) (7) (8) (9) (13) (16) (18) (22) (25) (27) (31) (34) }

outDefs Set bitvector :bitvector Size = 151 bSize = 19
Bits on { (2) (3) (4) (5) (6) (7) (8) (9) (13) (16) (18) (22) (25) (27) (31) (34) (36) }

usesDefs Set bitvector :bitvector Size = 151 bSize = 19
Bits on { (4) (36) }

----------------------------------------------------------------
main.c(197:30:35:0:0)	 _ifend_4($9) :
main.c(198:31:36:0:0)		iTemp7 [k18 lr31:32 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{char} = iTemp2 [k6 lr5:31 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sensorSample_phase_1_1}[r0x67 ] == 0x3 {literal unsigned char}
main.c(198:32:37:0:0)		if iTemp7 [k18 lr31:32 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{char} == 0 goto _iffalse_5($11)
main.c(198:33:40:0:0)		_sensorSample_phase3_1_1 [k19 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{bit bit {0,1}} := 0x1 {literal bit {0,1}}
main.c(198:34:41:0:0)		 goto _ifend_5($12)
main.c(198:35:42:0:0)	 _iffalse_5($11) :
main.c(198:36:43:0:0)		_sensorSample_phase3_1_1 [k19 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{bit bit {0,1}} := 0x0 {literal bit {0,1}}
main.c(198:37:44:0:0)	 _ifend_5($12) :
main.c(204:38:45:0:0)		iTemp8 [k20 lr38:40 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sensorSample_halfcycles_1_1}[r0x68 ] := _numResonationHalfcycles [k22 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{unsigned char}
main.c(205:40:150:0:0)		iTemp10 [k24 lr40:65 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sensorSample_halfcycles_1_1}[r0x69 ] := iTemp8 [k20 lr38:40 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sensorSample_halfcycles_1_1}[r0x68 ]
main.c(205:41:46:1:0)	 _dobody_1($13) :
main.c(206:42:47:1:0)		_PORTB [k8 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{sfr volatile unsigned char} = _PORTB [k8 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{sfr volatile unsigned char} ^ iTemp0 [k2 lr3:65 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sensorSample_transmitter_1_1}[r0x65 ]
main.c(207:44:49:1:0)	 nop main.c(207:45:50:1:0)	 nop main.c(207:46:51:1:0)	 nop main.c(208:47:52:1:0)	 nop main.c(208:48:53:1:0)	 nop main.c(208:49:54:1:0)	 nop main.c(208:50:55:1:0)	 nop main.c(208:51:56:1:0)	 nop main.c(209:52:57:1:0)	 nop main.c(209:53:58:1:0)	 nop main.c(209:54:59:1:0)	 nop main.c(209:55:60:1:0)	 nop main.c(209:56:61:1:0)	 nop main.c(210:57:62:1:0)	 nop main.c(210:58:63:1:0)	 nop main.c(210:59:64:1:0)	 nop main.c(210:60:65:1:0)	 nop main.c(210:61:66:1:0)	 nop main.c(211:62:67:1:0)	 nop main.c(211:63:68:1:0)	 nop main.c(212:64:70:1:0)		iTemp10 [k24 lr40:65 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sensorSample_halfcycles_1_1}[r0x69 ] = iTemp10 [k24 lr40:65 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sensorSample_halfcycles_1_1}[r0x69 ] - 0x1 {literal unsigned char}
main.c(212:65:72:1:0)		if iTemp10 [k24 lr40:65 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sensorSample_halfcycles_1_1}[r0x69 ] != 0 goto _dobody_1($13)
main.c(215:66:75:0:0)		if _sensorSample_phase0_1_1 [k13 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{bit bit {0,1}} == 0 goto _iffalse_6($17)
main.c(216:67:79:0:0)		_GO_DONE [k25 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{bit bit {0,1}} := 0x1 {literal bit {0,1}}
main.c(216:68:80:0:0)	 _iffalse_6($17) :
main.c(217:69:81:0:0)	 nop main.c(217:70:82:0:0)	 nop main.c(217:71:83:0:0)	 nop main.c(217:72:84:0:0)	 nop main.c(217:73:85:0:0)	 nop main.c(218:74:86:0:0)	 nop main.c(218:75:87:0:0)	 nop main.c(218:76:88:0:0)	 nop main.c(218:77:89:0:0)	 nop main.c(218:78:90:0:0)	 nop main.c(219:79:91:0:0)		if _sensorSample_phase1_1_1 [k15 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{bit bit {0,1}} == 0 goto _iffalse_7($19)
main.c(220:80:95:0:0)		_GO_DONE [k25 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{bit bit {0,1}} := 0x1 {literal bit {0,1}}
main.c(220:81:96:0:0)	 _iffalse_7($19) :
main.c(221:82:97:0:0)	 nop main.c(221:83:98:0:0)	 nop main.c(221:84:99:0:0)	 nop main.c(221:85:100:0:0)	 nop main.c(221:86:101:0:0)	 nop main.c(222:87:102:0:0)	 nop main.c(222:88:103:0:0)	 nop main.c(222:89:104:0:0)	 nop main.c(222:90:105:0:0)	 nop main.c(222:91:106:0:0)	 nop main.c(222:92:107:0:0)	 nop main.c(223:93:108:0:0)		if _sensorSample_phase2_1_1 [k17 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{bit bit {0,1}} == 0 goto _iffalse_8($21)
main.c(224:94:112:0:0)		_GO_DONE [k25 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{bit bit {0,1}} := 0x1 {literal bit {0,1}}
main.c(224:95:113:0:0)	 _iffalse_8($21) :
main.c(225:96:114:0:0)	 nop main.c(225:97:115:0:0)	 nop main.c(225:98:116:0:0)	 nop main.c(225:99:117:0:0)	 nop main.c(225:100:118:0:0)	 nop main.c(226:101:119:0:0)	 nop main.c(226:102:120:0:0)	 nop main.c(226:103:121:0:0)	 nop main.c(226:104:122:0:0)	 nop main.c(226:105:123:0:0)	 nop main.c(227:106:124:0:0)		if _sensorSample_phase3_1_1 [k19 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{bit bit {0,1}} == 0 goto _whilecontinue_1($24)
main.c(228:107:128:0:0)		_GO_DONE [k25 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{bit bit {0,1}} := 0x1 {literal bit {0,1}}
main.c(231:109:130:1:0)	 _whilecontinue_1($24) :
main.c(231:110:131:1:0)		if _NOT_DONE [k30 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{bit bit {0,1}} != 0 goto _whilecontinue_1($24)
main.c(234:111:136:0:0)		_TRISB [k10 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{sfr volatile unsigned char} := 0xf0 {literal unsigned char}
main.c(236:112:137:0:0)		iTemp15 [k31 lr112:114 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sensorSample_h_1_1}[r0x6A ] := _ADRESH [k33 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{sfr volatile unsigned char}
main.c(237:113:138:0:0)		iTemp16 [k34 lr113:116 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sensorSample_l_1_1}[r0x6B ] := _ADRESL [k36 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{sfr volatile unsigned char}
main.c(238:114:139:0:0)		iTemp17 [k37 lr114:115 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int}[r0x6A r0x6C ] = (int)iTemp15 [k31 lr112:114 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sensorSample_h_1_1}[r0x6A ]
main.c(238:115:140:0:0)		iTemp18 [k38 lr115:117 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int}[r0x6A r0x6C ] = iTemp17 [k37 lr114:115 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int}[r0x6A r0x6C ] << 0x8 {literal unsigned char}
main.c(238:116:142:0:0)		iTemp20 [k40 lr116:117 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{unsigned int}[r0x6B r0x6D ] = (unsigned int)iTemp16 [k34 lr113:116 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sensorSample_l_1_1}[r0x6B ]
main.c(238:117:143:0:0)		iTemp21 [k41 lr117:118 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{unsigned int}[r0x6A r0x6C ] = iTemp18 [k38 lr115:117 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int}[r0x6A r0x6C ] | iTemp20 [k40 lr116:117 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{unsigned int}[r0x6B r0x6D ]
main.c(238:118:145:0:0)		ret iTemp21 [k41 lr117:118 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{unsigned int}[r0x6A r0x6C ]
main.c(238:119:146:0:0)	 _return($27) :
main.c(238:120:147:0:0)		eproc _sensorSample [k1 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int function    }

----------------------------------------------------------------
Basic Block _eBBlock3 : loop Depth = 0 noPath = 0 , lastinLoop = 0
depth 1st num 11 : bbnum = 10 1st iCode = 33 , last iCode = 34
visited 0 : hasFcall = 0

defines bitVector :bitvector Size = 151 bSize = 19
Bits on { (40) }

local defines bitVector :bitvector Size = 151 bSize = 19
Bits on { (40) }

pointers Set bitvector :
in pointers Set bitvector :
inDefs Set bitvector :bitvector Size = 151 bSize = 19
Bits on { (2) (3) (4) (5) (6) (7) (8) (9) (13) (16) (18) (22) (25) (27) (31) (34) (36) }

outDefs Set bitvector :bitvector Size = 151 bSize = 19
Bits on { (2) (3) (4) (5) (6) (7) (8) (9) (13) (16) (18) (22) (25) (27) (31) (34) (36) (40) }

usesDefs Set bitvector :bitvector Size = 151 bSize = 19
Bits on { }

----------------------------------------------------------------
main.c(198:33:40:0:0)		_sensorSample_phase3_1_1 [k19 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{bit bit {0,1}} := 0x1 {literal bit {0,1}}
main.c(198:34:41:0:0)		 goto _ifend_5($12)
main.c(198:35:42:0:0)	 _iffalse_5($11) :
main.c(198:36:43:0:0)		_sensorSample_phase3_1_1 [k19 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{bit bit {0,1}} := 0x0 {literal bit {0,1}}
main.c(198:37:44:0:0)	 _ifend_5($12) :
main.c(204:38:45:0:0)		iTemp8 [k20 lr38:40 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sensorSample_halfcycles_1_1}[r0x68 ] := _numResonationHalfcycles [k22 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{unsigned char}
main.c(205:40:150:0:0)		iTemp10 [k24 lr40:65 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sensorSample_halfcycles_1_1}[r0x69 ] := iTemp8 [k20 lr38:40 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sensorSample_halfcycles_1_1}[r0x68 ]
main.c(205:41:46:1:0)	 _dobody_1($13) :
main.c(206:42:47:1:0)		_PORTB [k8 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{sfr volatile unsigned char} = _PORTB [k8 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{sfr volatile unsigned char} ^ iTemp0 [k2 lr3:65 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sensorSample_transmitter_1_1}[r0x65 ]
main.c(207:44:49:1:0)	 nop main.c(207:45:50:1:0)	 nop main.c(207:46:51:1:0)	 nop main.c(208:47:52:1:0)	 nop main.c(208:48:53:1:0)	 nop main.c(208:49:54:1:0)	 nop main.c(208:50:55:1:0)	 nop main.c(208:51:56:1:0)	 nop main.c(209:52:57:1:0)	 nop main.c(209:53:58:1:0)	 nop main.c(209:54:59:1:0)	 nop main.c(209:55:60:1:0)	 nop main.c(209:56:61:1:0)	 nop main.c(210:57:62:1:0)	 nop main.c(210:58:63:1:0)	 nop main.c(210:59:64:1:0)	 nop main.c(210:60:65:1:0)	 nop main.c(210:61:66:1:0)	 nop main.c(211:62:67:1:0)	 nop main.c(211:63:68:1:0)	 nop main.c(212:64:70:1:0)		iTemp10 [k24 lr40:65 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sensorSample_halfcycles_1_1}[r0x69 ] = iTemp10 [k24 lr40:65 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sensorSample_halfcycles_1_1}[r0x69 ] - 0x1 {literal unsigned char}
main.c(212:65:72:1:0)		if iTemp10 [k24 lr40:65 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sensorSample_halfcycles_1_1}[r0x69 ] != 0 goto _dobody_1($13)
main.c(215:66:75:0:0)		if _sensorSample_phase0_1_1 [k13 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{bit bit {0,1}} == 0 goto _iffalse_6($17)
main.c(216:67:79:0:0)		_GO_DONE [k25 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{bit bit {0,1}} := 0x1 {literal bit {0,1}}
main.c(216:68:80:0:0)	 _iffalse_6($17) :
main.c(217:69:81:0:0)	 nop main.c(217:70:82:0:0)	 nop main.c(217:71:83:0:0)	 nop main.c(217:72:84:0:0)	 nop main.c(217:73:85:0:0)	 nop main.c(218:74:86:0:0)	 nop main.c(218:75:87:0:0)	 nop main.c(218:76:88:0:0)	 nop main.c(218:77:89:0:0)	 nop main.c(218:78:90:0:0)	 nop main.c(219:79:91:0:0)		if _sensorSample_phase1_1_1 [k15 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{bit bit {0,1}} == 0 goto _iffalse_7($19)
main.c(220:80:95:0:0)		_GO_DONE [k25 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{bit bit {0,1}} := 0x1 {literal bit {0,1}}
main.c(220:81:96:0:0)	 _iffalse_7($19) :
main.c(221:82:97:0:0)	 nop main.c(221:83:98:0:0)	 nop main.c(221:84:99:0:0)	 nop main.c(221:85:100:0:0)	 nop main.c(221:86:101:0:0)	 nop main.c(222:87:102:0:0)	 nop main.c(222:88:103:0:0)	 nop main.c(222:89:104:0:0)	 nop main.c(222:90:105:0:0)	 nop main.c(222:91:106:0:0)	 nop main.c(222:92:107:0:0)	 nop main.c(223:93:108:0:0)		if _sensorSample_phase2_1_1 [k17 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{bit bit {0,1}} == 0 goto _iffalse_8($21)
main.c(224:94:112:0:0)		_GO_DONE [k25 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{bit bit {0,1}} := 0x1 {literal bit {0,1}}
main.c(224:95:113:0:0)	 _iffalse_8($21) :
main.c(225:96:114:0:0)	 nop main.c(225:97:115:0:0)	 nop main.c(225:98:116:0:0)	 nop main.c(225:99:117:0:0)	 nop main.c(225:100:118:0:0)	 nop main.c(226:101:119:0:0)	 nop main.c(226:102:120:0:0)	 nop main.c(226:103:121:0:0)	 nop main.c(226:104:122:0:0)	 nop main.c(226:105:123:0:0)	 nop main.c(227:106:124:0:0)		if _sensorSample_phase3_1_1 [k19 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{bit bit {0,1}} == 0 goto _whilecontinue_1($24)
main.c(228:107:128:0:0)		_GO_DONE [k25 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{bit bit {0,1}} := 0x1 {literal bit {0,1}}
main.c(231:109:130:1:0)	 _whilecontinue_1($24) :
main.c(231:110:131:1:0)		if _NOT_DONE [k30 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{bit bit {0,1}} != 0 goto _whilecontinue_1($24)
main.c(234:111:136:0:0)		_TRISB [k10 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{sfr volatile unsigned char} := 0xf0 {literal unsigned char}
main.c(236:112:137:0:0)		iTemp15 [k31 lr112:114 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sensorSample_h_1_1}[r0x6A ] := _ADRESH [k33 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{sfr volatile unsigned char}
main.c(237:113:138:0:0)		iTemp16 [k34 lr113:116 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sensorSample_l_1_1}[r0x6B ] := _ADRESL [k36 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{sfr volatile unsigned char}
main.c(238:114:139:0:0)		iTemp17 [k37 lr114:115 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int}[r0x6A r0x6C ] = (int)iTemp15 [k31 lr112:114 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sensorSample_h_1_1}[r0x6A ]
main.c(238:115:140:0:0)		iTemp18 [k38 lr115:117 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int}[r0x6A r0x6C ] = iTemp17 [k37 lr114:115 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int}[r0x6A r0x6C ] << 0x8 {literal unsigned char}
main.c(238:116:142:0:0)		iTemp20 [k40 lr116:117 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{unsigned int}[r0x6B r0x6D ] = (unsigned int)iTemp16 [k34 lr113:116 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sensorSample_l_1_1}[r0x6B ]
main.c(238:117:143:0:0)		iTemp21 [k41 lr117:118 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{unsigned int}[r0x6A r0x6C ] = iTemp18 [k38 lr115:117 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int}[r0x6A r0x6C ] | iTemp20 [k40 lr116:117 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{unsigned int}[r0x6B r0x6D ]
main.c(238:118:145:0:0)		ret iTemp21 [k41 lr117:118 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{unsigned int}[r0x6A r0x6C ]
main.c(238:119:146:0:0)	 _return($27) :
main.c(238:120:147:0:0)		eproc _sensorSample [k1 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int function    }

----------------------------------------------------------------
Basic Block _iffalse_5 : loop Depth = 0 noPath = 0 , lastinLoop = 0
depth 1st num 12 : bbnum = 11 1st iCode = 35 , last iCode = 36
visited 0 : hasFcall = 0

defines bitVector :bitvector Size = 151 bSize = 19
Bits on { (43) }

local defines bitVector :bitvector Size = 151 bSize = 19
Bits on { (43) }

pointers Set bitvector :
in pointers Set bitvector :
inDefs Set bitvector :bitvector Size = 151 bSize = 19
Bits on { (2) (3) (4) (5) (6) (7) (8) (9) (13) (16) (18) (22) (25) (27) (31) (34) (36) }

outDefs Set bitvector :bitvector Size = 151 bSize = 19
Bits on { (2) (3) (4) (5) (6) (7) (8) (9) (13) (16) (18) (22) (25) (27) (31) (34) (36) (43) }

usesDefs Set bitvector :bitvector Size = 151 bSize = 19
Bits on { }

----------------------------------------------------------------
main.c(198:35:42:0:0)	 _iffalse_5($11) :
main.c(198:36:43:0:0)		_sensorSample_phase3_1_1 [k19 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{bit bit {0,1}} := 0x0 {literal bit {0,1}}
main.c(198:37:44:0:0)	 _ifend_5($12) :
main.c(204:38:45:0:0)		iTemp8 [k20 lr38:40 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sensorSample_halfcycles_1_1}[r0x68 ] := _numResonationHalfcycles [k22 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{unsigned char}
main.c(205:40:150:0:0)		iTemp10 [k24 lr40:65 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sensorSample_halfcycles_1_1}[r0x69 ] := iTemp8 [k20 lr38:40 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sensorSample_halfcycles_1_1}[r0x68 ]
main.c(205:41:46:1:0)	 _dobody_1($13) :
main.c(206:42:47:1:0)		_PORTB [k8 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{sfr volatile unsigned char} = _PORTB [k8 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{sfr volatile unsigned char} ^ iTemp0 [k2 lr3:65 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sensorSample_transmitter_1_1}[r0x65 ]
main.c(207:44:49:1:0)	 nop main.c(207:45:50:1:0)	 nop main.c(207:46:51:1:0)	 nop main.c(208:47:52:1:0)	 nop main.c(208:48:53:1:0)	 nop main.c(208:49:54:1:0)	 nop main.c(208:50:55:1:0)	 nop main.c(208:51:56:1:0)	 nop main.c(209:52:57:1:0)	 nop main.c(209:53:58:1:0)	 nop main.c(209:54:59:1:0)	 nop main.c(209:55:60:1:0)	 nop main.c(209:56:61:1:0)	 nop main.c(210:57:62:1:0)	 nop main.c(210:58:63:1:0)	 nop main.c(210:59:64:1:0)	 nop main.c(210:60:65:1:0)	 nop main.c(210:61:66:1:0)	 nop main.c(211:62:67:1:0)	 nop main.c(211:63:68:1:0)	 nop main.c(212:64:70:1:0)		iTemp10 [k24 lr40:65 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sensorSample_halfcycles_1_1}[r0x69 ] = iTemp10 [k24 lr40:65 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sensorSample_halfcycles_1_1}[r0x69 ] - 0x1 {literal unsigned char}
main.c(212:65:72:1:0)		if iTemp10 [k24 lr40:65 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sensorSample_halfcycles_1_1}[r0x69 ] != 0 goto _dobody_1($13)
main.c(215:66:75:0:0)		if _sensorSample_phase0_1_1 [k13 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{bit bit {0,1}} == 0 goto _iffalse_6($17)
main.c(216:67:79:0:0)		_GO_DONE [k25 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{bit bit {0,1}} := 0x1 {literal bit {0,1}}
main.c(216:68:80:0:0)	 _iffalse_6($17) :
main.c(217:69:81:0:0)	 nop main.c(217:70:82:0:0)	 nop main.c(217:71:83:0:0)	 nop main.c(217:72:84:0:0)	 nop main.c(217:73:85:0:0)	 nop main.c(218:74:86:0:0)	 nop main.c(218:75:87:0:0)	 nop main.c(218:76:88:0:0)	 nop main.c(218:77:89:0:0)	 nop main.c(218:78:90:0:0)	 nop main.c(219:79:91:0:0)		if _sensorSample_phase1_1_1 [k15 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{bit bit {0,1}} == 0 goto _iffalse_7($19)
main.c(220:80:95:0:0)		_GO_DONE [k25 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{bit bit {0,1}} := 0x1 {literal bit {0,1}}
main.c(220:81:96:0:0)	 _iffalse_7($19) :
main.c(221:82:97:0:0)	 nop main.c(221:83:98:0:0)	 nop main.c(221:84:99:0:0)	 nop main.c(221:85:100:0:0)	 nop main.c(221:86:101:0:0)	 nop main.c(222:87:102:0:0)	 nop main.c(222:88:103:0:0)	 nop main.c(222:89:104:0:0)	 nop main.c(222:90:105:0:0)	 nop main.c(222:91:106:0:0)	 nop main.c(222:92:107:0:0)	 nop main.c(223:93:108:0:0)		if _sensorSample_phase2_1_1 [k17 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{bit bit {0,1}} == 0 goto _iffalse_8($21)
main.c(224:94:112:0:0)		_GO_DONE [k25 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{bit bit {0,1}} := 0x1 {literal bit {0,1}}
main.c(224:95:113:0:0)	 _iffalse_8($21) :
main.c(225:96:114:0:0)	 nop main.c(225:97:115:0:0)	 nop main.c(225:98:116:0:0)	 nop main.c(225:99:117:0:0)	 nop main.c(225:100:118:0:0)	 nop main.c(226:101:119:0:0)	 nop main.c(226:102:120:0:0)	 nop main.c(226:103:121:0:0)	 nop main.c(226:104:122:0:0)	 nop main.c(226:105:123:0:0)	 nop main.c(227:106:124:0:0)		if _sensorSample_phase3_1_1 [k19 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{bit bit {0,1}} == 0 goto _whilecontinue_1($24)
main.c(228:107:128:0:0)		_GO_DONE [k25 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{bit bit {0,1}} := 0x1 {literal bit {0,1}}
main.c(231:109:130:1:0)	 _whilecontinue_1($24) :
main.c(231:110:131:1:0)		if _NOT_DONE [k30 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{bit bit {0,1}} != 0 goto _whilecontinue_1($24)
main.c(234:111:136:0:0)		_TRISB [k10 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{sfr volatile unsigned char} := 0xf0 {literal unsigned char}
main.c(236:112:137:0:0)		iTemp15 [k31 lr112:114 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sensorSample_h_1_1}[r0x6A ] := _ADRESH [k33 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{sfr volatile unsigned char}
main.c(237:113:138:0:0)		iTemp16 [k34 lr113:116 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sensorSample_l_1_1}[r0x6B ] := _ADRESL [k36 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{sfr volatile unsigned char}
main.c(238:114:139:0:0)		iTemp17 [k37 lr114:115 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int}[r0x6A r0x6C ] = (int)iTemp15 [k31 lr112:114 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sensorSample_h_1_1}[r0x6A ]
main.c(238:115:140:0:0)		iTemp18 [k38 lr115:117 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int}[r0x6A r0x6C ] = iTemp17 [k37 lr114:115 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int}[r0x6A r0x6C ] << 0x8 {literal unsigned char}
main.c(238:116:142:0:0)		iTemp20 [k40 lr116:117 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{unsigned int}[r0x6B r0x6D ] = (unsigned int)iTemp16 [k34 lr113:116 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sensorSample_l_1_1}[r0x6B ]
main.c(238:117:143:0:0)		iTemp21 [k41 lr117:118 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{unsigned int}[r0x6A r0x6C ] = iTemp18 [k38 lr115:117 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int}[r0x6A r0x6C ] | iTemp20 [k40 lr116:117 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{unsigned int}[r0x6B r0x6D ]
main.c(238:118:145:0:0)		ret iTemp21 [k41 lr117:118 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{unsigned int}[r0x6A r0x6C ]
main.c(238:119:146:0:0)	 _return($27) :
main.c(238:120:147:0:0)		eproc _sensorSample [k1 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int function    }

----------------------------------------------------------------
Basic Block _ifend_5 : loop Depth = 0 noPath = 0 , lastinLoop = 0
depth 1st num 13 : bbnum = 12 1st iCode = 37 , last iCode = 38
visited 0 : hasFcall = 0

defines bitVector :bitvector Size = 151 bSize = 19
Bits on { (45) }

local defines bitVector :bitvector Size = 151 bSize = 19
Bits on { (45) }

pointers Set bitvector :
in pointers Set bitvector :
inDefs Set bitvector :bitvector Size = 151 bSize = 19
Bits on { (2) (3) (4) (5) (6) (7) (8) (9) (13) (16) (18) (22) (25) (27) (31) (34) (36) (40) (43) }

outDefs Set bitvector :bitvector Size = 151 bSize = 19
Bits on { (2) (3) (4) (5) (6) (7) (8) (9) (13) (16) (18) (22) (25) (27) (31) (34) (36) (40) (43) (45) }

usesDefs Set bitvector :bitvector Size = 151 bSize = 19
Bits on { }

----------------------------------------------------------------
main.c(198:37:44:0:0)	 _ifend_5($12) :
main.c(204:38:45:0:0)		iTemp8 [k20 lr38:40 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sensorSample_halfcycles_1_1}[r0x68 ] := _numResonationHalfcycles [k22 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{unsigned char}
main.c(205:40:150:0:0)		iTemp10 [k24 lr40:65 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sensorSample_halfcycles_1_1}[r0x69 ] := iTemp8 [k20 lr38:40 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sensorSample_halfcycles_1_1}[r0x68 ]
main.c(205:41:46:1:0)	 _dobody_1($13) :
main.c(206:42:47:1:0)		_PORTB [k8 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{sfr volatile unsigned char} = _PORTB [k8 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{sfr volatile unsigned char} ^ iTemp0 [k2 lr3:65 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sensorSample_transmitter_1_1}[r0x65 ]
main.c(207:44:49:1:0)	 nop main.c(207:45:50:1:0)	 nop main.c(207:46:51:1:0)	 nop main.c(208:47:52:1:0)	 nop main.c(208:48:53:1:0)	 nop main.c(208:49:54:1:0)	 nop main.c(208:50:55:1:0)	 nop main.c(208:51:56:1:0)	 nop main.c(209:52:57:1:0)	 nop main.c(209:53:58:1:0)	 nop main.c(209:54:59:1:0)	 nop main.c(209:55:60:1:0)	 nop main.c(209:56:61:1:0)	 nop main.c(210:57:62:1:0)	 nop main.c(210:58:63:1:0)	 nop main.c(210:59:64:1:0)	 nop main.c(210:60:65:1:0)	 nop main.c(210:61:66:1:0)	 nop main.c(211:62:67:1:0)	 nop main.c(211:63:68:1:0)	 nop main.c(212:64:70:1:0)		iTemp10 [k24 lr40:65 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sensorSample_halfcycles_1_1}[r0x69 ] = iTemp10 [k24 lr40:65 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sensorSample_halfcycles_1_1}[r0x69 ] - 0x1 {literal unsigned char}
main.c(212:65:72:1:0)		if iTemp10 [k24 lr40:65 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sensorSample_halfcycles_1_1}[r0x69 ] != 0 goto _dobody_1($13)
main.c(215:66:75:0:0)		if _sensorSample_phase0_1_1 [k13 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{bit bit {0,1}} == 0 goto _iffalse_6($17)
main.c(216:67:79:0:0)		_GO_DONE [k25 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{bit bit {0,1}} := 0x1 {literal bit {0,1}}
main.c(216:68:80:0:0)	 _iffalse_6($17) :
main.c(217:69:81:0:0)	 nop main.c(217:70:82:0:0)	 nop main.c(217:71:83:0:0)	 nop main.c(217:72:84:0:0)	 nop main.c(217:73:85:0:0)	 nop main.c(218:74:86:0:0)	 nop main.c(218:75:87:0:0)	 nop main.c(218:76:88:0:0)	 nop main.c(218:77:89:0:0)	 nop main.c(218:78:90:0:0)	 nop main.c(219:79:91:0:0)		if _sensorSample_phase1_1_1 [k15 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{bit bit {0,1}} == 0 goto _iffalse_7($19)
main.c(220:80:95:0:0)		_GO_DONE [k25 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{bit bit {0,1}} := 0x1 {literal bit {0,1}}
main.c(220:81:96:0:0)	 _iffalse_7($19) :
main.c(221:82:97:0:0)	 nop main.c(221:83:98:0:0)	 nop main.c(221:84:99:0:0)	 nop main.c(221:85:100:0:0)	 nop main.c(221:86:101:0:0)	 nop main.c(222:87:102:0:0)	 nop main.c(222:88:103:0:0)	 nop main.c(222:89:104:0:0)	 nop main.c(222:90:105:0:0)	 nop main.c(222:91:106:0:0)	 nop main.c(222:92:107:0:0)	 nop main.c(223:93:108:0:0)		if _sensorSample_phase2_1_1 [k17 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{bit bit {0,1}} == 0 goto _iffalse_8($21)
main.c(224:94:112:0:0)		_GO_DONE [k25 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{bit bit {0,1}} := 0x1 {literal bit {0,1}}
main.c(224:95:113:0:0)	 _iffalse_8($21) :
main.c(225:96:114:0:0)	 nop main.c(225:97:115:0:0)	 nop main.c(225:98:116:0:0)	 nop main.c(225:99:117:0:0)	 nop main.c(225:100:118:0:0)	 nop main.c(226:101:119:0:0)	 nop main.c(226:102:120:0:0)	 nop main.c(226:103:121:0:0)	 nop main.c(226:104:122:0:0)	 nop main.c(226:105:123:0:0)	 nop main.c(227:106:124:0:0)		if _sensorSample_phase3_1_1 [k19 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{bit bit {0,1}} == 0 goto _whilecontinue_1($24)
main.c(228:107:128:0:0)		_GO_DONE [k25 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{bit bit {0,1}} := 0x1 {literal bit {0,1}}
main.c(231:109:130:1:0)	 _whilecontinue_1($24) :
main.c(231:110:131:1:0)		if _NOT_DONE [k30 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{bit bit {0,1}} != 0 goto _whilecontinue_1($24)
main.c(234:111:136:0:0)		_TRISB [k10 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{sfr volatile unsigned char} := 0xf0 {literal unsigned char}
main.c(236:112:137:0:0)		iTemp15 [k31 lr112:114 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sensorSample_h_1_1}[r0x6A ] := _ADRESH [k33 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{sfr volatile unsigned char}
main.c(237:113:138:0:0)		iTemp16 [k34 lr113:116 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sensorSample_l_1_1}[r0x6B ] := _ADRESL [k36 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{sfr volatile unsigned char}
main.c(238:114:139:0:0)		iTemp17 [k37 lr114:115 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int}[r0x6A r0x6C ] = (int)iTemp15 [k31 lr112:114 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sensorSample_h_1_1}[r0x6A ]
main.c(238:115:140:0:0)		iTemp18 [k38 lr115:117 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int}[r0x6A r0x6C ] = iTemp17 [k37 lr114:115 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int}[r0x6A r0x6C ] << 0x8 {literal unsigned char}
main.c(238:116:142:0:0)		iTemp20 [k40 lr116:117 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{unsigned int}[r0x6B r0x6D ] = (unsigned int)iTemp16 [k34 lr113:116 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sensorSample_l_1_1}[r0x6B ]
main.c(238:117:143:0:0)		iTemp21 [k41 lr117:118 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{unsigned int}[r0x6A r0x6C ] = iTemp18 [k38 lr115:117 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int}[r0x6A r0x6C ] | iTemp20 [k40 lr116:117 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{unsigned int}[r0x6B r0x6D ]
main.c(238:118:145:0:0)		ret iTemp21 [k41 lr117:118 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{unsigned int}[r0x6A r0x6C ]
main.c(238:119:146:0:0)	 _return($27) :
main.c(238:120:147:0:0)		eproc _sensorSample [k1 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int function    }

----------------------------------------------------------------
Basic Block preHeaderLbl0 : loop Depth = 0 noPath = 0 , lastinLoop = 0
depth 1st num 14 : bbnum = 13 1st iCode = 39 , last iCode = 40
visited 0 : hasFcall = 0

defines bitVector :bitvector Size = 151 bSize = 19
Bits on { (150) }

local defines bitVector :bitvector Size = 151 bSize = 19
Bits on { (150) }

pointers Set bitvector :
in pointers Set bitvector :
inDefs Set bitvector :bitvector Size = 151 bSize = 19
Bits on { (2) (3) (4) (5) (6) (7) (8) (9) (13) (16) (18) (22) (25) (27) (31) (34) (36) (40) (43) (45) }

outDefs Set bitvector :bitvector Size = 151 bSize = 19
Bits on { (2) (3) (4) (5) (6) (7) (8) (9) (13) (16) (18) (22) (25) (27) (31) (34) (36) (40) (43) (45) (150) }

usesDefs Set bitvector :bitvector Size = 151 bSize = 19
Bits on { (45) }

----------------------------------------------------------------
main.c(205:39:148:0:0)	 preHeaderLbl0($33) :
main.c(205:40:150:0:0)		iTemp10 [k24 lr40:65 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sensorSample_halfcycles_1_1}[r0x69 ] := iTemp8 [k20 lr38:40 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sensorSample_halfcycles_1_1}[r0x68 ]
main.c(205:41:46:1:0)	 _dobody_1($13) :
main.c(206:42:47:1:0)		_PORTB [k8 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{sfr volatile unsigned char} = _PORTB [k8 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{sfr volatile unsigned char} ^ iTemp0 [k2 lr3:65 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sensorSample_transmitter_1_1}[r0x65 ]
main.c(207:44:49:1:0)	 nop main.c(207:45:50:1:0)	 nop main.c(207:46:51:1:0)	 nop main.c(208:47:52:1:0)	 nop main.c(208:48:53:1:0)	 nop main.c(208:49:54:1:0)	 nop main.c(208:50:55:1:0)	 nop main.c(208:51:56:1:0)	 nop main.c(209:52:57:1:0)	 nop main.c(209:53:58:1:0)	 nop main.c(209:54:59:1:0)	 nop main.c(209:55:60:1:0)	 nop main.c(209:56:61:1:0)	 nop main.c(210:57:62:1:0)	 nop main.c(210:58:63:1:0)	 nop main.c(210:59:64:1:0)	 nop main.c(210:60:65:1:0)	 nop main.c(210:61:66:1:0)	 nop main.c(211:62:67:1:0)	 nop main.c(211:63:68:1:0)	 nop main.c(212:64:70:1:0)		iTemp10 [k24 lr40:65 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sensorSample_halfcycles_1_1}[r0x69 ] = iTemp10 [k24 lr40:65 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sensorSample_halfcycles_1_1}[r0x69 ] - 0x1 {literal unsigned char}
main.c(212:65:72:1:0)		if iTemp10 [k24 lr40:65 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sensorSample_halfcycles_1_1}[r0x69 ] != 0 goto _dobody_1($13)
main.c(215:66:75:0:0)		if _sensorSample_phase0_1_1 [k13 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{bit bit {0,1}} == 0 goto _iffalse_6($17)
main.c(216:67:79:0:0)		_GO_DONE [k25 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{bit bit {0,1}} := 0x1 {literal bit {0,1}}
main.c(216:68:80:0:0)	 _iffalse_6($17) :
main.c(217:69:81:0:0)	 nop main.c(217:70:82:0:0)	 nop main.c(217:71:83:0:0)	 nop main.c(217:72:84:0:0)	 nop main.c(217:73:85:0:0)	 nop main.c(218:74:86:0:0)	 nop main.c(218:75:87:0:0)	 nop main.c(218:76:88:0:0)	 nop main.c(218:77:89:0:0)	 nop main.c(218:78:90:0:0)	 nop main.c(219:79:91:0:0)		if _sensorSample_phase1_1_1 [k15 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{bit bit {0,1}} == 0 goto _iffalse_7($19)
main.c(220:80:95:0:0)		_GO_DONE [k25 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{bit bit {0,1}} := 0x1 {literal bit {0,1}}
main.c(220:81:96:0:0)	 _iffalse_7($19) :
main.c(221:82:97:0:0)	 nop main.c(221:83:98:0:0)	 nop main.c(221:84:99:0:0)	 nop main.c(221:85:100:0:0)	 nop main.c(221:86:101:0:0)	 nop main.c(222:87:102:0:0)	 nop main.c(222:88:103:0:0)	 nop main.c(222:89:104:0:0)	 nop main.c(222:90:105:0:0)	 nop main.c(222:91:106:0:0)	 nop main.c(222:92:107:0:0)	 nop main.c(223:93:108:0:0)		if _sensorSample_phase2_1_1 [k17 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{bit bit {0,1}} == 0 goto _iffalse_8($21)
main.c(224:94:112:0:0)		_GO_DONE [k25 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{bit bit {0,1}} := 0x1 {literal bit {0,1}}
main.c(224:95:113:0:0)	 _iffalse_8($21) :
main.c(225:96:114:0:0)	 nop main.c(225:97:115:0:0)	 nop main.c(225:98:116:0:0)	 nop main.c(225:99:117:0:0)	 nop main.c(225:100:118:0:0)	 nop main.c(226:101:119:0:0)	 nop main.c(226:102:120:0:0)	 nop main.c(226:103:121:0:0)	 nop main.c(226:104:122:0:0)	 nop main.c(226:105:123:0:0)	 nop main.c(227:106:124:0:0)		if _sensorSample_phase3_1_1 [k19 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{bit bit {0,1}} == 0 goto _whilecontinue_1($24)
main.c(228:107:128:0:0)		_GO_DONE [k25 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{bit bit {0,1}} := 0x1 {literal bit {0,1}}
main.c(231:109:130:1:0)	 _whilecontinue_1($24) :
main.c(231:110:131:1:0)		if _NOT_DONE [k30 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{bit bit {0,1}} != 0 goto _whilecontinue_1($24)
main.c(234:111:136:0:0)		_TRISB [k10 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{sfr volatile unsigned char} := 0xf0 {literal unsigned char}
main.c(236:112:137:0:0)		iTemp15 [k31 lr112:114 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sensorSample_h_1_1}[r0x6A ] := _ADRESH [k33 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{sfr volatile unsigned char}
main.c(237:113:138:0:0)		iTemp16 [k34 lr113:116 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sensorSample_l_1_1}[r0x6B ] := _ADRESL [k36 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{sfr volatile unsigned char}
main.c(238:114:139:0:0)		iTemp17 [k37 lr114:115 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int}[r0x6A r0x6C ] = (int)iTemp15 [k31 lr112:114 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sensorSample_h_1_1}[r0x6A ]
main.c(238:115:140:0:0)		iTemp18 [k38 lr115:117 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int}[r0x6A r0x6C ] = iTemp17 [k37 lr114:115 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int}[r0x6A r0x6C ] << 0x8 {literal unsigned char}
main.c(238:116:142:0:0)		iTemp20 [k40 lr116:117 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{unsigned int}[r0x6B r0x6D ] = (unsigned int)iTemp16 [k34 lr113:116 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sensorSample_l_1_1}[r0x6B ]
main.c(238:117:143:0:0)		iTemp21 [k41 lr117:118 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{unsigned int}[r0x6A r0x6C ] = iTemp18 [k38 lr115:117 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int}[r0x6A r0x6C ] | iTemp20 [k40 lr116:117 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{unsigned int}[r0x6B r0x6D ]
main.c(238:118:145:0:0)		ret iTemp21 [k41 lr117:118 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{unsigned int}[r0x6A r0x6C ]
main.c(238:119:146:0:0)	 _return($27) :
main.c(238:120:147:0:0)		eproc _sensorSample [k1 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int function    }

----------------------------------------------------------------
Basic Block _dobody_1 : loop Depth = 1 noPath = 0 , lastinLoop = 1
depth 1st num 15 : bbnum = 14 1st iCode = 41 , last iCode = 65
visited 0 : hasFcall = 0

defines bitVector :bitvector Size = 151 bSize = 19
Bits on { (47) (48) (70) }

local defines bitVector :bitvector Size = 151 bSize = 19
Bits on { (47) (48) (70) (71) }

pointers Set bitvector :
in pointers Set bitvector :
inDefs Set bitvector :bitvector Size = 151 bSize = 19
Bits on { (2) (3) (4) (5) (6) (7) (8) (9) (13) (16) (18) (22) (25) (27) (31) (34) (36) (40) (43) (45) (47) (48) (70) (71) (150) }

outDefs Set bitvector :bitvector Size = 151 bSize = 19
Bits on { (2) (3) (4) (5) (7) (8) (9) (13) (16) (18) (22) (25) (27) (31) (34) (36) (40) (43) (47) (48) (70) }

usesDefs Set bitvector :bitvector Size = 151 bSize = 19
Bits on { (2) (6) (47) (48) (70) (150) }

----------------------------------------------------------------
main.c(205:41:46:1:0)	 _dobody_1($13) :
main.c(206:42:47:1:0)		_PORTB [k8 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{sfr volatile unsigned char} = _PORTB [k8 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{sfr volatile unsigned char} ^ iTemp0 [k2 lr3:65 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sensorSample_transmitter_1_1}[r0x65 ]
main.c(207:44:49:1:0)	 nop main.c(207:45:50:1:0)	 nop main.c(207:46:51:1:0)	 nop main.c(208:47:52:1:0)	 nop main.c(208:48:53:1:0)	 nop main.c(208:49:54:1:0)	 nop main.c(208:50:55:1:0)	 nop main.c(208:51:56:1:0)	 nop main.c(209:52:57:1:0)	 nop main.c(209:53:58:1:0)	 nop main.c(209:54:59:1:0)	 nop main.c(209:55:60:1:0)	 nop main.c(209:56:61:1:0)	 nop main.c(210:57:62:1:0)	 nop main.c(210:58:63:1:0)	 nop main.c(210:59:64:1:0)	 nop main.c(210:60:65:1:0)	 nop main.c(210:61:66:1:0)	 nop main.c(211:62:67:1:0)	 nop main.c(211:63:68:1:0)	 nop main.c(212:64:70:1:0)		iTemp10 [k24 lr40:65 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sensorSample_halfcycles_1_1}[r0x69 ] = iTemp10 [k24 lr40:65 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sensorSample_halfcycles_1_1}[r0x69 ] - 0x1 {literal unsigned char}
main.c(212:65:72:1:0)		if iTemp10 [k24 lr40:65 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sensorSample_halfcycles_1_1}[r0x69 ] != 0 goto _dobody_1($13)
main.c(215:66:75:0:0)		if _sensorSample_phase0_1_1 [k13 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{bit bit {0,1}} == 0 goto _iffalse_6($17)
main.c(216:67:79:0:0)		_GO_DONE [k25 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{bit bit {0,1}} := 0x1 {literal bit {0,1}}
main.c(216:68:80:0:0)	 _iffalse_6($17) :
main.c(217:69:81:0:0)	 nop main.c(217:70:82:0:0)	 nop main.c(217:71:83:0:0)	 nop main.c(217:72:84:0:0)	 nop main.c(217:73:85:0:0)	 nop main.c(218:74:86:0:0)	 nop main.c(218:75:87:0:0)	 nop main.c(218:76:88:0:0)	 nop main.c(218:77:89:0:0)	 nop main.c(218:78:90:0:0)	 nop main.c(219:79:91:0:0)		if _sensorSample_phase1_1_1 [k15 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{bit bit {0,1}} == 0 goto _iffalse_7($19)
main.c(220:80:95:0:0)		_GO_DONE [k25 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{bit bit {0,1}} := 0x1 {literal bit {0,1}}
main.c(220:81:96:0:0)	 _iffalse_7($19) :
main.c(221:82:97:0:0)	 nop main.c(221:83:98:0:0)	 nop main.c(221:84:99:0:0)	 nop main.c(221:85:100:0:0)	 nop main.c(221:86:101:0:0)	 nop main.c(222:87:102:0:0)	 nop main.c(222:88:103:0:0)	 nop main.c(222:89:104:0:0)	 nop main.c(222:90:105:0:0)	 nop main.c(222:91:106:0:0)	 nop main.c(222:92:107:0:0)	 nop main.c(223:93:108:0:0)		if _sensorSample_phase2_1_1 [k17 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{bit bit {0,1}} == 0 goto _iffalse_8($21)
main.c(224:94:112:0:0)		_GO_DONE [k25 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{bit bit {0,1}} := 0x1 {literal bit {0,1}}
main.c(224:95:113:0:0)	 _iffalse_8($21) :
main.c(225:96:114:0:0)	 nop main.c(225:97:115:0:0)	 nop main.c(225:98:116:0:0)	 nop main.c(225:99:117:0:0)	 nop main.c(225:100:118:0:0)	 nop main.c(226:101:119:0:0)	 nop main.c(226:102:120:0:0)	 nop main.c(226:103:121:0:0)	 nop main.c(226:104:122:0:0)	 nop main.c(226:105:123:0:0)	 nop main.c(227:106:124:0:0)		if _sensorSample_phase3_1_1 [k19 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{bit bit {0,1}} == 0 goto _whilecontinue_1($24)
main.c(228:107:128:0:0)		_GO_DONE [k25 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{bit bit {0,1}} := 0x1 {literal bit {0,1}}
main.c(231:109:130:1:0)	 _whilecontinue_1($24) :
main.c(231:110:131:1:0)		if _NOT_DONE [k30 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{bit bit {0,1}} != 0 goto _whilecontinue_1($24)
main.c(234:111:136:0:0)		_TRISB [k10 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{sfr volatile unsigned char} := 0xf0 {literal unsigned char}
main.c(236:112:137:0:0)		iTemp15 [k31 lr112:114 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sensorSample_h_1_1}[r0x6A ] := _ADRESH [k33 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{sfr volatile unsigned char}
main.c(237:113:138:0:0)		iTemp16 [k34 lr113:116 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sensorSample_l_1_1}[r0x6B ] := _ADRESL [k36 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{sfr volatile unsigned char}
main.c(238:114:139:0:0)		iTemp17 [k37 lr114:115 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int}[r0x6A r0x6C ] = (int)iTemp15 [k31 lr112:114 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sensorSample_h_1_1}[r0x6A ]
main.c(238:115:140:0:0)		iTemp18 [k38 lr115:117 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int}[r0x6A r0x6C ] = iTemp17 [k37 lr114:115 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int}[r0x6A r0x6C ] << 0x8 {literal unsigned char}
main.c(238:116:142:0:0)		iTemp20 [k40 lr116:117 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{unsigned int}[r0x6B r0x6D ] = (unsigned int)iTemp16 [k34 lr113:116 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sensorSample_l_1_1}[r0x6B ]
main.c(238:117:143:0:0)		iTemp21 [k41 lr117:118 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{unsigned int}[r0x6A r0x6C ] = iTemp18 [k38 lr115:117 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int}[r0x6A r0x6C ] | iTemp20 [k40 lr116:117 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{unsigned int}[r0x6B r0x6D ]
main.c(238:118:145:0:0)		ret iTemp21 [k41 lr117:118 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{unsigned int}[r0x6A r0x6C ]
main.c(238:119:146:0:0)	 _return($27) :
main.c(238:120:147:0:0)		eproc _sensorSample [k1 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int function    }

----------------------------------------------------------------
Basic Block _eBBlock4 : loop Depth = 0 noPath = 0 , lastinLoop = 0
depth 1st num 16 : bbnum = 15 1st iCode = 66 , last iCode = 66
visited 0 : hasFcall = 0

defines bitVector :bitvector Size = 151 bSize = 19
Bits on { }

local defines bitVector :
pointers Set bitvector :
in pointers Set bitvector :
inDefs Set bitvector :bitvector Size = 151 bSize = 19
Bits on { (2) (3) (4) (5) (7) (8) (9) (13) (16) (18) (22) (25) (27) (31) (34) (36) (40) (43) (47) (48) (70) (71) }

outDefs Set bitvector :bitvector Size = 151 bSize = 19
Bits on { (2) (3) (4) (5) (7) (8) (9) (13) (16) (18) (22) (25) (27) (31) (34) (36) (40) (43) (47) (48) (70) (71) }

usesDefs Set bitvector :bitvector Size = 151 bSize = 19
Bits on { (13) (16) }

----------------------------------------------------------------
main.c(215:66:75:0:0)		if _sensorSample_phase0_1_1 [k13 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{bit bit {0,1}} == 0 goto _iffalse_6($17)
main.c(216:67:79:0:0)		_GO_DONE [k25 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{bit bit {0,1}} := 0x1 {literal bit {0,1}}
main.c(216:68:80:0:0)	 _iffalse_6($17) :
main.c(217:69:81:0:0)	 nop main.c(217:70:82:0:0)	 nop main.c(217:71:83:0:0)	 nop main.c(217:72:84:0:0)	 nop main.c(217:73:85:0:0)	 nop main.c(218:74:86:0:0)	 nop main.c(218:75:87:0:0)	 nop main.c(218:76:88:0:0)	 nop main.c(218:77:89:0:0)	 nop main.c(218:78:90:0:0)	 nop main.c(219:79:91:0:0)		if _sensorSample_phase1_1_1 [k15 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{bit bit {0,1}} == 0 goto _iffalse_7($19)
main.c(220:80:95:0:0)		_GO_DONE [k25 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{bit bit {0,1}} := 0x1 {literal bit {0,1}}
main.c(220:81:96:0:0)	 _iffalse_7($19) :
main.c(221:82:97:0:0)	 nop main.c(221:83:98:0:0)	 nop main.c(221:84:99:0:0)	 nop main.c(221:85:100:0:0)	 nop main.c(221:86:101:0:0)	 nop main.c(222:87:102:0:0)	 nop main.c(222:88:103:0:0)	 nop main.c(222:89:104:0:0)	 nop main.c(222:90:105:0:0)	 nop main.c(222:91:106:0:0)	 nop main.c(222:92:107:0:0)	 nop main.c(223:93:108:0:0)		if _sensorSample_phase2_1_1 [k17 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{bit bit {0,1}} == 0 goto _iffalse_8($21)
main.c(224:94:112:0:0)		_GO_DONE [k25 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{bit bit {0,1}} := 0x1 {literal bit {0,1}}
main.c(224:95:113:0:0)	 _iffalse_8($21) :
main.c(225:96:114:0:0)	 nop main.c(225:97:115:0:0)	 nop main.c(225:98:116:0:0)	 nop main.c(225:99:117:0:0)	 nop main.c(225:100:118:0:0)	 nop main.c(226:101:119:0:0)	 nop main.c(226:102:120:0:0)	 nop main.c(226:103:121:0:0)	 nop main.c(226:104:122:0:0)	 nop main.c(226:105:123:0:0)	 nop main.c(227:106:124:0:0)		if _sensorSample_phase3_1_1 [k19 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{bit bit {0,1}} == 0 goto _whilecontinue_1($24)
main.c(228:107:128:0:0)		_GO_DONE [k25 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{bit bit {0,1}} := 0x1 {literal bit {0,1}}
main.c(231:109:130:1:0)	 _whilecontinue_1($24) :
main.c(231:110:131:1:0)		if _NOT_DONE [k30 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{bit bit {0,1}} != 0 goto _whilecontinue_1($24)
main.c(234:111:136:0:0)		_TRISB [k10 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{sfr volatile unsigned char} := 0xf0 {literal unsigned char}
main.c(236:112:137:0:0)		iTemp15 [k31 lr112:114 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sensorSample_h_1_1}[r0x6A ] := _ADRESH [k33 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{sfr volatile unsigned char}
main.c(237:113:138:0:0)		iTemp16 [k34 lr113:116 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sensorSample_l_1_1}[r0x6B ] := _ADRESL [k36 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{sfr volatile unsigned char}
main.c(238:114:139:0:0)		iTemp17 [k37 lr114:115 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int}[r0x6A r0x6C ] = (int)iTemp15 [k31 lr112:114 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sensorSample_h_1_1}[r0x6A ]
main.c(238:115:140:0:0)		iTemp18 [k38 lr115:117 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int}[r0x6A r0x6C ] = iTemp17 [k37 lr114:115 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int}[r0x6A r0x6C ] << 0x8 {literal unsigned char}
main.c(238:116:142:0:0)		iTemp20 [k40 lr116:117 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{unsigned int}[r0x6B r0x6D ] = (unsigned int)iTemp16 [k34 lr113:116 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sensorSample_l_1_1}[r0x6B ]
main.c(238:117:143:0:0)		iTemp21 [k41 lr117:118 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{unsigned int}[r0x6A r0x6C ] = iTemp18 [k38 lr115:117 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int}[r0x6A r0x6C ] | iTemp20 [k40 lr116:117 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{unsigned int}[r0x6B r0x6D ]
main.c(238:118:145:0:0)		ret iTemp21 [k41 lr117:118 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{unsigned int}[r0x6A r0x6C ]
main.c(238:119:146:0:0)	 _return($27) :
main.c(238:120:147:0:0)		eproc _sensorSample [k1 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int function    }

----------------------------------------------------------------
Basic Block _eBBlock5 : loop Depth = 0 noPath = 0 , lastinLoop = 0
depth 1st num 17 : bbnum = 16 1st iCode = 67 , last iCode = 67
visited 0 : hasFcall = 0

defines bitVector :bitvector Size = 151 bSize = 19
Bits on { (79) }

local defines bitVector :bitvector Size = 151 bSize = 19
Bits on { (79) }

pointers Set bitvector :
in pointers Set bitvector :
inDefs Set bitvector :bitvector Size = 151 bSize = 19
Bits on { (2) (3) (4) (5) (7) (8) (9) (13) (16) (18) (22) (25) (27) (31) (34) (36) (40) (43) (47) (48) (70) (71) }

outDefs Set bitvector :bitvector Size = 151 bSize = 19
Bits on { (2) (3) (4) (5) (7) (8) (9) (13) (16) (18) (22) (25) (27) (31) (34) (36) (40) (43) (47) (48) (70) (71) (79) }

usesDefs Set bitvector :bitvector Size = 151 bSize = 19
Bits on { }

----------------------------------------------------------------
main.c(216:67:79:0:0)		_GO_DONE [k25 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{bit bit {0,1}} := 0x1 {literal bit {0,1}}
main.c(216:68:80:0:0)	 _iffalse_6($17) :
main.c(217:69:81:0:0)	 nop main.c(217:70:82:0:0)	 nop main.c(217:71:83:0:0)	 nop main.c(217:72:84:0:0)	 nop main.c(217:73:85:0:0)	 nop main.c(218:74:86:0:0)	 nop main.c(218:75:87:0:0)	 nop main.c(218:76:88:0:0)	 nop main.c(218:77:89:0:0)	 nop main.c(218:78:90:0:0)	 nop main.c(219:79:91:0:0)		if _sensorSample_phase1_1_1 [k15 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{bit bit {0,1}} == 0 goto _iffalse_7($19)
main.c(220:80:95:0:0)		_GO_DONE [k25 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{bit bit {0,1}} := 0x1 {literal bit {0,1}}
main.c(220:81:96:0:0)	 _iffalse_7($19) :
main.c(221:82:97:0:0)	 nop main.c(221:83:98:0:0)	 nop main.c(221:84:99:0:0)	 nop main.c(221:85:100:0:0)	 nop main.c(221:86:101:0:0)	 nop main.c(222:87:102:0:0)	 nop main.c(222:88:103:0:0)	 nop main.c(222:89:104:0:0)	 nop main.c(222:90:105:0:0)	 nop main.c(222:91:106:0:0)	 nop main.c(222:92:107:0:0)	 nop main.c(223:93:108:0:0)		if _sensorSample_phase2_1_1 [k17 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{bit bit {0,1}} == 0 goto _iffalse_8($21)
main.c(224:94:112:0:0)		_GO_DONE [k25 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{bit bit {0,1}} := 0x1 {literal bit {0,1}}
main.c(224:95:113:0:0)	 _iffalse_8($21) :
main.c(225:96:114:0:0)	 nop main.c(225:97:115:0:0)	 nop main.c(225:98:116:0:0)	 nop main.c(225:99:117:0:0)	 nop main.c(225:100:118:0:0)	 nop main.c(226:101:119:0:0)	 nop main.c(226:102:120:0:0)	 nop main.c(226:103:121:0:0)	 nop main.c(226:104:122:0:0)	 nop main.c(226:105:123:0:0)	 nop main.c(227:106:124:0:0)		if _sensorSample_phase3_1_1 [k19 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{bit bit {0,1}} == 0 goto _whilecontinue_1($24)
main.c(228:107:128:0:0)		_GO_DONE [k25 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{bit bit {0,1}} := 0x1 {literal bit {0,1}}
main.c(231:109:130:1:0)	 _whilecontinue_1($24) :
main.c(231:110:131:1:0)		if _NOT_DONE [k30 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{bit bit {0,1}} != 0 goto _whilecontinue_1($24)
main.c(234:111:136:0:0)		_TRISB [k10 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{sfr volatile unsigned char} := 0xf0 {literal unsigned char}
main.c(236:112:137:0:0)		iTemp15 [k31 lr112:114 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sensorSample_h_1_1}[r0x6A ] := _ADRESH [k33 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{sfr volatile unsigned char}
main.c(237:113:138:0:0)		iTemp16 [k34 lr113:116 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sensorSample_l_1_1}[r0x6B ] := _ADRESL [k36 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{sfr volatile unsigned char}
main.c(238:114:139:0:0)		iTemp17 [k37 lr114:115 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int}[r0x6A r0x6C ] = (int)iTemp15 [k31 lr112:114 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sensorSample_h_1_1}[r0x6A ]
main.c(238:115:140:0:0)		iTemp18 [k38 lr115:117 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int}[r0x6A r0x6C ] = iTemp17 [k37 lr114:115 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int}[r0x6A r0x6C ] << 0x8 {literal unsigned char}
main.c(238:116:142:0:0)		iTemp20 [k40 lr116:117 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{unsigned int}[r0x6B r0x6D ] = (unsigned int)iTemp16 [k34 lr113:116 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sensorSample_l_1_1}[r0x6B ]
main.c(238:117:143:0:0)		iTemp21 [k41 lr117:118 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{unsigned int}[r0x6A r0x6C ] = iTemp18 [k38 lr115:117 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int}[r0x6A r0x6C ] | iTemp20 [k40 lr116:117 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{unsigned int}[r0x6B r0x6D ]
main.c(238:118:145:0:0)		ret iTemp21 [k41 lr117:118 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{unsigned int}[r0x6A r0x6C ]
main.c(238:119:146:0:0)	 _return($27) :
main.c(238:120:147:0:0)		eproc _sensorSample [k1 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int function    }

----------------------------------------------------------------
Basic Block _iffalse_6 : loop Depth = 0 noPath = 0 , lastinLoop = 0
depth 1st num 18 : bbnum = 17 1st iCode = 68 , last iCode = 79
visited 0 : hasFcall = 0

defines bitVector :bitvector Size = 151 bSize = 19
Bits on { }

local defines bitVector :
pointers Set bitvector :
in pointers Set bitvector :
inDefs Set bitvector :bitvector Size = 151 bSize = 19
Bits on { (2) (3) (4) (5) (7) (8) (9) (13) (16) (18) (22) (25) (27) (31) (34) (36) (40) (43) (47) (48) (70) (71) (79) }

outDefs Set bitvector :bitvector Size = 151 bSize = 19
Bits on { (2) (3) (4) (5) (7) (8) (9) (13) (16) (18) (22) (25) (27) (31) (34) (36) (40) (43) (47) (48) (70) (71) (79) }

usesDefs Set bitvector :bitvector Size = 151 bSize = 19
Bits on { (22) (25) }

----------------------------------------------------------------
main.c(216:68:80:0:0)	 _iffalse_6($17) :
main.c(217:69:81:0:0)	 nop main.c(217:70:82:0:0)	 nop main.c(217:71:83:0:0)	 nop main.c(217:72:84:0:0)	 nop main.c(217:73:85:0:0)	 nop main.c(218:74:86:0:0)	 nop main.c(218:75:87:0:0)	 nop main.c(218:76:88:0:0)	 nop main.c(218:77:89:0:0)	 nop main.c(218:78:90:0:0)	 nop main.c(219:79:91:0:0)		if _sensorSample_phase1_1_1 [k15 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{bit bit {0,1}} == 0 goto _iffalse_7($19)
main.c(220:80:95:0:0)		_GO_DONE [k25 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{bit bit {0,1}} := 0x1 {literal bit {0,1}}
main.c(220:81:96:0:0)	 _iffalse_7($19) :
main.c(221:82:97:0:0)	 nop main.c(221:83:98:0:0)	 nop main.c(221:84:99:0:0)	 nop main.c(221:85:100:0:0)	 nop main.c(221:86:101:0:0)	 nop main.c(222:87:102:0:0)	 nop main.c(222:88:103:0:0)	 nop main.c(222:89:104:0:0)	 nop main.c(222:90:105:0:0)	 nop main.c(222:91:106:0:0)	 nop main.c(222:92:107:0:0)	 nop main.c(223:93:108:0:0)		if _sensorSample_phase2_1_1 [k17 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{bit bit {0,1}} == 0 goto _iffalse_8($21)
main.c(224:94:112:0:0)		_GO_DONE [k25 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{bit bit {0,1}} := 0x1 {literal bit {0,1}}
main.c(224:95:113:0:0)	 _iffalse_8($21) :
main.c(225:96:114:0:0)	 nop main.c(225:97:115:0:0)	 nop main.c(225:98:116:0:0)	 nop main.c(225:99:117:0:0)	 nop main.c(225:100:118:0:0)	 nop main.c(226:101:119:0:0)	 nop main.c(226:102:120:0:0)	 nop main.c(226:103:121:0:0)	 nop main.c(226:104:122:0:0)	 nop main.c(226:105:123:0:0)	 nop main.c(227:106:124:0:0)		if _sensorSample_phase3_1_1 [k19 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{bit bit {0,1}} == 0 goto _whilecontinue_1($24)
main.c(228:107:128:0:0)		_GO_DONE [k25 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{bit bit {0,1}} := 0x1 {literal bit {0,1}}
main.c(231:109:130:1:0)	 _whilecontinue_1($24) :
main.c(231:110:131:1:0)		if _NOT_DONE [k30 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{bit bit {0,1}} != 0 goto _whilecontinue_1($24)
main.c(234:111:136:0:0)		_TRISB [k10 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{sfr volatile unsigned char} := 0xf0 {literal unsigned char}
main.c(236:112:137:0:0)		iTemp15 [k31 lr112:114 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sensorSample_h_1_1}[r0x6A ] := _ADRESH [k33 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{sfr volatile unsigned char}
main.c(237:113:138:0:0)		iTemp16 [k34 lr113:116 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sensorSample_l_1_1}[r0x6B ] := _ADRESL [k36 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{sfr volatile unsigned char}
main.c(238:114:139:0:0)		iTemp17 [k37 lr114:115 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int}[r0x6A r0x6C ] = (int)iTemp15 [k31 lr112:114 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sensorSample_h_1_1}[r0x6A ]
main.c(238:115:140:0:0)		iTemp18 [k38 lr115:117 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int}[r0x6A r0x6C ] = iTemp17 [k37 lr114:115 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int}[r0x6A r0x6C ] << 0x8 {literal unsigned char}
main.c(238:116:142:0:0)		iTemp20 [k40 lr116:117 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{unsigned int}[r0x6B r0x6D ] = (unsigned int)iTemp16 [k34 lr113:116 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sensorSample_l_1_1}[r0x6B ]
main.c(238:117:143:0:0)		iTemp21 [k41 lr117:118 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{unsigned int}[r0x6A r0x6C ] = iTemp18 [k38 lr115:117 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int}[r0x6A r0x6C ] | iTemp20 [k40 lr116:117 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{unsigned int}[r0x6B r0x6D ]
main.c(238:118:145:0:0)		ret iTemp21 [k41 lr117:118 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{unsigned int}[r0x6A r0x6C ]
main.c(238:119:146:0:0)	 _return($27) :
main.c(238:120:147:0:0)		eproc _sensorSample [k1 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int function    }

----------------------------------------------------------------
Basic Block _eBBlock6 : loop Depth = 0 noPath = 0 , lastinLoop = 0
depth 1st num 19 : bbnum = 18 1st iCode = 80 , last iCode = 80
visited 0 : hasFcall = 0

defines bitVector :bitvector Size = 151 bSize = 19
Bits on { (95) }

local defines bitVector :bitvector Size = 151 bSize = 19
Bits on { (95) }

pointers Set bitvector :
in pointers Set bitvector :
inDefs Set bitvector :bitvector Size = 151 bSize = 19
Bits on { (2) (3) (4) (5) (7) (8) (9) (13) (16) (18) (22) (25) (27) (31) (34) (36) (40) (43) (47) (48) (70) (71) (79) }

outDefs Set bitvector :bitvector Size = 151 bSize = 19
Bits on { (2) (3) (4) (5) (7) (8) (9) (13) (16) (18) (22) (25) (27) (31) (34) (36) (40) (43) (47) (48) (70) (71) (95) }

usesDefs Set bitvector :bitvector Size = 151 bSize = 19
Bits on { }

----------------------------------------------------------------
main.c(220:80:95:0:0)		_GO_DONE [k25 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{bit bit {0,1}} := 0x1 {literal bit {0,1}}
main.c(220:81:96:0:0)	 _iffalse_7($19) :
main.c(221:82:97:0:0)	 nop main.c(221:83:98:0:0)	 nop main.c(221:84:99:0:0)	 nop main.c(221:85:100:0:0)	 nop main.c(221:86:101:0:0)	 nop main.c(222:87:102:0:0)	 nop main.c(222:88:103:0:0)	 nop main.c(222:89:104:0:0)	 nop main.c(222:90:105:0:0)	 nop main.c(222:91:106:0:0)	 nop main.c(222:92:107:0:0)	 nop main.c(223:93:108:0:0)		if _sensorSample_phase2_1_1 [k17 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{bit bit {0,1}} == 0 goto _iffalse_8($21)
main.c(224:94:112:0:0)		_GO_DONE [k25 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{bit bit {0,1}} := 0x1 {literal bit {0,1}}
main.c(224:95:113:0:0)	 _iffalse_8($21) :
main.c(225:96:114:0:0)	 nop main.c(225:97:115:0:0)	 nop main.c(225:98:116:0:0)	 nop main.c(225:99:117:0:0)	 nop main.c(225:100:118:0:0)	 nop main.c(226:101:119:0:0)	 nop main.c(226:102:120:0:0)	 nop main.c(226:103:121:0:0)	 nop main.c(226:104:122:0:0)	 nop main.c(226:105:123:0:0)	 nop main.c(227:106:124:0:0)		if _sensorSample_phase3_1_1 [k19 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{bit bit {0,1}} == 0 goto _whilecontinue_1($24)
main.c(228:107:128:0:0)		_GO_DONE [k25 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{bit bit {0,1}} := 0x1 {literal bit {0,1}}
main.c(231:109:130:1:0)	 _whilecontinue_1($24) :
main.c(231:110:131:1:0)		if _NOT_DONE [k30 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{bit bit {0,1}} != 0 goto _whilecontinue_1($24)
main.c(234:111:136:0:0)		_TRISB [k10 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{sfr volatile unsigned char} := 0xf0 {literal unsigned char}
main.c(236:112:137:0:0)		iTemp15 [k31 lr112:114 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sensorSample_h_1_1}[r0x6A ] := _ADRESH [k33 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{sfr volatile unsigned char}
main.c(237:113:138:0:0)		iTemp16 [k34 lr113:116 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sensorSample_l_1_1}[r0x6B ] := _ADRESL [k36 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{sfr volatile unsigned char}
main.c(238:114:139:0:0)		iTemp17 [k37 lr114:115 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int}[r0x6A r0x6C ] = (int)iTemp15 [k31 lr112:114 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sensorSample_h_1_1}[r0x6A ]
main.c(238:115:140:0:0)		iTemp18 [k38 lr115:117 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int}[r0x6A r0x6C ] = iTemp17 [k37 lr114:115 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int}[r0x6A r0x6C ] << 0x8 {literal unsigned char}
main.c(238:116:142:0:0)		iTemp20 [k40 lr116:117 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{unsigned int}[r0x6B r0x6D ] = (unsigned int)iTemp16 [k34 lr113:116 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sensorSample_l_1_1}[r0x6B ]
main.c(238:117:143:0:0)		iTemp21 [k41 lr117:118 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{unsigned int}[r0x6A r0x6C ] = iTemp18 [k38 lr115:117 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int}[r0x6A r0x6C ] | iTemp20 [k40 lr116:117 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{unsigned int}[r0x6B r0x6D ]
main.c(238:118:145:0:0)		ret iTemp21 [k41 lr117:118 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{unsigned int}[r0x6A r0x6C ]
main.c(238:119:146:0:0)	 _return($27) :
main.c(238:120:147:0:0)		eproc _sensorSample [k1 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int function    }

----------------------------------------------------------------
Basic Block _iffalse_7 : loop Depth = 0 noPath = 0 , lastinLoop = 0
depth 1st num 20 : bbnum = 19 1st iCode = 81 , last iCode = 93
visited 0 : hasFcall = 0

defines bitVector :bitvector Size = 151 bSize = 19
Bits on { }

local defines bitVector :
pointers Set bitvector :
in pointers Set bitvector :
inDefs Set bitvector :bitvector Size = 151 bSize = 19
Bits on { (2) (3) (4) (5) (7) (8) (9) (13) (16) (18) (22) (25) (27) (31) (34) (36) (40) (43) (47) (48) (70) (71) (79) (95) }

outDefs Set bitvector :bitvector Size = 151 bSize = 19
Bits on { (2) (3) (4) (5) (7) (8) (9) (13) (16) (18) (22) (25) (27) (31) (34) (36) (40) (43) (47) (48) (70) (71) (79) (95) }

usesDefs Set bitvector :bitvector Size = 151 bSize = 19
Bits on { (31) (34) }

----------------------------------------------------------------
main.c(220:81:96:0:0)	 _iffalse_7($19) :
main.c(221:82:97:0:0)	 nop main.c(221:83:98:0:0)	 nop main.c(221:84:99:0:0)	 nop main.c(221:85:100:0:0)	 nop main.c(221:86:101:0:0)	 nop main.c(222:87:102:0:0)	 nop main.c(222:88:103:0:0)	 nop main.c(222:89:104:0:0)	 nop main.c(222:90:105:0:0)	 nop main.c(222:91:106:0:0)	 nop main.c(222:92:107:0:0)	 nop main.c(223:93:108:0:0)		if _sensorSample_phase2_1_1 [k17 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{bit bit {0,1}} == 0 goto _iffalse_8($21)
main.c(224:94:112:0:0)		_GO_DONE [k25 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{bit bit {0,1}} := 0x1 {literal bit {0,1}}
main.c(224:95:113:0:0)	 _iffalse_8($21) :
main.c(225:96:114:0:0)	 nop main.c(225:97:115:0:0)	 nop main.c(225:98:116:0:0)	 nop main.c(225:99:117:0:0)	 nop main.c(225:100:118:0:0)	 nop main.c(226:101:119:0:0)	 nop main.c(226:102:120:0:0)	 nop main.c(226:103:121:0:0)	 nop main.c(226:104:122:0:0)	 nop main.c(226:105:123:0:0)	 nop main.c(227:106:124:0:0)		if _sensorSample_phase3_1_1 [k19 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{bit bit {0,1}} == 0 goto _whilecontinue_1($24)
main.c(228:107:128:0:0)		_GO_DONE [k25 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{bit bit {0,1}} := 0x1 {literal bit {0,1}}
main.c(231:109:130:1:0)	 _whilecontinue_1($24) :
main.c(231:110:131:1:0)		if _NOT_DONE [k30 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{bit bit {0,1}} != 0 goto _whilecontinue_1($24)
main.c(234:111:136:0:0)		_TRISB [k10 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{sfr volatile unsigned char} := 0xf0 {literal unsigned char}
main.c(236:112:137:0:0)		iTemp15 [k31 lr112:114 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sensorSample_h_1_1}[r0x6A ] := _ADRESH [k33 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{sfr volatile unsigned char}
main.c(237:113:138:0:0)		iTemp16 [k34 lr113:116 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sensorSample_l_1_1}[r0x6B ] := _ADRESL [k36 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{sfr volatile unsigned char}
main.c(238:114:139:0:0)		iTemp17 [k37 lr114:115 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int}[r0x6A r0x6C ] = (int)iTemp15 [k31 lr112:114 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sensorSample_h_1_1}[r0x6A ]
main.c(238:115:140:0:0)		iTemp18 [k38 lr115:117 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int}[r0x6A r0x6C ] = iTemp17 [k37 lr114:115 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int}[r0x6A r0x6C ] << 0x8 {literal unsigned char}
main.c(238:116:142:0:0)		iTemp20 [k40 lr116:117 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{unsigned int}[r0x6B r0x6D ] = (unsigned int)iTemp16 [k34 lr113:116 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sensorSample_l_1_1}[r0x6B ]
main.c(238:117:143:0:0)		iTemp21 [k41 lr117:118 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{unsigned int}[r0x6A r0x6C ] = iTemp18 [k38 lr115:117 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int}[r0x6A r0x6C ] | iTemp20 [k40 lr116:117 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{unsigned int}[r0x6B r0x6D ]
main.c(238:118:145:0:0)		ret iTemp21 [k41 lr117:118 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{unsigned int}[r0x6A r0x6C ]
main.c(238:119:146:0:0)	 _return($27) :
main.c(238:120:147:0:0)		eproc _sensorSample [k1 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int function    }

----------------------------------------------------------------
Basic Block _eBBlock7 : loop Depth = 0 noPath = 0 , lastinLoop = 0
depth 1st num 21 : bbnum = 20 1st iCode = 94 , last iCode = 94
visited 0 : hasFcall = 0

defines bitVector :bitvector Size = 151 bSize = 19
Bits on { (112) }

local defines bitVector :bitvector Size = 151 bSize = 19
Bits on { (112) }

pointers Set bitvector :
in pointers Set bitvector :
inDefs Set bitvector :bitvector Size = 151 bSize = 19
Bits on { (2) (3) (4) (5) (7) (8) (9) (13) (16) (18) (22) (25) (27) (31) (34) (36) (40) (43) (47) (48) (70) (71) (79) (95) }

outDefs Set bitvector :bitvector Size = 151 bSize = 19
Bits on { (2) (3) (4) (5) (7) (8) (9) (13) (16) (18) (22) (25) (27) (31) (34) (36) (40) (43) (47) (48) (70) (71) (112) }

usesDefs Set bitvector :bitvector Size = 151 bSize = 19
Bits on { }

----------------------------------------------------------------
main.c(224:94:112:0:0)		_GO_DONE [k25 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{bit bit {0,1}} := 0x1 {literal bit {0,1}}
main.c(224:95:113:0:0)	 _iffalse_8($21) :
main.c(225:96:114:0:0)	 nop main.c(225:97:115:0:0)	 nop main.c(225:98:116:0:0)	 nop main.c(225:99:117:0:0)	 nop main.c(225:100:118:0:0)	 nop main.c(226:101:119:0:0)	 nop main.c(226:102:120:0:0)	 nop main.c(226:103:121:0:0)	 nop main.c(226:104:122:0:0)	 nop main.c(226:105:123:0:0)	 nop main.c(227:106:124:0:0)		if _sensorSample_phase3_1_1 [k19 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{bit bit {0,1}} == 0 goto _whilecontinue_1($24)
main.c(228:107:128:0:0)		_GO_DONE [k25 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{bit bit {0,1}} := 0x1 {literal bit {0,1}}
main.c(231:109:130:1:0)	 _whilecontinue_1($24) :
main.c(231:110:131:1:0)		if _NOT_DONE [k30 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{bit bit {0,1}} != 0 goto _whilecontinue_1($24)
main.c(234:111:136:0:0)		_TRISB [k10 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{sfr volatile unsigned char} := 0xf0 {literal unsigned char}
main.c(236:112:137:0:0)		iTemp15 [k31 lr112:114 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sensorSample_h_1_1}[r0x6A ] := _ADRESH [k33 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{sfr volatile unsigned char}
main.c(237:113:138:0:0)		iTemp16 [k34 lr113:116 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sensorSample_l_1_1}[r0x6B ] := _ADRESL [k36 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{sfr volatile unsigned char}
main.c(238:114:139:0:0)		iTemp17 [k37 lr114:115 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int}[r0x6A r0x6C ] = (int)iTemp15 [k31 lr112:114 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sensorSample_h_1_1}[r0x6A ]
main.c(238:115:140:0:0)		iTemp18 [k38 lr115:117 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int}[r0x6A r0x6C ] = iTemp17 [k37 lr114:115 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int}[r0x6A r0x6C ] << 0x8 {literal unsigned char}
main.c(238:116:142:0:0)		iTemp20 [k40 lr116:117 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{unsigned int}[r0x6B r0x6D ] = (unsigned int)iTemp16 [k34 lr113:116 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sensorSample_l_1_1}[r0x6B ]
main.c(238:117:143:0:0)		iTemp21 [k41 lr117:118 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{unsigned int}[r0x6A r0x6C ] = iTemp18 [k38 lr115:117 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int}[r0x6A r0x6C ] | iTemp20 [k40 lr116:117 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{unsigned int}[r0x6B r0x6D ]
main.c(238:118:145:0:0)		ret iTemp21 [k41 lr117:118 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{unsigned int}[r0x6A r0x6C ]
main.c(238:119:146:0:0)	 _return($27) :
main.c(238:120:147:0:0)		eproc _sensorSample [k1 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int function    }

----------------------------------------------------------------
Basic Block _iffalse_8 : loop Depth = 0 noPath = 0 , lastinLoop = 0
depth 1st num 22 : bbnum = 21 1st iCode = 95 , last iCode = 106
visited 0 : hasFcall = 0

defines bitVector :bitvector Size = 151 bSize = 19
Bits on { }

local defines bitVector :
pointers Set bitvector :
in pointers Set bitvector :
inDefs Set bitvector :bitvector Size = 151 bSize = 19
Bits on { (2) (3) (4) (5) (7) (8) (9) (13) (16) (18) (22) (25) (27) (31) (34) (36) (40) (43) (47) (48) (70) (71) (79) (95) (112) }

outDefs Set bitvector :bitvector Size = 151 bSize = 19
Bits on { (2) (3) (4) (5) (7) (8) (9) (13) (16) (18) (22) (25) (27) (31) (34) (36) (40) (43) (47) (48) (70) (71) (79) (95) (112) }

usesDefs Set bitvector :bitvector Size = 151 bSize = 19
Bits on { (40) (43) }

----------------------------------------------------------------
main.c(224:95:113:0:0)	 _iffalse_8($21) :
main.c(225:96:114:0:0)	 nop main.c(225:97:115:0:0)	 nop main.c(225:98:116:0:0)	 nop main.c(225:99:117:0:0)	 nop main.c(225:100:118:0:0)	 nop main.c(226:101:119:0:0)	 nop main.c(226:102:120:0:0)	 nop main.c(226:103:121:0:0)	 nop main.c(226:104:122:0:0)	 nop main.c(226:105:123:0:0)	 nop main.c(227:106:124:0:0)		if _sensorSample_phase3_1_1 [k19 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{bit bit {0,1}} == 0 goto _whilecontinue_1($24)
main.c(228:107:128:0:0)		_GO_DONE [k25 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{bit bit {0,1}} := 0x1 {literal bit {0,1}}
main.c(231:109:130:1:0)	 _whilecontinue_1($24) :
main.c(231:110:131:1:0)		if _NOT_DONE [k30 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{bit bit {0,1}} != 0 goto _whilecontinue_1($24)
main.c(234:111:136:0:0)		_TRISB [k10 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{sfr volatile unsigned char} := 0xf0 {literal unsigned char}
main.c(236:112:137:0:0)		iTemp15 [k31 lr112:114 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sensorSample_h_1_1}[r0x6A ] := _ADRESH [k33 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{sfr volatile unsigned char}
main.c(237:113:138:0:0)		iTemp16 [k34 lr113:116 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sensorSample_l_1_1}[r0x6B ] := _ADRESL [k36 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{sfr volatile unsigned char}
main.c(238:114:139:0:0)		iTemp17 [k37 lr114:115 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int}[r0x6A r0x6C ] = (int)iTemp15 [k31 lr112:114 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sensorSample_h_1_1}[r0x6A ]
main.c(238:115:140:0:0)		iTemp18 [k38 lr115:117 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int}[r0x6A r0x6C ] = iTemp17 [k37 lr114:115 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int}[r0x6A r0x6C ] << 0x8 {literal unsigned char}
main.c(238:116:142:0:0)		iTemp20 [k40 lr116:117 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{unsigned int}[r0x6B r0x6D ] = (unsigned int)iTemp16 [k34 lr113:116 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sensorSample_l_1_1}[r0x6B ]
main.c(238:117:143:0:0)		iTemp21 [k41 lr117:118 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{unsigned int}[r0x6A r0x6C ] = iTemp18 [k38 lr115:117 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int}[r0x6A r0x6C ] | iTemp20 [k40 lr116:117 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{unsigned int}[r0x6B r0x6D ]
main.c(238:118:145:0:0)		ret iTemp21 [k41 lr117:118 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{unsigned int}[r0x6A r0x6C ]
main.c(238:119:146:0:0)	 _return($27) :
main.c(238:120:147:0:0)		eproc _sensorSample [k1 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int function    }

----------------------------------------------------------------
Basic Block _eBBlock8 : loop Depth = 0 noPath = 0 , lastinLoop = 0
depth 1st num 23 : bbnum = 22 1st iCode = 107 , last iCode = 107
visited 0 : hasFcall = 0

defines bitVector :bitvector Size = 151 bSize = 19
Bits on { (128) }

local defines bitVector :bitvector Size = 151 bSize = 19
Bits on { (128) }

pointers Set bitvector :
in pointers Set bitvector :
inDefs Set bitvector :bitvector Size = 151 bSize = 19
Bits on { (2) (3) (4) (5) (7) (8) (9) (13) (16) (18) (22) (25) (27) (31) (34) (36) (40) (43) (47) (48) (70) (71) (79) (95) (112) }

outDefs Set bitvector :bitvector Size = 151 bSize = 19
Bits on { (2) (3) (4) (5) (7) (8) (9) (13) (16) (18) (22) (25) (27) (31) (34) (36) (40) (43) (47) (48) (70) (71) (128) }

usesDefs Set bitvector :bitvector Size = 151 bSize = 19
Bits on { }

----------------------------------------------------------------
main.c(228:107:128:0:0)		_GO_DONE [k25 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{bit bit {0,1}} := 0x1 {literal bit {0,1}}
main.c(231:109:130:1:0)	 _whilecontinue_1($24) :
main.c(231:110:131:1:0)		if _NOT_DONE [k30 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{bit bit {0,1}} != 0 goto _whilecontinue_1($24)
main.c(234:111:136:0:0)		_TRISB [k10 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{sfr volatile unsigned char} := 0xf0 {literal unsigned char}
main.c(236:112:137:0:0)		iTemp15 [k31 lr112:114 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sensorSample_h_1_1}[r0x6A ] := _ADRESH [k33 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{sfr volatile unsigned char}
main.c(237:113:138:0:0)		iTemp16 [k34 lr113:116 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sensorSample_l_1_1}[r0x6B ] := _ADRESL [k36 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{sfr volatile unsigned char}
main.c(238:114:139:0:0)		iTemp17 [k37 lr114:115 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int}[r0x6A r0x6C ] = (int)iTemp15 [k31 lr112:114 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sensorSample_h_1_1}[r0x6A ]
main.c(238:115:140:0:0)		iTemp18 [k38 lr115:117 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int}[r0x6A r0x6C ] = iTemp17 [k37 lr114:115 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int}[r0x6A r0x6C ] << 0x8 {literal unsigned char}
main.c(238:116:142:0:0)		iTemp20 [k40 lr116:117 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{unsigned int}[r0x6B r0x6D ] = (unsigned int)iTemp16 [k34 lr113:116 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sensorSample_l_1_1}[r0x6B ]
main.c(238:117:143:0:0)		iTemp21 [k41 lr117:118 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{unsigned int}[r0x6A r0x6C ] = iTemp18 [k38 lr115:117 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int}[r0x6A r0x6C ] | iTemp20 [k40 lr116:117 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{unsigned int}[r0x6B r0x6D ]
main.c(238:118:145:0:0)		ret iTemp21 [k41 lr117:118 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{unsigned int}[r0x6A r0x6C ]
main.c(238:119:146:0:0)	 _return($27) :
main.c(238:120:147:0:0)		eproc _sensorSample [k1 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int function    }

----------------------------------------------------------------
Basic Block preHeaderLbl1 : loop Depth = 0 noPath = 0 , lastinLoop = 0
depth 1st num 24 : bbnum = 23 1st iCode = 108 , last iCode = 108
visited 0 : hasFcall = 0

defines bitVector :bitvector Size = 151 bSize = 19
Bits on { }

local defines bitVector :
pointers Set bitvector :
in pointers Set bitvector :
inDefs Set bitvector :bitvector Size = 151 bSize = 19
Bits on { (2) (3) (4) (5) (7) (8) (9) (13) (16) (18) (22) (25) (27) (31) (34) (36) (40) (43) (47) (48) (70) (71) (79) (95) (112) (128) }

outDefs Set bitvector :bitvector Size = 151 bSize = 19
Bits on { (2) (3) (4) (5) (7) (8) (9) (13) (16) (18) (22) (25) (27) (31) (34) (36) (40) (43) (47) (48) (70) (71) (79) (95) (112) (128) }

usesDefs Set bitvector :bitvector Size = 151 bSize = 19
Bits on { }

----------------------------------------------------------------
main.c(231:108:149:0:0)	 preHeaderLbl1($39) :
main.c(231:109:130:1:0)	 _whilecontinue_1($24) :
main.c(231:110:131:1:0)		if _NOT_DONE [k30 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{bit bit {0,1}} != 0 goto _whilecontinue_1($24)
main.c(234:111:136:0:0)		_TRISB [k10 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{sfr volatile unsigned char} := 0xf0 {literal unsigned char}
main.c(236:112:137:0:0)		iTemp15 [k31 lr112:114 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sensorSample_h_1_1}[r0x6A ] := _ADRESH [k33 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{sfr volatile unsigned char}
main.c(237:113:138:0:0)		iTemp16 [k34 lr113:116 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sensorSample_l_1_1}[r0x6B ] := _ADRESL [k36 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{sfr volatile unsigned char}
main.c(238:114:139:0:0)		iTemp17 [k37 lr114:115 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int}[r0x6A r0x6C ] = (int)iTemp15 [k31 lr112:114 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sensorSample_h_1_1}[r0x6A ]
main.c(238:115:140:0:0)		iTemp18 [k38 lr115:117 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int}[r0x6A r0x6C ] = iTemp17 [k37 lr114:115 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int}[r0x6A r0x6C ] << 0x8 {literal unsigned char}
main.c(238:116:142:0:0)		iTemp20 [k40 lr116:117 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{unsigned int}[r0x6B r0x6D ] = (unsigned int)iTemp16 [k34 lr113:116 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sensorSample_l_1_1}[r0x6B ]
main.c(238:117:143:0:0)		iTemp21 [k41 lr117:118 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{unsigned int}[r0x6A r0x6C ] = iTemp18 [k38 lr115:117 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int}[r0x6A r0x6C ] | iTemp20 [k40 lr116:117 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{unsigned int}[r0x6B r0x6D ]
main.c(238:118:145:0:0)		ret iTemp21 [k41 lr117:118 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{unsigned int}[r0x6A r0x6C ]
main.c(238:119:146:0:0)	 _return($27) :
main.c(238:120:147:0:0)		eproc _sensorSample [k1 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int function    }

----------------------------------------------------------------
Basic Block _whilecontinue_1 : loop Depth = 1 noPath = 0 , lastinLoop = 0
depth 1st num 25 : bbnum = 24 1st iCode = 109 , last iCode = 110
visited 0 : hasFcall = 0

defines bitVector :bitvector Size = 151 bSize = 19
Bits on { }

local defines bitVector :
pointers Set bitvector :
in pointers Set bitvector :
inDefs Set bitvector :bitvector Size = 151 bSize = 19
Bits on { (2) (3) (4) (5) (7) (8) (9) (13) (16) (18) (22) (25) (27) (31) (34) (36) (40) (43) (47) (48) (70) (71) (78) (79) (94) (95) (111) (112) (127) (128) }

outDefs Set bitvector :bitvector Size = 151 bSize = 19
Bits on { (2) (3) (4) (5) (7) (8) (9) (13) (16) (18) (22) (25) (27) (31) (34) (36) (40) (43) (47) (48) (70) (71) (78) (79) (94) (95) (111) (112) (127) (128) }

usesDefs Set bitvector :bitvector Size = 151 bSize = 19
Bits on { }

----------------------------------------------------------------
main.c(231:109:130:1:0)	 _whilecontinue_1($24) :
main.c(231:110:131:1:0)		if _NOT_DONE [k30 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{bit bit {0,1}} != 0 goto _whilecontinue_1($24)
main.c(234:111:136:0:0)		_TRISB [k10 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{sfr volatile unsigned char} := 0xf0 {literal unsigned char}
main.c(236:112:137:0:0)		iTemp15 [k31 lr112:114 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sensorSample_h_1_1}[r0x6A ] := _ADRESH [k33 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{sfr volatile unsigned char}
main.c(237:113:138:0:0)		iTemp16 [k34 lr113:116 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sensorSample_l_1_1}[r0x6B ] := _ADRESL [k36 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{sfr volatile unsigned char}
main.c(238:114:139:0:0)		iTemp17 [k37 lr114:115 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int}[r0x6A r0x6C ] = (int)iTemp15 [k31 lr112:114 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sensorSample_h_1_1}[r0x6A ]
main.c(238:115:140:0:0)		iTemp18 [k38 lr115:117 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int}[r0x6A r0x6C ] = iTemp17 [k37 lr114:115 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int}[r0x6A r0x6C ] << 0x8 {literal unsigned char}
main.c(238:116:142:0:0)		iTemp20 [k40 lr116:117 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{unsigned int}[r0x6B r0x6D ] = (unsigned int)iTemp16 [k34 lr113:116 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sensorSample_l_1_1}[r0x6B ]
main.c(238:117:143:0:0)		iTemp21 [k41 lr117:118 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{unsigned int}[r0x6A r0x6C ] = iTemp18 [k38 lr115:117 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int}[r0x6A r0x6C ] | iTemp20 [k40 lr116:117 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{unsigned int}[r0x6B r0x6D ]
main.c(238:118:145:0:0)		ret iTemp21 [k41 lr117:118 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{unsigned int}[r0x6A r0x6C ]
main.c(238:119:146:0:0)	 _return($27) :
main.c(238:120:147:0:0)		eproc _sensorSample [k1 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int function    }

----------------------------------------------------------------
Basic Block _eBBlock9 : loop Depth = 0 noPath = 0 , lastinLoop = 0
depth 1st num 26 : bbnum = 25 1st iCode = 111 , last iCode = 118
visited 0 : hasFcall = 0

defines bitVector :bitvector Size = 151 bSize = 19
Bits on { (136) (137) (138) (139) (140) (142) (143) }

local defines bitVector :bitvector Size = 151 bSize = 19
Bits on { (136) (137) (138) (139) (140) (142) (143) }

pointers Set bitvector :
in pointers Set bitvector :
inDefs Set bitvector :bitvector Size = 151 bSize = 19
Bits on { (2) (3) (4) (5) (7) (8) (9) (13) (16) (18) (22) (25) (27) (31) (34) (36) (40) (43) (47) (48) (70) (71) (78) (79) (94) (95) (111) (112) (127) (128) }

outDefs Set bitvector :bitvector Size = 151 bSize = 19
Bits on { (2) (3) (4) (5) (8) (9) (13) (16) (18) (22) (25) (27) (31) (34) (36) (40) (43) (47) (48) (70) (71) (78) (79) (94) (95) (111) (112) (127) (128) (136) (137) (138) (139) (140) (142) (143) }

usesDefs Set bitvector :bitvector Size = 151 bSize = 19
Bits on { (137) (138) (139) (140) (142) (143) }

----------------------------------------------------------------
main.c(234:111:136:0:0)		_TRISB [k10 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{sfr volatile unsigned char} := 0xf0 {literal unsigned char}
main.c(236:112:137:0:0)		iTemp15 [k31 lr112:114 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sensorSample_h_1_1}[r0x6A ] := _ADRESH [k33 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{sfr volatile unsigned char}
main.c(237:113:138:0:0)		iTemp16 [k34 lr113:116 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sensorSample_l_1_1}[r0x6B ] := _ADRESL [k36 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{sfr volatile unsigned char}
main.c(238:114:139:0:0)		iTemp17 [k37 lr114:115 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int}[r0x6A r0x6C ] = (int)iTemp15 [k31 lr112:114 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sensorSample_h_1_1}[r0x6A ]
main.c(238:115:140:0:0)		iTemp18 [k38 lr115:117 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int}[r0x6A r0x6C ] = iTemp17 [k37 lr114:115 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int}[r0x6A r0x6C ] << 0x8 {literal unsigned char}
main.c(238:116:142:0:0)		iTemp20 [k40 lr116:117 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{unsigned int}[r0x6B r0x6D ] = (unsigned int)iTemp16 [k34 lr113:116 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sensorSample_l_1_1}[r0x6B ]
main.c(238:117:143:0:0)		iTemp21 [k41 lr117:118 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{unsigned int}[r0x6A r0x6C ] = iTemp18 [k38 lr115:117 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int}[r0x6A r0x6C ] | iTemp20 [k40 lr116:117 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{unsigned int}[r0x6B r0x6D ]
main.c(238:118:145:0:0)		ret iTemp21 [k41 lr117:118 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{unsigned int}[r0x6A r0x6C ]
main.c(238:119:146:0:0)	 _return($27) :
main.c(238:120:147:0:0)		eproc _sensorSample [k1 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int function    }

----------------------------------------------------------------
Basic Block _return : loop Depth = 0 noPath = 0 , lastinLoop = 0
depth 1st num 27 : bbnum = 26 1st iCode = 119 , last iCode = 120
visited 0 : hasFcall = 0

defines bitVector :bitvector Size = 151 bSize = 19
Bits on { }

local defines bitVector :
pointers Set bitvector :
in pointers Set bitvector :
inDefs Set bitvector :bitvector Size = 151 bSize = 19
Bits on { (2) (3) (4) (5) (8) (9) (13) (16) (18) (22) (25) (27) (31) (34) (36) (40) (43) (47) (48) (70) (71) (78) (79) (94) (95) (111) (112) (127) (128) (136) (137) (138) (139) (140) (142) (143) }

outDefs Set bitvector :bitvector Size = 151 bSize = 19
Bits on { (2) (3) (4) (5) (8) (9) (13) (16) (18) (22) (25) (27) (31) (34) (36) (40) (43) (47) (48) (70) (71) (78) (79) (94) (95) (111) (112) (127) (128) (136) (137) (138) (139) (140) (142) (143) }

usesDefs Set bitvector :bitvector Size = 151 bSize = 19
Bits on { }

----------------------------------------------------------------
main.c(238:119:146:0:0)	 _return($27) :
main.c(238:120:147:0:0)		eproc _sensorSample [k1 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int function    }
typeRegWithIdx - requesting index = 0x65
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
Found a Stack Register!
typeRegWithIdx - requesting index = 0x66
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x1000
typeRegWithIdx - requesting index = 0x1000
typeRegWithIdx - requesting index = 0x1000
Found a Stack Register!
typeRegWithIdx - requesting index = 0x67
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x66
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x67
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x67
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x67
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x67
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x68
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x68
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x69
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x65
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x69
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6a
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6b
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6a
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6a
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6c
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6a
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6c
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6a
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6b
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6b
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6d
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6b
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6a
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6d
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6c
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6a
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
Found a Stack Register!
typeRegWithIdx - requesting index = 0x6c
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
main.c(251:1:0:0:0)	 _entry($5) :
main.c(251:2:1:0:0)		proc _serialBitDelay [k1 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{void function    }

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
main.c(253:3:13:0:0)	 preHeaderLbl0($6) :
main.c(253:4:14:0:0)		iTemp1 [k4 lr4:8 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _serialBitDelay_i_1_1} := 0x40 {literal unsigned char}

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
main.c(253:5:3:1:0)	 _dobody_2($1) :
main.c(254:6:4:1:0)	 nop main.c(255:7:6:1:0)		iTemp1 [k4 lr4:8 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _serialBitDelay_i_1_1} = iTemp1 [k4 lr4:8 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _serialBitDelay_i_1_1} - 0x1 {literal unsigned char}
main.c(255:8:8:1:0)		if iTemp1 [k4 lr4:8 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _serialBitDelay_i_1_1} != 0 goto _dobody_2($1)

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
main.c(255:9:11:0:0)	 _return($4) :
main.c(255:10:12:0:0)		eproc _serialBitDelay [k1 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{void function    }
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
  -- #75 reg = r0x63  key 0, rIdx = 99, size 1
  -- #76 reg = r0x64  key 0, rIdx = 100, size 1
  -- #77 reg = _ADRESH  key 0, rIdx = 36, size 1
  -- #78 reg = r0x65  key 0, rIdx = 101, size 1
  -- #79 reg = r0x66  key 0, rIdx = 102, size 1
  -- #80 reg = r0x67  key 0, rIdx = 103, size 1
  -- #81 reg = r0x68  key 0, rIdx = 104, size 1
  -- #82 reg = r0x69  key 0, rIdx = 105, size 1
  -- #83 reg = r0x6A  key 0, rIdx = 106, size 1
  -- #84 reg = r0x6B  key 0, rIdx = 107, size 1
  -- #85 reg = r0x6C  key 0, rIdx = 108, size 1
  -- #86 reg = r0x6D  key 0, rIdx = 109, size 1
  -- #87 reg = _ADRESL  key 5, rIdx = 37, size 1
  -- #88 reg = _PCLATH  key 6, rIdx = 10, size 1
  -- #89 reg = _TRISA  key 7, rIdx = 1, size 1
  -- #90 reg = _TRISB  key 8, rIdx = 2, size 1
  -- #91 reg = _TRISB  key 8, rIdx = 24, size 1
  -- #92 reg = _TRISB  key 8, rIdx = 26, size 1
  -- #93 reg = _TRISB  key 8, rIdx = 35, size 1
  -- #94 reg = _TRISC  key 9, rIdx = 3, size 1
  -- #95 reg = _TRISD  key 10, rIdx = 4, size 1
  -- #96 reg = _PORTA  key 10, rIdx = 6, size 1
  -- #97 reg = _TRISE  key 11, rIdx = 5, size 1
  -- #98 reg = _PORTB  key 11, rIdx = 7, size 1
  -- #99 reg = _PORTB  key 11, rIdx = 23, size 1
  -- #100 reg = _PORTB  key 11, rIdx = 33, size 1
  -- #101 reg = _PORTC  key 12, rIdx = 8, size 1
  -- #102 reg = _GIE  key 12, rIdx = 13, size 1
  -- #103 reg = _PORTD  key 13, rIdx = 9, size 1
  -- #104 reg = _PORTE  key 14, rIdx = 10, size 1
  -- #105 reg = _numResonationHalfcycles  key 14, rIdx = 32, size 1
  -- #106 reg = _sensorSample_phase0_1_1  key 15, rIdx = 28, size 1
  -- #107 reg = _sensorSample_phase1_1_1  key 16, rIdx = 29, size 1
  -- #108 reg = _sensorSample_phase2_1_1  key 17, rIdx = 30, size 1
  -- #109 reg = _sensorSample_phase3_1_1  key 18, rIdx = 31, size 1
  -- #110 reg = _ADCON0  key 27, rIdx = 12, size 1
  -- #111 reg = _ADCON0  key 27, rIdx = 25, size 1
  -- #112 reg = _ADCON0  key 27, rIdx = 27, size 1
  -- #113 reg = _ADCON1  key 28, rIdx = 11, size 1
  -- #114 reg = SSAVE  key 32, rIdx = 32765, size 1
  -- #115 reg = WSAVE  key 36, rIdx = 32766, size 1
  -- #116 reg = _reading0  key 44, rIdx = 14, size 2
  -- #117 reg = _GO_DONE  key 44, rIdx = 34, size 1
  -- #118 reg = _reading1  key 45, rIdx = 15, size 2
  -- #119 reg = _reading2  key 46, rIdx = 16, size 2
  -- #120 reg = INTCON  key 47, rIdx = 11, size 1
  -- #121 reg = _reading3  key 47, rIdx = 17, size 2
  -- #122 reg = _STATUS  key 48, rIdx = 3, size 1
  -- #123 reg = PCL  key 48, rIdx = 2, size 1
  -- #124 reg = _reading4  key 48, rIdx = 18, size 2
  -- #125 reg = KZ  key 49, rIdx = 32767, size 1
  -- #126 reg = _reading5  key 49, rIdx = 19, size 2
  -- #127 reg = _reading6  key 50, rIdx = 20, size 2
  -- #128 reg = _reading7  key 51, rIdx = 21, size 2
  -- #129 reg = INDF  key 56, rIdx = 0, size 1
  -- #130 reg = _Light0  key 56, rIdx = 0, size 1
  -- #131 reg = _numIntegrationSamples  key 56, rIdx = 22, size 1
  -- #132 reg = FSR  key 60, rIdx = 4, size 1
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
bitvector Size = 59 bSize = 8
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
main.c(251:1:0:0:0)	 _entry($5) :
main.c(251:2:1:0:0)		proc _serialBitDelay [k1 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{void function    }
main.c(253:4:14:0:0)		iTemp1 [k4 lr4:8 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _serialBitDelay_i_1_1}[r0x6E ] := 0x40 {literal unsigned char}
main.c(253:5:3:1:0)	 _dobody_2($1) :
main.c(254:6:4:1:0)	 nop main.c(255:7:6:1:0)		iTemp1 [k4 lr4:8 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _serialBitDelay_i_1_1}[r0x6E ] = iTemp1 [k4 lr4:8 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _serialBitDelay_i_1_1}[r0x6E ] - 0x1 {literal unsigned char}
main.c(255:8:8:1:0)		if iTemp1 [k4 lr4:8 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _serialBitDelay_i_1_1}[r0x6E ] != 0 goto _dobody_2($1)
main.c(255:9:11:0:0)	 _return($4) :
main.c(255:10:12:0:0)		eproc _serialBitDelay [k1 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{void function    }

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
main.c(253:3:13:0:0)	 preHeaderLbl0($6) :
main.c(253:4:14:0:0)		iTemp1 [k4 lr4:8 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _serialBitDelay_i_1_1}[r0x6E ] := 0x40 {literal unsigned char}
main.c(253:5:3:1:0)	 _dobody_2($1) :
main.c(254:6:4:1:0)	 nop main.c(255:7:6:1:0)		iTemp1 [k4 lr4:8 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _serialBitDelay_i_1_1}[r0x6E ] = iTemp1 [k4 lr4:8 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _serialBitDelay_i_1_1}[r0x6E ] - 0x1 {literal unsigned char}
main.c(255:8:8:1:0)		if iTemp1 [k4 lr4:8 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _serialBitDelay_i_1_1}[r0x6E ] != 0 goto _dobody_2($1)
main.c(255:9:11:0:0)	 _return($4) :
main.c(255:10:12:0:0)		eproc _serialBitDelay [k1 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{void function    }

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
main.c(253:5:3:1:0)	 _dobody_2($1) :
main.c(254:6:4:1:0)	 nop main.c(255:7:6:1:0)		iTemp1 [k4 lr4:8 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _serialBitDelay_i_1_1}[r0x6E ] = iTemp1 [k4 lr4:8 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _serialBitDelay_i_1_1}[r0x6E ] - 0x1 {literal unsigned char}
main.c(255:8:8:1:0)		if iTemp1 [k4 lr4:8 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _serialBitDelay_i_1_1}[r0x6E ] != 0 goto _dobody_2($1)
main.c(255:9:11:0:0)	 _return($4) :
main.c(255:10:12:0:0)		eproc _serialBitDelay [k1 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{void function    }

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
main.c(255:9:11:0:0)	 _return($4) :
main.c(255:10:12:0:0)		eproc _serialBitDelay [k1 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{void function    }
typeRegWithIdx - requesting index = 0x6e
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
main.c(258:1:0:0:0)	 _entry($5) :
main.c(258:2:1:0:0)		proc _halfSerialBitDelay [k1 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{void function    }

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
main.c(260:3:13:0:0)	 preHeaderLbl0($6) :
main.c(260:4:14:0:0)		iTemp1 [k4 lr4:8 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _halfSerialBitDelay_i_1_1} := 0x20 {literal unsigned char}

----------------------------------------------------------------
Basic Block _dobody_3 : loop Depth = 1 noPath = 0 , lastinLoop = 1
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
main.c(260:5:3:1:0)	 _dobody_3($1) :
main.c(261:6:4:1:0)	 nop main.c(262:7:6:1:0)		iTemp1 [k4 lr4:8 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _halfSerialBitDelay_i_1_1} = iTemp1 [k4 lr4:8 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _halfSerialBitDelay_i_1_1} - 0x1 {literal unsigned char}
main.c(262:8:8:1:0)		if iTemp1 [k4 lr4:8 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _halfSerialBitDelay_i_1_1} != 0 goto _dobody_3($1)

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
main.c(262:9:11:0:0)	 _return($4) :
main.c(262:10:12:0:0)		eproc _halfSerialBitDelay [k1 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{void function    }
packRegisters
  left:_halfSerialBitDelay [k1 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{void function    }
        Symbol type: void function    
packRegisters
packRegsForAssign
  result:iTemp1 [k4 lr4:8 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _halfSerialBitDelay_i_1_1}
  left:
  right:0x20 {literal unsigned char}
  2705 - not packing - right is not temp
allocDirReg BAD, op is NULL
  result:iTemp1 [k4 lr4:8 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _halfSerialBitDelay_i_1_1}
  left:
  right:0x20 {literal unsigned char}
  result:iTemp1 [k4 lr4:8 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _halfSerialBitDelay_i_1_1}
          Symbol type: unsigned char
  3594 - pointer reg req = 0
packRegisters
  left:iTemp1 [k4 lr4:8 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _halfSerialBitDelay_i_1_1}
        Symbol type: unsigned char
  result:iTemp1 [k4 lr4:8 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _halfSerialBitDelay_i_1_1}
          Symbol type: unsigned char
  3594 - pointer reg req = 0
packRegsForSupport
packRegsForAccUse
  packRegsForAccUse:3181
  packRegsForAccUse:3188
  left:iTemp1 [k4 lr4:8 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _halfSerialBitDelay_i_1_1}
        Symbol type: unsigned char
  3594 - pointer reg req = 0
packRegisters
  left:_halfSerialBitDelay [k1 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{void function    }
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
  -- #75 reg = r0x63  key 0, rIdx = 99, size 1
  -- #76 reg = r0x64  key 0, rIdx = 100, size 1
  -- #77 reg = _ADRESH  key 0, rIdx = 36, size 1
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
  -- #88 reg = _ADRESL  key 5, rIdx = 37, size 1
  -- #89 reg = _PCLATH  key 6, rIdx = 10, size 1
  -- #90 reg = _TRISA  key 7, rIdx = 1, size 1
  -- #91 reg = _TRISB  key 8, rIdx = 2, size 1
  -- #92 reg = _TRISB  key 8, rIdx = 24, size 1
  -- #93 reg = _TRISB  key 8, rIdx = 26, size 1
  -- #94 reg = _TRISB  key 8, rIdx = 35, size 1
  -- #95 reg = _TRISC  key 9, rIdx = 3, size 1
  -- #96 reg = _TRISD  key 10, rIdx = 4, size 1
  -- #97 reg = _PORTA  key 10, rIdx = 6, size 1
  -- #98 reg = _TRISE  key 11, rIdx = 5, size 1
  -- #99 reg = _PORTB  key 11, rIdx = 7, size 1
  -- #100 reg = _PORTB  key 11, rIdx = 23, size 1
  -- #101 reg = _PORTB  key 11, rIdx = 33, size 1
  -- #102 reg = _PORTC  key 12, rIdx = 8, size 1
  -- #103 reg = _GIE  key 12, rIdx = 13, size 1
  -- #104 reg = _PORTD  key 13, rIdx = 9, size 1
  -- #105 reg = _PORTE  key 14, rIdx = 10, size 1
  -- #106 reg = _numResonationHalfcycles  key 14, rIdx = 32, size 1
  -- #107 reg = _sensorSample_phase0_1_1  key 15, rIdx = 28, size 1
  -- #108 reg = _sensorSample_phase1_1_1  key 16, rIdx = 29, size 1
  -- #109 reg = _sensorSample_phase2_1_1  key 17, rIdx = 30, size 1
  -- #110 reg = _sensorSample_phase3_1_1  key 18, rIdx = 31, size 1
  -- #111 reg = _ADCON0  key 27, rIdx = 12, size 1
  -- #112 reg = _ADCON0  key 27, rIdx = 25, size 1
  -- #113 reg = _ADCON0  key 27, rIdx = 27, size 1
  -- #114 reg = _ADCON1  key 28, rIdx = 11, size 1
  -- #115 reg = SSAVE  key 32, rIdx = 32765, size 1
  -- #116 reg = WSAVE  key 36, rIdx = 32766, size 1
  -- #117 reg = _reading0  key 44, rIdx = 14, size 2
  -- #118 reg = _GO_DONE  key 44, rIdx = 34, size 1
  -- #119 reg = _reading1  key 45, rIdx = 15, size 2
  -- #120 reg = _reading2  key 46, rIdx = 16, size 2
  -- #121 reg = INTCON  key 47, rIdx = 11, size 1
  -- #122 reg = _reading3  key 47, rIdx = 17, size 2
  -- #123 reg = _STATUS  key 48, rIdx = 3, size 1
  -- #124 reg = PCL  key 48, rIdx = 2, size 1
  -- #125 reg = _reading4  key 48, rIdx = 18, size 2
  -- #126 reg = KZ  key 49, rIdx = 32767, size 1
  -- #127 reg = _reading5  key 49, rIdx = 19, size 2
  -- #128 reg = _reading6  key 50, rIdx = 20, size 2
  -- #129 reg = _reading7  key 51, rIdx = 21, size 2
  -- #130 reg = INDF  key 56, rIdx = 0, size 1
  -- #131 reg = _Light0  key 56, rIdx = 0, size 1
  -- #132 reg = _numIntegrationSamples  key 56, rIdx = 22, size 1
  -- #133 reg = FSR  key 60, rIdx = 4, size 1
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
bitvector Size = 59 bSize = 8
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
main.c(258:1:0:0:0)	 _entry($5) :
main.c(258:2:1:0:0)		proc _halfSerialBitDelay [k1 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{void function    }
main.c(260:4:14:0:0)		iTemp1 [k4 lr4:8 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _halfSerialBitDelay_i_1_1}[r0x6F ] := 0x20 {literal unsigned char}
main.c(260:5:3:1:0)	 _dobody_3($1) :
main.c(261:6:4:1:0)	 nop main.c(262:7:6:1:0)		iTemp1 [k4 lr4:8 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _halfSerialBitDelay_i_1_1}[r0x6F ] = iTemp1 [k4 lr4:8 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _halfSerialBitDelay_i_1_1}[r0x6F ] - 0x1 {literal unsigned char}
main.c(262:8:8:1:0)		if iTemp1 [k4 lr4:8 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _halfSerialBitDelay_i_1_1}[r0x6F ] != 0 goto _dobody_3($1)
main.c(262:9:11:0:0)	 _return($4) :
main.c(262:10:12:0:0)		eproc _halfSerialBitDelay [k1 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{void function    }

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
main.c(260:3:13:0:0)	 preHeaderLbl0($6) :
main.c(260:4:14:0:0)		iTemp1 [k4 lr4:8 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _halfSerialBitDelay_i_1_1}[r0x6F ] := 0x20 {literal unsigned char}
main.c(260:5:3:1:0)	 _dobody_3($1) :
main.c(261:6:4:1:0)	 nop main.c(262:7:6:1:0)		iTemp1 [k4 lr4:8 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _halfSerialBitDelay_i_1_1}[r0x6F ] = iTemp1 [k4 lr4:8 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _halfSerialBitDelay_i_1_1}[r0x6F ] - 0x1 {literal unsigned char}
main.c(262:8:8:1:0)		if iTemp1 [k4 lr4:8 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _halfSerialBitDelay_i_1_1}[r0x6F ] != 0 goto _dobody_3($1)
main.c(262:9:11:0:0)	 _return($4) :
main.c(262:10:12:0:0)		eproc _halfSerialBitDelay [k1 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{void function    }

----------------------------------------------------------------
Basic Block _dobody_3 : loop Depth = 1 noPath = 0 , lastinLoop = 1
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
main.c(260:5:3:1:0)	 _dobody_3($1) :
main.c(261:6:4:1:0)	 nop main.c(262:7:6:1:0)		iTemp1 [k4 lr4:8 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _halfSerialBitDelay_i_1_1}[r0x6F ] = iTemp1 [k4 lr4:8 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _halfSerialBitDelay_i_1_1}[r0x6F ] - 0x1 {literal unsigned char}
main.c(262:8:8:1:0)		if iTemp1 [k4 lr4:8 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _halfSerialBitDelay_i_1_1}[r0x6F ] != 0 goto _dobody_3($1)
main.c(262:9:11:0:0)	 _return($4) :
main.c(262:10:12:0:0)		eproc _halfSerialBitDelay [k1 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{void function    }

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
main.c(262:9:11:0:0)	 _return($4) :
main.c(262:10:12:0:0)		eproc _halfSerialBitDelay [k1 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{void function    }
typeRegWithIdx - requesting index = 0x6f
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6f
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
main.c(265:1:0:0:0)	 _entry($8) :
main.c(265:2:1:0:0)		proc _putc [k1 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{void function    }
main.c(265:3:2:0:0)	iTemp0 [k2 lr3:21 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _putc_byte_1_1} = recv 
main.c(269:4:4:0:0)		_SerialPort [k4 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{bit bit {0,1}} := 0x0 {literal bit {0,1}}
main.c(270:5:6:0:0)		_SerialTris [k6 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{bit bit {0,1}} := 0x0 {literal bit {0,1}}
main.c(271:6:7:0:0)		iTemp3 [k9 lr6:6 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{void} = call _serialBitDelay [k8 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{void function    }

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
main.c(275:7:37:0:0)	 preHeaderLbl0($10) :
main.c(275:8:38:0:0)		iTemp10 [k17 lr8:21 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _putc_bitnum_1_1} := 0x8 {literal unsigned char}

----------------------------------------------------------------
Basic Block _dobody_4 : loop Depth = 1 noPath = 0 , lastinLoop = 0
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
main.c(275:9:9:1:0)	 _dobody_4($4) :
main.c(276:10:10:1:0)		iTemp5 [k12 lr10:11 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{unsigned char} = iTemp0 [k2 lr3:21 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _putc_byte_1_1} & 0x1 {literal unsigned char}
main.c(276:11:11:1:0)		if iTemp5 [k12 lr10:11 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{unsigned char} == 0 goto _iffalse_10($2)

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
main.c(277:12:15:1:0)		_SerialPort [k4 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{bit bit {0,1}} := 0x1 {literal bit {0,1}}
main.c(277:13:16:1:0)		 goto _ifend_10($3)

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
main.c(277:14:17:1:0)	 _iffalse_10($2) :
main.c(279:15:19:1:0)		_SerialPort [k4 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{bit bit {0,1}} := 0x0 {literal bit {0,1}}

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
main.c(279:16:20:1:0)	 _ifend_10($3) :
main.c(280:17:21:1:0)		iTemp8 [k15 lr17:18 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{unsigned char} = iTemp0 [k2 lr3:21 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _putc_byte_1_1} >> 0x1 {literal unsigned char}
main.c(280:18:22:1:0)		iTemp0 [k2 lr3:21 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _putc_byte_1_1} := iTemp8 [k15 lr17:18 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{unsigned char}
main.c(281:19:23:1:0)		iTemp9 [k16 lr19:19 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{void} = call _serialBitDelay [k8 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{void function    }
main.c(282:20:25:1:0)		iTemp10 [k17 lr8:21 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _putc_bitnum_1_1} = iTemp10 [k17 lr8:21 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _putc_bitnum_1_1} - 0x1 {literal unsigned char}
main.c(282:21:27:1:0)		if iTemp10 [k17 lr8:21 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _putc_bitnum_1_1} != 0 goto _dobody_4($4)

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
main.c(285:22:31:0:0)		_SerialPort [k4 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{bit bit {0,1}} := 0x1 {literal bit {0,1}}
main.c(286:23:32:0:0)		iTemp12 [k19 lr23:23 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{void} = call _serialBitDelay [k8 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{void function    }
main.c(287:24:34:0:0)		_SerialTris [k6 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{bit bit {0,1}} := 0x1 {literal bit {0,1}}

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
main.c(287:25:35:0:0)	 _return($7) :
main.c(287:26:36:0:0)		eproc _putc [k1 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{void function    }
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
  -- #75 reg = r0x63  key 0, rIdx = 99, size 1
  -- #76 reg = r0x64  key 0, rIdx = 100, size 1
  -- #77 reg = _ADRESH  key 0, rIdx = 36, size 1
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
  -- #88 reg = r0x6F  key 0, rIdx = 111, size 1
  -- #89 reg = _ADRESL  key 5, rIdx = 37, size 1
  -- #90 reg = _PCLATH  key 6, rIdx = 10, size 1
  -- #91 reg = _TRISA  key 7, rIdx = 1, size 1
  -- #92 reg = _TRISB  key 8, rIdx = 2, size 1
  -- #93 reg = _TRISB  key 8, rIdx = 24, size 1
  -- #94 reg = _TRISB  key 8, rIdx = 26, size 1
  -- #95 reg = _TRISB  key 8, rIdx = 35, size 1
  -- #96 reg = _TRISC  key 9, rIdx = 3, size 1
  -- #97 reg = _TRISD  key 10, rIdx = 4, size 1
  -- #98 reg = _PORTA  key 10, rIdx = 6, size 1
  -- #99 reg = _TRISE  key 11, rIdx = 5, size 1
  -- #100 reg = _PORTB  key 11, rIdx = 7, size 1
  -- #101 reg = _PORTB  key 11, rIdx = 23, size 1
  -- #102 reg = _PORTB  key 11, rIdx = 33, size 1
  -- #103 reg = _PORTC  key 12, rIdx = 8, size 1
  -- #104 reg = _GIE  key 12, rIdx = 13, size 1
  -- #105 reg = _PORTD  key 13, rIdx = 9, size 1
  -- #106 reg = _PORTE  key 14, rIdx = 10, size 1
  -- #107 reg = _numResonationHalfcycles  key 14, rIdx = 32, size 1
  -- #108 reg = _sensorSample_phase0_1_1  key 15, rIdx = 28, size 1
  -- #109 reg = _sensorSample_phase1_1_1  key 16, rIdx = 29, size 1
  -- #110 reg = _sensorSample_phase2_1_1  key 17, rIdx = 30, size 1
  -- #111 reg = _sensorSample_phase3_1_1  key 18, rIdx = 31, size 1
  -- #112 reg = _ADCON0  key 27, rIdx = 12, size 1
  -- #113 reg = _ADCON0  key 27, rIdx = 25, size 1
  -- #114 reg = _ADCON0  key 27, rIdx = 27, size 1
  -- #115 reg = _ADCON1  key 28, rIdx = 11, size 1
  -- #116 reg = SSAVE  key 32, rIdx = 32765, size 1
  -- #117 reg = WSAVE  key 36, rIdx = 32766, size 1
  -- #118 reg = _reading0  key 44, rIdx = 14, size 2
  -- #119 reg = _GO_DONE  key 44, rIdx = 34, size 1
  -- #120 reg = _reading1  key 45, rIdx = 15, size 2
  -- #121 reg = _reading2  key 46, rIdx = 16, size 2
  -- #122 reg = INTCON  key 47, rIdx = 11, size 1
  -- #123 reg = _reading3  key 47, rIdx = 17, size 2
  -- #124 reg = _STATUS  key 48, rIdx = 3, size 1
  -- #125 reg = PCL  key 48, rIdx = 2, size 1
  -- #126 reg = _reading4  key 48, rIdx = 18, size 2
  -- #127 reg = KZ  key 49, rIdx = 32767, size 1
  -- #128 reg = _reading5  key 49, rIdx = 19, size 2
  -- #129 reg = _reading6  key 50, rIdx = 20, size 2
  -- #130 reg = _reading7  key 51, rIdx = 21, size 2
  -- #131 reg = _SerialTris  key 54, rIdx = 39, size 1
  -- #132 reg = INDF  key 56, rIdx = 0, size 1
  -- #133 reg = _Light0  key 56, rIdx = 0, size 1
  -- #134 reg = _numIntegrationSamples  key 56, rIdx = 22, size 1
  -- #135 reg = _SerialPort  key 57, rIdx = 38, size 1
  -- #136 reg = FSR  key 60, rIdx = 4, size 1
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
bitvector Size = 59 bSize = 8
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
bitvector Size = 59 bSize = 8
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
main.c(265:1:0:0:0)	 _entry($8) :
main.c(265:2:1:0:0)		proc _putc [k1 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{void function    }
main.c(265:3:2:0:0)	iTemp0 [k2 lr3:21 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _putc_byte_1_1}[r0x70 ] = recv 
main.c(269:4:4:0:0)		_SerialPort [k4 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{bit bit {0,1}} := 0x0 {literal bit {0,1}}
main.c(270:5:6:0:0)		_SerialTris [k6 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{bit bit {0,1}} := 0x0 {literal bit {0,1}}
main.c(271:6:7:0:0)		iTemp3 [k9 lr6:6 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{void} = call _serialBitDelay [k8 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{void function    }
main.c(275:8:38:0:0)		iTemp10 [k17 lr8:21 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _putc_bitnum_1_1}[r0x71 ] := 0x8 {literal unsigned char}
main.c(275:9:9:1:0)	 _dobody_4($4) :
main.c(276:10:10:1:0)		iTemp5 [k12 lr10:11 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{unsigned char} = iTemp0 [k2 lr3:21 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _putc_byte_1_1}[r0x70 ] & 0x1 {literal unsigned char}
main.c(276:11:11:1:0)		if iTemp5 [k12 lr10:11 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{unsigned char} == 0 goto _iffalse_10($2)
main.c(277:12:15:1:0)		_SerialPort [k4 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{bit bit {0,1}} := 0x1 {literal bit {0,1}}
main.c(277:13:16:1:0)		 goto _ifend_10($3)
main.c(277:14:17:1:0)	 _iffalse_10($2) :
main.c(279:15:19:1:0)		_SerialPort [k4 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{bit bit {0,1}} := 0x0 {literal bit {0,1}}
main.c(279:16:20:1:0)	 _ifend_10($3) :
main.c(280:17:21:1:0)		iTemp0 [k2 lr3:21 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _putc_byte_1_1}[r0x70 ] = iTemp0 [k2 lr3:21 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _putc_byte_1_1}[r0x70 ] >> 0x1 {literal unsigned char}
main.c(281:19:23:1:0)		iTemp9 [k16 lr19:19 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{void} = call _serialBitDelay [k8 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{void function    }
main.c(282:20:25:1:0)		iTemp10 [k17 lr8:21 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _putc_bitnum_1_1}[r0x71 ] = iTemp10 [k17 lr8:21 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _putc_bitnum_1_1}[r0x71 ] - 0x1 {literal unsigned char}
main.c(282:21:27:1:0)		if iTemp10 [k17 lr8:21 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _putc_bitnum_1_1}[r0x71 ] != 0 goto _dobody_4($4)
main.c(285:22:31:0:0)		_SerialPort [k4 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{bit bit {0,1}} := 0x1 {literal bit {0,1}}
main.c(286:23:32:0:0)		iTemp12 [k19 lr23:23 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{void} = call _serialBitDelay [k8 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{void function    }
main.c(287:24:34:0:0)		_SerialTris [k6 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{bit bit {0,1}} := 0x1 {literal bit {0,1}}
main.c(287:25:35:0:0)	 _return($7) :
main.c(287:26:36:0:0)		eproc _putc [k1 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{void function    }

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
main.c(275:7:37:0:0)	 preHeaderLbl0($10) :
main.c(275:8:38:0:0)		iTemp10 [k17 lr8:21 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _putc_bitnum_1_1}[r0x71 ] := 0x8 {literal unsigned char}
main.c(275:9:9:1:0)	 _dobody_4($4) :
main.c(276:10:10:1:0)		iTemp5 [k12 lr10:11 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{unsigned char} = iTemp0 [k2 lr3:21 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _putc_byte_1_1}[r0x70 ] & 0x1 {literal unsigned char}
main.c(276:11:11:1:0)		if iTemp5 [k12 lr10:11 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{unsigned char} == 0 goto _iffalse_10($2)
main.c(277:12:15:1:0)		_SerialPort [k4 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{bit bit {0,1}} := 0x1 {literal bit {0,1}}
main.c(277:13:16:1:0)		 goto _ifend_10($3)
main.c(277:14:17:1:0)	 _iffalse_10($2) :
main.c(279:15:19:1:0)		_SerialPort [k4 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{bit bit {0,1}} := 0x0 {literal bit {0,1}}
main.c(279:16:20:1:0)	 _ifend_10($3) :
main.c(280:17:21:1:0)		iTemp0 [k2 lr3:21 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _putc_byte_1_1}[r0x70 ] = iTemp0 [k2 lr3:21 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _putc_byte_1_1}[r0x70 ] >> 0x1 {literal unsigned char}
main.c(281:19:23:1:0)		iTemp9 [k16 lr19:19 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{void} = call _serialBitDelay [k8 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{void function    }
main.c(282:20:25:1:0)		iTemp10 [k17 lr8:21 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _putc_bitnum_1_1}[r0x71 ] = iTemp10 [k17 lr8:21 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _putc_bitnum_1_1}[r0x71 ] - 0x1 {literal unsigned char}
main.c(282:21:27:1:0)		if iTemp10 [k17 lr8:21 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _putc_bitnum_1_1}[r0x71 ] != 0 goto _dobody_4($4)
main.c(285:22:31:0:0)		_SerialPort [k4 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{bit bit {0,1}} := 0x1 {literal bit {0,1}}
main.c(286:23:32:0:0)		iTemp12 [k19 lr23:23 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{void} = call _serialBitDelay [k8 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{void function    }
main.c(287:24:34:0:0)		_SerialTris [k6 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{bit bit {0,1}} := 0x1 {literal bit {0,1}}
main.c(287:25:35:0:0)	 _return($7) :
main.c(287:26:36:0:0)		eproc _putc [k1 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{void function    }

----------------------------------------------------------------
Basic Block _dobody_4 : loop Depth = 1 noPath = 0 , lastinLoop = 0
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
main.c(275:9:9:1:0)	 _dobody_4($4) :
main.c(276:10:10:1:0)		iTemp5 [k12 lr10:11 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{unsigned char} = iTemp0 [k2 lr3:21 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _putc_byte_1_1}[r0x70 ] & 0x1 {literal unsigned char}
main.c(276:11:11:1:0)		if iTemp5 [k12 lr10:11 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{unsigned char} == 0 goto _iffalse_10($2)
main.c(277:12:15:1:0)		_SerialPort [k4 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{bit bit {0,1}} := 0x1 {literal bit {0,1}}
main.c(277:13:16:1:0)		 goto _ifend_10($3)
main.c(277:14:17:1:0)	 _iffalse_10($2) :
main.c(279:15:19:1:0)		_SerialPort [k4 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{bit bit {0,1}} := 0x0 {literal bit {0,1}}
main.c(279:16:20:1:0)	 _ifend_10($3) :
main.c(280:17:21:1:0)		iTemp0 [k2 lr3:21 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _putc_byte_1_1}[r0x70 ] = iTemp0 [k2 lr3:21 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _putc_byte_1_1}[r0x70 ] >> 0x1 {literal unsigned char}
main.c(281:19:23:1:0)		iTemp9 [k16 lr19:19 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{void} = call _serialBitDelay [k8 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{void function    }
main.c(282:20:25:1:0)		iTemp10 [k17 lr8:21 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _putc_bitnum_1_1}[r0x71 ] = iTemp10 [k17 lr8:21 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _putc_bitnum_1_1}[r0x71 ] - 0x1 {literal unsigned char}
main.c(282:21:27:1:0)		if iTemp10 [k17 lr8:21 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _putc_bitnum_1_1}[r0x71 ] != 0 goto _dobody_4($4)
main.c(285:22:31:0:0)		_SerialPort [k4 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{bit bit {0,1}} := 0x1 {literal bit {0,1}}
main.c(286:23:32:0:0)		iTemp12 [k19 lr23:23 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{void} = call _serialBitDelay [k8 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{void function    }
main.c(287:24:34:0:0)		_SerialTris [k6 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{bit bit {0,1}} := 0x1 {literal bit {0,1}}
main.c(287:25:35:0:0)	 _return($7) :
main.c(287:26:36:0:0)		eproc _putc [k1 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{void function    }

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
main.c(277:12:15:1:0)		_SerialPort [k4 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{bit bit {0,1}} := 0x1 {literal bit {0,1}}
main.c(277:13:16:1:0)		 goto _ifend_10($3)
main.c(277:14:17:1:0)	 _iffalse_10($2) :
main.c(279:15:19:1:0)		_SerialPort [k4 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{bit bit {0,1}} := 0x0 {literal bit {0,1}}
main.c(279:16:20:1:0)	 _ifend_10($3) :
main.c(280:17:21:1:0)		iTemp0 [k2 lr3:21 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _putc_byte_1_1}[r0x70 ] = iTemp0 [k2 lr3:21 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _putc_byte_1_1}[r0x70 ] >> 0x1 {literal unsigned char}
main.c(281:19:23:1:0)		iTemp9 [k16 lr19:19 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{void} = call _serialBitDelay [k8 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{void function    }
main.c(282:20:25:1:0)		iTemp10 [k17 lr8:21 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _putc_bitnum_1_1}[r0x71 ] = iTemp10 [k17 lr8:21 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _putc_bitnum_1_1}[r0x71 ] - 0x1 {literal unsigned char}
main.c(282:21:27:1:0)		if iTemp10 [k17 lr8:21 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _putc_bitnum_1_1}[r0x71 ] != 0 goto _dobody_4($4)
main.c(285:22:31:0:0)		_SerialPort [k4 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{bit bit {0,1}} := 0x1 {literal bit {0,1}}
main.c(286:23:32:0:0)		iTemp12 [k19 lr23:23 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{void} = call _serialBitDelay [k8 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{void function    }
main.c(287:24:34:0:0)		_SerialTris [k6 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{bit bit {0,1}} := 0x1 {literal bit {0,1}}
main.c(287:25:35:0:0)	 _return($7) :
main.c(287:26:36:0:0)		eproc _putc [k1 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{void function    }

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
main.c(277:14:17:1:0)	 _iffalse_10($2) :
main.c(279:15:19:1:0)		_SerialPort [k4 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{bit bit {0,1}} := 0x0 {literal bit {0,1}}
main.c(279:16:20:1:0)	 _ifend_10($3) :
main.c(280:17:21:1:0)		iTemp0 [k2 lr3:21 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _putc_byte_1_1}[r0x70 ] = iTemp0 [k2 lr3:21 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _putc_byte_1_1}[r0x70 ] >> 0x1 {literal unsigned char}
main.c(281:19:23:1:0)		iTemp9 [k16 lr19:19 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{void} = call _serialBitDelay [k8 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{void function    }
main.c(282:20:25:1:0)		iTemp10 [k17 lr8:21 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _putc_bitnum_1_1}[r0x71 ] = iTemp10 [k17 lr8:21 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _putc_bitnum_1_1}[r0x71 ] - 0x1 {literal unsigned char}
main.c(282:21:27:1:0)		if iTemp10 [k17 lr8:21 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _putc_bitnum_1_1}[r0x71 ] != 0 goto _dobody_4($4)
main.c(285:22:31:0:0)		_SerialPort [k4 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{bit bit {0,1}} := 0x1 {literal bit {0,1}}
main.c(286:23:32:0:0)		iTemp12 [k19 lr23:23 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{void} = call _serialBitDelay [k8 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{void function    }
main.c(287:24:34:0:0)		_SerialTris [k6 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{bit bit {0,1}} := 0x1 {literal bit {0,1}}
main.c(287:25:35:0:0)	 _return($7) :
main.c(287:26:36:0:0)		eproc _putc [k1 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{void function    }

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
main.c(279:16:20:1:0)	 _ifend_10($3) :
main.c(280:17:21:1:0)		iTemp0 [k2 lr3:21 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _putc_byte_1_1}[r0x70 ] = iTemp0 [k2 lr3:21 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _putc_byte_1_1}[r0x70 ] >> 0x1 {literal unsigned char}
main.c(281:19:23:1:0)		iTemp9 [k16 lr19:19 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{void} = call _serialBitDelay [k8 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{void function    }
main.c(282:20:25:1:0)		iTemp10 [k17 lr8:21 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _putc_bitnum_1_1}[r0x71 ] = iTemp10 [k17 lr8:21 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _putc_bitnum_1_1}[r0x71 ] - 0x1 {literal unsigned char}
main.c(282:21:27:1:0)		if iTemp10 [k17 lr8:21 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _putc_bitnum_1_1}[r0x71 ] != 0 goto _dobody_4($4)
main.c(285:22:31:0:0)		_SerialPort [k4 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{bit bit {0,1}} := 0x1 {literal bit {0,1}}
main.c(286:23:32:0:0)		iTemp12 [k19 lr23:23 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{void} = call _serialBitDelay [k8 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{void function    }
main.c(287:24:34:0:0)		_SerialTris [k6 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{bit bit {0,1}} := 0x1 {literal bit {0,1}}
main.c(287:25:35:0:0)	 _return($7) :
main.c(287:26:36:0:0)		eproc _putc [k1 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{void function    }

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
main.c(285:22:31:0:0)		_SerialPort [k4 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{bit bit {0,1}} := 0x1 {literal bit {0,1}}
main.c(286:23:32:0:0)		iTemp12 [k19 lr23:23 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{void} = call _serialBitDelay [k8 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{void function    }
main.c(287:24:34:0:0)		_SerialTris [k6 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{bit bit {0,1}} := 0x1 {literal bit {0,1}}
main.c(287:25:35:0:0)	 _return($7) :
main.c(287:26:36:0:0)		eproc _putc [k1 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{void function    }

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
main.c(287:25:35:0:0)	 _return($7) :
main.c(287:26:36:0:0)		eproc _putc [k1 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{void function    }
typeRegWithIdx - requesting index = 0x70
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x71
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x70
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x71
Found a Dynamic Register!
leaving
<><><><><><><><><><><><><><><><><>
<><><><><><><><><><><><><><><><><>
starting	ralloc.c:pic14_assignRegisters
ebbs before optimizing:

----------------------------------------------------------------
Basic Block _entry : loop Depth = 0 noPath = 0 , lastinLoop = 0
depth 1st num 9 : bbnum = 0 1st iCode = 1 , last iCode = 5
visited 0 : hasFcall = 0

defines bitVector :bitvector Size = 51 bSize = 7
Bits on { (3) (5) }

local defines bitVector :bitvector Size = 51 bSize = 7
Bits on { (3) (5) }

pointers Set bitvector :
in pointers Set bitvector :
inDefs Set bitvector :
outDefs Set bitvector :bitvector Size = 51 bSize = 7
Bits on { (3) (5) }

usesDefs Set bitvector :bitvector Size = 51 bSize = 7
Bits on { }

----------------------------------------------------------------
main.c(290:1:0:0:0)	 _entry($13) :
main.c(290:2:1:0:0)		proc _getc [k1 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{unsigned char function    }
main.c(294:3:3:0:0)		_SerialPort [k2 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{bit bit {0,1}} := 0x1 {literal bit {0,1}}
main.c(295:4:5:0:0)		_SerialTris [k4 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{bit bit {0,1}} := 0x1 {literal bit {0,1}}
main.c(296:5:6:0:0)	 nop 
----------------------------------------------------------------
Basic Block preHeaderLbl0 : loop Depth = 0 noPath = 0 , lastinLoop = 0
depth 1st num 10 : bbnum = 1 1st iCode = 6 , last iCode = 6
visited 0 : hasFcall = 0

defines bitVector :bitvector Size = 51 bSize = 7
Bits on { }

local defines bitVector :
pointers Set bitvector :
in pointers Set bitvector :
inDefs Set bitvector :bitvector Size = 51 bSize = 7
Bits on { (3) (5) }

outDefs Set bitvector :bitvector Size = 51 bSize = 7
Bits on { (3) (5) }

usesDefs Set bitvector :bitvector Size = 51 bSize = 7
Bits on { }

----------------------------------------------------------------
main.c(300:6:48:0:0)	 preHeaderLbl0($14) :

----------------------------------------------------------------
Basic Block _whilecontinue_2 : loop Depth = 1 noPath = 0 , lastinLoop = 0
depth 1st num 11 : bbnum = 2 1st iCode = 7 , last iCode = 8
visited 0 : hasFcall = 0

defines bitVector :bitvector Size = 51 bSize = 7
Bits on { }

local defines bitVector :
pointers Set bitvector :
in pointers Set bitvector :
inDefs Set bitvector :bitvector Size = 51 bSize = 7
Bits on { (2) (3) (4) (5) (14) (15) (16) (17) (18) (22) (23) (27) (28) (30) (32) (33) }

outDefs Set bitvector :bitvector Size = 51 bSize = 7
Bits on { (2) (3) (4) (5) (14) (15) (16) (17) (18) (22) (23) (27) (28) (30) (32) (33) }

usesDefs Set bitvector :bitvector Size = 51 bSize = 7
Bits on { }

----------------------------------------------------------------
main.c(300:7:8:1:0)	 _whilecontinue_2($1) :
main.c(300:8:9:1:0)		 goto _whilecontinue_2($1)

----------------------------------------------------------------
Basic Block _eBBlock0 : loop Depth = 1 noPath = 1 , lastinLoop = 0
depth 1st num 0 : bbnum = 3 1st iCode = 9 , last iCode = 14
visited 0 : hasFcall = 1

defines bitVector :bitvector Size = 50 bSize = 7
Bits on { (16) (17) (18) (20) }

local defines bitVector :bitvector Size = 50 bSize = 7
Bits on { (14) (15) (16) (17) (18) (19) (20) }

pointers Set bitvector :
in pointers Set bitvector :
inDefs Set bitvector :bitvector Size = 50 bSize = 7
Bits on { (2) (3) (4) (5) (14) (15) (16) (17) (18) (22) (23) (27) (28) (30) (32) (33) }

outDefs Set bitvector :bitvector Size = 50 bSize = 7
Bits on { (2) (3) (4) (5) (14) (15) (16) (17) (18) (20) (22) (27) (30) (32) }

usesDefs Set bitvector :bitvector Size = 50 bSize = 7
Bits on { (14) (15) (16) (17) (18) }

----------------------------------------------------------------
main.c(301:9:14:1:0)		iTemp2 [k7 lr9:9 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{void} = call _halfSerialBitDelay [k6 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{void function    }
main.c(302:10:15:1:0)		iTemp3 [k9 lr10:10 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{void} = call _serialBitDelay [k8 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{void function    }
main.c(304:11:16:1:0)		iTemp4 [k11 lr11:12 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{bit bit {0,1}} = not _Light3 [k10 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{bit bit {0,1}}
main.c(304:12:17:1:0)		iTemp5 [k12 lr12:13 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{bit bit {0,1}}{ sir@ _Light3} := iTemp4 [k11 lr11:12 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{bit bit {0,1}}
main.c(304:13:18:1:0)		_Light3 [k10 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{bit bit {0,1}} := iTemp5 [k12 lr12:13 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{bit bit {0,1}}{ sir@ _Light3}
main.c(308:14:20:1:0)		iTemp7 [k15 lr14:31 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _getc_byte_1_1} := 0x0 {literal unsigned char}

----------------------------------------------------------------
Basic Block preHeaderLbl1 : loop Depth = 1 noPath = 1 , lastinLoop = 0
depth 1st num 0 : bbnum = 4 1st iCode = 15 , last iCode = 15
visited 0 : hasFcall = 0

defines bitVector :bitvector Size = 50 bSize = 7
Bits on { }

local defines bitVector :
pointers Set bitvector :
in pointers Set bitvector :
inDefs Set bitvector :bitvector Size = 50 bSize = 7
Bits on { (2) (3) (4) (5) (14) (15) (16) (17) (18) (19) (20) (22) (27) (30) (32) }

outDefs Set bitvector :bitvector Size = 50 bSize = 7
Bits on { (2) (3) (4) (5) (14) (15) (16) (17) (18) (19) (20) (22) (27) (30) (32) }

usesDefs Set bitvector :bitvector Size = 50 bSize = 7
Bits on { }

----------------------------------------------------------------
main.c(309:15:49:1:0)	 preHeaderLbl1($17) :

----------------------------------------------------------------
Basic Block _dobody_6 : loop Depth = 2 noPath = 1 , lastinLoop = 0
depth 1st num 0 : bbnum = 5 1st iCode = 16 , last iCode = 19
visited 0 : hasFcall = 0

defines bitVector :bitvector Size = 50 bSize = 7
Bits on { (22) (23) }

local defines bitVector :bitvector Size = 50 bSize = 7
Bits on { (22) (23) }

pointers Set bitvector :
in pointers Set bitvector :
inDefs Set bitvector :bitvector Size = 50 bSize = 7
Bits on { (2) (3) (4) (5) (14) (15) (16) (17) (18) (19) (20) (22) (23) (27) (28) (30) (32) (33) }

outDefs Set bitvector :bitvector Size = 50 bSize = 7
Bits on { (2) (3) (4) (5) (14) (15) (16) (17) (18) (19) (22) (23) (27) (30) (32) (33) }

usesDefs Set bitvector :bitvector Size = 50 bSize = 7
Bits on { (3) (20) (22) (23) (28) }

----------------------------------------------------------------
main.c(309:16:21:2:0)	 _dobody_6($6) :
main.c(310:17:22:2:0)		iTemp8 [k17 lr17:18 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{unsigned char} = iTemp7 [k15 lr14:31 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _getc_byte_1_1} >> 0x1 {literal unsigned char}
main.c(310:18:23:2:0)		iTemp7 [k15 lr14:31 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _getc_byte_1_1} := iTemp8 [k17 lr17:18 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{unsigned char}
main.c(311:19:24:2:0)		if _SerialPort [k2 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{bit bit {0,1}} == 0 goto _iffalse_11($5)

----------------------------------------------------------------
Basic Block _eBBlock1 : loop Depth = 2 noPath = 1 , lastinLoop = 0
depth 1st num 0 : bbnum = 6 1st iCode = 20 , last iCode = 21
visited 0 : hasFcall = 0

defines bitVector :bitvector Size = 50 bSize = 7
Bits on { (27) (28) }

local defines bitVector :bitvector Size = 50 bSize = 7
Bits on { (27) (28) }

pointers Set bitvector :
in pointers Set bitvector :
inDefs Set bitvector :bitvector Size = 50 bSize = 7
Bits on { (2) (3) (4) (5) (14) (15) (16) (17) (18) (19) (22) (23) (27) (30) (32) (33) }

outDefs Set bitvector :bitvector Size = 50 bSize = 7
Bits on { (2) (3) (4) (5) (14) (15) (16) (17) (18) (19) (22) (27) (28) (30) (32) (33) }

usesDefs Set bitvector :bitvector Size = 50 bSize = 7
Bits on { (23) (27) }

----------------------------------------------------------------
main.c(312:20:27:2:0)		iTemp9 [k18 lr20:21 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{unsigned char} = iTemp7 [k15 lr14:31 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _getc_byte_1_1} | 0x80 {literal unsigned char}
main.c(312:21:28:2:0)		iTemp7 [k15 lr14:31 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _getc_byte_1_1} := iTemp9 [k18 lr20:21 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{unsigned char}

----------------------------------------------------------------
Basic Block _iffalse_11 : loop Depth = 2 noPath = 1 , lastinLoop = 1
depth 1st num 0 : bbnum = 7 1st iCode = 22 , last iCode = 25
visited 0 : hasFcall = 1

defines bitVector :bitvector Size = 50 bSize = 7
Bits on { (32) }

local defines bitVector :bitvector Size = 50 bSize = 7
Bits on { (30) (32) (33) }

pointers Set bitvector :
in pointers Set bitvector :
inDefs Set bitvector :bitvector Size = 50 bSize = 7
Bits on { (2) (3) (4) (5) (14) (15) (16) (17) (18) (19) (22) (23) (27) (28) (30) (32) (33) }

outDefs Set bitvector :bitvector Size = 50 bSize = 7
Bits on { (2) (3) (4) (5) (14) (15) (16) (17) (18) (22) (23) (27) (28) (30) (32) }

usesDefs Set bitvector :bitvector Size = 50 bSize = 7
Bits on { (19) (30) (32) (33) }

----------------------------------------------------------------
main.c(312:22:29:2:0)	 _iffalse_11($5) :
main.c(313:23:30:2:0)		iTemp10 [k19 lr23:23 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{void} = call _serialBitDelay [k8 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{void function    }
main.c(314:24:32:2:0)		iTemp11 [k20 lr24:25 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _getc_bitnum_1_1} = iTemp11 [k20 lr24:25 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _getc_bitnum_1_1} - 0x1 {literal unsigned char}
main.c(314:25:34:2:0)		if iTemp11 [k20 lr24:25 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _getc_bitnum_1_1} != 0 goto _dobody_6($6)

----------------------------------------------------------------
Basic Block _eBBlock2 : loop Depth = 1 noPath = 1 , lastinLoop = 0
depth 1st num 0 : bbnum = 8 1st iCode = 26 , last iCode = 26
visited 0 : hasFcall = 0

defines bitVector :bitvector Size = 50 bSize = 7
Bits on { }

local defines bitVector :
pointers Set bitvector :
in pointers Set bitvector :
inDefs Set bitvector :bitvector Size = 50 bSize = 7
Bits on { (2) (3) (4) (5) (14) (15) (16) (17) (18) (22) (23) (27) (28) (30) (32) (33) }

outDefs Set bitvector :bitvector Size = 50 bSize = 7
Bits on { (2) (3) (4) (5) (14) (15) (16) (17) (18) (22) (23) (27) (28) (30) (32) (33) }

usesDefs Set bitvector :bitvector Size = 50 bSize = 7
Bits on { (3) }

----------------------------------------------------------------
main.c(317:26:38:1:0)		if _SerialPort [k2 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{bit bit {0,1}} == 0 goto _whilecontinue_2($1)

----------------------------------------------------------------
Basic Block _eBBlock3 : loop Depth = 0 noPath = 1 , lastinLoop = 0
depth 1st num 0 : bbnum = 9 1st iCode = 27 , last iCode = 31
visited 0 : hasFcall = 0

defines bitVector :bitvector Size = 50 bSize = 7
Bits on { (42) (43) (44) }

local defines bitVector :bitvector Size = 50 bSize = 7
Bits on { (41) (42) (43) (44) }

pointers Set bitvector :
in pointers Set bitvector :
inDefs Set bitvector :bitvector Size = 50 bSize = 7
Bits on { (2) (3) (4) (5) (14) (15) (16) (17) (18) (22) (23) (27) (28) (30) (32) (33) }

outDefs Set bitvector :bitvector Size = 50 bSize = 7
Bits on { (2) (3) (4) (5) (14) (15) (16) (17) (18) (22) (23) (27) (28) (30) (32) (33) (41) (42) (43) (44) }

usesDefs Set bitvector :bitvector Size = 50 bSize = 7
Bits on { (23) (28) (41) (42) (43) }

----------------------------------------------------------------
main.c(318:27:41:0:0)		iTemp12 [k21 lr27:27 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{void} = call _serialBitDelay [k8 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{void function    }
main.c(320:28:42:0:0)		iTemp13 [k23 lr28:29 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{bit bit {0,1}} = not _Light2 [k22 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{bit bit {0,1}}
main.c(320:29:43:0:0)		iTemp14 [k24 lr29:30 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{bit bit {0,1}}{ sir@ _Light2} := iTemp13 [k23 lr28:29 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{bit bit {0,1}}
main.c(320:30:44:0:0)		_Light2 [k22 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{bit bit {0,1}} := iTemp14 [k24 lr29:30 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{bit bit {0,1}}{ sir@ _Light2}
main.c(322:31:45:0:0)		ret iTemp7 [k15 lr14:31 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _getc_byte_1_1}

----------------------------------------------------------------
Basic Block _return : loop Depth = 0 noPath = 1 , lastinLoop = 0
depth 1st num 0 : bbnum = 10 1st iCode = 32 , last iCode = 33
visited 0 : hasFcall = 0

defines bitVector :bitvector Size = 50 bSize = 7
Bits on { }

local defines bitVector :
pointers Set bitvector :
in pointers Set bitvector :
inDefs Set bitvector :bitvector Size = 50 bSize = 7
Bits on { (2) (3) (4) (5) (14) (15) (16) (17) (18) (22) (23) (27) (28) (30) (32) (33) (41) (42) (43) (44) }

outDefs Set bitvector :bitvector Size = 50 bSize = 7
Bits on { (2) (3) (4) (5) (14) (15) (16) (17) (18) (22) (23) (27) (28) (30) (32) (33) (41) (42) (43) (44) }

usesDefs Set bitvector :bitvector Size = 50 bSize = 7
Bits on { }

----------------------------------------------------------------
main.c(322:32:46:0:0)	 _return($12) :
main.c(322:33:47:0:0)		eproc _getc [k1 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{unsigned char function    }
packRegisters
packRegsForAssign
  result:_SerialPort [k2 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{bit bit {0,1}}
  left:
  right:0x1 {literal bit {0,1}}
allocDirReg symbol name _SerialPort
  637  storage class 13 
 642  integral
 646  specifier
_SerialPort [k2 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{bit bit {0,1}}
  -- and it is at a fixed address 0x4a
  2695 - result is not temp
  2705 - not packing - right is not temp
allocDirReg BAD, op is NULL
  result:_SerialPort [k2 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{bit bit {0,1}}
  left:
  right:0x1 {literal bit {0,1}}
packRegsForAssign
  result:_SerialTris [k4 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{bit bit {0,1}}
  left:
  right:0x1 {literal bit {0,1}}
allocDirReg symbol name _SerialTris
  637  storage class 13 
 642  integral
 646  specifier
_SerialTris [k4 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{bit bit {0,1}}
  -- and it is at a fixed address 0x44a
  2695 - result is not temp
  2705 - not packing - right is not temp
allocDirReg BAD, op is NULL
  result:_SerialTris [k4 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{bit bit {0,1}}
  left:
  right:0x1 {literal bit {0,1}}
  left:_getc [k1 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{unsigned char function    }
        Symbol type: unsigned char function    
  result:_SerialPort [k2 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{bit bit {0,1}}
          Symbol type: bit bit {0,1}
  3594 - pointer reg req = 0
  result:_SerialTris [k4 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{bit bit {0,1}}
          Symbol type: bit bit {0,1}
  3594 - pointer reg req = 0
packRegisters
packRegisters
packRegisters
packRegsForAssign
  result:iTemp5 [k12 lr12:13 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{bit bit {0,1}}{ sir@ _Light3}
  left:
  right:iTemp4 [k11 lr11:12 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{bit bit {0,1}}
  2760 - dic result key == ic right key -- pointer set=N
  packing. removing iTemp4
  replacing with iTemp4
  result:iTemp5 [k12 lr11:13 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{bit bit {0,1}}{ sir@ _Light3}
  left:
  right:iTemp4 [k11 lr11:12 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{bit bit {0,1}}
packRegsForAssign
  result:_Light3 [k10 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{bit bit {0,1}}
  left:
  right:iTemp5 [k12 lr11:13 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{bit bit {0,1}}{ sir@ _Light3}
allocDirReg symbol name _Light3
  637  storage class 13 
 642  integral
 646  specifier
_Light3 [k10 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{bit bit {0,1}}
  -- added _Light3 to hash, size = 1
  -- and it is at a fixed address 0x33
  2695 - result is not temp
  2760 - dic result key == ic right key -- pointer set=N
  packing. removing iTemp5
  replacing with iTemp5
  result:_Light3 [k10 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{bit bit {0,1}}
  left:
  right:iTemp5 [k12 lr11:13 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{bit bit {0,1}}{ sir@ _Light3}
packRegsForAssign
  result:iTemp7 [k15 lr14:31 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _getc_byte_1_1}
  left:
  right:0x0 {literal unsigned char}
  2705 - not packing - right is not temp
allocDirReg BAD, op is NULL
  result:iTemp7 [k15 lr14:31 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _getc_byte_1_1}
  left:
  right:0x0 {literal unsigned char}
packRegsForAssign
  result:iTemp7 [k15 lr14:31 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _getc_byte_1_1}
  left:
  right:0x0 {literal unsigned char}
  2705 - not packing - right is not temp
allocDirReg BAD, op is NULL
  result:iTemp7 [k15 lr14:31 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _getc_byte_1_1}
  left:
  right:0x0 {literal unsigned char}
  left:_halfSerialBitDelay [k6 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{void function    }
        Symbol type: void function    
  result:iTemp2 [k7 lr9:9 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{void}
          Symbol type: void
  3594 - pointer reg req = 0
  left:_serialBitDelay [k8 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{void function    }
        Symbol type: void function    
  result:iTemp3 [k9 lr10:10 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{void}
          Symbol type: void
  3594 - pointer reg req = 0
  left:_Light3 [k10 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{bit bit {0,1}}
bit         Symbol type: bit bit {0,1}
  result:_Light3 [k10 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{bit bit {0,1}}
          Symbol type: bit bit {0,1}
  3594 - pointer reg req = 0
  result:iTemp7 [k15 lr14:31 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _getc_byte_1_1}
          Symbol type: unsigned char
  3594 - pointer reg req = 0
packRegisters
packRegisters
packRegsForAssign
  result:iTemp7 [k15 lr14:31 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _getc_byte_1_1}
  left:
  right:iTemp8 [k17 lr17:18 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{unsigned char}
  2760 - dic result key == ic right key -- pointer set=N
  packing. removing iTemp8
  replacing with iTemp8
  result:iTemp7 [k15 lr14:31 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _getc_byte_1_1}
  left:
  right:iTemp8 [k17 lr17:18 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{unsigned char}
  left:iTemp7 [k15 lr14:31 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _getc_byte_1_1}
        Symbol type: unsigned char
  result:iTemp7 [k15 lr14:31 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _getc_byte_1_1}
          Symbol type: unsigned char
  3594 - pointer reg req = 0
packRegsForAccUse
  packRegsForAccUse:3181
  packRegsForAccUse:3188
  left:_SerialPort [k2 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{bit bit {0,1}}
bit         Symbol type: bit bit {0,1}
  3594 - pointer reg req = 0
packRegisters
packRegsForAssign
  result:iTemp7 [k15 lr14:31 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _getc_byte_1_1}
  left:
  right:iTemp9 [k18 lr20:21 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{unsigned char}
  2760 - dic result key == ic right key -- pointer set=N
  packing. removing iTemp9
  replacing with iTemp9
  result:iTemp7 [k15 lr14:31 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _getc_byte_1_1}
  left:
  right:iTemp9 [k18 lr20:21 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{unsigned char}
  left:iTemp7 [k15 lr14:31 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _getc_byte_1_1}
        Symbol type: unsigned char
  result:iTemp7 [k15 lr14:31 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _getc_byte_1_1}
          Symbol type: unsigned char
  3594 - pointer reg req = 0
isBitwiseOptimizable
packRegsForAccUse
  packRegsForAccUse:3181
  packRegsForAccUse:3188
packRegisters
  left:_serialBitDelay [k8 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{void function    }
        Symbol type: void function    
  result:iTemp10 [k19 lr23:23 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{void}
          Symbol type: void
  3594 - pointer reg req = 0
  left:iTemp11 [k20 lr24:25 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _getc_bitnum_1_1}
        Symbol type: unsigned char
  result:iTemp11 [k20 lr24:25 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _getc_bitnum_1_1}
          Symbol type: unsigned char
  3594 - pointer reg req = 0
packRegsForSupport
packRegsForAccUse
  packRegsForAccUse:3181
  packRegsForAccUse:3188
  left:iTemp11 [k20 lr24:25 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _getc_bitnum_1_1}
        Symbol type: unsigned char
  3594 - pointer reg req = 0
packRegisters
  left:_SerialPort [k2 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{bit bit {0,1}}
bit         Symbol type: bit bit {0,1}
  3594 - pointer reg req = 0
packRegisters
packRegsForAssign
  result:iTemp14 [k24 lr29:30 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{bit bit {0,1}}{ sir@ _Light2}
  left:
  right:iTemp13 [k23 lr28:29 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{bit bit {0,1}}
  2760 - dic result key == ic right key -- pointer set=N
  packing. removing iTemp13
  replacing with iTemp13
  result:iTemp14 [k24 lr28:30 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{bit bit {0,1}}{ sir@ _Light2}
  left:
  right:iTemp13 [k23 lr28:29 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{bit bit {0,1}}
packRegsForAssign
  result:_Light2 [k22 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{bit bit {0,1}}
  left:
  right:iTemp14 [k24 lr28:30 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{bit bit {0,1}}{ sir@ _Light2}
allocDirReg symbol name _Light2
  637  storage class 13 
 642  integral
 646  specifier
_Light2 [k22 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{bit bit {0,1}}
  -- added _Light2 to hash, size = 1
  -- and it is at a fixed address 0x32
  2695 - result is not temp
  2760 - dic result key == ic right key -- pointer set=N
  packing. removing iTemp14
  replacing with iTemp14
  result:_Light2 [k22 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{bit bit {0,1}}
  left:
  right:iTemp14 [k24 lr28:30 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{bit bit {0,1}}{ sir@ _Light2}
  left:_serialBitDelay [k8 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{void function    }
        Symbol type: void function    
  result:iTemp12 [k21 lr27:27 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{void}
          Symbol type: void
  3594 - pointer reg req = 0
  left:_Light2 [k22 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{bit bit {0,1}}
bit         Symbol type: bit bit {0,1}
  result:_Light2 [k22 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{bit bit {0,1}}
          Symbol type: bit bit {0,1}
  3594 - pointer reg req = 0
  left:iTemp7 [k15 lr14:31 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _getc_byte_1_1}
        Symbol type: unsigned char
  3594 - pointer reg req = 0
packRegisters
  left:_getc [k1 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{unsigned char function    }
        Symbol type: unsigned char function    
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
  -- #75 reg = r0x63  key 0, rIdx = 99, size 1
  -- #76 reg = r0x64  key 0, rIdx = 100, size 1
  -- #77 reg = _ADRESH  key 0, rIdx = 36, size 1
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
  -- #88 reg = r0x6F  key 0, rIdx = 111, size 1
  -- #89 reg = r0x70  key 0, rIdx = 112, size 1
  -- #90 reg = r0x71  key 0, rIdx = 113, size 1
  -- #91 reg = _ADRESL  key 5, rIdx = 37, size 1
  -- #92 reg = _PCLATH  key 6, rIdx = 10, size 1
  -- #93 reg = _TRISA  key 7, rIdx = 1, size 1
  -- #94 reg = _TRISB  key 8, rIdx = 2, size 1
  -- #95 reg = _TRISB  key 8, rIdx = 24, size 1
  -- #96 reg = _TRISB  key 8, rIdx = 26, size 1
  -- #97 reg = _TRISB  key 8, rIdx = 35, size 1
  -- #98 reg = _TRISC  key 9, rIdx = 3, size 1
  -- #99 reg = _TRISD  key 10, rIdx = 4, size 1
  -- #100 reg = _PORTA  key 10, rIdx = 6, size 1
  -- #101 reg = _TRISE  key 11, rIdx = 5, size 1
  -- #102 reg = _PORTB  key 11, rIdx = 7, size 1
  -- #103 reg = _PORTB  key 11, rIdx = 23, size 1
  -- #104 reg = _PORTB  key 11, rIdx = 33, size 1
  -- #105 reg = _PORTC  key 12, rIdx = 8, size 1
  -- #106 reg = _GIE  key 12, rIdx = 13, size 1
  -- #107 reg = _PORTD  key 13, rIdx = 9, size 1
  -- #108 reg = _PORTE  key 14, rIdx = 10, size 1
  -- #109 reg = _numResonationHalfcycles  key 14, rIdx = 32, size 1
  -- #110 reg = _sensorSample_phase0_1_1  key 15, rIdx = 28, size 1
  -- #111 reg = _sensorSample_phase1_1_1  key 16, rIdx = 29, size 1
  -- #112 reg = _sensorSample_phase2_1_1  key 17, rIdx = 30, size 1
  -- #113 reg = _sensorSample_phase3_1_1  key 18, rIdx = 31, size 1
  -- #114 reg = _ADCON0  key 27, rIdx = 12, size 1
  -- #115 reg = _ADCON0  key 27, rIdx = 25, size 1
  -- #116 reg = _ADCON0  key 27, rIdx = 27, size 1
  -- #117 reg = _ADCON1  key 28, rIdx = 11, size 1
  -- #118 reg = SSAVE  key 32, rIdx = 32765, size 1
  -- #119 reg = WSAVE  key 36, rIdx = 32766, size 1
  -- #120 reg = _reading0  key 44, rIdx = 14, size 2
  -- #121 reg = _GO_DONE  key 44, rIdx = 34, size 1
  -- #122 reg = _reading1  key 45, rIdx = 15, size 2
  -- #123 reg = _reading2  key 46, rIdx = 16, size 2
  -- #124 reg = INTCON  key 47, rIdx = 11, size 1
  -- #125 reg = _reading3  key 47, rIdx = 17, size 2
  -- #126 reg = _STATUS  key 48, rIdx = 3, size 1
  -- #127 reg = PCL  key 48, rIdx = 2, size 1
  -- #128 reg = _reading4  key 48, rIdx = 18, size 2
  -- #129 reg = KZ  key 49, rIdx = 32767, size 1
  -- #130 reg = _reading5  key 49, rIdx = 19, size 2
  -- #131 reg = _reading6  key 50, rIdx = 20, size 2
  -- #132 reg = _reading7  key 51, rIdx = 21, size 2
  -- #133 reg = _SerialTris  key 54, rIdx = 39, size 1
  -- #134 reg = INDF  key 56, rIdx = 0, size 1
  -- #135 reg = _Light0  key 56, rIdx = 0, size 1
  -- #136 reg = _numIntegrationSamples  key 56, rIdx = 22, size 1
  -- #137 reg = _SerialPort  key 57, rIdx = 38, size 1
  -- #138 reg = _Light2  key 59, rIdx = 41, size 1
  -- #139 reg = FSR  key 60, rIdx = 4, size 1
  -- #140 reg = _Light3  key 60, rIdx = 40, size 1
regTypeNum
  2421 - iTemp2
  2421 - iTemp3
  2421 - iTemp4
  2431 - itemp register
  reg name iTemp4,  reg type REG_GPR
  2421 - iTemp5
  2431 - itemp register
  reg name iTemp5,  reg type REG_GPR
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
  2421 - iTemp11
  2431 - itemp register
  reg name iTemp11,  reg type REG_GPR
  2421 - iTemp12
  2421 - iTemp13
  2431 - itemp register
  reg name iTemp13,  reg type REG_GPR
  2421 - iTemp14
  2431 - itemp register
  reg name iTemp14,  reg type REG_GPR
serialRegAssign
  op: LABEL
deassignLRs
  op: unkown op 9 	
deassignLRs
  op: =
deassignLRs
  op: =
deassignLRs
  op: INLINEASM
deassignLRs
  op: LABEL
deassignLRs
  op: LABEL
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
ebbs after optimizing:

----------------------------------------------------------------
Basic Block _entry : loop Depth = 0 noPath = 0 , lastinLoop = 0
depth 1st num 9 : bbnum = 0 1st iCode = 1 , last iCode = 5
visited 0 : hasFcall = 0

defines bitVector :bitvector Size = 51 bSize = 7
Bits on { (3) (5) }

local defines bitVector :bitvector Size = 51 bSize = 7
Bits on { (3) (5) }

pointers Set bitvector :
in pointers Set bitvector :
inDefs Set bitvector :
outDefs Set bitvector :bitvector Size = 51 bSize = 7
Bits on { (3) (5) }

usesDefs Set bitvector :bitvector Size = 51 bSize = 7
Bits on { }

----------------------------------------------------------------
main.c(290:1:0:0:0)	 _entry($13) :
main.c(290:2:1:0:0)		proc _getc [k1 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{unsigned char function    }
main.c(294:3:3:0:0)		_SerialPort [k2 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{bit bit {0,1}} := 0x1 {literal bit {0,1}}
main.c(295:4:5:0:0)		_SerialTris [k4 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{bit bit {0,1}} := 0x1 {literal bit {0,1}}
main.c(296:5:6:0:0)	 nop main.c(300:7:8:1:0)	 _whilecontinue_2($1) :
main.c(300:8:9:1:0)		 goto _whilecontinue_2($1)
main.c(322:32:46:0:0)	 _return($12) :
main.c(322:33:47:0:0)		eproc _getc [k1 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{unsigned char function    }

----------------------------------------------------------------
Basic Block preHeaderLbl0 : loop Depth = 0 noPath = 0 , lastinLoop = 0
depth 1st num 10 : bbnum = 1 1st iCode = 6 , last iCode = 6
visited 0 : hasFcall = 0

defines bitVector :bitvector Size = 51 bSize = 7
Bits on { }

local defines bitVector :
pointers Set bitvector :
in pointers Set bitvector :
inDefs Set bitvector :bitvector Size = 51 bSize = 7
Bits on { (3) (5) }

outDefs Set bitvector :bitvector Size = 51 bSize = 7
Bits on { (3) (5) }

usesDefs Set bitvector :bitvector Size = 51 bSize = 7
Bits on { }

----------------------------------------------------------------
main.c(300:6:48:0:0)	 preHeaderLbl0($14) :
main.c(300:7:8:1:0)	 _whilecontinue_2($1) :
main.c(300:8:9:1:0)		 goto _whilecontinue_2($1)
main.c(322:32:46:0:0)	 _return($12) :
main.c(322:33:47:0:0)		eproc _getc [k1 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{unsigned char function    }

----------------------------------------------------------------
Basic Block _whilecontinue_2 : loop Depth = 1 noPath = 0 , lastinLoop = 0
depth 1st num 11 : bbnum = 2 1st iCode = 7 , last iCode = 8
visited 0 : hasFcall = 0

defines bitVector :bitvector Size = 51 bSize = 7
Bits on { }

local defines bitVector :
pointers Set bitvector :
in pointers Set bitvector :
inDefs Set bitvector :bitvector Size = 51 bSize = 7
Bits on { (2) (3) (4) (5) (14) (15) (16) (17) (18) (22) (23) (27) (28) (30) (32) (33) }

outDefs Set bitvector :bitvector Size = 51 bSize = 7
Bits on { (2) (3) (4) (5) (14) (15) (16) (17) (18) (22) (23) (27) (28) (30) (32) (33) }

usesDefs Set bitvector :bitvector Size = 51 bSize = 7
Bits on { }

----------------------------------------------------------------
main.c(300:7:8:1:0)	 _whilecontinue_2($1) :
main.c(300:8:9:1:0)		 goto _whilecontinue_2($1)
main.c(322:32:46:0:0)	 _return($12) :
main.c(322:33:47:0:0)		eproc _getc [k1 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{unsigned char function    }

----------------------------------------------------------------
Basic Block _eBBlock0 : loop Depth = 1 noPath = 1 , lastinLoop = 0
depth 1st num 0 : bbnum = 3 1st iCode = 9 , last iCode = 14
visited 0 : hasFcall = 1

defines bitVector :bitvector Size = 50 bSize = 7
Bits on { (16) (17) (18) (20) }

local defines bitVector :bitvector Size = 50 bSize = 7
Bits on { (14) (15) (16) (17) (18) (19) (20) }

pointers Set bitvector :
in pointers Set bitvector :
inDefs Set bitvector :bitvector Size = 50 bSize = 7
Bits on { (2) (3) (4) (5) (14) (15) (16) (17) (18) (22) (23) (27) (28) (30) (32) (33) }

outDefs Set bitvector :bitvector Size = 50 bSize = 7
Bits on { (2) (3) (4) (5) (14) (15) (16) (17) (18) (20) (22) (27) (30) (32) }

usesDefs Set bitvector :bitvector Size = 50 bSize = 7
Bits on { (14) (15) (16) (17) (18) }

----------------------------------------------------------------
main.c(301:9:14:1:0)		iTemp2 [k7 lr9:9 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{void} = call _halfSerialBitDelay [k6 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{void function    }
main.c(302:10:15:1:0)		iTemp3 [k9 lr10:10 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{void} = call _serialBitDelay [k8 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{void function    }
main.c(304:11:16:1:0)		_Light3 [k10 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{bit bit {0,1}} = not _Light3 [k10 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{bit bit {0,1}}
main.c(308:14:20:1:0)		iTemp7 [k15 lr14:31 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _getc_byte_1_1}[err ] := 0x0 {literal unsigned char}

----------------------------------------------------------------
Basic Block preHeaderLbl1 : loop Depth = 1 noPath = 1 , lastinLoop = 0
depth 1st num 0 : bbnum = 4 1st iCode = 15 , last iCode = 15
visited 0 : hasFcall = 0

defines bitVector :bitvector Size = 50 bSize = 7
Bits on { }

local defines bitVector :
pointers Set bitvector :
in pointers Set bitvector :
inDefs Set bitvector :bitvector Size = 50 bSize = 7
Bits on { (2) (3) (4) (5) (14) (15) (16) (17) (18) (19) (20) (22) (27) (30) (32) }

outDefs Set bitvector :bitvector Size = 50 bSize = 7
Bits on { (2) (3) (4) (5) (14) (15) (16) (17) (18) (19) (20) (22) (27) (30) (32) }

usesDefs Set bitvector :bitvector Size = 50 bSize = 7
Bits on { }

----------------------------------------------------------------
main.c(309:15:49:1:0)	 preHeaderLbl1($17) :

----------------------------------------------------------------
Basic Block _dobody_6 : loop Depth = 2 noPath = 1 , lastinLoop = 0
depth 1st num 0 : bbnum = 5 1st iCode = 16 , last iCode = 19
visited 0 : hasFcall = 0

defines bitVector :bitvector Size = 50 bSize = 7
Bits on { (22) (23) }

local defines bitVector :bitvector Size = 50 bSize = 7
Bits on { (22) (23) }

pointers Set bitvector :
in pointers Set bitvector :
inDefs Set bitvector :bitvector Size = 50 bSize = 7
Bits on { (2) (3) (4) (5) (14) (15) (16) (17) (18) (19) (20) (22) (23) (27) (28) (30) (32) (33) }

outDefs Set bitvector :bitvector Size = 50 bSize = 7
Bits on { (2) (3) (4) (5) (14) (15) (16) (17) (18) (19) (22) (23) (27) (30) (32) (33) }

usesDefs Set bitvector :bitvector Size = 50 bSize = 7
Bits on { (3) (20) (22) (23) (28) }

----------------------------------------------------------------
main.c(309:16:21:2:0)	 _dobody_6($6) :
main.c(310:17:22:2:0)		iTemp7 [k15 lr14:31 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _getc_byte_1_1}[err ] = iTemp7 [k15 lr14:31 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _getc_byte_1_1}[err ] >> 0x1 {literal unsigned char}
main.c(311:19:24:2:0)		if _SerialPort [k2 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{bit bit {0,1}} == 0 goto _iffalse_11($5)

----------------------------------------------------------------
Basic Block _eBBlock1 : loop Depth = 2 noPath = 1 , lastinLoop = 0
depth 1st num 0 : bbnum = 6 1st iCode = 20 , last iCode = 21
visited 0 : hasFcall = 0

defines bitVector :bitvector Size = 50 bSize = 7
Bits on { (27) (28) }

local defines bitVector :bitvector Size = 50 bSize = 7
Bits on { (27) (28) }

pointers Set bitvector :
in pointers Set bitvector :
inDefs Set bitvector :bitvector Size = 50 bSize = 7
Bits on { (2) (3) (4) (5) (14) (15) (16) (17) (18) (19) (22) (23) (27) (30) (32) (33) }

outDefs Set bitvector :bitvector Size = 50 bSize = 7
Bits on { (2) (3) (4) (5) (14) (15) (16) (17) (18) (19) (22) (27) (28) (30) (32) (33) }

usesDefs Set bitvector :bitvector Size = 50 bSize = 7
Bits on { (23) (27) }

----------------------------------------------------------------
main.c(312:20:27:2:0)		iTemp7 [k15 lr14:31 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _getc_byte_1_1}[err ] = iTemp7 [k15 lr14:31 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _getc_byte_1_1}[err ] | 0x80 {literal unsigned char}

----------------------------------------------------------------
Basic Block _iffalse_11 : loop Depth = 2 noPath = 1 , lastinLoop = 1
depth 1st num 0 : bbnum = 7 1st iCode = 22 , last iCode = 25
visited 0 : hasFcall = 1

defines bitVector :bitvector Size = 50 bSize = 7
Bits on { (32) }

local defines bitVector :bitvector Size = 50 bSize = 7
Bits on { (30) (32) (33) }

pointers Set bitvector :
in pointers Set bitvector :
inDefs Set bitvector :bitvector Size = 50 bSize = 7
Bits on { (2) (3) (4) (5) (14) (15) (16) (17) (18) (19) (22) (23) (27) (28) (30) (32) (33) }

outDefs Set bitvector :bitvector Size = 50 bSize = 7
Bits on { (2) (3) (4) (5) (14) (15) (16) (17) (18) (22) (23) (27) (28) (30) (32) }

usesDefs Set bitvector :bitvector Size = 50 bSize = 7
Bits on { (19) (30) (32) (33) }

----------------------------------------------------------------
main.c(312:22:29:2:0)	 _iffalse_11($5) :
main.c(313:23:30:2:0)		iTemp10 [k19 lr23:23 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{void} = call _serialBitDelay [k8 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{void function    }
main.c(314:24:32:2:0)		iTemp11 [k20 lr24:25 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _getc_bitnum_1_1}[err ] = iTemp11 [k20 lr24:25 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _getc_bitnum_1_1}[err ] - 0x1 {literal unsigned char}
main.c(314:25:34:2:0)		if iTemp11 [k20 lr24:25 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _getc_bitnum_1_1}[err ] != 0 goto _dobody_6($6)

----------------------------------------------------------------
Basic Block _eBBlock2 : loop Depth = 1 noPath = 1 , lastinLoop = 0
depth 1st num 0 : bbnum = 8 1st iCode = 26 , last iCode = 26
visited 0 : hasFcall = 0

defines bitVector :bitvector Size = 50 bSize = 7
Bits on { }

local defines bitVector :
pointers Set bitvector :
in pointers Set bitvector :
inDefs Set bitvector :bitvector Size = 50 bSize = 7
Bits on { (2) (3) (4) (5) (14) (15) (16) (17) (18) (22) (23) (27) (28) (30) (32) (33) }

outDefs Set bitvector :bitvector Size = 50 bSize = 7
Bits on { (2) (3) (4) (5) (14) (15) (16) (17) (18) (22) (23) (27) (28) (30) (32) (33) }

usesDefs Set bitvector :bitvector Size = 50 bSize = 7
Bits on { (3) }

----------------------------------------------------------------
main.c(317:26:38:1:0)		if _SerialPort [k2 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{bit bit {0,1}} == 0 goto _whilecontinue_2($1)

----------------------------------------------------------------
Basic Block _eBBlock3 : loop Depth = 0 noPath = 1 , lastinLoop = 0
depth 1st num 0 : bbnum = 9 1st iCode = 27 , last iCode = 31
visited 0 : hasFcall = 0

defines bitVector :bitvector Size = 50 bSize = 7
Bits on { (42) (43) (44) }

local defines bitVector :bitvector Size = 50 bSize = 7
Bits on { (41) (42) (43) (44) }

pointers Set bitvector :
in pointers Set bitvector :
inDefs Set bitvector :bitvector Size = 50 bSize = 7
Bits on { (2) (3) (4) (5) (14) (15) (16) (17) (18) (22) (23) (27) (28) (30) (32) (33) }

outDefs Set bitvector :bitvector Size = 50 bSize = 7
Bits on { (2) (3) (4) (5) (14) (15) (16) (17) (18) (22) (23) (27) (28) (30) (32) (33) (41) (42) (43) (44) }

usesDefs Set bitvector :bitvector Size = 50 bSize = 7
Bits on { (23) (28) (41) (42) (43) }

----------------------------------------------------------------
main.c(318:27:41:0:0)		iTemp12 [k21 lr27:27 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{void} = call _serialBitDelay [k8 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{void function    }
main.c(320:28:42:0:0)		_Light2 [k22 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{bit bit {0,1}} = not _Light2 [k22 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{bit bit {0,1}}
main.c(322:31:45:0:0)		ret iTemp7 [k15 lr14:31 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _getc_byte_1_1}[err ]

----------------------------------------------------------------
Basic Block _return : loop Depth = 0 noPath = 1 , lastinLoop = 0
depth 1st num 0 : bbnum = 10 1st iCode = 32 , last iCode = 33
visited 0 : hasFcall = 0

defines bitVector :bitvector Size = 50 bSize = 7
Bits on { }

local defines bitVector :
pointers Set bitvector :
in pointers Set bitvector :
inDefs Set bitvector :bitvector Size = 50 bSize = 7
Bits on { (2) (3) (4) (5) (14) (15) (16) (17) (18) (22) (23) (27) (28) (30) (32) (33) (41) (42) (43) (44) }

outDefs Set bitvector :bitvector Size = 50 bSize = 7
Bits on { (2) (3) (4) (5) (14) (15) (16) (17) (18) (22) (23) (27) (28) (30) (32) (33) (41) (42) (43) (44) }

usesDefs Set bitvector :bitvector Size = 50 bSize = 7
Bits on { }

----------------------------------------------------------------
main.c(322:32:46:0:0)	 _return($12) :
main.c(322:33:47:0:0)		eproc _getc [k1 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{unsigned char function    }
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
main.c(326:1:0:0:0)	 _entry($2) :
main.c(326:2:1:0:0)		proc _sendPacket [k1 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{void function    }
main.c(331:3:3:0:0)		send 0x80 {literal unsigned char}{argreg = 1}
main.c(331:4:4:0:0)		iTemp1 [k5 lr4:4 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{void} = call _putc [k4 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{void function    }
main.c(333:5:5:0:0)		iTemp3 [k9 lr5:6 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int} = _reading0 [k8 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int} >> 0x8 {literal unsigned char}
main.c(333:6:6:0:0)		iTemp4 [k10 lr6:7 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sendPacket_h_1_1} = (unsigned char)iTemp3 [k9 lr5:6 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int}
main.c(333:7:7:0:0)		iTemp2 [k6 lr7:98 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sendPacket_h_1_1} := iTemp4 [k10 lr6:7 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sendPacket_h_1_1}
main.c(334:8:8:0:0)		iTemp6 [k13 lr8:9 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sendPacket_l_1_1} = (unsigned char)_reading0 [k8 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int}
main.c(334:9:9:0:0)		iTemp5 [k11 lr9:98 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sendPacket_l_1_1} := iTemp6 [k13 lr8:9 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sendPacket_l_1_1}
main.c(335:10:10:0:0)		send iTemp2 [k6 lr7:98 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sendPacket_h_1_1}{argreg = 1}
main.c(335:11:11:0:0)		iTemp7 [k14 lr11:11 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{void} = call _putc [k4 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{void function    }
main.c(336:12:12:0:0)		send iTemp5 [k11 lr9:98 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sendPacket_l_1_1}{argreg = 1}
main.c(336:13:13:0:0)		iTemp8 [k15 lr13:13 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{void} = call _putc [k4 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{void function    }
main.c(337:14:14:0:0)		iTemp9 [k16 lr14:15 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{unsigned char} = iTemp2 [k6 lr7:98 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sendPacket_h_1_1} + iTemp5 [k11 lr9:98 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sendPacket_l_1_1}
main.c(337:15:15:0:0)		iTemp10 [k17 lr15:16 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sendPacket_checksum_1_1} := iTemp9 [k16 lr14:15 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{unsigned char}
main.c(337:16:16:0:0)		iTemp0 [k2 lr16:101 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sendPacket_checksum_1_1} := iTemp10 [k17 lr15:16 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sendPacket_checksum_1_1}
main.c(339:17:17:0:0)		iTemp11 [k19 lr17:18 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int} = _reading1 [k18 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int} >> 0x8 {literal unsigned char}
main.c(339:18:18:0:0)		iTemp12 [k20 lr18:19 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{unsigned char} = (unsigned char)iTemp11 [k19 lr17:18 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int}
main.c(339:19:19:0:0)		iTemp2 [k6 lr7:98 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sendPacket_h_1_1} := iTemp12 [k20 lr18:19 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{unsigned char}
main.c(340:20:20:0:0)		iTemp13 [k21 lr20:21 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{unsigned char} = (unsigned char)_reading1 [k18 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int}
main.c(340:21:21:0:0)		iTemp5 [k11 lr9:98 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sendPacket_l_1_1} := iTemp13 [k21 lr20:21 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{unsigned char}
main.c(341:22:22:0:0)		send iTemp2 [k6 lr7:98 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sendPacket_h_1_1}{argreg = 1}
main.c(341:23:23:0:0)		iTemp14 [k22 lr23:23 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{void} = call _putc [k4 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{void function    }
main.c(342:24:24:0:0)		send iTemp5 [k11 lr9:98 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sendPacket_l_1_1}{argreg = 1}
main.c(342:25:25:0:0)		iTemp15 [k23 lr25:25 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{void} = call _putc [k4 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{void function    }
main.c(343:26:26:0:0)		iTemp16 [k24 lr26:27 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{unsigned char} = iTemp2 [k6 lr7:98 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sendPacket_h_1_1} + iTemp5 [k11 lr9:98 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sendPacket_l_1_1}
main.c(343:27:27:0:0)		iTemp17 [k25 lr27:28 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{unsigned char} = iTemp0 [k2 lr16:101 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sendPacket_checksum_1_1} + iTemp16 [k24 lr26:27 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{unsigned char}
main.c(343:28:28:0:0)		iTemp0 [k2 lr16:101 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sendPacket_checksum_1_1} := iTemp17 [k25 lr27:28 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{unsigned char}
main.c(345:29:29:0:0)		iTemp18 [k27 lr29:30 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int} = _reading2 [k26 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int} >> 0x8 {literal unsigned char}
main.c(345:30:30:0:0)		iTemp19 [k28 lr30:31 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{unsigned char} = (unsigned char)iTemp18 [k27 lr29:30 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int}
main.c(345:31:31:0:0)		iTemp2 [k6 lr7:98 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sendPacket_h_1_1} := iTemp19 [k28 lr30:31 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{unsigned char}
main.c(346:32:32:0:0)		iTemp20 [k29 lr32:33 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{unsigned char} = (unsigned char)_reading2 [k26 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int}
main.c(346:33:33:0:0)		iTemp5 [k11 lr9:98 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sendPacket_l_1_1} := iTemp20 [k29 lr32:33 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{unsigned char}
main.c(347:34:34:0:0)		send iTemp2 [k6 lr7:98 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sendPacket_h_1_1}{argreg = 1}
main.c(347:35:35:0:0)		iTemp21 [k30 lr35:35 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{void} = call _putc [k4 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{void function    }
main.c(348:36:36:0:0)		send iTemp5 [k11 lr9:98 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sendPacket_l_1_1}{argreg = 1}
main.c(348:37:37:0:0)		iTemp22 [k31 lr37:37 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{void} = call _putc [k4 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{void function    }
main.c(349:38:38:0:0)		iTemp23 [k32 lr38:39 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{unsigned char} = iTemp2 [k6 lr7:98 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sendPacket_h_1_1} + iTemp5 [k11 lr9:98 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sendPacket_l_1_1}
main.c(349:39:39:0:0)		iTemp24 [k33 lr39:40 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{unsigned char} = iTemp0 [k2 lr16:101 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sendPacket_checksum_1_1} + iTemp23 [k32 lr38:39 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{unsigned char}
main.c(349:40:40:0:0)		iTemp0 [k2 lr16:101 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sendPacket_checksum_1_1} := iTemp24 [k33 lr39:40 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{unsigned char}
main.c(351:41:41:0:0)		iTemp25 [k35 lr41:42 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int} = _reading3 [k34 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int} >> 0x8 {literal unsigned char}
main.c(351:42:42:0:0)		iTemp26 [k36 lr42:43 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{unsigned char} = (unsigned char)iTemp25 [k35 lr41:42 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int}
main.c(351:43:43:0:0)		iTemp2 [k6 lr7:98 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sendPacket_h_1_1} := iTemp26 [k36 lr42:43 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{unsigned char}
main.c(352:44:44:0:0)		iTemp27 [k37 lr44:45 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{unsigned char} = (unsigned char)_reading3 [k34 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int}
main.c(352:45:45:0:0)		iTemp5 [k11 lr9:98 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sendPacket_l_1_1} := iTemp27 [k37 lr44:45 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{unsigned char}
main.c(353:46:46:0:0)		send iTemp2 [k6 lr7:98 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sendPacket_h_1_1}{argreg = 1}
main.c(353:47:47:0:0)		iTemp28 [k38 lr47:47 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{void} = call _putc [k4 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{void function    }
main.c(354:48:48:0:0)		send iTemp5 [k11 lr9:98 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sendPacket_l_1_1}{argreg = 1}
main.c(354:49:49:0:0)		iTemp29 [k39 lr49:49 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{void} = call _putc [k4 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{void function    }
main.c(355:50:50:0:0)		iTemp30 [k40 lr50:51 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{unsigned char} = iTemp2 [k6 lr7:98 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sendPacket_h_1_1} + iTemp5 [k11 lr9:98 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sendPacket_l_1_1}
main.c(355:51:51:0:0)		iTemp31 [k41 lr51:52 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{unsigned char} = iTemp0 [k2 lr16:101 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sendPacket_checksum_1_1} + iTemp30 [k40 lr50:51 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{unsigned char}
main.c(355:52:52:0:0)		iTemp0 [k2 lr16:101 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sendPacket_checksum_1_1} := iTemp31 [k41 lr51:52 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{unsigned char}
main.c(357:53:53:0:0)		iTemp32 [k43 lr53:54 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int} = _reading4 [k42 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int} >> 0x8 {literal unsigned char}
main.c(357:54:54:0:0)		iTemp33 [k44 lr54:55 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{unsigned char} = (unsigned char)iTemp32 [k43 lr53:54 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int}
main.c(357:55:55:0:0)		iTemp2 [k6 lr7:98 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sendPacket_h_1_1} := iTemp33 [k44 lr54:55 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{unsigned char}
main.c(358:56:56:0:0)		iTemp34 [k45 lr56:57 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{unsigned char} = (unsigned char)_reading4 [k42 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int}
main.c(358:57:57:0:0)		iTemp5 [k11 lr9:98 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sendPacket_l_1_1} := iTemp34 [k45 lr56:57 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{unsigned char}
main.c(359:58:58:0:0)		send iTemp2 [k6 lr7:98 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sendPacket_h_1_1}{argreg = 1}
main.c(359:59:59:0:0)		iTemp35 [k46 lr59:59 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{void} = call _putc [k4 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{void function    }
main.c(360:60:60:0:0)		send iTemp5 [k11 lr9:98 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sendPacket_l_1_1}{argreg = 1}
main.c(360:61:61:0:0)		iTemp36 [k47 lr61:61 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{void} = call _putc [k4 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{void function    }
main.c(361:62:62:0:0)		iTemp37 [k48 lr62:63 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{unsigned char} = iTemp2 [k6 lr7:98 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sendPacket_h_1_1} + iTemp5 [k11 lr9:98 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sendPacket_l_1_1}
main.c(361:63:63:0:0)		iTemp38 [k49 lr63:64 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{unsigned char} = iTemp0 [k2 lr16:101 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sendPacket_checksum_1_1} + iTemp37 [k48 lr62:63 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{unsigned char}
main.c(361:64:64:0:0)		iTemp0 [k2 lr16:101 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sendPacket_checksum_1_1} := iTemp38 [k49 lr63:64 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{unsigned char}
main.c(363:65:65:0:0)		iTemp39 [k51 lr65:66 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int} = _reading5 [k50 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int} >> 0x8 {literal unsigned char}
main.c(363:66:66:0:0)		iTemp40 [k52 lr66:67 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{unsigned char} = (unsigned char)iTemp39 [k51 lr65:66 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int}
main.c(363:67:67:0:0)		iTemp2 [k6 lr7:98 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sendPacket_h_1_1} := iTemp40 [k52 lr66:67 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{unsigned char}
main.c(364:68:68:0:0)		iTemp41 [k53 lr68:69 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{unsigned char} = (unsigned char)_reading5 [k50 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int}
main.c(364:69:69:0:0)		iTemp5 [k11 lr9:98 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sendPacket_l_1_1} := iTemp41 [k53 lr68:69 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{unsigned char}
main.c(365:70:70:0:0)		send iTemp2 [k6 lr7:98 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sendPacket_h_1_1}{argreg = 1}
main.c(365:71:71:0:0)		iTemp42 [k54 lr71:71 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{void} = call _putc [k4 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{void function    }
main.c(366:72:72:0:0)		send iTemp5 [k11 lr9:98 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sendPacket_l_1_1}{argreg = 1}
main.c(366:73:73:0:0)		iTemp43 [k55 lr73:73 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{void} = call _putc [k4 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{void function    }
main.c(367:74:74:0:0)		iTemp44 [k56 lr74:75 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{unsigned char} = iTemp2 [k6 lr7:98 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sendPacket_h_1_1} + iTemp5 [k11 lr9:98 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sendPacket_l_1_1}
main.c(367:75:75:0:0)		iTemp45 [k57 lr75:76 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{unsigned char} = iTemp0 [k2 lr16:101 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sendPacket_checksum_1_1} + iTemp44 [k56 lr74:75 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{unsigned char}
main.c(367:76:76:0:0)		iTemp0 [k2 lr16:101 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sendPacket_checksum_1_1} := iTemp45 [k57 lr75:76 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{unsigned char}
main.c(369:77:77:0:0)		iTemp46 [k59 lr77:78 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int} = _reading6 [k58 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int} >> 0x8 {literal unsigned char}
main.c(369:78:78:0:0)		iTemp47 [k60 lr78:79 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{unsigned char} = (unsigned char)iTemp46 [k59 lr77:78 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int}
main.c(369:79:79:0:0)		iTemp2 [k6 lr7:98 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sendPacket_h_1_1} := iTemp47 [k60 lr78:79 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{unsigned char}
main.c(370:80:80:0:0)		iTemp48 [k61 lr80:81 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{unsigned char} = (unsigned char)_reading6 [k58 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int}
main.c(370:81:81:0:0)		iTemp5 [k11 lr9:98 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sendPacket_l_1_1} := iTemp48 [k61 lr80:81 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{unsigned char}
main.c(371:82:82:0:0)		send iTemp2 [k6 lr7:98 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sendPacket_h_1_1}{argreg = 1}
main.c(371:83:83:0:0)		iTemp49 [k62 lr83:83 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{void} = call _putc [k4 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{void function    }
main.c(372:84:84:0:0)		send iTemp5 [k11 lr9:98 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sendPacket_l_1_1}{argreg = 1}
main.c(372:85:85:0:0)		iTemp50 [k63 lr85:85 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{void} = call _putc [k4 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{void function    }
main.c(373:86:86:0:0)		iTemp51 [k64 lr86:87 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{unsigned char} = iTemp2 [k6 lr7:98 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sendPacket_h_1_1} + iTemp5 [k11 lr9:98 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sendPacket_l_1_1}
main.c(373:87:87:0:0)		iTemp52 [k65 lr87:88 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{unsigned char} = iTemp0 [k2 lr16:101 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sendPacket_checksum_1_1} + iTemp51 [k64 lr86:87 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{unsigned char}
main.c(373:88:88:0:0)		iTemp0 [k2 lr16:101 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sendPacket_checksum_1_1} := iTemp52 [k65 lr87:88 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{unsigned char}
main.c(375:89:89:0:0)		iTemp53 [k67 lr89:90 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int} = _reading7 [k66 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int} >> 0x8 {literal unsigned char}
main.c(375:90:90:0:0)		iTemp54 [k68 lr90:91 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{unsigned char} = (unsigned char)iTemp53 [k67 lr89:90 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int}
main.c(375:91:91:0:0)		iTemp2 [k6 lr7:98 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sendPacket_h_1_1} := iTemp54 [k68 lr90:91 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{unsigned char}
main.c(376:92:92:0:0)		iTemp55 [k69 lr92:93 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{unsigned char} = (unsigned char)_reading7 [k66 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int}
main.c(376:93:93:0:0)		iTemp5 [k11 lr9:98 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sendPacket_l_1_1} := iTemp55 [k69 lr92:93 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{unsigned char}
main.c(377:94:94:0:0)		send iTemp2 [k6 lr7:98 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sendPacket_h_1_1}{argreg = 1}
main.c(377:95:95:0:0)		iTemp56 [k70 lr95:95 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{void} = call _putc [k4 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{void function    }
main.c(378:96:96:0:0)		send iTemp5 [k11 lr9:98 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sendPacket_l_1_1}{argreg = 1}
main.c(378:97:97:0:0)		iTemp57 [k71 lr97:97 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{void} = call _putc [k4 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{void function    }
main.c(379:98:98:0:0)		iTemp58 [k72 lr98:99 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{unsigned char} = iTemp2 [k6 lr7:98 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sendPacket_h_1_1} + iTemp5 [k11 lr9:98 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sendPacket_l_1_1}
main.c(379:99:99:0:0)		iTemp59 [k73 lr99:100 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{unsigned char} = iTemp0 [k2 lr16:101 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sendPacket_checksum_1_1} + iTemp58 [k72 lr98:99 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{unsigned char}
main.c(379:100:100:0:0)		iTemp0 [k2 lr16:101 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sendPacket_checksum_1_1} := iTemp59 [k73 lr99:100 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{unsigned char}
main.c(382:101:101:0:0)		send iTemp0 [k2 lr16:101 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sendPacket_checksum_1_1}{argreg = 1}
main.c(382:102:102:0:0)		iTemp60 [k74 lr102:102 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{void} = call _putc [k4 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{void function    }

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
main.c(382:103:103:0:0)	 _return($1) :
main.c(382:104:104:0:0)		eproc _sendPacket [k1 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{void function    }
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
  -- #75 reg = r0x63  key 0, rIdx = 99, size 1
  -- #76 reg = r0x64  key 0, rIdx = 100, size 1
  -- #77 reg = _ADRESH  key 0, rIdx = 36, size 1
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
  -- #88 reg = r0x6F  key 0, rIdx = 111, size 1
  -- #89 reg = r0x70  key 0, rIdx = 112, size 1
  -- #90 reg = r0x71  key 0, rIdx = 113, size 1
  -- #91 reg = _ADRESL  key 5, rIdx = 37, size 1
  -- #92 reg = _PCLATH  key 6, rIdx = 10, size 1
  -- #93 reg = _TRISA  key 7, rIdx = 1, size 1
  -- #94 reg = _TRISB  key 8, rIdx = 2, size 1
  -- #95 reg = _TRISB  key 8, rIdx = 24, size 1
  -- #96 reg = _TRISB  key 8, rIdx = 26, size 1
  -- #97 reg = _TRISB  key 8, rIdx = 35, size 1
  -- #98 reg = _TRISC  key 9, rIdx = 3, size 1
  -- #99 reg = _TRISD  key 10, rIdx = 4, size 1
  -- #100 reg = _PORTA  key 10, rIdx = 6, size 1
  -- #101 reg = _TRISE  key 11, rIdx = 5, size 1
  -- #102 reg = _PORTB  key 11, rIdx = 7, size 1
  -- #103 reg = _PORTB  key 11, rIdx = 23, size 1
  -- #104 reg = _PORTB  key 11, rIdx = 33, size 1
  -- #105 reg = _PORTC  key 12, rIdx = 8, size 1
  -- #106 reg = _GIE  key 12, rIdx = 13, size 1
  -- #107 reg = _PORTD  key 13, rIdx = 9, size 1
  -- #108 reg = _PORTE  key 14, rIdx = 10, size 1
  -- #109 reg = _numResonationHalfcycles  key 14, rIdx = 32, size 1
  -- #110 reg = _sensorSample_phase0_1_1  key 15, rIdx = 28, size 1
  -- #111 reg = _sensorSample_phase1_1_1  key 16, rIdx = 29, size 1
  -- #112 reg = _sensorSample_phase2_1_1  key 17, rIdx = 30, size 1
  -- #113 reg = _sensorSample_phase3_1_1  key 18, rIdx = 31, size 1
  -- #114 reg = _ADCON0  key 27, rIdx = 12, size 1
  -- #115 reg = _ADCON0  key 27, rIdx = 25, size 1
  -- #116 reg = _ADCON0  key 27, rIdx = 27, size 1
  -- #117 reg = _ADCON1  key 28, rIdx = 11, size 1
  -- #118 reg = SSAVE  key 32, rIdx = 32765, size 1
  -- #119 reg = WSAVE  key 36, rIdx = 32766, size 1
  -- #120 reg = _reading0  key 44, rIdx = 14, size 2
  -- #121 reg = _GO_DONE  key 44, rIdx = 34, size 1
  -- #122 reg = _reading1  key 45, rIdx = 15, size 2
  -- #123 reg = _reading2  key 46, rIdx = 16, size 2
  -- #124 reg = INTCON  key 47, rIdx = 11, size 1
  -- #125 reg = _reading3  key 47, rIdx = 17, size 2
  -- #126 reg = _STATUS  key 48, rIdx = 3, size 1
  -- #127 reg = PCL  key 48, rIdx = 2, size 1
  -- #128 reg = _reading4  key 48, rIdx = 18, size 2
  -- #129 reg = KZ  key 49, rIdx = 32767, size 1
  -- #130 reg = _reading5  key 49, rIdx = 19, size 2
  -- #131 reg = _reading6  key 50, rIdx = 20, size 2
  -- #132 reg = _reading7  key 51, rIdx = 21, size 2
  -- #133 reg = _SerialTris  key 54, rIdx = 39, size 1
  -- #134 reg = INDF  key 56, rIdx = 0, size 1
  -- #135 reg = _Light0  key 56, rIdx = 0, size 1
  -- #136 reg = _numIntegrationSamples  key 56, rIdx = 22, size 1
  -- #137 reg = _SerialPort  key 57, rIdx = 38, size 1
  -- #138 reg = _Light2  key 59, rIdx = 41, size 1
  -- #139 reg = FSR  key 60, rIdx = 4, size 1
  -- #140 reg = _Light3  key 60, rIdx = 40, size 1
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
main.c(326:1:0:0:0)	 _entry($2) :
main.c(326:2:1:0:0)		proc _sendPacket [k1 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{void function    }
main.c(331:3:3:0:0)		send 0x80 {literal unsigned char}{argreg = 1}
main.c(331:4:4:0:0)		iTemp1 [k5 lr4:4 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{void} = call _putc [k4 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{void function    }
main.c(333:5:5:0:0)		iTemp3 [k9 lr5:6 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int}[r0x72 r0x73 ] = _reading0 [k8 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int} >> 0x8 {literal unsigned char}
main.c(333:6:6:0:0)		iTemp2 [k6 lr6:98 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sendPacket_h_1_1}[r0x72 ] = (unsigned char)iTemp3 [k9 lr5:6 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int}[r0x72 r0x73 ]
main.c(334:8:8:0:0)		iTemp5 [k11 lr8:98 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sendPacket_l_1_1}[r0x74 ] = (unsigned char)_reading0 [k8 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int}
main.c(335:10:10:0:0)		send iTemp2 [k6 lr6:98 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sendPacket_h_1_1}[r0x72 ]{argreg = 1}
main.c(335:11:11:0:0)		iTemp7 [k14 lr11:11 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{void} = call _putc [k4 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{void function    }
main.c(336:12:12:0:0)		send iTemp5 [k11 lr8:98 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sendPacket_l_1_1}[r0x74 ]{argreg = 1}
main.c(336:13:13:0:0)		iTemp8 [k15 lr13:13 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{void} = call _putc [k4 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{void function    }
main.c(337:14:14:0:0)		iTemp0 [k2 lr14:101 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sendPacket_checksum_1_1}[r0x75 ] = iTemp2 [k6 lr6:98 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sendPacket_h_1_1}[r0x72 ] + iTemp5 [k11 lr8:98 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sendPacket_l_1_1}[r0x74 ]
main.c(339:17:17:0:0)		iTemp11 [k19 lr17:18 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int}[r0x76 r0x77 ] = _reading1 [k18 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int} >> 0x8 {literal unsigned char}
main.c(339:18:18:0:0)		iTemp2 [k6 lr6:98 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sendPacket_h_1_1}[r0x72 ] = (unsigned char)iTemp11 [k19 lr17:18 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int}[r0x76 r0x77 ]
main.c(340:20:20:0:0)		iTemp5 [k11 lr8:98 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sendPacket_l_1_1}[r0x74 ] = (unsigned char)_reading1 [k18 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int}
main.c(341:22:22:0:0)		send iTemp2 [k6 lr6:98 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sendPacket_h_1_1}[r0x72 ]{argreg = 1}
main.c(341:23:23:0:0)		iTemp14 [k22 lr23:23 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{void} = call _putc [k4 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{void function    }
main.c(342:24:24:0:0)		send iTemp5 [k11 lr8:98 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sendPacket_l_1_1}[r0x74 ]{argreg = 1}
main.c(342:25:25:0:0)		iTemp15 [k23 lr25:25 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{void} = call _putc [k4 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{void function    }
main.c(343:26:26:0:0)		iTemp16 [k24 lr26:27 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{unsigned char} = iTemp2 [k6 lr6:98 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sendPacket_h_1_1}[r0x72 ] + iTemp5 [k11 lr8:98 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sendPacket_l_1_1}[r0x74 ]
main.c(343:27:27:0:0)		iTemp0 [k2 lr14:101 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sendPacket_checksum_1_1}[r0x75 ] = iTemp0 [k2 lr14:101 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sendPacket_checksum_1_1}[r0x75 ] + iTemp16 [k24 lr26:27 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{unsigned char}
main.c(345:29:29:0:0)		iTemp18 [k27 lr29:30 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int}[r0x78 r0x79 ] = _reading2 [k26 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int} >> 0x8 {literal unsigned char}
main.c(345:30:30:0:0)		iTemp2 [k6 lr6:98 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sendPacket_h_1_1}[r0x72 ] = (unsigned char)iTemp18 [k27 lr29:30 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int}[r0x78 r0x79 ]
main.c(346:32:32:0:0)		iTemp5 [k11 lr8:98 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sendPacket_l_1_1}[r0x74 ] = (unsigned char)_reading2 [k26 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int}
main.c(347:34:34:0:0)		send iTemp2 [k6 lr6:98 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sendPacket_h_1_1}[r0x72 ]{argreg = 1}
main.c(347:35:35:0:0)		iTemp21 [k30 lr35:35 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{void} = call _putc [k4 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{void function    }
main.c(348:36:36:0:0)		send iTemp5 [k11 lr8:98 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sendPacket_l_1_1}[r0x74 ]{argreg = 1}
main.c(348:37:37:0:0)		iTemp22 [k31 lr37:37 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{void} = call _putc [k4 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{void function    }
main.c(349:38:38:0:0)		iTemp23 [k32 lr38:39 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{unsigned char} = iTemp2 [k6 lr6:98 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sendPacket_h_1_1}[r0x72 ] + iTemp5 [k11 lr8:98 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sendPacket_l_1_1}[r0x74 ]
main.c(349:39:39:0:0)		iTemp0 [k2 lr14:101 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sendPacket_checksum_1_1}[r0x75 ] = iTemp0 [k2 lr14:101 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sendPacket_checksum_1_1}[r0x75 ] + iTemp23 [k32 lr38:39 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{unsigned char}
main.c(351:41:41:0:0)		iTemp25 [k35 lr41:42 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int}[r0x7A r0x7B ] = _reading3 [k34 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int} >> 0x8 {literal unsigned char}
main.c(351:42:42:0:0)		iTemp2 [k6 lr6:98 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sendPacket_h_1_1}[r0x72 ] = (unsigned char)iTemp25 [k35 lr41:42 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int}[r0x7A r0x7B ]
main.c(352:44:44:0:0)		iTemp5 [k11 lr8:98 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sendPacket_l_1_1}[r0x74 ] = (unsigned char)_reading3 [k34 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int}
main.c(353:46:46:0:0)		send iTemp2 [k6 lr6:98 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sendPacket_h_1_1}[r0x72 ]{argreg = 1}
main.c(353:47:47:0:0)		iTemp28 [k38 lr47:47 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{void} = call _putc [k4 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{void function    }
main.c(354:48:48:0:0)		send iTemp5 [k11 lr8:98 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sendPacket_l_1_1}[r0x74 ]{argreg = 1}
main.c(354:49:49:0:0)		iTemp29 [k39 lr49:49 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{void} = call _putc [k4 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{void function    }
main.c(355:50:50:0:0)		iTemp30 [k40 lr50:51 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{unsigned char} = iTemp2 [k6 lr6:98 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sendPacket_h_1_1}[r0x72 ] + iTemp5 [k11 lr8:98 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sendPacket_l_1_1}[r0x74 ]
main.c(355:51:51:0:0)		iTemp0 [k2 lr14:101 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sendPacket_checksum_1_1}[r0x75 ] = iTemp0 [k2 lr14:101 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sendPacket_checksum_1_1}[r0x75 ] + iTemp30 [k40 lr50:51 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{unsigned char}
main.c(357:53:53:0:0)		iTemp32 [k43 lr53:54 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int}[r0x7C r0x7D ] = _reading4 [k42 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int} >> 0x8 {literal unsigned char}
main.c(357:54:54:0:0)		iTemp2 [k6 lr6:98 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sendPacket_h_1_1}[r0x72 ] = (unsigned char)iTemp32 [k43 lr53:54 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int}[r0x7C r0x7D ]
main.c(358:56:56:0:0)		iTemp5 [k11 lr8:98 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sendPacket_l_1_1}[r0x74 ] = (unsigned char)_reading4 [k42 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int}
main.c(359:58:58:0:0)		send iTemp2 [k6 lr6:98 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sendPacket_h_1_1}[r0x72 ]{argreg = 1}
main.c(359:59:59:0:0)		iTemp35 [k46 lr59:59 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{void} = call _putc [k4 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{void function    }
main.c(360:60:60:0:0)		send iTemp5 [k11 lr8:98 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sendPacket_l_1_1}[r0x74 ]{argreg = 1}
main.c(360:61:61:0:0)		iTemp36 [k47 lr61:61 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{void} = call _putc [k4 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{void function    }
main.c(361:62:62:0:0)		iTemp37 [k48 lr62:63 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{unsigned char} = iTemp2 [k6 lr6:98 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sendPacket_h_1_1}[r0x72 ] + iTemp5 [k11 lr8:98 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sendPacket_l_1_1}[r0x74 ]
main.c(361:63:63:0:0)		iTemp0 [k2 lr14:101 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sendPacket_checksum_1_1}[r0x75 ] = iTemp0 [k2 lr14:101 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sendPacket_checksum_1_1}[r0x75 ] + iTemp37 [k48 lr62:63 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{unsigned char}
main.c(363:65:65:0:0)		iTemp39 [k51 lr65:66 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int}[r0x7E r0x7F ] = _reading5 [k50 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int} >> 0x8 {literal unsigned char}
main.c(363:66:66:0:0)		iTemp2 [k6 lr6:98 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sendPacket_h_1_1}[r0x72 ] = (unsigned char)iTemp39 [k51 lr65:66 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int}[r0x7E r0x7F ]
main.c(364:68:68:0:0)		iTemp5 [k11 lr8:98 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sendPacket_l_1_1}[r0x74 ] = (unsigned char)_reading5 [k50 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int}
main.c(365:70:70:0:0)		send iTemp2 [k6 lr6:98 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sendPacket_h_1_1}[r0x72 ]{argreg = 1}
main.c(365:71:71:0:0)		iTemp42 [k54 lr71:71 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{void} = call _putc [k4 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{void function    }
main.c(366:72:72:0:0)		send iTemp5 [k11 lr8:98 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sendPacket_l_1_1}[r0x74 ]{argreg = 1}
main.c(366:73:73:0:0)		iTemp43 [k55 lr73:73 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{void} = call _putc [k4 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{void function    }
main.c(367:74:74:0:0)		iTemp44 [k56 lr74:75 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{unsigned char} = iTemp2 [k6 lr6:98 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sendPacket_h_1_1}[r0x72 ] + iTemp5 [k11 lr8:98 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sendPacket_l_1_1}[r0x74 ]
main.c(367:75:75:0:0)		iTemp0 [k2 lr14:101 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sendPacket_checksum_1_1}[r0x75 ] = iTemp0 [k2 lr14:101 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sendPacket_checksum_1_1}[r0x75 ] + iTemp44 [k56 lr74:75 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{unsigned char}
main.c(369:77:77:0:0)		iTemp46 [k59 lr77:78 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int}[r0x80 r0x81 ] = _reading6 [k58 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int} >> 0x8 {literal unsigned char}
main.c(369:78:78:0:0)		iTemp2 [k6 lr6:98 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sendPacket_h_1_1}[r0x72 ] = (unsigned char)iTemp46 [k59 lr77:78 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int}[r0x80 r0x81 ]
main.c(370:80:80:0:0)		iTemp5 [k11 lr8:98 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sendPacket_l_1_1}[r0x74 ] = (unsigned char)_reading6 [k58 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int}
main.c(371:82:82:0:0)		send iTemp2 [k6 lr6:98 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sendPacket_h_1_1}[r0x72 ]{argreg = 1}
main.c(371:83:83:0:0)		iTemp49 [k62 lr83:83 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{void} = call _putc [k4 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{void function    }
main.c(372:84:84:0:0)		send iTemp5 [k11 lr8:98 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sendPacket_l_1_1}[r0x74 ]{argreg = 1}
main.c(372:85:85:0:0)		iTemp50 [k63 lr85:85 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{void} = call _putc [k4 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{void function    }
main.c(373:86:86:0:0)		iTemp51 [k64 lr86:87 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{unsigned char} = iTemp2 [k6 lr6:98 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sendPacket_h_1_1}[r0x72 ] + iTemp5 [k11 lr8:98 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sendPacket_l_1_1}[r0x74 ]
main.c(373:87:87:0:0)		iTemp0 [k2 lr14:101 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sendPacket_checksum_1_1}[r0x75 ] = iTemp0 [k2 lr14:101 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sendPacket_checksum_1_1}[r0x75 ] + iTemp51 [k64 lr86:87 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{unsigned char}
main.c(375:89:89:0:0)		iTemp53 [k67 lr89:90 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int}[r0x82 r0x83 ] = _reading7 [k66 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int} >> 0x8 {literal unsigned char}
main.c(375:90:90:0:0)		iTemp2 [k6 lr6:98 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sendPacket_h_1_1}[r0x72 ] = (unsigned char)iTemp53 [k67 lr89:90 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int}[r0x82 r0x83 ]
main.c(376:92:92:0:0)		iTemp5 [k11 lr8:98 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sendPacket_l_1_1}[r0x74 ] = (unsigned char)_reading7 [k66 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{int}
main.c(377:94:94:0:0)		send iTemp2 [k6 lr6:98 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sendPacket_h_1_1}[r0x72 ]{argreg = 1}
main.c(377:95:95:0:0)		iTemp56 [k70 lr95:95 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{void} = call _putc [k4 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{void function    }
main.c(378:96:96:0:0)		send iTemp5 [k11 lr8:98 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sendPacket_l_1_1}[r0x74 ]{argreg = 1}
main.c(378:97:97:0:0)		iTemp57 [k71 lr97:97 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{void} = call _putc [k4 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{void function    }
main.c(379:98:98:0:0)		iTemp58 [k72 lr98:99 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{unsigned char} = iTemp2 [k6 lr6:98 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sendPacket_h_1_1}[r0x72 ] + iTemp5 [k11 lr8:98 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sendPacket_l_1_1}[r0x74 ]
main.c(379:99:99:0:0)		iTemp0 [k2 lr14:101 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sendPacket_checksum_1_1}[r0x75 ] = iTemp0 [k2 lr14:101 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sendPacket_checksum_1_1}[r0x75 ] + iTemp58 [k72 lr98:99 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{unsigned char}
main.c(382:101:101:0:0)		send iTemp0 [k2 lr14:101 so:0]{ ia0 re1 rm0 nos0 ru0 dp0}{unsigned char}{ sir@ _sendPacket_checksum_1_1}[r0x75 ]{argreg = 1}
main.c(382:102:102:0:0)		iTemp60 [k74 lr102:102 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{void} = call _putc [k4 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{void function    }
main.c(382:103:103:0:0)	 _return($1) :
main.c(382:104:104:0:0)		eproc _sendPacket [k1 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{void function    }

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
main.c(382:103:103:0:0)	 _return($1) :
main.c(382:104:104:0:0)		eproc _sendPacket [k1 lr0:0 so:0]{ ia0 re0 rm0 nos0 ru0 dp0}{void function    }
typeRegWithIdx - requesting index = 0x72
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x73
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x73
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x72
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x72
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x74
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x72
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x74
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x74
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x72
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x75
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x76
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x77
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x77
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x76
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x72
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x74
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x72
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x74
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x74
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x72
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x75
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x78
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x79
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x79
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x78
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x72
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x74
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x72
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x74
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x74
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x72
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x75
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x7a
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x7b
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x7b
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x7a
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x72
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x74
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x72
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x74
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x74
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x72
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x75
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x7c
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x7d
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x7d
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x7c
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x72
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x74
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x72
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x74
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x74
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x72
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x75
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x7e
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x7f
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x7f
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x7e
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x72
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x74
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x72
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x74
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x74
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x72
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x75
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x80
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x81
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x81
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x80
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x72
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x74
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x72
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x74
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x74
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x72
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x75
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x82
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x83
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x83
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x82
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x72
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x74
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x72
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x74
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x74
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x72
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x75
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x75
Found a Dynamic Register!
leaving
<><><><><><><><><><><><><><><><><>
<><><><><><><><><><><><><><><><><>
starting	ralloc.c:pic14_assignRegisters
ebbs before optimizing:

----------------------------------------------------------------
Basic Block _eBBlock0 : loop Depth = 0 noPath = 0 , lastinLoop = 0
depth 1st num 1 : bbnum = 0 1st iCode = 1 , last iCode = 1
visited 0 : hasFcall = 0

defines bitVector :bitvector Size = 107 bSize = 14
Bits on { (106) }

local defines bitVector :bitvector Size = 107 bSize = 14
Bits on { (106) }

pointers Set bitvector :
in pointers Set bitvector :
inDefs Set bitvector :
outDefs Set bitvector :bitvector Size = 107 bSize = 14
Bits on { (106) }

usesDefs Set bitvector :bitvector Size = 107 bSize = 14
Bits on { }

----------------------------------------------------------------
main.c(384:1:106:0:0)		_numResonationHalfcycles [k75 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{unsigned char} := 0x3 {literal unsigned char}
packRegisters
packRegsForAssign
  result:_numResonationHalfcycles [k75 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{unsigned char}
  left:
  right:0x3 {literal unsigned char}
allocDirReg symbol name _numResonationHalfcycles
  637  storage class 0 
 642  integral
 646  specifier
_numResonationHalfcycles [k75 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{unsigned char}
  2695 - result is not temp
  2705 - not packing - right is not temp
allocDirReg BAD, op is NULL
  result:_numResonationHalfcycles [k75 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{unsigned char}
  left:
  right:0x3 {literal unsigned char}
  result:_numResonationHalfcycles [k75 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{unsigned char}
          Symbol type: unsigned char
  3594 - pointer reg req = 0
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
  -- #75 reg = r0x63  key 0, rIdx = 99, size 1
  -- #76 reg = r0x64  key 0, rIdx = 100, size 1
  -- #77 reg = _ADRESH  key 0, rIdx = 36, size 1
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
  -- #88 reg = r0x6F  key 0, rIdx = 111, size 1
  -- #89 reg = r0x70  key 0, rIdx = 112, size 1
  -- #90 reg = r0x71  key 0, rIdx = 113, size 1
  -- #91 reg = r0x72  key 0, rIdx = 114, size 1
  -- #92 reg = r0x73  key 0, rIdx = 115, size 1
  -- #93 reg = r0x74  key 0, rIdx = 116, size 1
  -- #94 reg = r0x75  key 0, rIdx = 117, size 1
  -- #95 reg = r0x76  key 0, rIdx = 118, size 1
  -- #96 reg = r0x77  key 0, rIdx = 119, size 1
  -- #97 reg = r0x78  key 0, rIdx = 120, size 1
  -- #98 reg = r0x79  key 0, rIdx = 121, size 1
  -- #99 reg = r0x7A  key 0, rIdx = 122, size 1
  -- #100 reg = r0x7B  key 0, rIdx = 123, size 1
  -- #101 reg = r0x7C  key 0, rIdx = 124, size 1
  -- #102 reg = r0x7D  key 0, rIdx = 125, size 1
  -- #103 reg = r0x7E  key 0, rIdx = 126, size 1
  -- #104 reg = r0x7F  key 0, rIdx = 127, size 1
  -- #105 reg = r0x80  key 0, rIdx = 128, size 1
  -- #106 reg = r0x81  key 0, rIdx = 129, size 1
  -- #107 reg = r0x82  key 0, rIdx = 130, size 1
  -- #108 reg = r0x83  key 0, rIdx = 131, size 1
  -- #109 reg = _ADRESL  key 5, rIdx = 37, size 1
  -- #110 reg = _PCLATH  key 6, rIdx = 10, size 1
  -- #111 reg = _TRISA  key 7, rIdx = 1, size 1
  -- #112 reg = _TRISB  key 8, rIdx = 2, size 1
  -- #113 reg = _TRISB  key 8, rIdx = 24, size 1
  -- #114 reg = _TRISB  key 8, rIdx = 26, size 1
  -- #115 reg = _TRISB  key 8, rIdx = 35, size 1
  -- #116 reg = _TRISC  key 9, rIdx = 3, size 1
  -- #117 reg = _TRISD  key 10, rIdx = 4, size 1
  -- #118 reg = _PORTA  key 10, rIdx = 6, size 1
  -- #119 reg = _TRISE  key 11, rIdx = 5, size 1
  -- #120 reg = _PORTB  key 11, rIdx = 7, size 1
  -- #121 reg = _PORTB  key 11, rIdx = 23, size 1
  -- #122 reg = _PORTB  key 11, rIdx = 33, size 1
  -- #123 reg = _PORTC  key 12, rIdx = 8, size 1
  -- #124 reg = _GIE  key 12, rIdx = 13, size 1
  -- #125 reg = _PORTD  key 13, rIdx = 9, size 1
  -- #126 reg = _PORTE  key 14, rIdx = 10, size 1
  -- #127 reg = _numResonationHalfcycles  key 14, rIdx = 32, size 1
  -- #128 reg = _sensorSample_phase0_1_1  key 15, rIdx = 28, size 1
  -- #129 reg = _sensorSample_phase1_1_1  key 16, rIdx = 29, size 1
  -- #130 reg = _sensorSample_phase2_1_1  key 17, rIdx = 30, size 1
  -- #131 reg = _sensorSample_phase3_1_1  key 18, rIdx = 31, size 1
  -- #132 reg = _ADCON0  key 27, rIdx = 12, size 1
  -- #133 reg = _ADCON0  key 27, rIdx = 25, size 1
  -- #134 reg = _ADCON0  key 27, rIdx = 27, size 1
  -- #135 reg = _ADCON1  key 28, rIdx = 11, size 1
  -- #136 reg = SSAVE  key 32, rIdx = 32765, size 1
  -- #137 reg = WSAVE  key 36, rIdx = 32766, size 1
  -- #138 reg = _reading0  key 44, rIdx = 14, size 2
  -- #139 reg = _GO_DONE  key 44, rIdx = 34, size 1
  -- #140 reg = _reading1  key 45, rIdx = 15, size 2
  -- #141 reg = _reading2  key 46, rIdx = 16, size 2
  -- #142 reg = INTCON  key 47, rIdx = 11, size 1
  -- #143 reg = _reading3  key 47, rIdx = 17, size 2
  -- #144 reg = _STATUS  key 48, rIdx = 3, size 1
  -- #145 reg = PCL  key 48, rIdx = 2, size 1
  -- #146 reg = _reading4  key 48, rIdx = 18, size 2
  -- #147 reg = KZ  key 49, rIdx = 32767, size 1
  -- #148 reg = _reading5  key 49, rIdx = 19, size 2
  -- #149 reg = _reading6  key 50, rIdx = 20, size 2
  -- #150 reg = _reading7  key 51, rIdx = 21, size 2
  -- #151 reg = _SerialTris  key 54, rIdx = 39, size 1
  -- #152 reg = INDF  key 56, rIdx = 0, size 1
  -- #153 reg = _Light0  key 56, rIdx = 0, size 1
  -- #154 reg = _numIntegrationSamples  key 56, rIdx = 22, size 1
  -- #155 reg = _SerialPort  key 57, rIdx = 38, size 1
  -- #156 reg = _Light2  key 59, rIdx = 41, size 1
  -- #157 reg = FSR  key 60, rIdx = 4, size 1
  -- #158 reg = _Light3  key 60, rIdx = 40, size 1
regTypeNum
serialRegAssign
  op: =
deassignLRs
createRegMask
regsUsedIniCode
rUmaskForOp
rUmaskForOp
ebbs after optimizing:

----------------------------------------------------------------
Basic Block _eBBlock0 : loop Depth = 0 noPath = 0 , lastinLoop = 0
depth 1st num 1 : bbnum = 0 1st iCode = 1 , last iCode = 1
visited 0 : hasFcall = 0

defines bitVector :bitvector Size = 107 bSize = 14
Bits on { (106) }

local defines bitVector :bitvector Size = 107 bSize = 14
Bits on { (106) }

pointers Set bitvector :
in pointers Set bitvector :
inDefs Set bitvector :
outDefs Set bitvector :bitvector Size = 107 bSize = 14
Bits on { (106) }

usesDefs Set bitvector :bitvector Size = 107 bSize = 14
Bits on { }

----------------------------------------------------------------
main.c(384:1:106:0:0)		_numResonationHalfcycles [k75 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{unsigned char} := 0x3 {literal unsigned char}
leaving
<><><><><><><><><><><><><><><><><>
<><><><><><><><><><><><><><><><><>
starting	ralloc.c:pic14_assignRegisters
ebbs before optimizing:

----------------------------------------------------------------
Basic Block _eBBlock0 : loop Depth = 0 noPath = 0 , lastinLoop = 0
depth 1st num 1 : bbnum = 0 1st iCode = 1 , last iCode = 1
visited 0 : hasFcall = 0

defines bitVector :bitvector Size = 109 bSize = 14
Bits on { (108) }

local defines bitVector :bitvector Size = 109 bSize = 14
Bits on { (108) }

pointers Set bitvector :
in pointers Set bitvector :
inDefs Set bitvector :
outDefs Set bitvector :bitvector Size = 109 bSize = 14
Bits on { (108) }

usesDefs Set bitvector :bitvector Size = 109 bSize = 14
Bits on { }

----------------------------------------------------------------
main.c(384:1:108:0:0)		_numIntegrationSamples [k77 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{unsigned char} := 0x8 {literal unsigned char}
packRegisters
packRegsForAssign
  result:_numIntegrationSamples [k77 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{unsigned char}
  left:
  right:0x8 {literal unsigned char}
allocDirReg symbol name _numIntegrationSamples
  637  storage class 0 
 642  integral
 646  specifier
_numIntegrationSamples [k77 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{unsigned char}
  2695 - result is not temp
  2705 - not packing - right is not temp
allocDirReg BAD, op is NULL
  result:_numIntegrationSamples [k77 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{unsigned char}
  left:
  right:0x8 {literal unsigned char}
  result:_numIntegrationSamples [k77 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{unsigned char}
          Symbol type: unsigned char
  3594 - pointer reg req = 0
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
  -- #75 reg = r0x63  key 0, rIdx = 99, size 1
  -- #76 reg = r0x64  key 0, rIdx = 100, size 1
  -- #77 reg = _ADRESH  key 0, rIdx = 36, size 1
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
  -- #88 reg = r0x6F  key 0, rIdx = 111, size 1
  -- #89 reg = r0x70  key 0, rIdx = 112, size 1
  -- #90 reg = r0x71  key 0, rIdx = 113, size 1
  -- #91 reg = r0x72  key 0, rIdx = 114, size 1
  -- #92 reg = r0x73  key 0, rIdx = 115, size 1
  -- #93 reg = r0x74  key 0, rIdx = 116, size 1
  -- #94 reg = r0x75  key 0, rIdx = 117, size 1
  -- #95 reg = r0x76  key 0, rIdx = 118, size 1
  -- #96 reg = r0x77  key 0, rIdx = 119, size 1
  -- #97 reg = r0x78  key 0, rIdx = 120, size 1
  -- #98 reg = r0x79  key 0, rIdx = 121, size 1
  -- #99 reg = r0x7A  key 0, rIdx = 122, size 1
  -- #100 reg = r0x7B  key 0, rIdx = 123, size 1
  -- #101 reg = r0x7C  key 0, rIdx = 124, size 1
  -- #102 reg = r0x7D  key 0, rIdx = 125, size 1
  -- #103 reg = r0x7E  key 0, rIdx = 126, size 1
  -- #104 reg = r0x7F  key 0, rIdx = 127, size 1
  -- #105 reg = r0x80  key 0, rIdx = 128, size 1
  -- #106 reg = r0x81  key 0, rIdx = 129, size 1
  -- #107 reg = r0x82  key 0, rIdx = 130, size 1
  -- #108 reg = r0x83  key 0, rIdx = 131, size 1
  -- #109 reg = _ADRESL  key 5, rIdx = 37, size 1
  -- #110 reg = _PCLATH  key 6, rIdx = 10, size 1
  -- #111 reg = _TRISA  key 7, rIdx = 1, size 1
  -- #112 reg = _TRISB  key 8, rIdx = 2, size 1
  -- #113 reg = _TRISB  key 8, rIdx = 24, size 1
  -- #114 reg = _TRISB  key 8, rIdx = 26, size 1
  -- #115 reg = _TRISB  key 8, rIdx = 35, size 1
  -- #116 reg = _TRISC  key 9, rIdx = 3, size 1
  -- #117 reg = _TRISD  key 10, rIdx = 4, size 1
  -- #118 reg = _PORTA  key 10, rIdx = 6, size 1
  -- #119 reg = _TRISE  key 11, rIdx = 5, size 1
  -- #120 reg = _PORTB  key 11, rIdx = 7, size 1
  -- #121 reg = _PORTB  key 11, rIdx = 23, size 1
  -- #122 reg = _PORTB  key 11, rIdx = 33, size 1
  -- #123 reg = _PORTC  key 12, rIdx = 8, size 1
  -- #124 reg = _GIE  key 12, rIdx = 13, size 1
  -- #125 reg = _PORTD  key 13, rIdx = 9, size 1
  -- #126 reg = _PORTE  key 14, rIdx = 10, size 1
  -- #127 reg = _numResonationHalfcycles  key 14, rIdx = 32, size 1
  -- #128 reg = _sensorSample_phase0_1_1  key 15, rIdx = 28, size 1
  -- #129 reg = _sensorSample_phase1_1_1  key 16, rIdx = 29, size 1
  -- #130 reg = _sensorSample_phase2_1_1  key 17, rIdx = 30, size 1
  -- #131 reg = _sensorSample_phase3_1_1  key 18, rIdx = 31, size 1
  -- #132 reg = _ADCON0  key 27, rIdx = 12, size 1
  -- #133 reg = _ADCON0  key 27, rIdx = 25, size 1
  -- #134 reg = _ADCON0  key 27, rIdx = 27, size 1
  -- #135 reg = _ADCON1  key 28, rIdx = 11, size 1
  -- #136 reg = SSAVE  key 32, rIdx = 32765, size 1
  -- #137 reg = WSAVE  key 36, rIdx = 32766, size 1
  -- #138 reg = _reading0  key 44, rIdx = 14, size 2
  -- #139 reg = _GO_DONE  key 44, rIdx = 34, size 1
  -- #140 reg = _reading1  key 45, rIdx = 15, size 2
  -- #141 reg = _reading2  key 46, rIdx = 16, size 2
  -- #142 reg = INTCON  key 47, rIdx = 11, size 1
  -- #143 reg = _reading3  key 47, rIdx = 17, size 2
  -- #144 reg = _STATUS  key 48, rIdx = 3, size 1
  -- #145 reg = PCL  key 48, rIdx = 2, size 1
  -- #146 reg = _reading4  key 48, rIdx = 18, size 2
  -- #147 reg = KZ  key 49, rIdx = 32767, size 1
  -- #148 reg = _reading5  key 49, rIdx = 19, size 2
  -- #149 reg = _reading6  key 50, rIdx = 20, size 2
  -- #150 reg = _reading7  key 51, rIdx = 21, size 2
  -- #151 reg = _SerialTris  key 54, rIdx = 39, size 1
  -- #152 reg = INDF  key 56, rIdx = 0, size 1
  -- #153 reg = _Light0  key 56, rIdx = 0, size 1
  -- #154 reg = _numIntegrationSamples  key 56, rIdx = 22, size 1
  -- #155 reg = _SerialPort  key 57, rIdx = 38, size 1
  -- #156 reg = _Light2  key 59, rIdx = 41, size 1
  -- #157 reg = FSR  key 60, rIdx = 4, size 1
  -- #158 reg = _Light3  key 60, rIdx = 40, size 1
regTypeNum
serialRegAssign
  op: =
deassignLRs
createRegMask
regsUsedIniCode
rUmaskForOp
rUmaskForOp
ebbs after optimizing:

----------------------------------------------------------------
Basic Block _eBBlock0 : loop Depth = 0 noPath = 0 , lastinLoop = 0
depth 1st num 1 : bbnum = 0 1st iCode = 1 , last iCode = 1
visited 0 : hasFcall = 0

defines bitVector :bitvector Size = 109 bSize = 14
Bits on { (108) }

local defines bitVector :bitvector Size = 109 bSize = 14
Bits on { (108) }

pointers Set bitvector :
in pointers Set bitvector :
inDefs Set bitvector :
outDefs Set bitvector :bitvector Size = 109 bSize = 14
Bits on { (108) }

usesDefs Set bitvector :bitvector Size = 109 bSize = 14
Bits on { }

----------------------------------------------------------------
main.c(384:1:108:0:0)		_numIntegrationSamples [k77 lr0:0 so:0]{ ia1 re0 rm0 nos0 ru0 dp0}{unsigned char} := 0x8 {literal unsigned char}
leaving
<><><><><><><><><><><><><><><><><>
typeRegWithIdx - requesting index = 0x72
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x73
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x72
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x72
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x72
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x73
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x72
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x74
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x72
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x74
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x72
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x74
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x76
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x77
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x72
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x76
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x72
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x74
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x72
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x74
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x72
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x74
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x78
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x79
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x72
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x78
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x72
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x74
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x72
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x74
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x72
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x74
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x7a
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x7b
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x72
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x7a
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x72
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x74
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x72
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x74
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x72
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x74
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x7c
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x7d
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x72
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x7c
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x72
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x74
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x72
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x74
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x72
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x74
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x7e
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x7f
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x72
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x7e
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x72
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x74
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x72
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x74
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x72
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x74
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x80
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x81
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x72
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x80
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x72
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x74
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x72
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x74
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x72
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x74
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x82
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x83
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x72
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x82
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x72
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x74
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x72
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x74
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x72
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x74
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
Found a Stack Register!
typeRegWithIdx - requesting index = 0x65
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
Found a Stack Register!
typeRegWithIdx - requesting index = 0x65
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x1000
typeRegWithIdx - requesting index = 0x1000
typeRegWithIdx - requesting index = 0x1000
Found a Stack Register!
typeRegWithIdx - requesting index = 0x66
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x66
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
Found a Stack Register!
typeRegWithIdx - requesting index = 0x1000
typeRegWithIdx - requesting index = 0x1000
typeRegWithIdx - requesting index = 0x1000
Found a Stack Register!
typeRegWithIdx - requesting index = 0x66
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x1000
typeRegWithIdx - requesting index = 0x1000
typeRegWithIdx - requesting index = 0x1000
Found a Stack Register!
typeRegWithIdx - requesting index = 0x66
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x67
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x1000
typeRegWithIdx - requesting index = 0x1000
typeRegWithIdx - requesting index = 0x1000
Found a Stack Register!
typeRegWithIdx - requesting index = 0x68
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x68
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
Found a Stack Register!
typeRegWithIdx - requesting index = 0x65
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
Found a Stack Register!
typeRegWithIdx - requesting index = 0x65
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x1000
typeRegWithIdx - requesting index = 0x1000
typeRegWithIdx - requesting index = 0x1000
Found a Stack Register!
typeRegWithIdx - requesting index = 0x66
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x66
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
Found a Stack Register!
typeRegWithIdx - requesting index = 0x1000
typeRegWithIdx - requesting index = 0x1000
typeRegWithIdx - requesting index = 0x1000
Found a Stack Register!
typeRegWithIdx - requesting index = 0x66
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x1000
typeRegWithIdx - requesting index = 0x1000
typeRegWithIdx - requesting index = 0x1000
Found a Stack Register!
typeRegWithIdx - requesting index = 0x66
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x67
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x1000
typeRegWithIdx - requesting index = 0x1000
typeRegWithIdx - requesting index = 0x1000
Found a Stack Register!
typeRegWithIdx - requesting index = 0x69
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x68
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
Found a Stack Register!
typeRegWithIdx - requesting index = 0x65
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
Found a Stack Register!
typeRegWithIdx - requesting index = 0x65
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x1000
typeRegWithIdx - requesting index = 0x1000
typeRegWithIdx - requesting index = 0x1000
Found a Stack Register!
typeRegWithIdx - requesting index = 0x66
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x66
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
Found a Stack Register!
typeRegWithIdx - requesting index = 0x1000
typeRegWithIdx - requesting index = 0x1000
typeRegWithIdx - requesting index = 0x1000
Found a Stack Register!
typeRegWithIdx - requesting index = 0x66
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x1000
typeRegWithIdx - requesting index = 0x1000
typeRegWithIdx - requesting index = 0x1000
Found a Stack Register!
typeRegWithIdx - requesting index = 0x66
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x67
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x1000
typeRegWithIdx - requesting index = 0x1000
typeRegWithIdx - requesting index = 0x1000
Found a Stack Register!
typeRegWithIdx - requesting index = 0x69
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x68
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
Found a Stack Register!
typeRegWithIdx - requesting index = 0x65
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
Found a Stack Register!
typeRegWithIdx - requesting index = 0x65
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x1000
typeRegWithIdx - requesting index = 0x1000
typeRegWithIdx - requesting index = 0x1000
Found a Stack Register!
typeRegWithIdx - requesting index = 0x66
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x66
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
Found a Stack Register!
typeRegWithIdx - requesting index = 0x1000
typeRegWithIdx - requesting index = 0x1000
typeRegWithIdx - requesting index = 0x1000
Found a Stack Register!
typeRegWithIdx - requesting index = 0x66
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x1000
typeRegWithIdx - requesting index = 0x1000
typeRegWithIdx - requesting index = 0x1000
Found a Stack Register!
typeRegWithIdx - requesting index = 0x66
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x67
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x1000
typeRegWithIdx - requesting index = 0x1000
typeRegWithIdx - requesting index = 0x1000
Found a Stack Register!
typeRegWithIdx - requesting index = 0x69
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x68
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
Found a Stack Register!
typeRegWithIdx - requesting index = 0x65
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
Found a Stack Register!
typeRegWithIdx - requesting index = 0x65
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x1000
typeRegWithIdx - requesting index = 0x1000
typeRegWithIdx - requesting index = 0x1000
Found a Stack Register!
typeRegWithIdx - requesting index = 0x66
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x66
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
Found a Stack Register!
typeRegWithIdx - requesting index = 0x1000
typeRegWithIdx - requesting index = 0x1000
typeRegWithIdx - requesting index = 0x1000
Found a Stack Register!
typeRegWithIdx - requesting index = 0x66
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x1000
typeRegWithIdx - requesting index = 0x1000
typeRegWithIdx - requesting index = 0x1000
Found a Stack Register!
typeRegWithIdx - requesting index = 0x66
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x67
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x1000
typeRegWithIdx - requesting index = 0x1000
typeRegWithIdx - requesting index = 0x1000
Found a Stack Register!
typeRegWithIdx - requesting index = 0x69
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x68
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
Found a Stack Register!
typeRegWithIdx - requesting index = 0x65
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
Found a Stack Register!
typeRegWithIdx - requesting index = 0x65
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x1000
typeRegWithIdx - requesting index = 0x1000
typeRegWithIdx - requesting index = 0x1000
Found a Stack Register!
typeRegWithIdx - requesting index = 0x66
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x66
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
Found a Stack Register!
typeRegWithIdx - requesting index = 0x1000
typeRegWithIdx - requesting index = 0x1000
typeRegWithIdx - requesting index = 0x1000
Found a Stack Register!
typeRegWithIdx - requesting index = 0x66
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x1000
typeRegWithIdx - requesting index = 0x1000
typeRegWithIdx - requesting index = 0x1000
Found a Stack Register!
typeRegWithIdx - requesting index = 0x66
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x67
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x1000
typeRegWithIdx - requesting index = 0x1000
typeRegWithIdx - requesting index = 0x1000
Found a Stack Register!
typeRegWithIdx - requesting index = 0x69
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x68
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6a
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6b
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6a
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6b
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6a
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6b
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6a
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6a
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
Found a Stack Register!
typeRegWithIdx - requesting index = 0x65
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
Found a Stack Register!
typeRegWithIdx - requesting index = 0x65
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x1000
typeRegWithIdx - requesting index = 0x1000
typeRegWithIdx - requesting index = 0x1000
Found a Stack Register!
typeRegWithIdx - requesting index = 0x66
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x66
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
Found a Stack Register!
typeRegWithIdx - requesting index = 0x1000
typeRegWithIdx - requesting index = 0x1000
typeRegWithIdx - requesting index = 0x1000
Found a Stack Register!
typeRegWithIdx - requesting index = 0x66
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x1000
typeRegWithIdx - requesting index = 0x1000
typeRegWithIdx - requesting index = 0x1000
Found a Stack Register!
typeRegWithIdx - requesting index = 0x66
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x67
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x1000
typeRegWithIdx - requesting index = 0x1000
typeRegWithIdx - requesting index = 0x1000
Found a Stack Register!
typeRegWithIdx - requesting index = 0x69
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x68
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6a
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6b
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6a
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6b
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6a
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6b
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6c
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6a
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6b
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6b
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
Found a Stack Register!
typeRegWithIdx - requesting index = 0x65
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
Found a Stack Register!
typeRegWithIdx - requesting index = 0x65
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x1000
typeRegWithIdx - requesting index = 0x1000
typeRegWithIdx - requesting index = 0x1000
Found a Stack Register!
typeRegWithIdx - requesting index = 0x66
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x66
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
Found a Stack Register!
typeRegWithIdx - requesting index = 0x1000
typeRegWithIdx - requesting index = 0x1000
typeRegWithIdx - requesting index = 0x1000
Found a Stack Register!
typeRegWithIdx - requesting index = 0x66
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x1000
typeRegWithIdx - requesting index = 0x1000
typeRegWithIdx - requesting index = 0x1000
Found a Stack Register!
typeRegWithIdx - requesting index = 0x66
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x67
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x1000
typeRegWithIdx - requesting index = 0x1000
typeRegWithIdx - requesting index = 0x1000
Found a Stack Register!
typeRegWithIdx - requesting index = 0x69
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x68
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6a
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6b
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6a
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6b
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6a
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6b
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6c
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6a
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6c
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
Found a Stack Register!
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
Found a Stack Register!
typeRegWithIdx - requesting index = 0x6a
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6c
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
Found a Stack Register!
typeRegWithIdx - requesting index = 0x6c
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
typeRegWithIdx - requesting index = 0x5a
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x5b
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x5b
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x59
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x5a
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x5b
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x5a
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x5b
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x5c
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x5a
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
typeRegWithIdx - requesting index = 0x5a
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x5b
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x5b
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x59
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x5a
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x5b
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x5a
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x5b
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x5c
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x5a
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
typeRegWithIdx - requesting index = 0x5a
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x5b
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x5b
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x59
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x5a
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x5b
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x5a
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x5b
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x5c
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x5a
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
typeRegWithIdx - requesting index = 0x5a
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x5b
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x5b
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x59
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x5a
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x5b
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x5a
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x5b
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x5c
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x5a
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
typeRegWithIdx - requesting index = 0x64
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
Found a Stack Register!
typeRegWithIdx - requesting index = 0x64
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x63
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x63
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
typeRegWithIdx - requesting index = 0x5a
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x5b
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x5b
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x59
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x5a
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x5b
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x5a
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x5b
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x5c
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x5a
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
typeRegWithIdx - requesting index = 0x64
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
Found a Stack Register!
typeRegWithIdx - requesting index = 0x64
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x63
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
Found a Stack Register!
typeRegWithIdx - requesting index = 0x5b
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x5c
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
typeRegWithIdx - requesting index = 0x72
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x73
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x72
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x74
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x72
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x74
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x72
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x74
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x76
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x77
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x72
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x76
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x72
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x74
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x72
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x74
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x72
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x74
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x78
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x79
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x72
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x78
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x72
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x74
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x72
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x74
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x72
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x74
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x7a
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x7b
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x72
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x7a
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x72
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x74
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x72
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x74
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x72
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x74
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x7c
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x7d
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x72
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x7c
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x72
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x74
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x72
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x74
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x72
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x74
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x7e
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x7f
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x72
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x7e
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x72
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x74
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x72
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x74
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x72
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x74
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x80
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x81
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x72
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x80
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x72
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x74
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x72
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x74
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x72
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x74
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x82
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x83
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x72
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x82
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x72
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x74
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x72
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x74
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x72
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x74
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
Found a Stack Register!
typeRegWithIdx - requesting index = 0x65
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
Found a Stack Register!
typeRegWithIdx - requesting index = 0x65
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x1000
typeRegWithIdx - requesting index = 0x1000
typeRegWithIdx - requesting index = 0x1000
Found a Stack Register!
typeRegWithIdx - requesting index = 0x66
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x66
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
Found a Stack Register!
typeRegWithIdx - requesting index = 0x1000
typeRegWithIdx - requesting index = 0x1000
typeRegWithIdx - requesting index = 0x1000
Found a Stack Register!
typeRegWithIdx - requesting index = 0x66
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x1000
typeRegWithIdx - requesting index = 0x1000
typeRegWithIdx - requesting index = 0x1000
Found a Stack Register!
typeRegWithIdx - requesting index = 0x66
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x67
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x1000
typeRegWithIdx - requesting index = 0x1000
typeRegWithIdx - requesting index = 0x1000
Found a Stack Register!
typeRegWithIdx - requesting index = 0x69
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x68
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6a
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6b
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6a
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6b
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6a
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6b
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6c
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6a
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6c
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
Found a Stack Register!
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
Found a Stack Register!
typeRegWithIdx - requesting index = 0x6a
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6c
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
Found a Stack Register!
typeRegWithIdx - requesting index = 0x6c
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
typeRegWithIdx - requesting index = 0x5a
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x5b
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x5b
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x59
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x5a
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x5b
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x5a
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x5b
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x5c
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x5a
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
typeRegWithIdx - requesting index = 0x64
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
Found a Stack Register!
typeRegWithIdx - requesting index = 0x64
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x63
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
Found a Stack Register!
typeRegWithIdx - requesting index = 0x5b
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x5c
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
typeRegWithIdx - requesting index = 0x6e
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6e
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6e
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x70
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x71
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6e
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6e
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6e
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6e
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x70
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x71
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6e
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6e
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6e
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6e
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x70
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x71
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6e
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6e
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6e
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6e
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x70
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x71
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6e
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6e
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6e
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6e
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x70
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x71
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6e
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6e
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6e
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6e
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x70
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x71
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6e
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6e
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6e
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6e
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x70
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x71
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6e
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6e
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6e
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6e
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x70
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x71
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6e
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6e
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6e
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6e
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x70
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x71
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6e
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6e
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6e
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6e
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x70
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x71
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6e
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6e
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6e
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6e
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x70
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x71
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6e
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6e
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6e
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6e
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x70
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x71
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6e
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6e
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6e
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6e
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x70
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x71
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6e
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6e
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6e
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6e
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x70
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x71
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6e
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6e
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6e
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6e
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x70
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x71
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6e
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6e
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6e
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6e
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x70
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x71
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6e
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6e
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6e
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6e
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x70
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x71
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6e
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6e
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6e
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6e
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x70
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x71
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6e
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
typeRegWithIdx - requesting index = 0x81
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x82
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x83
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x70
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x71
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6e
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6f
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x65
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
Found a Stack Register!
typeRegWithIdx - requesting index = 0x66
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x1000
typeRegWithIdx - requesting index = 0x1000
typeRegWithIdx - requesting index = 0x1000
Found a Stack Register!
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
typeRegWithIdx - requesting index = 0x6d
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x65
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
Found a Stack Register!
typeRegWithIdx - requesting index = 0x66
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x1000
typeRegWithIdx - requesting index = 0x1000
typeRegWithIdx - requesting index = 0x1000
Found a Stack Register!
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
typeRegWithIdx - requesting index = 0x6d
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x65
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
Found a Stack Register!
typeRegWithIdx - requesting index = 0x66
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x1000
typeRegWithIdx - requesting index = 0x1000
typeRegWithIdx - requesting index = 0x1000
Found a Stack Register!
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
typeRegWithIdx - requesting index = 0x6d
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x65
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
Found a Stack Register!
typeRegWithIdx - requesting index = 0x66
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x1000
typeRegWithIdx - requesting index = 0x1000
typeRegWithIdx - requesting index = 0x1000
Found a Stack Register!
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
typeRegWithIdx - requesting index = 0x6d
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x65
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
Found a Stack Register!
typeRegWithIdx - requesting index = 0x66
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x1000
typeRegWithIdx - requesting index = 0x1000
typeRegWithIdx - requesting index = 0x1000
Found a Stack Register!
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
typeRegWithIdx - requesting index = 0x6d
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
typeRegWithIdx - requesting index = 0x5b
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x5c
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x1000
typeRegWithIdx - requesting index = 0x1000
typeRegWithIdx - requesting index = 0x1000
Found a Stack Register!
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
typeRegWithIdx - requesting index = 0x64
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x63
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
typeRegWithIdx - requesting index = 0x6d
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x65
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
Found a Stack Register!
typeRegWithIdx - requesting index = 0x66
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x1000
typeRegWithIdx - requesting index = 0x1000
typeRegWithIdx - requesting index = 0x1000
Found a Stack Register!
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
typeRegWithIdx - requesting index = 0x6d
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x65
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
Found a Stack Register!
typeRegWithIdx - requesting index = 0x66
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x1000
typeRegWithIdx - requesting index = 0x1000
typeRegWithIdx - requesting index = 0x1000
Found a Stack Register!
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
typeRegWithIdx - requesting index = 0x6d
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x65
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
Found a Stack Register!
typeRegWithIdx - requesting index = 0x66
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x1000
typeRegWithIdx - requesting index = 0x1000
typeRegWithIdx - requesting index = 0x1000
Found a Stack Register!
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
typeRegWithIdx - requesting index = 0x6d
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x65
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
Found a Stack Register!
typeRegWithIdx - requesting index = 0x66
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x1000
typeRegWithIdx - requesting index = 0x1000
typeRegWithIdx - requesting index = 0x1000
Found a Stack Register!
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
typeRegWithIdx - requesting index = 0x6d
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
typeRegWithIdx - requesting index = 0x5b
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x5c
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x1000
typeRegWithIdx - requesting index = 0x1000
typeRegWithIdx - requesting index = 0x1000
Found a Stack Register!
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
typeRegWithIdx - requesting index = 0x64
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x63
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
typeRegWithIdx - requesting index = 0x6d
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x65
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
Found a Stack Register!
typeRegWithIdx - requesting index = 0x66
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x1000
typeRegWithIdx - requesting index = 0x1000
typeRegWithIdx - requesting index = 0x1000
Found a Stack Register!
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
typeRegWithIdx - requesting index = 0x6d
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x65
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
Found a Stack Register!
typeRegWithIdx - requesting index = 0x66
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x1000
typeRegWithIdx - requesting index = 0x1000
typeRegWithIdx - requesting index = 0x1000
Found a Stack Register!
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
typeRegWithIdx - requesting index = 0x6d
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x65
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
Found a Stack Register!
typeRegWithIdx - requesting index = 0x66
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x1000
typeRegWithIdx - requesting index = 0x1000
typeRegWithIdx - requesting index = 0x1000
Found a Stack Register!
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
typeRegWithIdx - requesting index = 0x6d
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x65
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
Found a Stack Register!
typeRegWithIdx - requesting index = 0x66
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x1000
typeRegWithIdx - requesting index = 0x1000
typeRegWithIdx - requesting index = 0x1000
Found a Stack Register!
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
typeRegWithIdx - requesting index = 0x6d
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
typeRegWithIdx - requesting index = 0x5b
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x5c
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x1000
typeRegWithIdx - requesting index = 0x1000
typeRegWithIdx - requesting index = 0x1000
Found a Stack Register!
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
typeRegWithIdx - requesting index = 0x64
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x63
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
typeRegWithIdx - requesting index = 0x6d
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x65
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
Found a Stack Register!
typeRegWithIdx - requesting index = 0x66
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x1000
typeRegWithIdx - requesting index = 0x1000
typeRegWithIdx - requesting index = 0x1000
Found a Stack Register!
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
typeRegWithIdx - requesting index = 0x6d
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x65
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
Found a Stack Register!
typeRegWithIdx - requesting index = 0x66
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x1000
typeRegWithIdx - requesting index = 0x1000
typeRegWithIdx - requesting index = 0x1000
Found a Stack Register!
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
typeRegWithIdx - requesting index = 0x6d
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x65
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
Found a Stack Register!
typeRegWithIdx - requesting index = 0x66
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x1000
typeRegWithIdx - requesting index = 0x1000
typeRegWithIdx - requesting index = 0x1000
Found a Stack Register!
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
typeRegWithIdx - requesting index = 0x6d
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x65
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
Found a Stack Register!
typeRegWithIdx - requesting index = 0x66
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x1000
typeRegWithIdx - requesting index = 0x1000
typeRegWithIdx - requesting index = 0x1000
Found a Stack Register!
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
typeRegWithIdx - requesting index = 0x6d
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
typeRegWithIdx - requesting index = 0x5b
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x5c
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x1000
typeRegWithIdx - requesting index = 0x1000
typeRegWithIdx - requesting index = 0x1000
Found a Stack Register!
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
typeRegWithIdx - requesting index = 0x64
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x63
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
typeRegWithIdx - requesting index = 0x6d
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x65
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
Found a Stack Register!
typeRegWithIdx - requesting index = 0x66
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x1000
typeRegWithIdx - requesting index = 0x1000
typeRegWithIdx - requesting index = 0x1000
Found a Stack Register!
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
typeRegWithIdx - requesting index = 0x6d
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x65
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
Found a Stack Register!
typeRegWithIdx - requesting index = 0x66
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x1000
typeRegWithIdx - requesting index = 0x1000
typeRegWithIdx - requesting index = 0x1000
Found a Stack Register!
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
typeRegWithIdx - requesting index = 0x6d
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x65
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
Found a Stack Register!
typeRegWithIdx - requesting index = 0x66
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x1000
typeRegWithIdx - requesting index = 0x1000
typeRegWithIdx - requesting index = 0x1000
Found a Stack Register!
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
typeRegWithIdx - requesting index = 0x6d
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x65
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
Found a Stack Register!
typeRegWithIdx - requesting index = 0x66
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x1000
typeRegWithIdx - requesting index = 0x1000
typeRegWithIdx - requesting index = 0x1000
Found a Stack Register!
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
typeRegWithIdx - requesting index = 0x6d
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
typeRegWithIdx - requesting index = 0x5b
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x5c
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x1000
typeRegWithIdx - requesting index = 0x1000
typeRegWithIdx - requesting index = 0x1000
Found a Stack Register!
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
typeRegWithIdx - requesting index = 0x64
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x63
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
typeRegWithIdx - requesting index = 0x6d
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x65
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
Found a Stack Register!
typeRegWithIdx - requesting index = 0x66
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x1000
typeRegWithIdx - requesting index = 0x1000
typeRegWithIdx - requesting index = 0x1000
Found a Stack Register!
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
typeRegWithIdx - requesting index = 0x6d
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x65
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
Found a Stack Register!
typeRegWithIdx - requesting index = 0x66
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x1000
typeRegWithIdx - requesting index = 0x1000
typeRegWithIdx - requesting index = 0x1000
Found a Stack Register!
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
typeRegWithIdx - requesting index = 0x6d
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x65
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
Found a Stack Register!
typeRegWithIdx - requesting index = 0x66
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x1000
typeRegWithIdx - requesting index = 0x1000
typeRegWithIdx - requesting index = 0x1000
Found a Stack Register!
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
typeRegWithIdx - requesting index = 0x6d
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x65
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
Found a Stack Register!
typeRegWithIdx - requesting index = 0x66
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x1000
typeRegWithIdx - requesting index = 0x1000
typeRegWithIdx - requesting index = 0x1000
Found a Stack Register!
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
typeRegWithIdx - requesting index = 0x6d
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
typeRegWithIdx - requesting index = 0x5b
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x5c
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x1000
typeRegWithIdx - requesting index = 0x1000
typeRegWithIdx - requesting index = 0x1000
Found a Stack Register!
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
typeRegWithIdx - requesting index = 0x64
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x63
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
typeRegWithIdx - requesting index = 0x6d
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x65
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
Found a Stack Register!
typeRegWithIdx - requesting index = 0x66
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x1000
typeRegWithIdx - requesting index = 0x1000
typeRegWithIdx - requesting index = 0x1000
Found a Stack Register!
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
typeRegWithIdx - requesting index = 0x6d
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x65
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
Found a Stack Register!
typeRegWithIdx - requesting index = 0x66
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x1000
typeRegWithIdx - requesting index = 0x1000
typeRegWithIdx - requesting index = 0x1000
Found a Stack Register!
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
typeRegWithIdx - requesting index = 0x6d
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x65
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
Found a Stack Register!
typeRegWithIdx - requesting index = 0x66
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x1000
typeRegWithIdx - requesting index = 0x1000
typeRegWithIdx - requesting index = 0x1000
Found a Stack Register!
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
typeRegWithIdx - requesting index = 0x6d
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x65
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
Found a Stack Register!
typeRegWithIdx - requesting index = 0x66
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x1000
typeRegWithIdx - requesting index = 0x1000
typeRegWithIdx - requesting index = 0x1000
Found a Stack Register!
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
typeRegWithIdx - requesting index = 0x6d
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
typeRegWithIdx - requesting index = 0x5b
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x5c
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x1000
typeRegWithIdx - requesting index = 0x1000
typeRegWithIdx - requesting index = 0x1000
Found a Stack Register!
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
typeRegWithIdx - requesting index = 0x64
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x63
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
typeRegWithIdx - requesting index = 0x6d
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x65
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
Found a Stack Register!
typeRegWithIdx - requesting index = 0x66
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x1000
typeRegWithIdx - requesting index = 0x1000
typeRegWithIdx - requesting index = 0x1000
Found a Stack Register!
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
typeRegWithIdx - requesting index = 0x6d
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x65
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
Found a Stack Register!
typeRegWithIdx - requesting index = 0x66
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x1000
typeRegWithIdx - requesting index = 0x1000
typeRegWithIdx - requesting index = 0x1000
Found a Stack Register!
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
typeRegWithIdx - requesting index = 0x6d
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x65
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
Found a Stack Register!
typeRegWithIdx - requesting index = 0x66
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x1000
typeRegWithIdx - requesting index = 0x1000
typeRegWithIdx - requesting index = 0x1000
Found a Stack Register!
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
typeRegWithIdx - requesting index = 0x6d
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x65
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
Found a Stack Register!
typeRegWithIdx - requesting index = 0x66
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x1000
typeRegWithIdx - requesting index = 0x1000
typeRegWithIdx - requesting index = 0x1000
Found a Stack Register!
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
typeRegWithIdx - requesting index = 0x6d
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
typeRegWithIdx - requesting index = 0x5b
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x5c
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x1000
typeRegWithIdx - requesting index = 0x1000
typeRegWithIdx - requesting index = 0x1000
Found a Stack Register!
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
typeRegWithIdx - requesting index = 0x64
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x63
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
typeRegWithIdx - requesting index = 0x6d
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x65
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
Found a Stack Register!
typeRegWithIdx - requesting index = 0x66
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x1000
typeRegWithIdx - requesting index = 0x1000
typeRegWithIdx - requesting index = 0x1000
Found a Stack Register!
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
typeRegWithIdx - requesting index = 0x6d
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x65
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
Found a Stack Register!
typeRegWithIdx - requesting index = 0x66
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x1000
typeRegWithIdx - requesting index = 0x1000
typeRegWithIdx - requesting index = 0x1000
Found a Stack Register!
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
typeRegWithIdx - requesting index = 0x6d
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x65
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
Found a Stack Register!
typeRegWithIdx - requesting index = 0x66
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x1000
typeRegWithIdx - requesting index = 0x1000
typeRegWithIdx - requesting index = 0x1000
Found a Stack Register!
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
typeRegWithIdx - requesting index = 0x6d
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x65
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
Found a Stack Register!
typeRegWithIdx - requesting index = 0x66
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x1000
typeRegWithIdx - requesting index = 0x1000
typeRegWithIdx - requesting index = 0x1000
Found a Stack Register!
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
typeRegWithIdx - requesting index = 0x6d
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
typeRegWithIdx - requesting index = 0x5b
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x5c
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x1000
typeRegWithIdx - requesting index = 0x1000
typeRegWithIdx - requesting index = 0x1000
Found a Stack Register!
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
typeRegWithIdx - requesting index = 0x64
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x63
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
typeRegWithIdx - requesting index = 0x6d
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
typeRegWithIdx - requesting index = 0x5b
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x5c
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
typeRegWithIdx - requesting index = 0x64
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x63
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
typeRegWithIdx - requesting index = 0x6d
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x65
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
Found a Stack Register!
typeRegWithIdx - requesting index = 0x66
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x1000
typeRegWithIdx - requesting index = 0x1000
typeRegWithIdx - requesting index = 0x1000
Found a Stack Register!
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
typeRegWithIdx - requesting index = 0x6d
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x65
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
Found a Stack Register!
typeRegWithIdx - requesting index = 0x66
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x1000
typeRegWithIdx - requesting index = 0x1000
typeRegWithIdx - requesting index = 0x1000
Found a Stack Register!
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
typeRegWithIdx - requesting index = 0x6d
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x65
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
Found a Stack Register!
typeRegWithIdx - requesting index = 0x66
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x1000
typeRegWithIdx - requesting index = 0x1000
typeRegWithIdx - requesting index = 0x1000
Found a Stack Register!
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
typeRegWithIdx - requesting index = 0x6d
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x65
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
Found a Stack Register!
typeRegWithIdx - requesting index = 0x66
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x1000
typeRegWithIdx - requesting index = 0x1000
typeRegWithIdx - requesting index = 0x1000
Found a Stack Register!
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
typeRegWithIdx - requesting index = 0x6d
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
typeRegWithIdx - requesting index = 0x5b
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x5c
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x1000
typeRegWithIdx - requesting index = 0x1000
typeRegWithIdx - requesting index = 0x1000
Found a Stack Register!
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
typeRegWithIdx - requesting index = 0x64
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x63
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
typeRegWithIdx - requesting index = 0x6d
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x65
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
Found a Stack Register!
typeRegWithIdx - requesting index = 0x66
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x1000
typeRegWithIdx - requesting index = 0x1000
typeRegWithIdx - requesting index = 0x1000
Found a Stack Register!
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
typeRegWithIdx - requesting index = 0x6d
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x65
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
Found a Stack Register!
typeRegWithIdx - requesting index = 0x66
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x1000
typeRegWithIdx - requesting index = 0x1000
typeRegWithIdx - requesting index = 0x1000
Found a Stack Register!
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
typeRegWithIdx - requesting index = 0x6d
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x65
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
Found a Stack Register!
typeRegWithIdx - requesting index = 0x66
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x1000
typeRegWithIdx - requesting index = 0x1000
typeRegWithIdx - requesting index = 0x1000
Found a Stack Register!
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
typeRegWithIdx - requesting index = 0x6d
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x65
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
Found a Stack Register!
typeRegWithIdx - requesting index = 0x66
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x1000
typeRegWithIdx - requesting index = 0x1000
typeRegWithIdx - requesting index = 0x1000
Found a Stack Register!
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
typeRegWithIdx - requesting index = 0x6d
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
typeRegWithIdx - requesting index = 0x5b
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x5c
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x1000
typeRegWithIdx - requesting index = 0x1000
typeRegWithIdx - requesting index = 0x1000
Found a Stack Register!
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
typeRegWithIdx - requesting index = 0x64
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x63
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
typeRegWithIdx - requesting index = 0x6d
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x65
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
Found a Stack Register!
typeRegWithIdx - requesting index = 0x66
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x1000
typeRegWithIdx - requesting index = 0x1000
typeRegWithIdx - requesting index = 0x1000
Found a Stack Register!
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
typeRegWithIdx - requesting index = 0x6d
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x65
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
Found a Stack Register!
typeRegWithIdx - requesting index = 0x66
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x1000
typeRegWithIdx - requesting index = 0x1000
typeRegWithIdx - requesting index = 0x1000
Found a Stack Register!
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
typeRegWithIdx - requesting index = 0x6d
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x65
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
Found a Stack Register!
typeRegWithIdx - requesting index = 0x66
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x1000
typeRegWithIdx - requesting index = 0x1000
typeRegWithIdx - requesting index = 0x1000
Found a Stack Register!
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
typeRegWithIdx - requesting index = 0x6d
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x65
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
Found a Stack Register!
typeRegWithIdx - requesting index = 0x66
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x1000
typeRegWithIdx - requesting index = 0x1000
typeRegWithIdx - requesting index = 0x1000
Found a Stack Register!
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
typeRegWithIdx - requesting index = 0x6d
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
typeRegWithIdx - requesting index = 0x5b
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x5c
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x1000
typeRegWithIdx - requesting index = 0x1000
typeRegWithIdx - requesting index = 0x1000
Found a Stack Register!
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
typeRegWithIdx - requesting index = 0x64
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x63
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
typeRegWithIdx - requesting index = 0x6d
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x65
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
Found a Stack Register!
typeRegWithIdx - requesting index = 0x66
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x1000
typeRegWithIdx - requesting index = 0x1000
typeRegWithIdx - requesting index = 0x1000
Found a Stack Register!
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
typeRegWithIdx - requesting index = 0x6d
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x65
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
Found a Stack Register!
typeRegWithIdx - requesting index = 0x66
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x1000
typeRegWithIdx - requesting index = 0x1000
typeRegWithIdx - requesting index = 0x1000
Found a Stack Register!
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
typeRegWithIdx - requesting index = 0x6d
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x65
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
Found a Stack Register!
typeRegWithIdx - requesting index = 0x66
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x1000
typeRegWithIdx - requesting index = 0x1000
typeRegWithIdx - requesting index = 0x1000
Found a Stack Register!
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
typeRegWithIdx - requesting index = 0x6d
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x65
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
Found a Stack Register!
typeRegWithIdx - requesting index = 0x66
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x1000
typeRegWithIdx - requesting index = 0x1000
typeRegWithIdx - requesting index = 0x1000
Found a Stack Register!
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
typeRegWithIdx - requesting index = 0x6d
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
typeRegWithIdx - requesting index = 0x5b
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x5c
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x1000
typeRegWithIdx - requesting index = 0x1000
typeRegWithIdx - requesting index = 0x1000
Found a Stack Register!
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
typeRegWithIdx - requesting index = 0x64
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x63
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
typeRegWithIdx - requesting index = 0x6d
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x65
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
Found a Stack Register!
typeRegWithIdx - requesting index = 0x66
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x1000
typeRegWithIdx - requesting index = 0x1000
typeRegWithIdx - requesting index = 0x1000
Found a Stack Register!
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
typeRegWithIdx - requesting index = 0x6d
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x65
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
Found a Stack Register!
typeRegWithIdx - requesting index = 0x66
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x1000
typeRegWithIdx - requesting index = 0x1000
typeRegWithIdx - requesting index = 0x1000
Found a Stack Register!
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
typeRegWithIdx - requesting index = 0x6d
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x65
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
Found a Stack Register!
typeRegWithIdx - requesting index = 0x66
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x1000
typeRegWithIdx - requesting index = 0x1000
typeRegWithIdx - requesting index = 0x1000
Found a Stack Register!
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
typeRegWithIdx - requesting index = 0x6d
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x65
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
Found a Stack Register!
typeRegWithIdx - requesting index = 0x66
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x1000
typeRegWithIdx - requesting index = 0x1000
typeRegWithIdx - requesting index = 0x1000
Found a Stack Register!
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
typeRegWithIdx - requesting index = 0x6d
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
typeRegWithIdx - requesting index = 0x5b
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x5c
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x1000
typeRegWithIdx - requesting index = 0x1000
typeRegWithIdx - requesting index = 0x1000
Found a Stack Register!
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
typeRegWithIdx - requesting index = 0x64
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x63
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
typeRegWithIdx - requesting index = 0x6d
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x65
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
Found a Stack Register!
typeRegWithIdx - requesting index = 0x66
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x1000
typeRegWithIdx - requesting index = 0x1000
typeRegWithIdx - requesting index = 0x1000
Found a Stack Register!
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
typeRegWithIdx - requesting index = 0x6d
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x65
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
Found a Stack Register!
typeRegWithIdx - requesting index = 0x66
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x1000
typeRegWithIdx - requesting index = 0x1000
typeRegWithIdx - requesting index = 0x1000
Found a Stack Register!
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
typeRegWithIdx - requesting index = 0x6d
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x65
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
Found a Stack Register!
typeRegWithIdx - requesting index = 0x66
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x1000
typeRegWithIdx - requesting index = 0x1000
typeRegWithIdx - requesting index = 0x1000
Found a Stack Register!
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
typeRegWithIdx - requesting index = 0x6d
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x65
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
Found a Stack Register!
typeRegWithIdx - requesting index = 0x66
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x1000
typeRegWithIdx - requesting index = 0x1000
typeRegWithIdx - requesting index = 0x1000
Found a Stack Register!
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
typeRegWithIdx - requesting index = 0x6d
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
typeRegWithIdx - requesting index = 0x5b
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x5c
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x1000
typeRegWithIdx - requesting index = 0x1000
typeRegWithIdx - requesting index = 0x1000
Found a Stack Register!
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
typeRegWithIdx - requesting index = 0x64
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x63
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
typeRegWithIdx - requesting index = 0x6d
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x65
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
Found a Stack Register!
typeRegWithIdx - requesting index = 0x66
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x1000
typeRegWithIdx - requesting index = 0x1000
typeRegWithIdx - requesting index = 0x1000
Found a Stack Register!
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
typeRegWithIdx - requesting index = 0x6d
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x65
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
Found a Stack Register!
typeRegWithIdx - requesting index = 0x66
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x1000
typeRegWithIdx - requesting index = 0x1000
typeRegWithIdx - requesting index = 0x1000
Found a Stack Register!
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
typeRegWithIdx - requesting index = 0x6d
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x65
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
Found a Stack Register!
typeRegWithIdx - requesting index = 0x66
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x1000
typeRegWithIdx - requesting index = 0x1000
typeRegWithIdx - requesting index = 0x1000
Found a Stack Register!
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
typeRegWithIdx - requesting index = 0x6d
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x65
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
Found a Stack Register!
typeRegWithIdx - requesting index = 0x66
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x1000
typeRegWithIdx - requesting index = 0x1000
typeRegWithIdx - requesting index = 0x1000
Found a Stack Register!
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
typeRegWithIdx - requesting index = 0x6d
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
typeRegWithIdx - requesting index = 0x5b
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x5c
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x1000
typeRegWithIdx - requesting index = 0x1000
typeRegWithIdx - requesting index = 0x1000
Found a Stack Register!
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
typeRegWithIdx - requesting index = 0x64
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x63
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
typeRegWithIdx - requesting index = 0x6d
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x65
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
Found a Stack Register!
typeRegWithIdx - requesting index = 0x66
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x1000
typeRegWithIdx - requesting index = 0x1000
typeRegWithIdx - requesting index = 0x1000
Found a Stack Register!
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
typeRegWithIdx - requesting index = 0x6d
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x65
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
Found a Stack Register!
typeRegWithIdx - requesting index = 0x66
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x1000
typeRegWithIdx - requesting index = 0x1000
typeRegWithIdx - requesting index = 0x1000
Found a Stack Register!
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
typeRegWithIdx - requesting index = 0x6d
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x65
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
Found a Stack Register!
typeRegWithIdx - requesting index = 0x66
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x1000
typeRegWithIdx - requesting index = 0x1000
typeRegWithIdx - requesting index = 0x1000
Found a Stack Register!
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
typeRegWithIdx - requesting index = 0x6d
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x65
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
Found a Stack Register!
typeRegWithIdx - requesting index = 0x66
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x1000
typeRegWithIdx - requesting index = 0x1000
typeRegWithIdx - requesting index = 0x1000
Found a Stack Register!
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
typeRegWithIdx - requesting index = 0x6d
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
typeRegWithIdx - requesting index = 0x5b
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x5c
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x1000
typeRegWithIdx - requesting index = 0x1000
typeRegWithIdx - requesting index = 0x1000
Found a Stack Register!
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
typeRegWithIdx - requesting index = 0x64
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x63
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
typeRegWithIdx - requesting index = 0x6d
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
typeRegWithIdx - requesting index = 0x5b
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x5c
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
typeRegWithIdx - requesting index = 0x64
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x63
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
typeRegWithIdx - requesting index = 0x6d
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6e
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6e
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6e
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x70
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x71
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6e
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6e
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6e
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6e
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x70
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x71
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6e
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6e
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6e
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6e
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x70
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x71
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6e
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6e
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6e
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6e
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x70
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x71
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6e
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6e
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6e
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6e
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x70
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x71
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6e
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6e
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6e
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6e
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x70
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x71
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6e
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6e
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6e
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6e
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x70
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x71
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6e
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6e
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6e
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6e
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x70
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x71
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6e
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6e
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6e
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6e
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x70
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x71
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6e
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6e
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6e
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6e
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x70
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x71
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6e
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6e
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6e
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6e
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x70
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x71
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6e
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6e
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6e
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6e
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x70
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x71
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6e
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6e
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6e
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6e
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x70
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x71
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6e
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6e
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6e
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6e
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x70
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x71
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6e
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6e
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6e
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6e
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x70
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x71
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6e
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6e
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6e
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6e
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x70
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x71
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6e
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6e
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6e
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6e
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x70
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x71
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6e
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6e
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6e
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6e
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x70
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x71
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6e
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
typeRegWithIdx - requesting index = 0x81
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x82
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x83
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x70
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x71
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6e
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
typeRegWithIdx - requesting index = 0x81
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x82
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x83
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x70
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x71
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6e
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x72
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x73
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x72
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x74
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x72
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x74
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x72
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x74
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x76
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x77
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x72
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x76
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x72
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x74
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x72
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x74
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x72
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x74
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x78
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x79
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x72
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x78
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x72
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x74
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x72
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x74
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x72
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x74
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x7a
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x7b
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x72
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x7a
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x72
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x74
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x72
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x74
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x72
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x74
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x7c
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x7d
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x72
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x7c
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x72
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x74
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x72
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x74
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x72
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x74
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x7e
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x7f
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x72
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x7e
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x72
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x74
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x72
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x74
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x72
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x74
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x80
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x81
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x72
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x80
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x72
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x74
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x72
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x74
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x72
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x74
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x82
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x83
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x72
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x82
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x72
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x74
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x72
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x74
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x72
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x74
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
Found a Stack Register!
typeRegWithIdx - requesting index = 0x65
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
Found a Stack Register!
typeRegWithIdx - requesting index = 0x65
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x1000
typeRegWithIdx - requesting index = 0x1000
typeRegWithIdx - requesting index = 0x1000
Found a Stack Register!
typeRegWithIdx - requesting index = 0x66
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x66
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
Found a Stack Register!
typeRegWithIdx - requesting index = 0x1000
typeRegWithIdx - requesting index = 0x1000
typeRegWithIdx - requesting index = 0x1000
Found a Stack Register!
typeRegWithIdx - requesting index = 0x66
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x1000
typeRegWithIdx - requesting index = 0x1000
typeRegWithIdx - requesting index = 0x1000
Found a Stack Register!
typeRegWithIdx - requesting index = 0x66
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x67
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x1000
typeRegWithIdx - requesting index = 0x1000
typeRegWithIdx - requesting index = 0x1000
Found a Stack Register!
typeRegWithIdx - requesting index = 0x69
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x68
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6a
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6b
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6a
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6b
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6a
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6b
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6c
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6a
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6c
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
Found a Stack Register!
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
Found a Stack Register!
typeRegWithIdx - requesting index = 0x6a
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6c
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
Found a Stack Register!
typeRegWithIdx - requesting index = 0x6c
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
typeRegWithIdx - requesting index = 0x5a
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x5b
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x5b
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x59
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x5a
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x5b
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x5a
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x5b
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x5c
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x5a
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
typeRegWithIdx - requesting index = 0x64
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
Found a Stack Register!
typeRegWithIdx - requesting index = 0x64
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x63
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
Found a Stack Register!
typeRegWithIdx - requesting index = 0x5b
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x5c
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
typeRegWithIdx - requesting index = 0x72
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x73
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x73
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x72
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x74
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x72
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x74
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x74
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x72
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x75
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x76
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x77
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x77
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x76
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x72
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x74
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x72
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x74
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x74
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x72
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x75
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x78
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x79
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x79
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x78
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x72
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x74
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x72
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x74
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x74
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x72
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x75
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x7a
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x7b
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x7b
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x7a
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x72
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x74
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x72
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x74
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x74
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x72
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x75
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x7c
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x7d
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x7d
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x7c
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x72
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x74
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x72
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x74
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x74
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x72
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x75
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x7e
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x7f
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x7f
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x7e
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x72
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x74
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x72
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x74
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x74
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x72
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x75
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x80
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x81
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x81
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x80
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x72
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x74
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x72
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x74
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x74
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x72
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x75
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x82
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x83
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x83
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x82
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x72
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x74
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x72
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x74
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x74
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x72
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x75
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x75
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x70
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x71
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x70
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x71
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6f
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6f
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6e
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6e
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x65
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
Found a Stack Register!
typeRegWithIdx - requesting index = 0x66
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x1000
typeRegWithIdx - requesting index = 0x1000
typeRegWithIdx - requesting index = 0x1000
Found a Stack Register!
typeRegWithIdx - requesting index = 0x67
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x66
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x67
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x67
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x67
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x67
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x68
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x69
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x65
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x69
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6a
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6b
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6a
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6c
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6a
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6c
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6a
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6b
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6d
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6b
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6a
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6d
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6c
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6a
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
Found a Stack Register!
typeRegWithIdx - requesting index = 0x6c
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
typeRegWithIdx - requesting index = 0x59
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x5b
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x5a
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x5c
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
typeRegWithIdx - requesting index = 0x55
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x5e
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x5e
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
typeRegWithIdx - requesting index = 0x60
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
Found a Stack Register!
typeRegWithIdx - requesting index = 0x5f
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x57
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x60
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x60
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
typeRegWithIdx - requesting index = 0x62
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
Found a Stack Register!
typeRegWithIdx - requesting index = 0x61
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x55
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x62
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x62
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
typeRegWithIdx - requesting index = 0x64
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
Found a Stack Register!
typeRegWithIdx - requesting index = 0x63
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x57
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x64
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x64
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x58
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x5b
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x5c
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x5b
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x5c
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
typeRegWithIdx - requesting index = 0x72
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x73
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x72
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x74
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x72
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x74
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x72
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x74
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x76
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x77
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x72
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x76
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x72
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x74
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x72
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x74
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x72
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x74
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x78
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x79
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x72
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x78
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x72
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x74
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x72
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x74
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x72
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x74
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x7a
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x7b
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x72
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x7a
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x72
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x74
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x72
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x74
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x72
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x74
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x7c
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x7d
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x72
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x7c
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x72
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x74
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x72
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x74
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x72
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x74
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x7e
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x7f
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x72
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x7e
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x72
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x74
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x72
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x74
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x72
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x74
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x80
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x81
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x72
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x80
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x72
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x74
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x72
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x74
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x72
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x74
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x82
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x83
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x72
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x82
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x72
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x74
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x72
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x74
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x72
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x74
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
Found a Stack Register!
typeRegWithIdx - requesting index = 0x65
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
Found a Stack Register!
typeRegWithIdx - requesting index = 0x65
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x67
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x1000
typeRegWithIdx - requesting index = 0x1000
typeRegWithIdx - requesting index = 0x1000
Found a Stack Register!
typeRegWithIdx - requesting index = 0x6a
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6b
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6a
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6b
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6a
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6b
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6c
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6a
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6b
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6b
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
Found a Stack Register!
typeRegWithIdx - requesting index = 0x65
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
Found a Stack Register!
typeRegWithIdx - requesting index = 0x65
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x67
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x1000
typeRegWithIdx - requesting index = 0x1000
typeRegWithIdx - requesting index = 0x1000
Found a Stack Register!
typeRegWithIdx - requesting index = 0x6a
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6b
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6a
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6b
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6a
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6b
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6c
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6a
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6c
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
Found a Stack Register!
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
Found a Stack Register!
typeRegWithIdx - requesting index = 0x6a
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6c
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
Found a Stack Register!
typeRegWithIdx - requesting index = 0x6c
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
typeRegWithIdx - requesting index = 0x64
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
Found a Stack Register!
typeRegWithIdx - requesting index = 0x64
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x5b
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x5c
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
typeRegWithIdx - requesting index = 0x72
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x73
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x72
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x74
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x72
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x74
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x72
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x74
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x76
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x77
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x72
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x76
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x72
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x74
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x72
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x74
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x72
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x74
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x78
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x79
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x72
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x78
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x72
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x74
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x72
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x74
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x72
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x74
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x7a
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x7b
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x72
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x7a
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x72
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x74
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x72
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x74
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x72
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x74
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x7c
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x7d
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x72
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x7c
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x72
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x74
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x72
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x74
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x72
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x74
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x7e
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x7f
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x72
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x7e
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x72
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x74
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x72
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x74
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x72
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x74
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x80
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x81
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x72
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x80
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x72
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x74
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x72
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x74
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x72
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x74
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x82
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x83
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x72
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x82
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x72
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x74
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x72
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x74
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x72
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x74
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
Found a Stack Register!
typeRegWithIdx - requesting index = 0x65
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
Found a Stack Register!
typeRegWithIdx - requesting index = 0x65
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x67
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x1000
typeRegWithIdx - requesting index = 0x1000
typeRegWithIdx - requesting index = 0x1000
Found a Stack Register!
typeRegWithIdx - requesting index = 0x6a
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6b
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6a
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6b
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6a
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6b
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6c
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6a
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6c
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
Found a Stack Register!
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
Found a Stack Register!
typeRegWithIdx - requesting index = 0x6a
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6c
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
Found a Stack Register!
typeRegWithIdx - requesting index = 0x6c
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
typeRegWithIdx - requesting index = 0x64
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
Found a Stack Register!
typeRegWithIdx - requesting index = 0x64
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x5b
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x5c
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
typeRegWithIdx - requesting index = 0x72
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x73
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x72
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x74
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x72
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x74
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x72
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x74
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x76
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x77
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x72
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x76
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x72
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x74
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x72
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x74
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x72
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x74
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x78
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x79
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x72
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x78
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x72
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x74
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x72
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x74
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x72
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x74
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x7a
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x7b
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x72
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x7a
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x72
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x74
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x72
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x74
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x72
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x74
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x7c
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x74
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x7c
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x74
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x7c
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x74
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x72
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x7c
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x7e
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x74
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x7e
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x74
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x7e
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x74
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x72
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x7e
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x80
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x74
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x80
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x74
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x80
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x74
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x72
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x80
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x82
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x74
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x82
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x74
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x82
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x74
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x72
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x82
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
Found a Stack Register!
typeRegWithIdx - requesting index = 0x65
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
Found a Stack Register!
typeRegWithIdx - requesting index = 0x65
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x67
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x1000
typeRegWithIdx - requesting index = 0x1000
typeRegWithIdx - requesting index = 0x1000
Found a Stack Register!
typeRegWithIdx - requesting index = 0x6c
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6a
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6c
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
Found a Stack Register!
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
Found a Stack Register!
typeRegWithIdx - requesting index = 0x6a
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6c
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
Found a Stack Register!
typeRegWithIdx - requesting index = 0x6c
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
typeRegWithIdx - requesting index = 0x64
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
Found a Stack Register!
typeRegWithIdx - requesting index = 0x64
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x5b
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x5c
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
typeRegWithIdx - requesting index = 0x6
typeRegWithIdx - requesting index = 0x6
typeRegWithIdx - requesting index = 0x72
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x73
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x73
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x72
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x74
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x72
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x74
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x74
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x72
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x75
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x76
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x77
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x77
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x76
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x72
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x74
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x72
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x74
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x74
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x72
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x75
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x78
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x79
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x79
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x78
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x72
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x74
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x72
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x74
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x74
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x72
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x75
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x7a
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x7b
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x7b
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x7a
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x72
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x74
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x72
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x74
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x74
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x72
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x75
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x7c
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x7d
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x7d
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x74
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x7c
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x72
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x74
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x74
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x72
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x75
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x7e
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x7f
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x7f
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x74
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x7e
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x72
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x74
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x74
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x72
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x75
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x80
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x81
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x81
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x74
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x80
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x72
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x74
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x74
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x72
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x75
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x82
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x83
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x83
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x74
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x82
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x72
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x74
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x74
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x72
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x75
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x75
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x70
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x71
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x70
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x71
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6f
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6f
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6e
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6e
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x65
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
Found a Stack Register!
typeRegWithIdx - requesting index = 0x1000
typeRegWithIdx - requesting index = 0x1000
typeRegWithIdx - requesting index = 0x1000
Found a Stack Register!
typeRegWithIdx - requesting index = 0x67
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x67
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x67
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x67
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x69
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x65
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x69
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6c
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6a
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6c
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6a
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6a
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6c
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x6a
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
Found a Stack Register!
typeRegWithIdx - requesting index = 0x6c
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
typeRegWithIdx - requesting index = 0x5b
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x5c
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
typeRegWithIdx - requesting index = 0x55
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x5e
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x5e
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
typeRegWithIdx - requesting index = 0x60
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
Found a Stack Register!
typeRegWithIdx - requesting index = 0x57
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x60
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x60
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
typeRegWithIdx - requesting index = 0x62
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
Found a Stack Register!
typeRegWithIdx - requesting index = 0x55
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x62
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x62
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
typeRegWithIdx - requesting index = 0x64
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
typeRegWithIdx - requesting index = 0xfff
Found a Stack Register!
typeRegWithIdx - requesting index = 0x57
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x64
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x64
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x58
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x5b
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x5c
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x5b
Found a Dynamic Register!
typeRegWithIdx - requesting index = 0x5c
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
