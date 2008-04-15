
OBJ
  prox : "OpticalProximity"
  vp : "ViewPortConduit_20"
  vcm : "VoiceCoil"
  
CON
  _clkmode = xtal1 + pll16x
  _xinfreq = 5_000_000

VAR
  long  reading
  long  stamp
  long  vcmStack[32]
  
  
PUB main
  prox.start(4,5,0)
  cognew(vcmThread, @vcmStack)

  vp.general(string("mode=dso"))

  vp.share(@reading, @stamp)
  vp.view(string("reading,stamp"))

  vp.start
  
  repeat
    reading := prox.read
    stamp++

PRI vcmThread | i
  vcm.start(6)
  repeat
    vcm.pulse(100)
    waitcnt(cnt + clkfreq/10)
    vcm.pulse(-100)
    waitcnt(cnt + clkfreq/10)
   