CON

  _clkmode = xtal1 + pll16x
  _xinfreq = 5_000_000  

  MEM_SIZE = 128

OBJ

  vm : "VectorMachine"
  text : "tv_text"

VAR
  long  mem[MEM_SIZE]

PUB start | i, sync, period

  text.start(12)
  text.str(string("Testing VectorMachine object...", 13))

  sync := cnt + clkfreq / 10
  period := clkfreq / 50
  vm.start(sync, 200000, -1, @mem, MEM_SIZE)
  
  show

  mem[1] := %11_0_0100000_1111_000000000_000001000
  mem[2] := %11_0_0100000_1111_000000000_111111000
  mem[3] := %11_0_0100000_1111_000000000_111111000
  mem[4] := %11_0_0100000_1111_000000000_000001000
  mem[5] := vm#INST_JUMP + 1

  vm.cmd(vm#INST_JUMP + 1)

  repeat
    show
    waitcnt(sync += period)

PRI show
  text.hex(LONG[vm.getOutputAddress(0)], 8)
  text.out(13)
    