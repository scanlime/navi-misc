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
    if pedal
      sew.motorOpenLoopSpeed(pedal)
    else        
      sew.runToPosition(sew#CYCLE_BOTTOM)
