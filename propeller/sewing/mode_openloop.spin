CON
  _clkmode = xtal1 + pll16x
  _xinfreq = 5_000_000

OBJ
  term : "Parallax Serial Terminal"
  sew : "SewingMachine"

PUB main | pedal

  sew.start
  term.start(115200)

  repeat
    pedal := sew.readPedal
    
    term.Str(String(term#NL, "Pedal: "))
    term.Dec(pedal)

    term.Str(String(" RPM: "))
    term.Dec(sew.getSpeed >> 16)

    sew.motorOpenLoopSpeed(pedal)